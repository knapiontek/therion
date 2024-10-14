import numpy as np
import pyvista as pv

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

    fixes = np.zeros(4 * (n - 1), dtype=np.int32)
    for i in range(n - 1):
        fixes[0 * (n - 1) + i] = i
        fixes[1 * (n - 1) + i] = (n - 1) + n * i
        fixes[2 * (n - 1) + i] = n * n - i - 1
        fixes[3 * (n - 1) + i] = (n - 1) + n * i + 1

    middle = n // 2
    forces = {middle * n + middle: [0, 0, 30]}
    centers = nodes[sorted(forces.keys())]
    arrows = np.array(list(dict(sorted(forces.items())).values()))

    print(nodes)
    print(edges)

    pl = pv.Plotter()

    pl.add_lines(nodes[np.hstack(edges)], color='lightblue', width=1)
    pl.add_points(nodes[fixes], color='darkblue', point_size=4)
    pl.add_arrows(centers, arrows, color='red', mag=0.02, line_width=2)

    pl.add_axes()
    pl.camera_position = 'xy'
    pl.background_color = 'grey'
    cpos = pl.show(return_cpos=True)
    print(cpos)
