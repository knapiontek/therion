#ifndef MESH_H
#define MESH_H

#include <QDebug>
#include <QList>
#include <QMap>

struct Point2D
{
    qreal x, y;

    Point2D operator+(const Point2D &rhs) const { return {x + rhs.x, y + rhs.y}; }
    Point2D operator-(const Point2D &rhs) const { return {x - rhs.x, y - rhs.y}; }
    Point2D operator*(qreal value) const { return {x * value, y * value}; }
    Point2D operator/(qreal value) const { return {x / value, y / value}; }
    qreal operator*(const Point2D &rhs) const { return x * rhs.x + y * rhs.y; }
};

inline QDebug operator<<(QDebug dbg, const Point2D &p)
{
    QDebugStateSaver saver(dbg);
    dbg.nospace() << "(" << p.x << ", " << p.y << ")";
    return dbg;
}

struct Element
{
    qint32 p1, p2;
};

struct Fix2D
{
    bool x, y;
};

struct InputMesh
{
    QList<Point2D> pointSeq;
    QList<Element> elementSeq;
    QMap<qint32, Fix2D> fixMap;
    QMap<qint32, Point2D> forceMap;
};

struct OutputMesh
{
    QList<Point2D> deltaSeq;
    QList<Point2D> forceSeq;
};

InputMesh buildMesh(qint32 sizeH, qint32 sizeV);
void applyStarForce(InputMesh &inputMesh, qint32 point, qint32 length, qint32 sizeH, qint32 sizeV);

#endif // MESH_H
