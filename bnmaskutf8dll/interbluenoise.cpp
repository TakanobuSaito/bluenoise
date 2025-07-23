///////////////
// INTERBLUENOISEクラス
// C言語APIとCBNMASKクラスの橋渡し
#pragma once

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include "../bnmaskutf8lib/bnmaskwcs.h"
#include "interbluenoise.h"
#include	<io.h>
#include	<direct.h>
#include "../errcode.h"

// static unsigned int stbnmartix256x256[256][256] = {
#include "256x256raw.tbl"

// コンストラクタ
CInterBluenoise::CInterBluenoise()
{
	mptr = 0;					// 出力するブルーノイズの要素
	mwidth = mheight = 256;
	msize = mwidth * mheight;
	matrixfolder[0] = 0;
	mpbn = NULL;
}
CInterBluenoise::~CInterBluenoise()
{
	mclearbluenoise();
}
void CInterBluenoise::mclearbluenoise()
{
	if(mpbn) {
		delete mpbn;
		mpbn = NULL;
	}
}
// seedの設定
void CInterBluenoise::msetseed(unsigned int seed)
{
	mptr = seed % msize;
}
// 0～RAND_MAXの範囲でbluenoise数列の取得
int CInterBluenoise::mgetbluenoise()
{
	minitbluenoise();
	// 0～(msize - 1)の範囲の正数を0～ RAND_MAXに写像して返す
	unsigned int val = mpbn->mgetpmatrixraw()[mptr];
	
	double		rate = (double)val / (msize - 1);
	int ival = (int)(RAND_MAX * rate + 0.5);
	mptr = (mptr + 1) % msize;
	return(ival);
}
// 高精細bluenoise数列の取得
// 0～size * sizeの範囲でbluenoise数列の取得
int CInterBluenoise::mgetbluenoiseex()
{
	minitbluenoise();
	// 0～(msize - 1)の範囲の正数を0～ msizeに写像して返す
	unsigned int val = mpbn->mgetpmatrixraw()[mptr];
	mptr = (mptr + 1) % msize;
	return(val);
}
// bluenoiseフォルダ、サイズの設定
int CInterBluenoise::msetmatrixfolder(const wchar_t* pfolder,int size)
{
#if defined(_DEBUG)
	if(pfolder) {
		wprintf(L"pfolder = %s\n", pfolder); 
		if(pfolder[0] && pfolder[1] && pfolder[2]) {
			printf("pfolder[0] = %d ,pfolder[1] = %d , pfolder[2] = %d\n", pfolder[0], pfolder[1], pfolder[2]);
			unsigned char* ps1 = (unsigned char*)pfolder;
			printf("ps1[0] = %d ,ps1[1] = %d , ps1[2] = %d , ps1[3] = %d\n", ps1[0], ps1[1], ps1[2], ps1[3]);
		}
	}
#endif
	// pfolderがNULLあるいは""の場合 256*256で初期化・再初期化
	if(pfolder == NULL || pfolder[0] == 0) {
		if(mpbn == NULL) {
			minitbluenoise();
		}
		else if(matrixfolder[0] != 0) {
			mwidth = mheight = 256;
			msize = mwidth * mheight;
			matrixfolder[0] = 0;
			mclearbluenoise();
			minitbluenoise();
		}
	}
	// pfolderの変更あるいはsizeの変更がある場合
	// サイズとpfolderの変更(サイズが256になる場合は、内部マトリクスを使う
	else if(wcscmp(pfolder,matrixfolder) != 0 || size != mwidth) {
		wcscpy(matrixfolder,pfolder);
		mwidth = mheight = size;
		msize = mwidth * mheight;
		mclearbluenoise();
		minitbluenoise();
	}
	// mptr >= msizeの可能性あり
//	mptr = mptr % msize;
	mptr = 0;
	return(0);
}

// bluenoiseマトリクス初期化
int	CInterBluenoise::minitbluenoise()
{
	if(mpbn == NULL) {
		mpbn = new CBNMaskFast();
		// サイズが256×256の場合
		if(mwidth == 256) {
			// static マトリクスの設定
			int i1 = mpbn->msetbluenoisematrixraw(256,256,stbnmartix256x256);
			if(i1 < 0) return(i1);
		}
		else {
			if(matrixfolder[0] == 0) return(INITIALIZE_ERROR);
			wchar_t	amatrixfilepath[FILENAME_MAX];
			_swprintf(amatrixfilepath,L"%sbnmatrix%d_%d.dat",matrixfolder,mwidth,mheight);
			if(_waccess(amatrixfilepath,04) == 0) {
				int i1 = mpbn->mloadbnmatrixraw(amatrixfilepath);
				if(i1 < 0) return(i1);
			}
			// bnmaskの生成
			else {
				int i1 = mpbn->makebluenoisematrix(mwidth,mheight);
				if(i1 < 0) return(i1);
				i1 = mpbn->msavebnmatrixraw(amatrixfilepath);
				if(i1 < 0) return(i1);
			}
		}
	}
	return(0);
}

// 1...組織的ディザユーティリティ
// 生成したマトリクスを使ってordered ditherを実行する
//     (通常は、アプリケーション側でマトリクスを取得して、diteringを実行する)
// 入力
// 画像は、１プレーン分の画像(256階調画像専用)
//  int             no;             使用するプレーン番号(0～プレーン数-1)を指定
//                                  同じプレーン番号を指定するとdot-on-dotのブルーノイズマトリクス処理となる
//	int				width;			画像の幅(単位ピクセル)
//	int				height;			画像の高さ(単位ピクセル)
//	int				scanlinesize1;	画像の１ラインのバイト数
//	unsigned char*	pdata1;			256階調画像
//	int				scanlinesize2;	出力2/4階調画像の１ラインのバイト数
// 出力
//  unsigned char*	pdata2;			2/4階調画像
int CInterBluenoise::mordereddither2(int no,int width,int height,int scanlinesize1,unsigned char* pdata1,
						int scanlinesize2,unsigned char* pdata2)
{
	int i1 = minitbluenoise();
	if(i1 < 0) return(i1);
	mpbn->mordereddither2(no,width,height,scanlinesize1,pdata1,scanlinesize2,pdata2);
	return(0);
}
int CInterBluenoise::mordereddither4(int no,int width,int height,int scanlinesize1,unsigned char* pdata1,
						int scanlinesize2,unsigned char* pdata2)
{
	int i1 = minitbluenoise();
	if(i1 < 0) return(i1);
	mpbn->mordereddither4(no,width,height,scanlinesize1,pdata1,scanlinesize2,pdata2);
	return(0);
}
// 2...組織的ディザユーティリティ
//     1行単位で処理するバージョン
// 入力
//   int            y;               行番号
int CInterBluenoise::morderedditherline2(int no,int width,int y,unsigned char* pdata1,unsigned char* pdata2)
{
	int i1 = minitbluenoise();
	if(i1 < 0) return(i1);
	mpbn->morderedditherline2(no,width,y,pdata1,pdata2);
	return(0);
}

int CInterBluenoise::morderedditherline4(int no,int width,int y,unsigned char* pdata1,unsigned char* pdata2)
{
	int i1 = minitbluenoise();
	if(i1 < 0) return(i1);
	mpbn->morderedditherline4(no,width,y,pdata1,pdata2);
	return(0);
}

// 3...比較用：ホワイトノイズによる組織的ディザユーティリティ
//     1行単位で処理するバージョン
// 入力
int CInterBluenoise::mordereddither2white(int no,int width,int height,int scanlinesize1,unsigned char* pdata1,
						int scanlinesize2,unsigned char* pdata2)
{
	int i1 = minitbluenoise();
	if(i1 < 0) return(i1);
	mpbn->mordereddither2white(no,width,height,scanlinesize1,pdata1,scanlinesize2,pdata2);
	return(0);
}
int CInterBluenoise::mordereddither4white(int no,int width,int height,int scanlinesize1,unsigned char* pdata1,
						int scanlinesize2,unsigned char* pdata2)
{
	int i1 = minitbluenoise();
	if(i1 < 0) return(i1);
	mpbn->mordereddither4white(no,width,height,scanlinesize1,pdata1,scanlinesize2,pdata2);
	return(0);
}

// 4...比較用：Floyd & Steinberg方式による誤差拡散法ディザユーティリティ
//     プレーンnoはサポートしていない
//     pdata2の scanlinesize は、強制的に width となるので注意
int CInterBluenoise::mfloydsteinberg2(int width,int height,int scanlinesize1,unsigned char* pdata1,unsigned char* pdata2)
{
	int i1 = minitbluenoise();
	if(i1 < 0) return(i1);
	float*		pf = (float*)malloc(sizeof(float) * width * height);
	if(pf == NULL) return(MEMORY_SHORTAGE);
	mpbn->mfloydsteinberg2(width,height,scanlinesize1,pdata1,pf,pdata2);
	free(pf);
	return(0);
}
int CInterBluenoise::mfloydsteinberg4(int width,int height,int scanlinesize1,unsigned char* pdata1,unsigned char* pdata2)
{
	int i1 = minitbluenoise();
	if(i1 < 0) return(i1);
	float*		pf = (float*)malloc(sizeof(float) * width * height);
	if(pf == NULL) return(MEMORY_SHORTAGE);
	mpbn->mfloydsteinberg4(width,height,scanlinesize1,pdata1,pf,pdata2);
	free(pf);
	return(0);
}

