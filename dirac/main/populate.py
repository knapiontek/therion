import math
from copy import deepcopy

import numpy as np


def populate_equation(point_no, point_list, element_no, element_list, fix_dict, force_dict):
    dof_no = 3 * point_no  # degrees of freedom
    K = np.zeros((dof_no, dof_no))  # stiffness matrix
    F = np.zeros(dof_no)  # force vector

    # init F, K
    for i in range(point_no):
        fix = fix_dict.get(i) or [0, 0, 0]
        force = force_dict.get(i) or [0, 0, 0]
        px = 3 * i + 0
        py = 3 * i + 1
        pz = 3 * i + 2

        # reactions in K (in place of fixed displacement)
        if fix[0]:
            K[px][px] = -1
        else:
            F[px] = force[0]

        if fix[1]:
            K[py][py] = -1
        else:
            F[py] = force[1]

        if fix[2]:
            K[pz][pz] = -1
        else:
            F[pz] = force[2]

    # compose K - stiffness matrix
    for i in range(element_no):
        EA = 5000  # Young * Area
        element = element_list[i]
        point1 = point_list[element[0]]
        point2 = point_list[element[1]]
        fix1 = fix_dict.get(element[0]) or [0, 0, 0]
        fix2 = fix_dict.get(element[1]) or [0, 0, 0]
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
            K[p1x][p1x] += cxxEAl
            K[p1y][p1x] += cxyEAl
            K[p1z][p1x] += cxzEAl
            K[p2x][p1x] -= cxxEAl
            K[p2y][p1x] -= cxyEAl
            K[p2z][p1x] -= cxzEAl
        if not fix1[1]:
            K[p1x][p1y] += cxyEAl
            K[p1y][p1y] += cyyEAl
            K[p1z][p1y] += cyzEAl
            K[p2x][p1y] -= cxyEAl
            K[p2y][p1y] -= cyyEAl
            K[p2z][p1y] -= cyzEAl
        if not fix1[2]:
            K[p1x][p1z] += cxzEAl
            K[p1y][p1z] += cyzEAl
            K[p1z][p1z] += czzEAl
            K[p2x][p1z] -= cxzEAl
            K[p2y][p1z] -= cyzEAl
            K[p2z][p1z] -= czzEAl
        if not fix2[0]:
            K[p1x][p2x] -= cxxEAl
            K[p1y][p2x] -= cxyEAl
            K[p1z][p2x] -= cxzEAl
            K[p2x][p2x] += cxxEAl
            K[p2y][p2x] += cxyEAl
            K[p2z][p2x] += cxzEAl
        if not fix2[1]:
            K[p1x][p2y] -= cxyEAl
            K[p1y][p2y] -= cyyEAl
            K[p1z][p2y] -= cyzEAl
            K[p2x][p2y] += cxyEAl
            K[p2y][p2y] += cyyEAl
            K[p2z][p2y] += cyzEAl
        if not fix2[2]:
            K[p1x][p2z] -= cxzEAl
            K[p1y][p2z] -= cyzEAl
            K[p1z][p2z] -= czzEAl
            K[p2x][p2z] += cxzEAl
            K[p2y][p2z] += cyzEAl
            K[p2z][p2z] += czzEAl

    return K, F


def copy_results(dP, point_no, point_list, fix_dict, force_dict):
    output_list = deepcopy(point_list)

    for i in range(point_no):
        fix = fix_dict.get(i) or [0, 0, 0]
        force = force_dict.get(i) or [0, 0, 0]

        px = 3 * i + 0
        py = 3 * i + 1
        pz = 3 * i + 2

        if fix[0]:
            force[0] = dP[px]
        else:
            output_list[i][0] += dP[px]

        if fix[1]:
            force[1] = dP[py]
        else:
            output_list[i][1] += dP[py]

        if fix[2]:
            force[2] = dP[pz]
        else:
            output_list[i][2] += dP[pz]

        force_dict[i] = force

    return output_list
