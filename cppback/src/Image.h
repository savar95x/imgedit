#include <stdint.h>
#include <cstdio>

enum ImageType { PNG, JPG, BMP, TGA };

struct Image {
	uint8_t* data = NULL;
	size_t size = 0;
	int w, h, channels;

	Image(const char* filename); // reading
	Image(int w, int h, int channels); // solid blank image
	Image(const Image& img); // copy image
	~Image();

	bool read(const char* filename);
	bool write(const char* filename);

	ImageType getFileType(const char* filename);

	Image& grayscale_avg(float alpha);
	Image& brightness(float alpha);
	Image& contrast(float alpha); // we use linear interpolation
	Image& grayscale_lum();
	Image& color_mask(float r, float g, float b);
};
