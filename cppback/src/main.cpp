#include "Image.h"
#include <cstring>

enum options { COLORMASK, GRAYSCALE };

void print_help();
options getOption(char* arg);

int main(int argc, char** argv) {
	// color_mask test
	if (argc != 4) {
		print_help();
		return 0;
	}
	char* input = argv[1];
	options opt = getOption(argv[2]);
	char* output = argv[3];
	Image img(input);
	switch (opt) {
		case COLORMASK:
			img.color_mask(0.5, 0.5, 0.5);
			img.write(output);
			break;
		case GRAYSCALE:
			img.grayscale_avg();
			img.write(output);
			break;
		default:
			print_help();
			break;
	}
	return 0;
}

void print_help() { printf("Usage: imgedit input [colormask|grayscale] output\n"); }

options getOption(char* arg) {
	if (strcmp(arg, "colormask") == 0) {
		return COLORMASK;
	}
	else if (strcmp(arg, "grayscale") == 0) {
		return GRAYSCALE;
	}
	return GRAYSCALE;
}
