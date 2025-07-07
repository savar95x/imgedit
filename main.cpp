#include <stdint.h>
#include <fstream>
#include <sstream>  // Required for std::istringstream
//#include <inttypes.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <png++/png.hpp>
#include <thread>
#include <vector>
#include <algorithm>

#define DELIM "x,"

bool check_option(char *option) {
	if (strcmp(option, "--grayscale") == 0 || strcmp(option, "--contrast") == 0 || strcmp(option, "--brightness") == 0 || strcmp(option, "--colormask") == 0 || strcmp(option, "--sepia") == 0 || strcmp(option, "--saturate") == 0)
		return true;
	return false;
}

uint8_t clamp(uint8_t low, uint8_t a, uint8_t high) {
	if (a < low) { a = low; } else if (a > high) { a = high; }
	return a;
}

// grayscale, saturate, contrast, brightness, sepia, colormask, crop
void print_help() {
	printf("Usage: ./imgedit OPTIONS[=ARGs] INPUT [OUTPUT]\n");
	printf("\n");
	printf("OPTIONS\n");
	printf("--info                  output width x height\n");
	printf("\n");
	printf("--grayscale=x           mathematical average of RGB. x being the extent, in 0.00 to 1.00\n");
	printf("--saturate=x            x > 0.00. x=0 -> grayscale (luma average), x=1 -> original image, x>1 -> oversaturate\n");
	printf("--contrast=x            x > 0.00. x=0 -> solid gray image, x=1 -> original image, x>1 -> contrast\n");
	printf("--brightness=x          x > 0.00. x=0 -> solid black image, x=1 -> original image, x>1 -> brighten\n");
	printf("--sepia=x               x in 0.00 to 1.00\n");
	printf("--colormask=r,g,b       r, b, g in 0.00 to 1.00\n");
	printf("\n");
	printf("--crop=YxX,YxX          top_left and bottom_right points\n");
	printf("--rotate=deg            degrees to rotate\n");
	printf("\n");
	printf("EXAMPLE USAGE\n");
	printf("./imgedit --grayscale=0.83 input.png output.png\n");
	printf("./imgedit --crop=100x100,1920x1200 input.png output.png\n");
	printf("./imgedit --info input.png\n");
}

void grayscale_worker(png::image<png::rgb_pixel> &image, float alpha, int start_row, int end_row) {
	alpha = (float)clamp(0, 100*alpha, 100)/100;
	for (int y = start_row; y < end_row; y++) {
		for (int x = 0; x < image.get_width(); x++) {
			// mathematical average
			uint8_t avg = (image[y][x].red + image[y][x].green + image[y][x].blue ) / 3;
			image[y][x].red = (1-alpha)*image[y][x].red + alpha*avg;
			image[y][x].green = (1-alpha)*image[y][x].green + alpha*avg;
			image[y][x].blue = (1-alpha)*image[y][x].blue + alpha*avg;
		}
	}
}

void saturate_worker(png::image<png::rgb_pixel> &image, float alpha, int start_row, int end_row) {
	for (int y = start_row; y < end_row; y++) {
		for (int x = 0; x < image.get_width(); x++) {
			// mathematical average
			uint8_t gray = 0.299*image[y][x].red + 0.587*image[y][x].green + 0.114*image[y][x].blue;
			image[y][x].red = clamp(0, gray + alpha*(image[y][x].red - gray), 255);
			image[y][x].green = clamp(0, gray + alpha*(image[y][x].green - gray), 255);
			image[y][x].blue = clamp(0, gray + alpha*(image[y][x].blue - gray), 255);
		}
	}
}

void contrast_worker(png::image<png::rgb_pixel> &image, float alpha, int start_row, int end_row) {
	const uint8_t mean = 128;
	for (int y = start_row; y < end_row; y++) {
		for (int x = 0; x < image.get_width(); x++) {
			// linear interpolation
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

void sepia_worker(png::image<png::rgb_pixel> &image, float alpha, int start_row, int end_row) {
	for (int y = start_row; y < end_row; y++) {
		for (int x = 0; x < image.get_width(); x++) {
			// just so that it is more readable
			uint8_t red = image[y][x].red;
			uint8_t green = image[y][x].green;
			uint8_t blue = image[y][x].blue;
			image[y][x].red = clamp(0, (1-alpha)*red + alpha*(0.393*red + 0.769*green + 0.189*blue), 255);
			image[y][x].green = clamp(0, (1-alpha)*green + alpha*(0.349*red + 0.686*green + 0.168*blue), 255);
			image[y][x].blue = clamp(0, (1-alpha)*blue + alpha*(0.272*red + 0.534*green + 0.131*blue), 255);
		}
	}
}

void execute_option(char *option, char *args, char *input, char *output) {
	// file locking for no overlapping while reading the same image multiple times in a benchmark
	std::vector<unsigned char> buffer;
	{
		std::ifstream file(input, std::ios::binary);
		file.seekg(0, std::ios::end);
		buffer.resize(file.tellg());
		file.seekg(0, std::ios::beg);
		file.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
	}

	std::istringstream iss(std::string(buffer.begin(), buffer.end()));
	png::image<png::rgb_pixel> image;
	image.read_stream(iss);  // Now safe for multiple processes

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
			threads.push_back(std::thread(&contrast_worker, std::ref(image), atof(args), i*rpt, (i == n-1) ? h : (i+1)*rpt));
	}
	else if (strcmp(option, "--brightness") == 0) {
		for (int i = 0; i < n; i++)
			threads.push_back(std::thread(&brightness_worker, std::ref(image), atof(args), i*rpt, (i == n-1) ? h : (i+1)*rpt));
	}
	else if (strcmp(option, "--colormask") == 0) {
		uint8_t j = 0;
		char *saveptr;
		char *token = strtok_r(args, ",", &saveptr);
		float masks[3];
		while (token != NULL) {
			masks[j++] = atof(token);
			token = strtok_r(NULL, ",", &saveptr);
		}
		for (int i = 0; i < n; i++) {
			threads.push_back(std::thread(&colormask_worker, std::ref(image), masks, i*rpt, (i == n-1) ? h : (i+1)*rpt));
		}
	}
	else if (strcmp(option, "--sepia") == 0) {
		for (int i = 0; i < n; i++)
			threads.push_back(std::thread(&sepia_worker, std::ref(image), atof(args), i*rpt, (i == n-1) ? h : (i+1)*rpt));
	}
	else if (strcmp(option, "--saturate") == 0) {
		for (int i = 0; i < n; i++)
			threads.push_back(std::thread(&saturate_worker, std::ref(image), atof(args), i*rpt, (i == n-1) ? h : (i+1)*rpt));
	}
	for (int i = 0; i < n; i++) threads[i].join();
	image.write(output);
}

void crop(char *args, char *input, char *output) {
	// --crop=100x100,1920x1080
	uint8_t i = 0;
	char *token = strtok(args, DELIM);
	long positions[4];
	while (token != NULL) {
		char* endptr;
		positions[i++] = strtol(token, &endptr, 10);
		token = strtok(NULL, DELIM);
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

void rotate(char *args, char *input, char *output) {
	// --rotate=100x100,60
	uint16_t deg = atoi(args);
	png::image<png::rgb_pixel> image(input);
	int width = image.get_width();
	int height = image.get_height();
}

void info(char *input) {
	png::image<png::rgb_pixel> image(input);
	printf("%dx%d\n", image.get_width(), image.get_height());
}

int main(int argc, char **argv) {
	if (argc < 3 || argc > 4) {
		print_help();
		return 0;
	}
	char *option = strtok(argv[1], "=");
	char *args = strtok(NULL, "=");
	if (check_option(option)) {
		execute_option(option, args, argv[2], argv[3]);
	} else if (strcmp(option, "--info") == 0) {
		info(argv[2]);
	} else if (strcmp(option, "--crop") == 0) {
		crop(args, argv[2], argv[3]);
	//} else if (strcmp(option, "--rotate") == 0) {
		//rotate(args, argv[2], argv[3]);
	} else {
		print_help();
	}
	return 0;
}
