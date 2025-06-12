#ifndef MESH_H
#define MESH_H

#include <QList>
#include <QMap>

struct Point2D
{
    qreal x, y;
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
    QList<Point2D> pointSeq;
    QList<Point2D> forceSeq;
};

#endif // MESH_H
