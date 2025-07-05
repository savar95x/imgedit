#include <stdint.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <png++/png.hpp>
#include <thread>
#include <vector>
#include <algorithm>

bool check_option(char *option);
float clamp(float low, float a, float high);
void print_help();
void grayscale_worker(png::image<png::rgb_pixel> &image, float alpha, int start_row, int end_row);
void contrast_worker(png::image<png::rgb_pixel> &image, float alpha, int start_row, int end_row);
void brightness_worker(png::image<png::rgb_pixel> &image, float alpha, int start_row, int end_row);
void colormask_worker(png::image<png::rgb_pixel> &image, float *masks, int start_row, int end_row);
void execute_option(char *option, char *args, char *input, char *output);
void crop(char *args, char *input, char *output);

bool check_option(char *option) {
	if (strcmp(option, "--grayscale") == 0 || strcmp(option, "--contrast") == 0 || strcmp(option, "--brightness") == 0 || strcmp(option, "--colormask") == 0)
		return true;
	return false;
}

float clamp(float low, float a, float high) {
	if (a < low) { a = low; } else if (a > high) { a = high; }
	return a;
}

void print_help() {
	printf("Usage: ./imgedit OPTIONS=ARGs INPUT OUTPUT\n");
	printf("\n");
	printf("OPTIONS\n");
	printf("--grayscale=x           x being the extent of grayscale, 0.00 to 1.00\n");
	printf("--contrast=x            x being the extent of contrast, > 0.00\n");
	printf("--brightness=x          x being the extent of brightness, > 0.00\n");
	printf("--colormask=r,g,b       r, b, g belonging in 0.00 to 1.00\n");
	printf("--crop=x,y:x,y          top_left and bottom_right points\n");
	printf("\n");
	printf("EXAMPLE USAGE\n");
	printf("./imgedit --grayscale=0.83 input.png output.png\n");
}

void grayscale_worker(png::image<png::rgb_pixel> &image, float alpha, int start_row, int end_row) {
	alpha = clamp(0, alpha, 1);
	for (int y = start_row; y < end_row; y++) {
		for (int x = 0; x < image.get_width(); x++) {
			uint8_t avg = (image[y][x].red + image[y][x].green + image[y][x].blue ) / 3;
			image[y][x].red = (1-alpha)*image[y][x].red + alpha*avg;
			image[y][x].green = (1-alpha)*image[y][x].green + alpha*avg;
			image[y][x].blue = (1-alpha)*image[y][x].blue + alpha*avg;
		}
	}
}

void contrast_worker(png::image<png::rgb_pixel> &image, float alpha, int start_row, int end_row) {
	const uint8_t mean = 128;
	for (int y = start_row; y < end_row; y++) {
		for (int x = 0; x < image.get_width(); x++) {
			image[y][x].red = clamp(0, (alpha)*(image[y][x].red - mean) + mean, 255);
			image[y][x].green = clamp(0, (alpha)*(image[y][x].green - mean) + mean, 255);
			image[y][x].blue = clamp(0, (alpha)*(image[y][x].blue - mean) + mean, 255);
		}
	}
}

void brightness_worker(png::image<png::rgb_pixel> &image, float alpha, int start_row, int end_row) {
	for (int y = start_row; y < end_row; y++) {
		for (int x = 0; x < image.get_width(); x++) {
			image[y][x].red = clamp(0, (alpha)*(image[y][x].red), 255);
			image[y][x].green = clamp(0, (alpha)*(image[y][x].green), 255);
			image[y][x].blue = clamp(0, (alpha)*(image[y][x].blue), 255);
		}
	}
}

void colormask_worker(png::image<png::rgb_pixel> &image, float *masks, int start_row, int end_row) {
	for (int y = start_row; y < end_row; y++) {
		for (int x = 0; x < image.get_width(); x++) {
			image[y][x].red = clamp(0, (masks[0])*(image[y][x].red), 255);
			image[y][x].green = clamp(0, (masks[1])*(image[y][x].green), 255);
			image[y][x].blue = clamp(0, (masks[2])*(image[y][x].blue), 255);
		}
	}
}

void execute_option(char *option, char *args, char *input, char *output) {
	png::image<png::rgb_pixel> image(input);
	unsigned int n = std::thread::hardware_concurrency();
	if (n == 0) n = 4; // n is the number of threads
	unsigned int h = image.get_height();
	int rpt = h / n; // rows per thread
	std::vector<std::thread> threads;

	if (strcmp(option, "--grayscale") == 0) {
		for (int i = 0; i < n; i++)
			threads.push_back(std::thread(&grayscale_worker, std::ref(image), atof(args), i*rpt, (i == n-1) ? h : (i+1)*rpt));
	}
	else if (strcmp(option, "--contrast") == 0) {
		for (int i = 0; i < n; i++)
			threads.push_back(std::thread(contrast_worker, std::ref(image), atof(args), i*rpt, (i == n-1) ? h : (i+1)*rpt));
	}
	else if (strcmp(option, "--brightness") == 0) {
		for (int i = 0; i < n; i++)
			threads.push_back(std::thread(brightness_worker, std::ref(image), atof(args), i*rpt, (i == n-1) ? h : (i+1)*rpt));
	}
	else if (strcmp(option, "--colormask") == 0) {
		for (int i = 0; i < n; i++) {
			// for some weird reason, parsing args for each thread is faster than parsing once for every thread
			// :(
			uint8_t j = 0;
			char *mask = strtok(args, ",");
			float *masks = new float[3];
			while (mask != NULL) {
				char* endptr; masks[j++] = strtof(mask, &endptr);
				mask = strtok(NULL, ",");
			}
			threads.push_back(std::thread(colormask_worker, std::ref(image), masks, i*rpt, (i == n-1) ? h : (i+1)*rpt));
		}
	}
	for (int i = 0; i < n; i++) threads[i].join();
	image.write(output);
}

void crop(char *args, char *input, char *output) {
	uint8_t i = 0;
	char *token = strtok(args, "x:");
	long positions[4];
	while (token != NULL) {
		char* endptr;
		positions[i++] = strtol(token, &endptr, 10);
		token = strtok(NULL, "x:");
	}
	png::image<png::rgb_pixel> image(input);
	int start_y = positions[0];
	int start_x = positions[1];
	int width = positions[2] - positions[0];
	int height = positions[3] - positions[1];
	png::image<png::rgb_pixel> cropped(width, height);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			cropped[y][x] = image[start_y + y][start_x + x];
		}
	}
	cropped.write(output);
}

int main(int argc, char **argv) {
	if (argc != 4) {
		print_help();
		return 0;
	}
	char *option = strtok(argv[1], "=");
	char *args = strtok(NULL, "=");
	if (check_option(option)) { // grayscale, contrast, brightness. colormask
		execute_option(option, args, argv[2], argv[3]);
	}
	else if (strcmp(option, "--crop") == 0) {
		crop(args, argv[2], argv[3]);
	}
	else {
		print_help();
	}
	return 0;
}
