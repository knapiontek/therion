import sys

import numpy as np
import pyvista as pv
from scipy.sparse.linalg import spsolve

from main.transform import check_conflicts, prepare_equation, prepare_results

np.set_printoptions(precision=2, suppress=True, threshold=sys.maxsize, linewidth=sys.maxsize)

n = 3
n_1 = n - 1


def n0(_x: int, _y: int, _z: int):
    return (_x * n ** 2) + (_y * n) + _z


nodes = np.zeros((n ** 3, 3), dtype=np.float64)

for x in range(n):
    for y in range(n):
        for z in range(n):
            nodes[n0(x, y, z)] = [x, y, z]

edges = []

for x in range(n):
    for y in range(n):
        for z in range(n):
            if x == n_1 and y == n_1:
                pass
            elif x == n_1:
                edges += [n0(x, y, z), n0(x, y + 1, z)]
            elif y == n_1:
                edges += [n0(x, y, z), n0(x + 1, y, z)]
            else:
                edges += [n0(x, y, z), n0(x + 1, y, z),
                          n0(x + 1, y, z), n0(x, y + 1, z),
                          n0(x, y + 1, z), n0(x, y, z)]

edges = np.array(edges)
edges = edges.reshape((edges.shape[0] // 2, 2))

if __name__ == '__main__':
    fixes = {
        n0(0, 0, 0): [1, 1, 1],
        n0(0, n_1, 0): [1, 0, 1],
        n0(n_1, 0, 0): [1, 0, 1],
        n0(n_1, n_1, 0): [1, 0, 1],
    }

    middle = n // 2
    forces = {n0(middle, middle, 1): [0, 0, 30]}
    centers = nodes[sorted(forces.keys())]
    arrows = np.array(list(dict(sorted(forces.items())).values()))

    check_conflicts(fixes, forces)
    K, F = prepare_equation(nodes, edges, fixes, forces)
    print(K.todense())
    det = np.linalg.det(K.todense())
    print(f'det: {det}')
    if det:
        X = spsolve(K, F)
        diff = K.dot(X) - F.toarray().flatten()
        print(f'precision: {diff.dot(diff)}')

        results = prepare_results(X, nodes, fixes, forces)

    print(nodes)
    print(edges)

    pl = pv.Plotter()

    pl.add_points(nodes, color='darkblue', point_size=6)
    pl.add_lines(nodes[np.hstack(edges)], color='lightgrey', width=1)
    pl.add_arrows(centers, arrows, color='red', mag=0.02, line_width=2)

    pl.add_axes()
    pl.camera_position = 'xy'
    pl.background_color = 'grey'
    cpos = pl.show(return_cpos=True)
    print(cpos)
