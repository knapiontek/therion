#ifndef ENGINE_H
#define ENGINE_H

#include "input.h"

extern double K[DOF_NO][DOF_NO]; // stiffness matrix
extern double L[DOF_NO][DOF_NO]; // lower triangular
extern double U[DOF_NO][DOF_NO]; // upper triangular
extern double X[DOF_NO]; // shift

void engine_init();

#endif // ENGINE_H
