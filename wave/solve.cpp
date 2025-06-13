#include "solve.h"

OutputMesh solveMesh(const InputMesh &inputMesh)
{
    OutputMesh outputMesh;

    qint32 pointSize = inputMesh.pointSeq.size();
    qint32 equationSize = 2 * pointSize;

    QList<Triplet> K;
    Vector F(equationSize);

    // init K, F
    for (qint32 i = 0; i < pointSize; i++) {
        const Fix2D &fix = inputMesh.fixMap.value(i, Fix2D{false, false});
        const Point2D &force = inputMesh.forceMap.value(i, Point2D{0, 0});

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
    for (auto &element : inputMesh.elementSeq) {
        const Point2D &point1 = inputMesh.pointSeq[element.p1];
        const Point2D &point2 = inputMesh.pointSeq[element.p2];
        const Fix2D &fix1 = inputMesh.fixMap.value(element.p1, Fix2D{false, false});
        const Fix2D &fix2 = inputMesh.fixMap.value(element.p2, Fix2D{false, false});

        qint32 p1x = 2 * element.p1 + 0;
        qint32 p1y = 2 * element.p1 + 1;
        qint32 p2x = 2 * element.p2 + 0;
        qint32 p2y = 2 * element.p2 + 1;

        qreal EA = 10000; // Young * Area
        qreal dx = point2.x - point1.x;
        qreal dy = point2.y - point1.y;
        qreal l = std::sqrt(dx * dx + dy * dy);
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
    SparseMatrix KK(equationSize, equationSize);
    KK.setFromTriplets(K.begin(), K.end());
    LUSolver solver;
    solver.compute(KK);

    if (solver.info() != Eigen::Success) {
        throw std::runtime_error("precondition failed");
    }

    Vector dP = solver.solve(F);

    if (solver.info() != Eigen::Success) {
        throw std::runtime_error("solving 'Kx = F' failed");
    }

    // copy to output
    outputMesh.pointSeq.resize(pointSize);
    outputMesh.forceSeq.resize(pointSize);
    outputMesh.deltaSeq.resize(pointSize);
    outputMesh.deltaMax = 0;

    for (qint32 i = 0; i < pointSize; i++) {
        const Point2D &point = inputMesh.pointSeq[i];
        const Fix2D &fix = inputMesh.fixMap.value(i, Fix2D{false, false});
        Point2D &outputPoint = outputMesh.pointSeq[i];
        Point2D &ouputForce = outputMesh.forceSeq[i];
        qreal &delta = outputMesh.deltaSeq[i];

        qint32 px = 2 * i + 0;
        qint32 py = 2 * i + 1;
        outputPoint = point;
        ouputForce = Point2D{0, 0};

        qreal delta2X = 0;
        qreal delta2Y = 0;

        if (fix.x) {
            ouputForce.x = dP[px];
        } else {
            outputPoint.x += dP[px];
            delta2X = dP[px] * dP[px];
        }

        if (fix.y) {
            ouputForce.y = dP[py];
        } else {
            outputPoint.y += dP[py];
            delta2Y = dP[py] * dP[py];
        }

        delta = std::sqrt(delta2X + delta2Y);
        outputMesh.deltaMax = std::fmax(outputMesh.deltaMax, delta);
    }

    return outputMesh;
}
