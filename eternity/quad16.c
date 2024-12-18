
#include "eternity/quad16.h"

Quad quad_mx[QUAD_MX_SIZE2] = {
	// A
	{ { 15,  8,  0, 0  } }, //  1
	{ { 15, 21,  0, 0  } }, //  2
	{ {  7,  8,  0, 0  } }, //  3
	{ {  8,  7,  0, 0  } }, //  4
	{ { 14, 15,  0, 15 } }, //  5
	{ { 10,  7,  0, 15 } }, //  6
	{ { 11, 15,  0, 15 } }, //  7
	{ { 11, 22,  0, 15 } }, //  8

	{ {  2,  8,  0, 15 } }, //  9
	{ { 12, 21,  0, 15 } }, // 10
	{ {  5,  7,  0, 15 } }, // 11
	{ {  6, 21,  0, 15 } }, // 12
	{ {  6, 22,  0, 15 } }, // 13
	{ {  4, 21,  0, 15 } }, // 14
	{ { 10, 15,  0,  7 } }, // 15
	{ {  9,  8,  0,  7 } }, // 16

	{ { 20, 22,  0,  7 } }, // 17
	{ {  1, 22,  0,  7 } }, // 18
	{ { 12,  7,  0,  7 } }, // 19
	{ {  5,  7,  0,  7 } }, // 20
	{ { 17, 21,  0,  7 } }, // 21
	{ { 19, 15,  0,  7 } }, // 22
	{ { 19, 22,  0,  7 } }, // 23
	{ { 16, 15,  0,  7 } }, // 24

	{ {  4, 15,  0,  7 } }, // 25
	{ { 14,  7,  0,  8 } }, // 26
	{ { 14,  8,  0,  8 } }, // 27
	{ { 10,  8,  0,  8 } }, // 28
	{ {  9,  8,  0,  8 } }, // 29
	{ { 12, 22,  0,  8 } }, // 30
	{ {  5,  7,  0,  8 } }, // 31
	{ { 16,  8,  0,  8 } }, // 32

	{ {  6,  7,  0,  8 } }, // 33
	{ {  6, 21,  0,  8 } }, // 34
	{ { 13, 22,  0,  8 } }, // 35
	{ { 18, 21,  0,  8 } }, // 36
	{ {  9, 15,  0, 21 } }, // 37
	{ {  3, 22,  0, 21 } }, // 38
	{ {  2, 22,  0, 21 } }, // 39
	{ {  1,  7,  0, 21 } }, // 40

	{ {  1,  8,  0, 21 } }, // 41
	{ {  5, 15,  0, 21 } }, // 42
	{ {  5,  7,  0, 21 } }, // 43
	{ {  5,  8,  0, 21 } }, // 44
	{ { 17, 15,  0, 21 } }, // 45
	{ { 16, 21,  0, 21 } }, // 46
	{ {  6, 21,  0, 21 } }, // 47
	{ { 13, 21,  0, 21 } }, // 48

	// B
	{ { 14, 22,  0, 22 } }, // 49
	{ { 10, 15,  0, 22 } }, // 50
	{ { 10,  7,  0, 22 } }, // 51
	{ { 11, 15,  0, 22 } }, // 52
	{ { 12, 21,  0, 22 } }, // 53
	{ { 12, 21,  0, 22 } }, // 54
	{ { 12, 22,  0, 22 } }, // 55
	{ {  6,  8,  0, 22 } }, // 56

	{ { 13, 15,  0, 22 } }, // 57
	{ { 13, 22,  0, 22 } }, // 58
	{ {  4,  7,  0, 22 } }, // 59
	{ { 18,  8,  0, 22 } }, // 60
	{ { 11,  9, 14, 14 } }, // 61
	{ { 20, 12, 14, 14 } }, // 62
	{ { 10,  3, 14, 10 } }, // 63
	{ { 14,  6, 14,  9 } }, // 64

	{ {  9, 18, 14,  9 } }, // 65
	{ { 20, 20, 14,  9 } }, // 66
	{ {  2, 10, 14,  9 } }, // 67
	{ { 16, 11, 14,  9 } }, // 68
	{ { 18,  6, 14,  9 } }, // 69
	{ {  3, 12, 14,  3 } }, // 70
	{ { 12, 19, 14,  3 } }, // 71
	{ { 20,  9, 14,  2 } }, // 72

	{ {  5, 17, 14,  2 } }, // 73
	{ { 16,  5, 14,  2 } }, // 74
	{ {  6,  3, 14,  2 } }, // 75
	{ { 20,  5, 14,  1 } }, // 76
	{ {  1,  5, 14,  1 } }, // 77
	{ {  3, 13, 14, 12 } }, // 78
	{ { 16,  3, 14, 12 } }, // 79
	{ { 13,  4, 14, 12 } }, // 80

	{ {  1,  9, 14,  5 } }, // 81
	{ {  9,  9, 14, 17 } }, // 82
	{ {  2, 17, 14, 17 } }, // 83
	{ {  9,  1, 14, 19 } }, // 84
	{ { 11, 20, 14, 19 } }, // 85
	{ {  6, 19, 14, 19 } }, // 86
	{ { 13, 16, 14, 19 } }, // 87
	{ { 14,  4, 14, 16 } }, // 88

	{ { 11, 18, 14, 16 } }, // 89
	{ { 17, 13, 14, 16 } }, // 90
	{ {  2, 19, 14,  6 } }, // 91
	{ {  1,  5, 14,  6 } }, // 92
	{ {  1, 17, 14,  6 } }, // 93
	{ { 17,  4, 14,  6 } }, // 94
	{ { 19, 20, 14,  6 } }, // 95
	{ {  4,  3, 14,  4 } }, // 96

	// C
	{ { 17,  1, 14, 18 } }, // 97
	{ { 16,  6, 14, 18 } }, // 98
	{ {  4, 11, 14, 18 } }, // 99
	{ { 18,  4, 14, 18 } }, // 00
	{ { 19,  5, 10, 10 } }, // 101
	{ { 19, 13, 10, 10 } }, // 102
	{ { 13,  1, 10, 10 } }, // 103
	{ { 18, 11, 10, 10 } }, // 104

	{ { 17,  5, 10,  9 } }, // 105
	{ {  3,  6, 10, 11 } }, // 106
	{ {  1,  6, 10, 11 } }, // 107
	{ { 17,  5, 10, 11 } }, // 108
	{ { 13,  2, 10, 11 } }, // 109
	{ {  5, 19, 10, 20 } }, // 110
	{ { 19,  5, 10, 20 } }, // 111
	{ { 16,  1, 10,  3 } }, // 112

	{ { 16, 13, 10,  3 } }, // 113
	{ { 20, 17, 10,  2 } }, // 114
	{ { 12, 19, 10,  2 } }, // 115
	{ { 19,  2, 10,  2 } }, // 116
	{ { 18, 13, 10,  2 } }, // 117
	{ {  3,  4, 10,  1 } }, // 118
	{ { 13, 19, 10, 12 } }, // 119
	{ {  6, 18, 10,  5 } }, // 120

	{ { 20, 18, 10, 17 } }, // 121
	{ { 11, 13, 10, 16 } }, // 122
	{ { 20,  1, 10, 16 } }, // 123
	{ { 16,  5, 10, 16 } }, // 124
	{ { 19, 18, 10,  6 } }, // 125
	{ {  4,  5, 10,  6 } }, // 126
	{ { 20, 11, 10, 13 } }, // 127
	{ {  1,  4, 10, 13 } }, // 128

	{ { 17,  3, 10, 13 } }, // 129
	{ { 16,  6, 10, 13 } }, // 130
	{ { 11, 16, 10,  4 } }, // 131
	{ { 19, 20, 10,  4 } }, // 132
	{ { 20, 13, 10, 18 } }, // 133
	{ {  2, 17, 10, 18 } }, // 134
	{ { 17,  3, 10, 18 } }, // 135
	{ { 13, 16, 10, 18 } }, // 136

	{ { 16, 12,  9,  9 } }, // 137
	{ { 11,  3,  9, 11 } }, // 138
	{ { 11,  2,  9, 11 } }, // 139
	{ { 11, 19,  9, 11 } }, // 140
	{ {  1,  4,  9, 11 } }, // 141
	{ {  5, 11,  9, 11 } }, // 142
	{ { 13, 19,  9, 11 } }, // 143
	{ {  4,  4,  9, 11 } }, // 144

	// D
	{ {  3, 17,  9, 20 } }, // 145
	{ {  9, 19,  9,  3 } }, // 146
	{ {  9, 18,  9,  3 } }, // 147
	{ {  3, 20,  9,  3 } }, // 148
	{ {  5, 19,  9,  3 } }, // 149
	{ { 11,  2,  9,  1 } }, // 150
	{ { 17, 18,  9,  1 } }, // 151
	{ {  2,  2,  9, 12 } }, // 152

	{ {  2,  1,  9, 12 } }, // 153
	{ { 18, 13,  9, 12 } }, // 154
	{ { 12, 12,  9, 17 } }, // 155
	{ { 12, 19,  9, 17 } }, // 156
	{ { 18, 12,  9, 17 } }, // 157
	{ { 12, 13,  9, 16 } }, // 158
	{ {  6, 11,  9, 16 } }, // 159
	{ {  4,  4,  9,  6 } }, // 160

	{ { 11, 16,  9, 13 } }, // 161
	{ { 20, 16,  9, 13 } }, // 162
	{ {  5,  2,  9, 18 } }, // 163
	{ { 17, 13,  9, 18 } }, // 164
	{ {  3, 17, 11, 20 } }, // 165
	{ { 17,  6, 11, 20 } }, // 166
	{ {  3,  3, 11,  2 } }, // 167
	{ {  2,  5, 11,  1 } }, // 168

	{ {  1,  4, 11,  1 } }, // 169
	{ { 17,  6, 11, 12 } }, // 170
	{ { 16, 13, 11, 12 } }, // 171
	{ {  4,  2, 11, 12 } }, // 172
	{ {  5,  5, 11,  5 } }, // 173
	{ { 19, 16, 11,  5 } }, // 174
	{ { 12,  4, 11, 17 } }, // 175
	{ { 12,  1, 11, 19 } }, // 176

	{ { 20, 17, 11, 16 } }, // 177
	{ { 19, 13, 11,  6 } }, // 178
	{ {  6,  5, 11,  6 } }, // 179
	{ { 12, 13, 11, 13 } }, // 180
	{ {  2, 13, 11,  4 } }, // 181
	{ { 12,  2, 11,  4 } }, // 182
	{ {  1,  6, 20, 20 } }, // 183
	{ { 18, 12, 20,  3 } }, // 184

	{ {  1, 19, 20,  2 } }, // 185
	{ {  6,  6, 20,  2 } }, // 186
	{ {  4, 12, 20,  1 } }, // 187
	{ { 20,  4, 20, 12 } }, // 188
	{ {  3,  1, 20, 12 } }, // 189
	{ { 13,  1, 20, 12 } }, // 190
	{ {  3,  3, 20,  5 } }, // 191
	{ {  5, 16, 20,  5 } }, // 192

	// E
	{ {  2, 12, 20, 17 } }, // 193
	{ {  4,  2, 20, 19 } }, // 194
	{ {  4, 18, 20, 19 } }, // 195
	{ { 18,  5, 20, 19 } }, // 196
	{ {  2, 18, 20, 16 } }, // 197
	{ {  1,  6, 20, 16 } }, // 198
	{ { 16, 16, 20, 16 } }, // 199
	{ {  1,  3, 20,  6 } }, // 200

	{ {  4,  6, 20, 13 } }, // 201
	{ { 18, 12, 20, 13 } }, // 202
	{ { 19,  1, 20,  4 } }, // 203
	{ { 19,  6, 20,  4 } }, // 204
	{ { 13,  3, 20,  4 } }, // 205
	{ { 18,  4, 20,  4 } }, // 206
	{ { 18, 12,  3,  3 } }, // 207
	{ {  1, 18,  3,  2 } }, // 208

	{ {  6,  5,  3,  2 } }, // 209
	{ {  2,  1,  3, 12 } }, // 210
	{ { 13,  5,  3, 12 } }, // 211
	{ {  3, 13,  3,  5 } }, // 212
	{ {  6,  6,  3, 17 } }, // 213
	{ {  3, 16,  3, 19 } }, // 214
	{ { 17, 18,  3, 19 } }, // 215
	{ { 18,  4,  3, 19 } }, // 216

	{ {  1,  5,  3, 16 } }, // 217
	{ { 17, 19,  3, 13 } }, // 218
	{ {  2, 17,  3,  4 } }, // 219
	{ {  2, 13,  3, 18 } }, // 220
	{ {  4,  4,  3, 18 } }, // 221
	{ {  4, 18,  3, 18 } }, // 222
	{ { 19, 18,  2,  2 } }, // 223
	{ { 16, 12,  2,  2 } }, // 224

	{ { 13,  5,  2,  2 } }, // 225
	{ {  1,  5,  2, 12 } }, // 226
	{ { 19, 19,  2, 12 } }, // 227
	{ {  1,  6,  2, 17 } }, // 228
	{ { 12, 18,  2, 16 } }, // 229
	{ { 13,  6,  2, 16 } }, // 230
	{ { 19, 16,  2,  6 } }, // 231
	{ { 19,  4,  2,  6 } }, // 232

	{ {  1, 16,  1,  1 } }, // 233
	{ { 13, 17,  1, 12 } }, // 234
	{ { 12, 17,  1, 17 } }, // 235
	{ { 12, 16,  1, 17 } }, // 236
	{ { 19,  5,  1, 17 } }, // 237
	{ { 17, 13,  1, 19 } }, // 238
	{ {  5, 18,  1, 16 } }, // 239
	{ {  4,  5,  1, 16 } }, // 240

	// F
	{ { 12,  4,  1,  6 } }, // 241
	{ { 17,  4,  1,  6 } }, // 242
	{ {  5, 19, 12,  5 } }, // 243
	{ { 16,  6, 12,  5 } }, // 244
	{ { 18, 16, 12, 17 } }, // 245
	{ { 13, 18, 12,  4 } }, // 246
	{ {  4, 18,  5,  5 } }, // 247
	{ { 19, 17,  5, 17 } }, // 248
	{ { 17,  4,  5, 19 } }, // 249
	{ { 13,  4,  5, 16 } }, // 250
	{ { 18,  6, 17, 17 } }, // 251
	{ {  6, 19, 17, 19 } }, // 252
	{ { 13, 16, 19,  6 } }, // 253
	{ {  4, 13, 16, 13 } }, // 254
	{ { 13, 18, 16, 18 } }, // 255
	{ {  4, 18,  6, 18 } }  // 256
};

