#include <QPainter>
#include <QPen>

struct Point2D
{
    double x, y;
};

struct Element
{
    int p1, p2;
};

struct Mesh
{
    QList<Point2D> pointSeq;
    QList<Element> elementSeq;
};

using ImageCapture = std::function<void(const QImage &)>;

void buildMesh(Mesh &mesh, int sizeH, int sizeV)
{
    double PI = 4 * atan(1.0);
    double unit = 1;
    double unitH = unit / 2;
    double unitV = unit * cos(PI / 6);

    for (int h = 0; h < 40; h++) {
        for (int v = 0; v < 30; v++) {
            double shiftH = v % 2 == 0 ? unitH : 0;
            auto point = Point2D{h * unit + shiftH, v * unitV};
            mesh.pointSeq.append(point);
        }
    }
}

void charge(int width, int height, ImageCapture capture)
{
    QImage image(width, height, QImage::Format_RGB888);
    image.fill(QColor::fromRgb(255, 255, 255));

    QPainter painter;
    painter.begin(&image);

    QPen pen(Qt::black);
    pen.setWidth(3);
    painter.setPen(pen);

    int sizeH = 40;
    int sizeV = 40;
    double unit = width / sizeH;

    Mesh mesh;
    buildMesh(mesh, sizeH, sizeV);

    for (auto &p : mesh.pointSeq) {
        painter.drawPoint(unit * p.x + 18, unit * p.y + 60);
    }

    painter.end();

    image.save("sample.png");
    capture(image);
}
