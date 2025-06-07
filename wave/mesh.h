#ifndef MESH_H
#define MESH_H

#include <QList>

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

#endif // MESH_H
