// pngtest.cpp : Defines the entry point for the console application.
//

#include "PngProc.h"
#include <string.h>
#include <stdio.h>
#include <vector>

using namespace std;

// creating a palette with a specified number of bytes per channel
vector<RGB> CreatePalette(int bpp);

// finding the nearest color in the palette
RGB NearestColor(const RGB& pixelColor, const vector<RGB>& palette);

// processing image
vector<vector<RGB>> processImage(vector<vector<RGB>> image, size_t width, size_t height, int levelsPerChannel);

// creating a vector for convenient storage
vector<vector<RGB>> get_rgb_vector(unsigned char* buffer, size_t width, size_t height);

int main(int argc, char* argv[]) {

    class CBitsPtrGuard {
    public:
        CBitsPtrGuard(unsigned char** pB)
            : m_ppBits(pB) {}
        ~CBitsPtrGuard() {
            if (*m_ppBits) delete* m_ppBits;
        }
    private:
        unsigned char** m_ppBits;
    };

    // parse input parameters
    char  szInputFileName[256];
    char    szOutputFileName[256];
    if (argc < 2)
        printf("\nformat: pngtest <input_file> [<output_file>]");
    else {
        strcpy(szInputFileName, argv[1]);
        if (argc > 2) {
            strcpy(szOutputFileName, argv[2]);
        }
        else {
            strcpy(szOutputFileName, szInputFileName);
            strcat(szOutputFileName, "_2_out.png");
        }
    }


    size_t nReqSize = NPngProc::readPngFile(szInputFileName, 0, 0, 0, 0);
    if (nReqSize == NPngProc::PNG_ERROR)
    {
        printf("\nError ocured while pngfile was read");
        return -1;
    }


    unsigned char* pInputBits = new unsigned char[nReqSize];
    if (!pInputBits)
    {
        printf("\nCan't allocate memory for image, required size is %u", nReqSize);
        return -1;
    }
    CBitsPtrGuard InputBitsPtrGuard(&pInputBits);


    size_t nWidth, nHeight;
    unsigned int nBPP;

    size_t nRetSize = NPngProc::readPngFile(szInputFileName, pInputBits, &nWidth, &nHeight, &nBPP);

    vector<vector<RGB>> image = get_rgb_vector(pInputBits, nWidth, nHeight);

    auto dithering_image = processImage(image, nWidth, nHeight, 2);

    if (NPngProc::write_png_file(szOutputFileName, dithering_image, nWidth, nHeight) == NPngProc::PNG_ERROR) {
        perror("\nError ocuured during png file was written\n");
        return -1;
    }

    return 0;
}

vector<vector<RGB>> get_rgb_vector(unsigned char* buffer, size_t width, size_t height) {
    vector<vector<RGB>> image(height, vector<RGB>(width));

    // filling a two - dimensional vector with pixels from the buffer
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            // getting the index of the pixel in the buffer(each pixel consists of 3 bytes: R, G, B)
            size_t index = (y * width + x) * 3;

            // filling the RGB structure for the current pixel
            image[y][x].r = buffer[index];
            image[y][x].g = buffer[index + 1];
            image[y][x].b = buffer[index + 2];
        }
    }
   
    return image;
}

vector<RGB> CreatePalette(int bpp) {
    vector<RGB> palette;
    int colors = pow(2, bpp);
    int step = 255 / (colors - 1);

    for (int r = 0; r < 256; r += step) {
        for (int g = 0; g < 256; g += step) {
            for (int b = 0; b < 256; b += step) {
                palette.push_back({ static_cast<unsigned char>(r),
                           static_cast<unsigned char>(g),
                           static_cast<unsigned char>(b) });
            }
        }
    }
    return palette;
}

RGB NearestColor(const RGB& pixelColor, const vector<RGB>& palette) {
    RGB nearestColor = palette[0];
    int minDistance = INT_MAX;

    for (const RGB& color : palette) {
        int distance =
            (static_cast<int>(pixelColor.r) - static_cast<int>(color.r)) * (static_cast<int>(pixelColor.r) - static_cast<int>(color.r)) +
            (static_cast<int>(pixelColor.g) - static_cast<int>(color.g)) * (static_cast<int>(pixelColor.g) - static_cast<int>(color.g)) +
            (static_cast<int>(pixelColor.b) - static_cast<int>(color.b)) * (static_cast<int>(pixelColor.b) - static_cast<int>(color.b));

        if (distance < minDistance) {
            minDistance = distance;
            nearestColor = color;
        }
    }

    return nearestColor;
}

int clamp(int value) {
    return std::min(255, std::max(0, value));
}

vector<vector<RGB>> processImage(vector<vector<RGB>> image, size_t width, size_t height, int levelsPerChannel) {
    vector<RGB> palette = CreatePalette(levelsPerChannel);
    RGB newColor;
    int error_r, error_g, error_b;

    vector<vector<int>> new_image_r(height, vector<int>(width));
    vector<vector<int>> new_image_g(height, vector<int>(width));
    vector<vector<int>> new_image_b(height, vector<int>(width));

    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            new_image_r[y][x] = static_cast<int>(image[y][x].r);
            new_image_g[y][x] = static_cast<int>(image[y][x].g);
            new_image_b[y][x] = static_cast<int>(image[y][x].b);
        }
    }

    for (size_t y = 0; y < height - 1; ++y) {
        for (size_t x = 1; x < width - 1; ++x) {
            
            // finding the nearest color
            RGB currentColor = {
                static_cast<unsigned char>(clamp(new_image_r[y][x])),
                static_cast<unsigned char>(clamp(new_image_g[y][x])),
                static_cast<unsigned char>(clamp(new_image_b[y][x]))
            };

            newColor = NearestColor(currentColor, palette);

            new_image_r[y][x] = newColor.r;
            new_image_g[y][x] = newColor.g;
            new_image_b[y][x] = newColor.b;

            // error calculation
            error_r = static_cast<int>(currentColor.r) - static_cast<int>(newColor.r);
            error_g = static_cast<int>(currentColor.g) - static_cast<int>(newColor.g);
            error_b = static_cast<int>(currentColor.b) - static_cast<int>(newColor.b);

            // Floyd-Steinberg dithering
            new_image_r[y][x + 1] = new_image_r[y][x + 1] + static_cast<int>(error_r * 7.0 / 16);
            new_image_g[y][x + 1] = new_image_g[y][x + 1] + static_cast<int>(error_g * 7.0 / 16);
            new_image_b[y][x + 1] = new_image_b[y][x + 1] + static_cast<int>(error_b * 7.0 / 16);

            new_image_r[y + 1][x + 1] = new_image_r[y + 1][x + 1] + static_cast<int>(error_r * 1.0 / 16);
            new_image_g[y + 1][x + 1] = new_image_g[y + 1][x + 1] + static_cast<int>(error_g * 1.0 / 16);
            new_image_b[y + 1][x + 1] = new_image_b[y + 1][x + 1] + static_cast<int>(error_b * 1.0 / 16);

            new_image_r[y + 1][x] = new_image_r[y + 1][x] + static_cast<int>(error_r * 5.0 / 16);
            new_image_g[y + 1][x] = new_image_g[y + 1][x] + static_cast<int>(error_g * 5.0 / 16);
            new_image_b[y + 1][x] = new_image_b[y + 1][x] + static_cast<int>(error_b * 5.0 / 16);

            new_image_r[y + 1][x - 1] = new_image_r[y + 1][x - 1] + static_cast<int>(error_r * 3.0 / 16);
            new_image_g[y + 1][x - 1] = new_image_g[y + 1][x - 1] + static_cast<int>(error_g * 3.0 / 16);
            new_image_b[y + 1][x - 1] = new_image_b[y + 1][x - 1] + static_cast<int>(error_b * 3.0 / 16);
        }
    }

    // frame
    for (size_t y = 0; y < height; y++) {
        RGB currentColor = {
                static_cast<unsigned char>(clamp(new_image_r[y][0])),
                static_cast<unsigned char>(clamp(new_image_g[y][0])),
                static_cast<unsigned char>(clamp(new_image_b[y][0]))
        };

        newColor = NearestColor(currentColor, palette);

        new_image_r[y][0] = newColor.r;
        new_image_g[y][0] = newColor.g;
        new_image_b[y][0] = newColor.b;

        currentColor = {
                static_cast<unsigned char>(clamp(new_image_r[y][width - 1])),
                static_cast<unsigned char>(clamp(new_image_g[y][width - 1])),
                static_cast<unsigned char>(clamp(new_image_b[y][width - 1]))
        };

        newColor = NearestColor(currentColor, palette);

        new_image_r[y][width - 1] = newColor.r;
        new_image_g[y][width - 1] = newColor.g;
        new_image_b[y][width - 1] = newColor.b;
    }

    for (size_t x = 1; x < width - 1; x++) {
        RGB currentColor = {
                static_cast<unsigned char>(clamp(new_image_r[height - 1][x])),
                static_cast<unsigned char>(clamp(new_image_g[height - 1][x])),
                static_cast<unsigned char>(clamp(new_image_b[height - 1][x]))
        };

        newColor = NearestColor(currentColor, palette);

        new_image_r[height - 1][x] = newColor.r;
        new_image_g[height - 1][x] = newColor.g;
        new_image_b[height - 1][x] = newColor.b;
    }

    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            image[y][x].r = static_cast<unsigned char>(clamp(new_image_r[y][x]));
            image[y][x].g = static_cast<unsigned char>(clamp(new_image_g[y][x]));
            image[y][x].b = static_cast<unsigned char>(clamp(new_image_b[y][x]));
        }
    }

    return image;
}