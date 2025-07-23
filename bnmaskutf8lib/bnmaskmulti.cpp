////////////////////////
// blue noiseマスク生成
// bnmask.cpp
// Blue Noise Mask
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "bnmaskwcs.h"
#include "../errcode.h"

///////////////////
// シングルプレーンのブルーノイズマトリクスを元にしてマルチプレーンのブルーノイズマトリクスを作る
// シングルプレーンと同じパラメータを使う
// 分割するには、ブルーノイズマトリクスが十分な大きさを持つことが必要(64×64以上が望ましい)
// 処理時間は、makebluenoisematrix()×プレーン数と同等
// 入力
//  int      componentnum;     プレーン数(RGBなら3、CMYKなら4、最大8まで可能。
//                             プレーン数が多い場合は128×128や256×256で実行する方が品質が向上する)
// 戻り値
//  0....正常終了
//	負...エラー
int CBNMask::mseparatebluenoisematrix(int componentnum)
{
	if(mpmatrixraw == NULL) return(INITIALIZE_ERROR);
	mclearchild();
	if(componentnum <= 1) return(0);
	mppchild = (CBNMask**)malloc(sizeof(CBNMask*) * componentnum);
	if(mppchild == NULL) return(MEMORY_SHORTAGE);
	memset(mppchild,0,sizeof(CBNMask*) * componentnum);
	mcomponentnum = componentnum;
	for(int i = 0 ; i < componentnum ; i++) {
		mppchild[i] = new CBNMask();
		// マルチプレーン対応
		int size = mwidth * mheight / 8;			// 1 / 8が1の擬似ブルーノイズマスクを作るため
		int	low  = size * i;
		int high = size * (i + 1) - 1;
		if(i == 8) high = mwidth * mheight - 1;
		int	i1 = mppchild[i]->msetbluenoisemask(mpmatrixraw,low,high,mwidth,mheight,mlevel,mgausstablesize * 2 - 1,msigma);
		if(i1 < 0) return(i1);
		mppchild[i]->mclearwork();
	}
	return(0);
}
// 2009/05/22改善トライアル版
int CBNMask::mseparatebluenoisematrixjoyo(int componentnum)
{
	if(mpmatrixraw == NULL) return(INITIALIZE_ERROR);
	mclearchild();
	if(componentnum <= 1) return(0);
	mppchild = (CBNMask**)malloc(sizeof(CBNMask*) * componentnum);
	if(mppchild == NULL) return(MEMORY_SHORTAGE);
	memset(mppchild,0,sizeof(CBNMask*) * componentnum);
	mcomponentnum = componentnum;
	for(int i = 0 ; i < componentnum ; i++) {
		mppchild[i] = new CBNMask();
		// マルチプレーン対応
		int	i1 = mppchild[i]->msetbluenoisemaskjoyo(mpmatrixraw,i,mwidth,mheight,mlevel,mgausstablesize * 2 - 1,msigma);
		if(i1 < 0) return(i1);
		mppchild[i]->mclearwork();
	}
	return(0);
}
// マトリクスの値がlow～highの間(low,highを含む)にある場合に1にした擬似ブルーノイズマスクを作成する
int CBNMask::msetbluenoisemask(unsigned int* pmatrix,int low,int high,int width,int height,int level,int ws,double sigma)
{
	mclearbluenoisemask();
	mcleargauss();
	mclearbluenoisematrix();
	mclearweighttable();
	mclearchild();

	mwidth = width;
	mheight = height;
	mlevel = level;
	mgausstablesize = ws / 2 + 1;
	msigma = sigma;

	mpmask = (unsigned char *)malloc(sizeof(unsigned char) * mwidth * mheight);
	if(mpmask == NULL) return(MEMORY_SHORTAGE);
	mpixelnum = 0;
	int		totalpixel = mwidth * mheight;
	for(int i = 0 ; i < mheight ; i++) {
		unsigned int*	pi1 = pmatrix + mwidth * i;
		unsigned char*  ps1 = mpmask  + mwidth * i;
		for(int j = 0 ; j < mwidth ; j++,pi1++,ps1++) {
			int		i1 = totalpixel - *pi1;			// invert(componentが2のときに同じマトリクスになってしまうので)
			if(low <= i1 && i1 <= high) {
				*ps1 = 1;
				mpixelnum++;
			}
			else {
				*ps1 = 0;
			}
		}
	}
	int i1 = minitvoidandcluster();
	if(i1 < 0) return(i1);
// 2009/05/22分割ブルーノイズマスクについて、さらに低周波成分除去を行う
// 循環畳み込み(circular/cyclic convolution)
#if 0
	int			loopnum = 0;
	while(1) {
		loopnum++;
		// mpgauss0の最大値(cluster)を計算する
		// 同じ値が複数ある場合はランダムにゲット
		int		xmax,ymax;
		mcalcmaxgauss(0,mwidth,mheight,mpmask,mpgauss1,xmax,ymax);
		mpmask[mwidth * ymax + xmax] = 0;
		mincdecgauss(0,mwidth,mheight,mpgauss1,xmax,ymax);				// (x,y)の1が消えた分、mpgaussの値を調整
		mincdecgauss(1,mwidth,mheight,mpgauss0,xmax,ymax);				// (x,y)の1が消えた分、mpgaussの値を調整
		// mpgauss1の最大値(void)を計算する(mpgauss0の最小値の代わり)
		// 同じ値が複数ある場合はランダムにゲット
		int		xmin,ymin;
		mcalcmaxgauss(1,mwidth,mheight,mpmask,mpgauss0,xmin,ymin);
		mpmask[mwidth * ymin + xmin] = 1;
		mincdecgauss(1,mwidth,mheight,mpgauss1,xmin,ymin);				// (x,y)の1が増えた分、mpgaussの値を調整
		mincdecgauss(0,mwidth,mheight,mpgauss0,xmin,ymin);				// (x,y)の1が増えた分、mpgaussの値を調整
		if(xmin == xmax && ymin == ymax) break;
	}
#endif
#if defined(_DEBUG)
	if(1) {
		wchar_t		filename[FILENAME_MAX];
		// ブルーノイズパターンの保存
		_swprintf(filename,L"g:/tmp/mpmaskBNchild%d-%d.bmp",low,high);
		int			scanlinesize = (mwidth / 32 + (mwidth % 32 > 0)) * 32 / 8;
		unsigned char*		pdata = (unsigned char*)malloc(scanlinesize * mheight);
		if(pdata) {
			memset(pdata,0,scanlinesize * mheight);
			for(int i = 0 ; i < mheight ; i++) {
				unsigned char* ps1 = mpmask + mwidth * i;
				unsigned char* ps2 = pdata  + scanlinesize * i;
				for(int j = 0 ; j < mwidth ; j++,ps1++) {
					if(*ps1 == 1) {
						ps2[j / 8] |= (0x80 >> (j & 7));
					}
				}
			}
			msavebmp(filename,mwidth,mheight,1,scanlinesize,0,300,pdata);
			free(pdata);
		}
	}
#endif
	i1 = mask2matrix();
	if(i1 < 0) return(i1);
	return(0);
}
// マトリクスの値&7の値がjoyoにした擬似ブルーノイズマスクを作成する
int CBNMask::msetbluenoisemaskjoyo(unsigned int* pmatrix,int joyo,int width,int height,int level,int ws,double sigma)
{
	mclearbluenoisemask();
	mcleargauss();
	mclearbluenoisematrix();
	mclearweighttable();
	mclearchild();

	mwidth = width;
	mheight = height;
	mlevel = level;
	mgausstablesize = ws / 2 + 1;
	msigma = sigma;

	mpmask = (unsigned char *)malloc(sizeof(unsigned char) * mwidth * mheight);
	if(mpmask == NULL) return(MEMORY_SHORTAGE);
	mpixelnum = 0;
	int		totalpixel = mwidth * mheight;
	for(int i = 0 ; i < mheight ; i++) {
		unsigned int*	pi1 = pmatrix + mwidth * i;
		unsigned char*  ps1 = mpmask  + mwidth * i;
		for(int j = 0 ; j < mwidth ; j++,pi1++,ps1++) {
			int		i1 = totalpixel - *pi1;			// invert(componentが2のときに同じマトリクスになってしまうので)
			if((i1 & 7) == joyo) {
				*ps1 = 1;
				mpixelnum++;
			}
			else {
				*ps1 = 0;
			}
		}
	}
	int i1 = minitvoidandcluster();
	if(i1 < 0) return(i1);
// 2009/05/22分割ブルーノイズマスクについて、さらに低周波成分除去を行う
// 循環畳み込み(circular/cyclic convolution)
#if 0
	int			loopnum = 0;
	while(1) {
		loopnum++;
		// mpgauss0の最大値(cluster)を計算する
		// 同じ値が複数ある場合はランダムにゲット
		int		xmax,ymax;
		mcalcmaxgauss(0,mwidth,mheight,mpmask,mpgauss1,xmax,ymax);
		mpmask[mwidth * ymax + xmax] = 0;
		mincdecgauss(0,mwidth,mheight,mpgauss1,xmax,ymax);				// (x,y)の1が消えた分、mpgaussの値を調整
		mincdecgauss(1,mwidth,mheight,mpgauss0,xmax,ymax);				// (x,y)の1が消えた分、mpgaussの値を調整
		// mpgauss1の最大値(void)を計算する(mpgauss0の最小値の代わり)
		// 同じ値が複数ある場合はランダムにゲット
		int		xmin,ymin;
		mcalcmaxgauss(1,mwidth,mheight,mpmask,mpgauss0,xmin,ymin);
		mpmask[mwidth * ymin + xmin] = 1;
		mincdecgauss(1,mwidth,mheight,mpgauss1,xmin,ymin);				// (x,y)の1が増えた分、mpgaussの値を調整
		mincdecgauss(0,mwidth,mheight,mpgauss0,xmin,ymin);				// (x,y)の1が増えた分、mpgaussの値を調整
		if(xmin == xmax && ymin == ymax) break;
	}
#endif
#if defined(_DEBUG)
	if(1) {
		wchar_t		filename[FILENAME_MAX];
		// ブルーノイズパターンの保存
		_swprintf(filename,L"g:/tmp/mpmaskBNchild-%d.bmp",joyo);
		int			scanlinesize = (mwidth / 32 + (mwidth % 32 > 0)) * 32 / 8;
		unsigned char*		pdata = (unsigned char*)malloc(scanlinesize * mheight);
		if(pdata) {
			memset(pdata,0,scanlinesize * mheight);
			for(int i = 0 ; i < mheight ; i++) {
				unsigned char* ps1 = mpmask + mwidth * i;
				unsigned char* ps2 = pdata  + scanlinesize * i;
				for(int j = 0 ; j < mwidth ; j++,ps1++) {
					if(*ps1 == 1) {
						ps2[j / 8] |= (0x80 >> (j & 7));
					}
				}
			}
			msavebmp(filename,mwidth,mheight,1,scanlinesize,0,300,pdata);
			free(pdata);
		}
	}
#endif
	i1 = mask2matrix();
	if(i1 < 0) return(i1);
	return(0);
}
