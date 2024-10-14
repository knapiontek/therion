import numpy as np

nodes = np.array([
    [-2.0, -2.0, -2.0],  # 0
    [-2.0, 2.0, -2.0],  # 1
    [2.5, -2.0, -2.0],  # 2
    [-2.0, -2.0, 4.0]  # 3
], dtype=np.float64)

edges = np.array([
    [0, 1],  # 0
    [1, 2],  # 1
    [2, 0],  # 2
    [3, 0],  # 3
    [3, 1],  # 4
    [3, 2]  # 5
], dtype=np.int32)

# boolean values
fixes = {
    0: [1, 0, 1],
    1: [1, 0, 1],
    3: [1, 1, 1]
}

forces = {
    2: [0.0, 30.0, 30.0]
}
