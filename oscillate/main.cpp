#include <QApplication>
#include <QPainter>
#include <QTimer>
#include <QWidget>
#include <cmath>

const double k = 0.1;
const double m = 1.0;
const double dt = 0.1;
const double segment = 8.0;
const double omega = std::sqrt(k / m);
const int pointSize = 45;

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
        setFixedSize(400, 400);
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &LeapfrogOscillator::simulate);
        timer->start(16);

        for (int i = 0; i < pointSize; ++i) {
            pointSeq[i] = {20.0 + segment * i, 200.0};
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

        for ([[maybe_unused]] auto _ : {0, 1, 2, 3}) {
            _time += dt;
            pointSeq[0] = {20.0, 200 + 20 * std::sin(omega * _time / 20)};

            for (int i = 1; i < pointSize - 1; ++i) {
                QPointF &p1 = pointSeq[i - 1];
                QPointF &p2 = pointSeq[i];
                QPointF &p3 = pointSeq[i + 1];

                QPointF &v2 = velocitySeq[i];

                QPointF dp1 = p2 - p1;
                QPointF dp3 = p2 - p3;

                qreal l1 = length(dp1);
                qreal l3 = length(dp3);

                qreal d1 = l1 - segment;
                qreal d3 = l3 - segment;

                QPointF u1 = dp1 / l1;
                QPointF u3 = dp3 / l3;

                QPointF f1 = u1 * -k * d1;
                QPointF f3 = u3 * -k * d3;

                QPointF a2 = (f1 + f3) / m;

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
