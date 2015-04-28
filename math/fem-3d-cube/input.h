#ifndef INPUT_H
#define INPUT_H

const int NODE_NO = 8;
const int ELEMENT_NO = 18;
const int DOF_NO = 3 * NODE_NO;

struct Point { double x, y, z; };
struct Element { int n1, n2; };
struct Data { Point node, force, shift; bool fix; };

extern Data data_list[NODE_NO];
extern Element element_list[ELEMENT_NO];

#endif // INPUT_H
