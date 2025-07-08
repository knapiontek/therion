#include <QApplication>
#include <QPainter>
#include <QTimer>
#include <QWidget>
#include <cmath>

const double k = 0.1;
const double m = 10.0;
const double dt = 0.1;
const double dl = 8.0;
const double omega2 = k / m;
const double omega = std::sqrt(omega2);
const int pointSize = 150;

QPointF pointSeq[pointSize];
QPointF velocitySeq[pointSize];

double _time = 0.0;
double energy = 0.0;

qreal length(const QPointF &p)
{
    return std::sqrt(QPointF::dotProduct(p, p));
}

class LeapfrogOscillator : public QWidget
{
public:
    LeapfrogOscillator(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        setFixedSize(1200, 400);
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &LeapfrogOscillator::simulate);
        timer->start(16);

        for (int i = 0; i < pointSize; ++i) {
            pointSeq[i] = {20.0 + dl * i, 200.0};
        }
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(Qt::blue);
        painter.setBrush(Qt::blue);

        for (auto &point : pointSeq) {
            painter.drawEllipse(point, 1, 1);
        }

        for (int i = 1; i < pointSize; ++i) {
            QPointF &p1 = pointSeq[i - 1];
            QPointF &p2 = pointSeq[i];
            painter.drawLine(p1, p2);
        }

        painter.drawText(10, 20, QString("Total Energy: %1").arg(energy));
    }

private:
    void simulate()
    {
        energy = 0.0;

        for (int r = 0; r < 60; r++) {
            _time += dt;
            pointSeq[0] = {20.0, 200 + 30 * std::sin(0.05 * omega * _time)};

            for (int i = 1; i < pointSize - 1; ++i) {
                QPointF &p1 = pointSeq[i - 1];
                QPointF &p2 = pointSeq[i];
                QPointF &p3 = pointSeq[i + 1];

                QPointF &v2 = velocitySeq[i];

                QPointF dp1 = p2 - p1;
                QPointF dp3 = p2 - p3;

                qreal l1 = length(dp1);
                qreal l3 = length(dp3);

                qreal d1 = l1 - dl;
                qreal d3 = l3 - dl;

                QPointF u1 = dp1 * d1 / l1;
                QPointF u3 = dp3 * d3 / l3;

                QPointF a2 = -omega2 * (u1 + u3) - 0.001 * v2;

                v2 += a2 * dt;
                p2 += v2 * dt;

                energy += m * QPointF::dotProduct(v2, v2) + k * d1 * d1 + k * d3 * d3;
            }
        }

        update();
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    LeapfrogOscillator w;
    w.show();
    return app.exec();
}
