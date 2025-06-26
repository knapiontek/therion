#include <QApplication>
#include <QPainter>
#include <QTimer>
#include <QWidget>
#include <cmath>

struct Point
{
    double x, y;
};

const double k = 0.01;         // Spring constant
const double m = 1.0;          // Mass
const double dt = 0.5;         // Time step
const double restLength = 100; // Natural spring length

// Initial positions
Point p1 = {100, 200}; // Fixed
Point p3 = {300, 200}; // Fixed
Point p2 = {200, 150}; // Movable (initially displaced for oscillation)

// Velocity and acceleration of p2
Point v2 = {0, 0};
Point a2 = {0, 0};

class OscillatorWidget : public QWidget
{
public:
    OscillatorWidget(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        setFixedSize(400, 400);

        // Initial acceleration and half-step Leapfrog velocity
        computeAcceleration();
        v2.x -= 0.5 * dt * a2.x;
        v2.y -= 0.5 * dt * a2.y;

        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &OscillatorWidget::updateSimulation);
        timer->start(16);
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);

        // Draw springs
        p.setPen(Qt::black);
        p.drawLine(QPointF(p1.x, p1.y), QPointF(p2.x, p2.y));
        p.drawLine(QPointF(p2.x, p2.y), QPointF(p3.x, p3.y));

        // Draw masses
        p.setBrush(Qt::red);
        p.drawEllipse(QPointF(p1.x, p1.y), 5, 5);
        p.setBrush(Qt::blue);
        p.drawEllipse(QPointF(p2.x, p2.y), 5, 5);
        p.setBrush(Qt::red);
        p.drawEllipse(QPointF(p3.x, p3.y), 5, 5);
    }

private:
    void computeAcceleration()
    {
        // Force from p1 to p2
        double dx1 = p2.x - p1.x;
        double dy1 = p2.y - p1.y;
        double len1 = std::sqrt(dx1 * dx1 + dy1 * dy1);
        double f1x = -k * (len1 - restLength) * (dx1 / len1);
        double f1y = -k * (len1 - restLength) * (dy1 / len1);

        // Force from p3 to p2
        double dx2 = p2.x - p3.x;
        double dy2 = p2.y - p3.y;
        double len2 = std::sqrt(dx2 * dx2 + dy2 * dy2);
        double f2x = -k * (len2 - restLength) * (dx2 / len2);
        double f2y = -k * (len2 - restLength) * (dy2 / len2);

        // Net acceleration
        a2.x = (f1x + f2x) / m;
        a2.y = (f1y + f2y) / m;
    }

    void updateSimulation()
    {
        // Leapfrog integration
        v2.x += dt * a2.x;
        v2.y += dt * a2.y;
        p2.x += dt * v2.x;
        p2.y += dt * v2.y;
        computeAcceleration();
        update();
    }
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    OscillatorWidget w;
    w.show();
    return a.exec();
}
