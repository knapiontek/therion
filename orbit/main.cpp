#include <QApplication>
#include <QPainter>
#include <QTimer>
#include <QVector2D>
#include <QWidget>
#include <QtMath>

class PlanetWidget : public QWidget
{
    Q_OBJECT

public:
    PlanetWidget(QWidget *parent = nullptr)
        : QWidget(parent)
        , position(90.0f, 0.0f)
        , velocity(0.0f, 0.1f)
        , timeStep(13.5f)
        , scale(3.0f)
    {
        float r = position.length();
        acceleration = -position.normalized() * (1.0f / (r * r));

        connect(&timer, &QTimer::timeout, this, &PlanetWidget::updateSimulation);
        timer.start(16);
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        painter.fillRect(rect(), Qt::black);

        // Draw central star
        painter.setBrush(Qt::yellow);
        painter.drawEllipse(QPointF(width() / 2, height() / 2), 5, 5);

        // Convert simulation coordinates to screen
        QPointF screenPos = QPointF(width() / 2 + position.x() * scale,
                                    height() / 2 - position.y() * scale);

        // Draw planet
        painter.setBrush(Qt::cyan);
        painter.drawEllipse(screenPos, 5, 5);
    }

private slots:
    void updateSimulation()
    {
        velocity += 0.5f * acceleration * timeStep;
        position += velocity * timeStep;
        float r = position.length();
        acceleration = -position.normalized() * (1.0f / (r * r));
        velocity += 0.5f * acceleration * timeStep;
        update();
    }

private:
    QVector2D position;
    QVector2D velocity;
    QVector2D acceleration;
    float timeStep;
    float scale;
    QTimer timer;
};

#include "main.moc"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    PlanetWidget w;
    w.resize(800, 600);
    w.setWindowTitle("Leapfrog Planetary Motion - Stable Orbit");
    w.show();

    return app.exec();
}
