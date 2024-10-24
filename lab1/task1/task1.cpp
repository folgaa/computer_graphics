// pngtest.cpp : Defines the entry point for the console application.
//

#include "PngProc.h"
#include <string.h>
#include <stdio.h>
#include <iostream>

void ImageRoundGrayscale(unsigned char* pOut
	, unsigned char* pIn
	, size_t nWidth
	, size_t nHeight
	, unsigned char color);

int main(int argc, char* argv[])
{
	class CBitsPtrGuard
	{
	public:
		CBitsPtrGuard(unsigned char** pB) : m_ppBits(pB) { }
		~CBitsPtrGuard() { if (*m_ppBits) delete *m_ppBits, *m_ppBits = 0; }
	protected:
		unsigned char** m_ppBits;
	};

	// parse input parameters
	char	szInputFileName[256];
	char    szOutputFileName[256];
	if (argc < 2)
		printf("\nformat: pngtest <input_file> [<output_file>]");
	else 
	{
		strcpy(szInputFileName, argv[1]);
		if (argc > 2)
			strcpy(szOutputFileName, argv[2]);
		else
		{
			strcpy(szOutputFileName, szInputFileName);
			strcat(szOutputFileName, "_out.png");
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


	unsigned char* pOutputBits = new unsigned char[nReqSize];
	if (!pOutputBits)
	{
		printf("\nCan't allocate memory for image, required size is %u", nReqSize);
		return -1;
	}


	CBitsPtrGuard OutputBitsPtrGuard(&pOutputBits);

	size_t nWidth, nHeight;
	unsigned int nBPP;

	size_t nRetSize = NPngProc::readPngFileGray(szInputFileName, pInputBits, &nWidth, &nHeight);
	nBPP = 8;

	ImageRoundGrayscale(pOutputBits, pInputBits, nWidth, nHeight, 50);

	if (NPngProc::writePngFile(szOutputFileName, pOutputBits, nWidth, nHeight, nBPP) == NPngProc::PNG_ERROR)
	{
		printf("\nError ocuured during png file was written");
		return -1;
	}

	return 0;
}

void ImageRoundGrayscale(unsigned char* pOut
			 , unsigned char* pIn
			 , size_t nWidth
			 , size_t nHeight
			 , unsigned char color)
{
	// ASSERT(pOut != NULL && pIn != NULL && nWidth > 0 && nHeight > 0)
	if (!pOut || !pIn || nWidth == 0 || nHeight == 0)
	{
		printf("\nError: invalid input parameters in ImageRoundGrayscale (pOut: %p, pIn: %p, nWidth: %u, nHeight: %u)", (void*)pOut, (void*)pIn, nWidth, nHeight);
		return;
	}

	//find the minimum side of the image

	size_t minSide = 0;
	nWidth < nHeight ? minSide = nWidth : minSide = nHeight;

	//set the minimum distance from the circle to the frame (radius)

	size_t radius, radius2;
	radius = minSide * 95 / 200;
	radius2 = radius * radius;

	//find the center

	int centerX = nWidth / 2;
	int centerY = nHeight / 2;

	for (size_t y = 0; y < nHeight; ++y)
	{
		for (size_t x = 0; x < nWidth; ++x)
		{
			if ((x - centerX) * (x - centerX) + (y - centerY) * (y - centerY) > radius2)
			{
				*pIn++;
				*pOut++ = color;
			}
			else
				*pOut++ = *pIn++;
		}
	}
	return;
}
