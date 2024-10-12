import math

import numpy as np
import pyvista as pv

if __name__ == '__main__':
    n = 9
    n_1 = n - 1
    n_2 = n - 2
    w = 1 / 2
    h = math.sqrt(3.) / 2.

    # points

    points = np.zeros((n ** 2 + n_1 ** 2, 3), dtype=np.float64)

    for x in range(n):
        for y in range(n):
            points[n * x + y] = [x, 2 * h * y, 0]

    offset1 = n ** 2

    for x in range(n_1):
        for y in range(n_1):
            points[offset1 + n_1 * x + y] = [w + x, h + 2 * h * y, 0]

    # lines

    lines = np.zeros((4 * n_1 ** 2 + n_1 * n + n_2 * n_1, 2), dtype=np.int32)

    for x in range(n_1):
        for y in range(n_1):
            i1 = 4 * (n_1 * x + y)
            i2 = offset1 + n_1 * x + y
            print(i1 + 0)
            lines[i1 + 0] = [n * x + y, i2]

            print(i1 + 1)
            lines[i1 + 1] = [n * x + y + 1, i2]

            print(i1 + 2)
            lines[i1 + 2] = [n * (x + 1) + y, i2]

            print(i1 + 3)
            lines[i1 + 3] = [n * (x + 1) + y + 1, i2]

    offset2 = 4 * n_1 ** 2

    for x in range(n_1):
        for y in range(n):
            print(offset2 + n * x + y)
            lines[offset2 + n * x + y] = [n * x + y, n * (x + 1) + y]

    offset3 = offset2 + n_1 * n

    for x in range(n_2):
        for y in range(n_1):
            print(offset3 + n_1 * x + y)
            lines[offset3 + n_1 * x + y] = [offset1 + n_1 * x + y, offset1 + n_1 * (x + 1) + y]

    print(points)
    print(lines)

    # plot

    pl = pv.Plotter()
    pl.add_points(points, color='darkblue', point_size=4)
    pl.add_lines(points[np.hstack(lines)], color='lightblue', width=1)
    pl.add_axes()
    pl.camera_position = 'xy'
    pl.background_color = 'grey'
    cpos = pl.show(return_cpos=True)
    print(cpos)
