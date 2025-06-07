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
    QList<int> fixSeq;
    QList<Element> elementSeq;
};

using ImageCapture = std::function<void(const QImage &)>;

void buildMesh(Mesh &mesh, int sizeH, int sizeV)
{
    double PI = 4 * atan(1.0);
    double unit = 1;
    double unitH = unit / 2;
    double unitV = unit * cos(PI / 6);

    for (int v = 0; v < sizeV; v++) {
        for (int h = 0; h < sizeH; h++) {
            double shiftH = v % 2 == 0 ? unitH : 0;
            mesh.pointSeq.append(Point2D{h * unit + shiftH, v * unitV});
        }
    }

    for (int i = 0; i < sizeV - 1; i++) {
        mesh.fixSeq.append(i);
        mesh.fixSeq.append((i + 1) * sizeH);
    }

    for (int v = 0; v < sizeV; v++) {
        for (int h = 0; h < sizeH - 1; h++) {
            int p1 = v * sizeH + h;
            int p2 = v * sizeH + h + 1;
            mesh.elementSeq.append(Element{p1, p2});
        }
    }

    for (int h = 0; h < sizeH; h++) {
        for (int v = 0; v < sizeV - 1; v++) {
            int p1 = v * sizeH + h;
            int p2 = (v + 1) * sizeH + h;
            mesh.elementSeq.append(Element{p1, p2});
        }
    }

    for (int h = 0; h < sizeH - 1; h++) {
        for (int v = 0; v < sizeV - 1; v++) {
            if (v % 2 == 0) {
                int p1 = v * sizeH + h;
                int p2 = (v + 1) * sizeH + h + 1;
                mesh.elementSeq.append(Element{p1, p2});
            } else {
                int p1 = (v + 1) * sizeH + h;
                int p2 = v * sizeH + h + 1;
                mesh.elementSeq.append(Element{p1, p2});
            }
        }
    }
}

void charge(int width, int height, ImageCapture capture)
{
    QImage image(width, height, QImage::Format_RGB888);
    image.fill(QColor::fromRgb(255, 255, 255));

    QPen redPen(Qt::red);
    redPen.setWidth(3);

    QPen greenPen(Qt::green);
    greenPen.setWidth(2);

    QPen blackPen(Qt::black);
    blackPen.setWidthF(0.5);

    QPainter painter;
    painter.begin(&image);
    painter.setRenderHint(QPainter::Antialiasing, true);

    int sizeH = 40;
    int sizeV = 35;
    double unit = width / sizeH;

    Mesh mesh;
    buildMesh(mesh, sizeH, sizeV);

    painter.setPen(blackPen);
    for (auto &e : mesh.elementSeq) {
        auto &p1 = mesh.pointSeq[e.p1];
        auto &p2 = mesh.pointSeq[e.p2];
        painter.drawLine(unit * p1.x + 18, unit * p1.y + 18, unit * p2.x + 18, unit * p2.y + 18);
    }

    painter.setPen(greenPen);
    for (auto &p : mesh.pointSeq) {
        painter.drawPoint(unit * p.x + 18, unit * p.y + 18);
    }

    painter.setPen(redPen);
    for (auto &i : mesh.fixSeq) {
        auto &p = mesh.pointSeq[i];
        painter.drawPoint(unit * p.x + 18, unit * p.y + 18);
    }

    painter.end();

    image.save("sample.png");
    capture(image);
}
