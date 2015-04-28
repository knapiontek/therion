
#include <stdio.h>
#include <string.h>
#include "eternity/image.h"
#include "eternity/find.h"

int main(int argc, char* argv[]) {
	find_prepare_index();
	image_event_loop();
	return 0;
}

