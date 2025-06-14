#ifndef SOLVE_H
#define SOLVE_H

#include "mesh.h"
#include <Eigen/IterativeLinearSolvers>
#include <Eigen/Sparse>
#include <cmath>

const qreal EA = 1000; // Young * Area

typedef Eigen::Triplet<qreal> Triplet;
typedef Eigen::SparseMatrix<qreal> SparseMatrix;
typedef Eigen::VectorXd Vector;
typedef Eigen::SparseLU<SparseMatrix, Eigen::COLAMDOrdering<qint32>> LUSolver;

// K * x = F for x

OutputMesh solveMesh(const InputMesh &inputMesh);

#endif // SOLVE_H
