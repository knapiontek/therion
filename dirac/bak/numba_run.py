from __future__ import annotations

import numba
import numpy as np
import pyvista as pv

if __name__ == '__main__':
    array = np.arange(2000, dtype=np.float_)
    print(numba.typeof(array))
    print(numba.typeof(array.reshape((2, 10, 100))))


    @numba.jit('float64(float64[:])', nopython=True)
    def sum_all(A):
        """Naive sum of elements of an array... assumes one dimensional array of floats"""
        acc = 0.0
        for i in range(A.shape[0]):
            acc += A[i]
        return acc


    sample_array = np.arange(10000.0)
    print(sample_array)
    print(sum_all(sample_array))


    @numba.vectorize('float64(float64, float64, float64)')
    def lerp(A, B, factor):
        """interpolates A and B by factor"""
        return (1 - factor) * A + factor * B


    print(lerp(0.0, 10.0, 0.3))

    A = np.arange(0.0, 100000.0, 2.0)
    B = np.arange(100000.0, 0.0, -2.0)
    lerp(A, B, 0.5)


    @numba.guvectorize('float64[:,:], float64[:,:], float64[:,:]', '(m,n),(n,p)->(n,p)')
    def matmulcore(A, B, C):
        m, n = A.shape
        n, p = B.shape
        for i in range(m):
            for j in range(p):
                C[i, j] = 0
                for k in range(n):
                    C[i, j] += A[i, k] * B[k, j]


    matrix_ct = 10000
    gu_test_A = np.arange(matrix_ct * 2 * 4, dtype=np.float32).reshape(matrix_ct, 2, 4)
    gu_test_B = np.arange(matrix_ct * 4 * 5, dtype=np.float32).reshape(matrix_ct, 4, 5)
    print(matmulcore(gu_test_A, gu_test_B))

    arr = np.array([(1, 2)], dtype=[('a1', 'f8'), ('a2', 'f8')])
    fields_gl = ('a1', 'a2')


    @numba.njit
    def get_field_sum(rec):
        fields_lc = ('a1', 'a2')
        field_name1 = fields_lc[0]
        field_name2 = fields_gl[1]
        print(field_name1, field_name2)
        return rec[field_name1] + rec[field_name2]


    res = get_field_sum(arr[0])  # returns 3
    print(res)
    print(arr)
    print(arr[0])

    vertices = np.array([[0.0, 0.0, 0.0], [1, 0, 0], [1, 1, 0], [0, 1, 0]])
    surf = pv.PolyData(vertices)
    surf.plot()
