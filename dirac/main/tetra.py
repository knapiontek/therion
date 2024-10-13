import pyvista as pv
from scipy.sparse.linalg import spsolve

from data import *
from transform import *

if __name__ == '__main__':
    K, F = prepare_equation(nodes, elements, fixes, forces)
    X = spsolve(K, F)
    eq = np.allclose(np.dot(K.toarray(), X), F.toarray())
    print(f'eq: {eq}')

    results = prepare_results(X, nodes, fixes, forces)

    centers = results[sorted(forces.keys())]
    arrows = np.array(list(dict(sorted(forces.items())).values()))

    print(nodes)
    print(results)
    print(elements)
    print(centers)
    print(arrows)

    pl = pv.Plotter()

    pl.add_points(nodes, color='darkblue', point_size=4)
    pl.add_lines(nodes[np.hstack(elements)], color='lightgrey', width=1)

    pl.add_points(results, color='darkred', point_size=4)
    pl.add_lines(results[np.hstack(elements)], color='blue', width=2)

    pl.add_arrows(centers, arrows, color='red', mag=0.01, line_width=2)

    pl.add_axes()
    pl.camera_position = 'xz'
    pl.background_color = 'grey'
    cpos = pl.show(return_cpos=True)
    print(cpos)
