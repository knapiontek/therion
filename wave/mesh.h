#ifndef MESH_H
#define MESH_H

#include <QList>
#include <QMap>

struct Point2D
{
    double x, y;
};

struct Element
{
    int p1, p2;
};

struct Fix2D
{
    bool x, y;
};

struct Mesh
{
    QList<Point2D> pointSeq;
    QList<Element> elementSeq;
    QMap<int, Fix2D> fixMap;
    QMap<int, Point2D> forceMap;
};

#endif // MESH_H
