import pyvista as pv

from data import *
from populate import *

if __name__ == '__main__':
    K, F = populate_equation(point_no, point_list, element_no, element_list, fix_dict, force_dict)
    dP = np.linalg.solve(K, F)
    eq = np.allclose(np.dot(K, dP), F)
    print(f'eq: {eq}')

    output_list = copy_results(dP, point_no, point_list, fix_dict, force_dict)

    n = len(point_list)

    points = np.array(point_list)
    lines = np.array(element_list)

    output_points = np.array(output_list)

    centers = output_points[sorted(force_dict.keys())]
    forces = np.array(list(dict(sorted(force_dict.items())).values()))

    print(points)
    print(output_points)
    print(lines)
    print(centers)
    print(forces)

    pl = pv.Plotter()

    pl.add_points(points, color='darkblue', point_size=4)
    pl.add_lines(points[np.hstack(lines)], color='lightgrey', width=1)

    pl.add_points(output_points, color='darkred', point_size=4)
    pl.add_lines(output_points[np.hstack(lines)], color='blue', width=2)

    pl.add_arrows(centers, forces, color='red', mag=0.01, line_width=2)

    pl.add_axes()
    pl.camera_position = 'xz'
    pl.background_color = 'grey'
    cpos = pl.show(return_cpos=True)
    print(cpos)
