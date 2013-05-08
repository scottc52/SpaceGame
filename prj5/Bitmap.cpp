/**
* bitmap.cpp
*/

#include "Bitmap.h"
#include <iostream>
#include <cstdlib>

void swap(BYTE *a, BYTE *b){
	BYTE tmp = *a; 
	*a = *b;
	*b = tmp; 
}

void changeendian(BYTE *buff){
	swap(buff, buff+3);
	swap(buff+1, buff+2);
}

Bitmap* Bitmap::loadBitmap(const char *fname){
	FILE *f = fopen(fname, "rb");
	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bih; 
	size_t read = fread(BMFH_START(bfh), BMFH_SIZE, 1, f);
	if (read < 1 || !isBitmap (bfh)){
		return NULL; 
	}


	read = fread(&(bih), sizeof(BITMAPINFOHEADER), 1, f);
	int size= bfh.bfSize - bfh.bfOffBits;
	int skip = bfh.bfOffBits; 
	BYTE *data = new BYTE[size];

	int pad, bytewidth;
	int width = bih.biWidth;
	int height = bih.biHeight; 
	int channels = (float)bih.biBitCount / 8.0; 
	bytewidth = width * channels; 
	pad = bytewidth % 4; 

	int boffset = 0;
	int factor=1; 
	if (height < 0) height *= -1;
	else {factor =-1;boffset=bytewidth*(height-1);} 
	fseek(f, skip, SEEK_SET);
	for (int i = 0; i<height; i++){
		for (int j = 0; j < width; j ++){
			int pos = boffset+j*4; 
			Pixel *p = (Pixel *) &data[pos];
			fread(p, channels, 1, f);
			//changeendian(&data[pos]);
			if(p->a == 0)
				p->a = 255;
		}
		fseek(f, pad, SEEK_CUR);
		boffset +=factor*bytewidth; 
	}
	Bitmap *bmp = new Bitmap(bfh, bih); 
	bmp->data = data;
	bmp->width = width;
	bmp->height = height; 
	bmp->channels = channels; 
	return bmp; 
}