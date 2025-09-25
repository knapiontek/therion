#include <QApplication>
#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <Eigen/Dense>
#include <iostream>
#include <vector>

struct Node
{
    double x, y;
    bool isBoundary;
};

struct Element
{
    int nodes[4]; // Q1: 4 corner nodes
};

// Shape functions for bilinear quad
void shapeFunctions(double xi, double eta, Eigen::Vector4d &N, Eigen::Matrix<double, 2, 4> &dN)
{
    N(0) = 0.25 * (1 - xi) * (1 - eta);
    N(1) = 0.25 * (1 + xi) * (1 - eta);
    N(2) = 0.25 * (1 + xi) * (1 + eta);
    N(3) = 0.25 * (1 - xi) * (1 + eta);

    dN(0, 0) = -0.25 * (1 - eta);
    dN(1, 0) = -0.25 * (1 - xi);
    dN(0, 1) = 0.25 * (1 - eta);
    dN(1, 1) = -0.25 * (1 + xi);
    dN(0, 2) = 0.25 * (1 + eta);
    dN(1, 2) = 0.25 * (1 + xi);
    dN(0, 3) = -0.25 * (1 + eta);
    dN(1, 3) = 0.25 * (1 - xi);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // === 1. Mesh: 2x2 elements, 9 nodes ===
    int Nx = 18, Ny = 18;
    double hx = 2.0 / Nx, hy = 2.0 / Ny;

    std::vector<Node> nodes;
    for (int j = 0; j <= Ny; ++j) {
        for (int i = 0; i <= Nx; ++i) {
            double x = -1.0 + i * hx;
            double y = -1.0 + j * hy;
            bool boundary = (i == 0 || j == 0 || i == Nx || j == Ny);
            nodes.push_back({x, y, boundary});
        }
    }
    auto idx = [&](int i, int j) { return j * (Nx + 1) + i; };

    std::vector<Element> elements;
    for (int j = 0; j < Ny; ++j) {
        for (int i = 0; i < Nx; ++i) {
            Element e;
            e.nodes[0] = idx(i, j);
            e.nodes[1] = idx(i + 1, j);
            e.nodes[2] = idx(i + 1, j + 1);
            e.nodes[3] = idx(i, j + 1);
            elements.push_back(e);
        }
    }

    int nNodes = nodes.size();

    // === 2. Global stiffness and load ===
    Eigen::MatrixXd K = Eigen::MatrixXd::Zero(nNodes, nNodes);
    Eigen::VectorXd F = Eigen::VectorXd::Zero(nNodes);

    // Gauss points for 2x2 quadrature
    double gp[2] = {-0.57735026919, 0.57735026919};
    double w[2] = {1.0, 1.0};

    for (auto &e : elements) {
        Eigen::Matrix4d Ke = Eigen::Matrix4d::Zero();

        for (int gx = 0; gx < 2; ++gx) {
            for (int gy = 0; gy < 2; ++gy) {
                double xi = gp[gx], eta = gp[gy];
                Eigen::Vector4d N;
                Eigen::Matrix<double, 2, 4> dN;
                shapeFunctions(xi, eta, N, dN);

                // Jacobian
                Eigen::Matrix2d J = Eigen::Matrix2d::Zero();
                for (int a = 0; a < 4; ++a) {
                    int na = e.nodes[a];
                    J(0, 0) += dN(0, a) * nodes[na].x;
                    J(0, 1) += dN(0, a) * nodes[na].y;
                    J(1, 0) += dN(1, a) * nodes[na].x;
                    J(1, 1) += dN(1, a) * nodes[na].y;
                }
                double detJ = J.determinant();
                Eigen::Matrix2d invJ = J.inverse();

                // Gradients in physical coords
                Eigen::Matrix<double, 2, 4> dNxy = invJ * dN;

                // Assemble Ke
                for (int a = 0; a < 4; ++a) {
                    for (int b = 0; b < 4; ++b) {
                        double val = (dNxy.col(a).dot(dNxy.col(b))) * detJ * w[gx] * w[gy];
                        Ke(a, b) += val;
                    }
                }
            }
        }

        // Assemble into global K
        for (int a = 0; a < 4; ++a) {
            int A = e.nodes[a];
            for (int b = 0; b < 4; ++b) {
                int B = e.nodes[b];
                K(A, B) += Ke(a, b);
            }
        }
    }

    // === 3. Force vector: unit load at center node ===
    int centerNode = idx(Nx / 2, Ny / 2); // (1,1)
    F(centerNode) = 1.0;
    F(centerNode + 1) = 1.0;
    F(centerNode + 2) = 1.0;
    F(centerNode + 3) = 1.0;
    F(centerNode + 4) = 1.0;
    F(centerNode + 5) = 1.0;
    F(centerNode + 6) = 1.0;

    // === 4. Boundary conditions: u=0 ===
    for (int i = 0; i < nNodes; ++i) {
        if (nodes[i].isBoundary) {
            K.row(i).setZero();
            K.col(i).setZero();
            K(i, i) = 1.0;
            F(i) = 0.0;
        }
    }

    // === 5. Solve system ===
    Eigen::VectorXd U = K.ldlt().solve(F);
    std::cout << "Solution vector U:\n" << U.transpose() << std::endl;

    // === 6. Visualization ===
    QGraphicsScene scene;
    double scale = 150.0;

    double maxU = U.maxCoeff();
    double minU = U.minCoeff();

    for (int k = 0; k < nNodes; ++k) {
        double X = nodes[k].x * scale;
        double Y = -nodes[k].y * scale;
        double val = (U(k) - minU) / (maxU - minU + 1e-12);
        QColor col = QColor::fromHsvF(0.7 * (1 - val), 1.0, 1.0);
        scene.addEllipse(X - 6, Y - 6, 12, 12, QPen(Qt::black), QBrush(col));
        auto *txt = scene.addText(QString::number(U(k), 'f', 2));
        txt->setPos(X + 8, Y - 8);
    }

    QGraphicsView view(&scene);
    view.setRenderHint(QPainter::Antialiasing);
    view.setWindowTitle("FEM Q1 Poisson [-1,1]^2");
    view.resize(500, 500);
    view.show();

    return app.exec();
}
