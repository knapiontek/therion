
#ifndef ETERNITY_QUAD16_H
#define ETERNITY_QUAD16_H

#define QUAD_MX_SIZE 16
#define QUAD_MX_SIZE2 256
#define PATTERN_SIZE 23

typedef struct Quad {
	uint pattern[4];
	uint usage;
} Quad;

extern Quad quad_mx[QUAD_MX_SIZE2];

#endif // ETERNITY_QUAD16_H

