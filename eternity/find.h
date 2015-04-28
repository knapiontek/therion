
#ifndef ETERNITY_FIND_H
#define ETERNITY_FIND_H

#include "eternity/quad16.h"

typedef struct Edge {
	struct Edge* next;
	Quad* quad;
	uint pattern[4];
	uint pos;
} Edge;

extern Edge* edge_index[PATTERN_SIZE][PATTERN_SIZE][PATTERN_SIZE][PATTERN_SIZE];

void find_prepare_index();

#endif // ETERNITY_FIND_H

