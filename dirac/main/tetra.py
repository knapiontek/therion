import sys

import pyvista as pv
from scipy.sparse.linalg import spsolve

from transform import *

np.set_printoptions(precision=2, suppress=True, threshold=sys.maxsize, linewidth=sys.maxsize)

nodes = np.array([
    [-2.0, -2.0, -2.0],  # 0
    [-2.0, 2.0, -2.0],  # 1
    [2.5, -2.0, -2.0],  # 2
    [-2.0, -2.0, 4.0]  # 3
], dtype=np.float64)

edges = np.array([
    [0, 1],  # 0
    [1, 2],  # 1
    [2, 0],  # 2
    [3, 0],  # 3
    [3, 1],  # 4
    [3, 2]  # 5
], dtype=np.int32)

# boolean values
fixes = {
    0: [1, 0, 1],
    1: [1, 0, 1],
    3: [1, 1, 1]
}

forces = {
    2: [0.0, 30.0, 30.0]
}

if __name__ == '__main__':
    check_conflicts(fixes, forces)
    K, F = prepare_equation(nodes, edges, fixes, forces)
    X = spsolve(K, F)
    diff = K.dot(X) - F.toarray().flatten()
    print(f'precision: {diff.dot(diff)}')
    print(K.todense())
    det = np.linalg.det(K.todense())
    print(f'det: {det}')

    results = prepare_results(X, nodes, fixes, forces)

    centers = results[sorted(forces.keys())]
    arrows = np.array(list(dict(sorted(forces.items())).values()))

    print(nodes)
    print(results)
    print(edges)
    print(centers)
    print(arrows)

    pl = pv.Plotter()

    pl.add_points(nodes, color='darkblue', point_size=4)
    pl.add_lines(nodes[np.hstack(edges)], color='lightgrey', width=1)

    pl.add_points(results, color='darkred', point_size=4)
    pl.add_lines(results[np.hstack(edges)], color='blue', width=2)

    pl.add_arrows(centers, arrows, color='red', mag=0.01, line_width=2)

    pl.add_axes()
    pl.camera_position = 'xz'
    pl.background_color = 'grey'
    cpos = pl.show(return_cpos=True)
    print(cpos)
