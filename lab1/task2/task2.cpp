// pngtest.cpp : Defines the entry point for the console application.
//

#include "PngProc.h"
#include <string.h>
#include <stdio.h>
#include <iostream>

void Blending(unsigned char* pOut
			 , unsigned char* pIn1
			 , unsigned char* pIn2
			 , unsigned char* mask
			 , size_t nWidth
			 , size_t nHeight);


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
	char	szInputFileName1[256];
	char	szInputFileName2[256];
	char	szInputFileName3[256];
	char    szOutputFileName[256];

	if (argc < 4)
		printf("\nformat: pngtest <input_file1> <input_file2> <input_file3> [<output_file>]");
	else 
	{
		strcpy(szInputFileName1, argv[1]);
		strcpy(szInputFileName2, argv[2]);
		strcpy(szInputFileName3, argv[3]);
		if (argc > 4)
			strcpy(szOutputFileName, argv[4]);
		else
		{
			strcpy(szOutputFileName, szInputFileName1);
			strcat(szOutputFileName, "_out.png");
		}
	}

	size_t nReqSize1 = NPngProc::readPngFile(szInputFileName1, 0, 0, 0, 0);
	size_t nReqSize2 = NPngProc::readPngFile(szInputFileName2, 0, 0, 0, 0);
	size_t nReqSize3 = NPngProc::readPngFile(szInputFileName3, 0, 0, 0, 0);

	if (nReqSize1 == NPngProc::PNG_ERROR || nReqSize2 == NPngProc::PNG_ERROR || nReqSize3 == NPngProc::PNG_ERROR)
	{
		printf("\nError ocured while pngfile was read");
		return -1;
	}


	size_t nReqSize = nReqSize1;
	
	unsigned char* pInputBits1 = new unsigned char[nReqSize];
	unsigned char* pInputBits2 = new unsigned char[nReqSize];
	unsigned char* maskBits = new unsigned char[nReqSize];
	if (!pInputBits1)
	{
		printf("\nCan't allocate memory for image, required size is %u", nReqSize);
		return -1;
	}
	CBitsPtrGuard InputBitsPtrGuard1(&pInputBits1);
	if (!pInputBits2)
	{
		printf("\nCan't allocate memory for image, required size is %u", nReqSize);
		return -1;
	}
	CBitsPtrGuard InputBitsPtrGuard2(&pInputBits2);
	if (!maskBits)
	{
		printf("\nCan't allocate memory for image, required size is %u", nReqSize);
		return -1;
	}
	CBitsPtrGuard InputBitsPtrGuard3(&maskBits);


	unsigned char* pOutputBits = new unsigned char[nReqSize];
	if (!pOutputBits)
	{
		printf("\nCan't allocate memory for image, required size is %u", nReqSize);
		return -1;
	}
	CBitsPtrGuard OutputBitsPtrGuard(&pOutputBits);

	size_t nWidth1, nHeight1, nWidth2, nHeight2, nWidth3, nHeight3, nWidth, nHeight;
	unsigned int nBPP;

	size_t nRetSize1 = NPngProc::readPngFileGray(szInputFileName1, pInputBits1, &nWidth1, &nHeight1);
	size_t nRetSize2 = NPngProc::readPngFileGray(szInputFileName2, pInputBits2, &nWidth2, &nHeight2);
	size_t nRetSize3 = NPngProc::readPngFileGray(szInputFileName3, maskBits, &nWidth3, &nHeight3);
	nBPP = 8;
 
	if (nWidth1 != nWidth2 || nHeight1 != nHeight2 || nWidth2 != nWidth3 || nHeight2 != nHeight3) {
		printf("\nImage sizes don't match");
		return -1;
	}

	nWidth = nWidth1;
	nHeight = nHeight1;

	Blending(pOutputBits, pInputBits1, pInputBits2, maskBits, nWidth, nHeight);

	if (NPngProc::writePngFile(szOutputFileName, pOutputBits, nWidth, nHeight, nBPP) == NPngProc::PNG_ERROR)
	{
		printf("\nError ocuured during png file was written");
		return -1;
	}

	return 0;
}

void Blending(unsigned char* pOut
		, unsigned char* pIn1
		, unsigned char* pIn2
		, unsigned char* mask
		, size_t nWidth
		, size_t nHeight)
{
	// ASSERT(pOut != NULL && pIn != NULL && nWidth > 0 && nHeight > 0)
	if (!pOut || !pIn1 || !pIn2 || !mask || nWidth == 0 || nHeight == 0)
	{
		printf("\nError: invalid input parameters in ""Blending");
		return;
	}

	unsigned char C_b = 0, C_s = 0, alpha = 0;

	for (size_t y = 0; y < nHeight; ++y)
	{
		for (size_t x = 0; x < nWidth; ++x)
		{
			C_b = *pIn1++;
			C_s = *pIn2++;
			alpha = *mask++;
			*pOut++ = (unsigned char) ((1 - alpha / 255.0) * C_b + (alpha / 255.0) * C_s);
		}
	}
	return;
}
