
#include "eternity/image.h"
#include "eternity/quad16.h"
#include "eternity/find.h"
#include <cairo/cairo.h>
#include <gtk/gtk.h>
#include <math.h>
#include <assert.h>

#define R(pattern) (pallet[pattern][0])
#define G(pattern) (pallet[pattern][1])
#define B(pattern) (pallet[pattern][2])

#define MX_TO_IMAGE(x) (40 * (x) + 40.5)
#define IMAGE_SIZE MX_TO_IMAGE(QUAD_MX_SIZE) + 41

uint pattern_occur[PATTERN_SIZE] = {};
uint pattern_mx_x[QUAD_MX_SIZE + 1][QUAD_MX_SIZE] = {};
uint pattern_mx_y[QUAD_MX_SIZE][QUAD_MX_SIZE + 1] = {};

float pallet[PATTERN_SIZE][3];
cairo_surface_t* surface;
cairo_t* cairo;

static void count_pattern_occurence() {
	uint i, j, p;
	for(i = 0; i < QUAD_MX_SIZE2; i++) {
		Quad* quad = &quad_mx[i];
		for(j = 0; j < 4; j++) {
			p = quad->pattern[j];
			assert(p < PATTERN_SIZE);
			pattern_occur[p]++;
		}
	}
	i = pattern_occur[0];
	for(p = 1; p < PATTERN_SIZE; p++) {
		pattern_occur[p] /= 2;
		i += pattern_occur[p];
		printf("%d\n", pattern_occur[p]);
	}
	printf("sum: %d\n", i);
}

static void prepare_pallet() {
	uint p;
	for(p = 0; p <= PATTERN_SIZE; p++) {
		float* color = pallet[p];
		float pi = 3.14 * p / 23;
		color[0] = sin(pi / 2);
		color[1] = sin(pi);
		color[2] = sin(2 * pi) / 2 + .5;
	}
}

static void prepare_mx() {
	uint x, y, p = 1;
	// fill in internal, border is already 0
	for(x = 1; x <= QUAD_MX_SIZE - 1; x++) {
		for(y = 0; y < QUAD_MX_SIZE; y++) {
			pattern_mx_x[x][y] = p;
			assert(p < PATTERN_SIZE);
			if(--pattern_occur[p] == 0) {
				printf("p: %d occur[p]: %d\n", p, pattern_occur[p]);
				p++;
			}
			pattern_mx_y[y][x] = p;
			assert(p < PATTERN_SIZE);
			if(--pattern_occur[p] == 0) {
				printf("p: %d occur[p]: %d\n", p, pattern_occur[p]);
				p++;
			}
		}
	}
}

static gboolean on_expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer data) {
	uint x, y;

	// init
	cairo = gdk_cairo_create(widget->window);
	cairo_set_line_width(cairo, 1);

	// background
	cairo_rectangle(cairo, 0.0, 0.0, IMAGE_SIZE, IMAGE_SIZE);
	cairo_set_source_rgb(cairo, 1.0, 1.0, 1.0);
	cairo_fill(cairo);

	// pattern x
	for(x = 0; x <= QUAD_MX_SIZE; x++) {
		for(y = 0; y < QUAD_MX_SIZE; y++) {
			cairo_move_to(cairo, MX_TO_IMAGE(x), MX_TO_IMAGE(y));
			cairo_line_to(cairo, MX_TO_IMAGE(x + .5), MX_TO_IMAGE(y + .5));
			cairo_line_to(cairo, MX_TO_IMAGE(x), MX_TO_IMAGE(y + 1));
			cairo_line_to(cairo, MX_TO_IMAGE(x - .5), MX_TO_IMAGE(y + .5));
			cairo_close_path(cairo);
			cairo_stroke_preserve(cairo);
			int p = pattern_mx_x[x][y];
			cairo_set_source_rgb(cairo, R(p), G(p), B(p));
			cairo_fill(cairo);
			cairo_stroke(cairo);
		}
	}

	// pattern y
	for(x = 0; x < QUAD_MX_SIZE; x++) {
		for(y = 0; y <= QUAD_MX_SIZE; y++) {
			cairo_move_to(cairo, MX_TO_IMAGE(x + .5), MX_TO_IMAGE(y - .5));
			cairo_line_to(cairo, MX_TO_IMAGE(x + 1), MX_TO_IMAGE(y));
			cairo_line_to(cairo, MX_TO_IMAGE(x + .5), MX_TO_IMAGE(y + .5));
			cairo_line_to(cairo, MX_TO_IMAGE(x), MX_TO_IMAGE(y));
			cairo_close_path(cairo);
			cairo_set_source_rgb(cairo, 1.0, 1.0, 1.0);
			cairo_stroke_preserve(cairo);
			int p = pattern_mx_y[x][y];
			cairo_set_source_rgb(cairo, R(p), G(p), B(p));
			cairo_fill(cairo);
			cairo_stroke(cairo);
		}
	}

	// marks
	cairo_set_source_rgb(cairo, 1.0, 1.0, 1.0);
	for(x = 0; x < QUAD_MX_SIZE; x++) {
		for(y = 0; y < QUAD_MX_SIZE; y++) {
			int p0 = pattern_mx_x[x][y];
			int p1 = pattern_mx_x[x + 1][y];
			int p2 = pattern_mx_y[x][y];
			int p3 = pattern_mx_y[x][y + 1];
			if(edge_index[p0][p1][p2][p3]) {
				cairo_arc(cairo, MX_TO_IMAGE(x + .5), MX_TO_IMAGE(y + .5), 5, 0, 2 * M_PI);
				cairo_fill(cairo);
				cairo_stroke(cairo);
			}
		}
	}

	// quad
	cairo_set_source_rgb(cairo, 0.0, 0.0, 0.0);
	for(x = 0; x <= QUAD_MX_SIZE; x++) {
		cairo_move_to(cairo, MX_TO_IMAGE(0), MX_TO_IMAGE(x));
		cairo_line_to(cairo, MX_TO_IMAGE(QUAD_MX_SIZE), MX_TO_IMAGE(x));
		cairo_move_to(cairo, MX_TO_IMAGE(x), MX_TO_IMAGE(0));
		cairo_line_to(cairo, MX_TO_IMAGE(x), MX_TO_IMAGE(QUAD_MX_SIZE));
		cairo_stroke(cairo);
	}

	// release
	cairo_destroy(cairo);
	return FALSE;
}

void image_event_loop() {
	GtkWidget *window;
	GtkWidget *darea;

	count_pattern_occurence();
	prepare_pallet();
	prepare_mx();

	gtk_init(NULL, NULL);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	darea = gtk_drawing_area_new();
	gtk_container_add(GTK_CONTAINER(window), darea);

	g_signal_connect(darea, "expose-event", G_CALLBACK(on_expose_event), NULL);
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	gtk_window_set_pos(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(window), IMAGE_SIZE, IMAGE_SIZE);

	gtk_widget_show_all(window);

	gtk_main();
}

