////////////////////////
// blue noiseマスク生成
// bnmaskdebug.cpp
// Blue Noise Mask
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "bnmaskwcs.h"
#include "../errcode.h"

////////////////////////////////////////
// 例：RGBのブルーノイズマトリクスの作成
// CBNMask* pred;
// CBNMask* pred_green;
// CBNMask* pgreen;
// CBNMask* pblue;
// 1...pred->makebluenoisematrix(256,256);
// 2...pgreen->makebluenoisematrix(pred);
// 3...pred_green->mcopybluenoisematrix(pred);
// 4...pred_green->mcomposebluenoisematrix(pgreen);
// 5...pblue->makebluenoisematrix(pred_green);
// 6...pred/pgreen/pblueが各プレーン専用のblue noise matrixとなっている
// ユーティリティ
// 生成したマトリクスを使ってordered ditherを実行する
//     (通常は、アプリケーション側でマトリクスを取得して、diteringを実行する)
// 入力
// 画像は、１プレーン分の画像(256階調画像)
//	int				width;			画像の幅(単位ピクセル)
//	int				height;			画像の高さ(単位ピクセル)
//	int				scanlinesize1;	画像の１ラインのバイト数
//	unsigned char*	pdata1;			256階調画像
//	int				scanlinesize2;	出力2/4階調画像の１ラインのバイト数
// 出力
//  unsigned char*	pdata2;			2/4階調画像(１ピクセル１バイト)
//                                  2階調の場合は0,255
//                                  4階調の場合は0/85/170/255
// ２階調専用
void CBNMask::mordereddither2(int no,int width,int height,int scanlinesize1,unsigned char* pdata1,
						int scanlinesize2,unsigned char* pdata2)
{
	// mpmatrixrawがあって、正規化mpmatrixが無い場合には作る(一瞬)
	int i1 = mconvertmatrixraw();
	if(i1 == 0 && mcomponentnum == 1) {
		for(int i = 0 ,y = 0 ; i < height ;y++, i++) {
			if(y == mheight) y = 0;
			unsigned char*	ps1 = pdata1 + scanlinesize1 * i;
			unsigned char*  ps2 = pdata2 + scanlinesize2 * i;
			unsigned int*	ps3 = mpmatrix + mwidth * y;
			for(int j = 0,x = 0 ; j < width ; j++,x++,ps1++,ps2++) {
				if(x == mwidth) x = 0;
				// マトリクスの値以上(しきい値以上)
				if(ps3[x] <= *ps1) {
					*ps2 = 255;
				}
				else {
					*ps2 = 0;
				}
			}
		}
	}
	else {
		mppchild[no]->mordereddither2(0,width,height,scanlinesize1,pdata1,scanlinesize2,pdata2);
	}
}
// 比較用(white noise)によるディザリング ２階調
void CBNMask::mordereddither2white(int no,int width,int height,int scanlinesize1,unsigned char* pdata1,
						int scanlinesize2,unsigned char* pdata2)
{
	if(mcomponentnum == 1) {
		// white noiseマトリクス(256*256)の作成
		unsigned int* pmatrix = (unsigned int*)malloc(sizeof(unsigned int) * 256 * 256);
		if(pmatrix) {
			{
				for(int i = 0 ; i < 256 ; i++) {
					unsigned int* pi1 = pmatrix + 256 * i;
					for(int j = 0 ; j < 256 ; j++,pi1++) {
						int i1 = rand();
						*pi1 = (unsigned int)(((double)rand() / (double)RAND_MAX) * 256.0);
						if(*pi1 > 255) *pi1 = 255;
					}
				}
			}
			for(int i = 0 ,y = 0 ; i < height ;y++, i++) {
				if(y == 256) y = 0;
				unsigned char*	ps1 = pdata1 + scanlinesize1 * i;
				unsigned char*  ps2 = pdata2 + scanlinesize2 * i;
				unsigned int*	ps3 = pmatrix + 256 * y;
				for(int j = 0,x = 0 ; j < width ; j++,x++,ps1++,ps2++) {
					if(x == 256) x = 0;
					// マトリクスの値以上(しきい値以上)
					if(ps3[x] <= *ps1) {
						*ps2 = 255;
					}
					else {
						*ps2 = 0;
					}
				}
			}
			free(pmatrix);
		}
	}
	else {
		mppchild[no]->mordereddither2white(0,width,height,scanlinesize1,pdata1,scanlinesize2,pdata2);
	}
}
// 比較用(white noise)によるディザリング 4階調
void CBNMask::mordereddither4white(int no,int width,int height,int scanlinesize1,unsigned char* pdata1,
						int scanlinesize2,unsigned char* pdata2)
{
	if(mcomponentnum == 1) {
		// white noiseマトリクス(256*256)の作成
		unsigned int* pmatrix = (unsigned int*)malloc(sizeof(unsigned int) * 256 * 256);
		if(pmatrix) {
			{
				for(int i = 0 ; i < 256 ; i++) {
					unsigned int* pi1 = pmatrix + 256 * i;
					for(int j = 0 ; j < 256 ; j++,pi1++) {
						int i1 = rand();
						*pi1 = (unsigned int)(((double)rand() / (double)RAND_MAX) * 256.0);
						if(*pi1 > 255) *pi1 = 255;
					}
				}
			}
			for(int i = 0 ,y = 0 ; i < height ;y++, i++) {
				if(y == 256) y = 0;
				unsigned char*	ps1 = pdata1 + scanlinesize1 * i;
				unsigned char*  ps2 = pdata2 + scanlinesize2 * i;
				unsigned int*	ps3 = pmatrix + 256 * y;
				for(int j = 0 ,x = 0 ; j < width ; j++,x++,ps1++,ps2++) {
					if(x == 256) x = 0;
					// 0～85    0
					// 86～170  1
					// 171～255 2
					unsigned char	uc1;
					unsigned char	low;
					unsigned char	high;
					if(*ps1 <= 85) {
						low = 0;
						high = 85;
						uc1 = *ps1;
					}
					else if(*ps1 <= 170) {
						low = 85;
						high = 170;
						uc1 = (*ps1 - 85);
					}
					else {
						low = 170;
						high = 255;
						uc1 = (*ps1 - 170);
					}
					uc1 = uc1 + uc1 + uc1;
					// マトリクスの値以上(しきい値以上)
					if(ps3[x] <= uc1) {
						*ps2 = high;
					}
					else {
						*ps2 = low;
					}
				}
			}
			free(pmatrix);
		}
	}
	else {
		mppchild[no]->mordereddither2white(0,width,height,scanlinesize1,pdata1,scanlinesize2,pdata2);
	}
}
// Floyd & Steinberg型の誤差拡散法
// pdata2は255を超える場合があるため1画素4バイトにしておく
void CBNMask::mfloydsteinberg2(int width,int height,int scanlinesize1,unsigned char* pdata1,float* pdata2,unsigned char* pdata3)
{
	int			i;

	// データをint型に変換
	for(i = 0 ; i < height ; i++) {
		float* pf2           = pdata2 + width * i;
		unsigned char* ps1 = pdata1 + scanlinesize1 * i;
		for(int j = 0 ; j < width ; j++,ps1++,pf2++) {
			*pf2 = (float)*ps1;
		}
	}
	for(i = 0 ; i < height ; i++) {
		float*			pf2 = pdata2 + width * i;
		float*			pf3 = pf2 + width;
		if(i < height - 1) {
			for(int j = 0 ; j < width ; j++,pf2++,pf3++) {
				// マトリクスの値以上(しきい値以上)
				float		e;
				if(*pf2 >= 128.0f) {
					e = *pf2 - 255.0f;
					*pf2 = 255.0f;
				}
				else {
					e = *pf2;
					*pf2 = 0.0f;
				}
				if(j < width - 1) {
					*(pf2 + 1) += (e * 7.0f / 16.0f);
					*(pf3 + 1) += (e * 1.0f / 16.0f);
				}
				if(0 < j) {
					*(pf3 - 1) += (e * 3.0f / 16.0f);
				}
				*pf3 += (e * 5.0f / 16.0f);
			}
		}
		else {
			for(int j = 0 ; j < width ; j++,pf2++) {
				// マトリクスの値以上(しきい値以上)
				float		e;
				if(*pf2 >= 128.0f) {
					e = *pf2 - 255.0f;
					*pf2 = 255.0f;
				}
				else {
					e = *pf2;
					*pf2 = 0.0f;
				}
				if(j < width - 1) {
					*(pf2 + 1) += e;
				}
			}
		}
	}
	for(i = 0 ; i < height ; i++) {
		float*			pf2 = pdata2 + width * i;
		unsigned char*	ps3 = pdata3 + width * i;
		for(int j = 0 ; j < width ; j++,pf2++,ps3++) {
			if(*pf2 < 128.0f) *ps3 = 0;
			else *ps3 = 255;
		}
	}
}
void CBNMask::morderedditherline2(int no,int width,int y,unsigned char* pdata1,unsigned char* pdata2)
{
	// mpmatrixrawがあって、正規化mpmatrixが無い場合には作る(一瞬)
	int i1 = mconvertmatrixraw();
	if(i1 == 0 && mcomponentnum == 1) {
		unsigned char*	ps1 = pdata1;
		unsigned char*  ps2 = pdata2;
		y = y % mheight;
		unsigned int*	ps3 = mpmatrix + mwidth * y;
		for(int j = 0 ; j < width ; j++,ps1++,ps2++) {
			int				x = j % mwidth;
			// マトリクスの値以上(しきい値以上)
			if(ps3[x] <= *ps1) {
				*ps2 = 255;
			}
			else {
				*ps2 = 0;
			}
		}
	}
	else {
		mppchild[no]->morderedditherline2(0,width,y,pdata1,pdata2);
	}
}
// 4階調専用
// 入力
void CBNMask::mordereddither4(int no,int width,int height,int scanlinesize1,unsigned char* pdata1,
						int scanlinesize2,unsigned char* pdata2)
{
	// mpmatrixrawがあって、正規化mpmatrixが無い場合には作る(一瞬)
	int i1 = mconvertmatrixraw();
	if(i1 == 0 && mcomponentnum == 1) {
		int			total = mwidth * mheight;
		for(int i = 0,y = 0 ; i < height ; i++,y++) {
			if(y == mheight) y = 0;
			unsigned char*	ps1 = pdata1 + scanlinesize1 * i;
			unsigned char*  ps2 = pdata2 + scanlinesize2 * i;
			unsigned int*	ps3 = mpmatrix + mwidth * y;
			for(int j = 0 ,x = 0 ; j < width ; j++,x++,ps1++,ps2++) {
				if(x == mwidth) x = 0;
				// 0～85    0
				// 86～170  1
				// 171～255 2
				unsigned char	uc1;
				unsigned char	low;
				unsigned char	high;
				if(*ps1 <= 85) {
					low = 0;
					high = 85;
					uc1 = *ps1;
				}
				else if(*ps1 <= 170) {
					low = 85;
					high = 170;
					uc1 = (*ps1 - 85);
				}
				else {
					low = 170;
					high = 255;
					uc1 = (*ps1 - 170);
				}
				uc1 = uc1 + uc1 + uc1;
				// マトリクスの値以上(しきい値以上)
				if(ps3[x] <= uc1) {
					*ps2 = high;
				}
				else {
					*ps2 = low;
				}
			}
		}
	}
	else {
		mppchild[no]->mordereddither4(0,width,height,scanlinesize1,pdata1,scanlinesize2,pdata2);
	}
}
// Floyd & Steinberg型の誤差拡散法
// pdata2は255を超える場合があるため1画素4バイトにしておく
void CBNMask::mfloydsteinberg4(int width,int height,int scanlinesize1,unsigned char* pdata1,float* pdata2,unsigned char* pdata3)
{
	int			i;

	// データをint型に変換
	for(i = 0 ; i < height ; i++) {
		float* pf2           = pdata2 + width * i;
		unsigned char* ps1 = pdata1 + scanlinesize1 * i;
		for(int j = 0 ; j < width ; j++,ps1++,pf2++) {
			*pf2 = (float)*ps1;
		}
	}
	for(i = 0 ; i < height ; i++) {
		float*			pf2 = pdata2 + width * i;
		float*			pf3 = pf2 + width;
		if(i < height - 1) {
			for(int j = 0 ; j < width ; j++,pf2++,pf3++) {
				// マトリクスの値以上(しきい値以上)
				float		e;

				if(*pf2 > 213.3333f) {
					e = *pf2 - 255.0f;
					*pf2 = 255.0f;
				}
				else if(*pf2 > 128.0000f) {
					e = *pf2 - 170.6667f;
					*pf2 = 170.6667f;
				}
				else if(*pf2 > 42.6667f) {
					e = *pf2 - 85.3333f;
					*pf2 = 85.3333f;
				}
				else {
					e = *pf2;
					*pf2 = 0.0f;
				}

				if(j < width - 1) {
					*(pf2 + 1) += (e * 7.0f / 16.0f);
					*(pf3 + 1) += (e * 1.0f / 16.0f);
				}
				if(0 < j) {
					*(pf3 - 1) += (e * 3.0f / 16.0f);
				}
				*pf3 += (e * 5.0f / 16.0f);
			}
		}
		else {
			for(int j = 0 ; j < width ; j++,pf2++) {
				// マトリクスの値以上(しきい値以上)
				float		e;
				if(*pf2 > 213.3333f) {
					e = *pf2 - 255.0f;
					*pf2 = 255.0f;
				}
				else if(*pf2 > 128.0000f) {
					e = *pf2 - 170.6667f;
					*pf2 = 170.6667f;
				}
				else if(*pf2 > 42.6667f) {
					e = *pf2 - 85.3333f;
					*pf2 = 85.3333f;
				}
				else {
					e = *pf2;
					*pf2 = 0.0f;
				}

				if(j < width - 1) {
					*(pf2 + 1) += e;
				}
			}
		}
	}
	for(i = 0 ; i < height ; i++) {
		float*			pf2 = pdata2 + width * i;
		unsigned char*	ps3 = pdata3 + width * i;
		for(int j = 0 ; j < width ; j++,pf2++,ps3++) {
			if(*pf2 > 213.3333f) {
				*ps3 = 255;
			}
			else if(*pf2 > 128.0000f) {
				*ps3 = 170;
			}
			else if(*pf2 > 42.6667f) {
				*ps3 = 85;
			}
			else {
				*ps3 = 0;
			}
		}
	}
}
void CBNMask::morderedditherline4(int no,int width,int y,unsigned char* pdata1,unsigned char* pdata2)
{
	// mpmatrixrawがあって、正規化mpmatrixが無い場合には作る(一瞬)
	int i1 = mconvertmatrixraw();
	if(i1 == 0 && mcomponentnum == 1) {
		unsigned char*	ps1 = pdata1;
		unsigned char*  ps2 = pdata2;
		y = y % mheight;
		unsigned int*	ps3 = mpmatrix + mwidth * y;
		for(int j = 0,x = 0 ; j < width ; j++,x++,ps1++,ps2++) {
			if(x == mwidth) x = 0;
			// 0～85    0
			// 86～170  1
			// 171～255 2
			unsigned char	uc1;
			unsigned char	low;
			unsigned char	high;
			if(*ps1 <= 85) {
				low = 0;
				high = 85;
				uc1 = *ps1;
			}
			else if(*ps1 <= 170) {
				low = 85;
				high = 170;
				uc1 = (*ps1 - 85);
			}
			else {
				low = 170;
				high = 255;
				uc1 = (*ps1 - 170);
			}
			uc1 = uc1 + uc1 + uc1;
			// マトリクスの値以上(しきい値以上)
			if(ps3[x] <= uc1) {
				*ps2 = high;
			}
			else {
				*ps2 = low;
			}
		}
	}
	else {
		mppchild[no]->morderedditherline4(0,width,y,pdata1,pdata2);
	}
}
// デバッグ用
// bmpファイル保存
int	CBNMask::msavebmp(wchar_t* filename,int width,int height,int depth,int scanlinesize,int photometric,int dpi,unsigned char* pdata)
{
	int		i1 = 0;
	makebmpheader(width,height,depth,scanlinesize,photometric,dpi);
// ファイルオープン
	if((mfp = _wfopen(filename, L"wb")) == NULL) {
		return(0);
	}
// ヘッダの保存
	if(fwrite(mpbfh, sizeof(BITMAPFILEHEADER), 1, mfp) != 1) {
		i1 = FILE_WRITE_ERROR;
		goto err_end;
	}
	if(fwrite(mpbih, sizeof(BITMAPINFOHEADER), 1, mfp) != 1) {
		i1 = FILE_WRITE_ERROR;
		goto err_end;
	}
	if(depth == 1) {
		if(fwrite(mprgb, sizeof(RGBQUAD), 2, mfp) != 2) {
			i1 = FILE_WRITE_ERROR;
			goto err_end;
		}
	}
	else if(depth == 8) {
		if(fwrite(mprgb, sizeof(RGBQUAD), 256, mfp) != 256) {
			i1 = FILE_WRITE_ERROR;
			goto err_end;
		}
	}
	{
		int		scanlinesize4 = (scanlinesize / 4 + (scanlinesize % 4 > 0)) * 4;
		char*	tmpbuf = (char*)malloc(scanlinesize4);
		if(tmpbuf == NULL) {
			i1 = MEMORY_SHORTAGE;
			goto err_end;
		}
		for(int i = 0 ; i < height ; i++) {
			unsigned char*	ps1 = pdata + scanlinesize * (height - i - 1);
			memcpy(tmpbuf,ps1,scanlinesize);
			if(fwrite(tmpbuf,scanlinesize4,1, mfp) != 1) {
				i1 = FILE_WRITE_ERROR;
				free(tmpbuf);
				goto err_end;
			}
		}
		free(tmpbuf);
	}
err_end:
	if(mfp) fclose(mfp);
	mfp = NULL;
	return(0);
}
int CBNMask::makebmpheader(int width,int height,int depth,int scanlinesize,int photometric,int dpi)
{
	mclearbmp();
	mpbfh = (BITMAPFILEHEADER*)malloc(sizeof(BITMAPFILEHEADER));
	if(mpbfh == NULL) return(MEMORY_SHORTAGE);
	mpbih = (BITMAPINFOHEADER*)malloc(sizeof(BITMAPINFOHEADER));
	if(mpbih == NULL) return(MEMORY_SHORTAGE);
	mprgb = (RGBQUAD *)malloc(sizeof(RGBQUAD) * 256);
	if(mprgb == NULL) return(MEMORY_SHORTAGE);
	if(depth == 1) {
		// 0が白
		if(photometric == 0) {
			mprgb[0].rgbBlue     = 255;	/* 青 */
			mprgb[0].rgbRed      = 255;	/* 赤 */
			mprgb[0].rgbGreen    = 255;	/* 緑 */
			mprgb[0].rgbReserved = 0;		/* 予備 */
			mprgb[1].rgbBlue     = 0;		/* 青 */
			mprgb[1].rgbRed      = 0;		/* 赤 */
			mprgb[1].rgbGreen    = 0;		/* 緑 */
			mprgb[1].rgbReserved = 0;		/* 予備 */
		}
		else {
			mprgb[0].rgbBlue     = 0;		/* 青 */
			mprgb[0].rgbRed      = 0;		/* 赤 */
			mprgb[0].rgbGreen    = 0;		/* 緑 */
			mprgb[0].rgbReserved = 0;		/* 予備 */
			mprgb[1].rgbBlue     = 255;	/* 青 */
			mprgb[1].rgbRed      = 255;	/* 赤 */
			mprgb[1].rgbGreen    = 255;	/* 緑 */
			mprgb[1].rgbReserved = 0;		/* 予備 */
		}
	}
	else if(depth == 8) {
		if(photometric == 0) {
			for(int i = 0 ; i < 256 ; i++) {
				mprgb[i].rgbBlue     = i;	/* 青 */
				mprgb[i].rgbRed      = i;	/* 赤 */
				mprgb[i].rgbGreen    = i;	/* 緑 */
				mprgb[i].rgbReserved = 0;	/* 予備 */
			}
		}
		else {
			for(int i = 0 ; i < 256 ; i++) {
				mprgb[i].rgbBlue     = 255 - i;	/* 青 */
				mprgb[i].rgbRed      = 255 - i;	/* 赤 */
				mprgb[i].rgbGreen    = 255 - i;	/* 緑 */
				mprgb[i].rgbReserved = 0;	/* 予備 */
			}
		}
	}
	mpbfh->bfType = 19778;
	mpbfh->bfReserved1 = 0;
	mpbfh->bfReserved2 = 0;

	mpbih->biXPelsPerMeter = (long)(1000.0 * dpi / 25.4);
	mpbih->biYPelsPerMeter = (long)(1000.0 * dpi / 25.4);
	mpbih->biClrUsed = 0;
	mpbih->biClrImportant = 0;
	mpbih->biPlanes = 1;
	mpbih->biBitCount = depth;
	mpbih->biCompression = 0;
	mpbih->biWidth = width;
	mpbih->biHeight = height;
// 4バイトバウンダリ
	int		scanlinesize4 = (scanlinesize / 4 + (scanlinesize % 4 > 0)) * 4;
	if(depth == 1) {
		mpbfh->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 2 * sizeof(RGBQUAD);
	}
	else if(depth == 8) {
		mpbfh->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD);
	}
	else {
		mpbfh->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	}
	mpbih->biSizeImage = scanlinesize4 * height;
	mpbih->biSize = sizeof(BITMAPINFOHEADER);
	mpbfh->bfSize = mpbih->biSizeImage + mpbfh->bfOffBits;
	return(0);
}
// インスタンスデータsave/load
int	CBNMask::msavebnm(wchar_t* filename)
{
	FILE* fp = _wfopen(filename,L"wb");
	if(fp == NULL) return(FILE_OPEN_ERROR);
	int i1 =msavebnm(fp);
	if(i1 < 0) {
		fclose(fp);
		return(i1);
	}
	if(mcomponentnum >= 2 && mppchild) {
		for(int i = 0 ; i < mcomponentnum ; i++) {
			i1 = mppchild[i]->msavebnm(fp);
			if(i1 < 0) {
				fclose(fp);
				return(i1);
			}
		}
	}
	fclose(fp);
	return(0);
}
int	CBNMask::msavebnm(FILE* fp)
{
	size_t		wsize;
	int			sizeinc = mgausstablesize * 2 - 1;

	int		version = -mversion;							// 	mversion = 2;
	wsize = fwrite(&version,sizeof(int),1,fp);
	if(wsize != 1) return(FILE_WRITE_ERROR);
	wsize = fwrite(&mpixelnum,sizeof(int),1,fp);
	if(wsize != 1) return(FILE_WRITE_ERROR);
	wsize = fwrite(&mwidth,sizeof(int),1,fp);
	if(wsize != 1) return(FILE_WRITE_ERROR);
	wsize = fwrite(&mheight,sizeof(int),1,fp);
	if(wsize != 1) return(FILE_WRITE_ERROR);
	wsize = fwrite(&mgausstablesize,sizeof(int),1,fp);
	if(wsize != 1) return(FILE_WRITE_ERROR);
	wsize = fwrite(&mcomponentnum,sizeof(int),1,fp);
	if(wsize != 1) return(FILE_WRITE_ERROR);
	wsize = fwrite(&mlevel,sizeof(int),1,fp);
	if(wsize != 1) return(FILE_WRITE_ERROR);
	wsize = fwrite(&msigma,sizeof(double),1,fp);
	if(wsize != 1) return(FILE_WRITE_ERROR);

	int		buffermask = 0;
	buffermask |= (mpmask != NULL);
	buffermask |= (mpgauss1 != NULL) << 1;
	buffermask |= (mpmaskinc != NULL) << 2;
	buffermask |= (mpmatrixraw != NULL) << 3;

	wsize = fwrite(&buffermask,sizeof(int),1,fp);
	if(wsize != 1) return(FILE_WRITE_ERROR);

	if(mpmask != NULL) {
		wsize = fwrite(mpmask,sizeof(unsigned char),mwidth * mheight,fp);
		if(wsize != (size_t)(mwidth * mheight)) return(FILE_WRITE_ERROR);
	}
	if(mpgauss1 != NULL) {
		wsize = fwrite(mpgauss1,sizeof(double),mwidth * mheight,fp);
		if(wsize != (size_t)(mwidth * mheight)) return(FILE_WRITE_ERROR);
		wsize = fwrite(mpgauss0,sizeof(double),mwidth * mheight,fp);
		if(wsize != (size_t)(mwidth * mheight)) return(FILE_WRITE_ERROR);
	}
	if(mpmaskinc != NULL) {
		wsize = fwrite(mpmaskinc,sizeof(unsigned char),sizeinc * sizeinc,fp);
		if(wsize != (size_t)(sizeinc * sizeinc)) return(FILE_WRITE_ERROR);
		wsize = fwrite(mpgaussinc,sizeof(double),sizeinc * sizeinc,fp);
		if(wsize != (size_t)(sizeinc * sizeinc)) return(FILE_WRITE_ERROR);
	}
	if(mpmatrixraw != NULL) {
		wsize = fwrite(mpmatrixraw,sizeof(unsigned int),mwidth * mheight,fp);
		if(wsize != (size_t)(mwidth * mheight)) return(FILE_WRITE_ERROR);
		wsize = fwrite(mpmatrix,sizeof(unsigned int),mwidth * mheight,fp);
		if(wsize != (size_t)(mwidth * mheight)) return(FILE_WRITE_ERROR);
	}
	return(0);
}
int CBNMask::mloadbnm(wchar_t* filename)
{
	FILE* fp = _wfopen(filename,L"rb");
	if(fp == NULL) return(FILE_OPEN_ERROR);
	int i1 = mloadbnm(fp);
	if(i1 < 0) {
		fclose(fp);
		return(i1);
	}
	if(mcomponentnum >= 2) {
		mppchild = (CBNMask**)malloc(sizeof(CBNMask*) * mcomponentnum);
		if(mppchild == NULL) return(MEMORY_SHORTAGE);
		memset(mppchild,0,sizeof(CBNMask*) * mcomponentnum);
		for(int i = 0 ; i < mcomponentnum ; i++) {
			mppchild[i] = new CBNMask();
			i1 = mppchild[i]->mloadbnm(fp);
			if(i1 < 0) {
				fclose(fp);
				return(i1);
			}
		}
	}
	fclose(fp);
	return(0);
}
int CBNMask::mloadbnm(FILE* fp)
{
	size_t		rsize;
	int			sizeinc = 0;
	mclearbluenoisemask();
	mcleargauss();
	mclearbluenoisematrix();
	mclearweighttable();
	mclearbmp();

	rsize = fread(&mpixelnum,sizeof(int),1,fp);
	if(rsize != 1) return(FILE_READ_ERROR);
	// バージョン情報付加
	if(mpixelnum < 0) {
		mversion = -mpixelnum;
		return mloadbnm2(fp);
	}
	rsize = fread(&mwidth,sizeof(int),1,fp);
	if(rsize != 1) return(FILE_READ_ERROR);
	rsize = fread(&mheight,sizeof(int),1,fp);
	if(rsize != 1) return(FILE_READ_ERROR);
	rsize = fread(&mgausstablesize,sizeof(int),1,fp);
	if(rsize != 1) return(FILE_READ_ERROR);
	sizeinc = mgausstablesize * 2 - 1;
	rsize = fread(&mcomponentnum,sizeof(int),1,fp);
	if(rsize != 1) return(FILE_READ_ERROR);
	rsize = fread(&msigma,sizeof(double),1,fp);
	if(rsize != 1) return(FILE_READ_ERROR);

	mpmask = (unsigned char*)malloc(sizeof(unsigned char) * mwidth * mheight);
	if(mpmask == NULL) return(MEMORY_SHORTAGE);
	rsize = fread(mpmask,sizeof(unsigned char),mwidth * mheight,fp);
	if(rsize != (size_t)(mwidth * mheight)) return(FILE_READ_ERROR);

	mpgauss1 = (double *)malloc(sizeof(double) * mwidth * mheight);
	if(mpgauss1 == NULL) return(MEMORY_SHORTAGE);
	mpgauss0 = (double *)malloc(sizeof(double) * mwidth * mheight);
	if(mpgauss0 == NULL) return(MEMORY_SHORTAGE);
	mpgaussptr = (stPoint*)malloc(sizeof(stPoint) * mwidth * mheight);
	if(mpgaussptr == NULL) return(MEMORY_SHORTAGE);

	rsize = fread(mpgauss1,sizeof(double),mwidth * mheight,fp);
	if(rsize != (size_t)(mwidth * mheight)) return(FILE_READ_ERROR);
	rsize = fread(mpgauss0,sizeof(double),mwidth * mheight,fp);
	if(rsize != (size_t)(mwidth * mheight)) return(FILE_READ_ERROR);

	mpmaskinc = (unsigned char*)malloc(sizeof(unsigned char) * sizeinc * sizeinc);
	if(mpmaskinc == NULL) return(MEMORY_SHORTAGE);
	mpgaussinc = (double *)malloc(sizeof(double) * sizeinc * sizeinc);
	if(mpgaussinc == NULL) return(MEMORY_SHORTAGE);
	rsize = fread(mpmaskinc,sizeof(unsigned char),sizeinc * sizeinc,fp);
	if(rsize != (size_t)(sizeinc * sizeinc)) return(FILE_READ_ERROR);
	rsize = fread(mpgaussinc,sizeof(double),sizeinc * sizeinc,fp);
	if(rsize != (size_t)(sizeinc * sizeinc)) return(FILE_READ_ERROR);

	mpmatrixraw = (unsigned int *)malloc(sizeof(unsigned int) * mwidth * mheight);
	if(mpmatrixraw == NULL) return(MEMORY_SHORTAGE);
	rsize = fread(mpmatrixraw,sizeof(unsigned int),mwidth * mheight,fp);
	if(rsize != (size_t)(mwidth * mheight)) {
		free(mpmatrixraw);
		mpmatrixraw = NULL;
	}
	else {
		mpmatrix = (unsigned int *)malloc(sizeof(unsigned int) * mwidth * mheight);
		if(mpmatrix == NULL) return(MEMORY_SHORTAGE);
		rsize = fread(mpmatrix,sizeof(unsigned int),mwidth * mheight,fp);
		if(rsize != (size_t)(mwidth * mheight)) return(FILE_READ_ERROR);
	}
	mcalculateweighttable();
	return(0);
}
// バージョン２
int CBNMask::mloadbnm2(FILE* fp)
{
	size_t		rsize;
	int			sizeinc = 0;

	rsize = fread(&mpixelnum,sizeof(int),1,fp);
	if(rsize != 1) return(FILE_READ_ERROR);
	// バージョン情報付加
	rsize = fread(&mwidth,sizeof(int),1,fp);
	if(rsize != 1) return(FILE_READ_ERROR);
	rsize = fread(&mheight,sizeof(int),1,fp);
	if(rsize != 1) return(FILE_READ_ERROR);
	rsize = fread(&mgausstablesize,sizeof(int),1,fp);
	if(rsize != 1) return(FILE_READ_ERROR);
	sizeinc = mgausstablesize * 2 - 1;
	rsize = fread(&mcomponentnum,sizeof(int),1,fp);
	if(rsize != 1) return(FILE_READ_ERROR);
	rsize = fread(&mlevel,sizeof(int),1,fp);
	if(rsize != 1) return(FILE_READ_ERROR);
	rsize = fread(&msigma,sizeof(double),1,fp);
	if(rsize != 1) return(FILE_READ_ERROR);

	int			buffermask;
	rsize = fread(&buffermask,sizeof(int),1,fp);
	if(rsize != 1) return(FILE_READ_ERROR);

	if(buffermask & 1) {
		mpmask = (unsigned char*)malloc(sizeof(unsigned char) * mwidth * mheight);
		if(mpmask == NULL) return(MEMORY_SHORTAGE);
		rsize = fread(mpmask,sizeof(unsigned char),mwidth * mheight,fp);
		if(rsize != (size_t)(mwidth * mheight)) return(FILE_READ_ERROR);
	}
	if(buffermask & 2) {
		mpgauss1 = (double *)malloc(sizeof(double) * mwidth * mheight);
		if(mpgauss1 == NULL) return(MEMORY_SHORTAGE);
		mpgauss0 = (double *)malloc(sizeof(double) * mwidth * mheight);
		if(mpgauss0 == NULL) return(MEMORY_SHORTAGE);
		mpgaussptr = (stPoint*)malloc(sizeof(stPoint) * mwidth * mheight);
		if(mpgaussptr == NULL) return(MEMORY_SHORTAGE);
		rsize = fread(mpgauss1,sizeof(double),mwidth * mheight,fp);
		if(rsize != (size_t)(mwidth * mheight)) return(FILE_READ_ERROR);
		rsize = fread(mpgauss0,sizeof(double),mwidth * mheight,fp);
		if(rsize != (size_t)(mwidth * mheight)) return(FILE_READ_ERROR);
	}
	if(buffermask & 4) {
		mpmaskinc = (unsigned char*)malloc(sizeof(unsigned char) * sizeinc * sizeinc);
		if(mpmaskinc == NULL) return(MEMORY_SHORTAGE);
		mpgaussinc = (double *)malloc(sizeof(double) * sizeinc * sizeinc);
		if(mpgaussinc == NULL) return(MEMORY_SHORTAGE);
		rsize = fread(mpmaskinc,sizeof(unsigned char),sizeinc * sizeinc,fp);
		if(rsize != (size_t)(sizeinc * sizeinc)) return(FILE_READ_ERROR);
		rsize = fread(mpgaussinc,sizeof(double),sizeinc * sizeinc,fp);
		if(rsize != (size_t)(sizeinc * sizeinc)) return(FILE_READ_ERROR);
	}
	if(buffermask & 8) {
		mpmatrixraw = (unsigned int *)malloc(sizeof(unsigned int) * mwidth * mheight);
		if(mpmatrixraw == NULL) return(MEMORY_SHORTAGE);
		rsize = fread(mpmatrixraw,sizeof(unsigned int),mwidth * mheight,fp);
		if(rsize != (size_t)(mwidth * mheight)) {
			free(mpmatrixraw);
			mpmatrixraw = NULL;
		}
		else {
			mpmatrix = (unsigned int *)malloc(sizeof(unsigned int) * mwidth * mheight);
			if(mpmatrix == NULL) return(MEMORY_SHORTAGE);
			rsize = fread(mpmatrix,sizeof(unsigned int),mwidth * mheight,fp);
			if(rsize != (size_t)(mwidth * mheight)) return(FILE_READ_ERROR);
		}
	}
	mcalculateweighttable();
	return(0);
}
// 最終成果物のセーブとロード
int CBNMask::msavebnmatrixraw(wchar_t* filepath)
{
	FILE* fp = _wfopen(filepath,L"wb");
	if(fp == NULL) return(FILE_OPEN_ERROR);
	int i1 = msavebnmatrixraw(fp);
	fclose(fp);
	if(i1 < 0) return(i1);
	return(0);
}
int CBNMask::msavebnmatrixraw(FILE* fp)
{
	size_t wsize = fwrite(&mversion,sizeof(int),1,fp);
	if(wsize != 1) return(FILE_WRITE_ERROR);
	wsize = fwrite(&mwidth,sizeof(int),1,fp);
	if(wsize != 1) return(FILE_WRITE_ERROR);
	wsize = fwrite(&mheight,sizeof(int),1,fp);
	if(wsize != 1) return(FILE_WRITE_ERROR);
	wsize = fwrite(mpmatrixraw,sizeof(unsigned int),mwidth * mheight,fp);
	if(wsize != (size_t)(mwidth * mheight)) return(FILE_WRITE_ERROR);
	return(0);
}
// 最終成果物のセーブとロード
int CBNMask::mloadbnmatrixraw(wchar_t* filepath)
{
	FILE* fp = _wfopen(filepath,L"rb");
	if(fp == NULL) return(FILE_OPEN_ERROR);
	int i1 = mloadbnmatrixraw(fp);
	fclose(fp);
	if(i1 < 0) return(i1);
	return(0);
}
int CBNMask::mloadbnmatrixraw(FILE* fp)
{
	mclearbluenoisematrix();
	size_t rsize = fread(&mversion,sizeof(int),1,fp);
	if(rsize != 1) return(FILE_READ_ERROR);
	rsize = fread(&mwidth,sizeof(int),1,fp);
	if(rsize != 1) return(FILE_READ_ERROR);
	rsize = fread(&mheight,sizeof(int),1,fp);
	if(rsize != 1) return(FILE_READ_ERROR);
	mpmatrixraw = (unsigned int*)malloc(sizeof(unsigned int) * mwidth * mheight);
	if(mpmatrixraw == NULL) return(MEMORY_SHORTAGE);
	rsize = fread(mpmatrixraw,sizeof(unsigned int),mwidth * mheight,fp);
	if(rsize != (size_t)(mwidth * mheight)) return(FILE_READ_ERROR);
	return(0);
}
// 循環畳み込みテスト
void CBNMask::mconvolutiontest()
{
	unsigned char*	pmask = (unsigned char*)malloc(16 * 16);
	if(pmask == NULL) return;
	double*			pgauss = (double*)malloc(sizeof(double) * 16 * 16 * 2);
	if(pgauss == NULL) {
		free(pmask);
		return;
	}
	double*			pgausswk = pgauss + 16 * 16;
// 左上のみ
	{
		memset(pmask,0,16 * 16);
		for(int i = 0 ; i < 16 ; i++) {
			double* pd1 = pgauss + 16 * i;
			double* pd2 = pgausswk + 16 * i;
			for(int j = 0 ; j < 16 ; j++,pd1++,pd2++) {
				*pd1 = 0.0;
				*pd2 = 0.0;
			}
		}
		pmask[0] = 1;
		mgauss2passy(16,16,pmask,1,pgausswk);
		mgauss2passx(16,16,pgausswk,pgauss);
		wchar_t		filename[FILENAME_MAX];
		wcscpy(filename,L"g:/tmp/pgauss_00.bmp");
		unsigned char*	pdata = (unsigned char*)malloc(16 * 16);
		if(pdata) {
			memset(pdata,0,16 * 16);
			for(int i = 0 ; i < 16 ; i++) {
				double* pd1 = pgauss + 16 * i;
				unsigned char* ps2 = pdata  + 16 * i;
				for(int j = 0 ; j < 16 ; j++,pd1++,ps2++) {
					int			i1 = (int)(*pd1 * 255.0 + 0.5);
					if(i1 > 255) i1 = 255;
					*ps2 = i1;
				}
			}
			msavebmp(filename,16,16,8,16,1,300,pdata);
			free(pdata);
		}
		int		xmax,ymax;
		mcalcmaxgauss(0,0,16,16,pmask,pgauss,xmax,ymax);
		{
			int		foo = 0;
		}
	}
// 境界をまたいで近接点
	{
		memset(pmask,0,16 * 16);
		for(int i = 0 ; i < 16 ; i++) {
			double* pd1 = pgauss + 16 * i;
			double* pd2 = pgausswk + 16 * i;
			for(int j = 0 ; j < 16 ; j++,pd1++,pd2++) {
				*pd1 = 0.0;
				*pd2 = 0.0;
			}
		}
		pmask[5] = 1;
		pmask[16 * 15 + 6] = 1;
		mgauss2passy(16,16,pmask,1,pgausswk);
		mgauss2passx(16,16,pgausswk,pgauss);
		wchar_t		filename[FILENAME_MAX];
		wcscpy(filename,L"g:/tmp/pgauss_01.bmp");
		unsigned char*	pdata = (unsigned char*)malloc(16 * 16);
		if(pdata) {
			memset(pdata,0,16 * 16);
			for(int i = 0 ; i < 16 ; i++) {
				double* pd1 = pgauss + 16 * i;
				unsigned char* ps2 = pdata  + 16 * i;
				for(int j = 0 ; j < 16 ; j++,pd1++,ps2++) {
					int			i1 = (int)(*pd1 * 255.0 + 0.5);
					if(i1 > 255) i1 = 255;
					*ps2 = i1;
				}
			}
			msavebmp(filename,16,16,8,16,1,300,pdata);
			free(pdata);
		}
		int		xmax,ymax;
		mcalcmaxgauss(0,0,16,16,pmask,pgauss,xmax,ymax);
		{
			int		foo = 0;
		}
	}
// 境界部デクリメント
	{
		memset(pmask,0,16 * 16);
		for(int i = 0 ; i < 16 ; i++) {
			double* pd1 = pgauss + 16 * i;
			double* pd2 = pgausswk + 16 * i;
			for(int j = 0 ; j < 16 ; j++,pd1++,pd2++) {
				*pd1 = 0.0;
				*pd2 = 0.0;
			}
		}
		pmask[5] = 1;
		pmask[16 * 15 + 6] = 1;
		mgauss2passy(16,16,pmask,1,pgausswk);
		mgauss2passx(16,16,pgausswk,pgauss);
		mincdecgauss(0,1,16,16,pmask,pgauss,6,15);
		wchar_t		filename[FILENAME_MAX];
		wcscpy(filename,L"g:/tmp/pgauss_02.bmp");
		unsigned char*	pdata = (unsigned char*)malloc(16 * 16);
		if(pdata) {
			memset(pdata,0,16 * 16);
			for(int i = 0 ; i < 16 ; i++) {
				double* pd1 = pgauss + 16 * i;
				unsigned char* ps2 = pdata  + 16 * i;
				for(int j = 0 ; j < 16 ; j++,pd1++,ps2++) {
					int			i1 = (int)(*pd1 * 255.0 + 0.5);
					if(i1 > 255) i1 = 255;
					*ps2 = i1;
				}
			}
			msavebmp(filename,16,16,8,16,1,300,pdata);
			free(pdata);
		}
	}
// 境界部インクリメント
	{
		memset(pmask,0,16 * 16);
		for(int i = 0 ; i < 16 ; i++) {
			double* pd1 = pgauss + 16 * i;
			double* pd2 = pgausswk + 16 * i;
			for(int j = 0 ; j < 16 ; j++,pd1++,pd2++) {
				*pd1 = 0.0;
				*pd2 = 0.0;
			}
		}
		pmask[5] = 1;
		mgauss2passy(16,16,pmask,1,pgausswk);
		mgauss2passx(16,16,pgausswk,pgauss);
		mincdecgauss(1,1,16,16,pmask,pgauss,6,15);
		wchar_t		filename[FILENAME_MAX];
		wcscpy(filename,L"g:/tmp/pgauss_03.bmp");
		unsigned char*	pdata = (unsigned char*)malloc(16 * 16);
		if(pdata) {
			memset(pdata,0,16 * 16);
			for(int i = 0 ; i < 16 ; i++) {
				double* pd1 = pgauss + 16 * i;
				unsigned char* ps2 = pdata  + 16 * i;
				for(int j = 0 ; j < 16 ; j++,pd1++,ps2++) {
					int			i1 = (int)(*pd1 * 255.0 + 0.5);
					if(i1 > 255) i1 = 255;
					*ps2 = i1;
				}
			}
			msavebmp(filename,16,16,8,16,1,300,pdata);
			free(pdata);
		}
	}

	free(pmask);
	free(pgauss);
}
// 比較用のマトリクスのロード
void CBNMask::mloadpreparedmatrix(int size,wchar_t* filename)
{
	mclearwork();
	FILE* fp = _wfopen(filename,L"rb");
	if(fp) {
		if(mpmatrix) {
			free(mpmatrix);
			mpmatrix = (unsigned int *)malloc(sizeof(unsigned int) * size * size);
			if(mpmatrix == NULL) {
				fclose(fp);
				return;
			}
		}
		char	tmp[24];
		size_t rsize = fread(tmp,23,1,fp);
		if(rsize != 1) {
			fclose(fp);
			return;
		}
		for(int i = 0 ; i < size ; i++) {
			unsigned int*	pi = mpmatrix + size * i;
			for(int j = 0 ; j < size ; j++,pi++) {
				char	tmp[16];
				char	tmp1[16];
				tmp[8] = tmp1[8] = 0;
				rsize = fread(tmp,8,1,fp);
				if(rsize != 1) {
					fclose(fp);
					return;
				}
				tmp1[0] = tmp[7];
				tmp1[1] = tmp[6];
				tmp1[2] = tmp[5];
				tmp1[3] = tmp[4];
				tmp1[4] = tmp[3];
				tmp1[5] = tmp[2];
				tmp1[6] = tmp[1];
				tmp1[7] = tmp[0];
				double d1 = *(double*)tmp1;
				int i1 = (unsigned int)(d1 * 255.0 + 0.5);
				if(i1 < 0) {
					i1 = 0;
				}
				else if(255 < i1) {
					i1 = 255;
				}
				*pi = i1;
			}
		}
		fclose(fp);
		mwidth = mheight = size;
	}
}
// for _DEBUG only
// "1"/"0"ピクセルのカウント
int CBNMask::mcountpixel(int zeroone,unsigned char* pdata)
{
	int		pixelnum = 0;
	for(int i = 0 ; i < mheight ; i++) {
		unsigned char* ps1 = pdata + mwidth * i;
		for(int j = 0 ; j < mwidth ; j++,ps1++) {
			if(zeroone) {
				if(*ps1) pixelnum++;
			}
			else {
				if(*ps1 == 0) pixelnum++;
			}
		}
	}
	return(pixelnum);
}
// mpmatrixrawのRAW出力
int CBNMask::msavematrixraw(wchar_t* filename)
{
	FILE* fp = _wfopen(filename,L"wt");
	if(fp) {
		fwprintf(fp,L"static unsigned int stbnmartix%dx%d[%d][%d] = {\n",mheight,mwidth,mheight,mwidth);
		for(int i = 0 ; i < mheight ; i++) {
			fwprintf(fp,L"\t{");
			unsigned int* pj1 = mpmatrixraw + mwidth * i;
			for(int j = 0 ; j < mwidth ; j++,pj1++) {
				fwprintf(fp,L"%d,",*pj1);
			}
			fwprintf(fp,L"},\n");
		}
		fwprintf(fp,L"};\n");
		fclose(fp);
	}
	return(0);
}

// ブルーノイズをnum刻みで、擬似グレイスケール画像として出力する
// mpmatrixrawのbmp出力
// 刻みを num(1～100)にして filename00～filename99のbmpファイルとして出力する
// int	CBNMask::msavebmp(wchar_t* filename,int width,int height,int depth,int scanlinesize,int photometric,int dpi,unsigned char* pdata)
// mwidth * mheightのbmpファイル
int CBNMask::msavematrixrawasbmp(wchar_t* fileprefix, int num)
{
	if(1 <= num && num <= 100) {
		unsigned char* pdata = (unsigned char*)malloc(mwidth * mheight);
		if(pdata) {
			unsigned int step = (mwidth * mheight) / (num + 1);
			unsigned int th = step;
			for(int i = 0 ; i < num ; i++, th += step) {
				// th以上を0 th以下を255で出力
				for(int y = 0 ; y < mheight ; y++) {
					unsigned char* ps1 = pdata + mwidth * y;
					unsigned int*  pj1 = mpmatrixraw + mwidth * y;
					for(int x = 0 ; x < mwidth ; x++, ps1++, pj1++) {
						if(*pj1 >= th) {
							*ps1 = 0;
						}
						else {
							*ps1 = 255;
						}
					}
				}
				// 画像保存
				wchar_t		bmpfilename[FILENAME_MAX];
				swprintf(bmpfilename,L"%s%02d.bmp", fileprefix, i);
				msavebmp(bmpfilename, mwidth, mheight, 8, mwidth, 1, 300, pdata);
			}
			free(pdata);
		}
	}
	return(0);
}
// ホワイトノイズをnum刻みで、擬似グレイスケール画像として出力する
int CBNMask::msavewhitenoiseasbmp(wchar_t* fileprefix, int num)
{
	if(1 <= num && num <= 100) {
		unsigned char* pdata = (unsigned char*)malloc(mwidth * mheight);
		if(pdata) {
			unsigned int step = RAND_MAX / (num + 1);
			unsigned int th = step;
			for(int i = 0 ; i < num ; i++, th += step) {
				// th以上を0 th以下を255で出力
				for(int y = 0 ; y < mheight ; y++) {
					unsigned char* ps1 = pdata + mwidth * y;
					for(int x = 0 ; x < mwidth ; x++, ps1++) {
						if((unsigned)rand() >= th) {
							*ps1 = 0;
						}
						else {
							*ps1 = 255;
						}
					}
				}
				// 画像保存
				wchar_t		bmpfilename[FILENAME_MAX];
				swprintf(bmpfilename,L"%s%02d.bmp", fileprefix, i);
				msavebmp(bmpfilename, mwidth, mheight, 8, mwidth, 1, 300, pdata);
			}
			free(pdata);
		}
	}
	return(0);
}
