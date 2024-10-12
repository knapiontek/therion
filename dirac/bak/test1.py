import pyvista as pv

sphere = pv.Sphere()
cube = pv.Cube().translate([10, 10, 0])

p = pv.Plotter()
sphere_actor = p.add_mesh(sphere, pickable=True)  # initially pickable
cube_actor = p.add_mesh(cube, pickable=False)  # initially unpickable
p.enable_point_picking(pickable_window=False, left_clicking=True)

p.pickable_actors = [sphere_actor, cube_actor]  # now both are pickable
p.view_xy()
p.show()
