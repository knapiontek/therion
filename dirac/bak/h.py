import numpy as np
import pyvista as pv

if __name__ == '__main__':
    points = np.array([
        [0, 0, 0],
        [5, 0, 0],
        [5, 1, 0],
        [3, 1, 0],
        [3, 4, 0],
        [5, 4, 0],
        [5, 5, 0],
        [0, 5, 0],
        [0, 4, 0],
        [2, 4, 0],
        [2, 1, 0],
        [0, 1, 0],

        [0, 0, 2],
        [5, 0, 2],
        [5, 1, 2],
        [3, 1, 2],
        [3, 4, 2],
        [5, 4, 2],
        [5, 5, 2],
        [0, 5, 2],
        [0, 4, 2],
        [2, 4, 2],
        [2, 1, 2],
        [0, 1, 2]
    ], dtype=np.float64)

    lines = np.array([
        [0, 1],
        [1, 2],
        [2, 3],
        [3, 4],
        [4, 5],
        [5, 6],
        [6, 7],
        [7, 8],
        [8, 9],
        [9, 10],
        [10, 11],
        [11, 0],

        [12, 13],
        [13, 14],
        [14, 15],
        [15, 16],
        [16, 17],
        [17, 18],
        [18, 19],
        [19, 20],
        [20, 21],
        [21, 22],
        [22, 23],
        [23, 12],

        [0, 12],
        [1, 13],
        [2, 14],
        [3, 15],
        [4, 16],
        [5, 17],
        [6, 18],
        [7, 19],
        [8, 20],
        [9, 21],
        [10, 22],
        [11, 23]
    ], dtype=np.int32)

    pl = pv.Plotter()
    pl.add_points(points, color='darkblue', point_size=4)
    pl.add_lines(points[np.hstack(lines)], color='lightblue', width=1)
    pl.add_axes()
    pl.camera.position = [15, 10, 25]
    pl.background_color = 'grey'
    cpos = pl.show(return_cpos=True)
    print(cpos)
