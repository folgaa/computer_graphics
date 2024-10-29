#ifndef PNG_PROC_H
#define PNG_PROC_H

#define _CRT_SECURE_NO_WARNINGS

#include <vector>

struct RGB {
	unsigned char r = 0, g = 0, b = 0;
};

namespace NPngProc
{
	const size_t PNG_ERROR = (size_t)-1;

	struct SImage
	{
		SImage() : pBits(0), nWidth(0), nHeight(0), nBPP(0) { }
		~SImage() { release(); }

		void	release()
		{
			if (pBits)
				delete[] pBits, pBits = 0;
			nWidth = nHeight = nBPP = 0;
		}

	public:
		unsigned char* pBits;
		size_t		nWidth;
		size_t		nHeight;
		unsigned long	nBPP;

	};


	size_t readPngFile(const char* szFileName
		, unsigned char* pBuf
		, size_t* pWidth
		, size_t* pHeight
		, unsigned int* pBPP
	);

	size_t readPngFileGray(const char* szFileName
		, unsigned char* pBuf
		, size_t* pWidth
		, size_t* pHeight
	);

	size_t writePngFile(const char* szFileName
		, unsigned char* pBuf
		, size_t	nWidth
		, size_t	nHeight
		, unsigned int nBPP
	);

	size_t write_png_file(const char* filename
		, std::vector<std::vector<RGB>>& image
		, size_t width
		, size_t height
	);

};

#endif /* PNG_PROC_H */