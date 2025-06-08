#include "mesh.h"
#include <Eigen/IterativeLinearSolvers>
#include <Eigen/Sparse>
#include <cmath>

typedef Eigen::Triplet<double> Triplet;
typedef Eigen::SparseMatrix<double> SparseMatrix;

void solve(MeshInput &input, MeshOutput &output)
{
    int geomSize = input.pointSeq.size();
    int solveSize = 2 * geomSize;

    QList<Triplet> K;
    Eigen::VectorXd F(solveSize);

    // init F, K
    for (int i = 0; i < geomSize; i++) {
        const Fix2D &fix = input.fixMap.value(i, Fix2D{false, false});
        const Point2D &force = input.forceMap.value(i, Point2D{0, 0});

        int px = 2 * i + 0;
        int py = 2 * i + 1;

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

        int p1x = 2 * element.p1 + 0;
        int p1y = 2 * element.p1 + 1;
        int p2x = 2 * element.p2 + 0;
        int p2y = 2 * element.p2 + 1;

        double EA = 1000; // Young * Area
        double dx = point2.x - point1.x;
        double dy = point2.y - point1.y;
        double l = sqrt(dx * dx + dy * dy);
        double cx = dx / l;
        double cy = dy / l;
        double cxxEAl = cx * cx * EA / l;
        double cyyEAl = cy * cy * EA / l;
        double cxyEAl = cx * cy * EA / l;

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
    SparseMatrix KK(solveSize, solveSize);
    KK.setFromTriplets(K.begin(), K.end());
    Eigen::ConjugateGradient<SparseMatrix> solver;
    solver.compute(KK);

    if (solver.info() != Eigen::Success) {
        throw std::runtime_error("precondition failed");
    }

    Eigen::VectorXd dP = solver.solve(F);

    if (solver.info() != Eigen::Success) {
        throw std::runtime_error("solving failed");
    }

    // copy to global output
    output.pointSeq.resize(geomSize);
    output.forceSeq.resize(geomSize);
    for (int i = 0; i < geomSize; i++) {
        Point2D &point = input.pointSeq[i];
        const Fix2D &fix = input.fixMap.value(i, Fix2D{false, false});
        Point2D &outputPoint = output.pointSeq[i];
        Point2D &ouputForce = output.forceSeq[i];

        int px = 2 * i + 0;
        int py = 2 * i + 1;
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
