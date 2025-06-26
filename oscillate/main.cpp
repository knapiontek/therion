#include <QApplication>
#include <QPainter>
#include <QTimer>
#include <QWidget>
#include <cmath>

const double k = 0.01;
const double m = 1.0;
const double dt = 0.2;
const double restLength = 100.0;
const QPointF p1 = {100, 200};
const QPointF p3 = {300, 200};

QPointF p2 = {200, 100};
QPointF v2 = {-5.5, 3.5};
QPointF a2 = {0.0, 0.0};
double totalEnergy = 0.0;

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
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        // Draw springs
        painter.setPen(Qt::black);
        painter.drawLine(p1, p2);
        painter.drawLine(p3, p2);

        // Fixed points
        painter.setBrush(Qt::red);
        painter.drawEllipse(p1, 4, 4);
        painter.drawEllipse(p3, 4, 4);

        // Moving mass
        painter.setBrush(Qt::blue);
        painter.drawEllipse(p2, 6, 6);

        // Energy
        painter.drawText(10, 20, QString("Total Energy: %1").arg(totalEnergy));
    }

private:
    void simulate()
    {
        v2 += a2 * dt / 2;
        p2 += v2 * dt;

        auto diff1 = p2 - p1;
        auto diff3 = p2 - p3;

        auto l1 = length(diff1);
        auto l3 = length(diff3);

        auto d1 = l1 - restLength;
        auto d3 = l3 - restLength;

        auto f1 = diff1 / l1 * (-k * d1);
        auto f3 = diff3 / l3 * (-k * d3);

        a2 = (f1 + f3) / m;
        v2 += a2 * dt / 2;

        totalEnergy = m * QPointF::dotProduct(v2, v2) + k * d1 * d1 + k * d3 * d3;

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
