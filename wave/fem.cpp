#include "mesh.h"
#include <cmath>

const int point_no = 4;
const int element_no = 6;

// geometry nodes
const Point2D point_list[point_no] = {
    {-2.0, -2.0}, // 0
    {-2.0, 2.0},  // 1
    {2.5, -2.0},  // 2
    {-2.0, -2.0}  // 3
};

// geometry, refers to point_list or/and output_list
const Element element_list[element_no] = {
    {0, 1}, // 0
    {1, 2}, // 1
    {2, 0}, // 2
    {3, 0}, // 3
    {3, 1}, // 4
    {3, 2}  // 5
};

// fixed point displacement list, boolean values
Point2D fix_list[point_no] = {
    {1, 0}, // 0
    {1, 0}, // 1
    {0, 0}, // 2
    {1, 1}  // 3
};

// forces attached to coresponding points
Point2D force_list[point_no] = {
    {0.0, 0.0},  // 0
    {0.0, 0.0},  // 1
    {0.0, 30.0}, // 2
    {0.0, 0.0}   // 3
};

// displaced points
Point2D output_list[point_no] = {
    {0.0, 0.0}, // 0
    {0.0, 0.0}, // 1
    {0.0, 0.0}, // 2
    {0.0, 0.0}  // 3
};

const int dof_no = 2 * point_no;    // degrees of freedom
double K[dof_no][dof_no] = {{0.0}}; // stiffness matrix
double F[dof_no] = {0.0};           // force vector

void populate_equation()
{
    // init F, K
    for (int i = 0; i < point_no; i++) {
        const Point2D& fix = fix_list[i];
        Point2D& force = force_list[i];
        int px = 2 * i + 0;
        int py = 2 * i + 1;

        // reactions in K (in place of fixed displacement)
        if (fix.x)
            K[px][px] = -1;
        else
            F[px] = force.x;

        if (fix.y)
            K[py][py] = -1;
        else
            F[py] = force.y;
    }

    // compose K - stiffness matrix
    for (int i = 0; i < element_no; i++) {
        const double EA = 1000; // Young * Area
        const Element& element = element_list[i];
        const Point2D& point1 = point_list[element.p1];
        const Point2D& point2 = point_list[element.p2];
        const Point2D& fix1 = fix_list[element.p1];
        const Point2D& fix2 = fix_list[element.p2];

        int p1x = 2 * element.p1 + 0;
        int p1y = 2 * element.p1 + 1;
        int p2x = 2 * element.p2 + 0;
        int p2y = 2 * element.p2 + 1;

        double dx = point2.x - point1.x;
        double dy = point2.y - point1.y;
        double l = sqrt(dx * dx + dy * dy);
        double cx = dx / l;
        double cy = dy / l;
        double cxxEAl = cx * cx * EA / l;
        double cyyEAl = cy * cy * EA / l;
        double cxyEAl = cx * cy * EA / l;

        if (!fix1.x) {
            K[p1x][p1x] += cxxEAl;
            K[p1y][p1x] += cxyEAl;
            K[p2x][p1x] -= cxxEAl;
            K[p2y][p1x] -= cxyEAl;
        }
        if (!fix1.y) {
            K[p1x][p1y] += cxyEAl;
            K[p1y][p1y] += cyyEAl;
            K[p2x][p1y] -= cxyEAl;
            K[p2y][p1y] -= cyyEAl;
        }
        if (!fix2.x) {
            K[p1x][p2x] -= cxxEAl;
            K[p1y][p2x] -= cxyEAl;
            K[p2x][p2x] += cxxEAl;
            K[p2y][p2x] += cxyEAl;
        }
        if (!fix2.y) {
            K[p1x][p2y] -= cxyEAl;
            K[p1y][p2y] -= cyyEAl;
            K[p2x][p2y] += cxyEAl;
            K[p2y][p2y] += cyyEAl;
        }
    }
}
