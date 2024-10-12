point_no = 4
element_no = 6

# geometry nodes
point_list = [
    [-2.0, -2.0, -2.0],  # 0
    [-2.0, 2.0, -2.0],  # 1
    [2.5, -2.0, -2.0],  # 2
    [-2.0, -2.0, 4.0]  # 3
]

# geometry, refers to point_list or/and output_list
element_list = [
    [0, 1],  # 0
    [1, 2],  # 1
    [2, 0],  # 2
    [3, 0],  # 3
    [3, 1],  # 4
    [3, 2]  # 5
]

# fixed point displacement list, boolean values
fix_dict = {
    0: [1, 0, 1],
    1: [1, 0, 1],
    3: [1, 1, 1]
}

# forces attached to corresponding points
force_dict = {
    2: [0.0, 30.0, 30.0]
}

# displaced points
output_list = [
    [0.0, 0.0, 0.0],  # 0
    [0.0, 0.0, 0.0],  # 1
    [0.0, 0.0, 0.0],  # 2
    [0.0, 0.0, 0.0]  # 3
]
