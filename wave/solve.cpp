#include "mesh.h"
#include <Eigen/IterativeLinearSolvers>
#include <Eigen/Sparse>
#include <cmath>

typedef Eigen::Triplet<qreal> Triplet;
typedef Eigen::SparseMatrix<qreal> SparseMatrix;
typedef Eigen::VectorXd Vector;
typedef Eigen::ConjugateGradient<SparseMatrix> Solver;

// solveMesh K * x = F for x
void solveMesh(MeshInput &input, MeshOutput &output)
{
    qint32 pointSize = input.pointSeq.size();
    qint32 eqSize = 2 * pointSize;

    QList<Triplet> K;
    Vector F(eqSize);

    // init K, F
    for (qint32 i = 0; i < pointSize; i++) {
        const Fix2D &fix = input.fixMap.value(i, Fix2D{false, false});
        const Point2D &force = input.forceMap.value(i, Point2D{0, 0});

        qint32 px = 2 * i + 0;
        qint32 py = 2 * i + 1;

        // reactions in K (in place of fixed displacement)
        if (fix.x)
            K += Triplet(px, px, -1);
        else
            F[px] = force.x;

        if (fix.y)
            K += Triplet(py, py, -1);
        else
            F[py] = force.y;
    }

    // compose K - stiffness matrix
    for (auto &element : input.elementSeq) {
        Point2D &point1 = input.pointSeq[element.p1];
        Point2D &point2 = input.pointSeq[element.p2];
        const Fix2D &fix1 = input.fixMap.value(element.p1, Fix2D{false, false});
        const Fix2D &fix2 = input.fixMap.value(element.p2, Fix2D{false, false});

        qint32 p1x = 2 * element.p1 + 0;
        qint32 p1y = 2 * element.p1 + 1;
        qint32 p2x = 2 * element.p2 + 0;
        qint32 p2y = 2 * element.p2 + 1;

        qreal EA = 10000; // Young * Area
        qreal dx = point2.x - point1.x;
        qreal dy = point2.y - point1.y;
        qreal l = sqrt(dx * dx + dy * dy);
        qreal cx = dx / l;
        qreal cy = dy / l;
        qreal cxxEAl = cx * cx * EA / l;
        qreal cyyEAl = cy * cy * EA / l;
        qreal cxyEAl = cx * cy * EA / l;

        if (!fix1.x) {
            K += Triplet(p1x, p1x, cxxEAl);
            K += Triplet(p1y, p1x, cxyEAl);
            K += Triplet(p2x, p1x, -cxxEAl);
            K += Triplet(p2y, p1x, -cxyEAl);
        }
        if (!fix1.y) {
            K += Triplet(p1x, p1y, cxyEAl);
            K += Triplet(p1y, p1y, cyyEAl);
            K += Triplet(p2x, p1y, -cxyEAl);
            K += Triplet(p2y, p1y, -cyyEAl);
        }
        if (!fix2.x) {
            K += Triplet(p1x, p2x, -cxxEAl);
            K += Triplet(p1y, p2x, -cxyEAl);
            K += Triplet(p2x, p2x, cxxEAl);
            K += Triplet(p2y, p2x, cxyEAl);
        }
        if (!fix2.y) {
            K += Triplet(p1x, p2y, -cxyEAl);
            K += Triplet(p1y, p2y, -cyyEAl);
            K += Triplet(p2x, p2y, cxyEAl);
            K += Triplet(p2y, p2y, cyyEAl);
        }
    }

    // calculate
    SparseMatrix KK(eqSize, eqSize);
    KK.setFromTriplets(K.begin(), K.end());
    Solver solver;
    solver.compute(KK);

    if (solver.info() != Eigen::Success) {
        throw std::runtime_error("precondition failed");
    }

    Vector dP = solver.solve(F);

    if (solver.info() != Eigen::Success) {
        throw std::runtime_error("cgm solve failed");
    }

    // copy to global output
    output.pointSeq.resize(pointSize);
    output.forceSeq.resize(pointSize);

    for (qint32 i = 0; i < pointSize; i++) {
        Point2D &point = input.pointSeq[i];
        const Fix2D &fix = input.fixMap.value(i, Fix2D{false, false});
        Point2D &outputPoint = output.pointSeq[i];
        Point2D &ouputForce = output.forceSeq[i];

        qint32 px = 2 * i + 0;
        qint32 py = 2 * i + 1;
        outputPoint = point;
        ouputForce = Point2D{0, 0};

        if (fix.x)
            ouputForce.x = dP[px];
        else
            outputPoint.x += dP[px];

        if (fix.y)
            ouputForce.y = dP[py];
        else
            outputPoint.y += dP[py];
    }
}
