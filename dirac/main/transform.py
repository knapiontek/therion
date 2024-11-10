import math
from typing import TypeAlias, Any

import numpy as np
from scipy.sparse import csc_matrix
from scipy.sparse import csr_matrix
from scipy.sparse import lil_matrix

PointDict: TypeAlias = dict[int, list[float]]
Float64: TypeAlias = np.ndarray[Any, np.dtype[np.float64]]
Int32: TypeAlias = np.ndarray[Any, np.dtype[np.int32]]


def check_conflicts(fixes: PointDict, forces: PointDict):
    keys = fixes.keys() & forces.keys()
    for k in keys:
        if not np.dot(fixes[k], forces[k]):
            raise Exception(f'Conflict for {k} in {fixes[k]} and {forces[k]}')


def prepare_equation(nodes: Float64, elements: Int32,
                     fixes: PointDict, forces: PointDict) -> tuple[csr_matrix, csc_matrix]:
    nodes_len = nodes.shape[0]
    elements_len = elements.shape[0]
    dof = 3 * nodes_len  # degrees of freedom

    K = lil_matrix((dof, dof), dtype=np.float64)  # stiffness matrix
    F = lil_matrix((dof, 1), dtype=np.float64)  # force vector

    # init F, K
    for i in range(nodes_len):
        fix = fixes.get(i) or [0, 0, 0]
        force = forces.get(i) or [0, 0, 0]
        px = 3 * i + 0
        py = 3 * i + 1
        pz = 3 * i + 2

        F[px] = force[0]
        F[py] = force[1]
        F[pz] = force[2]

        # reactions in K (in place of fixed displacement)
        if fix[0]:
            K[px, px] = -1

        if fix[1]:
            K[py, py] = -1

        if fix[2]:
            K[pz, pz] = -1

    # compose K - stiffness matrix
    for i in range(elements_len):
        EA = 5000  # Young * Area
        element = elements[i]
        point1 = nodes[element[0]]
        point2 = nodes[element[1]]
        fix1 = fixes.get(int(element[0])) or [0, 0, 0]
        fix2 = fixes.get(int(element[1])) or [0, 0, 0]
        p1x = 3 * element[0] + 0
        p1y = 3 * element[0] + 1
        p1z = 3 * element[0] + 2
        p2x = 3 * element[1] + 0
        p2y = 3 * element[1] + 1
        p2z = 3 * element[1] + 2
        dx = point2[0] - point1[0]
        dy = point2[1] - point1[1]
        dz = point2[2] - point1[2]
        length = math.sqrt(dx * dx + dy * dy + dz * dz)
        cx = dx / length
        cy = dy / length
        cz = dz / length
        cxxEAl = cx * cx * EA / length
        cyyEAl = cy * cy * EA / length
        czzEAl = cz * cz * EA / length
        cxyEAl = cx * cy * EA / length
        cxzEAl = cx * cz * EA / length
        cyzEAl = cy * cz * EA / length

        if not fix1[0]:
            K[p1x, p1x] += cxxEAl
            K[p1y, p1x] += cxyEAl
            K[p1z, p1x] += cxzEAl
            K[p2x, p1x] -= cxxEAl
            K[p2y, p1x] -= cxyEAl
            K[p2z, p1x] -= cxzEAl
        if not fix1[1]:
            K[p1x, p1y] += cxyEAl
            K[p1y, p1y] += cyyEAl
            K[p1z, p1y] += cyzEAl
            K[p2x, p1y] -= cxyEAl
            K[p2y, p1y] -= cyyEAl
            K[p2z, p1y] -= cyzEAl
        if not fix1[2]:
            K[p1x, p1z] += cxzEAl
            K[p1y, p1z] += cyzEAl
            K[p1z, p1z] += czzEAl
            K[p2x, p1z] -= cxzEAl
            K[p2y, p1z] -= cyzEAl
            K[p2z, p1z] -= czzEAl
        if not fix2[0]:
            K[p1x, p2x] -= cxxEAl
            K[p1y, p2x] -= cxyEAl
            K[p1z, p2x] -= cxzEAl
            K[p2x, p2x] += cxxEAl
            K[p2y, p2x] += cxyEAl
            K[p2z, p2x] += cxzEAl
        if not fix2[1]:
            K[p1x, p2y] -= cxyEAl
            K[p1y, p2y] -= cyyEAl
            K[p1z, p2y] -= cyzEAl
            K[p2x, p2y] += cxyEAl
            K[p2y, p2y] += cyyEAl
            K[p2z, p2y] += cyzEAl
        if not fix2[2]:
            K[p1x, p2z] -= cxzEAl
            K[p1y, p2z] -= cyzEAl
            K[p1z, p2z] -= czzEAl
            K[p2x, p2z] += cxzEAl
            K[p2y, p2z] += cyzEAl
            K[p2z, p2z] += czzEAl

    return csr_matrix(K), csc_matrix(F)


def prepare_results(X: Float64, nodes: Float64, fixes: PointDict, forces: PointDict) -> Float64:
    results = nodes.copy()

    for i in range(nodes.shape[0]):
        fix = fixes.get(i) or [0, 0, 0]
        force = forces.get(i) or [0, 0, 0]

        px = 3 * i + 0
        py = 3 * i + 1
        pz = 3 * i + 2

        if not fix[0]:
            results[i][0] += X[px]
        else:
            force[0] = float(X[px])

        if not fix[1]:
            results[i][1] += X[py]
        else:
            force[1] = float(X[py])

        if not fix[2]:
            results[i][2] += X[pz]
        else:
            force[2] = float(X[pz])

        if sum(force) != 0:
            forces[i] = force

    return results
