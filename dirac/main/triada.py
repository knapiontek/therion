import numpy as np
import pyvista as pv

n = 13


def n0(x1: int, y1: int):
    return x1 * n + y1


def n1(x1: int, y1: int):
    return x1 * (n - 1) + y1


nodes = np.zeros((n ** 2, 3), dtype=np.float64)

for x in range(n):
    for y in range(n):
        nodes[n0(x, y)] = [x, y, 0]

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
            edges_xy[n1(x, y)] = [n0(x, y), n0(x + 1, y), n0(x + 1, y), n0(x, y + 1), n0(x, y + 1), n0(x, y)]

edges = np.append(np.hstack(edges_xy), np.append(np.hstack(edges_x), np.hstack(edges_y)))

if __name__ == '__main__':
    pl = pv.Plotter()

    pl.add_points(nodes, color='darkblue', point_size=4)
    pl.add_lines(nodes[edges], color='lightgrey', width=1)

    pl.add_axes()
    pl.camera_position = 'xy'
    pl.background_color = 'grey'
    cpos = pl.show(return_cpos=True)
    print(cpos)
