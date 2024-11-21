#pragma once
#include <string>

namespace Image {
unsigned char *Load(const std::string &filename, int &width, int &height,
                    int &channels);
void FreeImage(unsigned char *image);
}

