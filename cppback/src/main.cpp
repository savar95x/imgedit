#include "Image.h"
#include <cstring>
#include <cstdlib>

// functionality i want
// colormask, contrast, grayscale, brightness
// crop, rotate, steganography

enum operation { CONVERT };

void print_help();
operation get_operation(char* arg);

void print_help() {
	printf("Usage: imgedit OPERATION WHATTODO ARGS input output\n");
	printf("\n");
	printf("OPERATIONs - convert (only this for now)\n");
	printf("\n");
	printf("WHATTODO(convert)\n");
	printf("--grayscale x(0 to 1) \n");
	printf("--contrast x(> 0) \n");
	printf("--brightness x(> 0) \n");
	printf("--colormask r,g,b(0 to 1) \n");
	printf("\n");
	printf("Example usage: imgedit convert --colormask 0.12,0.5,0.3 input.jpg output.jpg\n");
}

void convert(char* whattodo, char* args, char* input, char* output) {
	Image img(input);
	if (strcmp(whattodo, "--grayscale") == 0) {
		img.grayscale_avg(atof(args));
	}
	else if (strcmp(whattodo, "--colormask") == 0) {
		float colors[3];
		int i = 0;
		char* color = strtok(args, ",");
		while (color != NULL) {
			char* endptr;
			colors[i++] = strtof(color, &endptr);
			color = strtok(NULL, ",");
		}
		img.color_mask(colors[0], colors[1], colors[2]);
	}
	else if (strcmp(whattodo, "--brightness") == 0) {
		img.brightness(atof(args));
	}
	else if (strcmp(whattodo, "--contrast") == 0) {
		img.contrast(atof(args));
	}
	img.write(output);
}

operation get_operation(char* arg) {
	if (strcmp(arg, "convert") == 0) {
		return CONVERT;
	}
	return CONVERT;
}

int main(int argc, char** argv) {
	if (strcmp(argv[1], "help") == 0 || strcmp(argv[1], "--help") == 0 || argc != 6) {
		print_help();
		return 1;
	}
	operation operation = get_operation(argv[1]);
	switch (operation) {
		case CONVERT:
			convert(argv[2], argv[3], argv[4], argv[5]);
			break;
		default:
			print_help();
			break;
	}
	return 0;
}
