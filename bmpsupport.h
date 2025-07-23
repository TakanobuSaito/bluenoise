// bmp�t�@�C�����o�̓T�|�[�g
// windows�ȊO��OS�ł�BITMAPFILEHEADER/BITMAPINFOHEADER/RGBQUAD�̎O�̍\���̂�pack(1)=1�o�C�g�o�E���_���Œ�`���邱�Ƃŗ��p�\
#include <malloc.h>
#include <stdio.h>
#include <memory.h>
#include "errcode.h"
#pragma once

#if !defined(_WINGDI_)
// bmp�w�b�_�쐬�p
#	pragma pack(push,bmpsupport)
#	pragma pack(1)
// #include <windows.h>���Ȃ��ł����삷��B
// OS�ˑ��t�@�C�����g��Ȃ��AC�̕W�����C�u�����̂ݗ��p
typedef struct tagRGBQUAD {
    unsigned char    rgbBlue;
    unsigned char    rgbGreen;
    unsigned char    rgbRed;
    unsigned char    rgbReserved;
} RGBQUAD;

typedef struct tagBITMAPFILEHEADER {
    unsigned short	bfType;
    unsigned long	bfSize;
    unsigned short	bfReserved1;
    unsigned short	bfReserved2;
    unsigned long	bfOffBits;
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER{
    unsigned long	biSize;
    long			biWidth;
    long			biHeight;
    unsigned short	biPlanes;
    unsigned short	biBitCount;
    unsigned long	biCompression;
    unsigned long	biSizeImage;
    long			biXPelsPerMeter;
    long			biYPelsPerMeter;
    unsigned long	biClrUsed;
    unsigned long	biClrImportant;
} BITMAPINFOHEADER;
#	pragma pack(pop,bmpsupport)
#endif

BITMAPFILEHEADER	gbfh;
BITMAPINFOHEADER	gbih;
RGBQUAD				grgbquad[256];
FILE*				gfp = NULL;

//////////////
// bmp�t�@�C���̃I�[�v��
//	char*	filename		�t�@�C����
//	int&	width			�摜�̕�(�P�ʃs�N�Z��)
//	int&	height			�摜�̍���(�P�ʃs�N�Z��)
//	int&	depth			1�s�N�Z�������r�b�g��(1/8/24�̂݃T�|�[�g)
//	int&	scanlinesize	�摜��1�s�����o�C�g��(bmp�t�@�C���ł�4�o�C�g�o�E���_��)
//	int&	photometric		0�����̏ꍇ��0/�P�x�l���傫����(1���邢��255)�����̏ꍇ��1
//	int&	dpi				1�C���`�ɉ��s�N�Z����(dot per inch)
//	int&	topdownflag		�g�b�v�_�E���摜�̏ꍇ��1
int	gopenbmp(char* filename,int& width,int& height,int& depth,int& scanlinesize,int& photometric,int& dpi,int& topdownflag)
{
	if ((gfp = fopen(filename, "rb")) == NULL) {
		return(FILE_OPEN_ERROR);
	}
// BITMAPFILEHEADER�w�b�_�ǂݍ���
	if(fread(&gbfh, sizeof(BITMAPFILEHEADER), 1,gfp) != 1) {
		fclose(gfp);
		gfp = NULL;
		return(FILE_READ_ERROR);
	}
	if(fread(&gbih, sizeof(BITMAPINFOHEADER), 1,gfp) != 1) {
		fclose(gfp);
		gfp = NULL;
		return(FILE_READ_ERROR);
	}
	if(gbih.biBitCount == 1) {
		if(gbih.biClrUsed == 2) {
			if(fread(grgbquad,sizeof(RGBQUAD),2,gfp) == 2) {
				// 0����
				if(grgbquad[0].rgbBlue == 255) {
					photometric = 0;
				}
				// 1����
				else {
					photometric = 1;
				}
				// 1����
			}
			else {
				fclose(gfp);
				gfp = NULL;
				return(FILE_READ_ERROR);
			}
		}
		else {
			photometric = 1;
		}
	}
	else if(gbih.biBitCount == 8) {
		if(gbih.biClrUsed == 256) {
			if(fread(grgbquad,sizeof(RGBQUAD),256,gfp) == 256) {
				// 0����
				if(grgbquad[0].rgbBlue == 255) {
					photometric = 0;
				}
				// 1(255)����
				else {
					photometric = 1;
				}
			}
			else {
				fclose(gfp);
				gfp = NULL;
				return(FILE_READ_ERROR);
			}
		}
		else {
			photometric = 1;
		}
	}
	else {
		// �J���[��1(255)����
		photometric = 1;
	}
// �Q�l�摜��256�K���O���C�X�P�[���摜
	depth = gbih.biBitCount;
	if(depth != 1 && depth != 8 && depth != 24) {
		fclose(gfp);
		gfp = NULL;
		return(ILLEGAL_FORMAT);
	}
// �摜�f�[�^�̐擪�܂ŃV�[�N
	if(fseek(gfp,gbfh.bfOffBits,SEEK_SET)) {
		fclose(gfp);
		gfp = NULL;
		return(FILE_SEEK_ERROR);
	}
	dpi = (int)((double)gbih.biXPelsPerMeter / 1000.0 * 25.4 + 0.5);
	if(dpi == 0) {
		dpi = 300;
	}
	width = gbih.biWidth;
	// �S�o�C�g�o�E���_��
	if(depth == 1)
		scanlinesize = (width / 32 + (width % 32 > 0)) * 4;
	else if(depth == 8)
		scanlinesize = (width / 4 + (width % 4 > 0)) * 4;
	else
		scanlinesize = ((width * 3) / 4 + ((width * 3) % 4 > 0)) * 4;
	height = gbih.biHeight;
	if(height < 0) {
		topdownflag = 1;
		height = -height;
	}
	else {
		topdownflag = 0;
	}
	return(0);
}
// bmp�t�@�C���̉摜�f�[�^�̓ǂݍ���
// gopenbmp�̌�ɌĂяo��
int	gloadbmp(int width,int scanlinesize,int height,int topdownflag,unsigned char* pdata)
{
	if(gfp == NULL) return(FILE_READ_ERROR);
// �摜��ǂݍ���
	unsigned char*	ps0 = pdata;

	// �g�b�v�_�E���ł͂Ȃ��@���@�{�g���A�b�v
	if(topdownflag == 0) {
		for(int i = 0 ; i < height ; i++) {
			unsigned char*	ps1 = ps0 + scanlinesize * (height - i - 1);
			size_t rsize = fread(ps1,1,scanlinesize,gfp);
			if(rsize != (unsigned)scanlinesize) {
				fclose(gfp);
				gfp = NULL;
				return(FILE_READ_ERROR);
			}
		}
	}
	// �g�b�v�_�E��
	else {
		for(int i = 0 ; i < height ; i++) {
			unsigned char*	ps1 = ps0 + scanlinesize * i;
			size_t rsize = fread(ps1,1,scanlinesize,gfp);
			if(rsize != (unsigned)scanlinesize) {
				fclose(gfp);
				gfp = NULL;
				return(FILE_READ_ERROR);
			}
		}
	}
	fclose(gfp);
	gfp = NULL;
	return(0);
}
int gcolor2gray(int width,int rawscanlinesize,int height,unsigned char* prawdata, int grayscanlinesize,unsigned char* pgraydata)
{
	for(int i = 0 ; i < height ; i++) {
		unsigned char*	ps1 = prawdata + i * rawscanlinesize;
		unsigned char*	ps2 = pgraydata + i * grayscanlinesize;
		for(int j = 0 ; j < width ; j++,ps1 += 3,ps2++) {
			// 0...B/1...G/2...R
			int		index = 37 * ps1[0] + 150 * ps1[1] + 77 * ps1[2];
			if(index >= 256 * 256) index = 256 * 256 - 1;
			*ps2 = index / 256;
		}
	}
	return(0);
}

int gray2mono(int width,int grayscanlinesize,int height,int photometric, unsigned char* pgraydata,int monoscanlinesize,unsigned char* pdata)
{
	memset(pdata,0,monoscanlinesize * height);			// �w�i��0

	////////////////////////////
	// �������l�����߂�Ƃ���܂�
	// threshold�����ɂQ�l��
	int		threshold = 1;

	unsigned long	n[256];			// �P�x�q�X�g�O����
	memset(n,0,sizeof(long) * 256);

	double			dn[256];		// ���������L���X�g����I�[�o�[�w�b�h�������
	double			dnall = (double)(width * height);
	// n[0]�`n[255]�����߂�
	for(int i = 0 ; i < height ; i++) {
		unsigned char*	ps1 = pgraydata + grayscanlinesize * i;
		for(int j = 0 ; j < width ; j++,ps1++) {
			n[*ps1]++;
		}
	}
	for(int i = 0 ; i < 256 ; i++)
		dn[i] = (double)n[i];
	// 0�`255�܂ł�
	// 0�����[�����g�A�P�����[�����g�����߂�
	double		omega[256];
	double		myu[256];

	omega[0] = dn[0]/dnall;
	myu[0] = 0.0;
	for(int i = 1 ; i < 256 ; i++) {
		omega[i] = omega[i - 1] + dn[i]/dnall;
		myu[i] = myu[i - 1] + i * dn[i]/dnall;
	}

	// �ő�l�����߂�
	double	dmax = -1.0;
	double	between[256];		// ���ԕ��U
	for(int i = 0 ; i < 255 ; i++) {
		double d1 = myu[255] * omega[i] - myu[i];
		d1 = d1 * d1;
		double d2 = omega[i] * (1.0 - omega[i]);
		if(d2 == 0.0)
			between[i] = -1.0;
		else
			between[i] = d1 / d2;
		if(dmax < between[i]) {
			dmax = between[i];
			threshold = i;
		}
	}

	// threshold�̃N���b�s���O
	if(threshold == 0) threshold = 1;

	double		variance = between[threshold];		// ���ԕ��U
	double		percent = omega[threshold - 1];		// 0�`threshold-1�܂ł̋P�x�l�̊���(%)

	// 1����
	if(photometric == 1) {
		for(int i = 0 ; i < height ; i++) {
			unsigned char* psg = pgraydata + grayscanlinesize * i;
			unsigned char* psm = pdata + monoscanlinesize * i;
			for(int j = 0 ; j < width ; j++,psg++) {
				if(*psg >= threshold) {
					psm[j / 8] |= (0x80 >> (j & 7));
				}
			}
		}
	}
	// 0����
	else {
		for(int i = 0 ; i < height ; i++) {
			unsigned char* psg = pgraydata + grayscanlinesize * i;
			unsigned char* psm = pdata + monoscanlinesize * i;
			for(int j = 0 ; j < width ; j++,psg++) {
				if(*psg < threshold) {
					psm[j / 8] |= (0x80 >> (j & 7));
				}
			}
		}
	}
	return(0);
}

int gmakebmpheader(int width,int height,int depth,int scanlinesize,int photometric,int dpi)
{
	if(gfp) {
		fclose(gfp);
		gfp = NULL;
	}
	if(depth == 1) {
		// 0����
		if(photometric == 0) {
			grgbquad[0].rgbBlue     = 255;	/* �� */
			grgbquad[0].rgbRed      = 255;	/* �� */
			grgbquad[0].rgbGreen    = 255;	/* �� */
			grgbquad[0].rgbReserved = 0;		/* �\�� */
			grgbquad[1].rgbBlue     = 0;		/* �� */
			grgbquad[1].rgbRed      = 0;		/* �� */
			grgbquad[1].rgbGreen    = 0;		/* �� */
			grgbquad[1].rgbReserved = 0;		/* �\�� */
		}
		else {
			grgbquad[0].rgbBlue     = 0;		/* �� */
			grgbquad[0].rgbRed      = 0;		/* �� */
			grgbquad[0].rgbGreen    = 0;		/* �� */
			grgbquad[0].rgbReserved = 0;		/* �\�� */
			grgbquad[1].rgbBlue     = 255;	/* �� */
			grgbquad[1].rgbRed      = 255;	/* �� */
			grgbquad[1].rgbGreen    = 255;	/* �� */
			grgbquad[1].rgbReserved = 0;		/* �\�� */
		}
	}
	else if(depth == 8) {
		if(photometric == 0) {
			for(int i = 0 ; i < 256 ; i++) {
				grgbquad[i].rgbBlue     = i;	/* �� */
				grgbquad[i].rgbRed      = i;	/* �� */
				grgbquad[i].rgbGreen    = i;	/* �� */
				grgbquad[i].rgbReserved = 0;	/* �\�� */
			}
		}
		else {
			for(int i = 0 ; i < 256 ; i++) {
				grgbquad[i].rgbBlue     = 255 - i;	/* �� */
				grgbquad[i].rgbRed      = 255 - i;	/* �� */
				grgbquad[i].rgbGreen    = 255 - i;	/* �� */
				grgbquad[i].rgbReserved = 0;	/* �\�� */
			}
		}
	}
	gbfh.bfType = 19778;
	gbfh.bfReserved1 = 0;
	gbfh.bfReserved2 = 0;

	gbih.biXPelsPerMeter = (long)(1000.0 * dpi / 25.4);
	gbih.biYPelsPerMeter = (long)(1000.0 * dpi / 25.4);
	gbih.biClrUsed = 0;
	gbih.biClrImportant = 0;
	gbih.biPlanes = 1;
	gbih.biBitCount = depth;
	gbih.biCompression = 0;
	gbih.biWidth = width;
	gbih.biHeight = height;
// 4�o�C�g�o�E���_��
	int		scanlinesize4 = (scanlinesize / 4 + (scanlinesize % 4 > 0)) * 4;
	if(depth == 1) {
		gbfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 2 * sizeof(RGBQUAD);
	}
	else if(depth == 8) {
		gbfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD);
	}
	else {
		gbfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	}
	gbih.biSizeImage = scanlinesize4 * height;
	gbih.biSize = sizeof(BITMAPINFOHEADER);
	gbfh.bfSize = gbih.biSizeImage + gbfh.bfOffBits;
	return(0);
}

// �f�o�b�O�p
// bmp�t�@�C���ۑ�
int	gsavebmp(wchar_t* filename,int width,int height,int depth,int scanlinesize,int photometric,int dpi,unsigned char* pdata)
{
	int		i1 = 0;
	gmakebmpheader(width,height,depth,scanlinesize,photometric,dpi);
// �t�@�C���I�[�v��
	if((gfp = _wfopen(filename, L"wb")) == NULL) {
		return(0);
	}
// �w�b�_�̕ۑ�
	if(fwrite(&gbfh, sizeof(BITMAPFILEHEADER), 1, gfp) != 1) {
		fclose(gfp);
		gfp = NULL;
		return(FILE_WRITE_ERROR);
	}
	if(fwrite(&gbih, sizeof(BITMAPINFOHEADER), 1, gfp) != 1) {
		fclose(gfp);
		gfp = NULL;
		return(FILE_WRITE_ERROR);
	}
	if(depth == 1) {
		if(fwrite(grgbquad, sizeof(RGBQUAD), 2, gfp) != 2) {
			fclose(gfp);
			gfp = NULL;
			return(FILE_WRITE_ERROR);
		}
	}
	else if(depth == 8) {
		if(fwrite(grgbquad, sizeof(RGBQUAD), 256, gfp) != 256) {
			fclose(gfp);
			gfp = NULL;
			return(FILE_WRITE_ERROR);
		}
	}
	{
		int		scanlinesize4 = (scanlinesize / 4 + (scanlinesize % 4 > 0)) * 4;
		char*	tmpbuf = (char*)malloc(scanlinesize4);
		if(tmpbuf == NULL) {
			fclose(gfp);
			gfp = NULL;
			return(MEMORY_SHORTAGE);
		}
		for(int i = 0 ; i < height ; i++) {
			unsigned char*	ps1 = pdata + scanlinesize * (height - i - 1);
			memcpy(tmpbuf,ps1,scanlinesize);
			if(fwrite(tmpbuf,scanlinesize4,1, gfp) != 1) {
				free(tmpbuf);
				fclose(gfp);
				gfp = NULL;
				return(FILE_WRITE_ERROR);
			}
		}
		free(tmpbuf);
	}
	fclose(gfp);
	gfp = NULL;
	return(0);
}
