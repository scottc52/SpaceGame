/**
* Bitmap.h
* code taken from resource 
* http://www.gamedev.net/page/resources/_/technical/game-programming/how-to-load-a-bitmap-r1966
*/
#ifndef _BITMAP_H
#define _BITMAP_H
#include <cstdio>

#ifdef __unix__
#include <stdint.h>
typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef int32_t LONG;
typedef uint8_t BYTE;
typedef struct tagBITMAPFILEHEADER {
  WORD 	 	UNUSED;
  WORD		bfType;
  DWORD   	bfSize;
  WORD		bfReserved1;
  WORD		bfReserved2;
  DWORD   	bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;

//Bitmap information header
//provides information specific to the image data

typedef struct tagBITMAPINFOHEADER{
  DWORD  biSize;
  LONG   biWidth;
  LONG   biHeight;
  WORD   biPlanes;
  WORD   biBitCount;
  DWORD  biCompression;
  DWORD  biSizeImage;
  LONG   biXPelsPerMeter;
  LONG   biYPelsPerMeter;
  DWORD  biClrUsed;
  DWORD  biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;

//Colour palette
typedef struct tagRGBQUAD {
  BYTE	rgbBlue;
  BYTE	rgbGreen;
  BYTE	rgbRed;
  BYTE	rgbReserved;
} RGBQUAD;
#else
#include <windows.h>
#endif

#define BMIH_SIZE 40;
#define BFTYPE (19778) //magic number for error checking  
#define BMFH_START(BMFH) (&(BMFH.bfType))
#define BMFH_SIZE 14

struct Pixel{
	BYTE r;
	BYTE g;
	BYTE b;
	BYTE a; 
};

class Bitmap{
public: 
	RGBQUAD cp;
	BYTE *data;
	int width;
	int height;
	int channels;
	Bitmap(const BITMAPFILEHEADER &bfh, const BITMAPINFOHEADER &bih){
		this->bfh = bfh;
		this->bih = bih;
	}
	~Bitmap(){delete data;};
	static bool isBitmap(const BITMAPFILEHEADER &bfh){
		return (bfh.bfType == BFTYPE);
	} 
	static Bitmap *loadBitmap(const char *fname);
private: 
	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bih;
	  
}; 

#endif