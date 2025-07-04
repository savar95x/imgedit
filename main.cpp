#include <stdint.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <png++/png.hpp>
#include <thread>
#include <vector>
#include <algorithm>

void grayscale(float alpha, char *input, char *output);
void grayscale_worker(png::image<png::rgb_pixel> &image, float alpha, int start_y, int end_y);
void contrast(float alpha, char *input, char *output);
void brightness(float alpha, char *input, char *output);
void colormask(char *args, char *input, char *output);

float clamp(float low, float a, float high);
void print_help();

float clamp(float low, float a, float high) {
	if (a < low) { a = low; } else if (a > high) { a = high; }
	return a;
}

void print_help() {
	printf("Usage: ./imgedit OPTIONS INPUT OUTPUT\n");
	printf("\n");
	printf("OPTIONS\n");
	printf("--grayscale=alpha	alpha being the extent of grayscale, 0.00 to 1.00\n");
	printf("--contrast=alpha	alpha being the extent of contrast, > 0.00\n");
	printf("--brightness=alpha	alpha being the extent of brightness, > 0.00\n");
	printf("--colormask=r,g,b	r, b, g belonging in 0.00 to 1.00\n");
	printf("\n");
	printf("EXAMPLE USAGE\n");
	printf("./imgedit --grayscale=0.83 input.png output.png\n");
}

void grayscale(float alpha, char *input, char *output) {
	png::image<png::rgb_pixel> image(input);
	const int h = image.get_height();
	unsigned int n = std::thread::hardware_concurrency();
	if (n == 0) n = 4;
	std::vector<std::thread> threads;
	// dividing threads equally on different regions of heights
	int rows_per_thread = h / n;
	for (unsigned int i = 0; i < n; ++i) {
		int start_y = i * rows_per_thread;
		int end_y = (i == n - 1) ? h : start_y + rows_per_thread;
		threads.emplace_back(grayscale_worker, std::ref(image), alpha, start_y, end_y);
	}
	for (auto& thread : threads) {
		thread.join();
	}
	image.write(output);
}
void grayscale_worker(png::image<png::rgb_pixel> &image, float alpha, int start_y, int end_y) {
	alpha = clamp(0, alpha, 1);
	for (int y = start_y; y < end_y; ++y) {
		for (int x = 0; x < image.get_width(); ++x) {
			int avg = (image[y][x].red + image[y][x].green + image[y][x].blue) / 3;
			image[y][x].red = (1-alpha)*image[y][x].red + alpha*avg;
			image[y][x].green = (1-alpha)*image[y][x].green + alpha*avg;
			image[y][x].blue = (1-alpha)*image[y][x].blue + alpha*avg;
		}
	}
}

void contrast(float alpha, char *input, char *output) {
	png::image<png::rgb_pixel> image(input);
	uint8_t mean = 128;
	for (int i = 0; i < image.get_width(); i++) {
		for (int j = 0; j < image.get_height(); j++) {
			image[j][i].red = clamp(0, (alpha)*(image[j][i].red - mean) + mean, 255);
			image[j][i].green = clamp(0, (alpha)*(image[j][i].green - mean) + mean, 255);
			image[j][i].blue = clamp(0, (alpha)*(image[j][i].blue - mean) + mean, 255);
		}
	}
	image.write(output);
}

void brightness(float alpha, char *input, char *output) {
	png::image<png::rgb_pixel> image(input);
	uint8_t mean = 128;
	for (int i = 0; i < image.get_width(); i++) {
		for (int j = 0; j < image.get_height(); j++) {
			image[j][i].red = clamp(0, (alpha)*(image[j][i].red), 255);
			image[j][i].green = clamp(0, (alpha)*(image[j][i].green), 255);
			image[j][i].blue = clamp(0, (alpha)*(image[j][i].blue), 255);
		}
	}
	image.write(output);
}

void colormask(char *args, char *input, char *output) {
	uint8_t i = 0;
	char* mask = strtok(args, ",");
	float masks[3];
	while (mask != NULL) {
		char* endptr;
		masks[i++] = strtof(mask, &endptr);
		mask = strtok(NULL, ",");
	}
	png::image<png::rgb_pixel> image(input);
	uint8_t mean = 128;
	for (int i = 0; i < image.get_width(); i++) {
		for (int j = 0; j < image.get_height(); j++) {
			image[j][i].red = clamp(0, masks[0]*image[j][i].red, 255);
			image[j][i].green = clamp(0, masks[1]*image[j][i].green, 255);
			image[j][i].blue = clamp(0, masks[2]*image[j][i].blue, 255);
		}
	}
	image.write(output);
}

int main(int argc, char **argv) {
	if (argc < 2 || argc > 4) {
		print_help();
		return 0;
	}
	char *option = strtok(argv[1], "=");
	char *args = strtok(NULL, "=");
	if (strcmp(option, "--grayscale") == 0) {
		grayscale(atof(args), argv[2], argv[3]);
	}
	else if (strcmp(option, "--contrast") == 0) {
		contrast(atof(args), argv[2], argv[3]);
	}
	else if (strcmp(option, "--brightness") == 0) {
		brightness(atof(args), argv[2], argv[3]);
	}
	else if (strcmp(option, "--colormask") == 0) {
		colormask(args, argv[2], argv[3]);
	}
	else {
		print_help();
	}
	return 0;
}
