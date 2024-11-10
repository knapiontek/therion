import sys

import numpy as np
import pyvista as pv
from scipy.sparse.linalg import spsolve

from main.transform import check_conflicts, prepare_equation, prepare_results

np.set_printoptions(precision=2, suppress=True, threshold=sys.maxsize, linewidth=sys.maxsize)

n = 3


def n0(x1: int, y1: int):
    return x1 * n + y1


def n1(x1: int, y1: int):
    return x1 * (n - 1) + y1


nodes = np.zeros((n ** 2, 3), dtype=np.float64)

for x in range(n):
    for y in range(n):
        nodes[n0(x, y)] = [x + (.5 * y), y, .1 * x * y]

edges_xy = np.zeros(((n - 1) ** 2, 6), dtype=np.int32)
edges_x = np.zeros(((n - 1), 2), dtype=np.int32)
edges_y = np.zeros(((n - 1), 2), dtype=np.int32)

for x in range(n):
    for y in range(n):
        if x == n - 1 and y == n - 1:
            pass
        elif x == n - 1:
            edges_y[y] = [n0(x, y), n0(x, y + 1)]
        elif y == n - 1:
            edges_x[x] = [n0(x, y), n0(x + 1, y)]
        else:
            edges_xy[n1(x, y)] = [n0(x, y), n0(x + 1, y),
                                  n0(x + 1, y), n0(x, y + 1),
                                  n0(x, y + 1), n0(x, y)]

edges = np.append(np.hstack(edges_xy), np.append(np.hstack(edges_x), np.hstack(edges_y)))
edges = edges.reshape((edges.shape[0] // 2, 2))

if __name__ == '__main__':
    fixes = {
        n0(0, 0): [1, 1, 1],
        n0(0, n - 1): [1, 0, 1],
        n0(n - 1, 0): [1, 0, 1],
        n0(n - 1, n - 1): [1, 0, 1],
    }

    middle = n // 2
    forces = {n0(middle, middle): [0, 0, 30]}
    centers = nodes[sorted(forces.keys())]
    arrows = np.array(list(dict(sorted(forces.items())).values()))

    check_conflicts(fixes, forces)
    K, F = prepare_equation(nodes, edges, fixes, forces)
    print(K.todense())
    det = np.linalg.det(K.todense())
    print(det)
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
