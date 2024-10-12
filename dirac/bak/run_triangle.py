import math

import numpy as np
import pyvista as pv

n = 2


def series_sum4(nn: int):
    # 3 + 3 * 4 + 3 * 4 * 4 + ...
    q = 4
    return 3 * (q ** nn - 1) // (q - 1)


length = series_sum4(n)

print(length)

points = np.zeros((length, 3), dtype=np.float64)
lines = np.zeros((length, 2), dtype=np.int32)


def sub_triangle(triangle: np.array, ii: int, nn: int):
    if nn:
        p0 = triangle[0]
        p1 = triangle[1]
        p2 = triangle[2]

        p01 = (p0 + p1) / 2
        p12 = (p1 + p2) / 2
        p20 = (p2 + p0) / 2

        points[ii + 0] = p01
        points[ii + 1] = p12
        points[ii + 2] = p20

        lines[ii + 0] = [ii + 0, ii + 1]
        lines[ii + 1] = [ii + 1, ii + 2]
        lines[ii + 2] = [ii + 2, ii + 0]

        ii += 3

        ii += sub_triangle(np.array([p01, p12, p20]), ii, nn - 1)
        ii += sub_triangle(np.array([triangle[0], p01, p20]), ii, nn - 1)
        ii += sub_triangle(np.array([p01, triangle[1], p12]), ii, nn - 1)
        ii += sub_triangle(np.array([p20, p12, triangle[2]]), ii, nn - 1)

        sum4 = series_sum4(nn)
        print(f'sum: {sum4}, ii: {ii}')
        return sum4

    return 0


if __name__ == '__main__':
    outer = np.array([[0, 0, 0], [1, 0, 0], [1 / 2, math.sqrt(3) / 2, 0]])
    sub_triangle(outer, 0, n)

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
