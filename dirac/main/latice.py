import pyvista as pv
from scipy.sparse.linalg import spsolve

from transform import *

if __name__ == '__main__':
    n = 5

    nodes = np.zeros((n ** 2, 3), dtype=np.float64)

    for x in range(n):
        for y in range(n):
            nodes[n * x + y] = [x, y, 0]

    edges = np.zeros((2 * n * (n - 1), 2), dtype=np.int32)

    for x in range(n - 1):
        for y in range(n):
            edges[n * x + y] = [n * x + y, n * (x + 1) + y]

    offset = n * (n - 1)
    for x in range(n):
        for y in range(n - 1):
            edges[offset + (n - 1) * x + y] = [n * x + y, n * x + (y + 1)]

    fix_keys = np.zeros(4 * (n - 1), dtype=np.int32)
    for i in range(n - 1):
        fix_keys[0 * (n - 1) + i] = i
        fix_keys[1 * (n - 1) + i] = (n - 1) + n * i
        fix_keys[2 * (n - 1) + i] = (n - 1) * n + i + 1
        fix_keys[3 * (n - 1) + i] = (n - 1) + n * i + 1
    fixes = {k: [1, 1, 1] for k in fix_keys}

    middle = n // 2
    forces = {middle * n + middle: [0, 0, 30]}
    centers = nodes[sorted(forces.keys())]
    arrows = np.array(list(dict(sorted(forces.items())).values()))

    check_conflicts(fixes, forces)
    K, F = prepare_equation(nodes, edges, fixes, forces)
    print(K.todense())
    X = spsolve(K, F)
    diff = K.dot(X) - F.toarray().flatten()
    print(f'precision: {diff.dot(diff)}')

    results = prepare_results(X, nodes, fixes, forces)

    print(nodes)
    print(edges)

    pl = pv.Plotter()

    pl.add_lines(nodes[np.hstack(edges)], color='lightblue', width=1)
    pl.add_points(nodes[fix_keys], color='darkblue', point_size=4)
    pl.add_arrows(centers, arrows, color='red', mag=0.02, line_width=2)

    pl.add_axes()
    pl.camera_position = 'xy'
    pl.background_color = 'grey'
    cpos = pl.show(return_cpos=True)
    print(cpos)
