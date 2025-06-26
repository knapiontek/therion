#include <QApplication>
#include <QPainter>
#include <QTimer>
#include <QWidget>
#include <cmath>

struct Point2D
{
    double x, y;

    Point2D operator+(const Point2D &b) const { return {x + b.x, y + b.y}; }
    Point2D operator-(const Point2D &b) const { return {x - b.x, y - b.y}; }
    Point2D operator*(double s) const { return {x * s, y * s}; }
    Point2D operator/(double s) const { return {x / s, y / s}; }

    Point2D &operator+=(const Point2D &b)
    {
        x += b.x;
        y += b.y;
        return *this;
    }
    Point2D &operator*=(double s)
    {
        x *= s;
        y *= s;
        return *this;
    }

    double length() const { return std::sqrt(x * x + y * y); }
    Point2D normalized() const
    {
        double len = length();
        return (len > 1e-8) ? *this / len : Point2D{0, 0};
    }
};

// Constants
const double k = 0.01;
const double m = 1.0;
const double dt = 0.5;
const double restLength = 100.0;

const Point2D p1 = {100, 200}; // Fixed
const Point2D p3 = {300, 200}; // Fixed

// State of moving point
Point2D p2 = {200, 100};       // Initial position
Point2D v2_half = {-5.5, 3.5}; // Velocity at t + ½·dt

class LeapfrogOscillator : public QWidget
{
public:
    LeapfrogOscillator(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        setFixedSize(400, 400);
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &LeapfrogOscillator::simulate);
        timer->start(16); // ~60 FPS
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        // Draw springs
        painter.setPen(Qt::black);
        painter.drawLine(QPointF(p1.x, p1.y), QPointF(p2.x, p2.y));
        painter.drawLine(QPointF(p3.x, p3.y), QPointF(p2.x, p2.y));

        // Fixed points
        painter.setBrush(Qt::red);
        painter.drawEllipse(QPointF(p1.x, p1.y), 5, 5);
        painter.drawEllipse(QPointF(p3.x, p3.y), 5, 5);

        // Moving mass
        painter.setBrush(Qt::blue);
        painter.drawEllipse(QPointF(p2.x, p2.y), 6, 6);
    }

private:
    Point2D springForce(const Point2D &anchor, const Point2D &mass)
    {
        Point2D delta = mass - anchor;
        double len = delta.length();
        double stretch = len - restLength;
        return delta.normalized() * (-k * stretch);
    }

    void simulate()
    {
        // Step 1: compute acceleration from current position
        Point2D f1 = springForce(p1, p2);
        Point2D f3 = springForce(p3, p2);
        Point2D totalForce = f1 + f3;
        Point2D a = totalForce / m;

        // Step 2: update velocity at half-step
        v2_half += a * dt;

        // Step 3: update position at full-step using updated half-step velocity
        p2 += v2_half * dt;

        update(); // Trigger paintEvent()
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    LeapfrogOscillator w;
    w.show();
    return app.exec();
}
