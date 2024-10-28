import numpy as np
import pyvista as pv

n = 13

nodes = np.zeros((n, n, 3), dtype=np.float64)

for x in range(n):
    for y in range(n):
        nodes[x, y] = [x, y, 0]

edges = np.zeros(((n - 1), (n - 1), 6, 3), dtype=np.float64)

for x in range(n - 1):
    for y in range(n - 1):
        edges[x, y] = [nodes[x, y], nodes[x + 1, y], nodes[x + 1, y], nodes[x, y + 1], nodes[x, y + 1], nodes[x, y]]

if __name__ == '__main__':
    pl = pv.Plotter()

    pl.add_points(np.vstack(nodes), color='darkblue', point_size=4)
    pl.add_lines(np.vstack(np.hstack(edges)), color='lightgrey', width=1)

    pl.add_axes()
    pl.camera_position = 'xy'
    pl.background_color = 'grey'
    cpos = pl.show(return_cpos=True)
    print(cpos)
