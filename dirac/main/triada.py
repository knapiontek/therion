import sys

import numpy as np
import pyvista as pv
from scipy.sparse.linalg import spsolve

from main.transform import check_conflicts, prepare_equation, prepare_results

np.set_printoptions(precision=2, suppress=True, threshold=sys.maxsize, linewidth=sys.maxsize)

n = 5
n_1 = n - 1


def n0(x: int, y: int, z: int):
    return (x * n ** 2) + (y * n) + z


def prepare_geometry():
    _nodes = np.zeros((n ** 3, 3), dtype=np.float64)

    for x in range(n):
        for y in range(n):
            for z in range(n):
                _nodes[n0(x, y, z)] = [x, y, z]

    _edges = []

    for x in range(n):
        for y in range(n):
            for z in range(n):
                if x < n_1:
                    _edges += [n0(x, y, z), n0(x + 1, y, z)]
                if y < n_1:
                    _edges += [n0(x, y, z), n0(x, y + 1, z)]
                if z < n_1:
                    _edges += [n0(x, y, z), n0(x, y, z + 1)]

                if x < n_1 and y < n_1:
                    _edges += [n0(x + 1, y, z), n0(x, y + 1, z)]
                if y < n_1 and z < n_1:
                    _edges += [n0(x, y + 1, z), n0(x, y, z + 1)]
                if x < n_1 and z < n_1:
                    _edges += [n0(x, y, z + 1), n0(x + 1, y, z)]

    _edges = np.array(_edges)
    _edges = _edges.reshape((_edges.shape[0] // 2, 2))

    return _nodes, _edges


if __name__ == '__main__':
    fixes = {
        n0(0, 0, 0): [1, 1, 1],
        n0(0, n_1, 0): [1, 0, 1],
        n0(n_1, 0, 0): [1, 0, 1],
        n0(n_1, n_1, 0): [1, 0, 1],
    }

    nodes, edges = prepare_geometry()
    middle = n // 2
    forces = {n0(middle, middle, n_1): [0, 0, 3000]}
    centers = nodes[sorted(forces.keys())]
    arrows = np.array(list(dict(sorted(forces.items())).values()))

    check_conflicts(fixes, forces)
    K, F = prepare_equation(nodes, edges, fixes, forces)
    # print(K.todense())
    # det = np.linalg.det(K.todense())
    # print(f'det: {det}')

    X = spsolve(K, F)
    diff = K.dot(X) - F.toarray().flatten()
    # print(f'precision: {diff.dot(diff)}')

    results = prepare_results(X, nodes, fixes, forces)

    # print(nodes)
    # print(results)
    # print(edges)

    pl = pv.Plotter()

    pl.add_points(results, color='darkblue', point_size=6)
    pl.add_lines(results[np.hstack(edges)], color='lightgrey', width=1)
    pl.add_arrows(centers, arrows, color='red', mag=0.0007, line_width=2)

    pl.add_axes()
    pl.camera_position = 'xy'
    pl.background_color = 'grey'
    cpos = pl.show(return_cpos=True)
    print(cpos)
