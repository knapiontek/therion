import math

import numpy as np
import pyvista as pv

if __name__ == '__main__':
    n = 23
    w = 1 / 2
    h = math.sqrt(3.) / 2.

    # points

    points = np.zeros((n ** 2 + (n - 1) ** 2, 3), dtype=np.float64)

    for x in range(n):
        for y in range(n):
            points[x * n + y] = [x, 2 * h * y, 0]

    offset1 = n ** 2

    for x in range(n - 1):
        for y in range(n - 1):
            points[offset1 + x * (n - 1) + y] = [w + x, h + 2 * h * y, 0]

    # lines

    lines = np.zeros((2 * (n - 1) * n + 4 * (n - 1) ** 2, 2), dtype=np.int32)

    for x in range(n - 1):
        for y in range(n):
            print(x * n + y)
            lines[x * n + y] = [x * n + y, (x + 1) * n + y]

    offset2 = (n - 1) * n

    for x in range(n):
        for y in range(n - 1):
            print(offset2 + x * (n - 1) + y)
            lines[offset2 + x * (n - 1) + y] = [x * n + y, x * n + y + 1]

    offset3 = 2 * offset2

    for x in range(n - 1):
        for y in range(n - 1):
            i1 = offset3 + 4 * (x * (n - 1) + y)
            i2 = offset1 + x * (n - 1) + y
            print(i1 + 0)
            lines[i1 + 0] = [x * n + y, i2]

            print(i1 + 1)
            lines[i1 + 1] = [x * n + y + 1, i2]

            print(i1 + 2)
            lines[i1 + 2] = [(x + 1) * n + y, i2]

            print(i1 + 3)
            lines[i1 + 3] = [(x + 1) * n + y + 1, i2]

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
