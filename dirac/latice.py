import numpy as np
import pyvista as pv

if __name__ == '__main__':
    n = 31

    points = np.zeros((n ** 2, 3), dtype=np.float64)

    for x in range(n):
        for y in range(n):
            points[n * x + y] = [x, y, 0]

    lines = np.zeros((2 * n * (n - 1), 2), dtype=np.int32)

    for x in range(n - 1):
        for y in range(n):
            lines[n * x + y] = [n * x + y, n * (x + 1) + y]

    offset = n * (n - 1)
    for x in range(n):
        for y in range(n - 1):
            lines[offset + (n - 1) * x + y] = [n * x + y, n * x + (y + 1)]

    fix = np.zeros(4 * (n - 1), dtype=np.int32)

    print(points)
    print(lines)

    pl = pv.Plotter()
    pl.add_points(points, color='darkblue', point_size=4)
    pl.add_lines(points[np.hstack(lines)], color='lightblue', width=1)
    pl.add_axes()
    pl.camera_position = 'xy'
    pl.background_color = 'grey'
    cpos = pl.show(return_cpos=True)
    print(cpos)
