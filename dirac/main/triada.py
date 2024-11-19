import sys

import numpy as np
import pyvista as pv
from scipy.sparse.linalg import spsolve

from main.transform import check_conflicts, prepare_equation, prepare_results

np.set_printoptions(precision=2, suppress=True, threshold=sys.maxsize, linewidth=sys.maxsize)

nx, ny, nz = 9, 9, 3
nx_1, ny_1, nz_1 = nx - 1, ny - 1, nz - 1


def n0(x: int, y: int, z: int) -> int:
    return (x * ny * nz) + (y * nz) + z


def prepare_geometry():
    _nodes = np.zeros((nx * ny * nz, 3), dtype=np.float64)

    for x in range(nx):
        for y in range(ny):
            for z in range(nz):
                _nodes[n0(x, y, z)] = [x, y, z]

    _edges = []

    for x in range(nx):
        for y in range(ny):
            for z in range(nz):
                if x < nx_1:
                    _edges += [n0(x, y, z), n0(x + 1, y, z)]
                if y < ny_1:
                    _edges += [n0(x, y, z), n0(x, y + 1, z)]
                if z < nz_1:
                    _edges += [n0(x, y, z), n0(x, y, z + 1)]

                if (z % 2 == 0) != (x % 2 == y % 2):
                    if x < nx_1 and y < ny_1:
                        _edges += [n0(x, y, z), n0(x + 1, y + 1, z)]
                    if x < nx_1 and z < nz_1:
                        _edges += [n0(x, y, z), n0(x + 1, y, z + 1)]
                    if y < ny_1 and z < nz_1:
                        _edges += [n0(x, y, z), n0(x, y + 1, z + 1)]
                else:
                    if x < nx_1 and y < ny_1:
                        _edges += [n0(x + 1, y, z), n0(x, y + 1, z)]
                    if x < nx_1 and z < nz_1:
                        _edges += [n0(x + 1, y, z), n0(x, y, z + 1)]
                    if y < ny_1 and z < nz_1:
                        _edges += [n0(x, y + 1, z), n0(x, y, z + 1)]

    _edges = np.array(_edges)
    _edges = _edges.reshape((_edges.shape[0] // 2, 2))

    _fixes = {}

    for x in range(nx):
        for y in range(ny):
            if x in (0, nx_1) or y in (0, ny_1):
                _fixes[n0(x, y, 0)] = [1, 1, 1]

    return _nodes, _edges, _fixes


if __name__ == '__main__':
    nodes, edges, fixes = prepare_geometry()
    mx = nx // 2
    my = ny // 2
    forces = {n0(mx, my, 1): [0, 0, 8000]}
    centers = nodes[sorted(forces.keys())]
    arrows = np.array(list(dict(sorted(forces.items())).values()))

    check_conflicts(fixes, forces)
    K, F = prepare_equation(nodes, edges, fixes, forces)
    # print(K.todense())
    # det = np.linalg.det(K.todense())
    # print(f'det: {det}')

    X = spsolve(K, F)
    # diff = K.dot(X) - F.toarray().flatten()
    # print(f'precision: {diff.dot(diff)}')
    #
    results = prepare_results(X, nodes, fixes, forces)

    # print(nodes)
    # print(results)
    # print(edges)

    pl = pv.Plotter()

    pl.add_points(results[sorted(fixes.keys())], color='darkblue', point_size=6)
    pl.add_lines(results[np.hstack(edges)], color='lightgrey', width=1)
    pl.add_arrows(centers, arrows, color='red', mag=0.0007, line_width=2)

    pl.add_axes()
    pl.camera_position = 'xy'
    pl.background_color = 'grey'
    cpos = pl.show(return_cpos=True)
    print(cpos)
