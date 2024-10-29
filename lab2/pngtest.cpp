// pngtest.cpp : Defines the entry point for the console application.
//

#include "PngProc.h"
#include <string.h>
#include <stdio.h>
#include <vector>
#include <ctime>
#include <iostream>

using namespace std;

// finding the nearest color 
RGB NearestColor(RGB& pixelColor, int nBPP);

// processing image
vector<vector<RGB>> processImage(vector<vector<RGB>> image, size_t width, size_t height, int nBPP);

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
	char	szInputFileName[256];
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
			strcat(szOutputFileName, "_1_out.png");
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

		auto dithering_image = processImage(image, nWidth, nHeight, 1);

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

RGB NearestColor(RGB& pixelColor, int nBPP) {

	pixelColor.r = pixelColor.r >> (8 - nBPP) << (8 - nBPP);
	pixelColor.g = pixelColor.g >> (8 - nBPP) << (8 - nBPP);
	pixelColor.b = pixelColor.b >> (8 - nBPP) << (8 - nBPP);

	return pixelColor;
}

vector<vector<RGB>> processImage(vector<vector<RGB>> image, size_t width, size_t height, int nBPP) {

	RGB newColor, error;

	for (size_t y = 0; y < height - 1; ++y) {
		for (size_t x = 1; x < width - 1; ++x) {
			newColor = NearestColor(image[y][x], nBPP);

			error.r = image[y][x].r - newColor.r;
			error.g = image[y][x].g - newColor.g;
			error.b = image[y][x].b - newColor.b;

			image[y][x].r = min(255, max(0, image[y][x].r - error.r));
			image[y][x].g = min(255, max(0, image[y][x].g - error.g));
			image[y][x].b = min(255, max(0, image[y][x].b - error.b));
			

			image[y][x + 1].r = min(255 ,max(0, image[y][x + 1].r + error.r * 7 / 16));
			image[y][x + 1].g = min(255, max(0, image[y][x + 1].g + error.g * 7 / 16));
			image[y][x + 1].b = min(255, max(0, image[y][x + 1].b + error.b * 7 / 16));

			image[y + 1][x + 1].r = min(255, max(0, image[y + 1][x + 1].r + error.r / 16));
			image[y + 1][x + 1].g = min(255, max(0, image[y + 1][x + 1].g + error.g / 16));
			image[y + 1][x + 1].b = min(255, max(0, image[y + 1][x + 1].b + error.b / 16));

			image[y + 1][x].r = min(255, max(0, image[y + 1][x].r + error.r * 5 / 16));
			image[y + 1][x].g = min(255, max(0, image[y + 1][x].g + error.g * 5 / 16));
			image[y + 1][x].b = min(255, max(0, image[y + 1][x].b + error.b * 5 / 16));

			image[y + 1][x - 1].r = min(255, max(0, image[y + 1][x - 1].r + error.r * 3 / 16));
			image[y + 1][x - 1].g = min(255, max(0, image[y + 1][x - 1].g + error.g * 3 / 16));
			image[y + 1][x - 1].b = min(255, max(0, image[y + 1][x - 1].b + error.b * 3 / 16));

		}
	}

	for (size_t x = 0; x < width; x++) {
		image[height - 1][x] = NearestColor(image[height - 1][x], nBPP);
	}

	for (size_t y = 0; y < height - 1; y++) {
		image[y][0] = NearestColor(image[y][0], nBPP);
		image[y][width-1] = NearestColor(image[y][width-1], nBPP);
	}

	return image;
}