
#include "eternity/find.h"

Edge edge_list[4 * QUAD_MX_SIZE * QUAD_MX_SIZE] = {};
Edge* edge_index[PATTERN_SIZE][PATTERN_SIZE][PATTERN_SIZE][PATTERN_SIZE] = {};

void find_prepare_index() {
	uint i;
	for(i = 0; i < 4 * QUAD_MX_SIZE * QUAD_MX_SIZE; i++) {
		Quad* quad = &quad_mx[i / 4];
		uint p0 = quad->pattern[(i + 0) % 4];
		uint p1 = quad->pattern[(i + 1) % 4];
		uint p2 = quad->pattern[(i + 2) % 4];
		uint p3 = quad->pattern[(i + 3) % 4];

		Edge* edge = &edge_list[i];
		edge->quad = quad;
		edge->pattern[0] = p0;
		edge->pattern[1] = p1;
		edge->pattern[2] = p2;
		edge->pattern[3] = p3;
		edge->pos = i % 4;

		edge->next = edge_index[p0][p1][p2][p3];
		edge_index[p0][p1][p2][p3] = edge;
	}
}

