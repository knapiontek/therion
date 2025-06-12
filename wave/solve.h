#ifndef SOLVE_H
#define SOLVE_H

#include "mesh.h"
#include <Eigen/IterativeLinearSolvers>
#include <Eigen/Sparse>
#include <cmath>

typedef Eigen::Triplet<qreal> Triplet;
typedef Eigen::SparseMatrix<qreal> SparseMatrix;
typedef Eigen::VectorXd Vector;
typedef Eigen::SparseLU<SparseMatrix, Eigen::COLAMDOrdering<qint32>> LUSolver;

// K * x = F for x

void solveMesh(MeshInput &input, MeshOutput &output);

#endif // SOLVE_H
