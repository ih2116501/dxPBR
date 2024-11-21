#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "Image.h"
#include "stb_image.h"
#include "stb_image_write.h"

unsigned char *Image::Load(const std::string &filename, int &width, int &height,
                           int &channels) {
    return stbi_load(filename.c_str(), &width, &height, &channels, 0);
}

void Image::FreeImage(unsigned char *image) { stbi_image_free(image); }

