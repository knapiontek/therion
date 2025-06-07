#include "mesh.h"
#include <Eigen/Sparse>
#include <cmath>

typedef Eigen::Triplet<double> T;

void populate(Mesh& mesh)
{
    QList<T> K;
    QList<double> F;

    // init F, K
    for (int i = 0; i < mesh.pointSeq.size(); i++) {
        const Point2D& fix = mesh.pointSeq[i];
        Point2D& force = mesh.pointSeq[i];
        int px = 2 * i + 0;
        int py = 2 * i + 1;

        // reactions in K (in place of fixed displacement)
        if (fix.x)
            K.append(T(px, px, -1));
        else
            F[px] = force.x;

        if (fix.y)
            K.append(T(py, py, -1));
        else
            F[py] = force.y;
    }

    // compose K - stiffness matrix
    for (int i = 0; i < mesh.elementSeq.size(); i++) {
        const double EA = 1000; // Young * Area
        const Element& element = mesh.elementSeq[i];
        const Point2D& point1 = mesh.pointSeq[element.p1];
        const Point2D& point2 = mesh.pointSeq[element.p2];
        const Point2D& fix1 = mesh.pointSeq[element.p1];
        const Point2D& fix2 = mesh.pointSeq[element.p2];

        int p1x = 2 * element.p1 + 0;
        int p1y = 2 * element.p1 + 1;
        int p2x = 2 * element.p2 + 0;
        int p2y = 2 * element.p2 + 1;

        double dx = point2.x - point1.x;
        double dy = point2.y - point1.y;
        double l = sqrt(dx * dx + dy * dy);
        double cx = dx / l;
        double cy = dy / l;
        double cxxEAl = cx * cx * EA / l;
        double cyyEAl = cy * cy * EA / l;
        double cxyEAl = cx * cy * EA / l;

        if (!fix1.x) {
            K.append(T(p1x, p1x, cxxEAl));
            K.append(T(p1y, p1x, cxyEAl));
            K.append(T(p2x, p1x, -cxxEAl));
            K.append(T(p2y, p1x, -cxyEAl));
        }
        if (!fix1.y) {
            K.append(T(p1x, p1y, cxyEAl));
            K.append(T(p1y, p1y, cyyEAl));
            K.append(T(p2x, p1y, -cxyEAl));
            K.append(T(p2y, p1y, -cyyEAl));
        }
        if (!fix2.x) {
            K.append(T(p1x, p2x, -cxxEAl));
            K.append(T(p1y, p2x, -cxyEAl));
            K.append(T(p2x, p2x, cxxEAl));
            K.append(T(p2y, p2x, cxyEAl));
        }
        if (!fix2.y) {
            K.append(T(p1x, p2y, -cxyEAl));
            K.append(T(p1y, p2y, -cyyEAl));
            K.append(T(p2x, p2y, cxyEAl));
            K.append(T(p2y, p2y, cyyEAl));
        }
    }
}
