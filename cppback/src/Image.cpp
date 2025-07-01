#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "Image.h"
#include "stb_image.h"
#include "stb_image_write.h"

Image::Image(const char* filename) {
	if (read(filename)) {
		printf("Read %s\n", filename);
		size = w*h*channels;
	} else {
		printf("Failed to read %s\n", filename);
	}
}

// solid blank img
Image::Image(int w, int h, int channels) : w(w), h(h), channels(channels) {
	size = w*h*channels;
	data = new uint8_t[size];
}

Image::Image(const Image& img) : Image(img.w, img.h, img.channels) {
	memcpy(data, img.data, size);
}

Image::~Image() {
	stbi_image_free(data);
}

bool Image::read(const char* filename) {
	data = stbi_load(filename, &w, &h, &channels, 0);
	return data != NULL;
}

bool Image::write(const char* filename) {
	ImageType type = getFileType(filename);
	int success;
	switch (type) {
		case PNG:
			success = stbi_write_png(filename, w, h, channels, data, w*channels);
			break;
		case JPG:
			success = stbi_write_jpg(filename, w, h, channels, data, 100);
			break;
		case BMP:
			success = stbi_write_bmp(filename, w, h, channels, data);
			break;
		case TGA:
			success = stbi_write_tga(filename, w, h, channels, data);
			break;
	}
	return success != 0;
}

ImageType Image::getFileType(const char* filename) {
	const char* ext = strrchr(filename, '.');
	if (ext != nullptr) {
		if (strcmp(ext, ".png") == 0) {
			return PNG;
		}
		else if (strcmp(ext, ".jpg") == 0) {
			return JPG;
		}
		else if (strcmp(ext, ".bmp") == 0) {
			return BMP;
		}
		else if (strcmp(ext, ".tga") == 0) {
			return TGA;
		}
	}
	return PNG;
}

Image& Image::grayscale_avg(float alpha) {
	if (channels < 3) {
		printf("Image is already assumed to be grayscale.\n");
		return *this;
	} 
	if (alpha > 1 || alpha < 0) {
		printf("--grayscale arg should be between 0.00 and 1.00!\n");
		return *this;
	}
	for (int i = 0; i < size; i += channels) {
		int avg = (data[i] + data[i+1] + data[i+2]) / 3;
		memset(data+i+1, (1-alpha)*(*(data+i+1))+(alpha)*(avg), 1);
		memset(data+i+2, (1-alpha)*(*(data+i+2))+(alpha)*(avg), 1);
		memset(data+i+3, (1-alpha)*(*(data+i+3))+(alpha)*(avg), 1);
	}
	return *this;
}

Image& Image::grayscale_lum() {
	// human percieved lumisence
	if (channels < 3) {
		printf("Image is already assumed to be grayscale.\n");
		return *this;
	}
	for (int i = 0; i < size; i += channels) {
		int col = (0.2126*data[i] + 0.7152*data[i+1] + 0.0722*data[i+2]) / 3;
		memset(data+i, col, 3);
	}
	return *this;
}

Image& Image::color_mask(float r, float g, float b) {
	if (channels < 3) {
		printf("The Image has less than 3 channels");
		return *this;
	}
	if (r > 1) r = 1;
	if (g > 1) g = 1;
	if (b > 1) b = 1;
	for (int i = 0; i < size; i += channels) {
		data[i] *= r;
		data[i+1] *= g;
		data[i+2] *= b;
	}
	return *this;
}

Image& Image::brightness(float alpha) {
	if (alpha < 0) {
		printf("--brightness arg should be positive!\n");
		return *this;
	}
	for (int i = 0; i < size; i++) {
		int bright = (alpha)*(*(data+i));
		if (bright > 255) bright = 255;
		memset(data+i, bright, 1);
	}
	return *this;
}

Image& Image::contrast(float alpha) {
	if (alpha < 0) {
		printf("--contrast arg should be positive!\n");
		return *this;
	}
	for (int i = 0; i < size; i++) {
		uint8_t mean = 128;
		int value = (alpha)*(*(data+i) - mean) + mean; // linear interpolation
		if (value > 255) value = 255;
		if (value < 0) value = 0;
		memset(data+i, value, 1);
	}
	return *this;
}
