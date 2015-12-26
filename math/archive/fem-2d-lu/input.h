#ifndef INPUT_H
#define INPUT_H

const int NODE_NO = 4;
const int ELEMENT_NO = 5;
const int DOF_NO = 2 * NODE_NO;

struct Point { double x, y; };
struct Element { int n1, n2; };
struct Data { Point node, force, shift, fix; };

extern Data data_list[NODE_NO];
extern Element element_list[ELEMENT_NO];

#endif // INPUT_H
