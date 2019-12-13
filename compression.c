/*
 ============================================================================
 Name        : compression.c
 Author      : Hidir Yuzuguzel, 244904
 Version     :
 Copyright   : Your copyright notice
 Description : SGN-31006 Image&Video Processing Techniques Project#2 -- Image Compression
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bitstream.h"
#include "dct.h"

#define DCT_BLOCK_SIZE 8

enum CODEC {ENCODE, DECODE, ENCODERGB, DECODERGB};
bitstream* input;
bitstream* output;
FILE* fp;
FILE* fpo;
enum CODEC codec;
unsigned int** InputImage;
unsigned int** OutputImage;
int** InputImageAfterOffset;
int** OutputImageBeforeOffset;
double** InputDCTImage;
double** OutputDCTImage;
signed int** InputQuantizedImage;
signed int** OutputQuantizedImage;
unsigned int*** InputRGBImage;
unsigned int*** OutputRGBImage;
int*** InputRGBImageAfterOffset;
int*** OutputRGBImageBeforeOffset;
double** InputYImage;
double** OutputYImage;
double** InputYDCTImage;
double** OutputYDCTImage;
signed int** InputYQuantizedImage;
signed int** OutputYQuantizedImage;
double** InputCbImage;
double** OutputCbImage;
double** InputCrImage;
double** OutputCrImage;
double** InputCbSubsampledImage;
double** OutputCbSubsampledImage;
double** InputCbSubsampledDCTImage;
double** OutputCbSubsampledDCTImage;
signed int** InputCbSubsampledQuantizedImage;
signed int** OutputCbSubsampledQuantizedImage;
double** InputCrSubsampledImage;
double** OutputCrSubsampledImage;
double** InputCrSubsampledDCTImage;
double** OutputCrSubsampledDCTImage;
signed int** InputCrSubsampledQuantizedImage;
signed int** OutputCrSubsampledQuantizedImage;
signed int* AC;
signed int DCDIFF;	
double* dctinputblock;
double* idctinputblock;
double* dctoutputblock;
double* idctoutputblock;
int ImageWidth, ImageHeight;

//Quantization matrix for a quality of 50%
double Qluminance[DCT_BLOCK_SIZE][DCT_BLOCK_SIZE] = { { 16, 11, 10, 16, 24, 40, 51, 61 },
														{12, 12, 14, 19, 26, 58, 60, 55 },
														{14, 13, 16, 24, 40, 57, 69, 56},
														{14, 17, 22, 29, 51, 87, 80, 62},
														{18, 22, 37, 56, 68, 109, 103, 77},
														{24, 35, 55, 64, 81, 104, 113, 92},
														{49, 64, 78, 87, 103, 121, 120, 101},
														{72, 92, 95, 98, 112, 100, 103, 99 } };

double Qchrominance[DCT_BLOCK_SIZE][DCT_BLOCK_SIZE] = { { 17, 18, 24, 47, 99, 99, 99, 99 },
														{ 18, 21, 26, 66, 99, 99, 99, 99 },
														{ 24, 26, 56, 99, 99, 99, 99, 99 },
														{ 47, 66, 99, 99, 99, 99, 99, 99 },
														{ 99, 99, 99, 99, 99, 99, 99, 99 },
														{ 99, 99, 99, 99, 99, 99, 99, 99 },
														{ 99, 99, 99, 99, 99, 99, 99, 99 },
														{ 99, 99, 99, 99, 99, 99, 99, 99 } };


//read image from the bitstream
void readImage()
{
	int i,j, k;
	//read 8-bit image
	if (codec == ENCODE)
	{
		InputImage = (unsigned int**)malloc(ImageHeight*sizeof(unsigned int*));
		InputImageAfterOffset = (int**)malloc(ImageHeight*sizeof(int*));
		for (i = 0; i<ImageHeight; i++)
		{
			InputImage[i] = (signed int*)malloc(ImageWidth*sizeof(signed int));
			InputImageAfterOffset[i] = (int*)malloc(ImageWidth*sizeof(int));
		}

		for (i = 0; i<ImageHeight; i++)
		{
			for (j = 0; j<ImageWidth; j++)
			{
				InputImage[i][j] = getbits(input, 8);
			}
		}
	}
	//read 24-bit image
	else if (codec == ENCODERGB)
	{
		InputRGBImage = (unsigned int***)malloc(ImageHeight*sizeof(unsigned int**));
		InputRGBImageAfterOffset = (int***)malloc(ImageHeight*sizeof(int**));
		for (i = 0; i<ImageHeight; i++)
		{
			InputRGBImage[i] = (unsigned int**)malloc(ImageWidth*sizeof(unsigned int*));
			InputRGBImageAfterOffset[i] = (int**)malloc(ImageWidth*sizeof(int*));
			for (j = 0; j < ImageWidth; j++)
			{
				InputRGBImage[i][j] = (unsigned int*)malloc(3 * sizeof(unsigned int));
				InputRGBImageAfterOffset[i][j] = (int*)malloc(3 *sizeof(int));
			}
		}

		for (i = 0; i<ImageHeight; i++)
		{
			for (j = 0; j<ImageWidth; j++)
			{
				for (k = 0; k < 3; k++)
				{
					InputRGBImage[i][j][k] = getbits(input, 8);
				}

			}
		}
	}
	
}

//write the image into a bitstream
void writeImage()
{
	int i, j, k;
	////write 8-bit input image
	//if (codec == ENCODE)
	//{
	//	for (i = 0; i<ImageHeight; i++)
	//	{
	//		for (j = 0; j<ImageWidth; j++)
	//		{
	//			fprintf(fp, "%d ", InputImage[i][j]);
	//		}
	//		fprintf(fp, "\n");
	//	}
	//}
	////write 24-bit input image
	//else if (codec == ENCODERGB)
	//{
	//	for (k = 0; k < 3; k++)
	//	{
	//		fprintf(fp, "InputImage(:,:,%d)\n", k);
	//		for (i = 0; i < ImageHeight; i++)
	//		{
	//			for (j = 0; j < ImageWidth; j++)
	//			{
	//				fprintf(fp, "%u ", InputRGBImage[i][j][k]);
	//			}
	//			fprintf(fp, "\n");
	//		}
	//	}
	//}
	if (codec == DECODE)
	{
		for (i = 0; i<ImageHeight; i++)
		{
			for (j = 0; j<ImageWidth; j++)
			{
				putbits(output, OutputImage[i][j], 8);	
			}
		}
	}
	else if (codec == DECODERGB)
	{
		for (i = 0; i<ImageHeight; i++)
		{
			for (j = 0; j<ImageWidth; j++)
			{
				for (k = 0; k < 3; k++)
				{
					putbits(output, OutputRGBImage[i][j][k], 8);
				}
			}
		}
	}

}

void applyOffset()
{
	int i, j, k;
	unsigned int offsetvalue = 128;
	if (codec == ENCODE)
	{
		for (i = 0; i < ImageHeight; i++)
		{
			for (j = 0; j < ImageWidth; j++)
			{
				InputImageAfterOffset[i][j] = InputImage[i][j] - offsetvalue;
			}
		}
	}
	else if (codec == ENCODERGB)
	{
		for (i = 0; i<ImageHeight; i++)
		{
			for (j = 0; j<ImageWidth; j++)
			{
				for (k = 0; k < 3; k++)
				{
					InputRGBImageAfterOffset[i][j][k] = InputRGBImage[i][j][k] - offsetvalue;
				}

			}
		}
	}
	else if (codec == DECODE)
	{
		for (i = 0; i < ImageHeight; i++)
		{
			for (j = 0; j < ImageWidth; j++)
			{
				OutputImageBeforeOffset[i][j] = OutputImageBeforeOffset[i][j] + offsetvalue;
				if (OutputImageBeforeOffset[i][j] < 0)	//values smaller than 0 are replaced by 0
				{
					OutputImage[i][j] = 0;
				}
				else if (OutputImageBeforeOffset[i][j] > 255)	//values greater than 255 are replaced by 255
				{
					OutputImage[i][j] = 255;
				}
				else
				{
					OutputImage[i][j] = OutputImageBeforeOffset[i][j];
				}
			}
		}
	}
	else if (codec == DECODERGB)
	{
		for (i = 0; i<ImageHeight; i++)
		{
			for (j = 0; j<ImageWidth; j++)
			{
				for (k = 0; k < 3; k++)
				{
					OutputRGBImageBeforeOffset[i][j][k] = OutputRGBImageBeforeOffset[i][j][k] + offsetvalue;
					if (OutputRGBImageBeforeOffset[i][j][k] < 0)	//values smaller than 0 are replaced by 0
					{
						OutputRGBImage[i][j][k] = 0;
					}
					else if (OutputRGBImageBeforeOffset[i][j][k] > 255)	//values greater than 255 are replaced by 255
					{
						OutputRGBImage[i][j][k] = 255;
					}
					else
					{
						OutputRGBImage[i][j][k] = OutputRGBImageBeforeOffset[i][j][k];
					}
				}
			}
		}
	}

}

//RGB image is converted to YCbCr image. Conversion is based on CCIR-601
void rgb2yuv()
{
	int i, j;
	if (codec == ENCODERGB)
	{
		//allocate memory for input yuv image
		InputYImage = (double**)malloc(ImageHeight*sizeof(double*));
		InputCbImage = (double**)malloc(ImageHeight*sizeof(double*));
		InputCrImage = (double**)malloc(ImageHeight*sizeof(double*));
		for (i = 0; i < ImageHeight; i++)
		{
			InputYImage[i] = (double*)malloc(ImageWidth*sizeof(double));
			InputCbImage[i] = (double*)malloc(ImageWidth*sizeof(double));
			InputCrImage[i] = (double*)malloc(ImageWidth*sizeof(double));

		}

		for (i = 0; i < ImageHeight; i++)
		{
			for (j = 0; j < ImageWidth; j++)
			{
				InputYImage[i][j] = 0.299*InputRGBImageAfterOffset[i][j][0] + 0.587*InputRGBImageAfterOffset[i][j][1] + 0.114*InputRGBImageAfterOffset[i][j][2];
				InputCbImage[i][j] = -0.1687*InputRGBImageAfterOffset[i][j][0] - 0.3313*InputRGBImageAfterOffset[i][j][1] + 0.5*InputRGBImageAfterOffset[i][j][2];
				InputCrImage[i][j] = 0.5*InputRGBImageAfterOffset[i][j][0] - 0.4187*InputRGBImageAfterOffset[i][j][1] - 0.0813*InputRGBImageAfterOffset[i][j][2];
			}
		}

	}

}

//YCbCr image is converted to RGB image
void yuv2rgb()
{
	int i, j;
	double R, G, B;
	if (codec == DECODERGB)
	{
		OutputRGBImageBeforeOffset = (int***)malloc(ImageHeight*sizeof(int**));
		for (i = 0; i<ImageHeight; i++)
		{
			OutputRGBImageBeforeOffset[i] = (int**)malloc(ImageWidth*sizeof(int*));
			for (j = 0; j < ImageWidth; j++)
			{
				OutputRGBImageBeforeOffset[i][j] = (int*)malloc(3*sizeof(int));
			}
		}

		for (i = 0; i < ImageHeight; i++)
		{
			for (j = 0; j < ImageWidth; j++)
			{
				R = OutputYImage[i][j] + 0 * OutputCbImage[i][j] + 1.402*OutputCrImage[i][j];
				OutputRGBImageBeforeOffset[i][j][0] = floor(R + 0.5);
				G = OutputYImage[i][j] - 0.34414 * OutputCbImage[i][j] - 0.71414*OutputCrImage[i][j];
				OutputRGBImageBeforeOffset[i][j][1] = floor(G + 0.5);
				B = OutputYImage[i][j] + 1.772 * OutputCbImage[i][j] + 0 * OutputCrImage[i][j];
				OutputRGBImageBeforeOffset[i][j][2] = floor(B + 0.5);
			}
		}

	}
}



//subsample chrominance channels by a factor of 2
void subsampleChrominance()
{
	int i, j, x, y;
	if (codec == ENCODERGB)
	{
		//allocate memory for subsampled Cb and Cr image
		InputCbSubsampledImage = (double**)malloc(ImageHeight / 2 * sizeof(double*));
		InputCrSubsampledImage = (double**)malloc(ImageHeight / 2 * sizeof(double*));
		for (i = 0; i < ImageHeight / 2; i++)
		{
			InputCbSubsampledImage[i] = (double*)malloc(ImageWidth/2*sizeof(double));
			InputCrSubsampledImage[i] = (double*)malloc(ImageWidth / 2 * sizeof(double));
		}

		for (i = 0, x = 0; i < ImageHeight/2; i++, x=x+2)
		{
			for (j = 0, y = 0; j < ImageWidth/2; j++, y=y+2)
			{
				InputCbSubsampledImage[i][j] = 0.25*(InputCbImage[x][y] + InputCbImage[x + 1][y] + InputCbImage[x][y + 1] + InputCbImage[x + 1][y + 1]);
				InputCrSubsampledImage[i][j] = 0.25*(InputCrImage[x][y] + InputCrImage[x + 1][y] + InputCrImage[x][y + 1] + InputCrImage[x + 1][y + 1]);
			}
		}
	}

}

//upsample chrominance channels by a factor of 2
void upsampleChrominance()
{
	int i, j, x, y;
	if (codec == DECODERGB)
	{
		//allocate memory for Cb and Cr image
		OutputCbImage = (double**)malloc(ImageHeight * sizeof(double*));
		OutputCrImage = (double**)malloc(ImageHeight * sizeof(double*));
		for (i = 0; i < ImageHeight; i++)
		{
			OutputCbImage[i] = (double*)malloc(ImageWidth * sizeof(double));
			OutputCrImage[i] = (double*)malloc(ImageWidth * sizeof(double));
		}

		for (i = 0, x = 0; i < ImageHeight / 2; i++, x = x + 2)
		{
			for (j = 0, y = 0; j < ImageWidth / 2; j++, y = y + 2)
			{
				OutputCbImage[x][y] = OutputCbImage[x + 1][y] = OutputCbImage[x][y + 1] = OutputCbImage[x + 1][y + 1] = OutputCbSubsampledImage[i][j];
				OutputCrImage[x][y] = OutputCrImage[x + 1][y] = OutputCrImage[x][y + 1] = OutputCrImage[x + 1][y + 1] = OutputCrSubsampledImage[i][j];
			}
		}

	}
}

//encode DC
void encodeDC(signed int** QuantizedImage, int i, int j, int Width)
{
	int  cat;
	if (i == 0 && j == 0)
	{
		DCDIFF = QuantizedImage[i][j];
	}
	else if (i != 0 && j == 0)
	{
		DCDIFF = QuantizedImage[i][j] - QuantizedImage[i - DCT_BLOCK_SIZE][Width - DCT_BLOCK_SIZE];
	}
	else
	{
		DCDIFF = QuantizedImage[i][j] - QuantizedImage[i][j - DCT_BLOCK_SIZE];
	}

	cat = solve_category(DCDIFF);
	putvlcdc(output, cat);
	putvli(output, cat, DCDIFF);
}

void encodeAC(signed int** QuantizedImage, int i, int j)
{
	int k, ii, jj, cat, run = 0;
	signed int val;
	int isScanned = 1;
	//scan zig-zag order
	ii = -1, jj = 2; k = 0, isScanned = 1;
	while (isScanned)
	{
		while (jj >= 0)
		{
			if (k == DCT_BLOCK_SIZE*DCT_BLOCK_SIZE - 1)
			{
				isScanned = 0;
				break;
			}
			ii++; jj--;
			AC[k++] = QuantizedImage[i + ii][j + jj];
			if (jj == 0)
			{
				if (ii == DCT_BLOCK_SIZE - 1)
				{
					jj++;
					AC[k++] = QuantizedImage[i + ii][j + jj];
				}
				else
				{
					ii++;
					AC[k++] = QuantizedImage[i + ii][j + jj];
				}
				break;
			}
			if (ii == DCT_BLOCK_SIZE - 1)
			{
				jj++;
				AC[k++] = QuantizedImage[i + ii][j + jj];
				break;
			}
		}
		while (ii >= 0)
		{
			if (k == DCT_BLOCK_SIZE*DCT_BLOCK_SIZE - 1)
			{
				isScanned = 0;
				break;
			}
			ii--; jj++;
			AC[k++] = QuantizedImage[i + ii][j + jj];
			if (ii == 0)
			{
				jj++;
				AC[k++] = QuantizedImage[i + ii][j + jj];
				break;
			}
			if (jj == DCT_BLOCK_SIZE - 1)
			{
				ii++;
				AC[k++] = QuantizedImage[i + ii][j + jj];
				break;
			}
		}
	}

	//run-length coding of AC coefficients
	for (k = 0; k < DCT_BLOCK_SIZE*DCT_BLOCK_SIZE - 1; k++)
	{
		if (abs(AC[k])>0)	//non-zero coefficient
		{
			while (run > 15)
			{
				putvlcac(output, 15, 0);
				putvli(output, 0, 0);	//has no effect
				run -= 16;
			}
			val = AC[k];
			cat = solve_category(val);
			putvlcac(output, run, cat);
			putvli(output, cat, val);
			run = 0;	cat = 0;	val = 0;
		}
		if (AC[k] == 0)	//zero-coefficient
		{
			run++;
		}
		if (k == DCT_BLOCK_SIZE*DCT_BLOCK_SIZE - 2)	//end of block 
		{
			if (run > 0)	//EOB symbol
			{
				run = 0;	cat = 0;	val = 0;
				putvlcac(output, run, cat);
				putvli(output, cat, val);	//has no effect
			}
		}
	}
}


void encode()
{
	int  i, j, k, ii, jj;
	signed int val;
	int isScanned = 1;
	dctinputblock = (double*)malloc(DCT_BLOCK_SIZE*DCT_BLOCK_SIZE*sizeof(double));
	dctoutputblock = (double*)malloc(DCT_BLOCK_SIZE*DCT_BLOCK_SIZE*sizeof(double));
	AC = (signed int*)malloc((DCT_BLOCK_SIZE*DCT_BLOCK_SIZE-1)*sizeof(signed int));
	
	
	init_huffman_tables();

	//encode the gray scale image
	if (codec == ENCODE)
	{
		InputDCTImage = (double**)malloc(ImageHeight*sizeof(double*));
		InputQuantizedImage = (signed int**)malloc(ImageHeight*sizeof(signed int*));
		for (i = 0; i < ImageHeight; i++)
		{
			InputDCTImage[i] = (double*)malloc(ImageWidth*sizeof(double));
			InputQuantizedImage[i] = (signed int*)malloc(ImageWidth*sizeof(signed int));
		}

		for (i = 0; i < ImageHeight; i = i + DCT_BLOCK_SIZE)
		{
			for (j = 0; j < ImageWidth; j = j + DCT_BLOCK_SIZE)
			{	
				for (ii = 0; ii < DCT_BLOCK_SIZE; ii++)		//for each N*N block
				{
					for (jj = 0; jj < DCT_BLOCK_SIZE; jj++)
					{
						dctinputblock[ii*DCT_BLOCK_SIZE + jj] = InputImageAfterOffset[i + ii][j + jj];
					}
				}
				fdct(dctinputblock, dctoutputblock);	//apply DCT

				for (ii = 0; ii < DCT_BLOCK_SIZE; ii++)		
				{
					for (jj = 0; jj < DCT_BLOCK_SIZE; jj++)
					{
						InputDCTImage[i + ii][j + jj] = dctoutputblock[ii*DCT_BLOCK_SIZE + jj];
						InputQuantizedImage[i + ii][j + jj] = floor((InputDCTImage[i + ii][j + jj] / Qluminance[ii][jj]) + 0.5);	//apply quantization
					}
				}

				encodeDC(InputQuantizedImage, i, j, ImageWidth);
				encodeAC(InputQuantizedImage, i, j);
			}
		}
	}

	//encode the rgb image
	else if (codec == ENCODERGB)
	{
		InputYDCTImage = (double**)malloc(ImageHeight*sizeof(double*));
		InputYQuantizedImage = (signed int**)malloc(ImageHeight*sizeof(signed int*));
		InputCbSubsampledDCTImage = (double**)malloc(ImageHeight/2*sizeof(double*));
		InputCbSubsampledQuantizedImage = (signed int**)malloc(ImageHeight / 2 * sizeof(signed int*));
		InputCrSubsampledDCTImage = (double**)malloc(ImageHeight/2*sizeof(double*));
		InputCrSubsampledQuantizedImage = (signed int**)malloc(ImageHeight / 2 * sizeof(signed int*));
		for (i = 0; i < ImageHeight; i++)
		{
			InputYDCTImage[i] = (double*)malloc(ImageWidth*sizeof(double));
			InputYQuantizedImage[i] = (signed int*)malloc(ImageWidth*sizeof(signed int));
		}

		for (i = 0; i < ImageHeight / 2; i++)
		{
			InputCbSubsampledDCTImage[i] = (double*)malloc(ImageWidth / 2 * sizeof(double));
			InputCbSubsampledQuantizedImage[i] = (signed int*)malloc(ImageWidth / 2 * sizeof(signed int));
			InputCrSubsampledDCTImage[i] = (double*)malloc(ImageWidth / 2 * sizeof(double));
			InputCrSubsampledQuantizedImage[i] = (signed int*)malloc(ImageWidth / 2 * sizeof(signed int));
		}

		//encode the luminance channel
		for (i = 0; i < ImageHeight; i = i + DCT_BLOCK_SIZE)
		{
			for (j = 0; j < ImageWidth; j = j + DCT_BLOCK_SIZE)
			{
				for (ii = 0; ii < DCT_BLOCK_SIZE; ii++)			//for each N*N block
				{
					for (jj = 0; jj < DCT_BLOCK_SIZE; jj++)
					{
						dctinputblock[ii*DCT_BLOCK_SIZE + jj] = InputYImage[i + ii][j + jj];
					}
				}

				fdct(dctinputblock, dctoutputblock);			//apply DCT

				for (ii = 0; ii < DCT_BLOCK_SIZE; ii++)			//for each N*N block
				{
					for (jj = 0; jj < DCT_BLOCK_SIZE; jj++)
					{
						InputYDCTImage[i + ii][j + jj] = dctoutputblock[ii*DCT_BLOCK_SIZE + jj];
						InputYQuantizedImage[i + ii][j + jj] = floor((InputYDCTImage[i + ii][j + jj] / Qluminance[ii][jj]) + 0.5);
					}
				}
				encodeDC(InputYQuantizedImage, i, j, ImageWidth);
				encodeAC(InputYQuantizedImage, i, j);
			}
		}

		//encode the chrominance channels (Cb)
		for (i = 0; i < ImageHeight / 2; i = i + DCT_BLOCK_SIZE)
		{
			for (j = 0; j < ImageWidth / 2; j = j + DCT_BLOCK_SIZE)
			{
				for (ii = 0; ii < DCT_BLOCK_SIZE; ii++)			//for each N*N block
				{
					for (jj = 0; jj < DCT_BLOCK_SIZE; jj++)
					{
						dctinputblock[ii*DCT_BLOCK_SIZE + jj] = InputCbSubsampledImage[i + ii][j + jj];						
					}
				}
				fdct(dctinputblock, dctoutputblock);			//apply DCT
				for (ii = 0; ii < DCT_BLOCK_SIZE; ii++)			//for each N*N block
				{
					for (jj = 0; jj < DCT_BLOCK_SIZE; jj++)
					{
						InputCbSubsampledDCTImage[i + ii][j + jj] = dctoutputblock[ii*DCT_BLOCK_SIZE + jj];
						InputCbSubsampledQuantizedImage[i + ii][j + jj] = floor((InputCbSubsampledDCTImage[i + ii][j + jj] / Qchrominance[ii][jj]) + 0.5);
					}
				}
				encodeDC(InputCbSubsampledQuantizedImage, i, j, ImageWidth / 2);
				encodeAC(InputCbSubsampledQuantizedImage, i, j);
			}
		}

		//encode the chrominance channels (Cr)
		for (i = 0; i < ImageHeight / 2; i = i + DCT_BLOCK_SIZE)
		{
			for (j = 0; j < ImageWidth / 2; j = j + DCT_BLOCK_SIZE)
			{
				for (ii = 0; ii < DCT_BLOCK_SIZE; ii++)			//for each N*N block
				{
					for (jj = 0; jj < DCT_BLOCK_SIZE; jj++)
					{
						dctinputblock[ii*DCT_BLOCK_SIZE + jj] = InputCrSubsampledImage[i + ii][j + jj];
					}
				}
				fdct(dctinputblock, dctoutputblock);			//apply DCT
				for (ii = 0; ii < DCT_BLOCK_SIZE; ii++)			//for each N*N block
				{
					for (jj = 0; jj < DCT_BLOCK_SIZE; jj++)
					{
						InputCrSubsampledDCTImage[i + ii][j + jj] = dctoutputblock[ii*DCT_BLOCK_SIZE + jj];
						InputCrSubsampledQuantizedImage[i + ii][j + jj] = floor((InputCrSubsampledDCTImage[i + ii][j + jj] / Qchrominance[ii][jj]) + 0.5);
					}
				}
				encodeDC(InputCrSubsampledQuantizedImage, i, j, ImageWidth / 2);
				encodeAC(InputCrSubsampledQuantizedImage, i, j);
			}
		}

	}
	delete_huffman_tables();
	
}

void decodeAC(signed int** QuantizedImage, int i, int j)
{
	int k, r, ii, jj, cat, run;
	int* runptr, *catptr;
	signed int val;
	int isScanned = 1;
	runptr = &run;
	catptr = &cat;


	for (k = 0; k < DCT_BLOCK_SIZE*DCT_BLOCK_SIZE - 1; k++)
	{
		AC[k] = 0;
	}
	k = 0;
	while (k < DCT_BLOCK_SIZE*DCT_BLOCK_SIZE - 1)
	{
		getvlcac(input, runptr, catptr);
		val = getvli(input, cat);
		if (run == 0 && cat == 0)		//EOB symbol
		{
			break;
		}
		else
		{
			for (r = 0; r < run; r++)
			{
				AC[k++] = 0;
			}
			AC[k++] = val;
		}
	}

	//scan zig-zag order
	ii = -1, jj = 2; k = 0, isScanned = 1;
	while (isScanned)
	{
		while (jj >= 0)
		{
			if (k == DCT_BLOCK_SIZE*DCT_BLOCK_SIZE - 1)
			{
				isScanned = 0;
				break;
			}
			ii++; jj--;
			QuantizedImage[i + ii][j + jj] = AC[k++];
			if (jj == 0)
			{
				if (ii == DCT_BLOCK_SIZE - 1)
				{
					jj++;
					QuantizedImage[i + ii][j + jj] = AC[k++];
				}
				else
				{
					ii++;
					QuantizedImage[i + ii][j + jj] = AC[k++];
				}
				break;
			}
			if (ii == DCT_BLOCK_SIZE - 1)
			{
				jj++;
				QuantizedImage[i + ii][j + jj] = AC[k++];
				break;
			}
		}
		while (ii >= 0)
		{
			if (k == DCT_BLOCK_SIZE*DCT_BLOCK_SIZE - 1)
			{
				isScanned = 0;
				break;
			}
			ii--; jj++;
			QuantizedImage[i + ii][j + jj] = AC[k++];
			if (ii == 0)
			{
				jj++;
				QuantizedImage[i + ii][j + jj] = AC[k++];
				break;
			}
			if (jj == DCT_BLOCK_SIZE - 1)
			{
				ii++;
				QuantizedImage[i + ii][j + jj] = AC[k++];
				break;
			}
		}
	}
}

//decoding of DC differential values
void decodeDC(signed int** QuantizedImage, int i, int j, int Width)
{
	int  cat;
	cat = getvlcdc(input);
	DCDIFF = getvli(input, cat);
	
	if (i == 0 && j == 0)
	{
		QuantizedImage[i][j] = DCDIFF;
	}
	else if (i != 0 && j == 0)
	{
		QuantizedImage[i][j] = DCDIFF + QuantizedImage[i - DCT_BLOCK_SIZE][Width - DCT_BLOCK_SIZE];
	}
	else
	{
		QuantizedImage[i][j] = DCDIFF + QuantizedImage[i][j - DCT_BLOCK_SIZE];
	}
}

void decode()
{
	int  i, j, k, ii, jj;
	int isScanned = 1;
	idctinputblock = (double*)malloc(DCT_BLOCK_SIZE*DCT_BLOCK_SIZE*sizeof(double));
	idctoutputblock = (double*)malloc(DCT_BLOCK_SIZE*DCT_BLOCK_SIZE*sizeof(double));
	AC = (signed int*)malloc((DCT_BLOCK_SIZE*DCT_BLOCK_SIZE - 1)*sizeof(signed int));	//buffer for AC coefficients

	init_huffman_tables();

	//decode the gray scale image
	if (codec == DECODE)
	{
		OutputDCTImage = (double**)malloc(ImageHeight*sizeof(double*));
		OutputQuantizedImage = (signed int**)malloc(ImageHeight*sizeof(signed int*));
		OutputImageBeforeOffset = (int**)malloc(ImageHeight*sizeof(int*));
		for (i = 0; i < ImageHeight; i++)
		{
			OutputDCTImage[i] = (double*)malloc(ImageWidth*sizeof(double));
			OutputQuantizedImage[i] = (signed int*)malloc(ImageWidth*sizeof(signed int));
			OutputImageBeforeOffset[i] = (int*)malloc(ImageWidth*sizeof(int));
		}

		for (i = 0; i < ImageHeight; i = i + DCT_BLOCK_SIZE)
		{
			for (j = 0; j < ImageWidth; j = j + DCT_BLOCK_SIZE)
			{
				decodeDC(OutputQuantizedImage, i, j, ImageWidth);		//decoding of DC differential values
				decodeAC(OutputQuantizedImage, i, j);					//decoding of AC coefficients
				
				for (ii = 0; ii < DCT_BLOCK_SIZE; ii++)			//for each N*N block
				{
					for (jj = 0; jj < DCT_BLOCK_SIZE; jj++)
					{
						OutputDCTImage[i + ii][j + jj] = OutputQuantizedImage[i + ii][j + jj] * Qluminance[ii][jj];	//rescaling
						idctinputblock[ii*DCT_BLOCK_SIZE + jj] = OutputDCTImage[i + ii][j + jj];
					}
				}

				idct(idctinputblock, idctoutputblock);	//inverse DCT

				for (ii = 0; ii < DCT_BLOCK_SIZE; ii++)		//for each N*N block
				{
					for (jj = 0; jj < DCT_BLOCK_SIZE; jj++)
					{
						OutputImageBeforeOffset[i + ii][j + jj] = floor(idctoutputblock[ii*DCT_BLOCK_SIZE + jj]+0.5);
						//OutputImageBeforeOffset[i + ii][j + jj] = floor(OutputImageBeforeOffset[i + ii][j + jj] + 0.5);	//HY: 08.07.2015 Doesn't work!!!
					}
				}

			}
		}
	}

	//decode the RGB image
	else if (codec == DECODERGB)
	{
		OutputYImage = (double**)malloc(ImageHeight*sizeof(double*));
		OutputYDCTImage = (double**)malloc(ImageHeight*sizeof(double*));
		OutputYQuantizedImage = (signed int**)malloc(ImageHeight*sizeof(signed int*));
		OutputCbSubsampledImage = (double**)malloc(ImageHeight / 2 * sizeof(double*));
		OutputCbSubsampledDCTImage = (double**)malloc(ImageHeight / 2 * sizeof(double*));
		OutputCbSubsampledQuantizedImage = (signed int**)malloc(ImageHeight / 2 * sizeof(signed int*));
		OutputCrSubsampledImage = (double**)malloc(ImageHeight / 2 * sizeof(double*));
		OutputCrSubsampledDCTImage = (double**)malloc(ImageHeight / 2 * sizeof(double*));
		OutputCrSubsampledQuantizedImage = (signed int**)malloc(ImageHeight / 2 * sizeof(signed int*));
		for (i = 0; i < ImageHeight; i++)
		{
			OutputYImage[i] = (double*)malloc(ImageWidth*sizeof(double));
			OutputYDCTImage[i] = (double*)malloc(ImageWidth*sizeof(double));
			OutputYQuantizedImage[i] = (signed int*)malloc(ImageWidth*sizeof(signed int));
		}

		for (i = 0; i < ImageHeight / 2; i++)
		{
			OutputCbSubsampledImage[i] = (double*)malloc(ImageWidth / 2 * sizeof(double));
			OutputCbSubsampledDCTImage[i] = (double*)malloc(ImageWidth / 2 * sizeof(double));
			OutputCbSubsampledQuantizedImage[i] = (signed int*)malloc(ImageWidth / 2 * sizeof(signed int));
			OutputCrSubsampledImage[i] = (double*)malloc(ImageWidth / 2 * sizeof(double));
			OutputCrSubsampledDCTImage[i] = (double*)malloc(ImageWidth / 2 * sizeof(double));
			OutputCrSubsampledQuantizedImage[i] = (signed int*)malloc(ImageWidth / 2 * sizeof(signed int));
		}

		//decode the luminance channel (Y)
		for (i = 0; i < ImageHeight; i = i + DCT_BLOCK_SIZE)
		{
			for (j = 0; j < ImageWidth; j = j + DCT_BLOCK_SIZE)
			{
				decodeDC(OutputYQuantizedImage, i, j, ImageWidth);		//decoding of DC differential values
				decodeAC(OutputYQuantizedImage, i, j);					//decoding of AC coefficients

				for (ii = 0; ii < DCT_BLOCK_SIZE; ii++)			//for each N*N block
				{
					for (jj = 0; jj < DCT_BLOCK_SIZE; jj++)
					{
						OutputYDCTImage[i + ii][j + jj] = OutputYQuantizedImage[i + ii][j + jj] * Qluminance[ii][jj];	//rescaling
						idctinputblock[ii*DCT_BLOCK_SIZE + jj] = OutputYDCTImage[i + ii][j + jj];
					}
				}

				idct(idctinputblock, idctoutputblock);	//inverse DCT

				for (ii = 0; ii < DCT_BLOCK_SIZE; ii++)		//for each N*N block
				{
					for (jj = 0; jj < DCT_BLOCK_SIZE; jj++)
					{
						//OutputYImage[i + ii][j + jj] = floor(idctoutputblock[ii*DCT_BLOCK_SIZE + jj] + 0.5);
						OutputYImage[i + ii][j + jj] = idctoutputblock[ii*DCT_BLOCK_SIZE + jj];
					}
				}

			}
		}

		//decode the chrominance channels (Cb)
		for (i = 0; i < ImageHeight / 2; i = i + DCT_BLOCK_SIZE)
		{
			for (j = 0; j < ImageWidth / 2; j = j + DCT_BLOCK_SIZE)
			{
				decodeDC(OutputCbSubsampledQuantizedImage, i, j, ImageWidth/2);		//decoding of DC differential values
				decodeAC(OutputCbSubsampledQuantizedImage, i, j);					//decoding of AC coefficients
			
				for (ii = 0; ii < DCT_BLOCK_SIZE; ii++)			//for each N*N block
				{
					for (jj = 0; jj < DCT_BLOCK_SIZE; jj++)
					{
						OutputCbSubsampledDCTImage[i + ii][j + jj] = OutputCbSubsampledQuantizedImage[i + ii][j + jj] * Qchrominance[ii][jj];	//rescaling
						idctinputblock[ii*DCT_BLOCK_SIZE + jj] = OutputCbSubsampledDCTImage[i + ii][j + jj];
					}
				}

				idct(idctinputblock, idctoutputblock);		//inverse DCT

				for (ii = 0; ii < DCT_BLOCK_SIZE; ii++)		//for each N*N block
				{
					for (jj = 0; jj < DCT_BLOCK_SIZE; jj++)
					{
						//OutputCbSubsampledImage[i + ii][j + jj] = floor(idctoutputblock[ii*DCT_BLOCK_SIZE + jj] + 0.5);
						OutputCbSubsampledImage[i + ii][j + jj] = idctoutputblock[ii*DCT_BLOCK_SIZE + jj];
					}
				}
			}
		}

		//decode the chrominance channels (Cr)
		for (i = 0; i < ImageHeight / 2; i = i + DCT_BLOCK_SIZE)
		{
			for (j = 0; j < ImageWidth / 2; j = j + DCT_BLOCK_SIZE)
			{
				decodeDC(OutputCrSubsampledQuantizedImage, i, j, ImageWidth / 2);		//decoding of DC differential values
				decodeAC(OutputCrSubsampledQuantizedImage, i, j);					//decoding of AC coefficients

				for (ii = 0; ii < DCT_BLOCK_SIZE; ii++)			//for each N*N block
				{
					for (jj = 0; jj < DCT_BLOCK_SIZE; jj++)
					{
						OutputCrSubsampledDCTImage[i + ii][j + jj] = OutputCrSubsampledQuantizedImage[i + ii][j + jj] * Qchrominance[ii][jj];	//rescaling
						idctinputblock[ii*DCT_BLOCK_SIZE + jj] = OutputCrSubsampledDCTImage[i + ii][j + jj];
					}
				}

				idct(idctinputblock, idctoutputblock);	//inverse DCT

				for (ii = 0; ii < DCT_BLOCK_SIZE; ii++)		//for each N*N block
				{
					for (jj = 0; jj < DCT_BLOCK_SIZE; jj++)
					{
						//OutputCrSubsampledImage[i + ii][j + jj] = floor(idctoutputblock1[ii*DCT_BLOCK_SIZE + jj] + 0.5);
						OutputCrSubsampledImage[i + ii][j + jj] = idctoutputblock[ii*DCT_BLOCK_SIZE + jj];
					}
				}
			}
		}

	}

	delete_huffman_tables();
}



int main(int argc, char *argv[]) {

	int  i, j, k;
	//fp = fopen("InpImg.txt","w");
	//fpo = fopen("OutImg.txt", "w");
	if (argc < 4)
	{
		printf("There is missing arguments\n");
	}
	else
	{
		input = open_input_bitstream((char*)argv[2]);
		output = open_output_bitstream((char*)argv[3]);
		if (argc == 4)
		{
			ImageHeight = 256;
			ImageWidth = 256;
		}
		else
		{
			ImageWidth = atoi(argv[4]);		//COLUMN
			ImageHeight = atoi(argv[5]);	//ROW
		}

		if(strcmp(argv[1],"encode")==0)
		{
			codec = ENCODE;

			readImage();
			applyOffset();
			encode();

			//delete memory
			for (i = 0; i < ImageHeight; i++)
			{
				free(InputImage[i]);
				free(InputImageAfterOffset[i]);
				free(InputDCTImage[i]);
				free(InputQuantizedImage[i]);
			}
			free(InputImage);
			free(InputImageAfterOffset);
			free(InputDCTImage);
			free(InputQuantizedImage);

		}
		else if (strcmp(argv[1],"decode")==0)
		{
			codec = DECODE;
			OutputImage = (unsigned int**)malloc(ImageHeight*sizeof(unsigned int*));
			for (i = 0; i<ImageHeight; i++)
			{
				OutputImage[i] = (unsigned int*)malloc(ImageWidth*sizeof(unsigned int));
			}
			decode();
			applyOffset();
			writeImage();

			//delete memory
			for (i = 0; i < ImageHeight; i++)
			{
				free(OutputImage[i]);
				free(OutputImageBeforeOffset[i]);
				free(OutputDCTImage[i]);
				free(OutputQuantizedImage[i]);
			}
			free(InputImage);
			free(OutputImageBeforeOffset);
			free(OutputDCTImage);
			free(OutputQuantizedImage);

		}
		else if (strcmp(argv[1],"encodergb")==0)
		{
			codec = ENCODERGB;

			readImage();
			applyOffset();
			rgb2yuv();
			subsampleChrominance();
			encode();

			//delete memory
			for (i = 0; i < ImageHeight; i++)
			{
				for (j = 0; j < ImageWidth; j++)
				{
					free(InputRGBImage[i][j]);
					free(InputRGBImageAfterOffset[i][j]);
				}
				free(InputRGBImage[i]);
				free(InputRGBImageAfterOffset[i]);
				free(InputYImage[i]);
				free(InputCbImage[i]);
				free(InputCrImage[i]);
				free(InputYDCTImage[i]);
			}

			for (i = 0; i < ImageHeight / 2; i++)
			{
				free(InputCbSubsampledDCTImage[i]);
				free(InputCbSubsampledQuantizedImage[i]);
				free(InputCrSubsampledDCTImage[i]);
				free(InputCrSubsampledQuantizedImage[i]);
			}

			free(InputRGBImage);
			free(InputRGBImageAfterOffset);
			free(InputYImage);
			free(InputCbImage);
			free(InputCrImage);
			free(InputYDCTImage);
			free(InputCbSubsampledDCTImage);
			free(InputCbSubsampledQuantizedImage);
			free(InputCrSubsampledDCTImage);
			free(InputCrSubsampledQuantizedImage);		
		}
		else if (strcmp(argv[1],"decodergb")==0)
		{
			codec = DECODERGB;
			OutputRGBImage = (unsigned int***)malloc(ImageHeight*sizeof(unsigned int**));
			for (i = 0; i<ImageHeight; i++)
			{
				OutputRGBImage[i] = (unsigned int**)malloc(ImageWidth*sizeof(unsigned int*));
				for (j = 0; j < ImageWidth; j++)
				{
					OutputRGBImage[i][j] = (unsigned int*)malloc(3 * sizeof(unsigned int));
				}
			}

			decode();
			upsampleChrominance();
			yuv2rgb();
			applyOffset();
			writeImage();

			//delete memory
			for (i = 0; i < ImageHeight; i++)
			{
				for (j = 0; j < ImageWidth; j++)
				{
					free(OutputRGBImage[i][j]);
					free(OutputRGBImageBeforeOffset[i][j]);
				}
				free(OutputRGBImage[i]);
				free(OutputRGBImageBeforeOffset[i]);
				free(OutputYImage[i]);
				free(OutputCbImage[i]);
				free(OutputCrImage[i]);
				free(OutputYDCTImage[i]);
			}
			for (i = 0; i < ImageHeight / 2; i++)
			{
				free(OutputCbSubsampledImage[i]);
				free(OutputCbSubsampledDCTImage[i]);
				free(OutputCbSubsampledQuantizedImage[i]);
				free(OutputCrSubsampledImage[i]);
				free(OutputCrSubsampledDCTImage[i]);
				free(OutputCrSubsampledQuantizedImage[i]);
			}
			free(OutputRGBImage);
			free(OutputRGBImageBeforeOffset);
			free(OutputYImage);
			free(OutputCbImage);
			free(OutputCrImage);
			free(OutputYDCTImage);
			free(OutputCbSubsampledImage);
			free(OutputCbSubsampledDCTImage);
			free(OutputCbSubsampledQuantizedImage);
			free(OutputCrSubsampledImage);
			free(OutputCrSubsampledDCTImage);
			free(OutputCrSubsampledQuantizedImage);
		}
		close_bitstream(input);
		close_bitstream(output);

	}

	//printf("input->output:%d\n",input->output);
//	int i;
//	 for (i = 0; i < argc; ++i)
//	{
//		printf("argv[%d]: %s\n", i, argv[i]);
//	}

	free(AC);
	free(dctinputblock);
	free(dctoutputblock);
	//fclose(fp);
	//fclose(fpo);

	//system("pause");
	return 0;
}
