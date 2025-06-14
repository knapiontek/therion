#ifndef MESH_H
#define MESH_H

#include <QList>
#include <QMap>

struct Point2D
{
    qreal x, y;

    Point2D operator+(const Point2D &rhs) const { return Point2D{x + rhs.x, y + rhs.y}; }
    Point2D operator-(const Point2D &rhs) const { return Point2D{x - rhs.x, y - rhs.y}; }
    Point2D operator*(qreal value) const { return Point2D{x * value, y * value}; }
    Point2D operator/(qreal value) const { return Point2D{x / value, y / value}; }
    qreal operator*(const Point2D &rhs) const { return x * rhs.x + y * rhs.y; }
};

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

#endif // MESH_H
