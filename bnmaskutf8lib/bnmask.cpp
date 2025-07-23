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

CBNMask::CBNMask()
{
	mversion = 2;
	mpmask = NULL;
	mpmask_phase1 = NULL;
	mpmask_phase23 = NULL;
	mpmaskinc = NULL;
	mpgauss1 = NULL;
	mpgauss1_phase1 = NULL;
	mpgauss0 = NULL;
	mpgauss0_phase23 = NULL;
	mpgaussptr = NULL;
	mpgaussinc = NULL;
	mpmatrix = NULL;
	mpmatrixraw = NULL;
	msigma = GAUSSSIGMA;				// σ原論文では1.5位がベストらしい
	mgausstablesize = GAUSSWINDOWSIZE;	// ガウス窓サイズ：原論文では7(便宜のため必ず奇数)
	mlevel = 256;
	mpweighttable = NULL;				// 窓サイズ/2 + 1のテーブル
	mfp = NULL;
	mpbfh = NULL;
	mpbih = NULL;
	mprgb = NULL;
	// マルチプレーン対応
	mcomponentnum = 1;
	mppchild = NULL;
}
CBNMask::~CBNMask()
{
	mclearbluenoisemask();
	mcleargauss();
	mclearbluenoisematrix();
	mclearweighttable();
	mclearbmp();
	mclearchild();
}
void CBNMask::mclearwork()
{
	mclearbluenoisemask();
	mcleargauss();
	mclearbluenoisematrix2();
	mclearweighttable();
	mclearbmp();
//	mclearchild();
	mclearchildwork();
}
void CBNMask::mclearchild()
{
	if(mppchild) {
		for(int i = 0 ; i < mcomponentnum ; i++) {
			if(mppchild[i]) delete mppchild[i];
		}
		free(mppchild);
		mppchild = NULL;
	}
	mcomponentnum = 1;
}
void CBNMask::mclearchildwork()
{
	if(mppchild) {
		for(int i = 0 ; i < mcomponentnum ; i++) {
			if(mppchild[i]) mppchild[i]->mclearwork();
		}
	}
}

void CBNMask::mclearbmp()
{
	if(mfp) {
		fclose(mfp);
		mfp = NULL;
	}
	if(mpbfh) {
		free(mpbfh);
		mpbfh = NULL;
	}
	if(mpbih) {
		free(mpbih);
		mpbih = NULL;
	}
	if(mprgb) {
		free(mprgb);
		mprgb = NULL;
	}
}
void CBNMask::mclearweighttable()
{
	if(mpweighttable) {
		free(mpweighttable);
		mpweighttable = NULL;
	}
}
void CBNMask::mclearbluenoisemask()
{
	if(mpmask) {
		free(mpmask);
		mpmask = NULL;
	}
	if(mpmaskinc) {
		free(mpmaskinc);
		mpmaskinc = NULL;
	}
}
void CBNMask::mcleargauss()
{
	if(mpgauss1) {
		free(mpgauss1);
		mpgauss1 = NULL;
		mpgausswk = NULL;
	}
	if(mpgauss0) {
		free(mpgauss0);
		mpgauss0 = NULL;
	}
	if(mpgaussptr) {
		free(mpgaussptr);
		mpgaussptr = NULL;
	}
	if(mpgaussinc) {
		free(mpgaussinc);
		mpgaussinc = NULL;
		mpgaussincwk = NULL;
	}
}
void CBNMask::mclearbluenoisematrix()
{
	if(mpmask_phase1) {
		free(mpmask_phase1);
		mpmask_phase1 = NULL;
	}
	if(mpgauss1_phase1) {
		free(mpgauss1_phase1);
		mpgauss1_phase1 = NULL;
	}
	if(mpgauss0_phase23) {
		free(mpgauss0_phase23);
		mpgauss0_phase23 = NULL;
	}
	if(mpmatrixraw) {
		free(mpmatrixraw);
		mpmatrixraw = NULL;
	}
	if(mpmatrix) {
		free(mpmatrix);
		mpmatrix = NULL;
	}
}
// mpmatrixだけ残す
void CBNMask::mclearbluenoisematrix2()
{
	if(mpmask_phase1) {
		free(mpmask_phase1);
		mpmask_phase1 = NULL;
	}
	if(mpmask_phase23) {
		free(mpmask_phase23);
		mpmask_phase23 = NULL;
	}
	if(mpgauss1_phase1) {
		free(mpgauss1_phase1);
		mpgauss1_phase1 = NULL;
	}
	if(mpgauss0_phase23) {
		free(mpgauss0_phase23);
		mpgauss0_phase23 = NULL;
	}
}
// ガウスフィルタのウェイトテーブル(１次元×２)の作成
// 2009高速化の検討:初期化部分で関係ない
int CBNMask::mcalculateweighttable()
{
	mpweighttable = (double *)malloc(sizeof(double) * mgausstablesize);
	if(mpweighttable == NULL) return(MEMORY_SHORTAGE);

	double	total = 0.0;
	int     i;
	for(i = 0; i < mgausstablesize ; i++ ){
		mpweighttable[i] = exp(-0.5 * (double)(i * i) / msigma);
		if(0 == i){
			total += mpweighttable[i];
		}
		else {
			// 中心以外は、２回同じ係数を使うので２倍
			total += 2.0 * mpweighttable[i];
		}
	}
	// 規格化
	for(i = 0 ; i< mgausstablesize ; i++) {
		mpweighttable[i] /= total;
	}
	return(0);
}
// mpmask上にホワイトマスクを生成
// 2009高速化の検討:初期化部分で関係ない
int CBNMask::makewhitenoisemask(double threshold)
{
	mpmask = (unsigned char *)malloc(sizeof(unsigned char) * mwidth * mheight);
	if(mpmask == NULL) return(MEMORY_SHORTAGE);
#if defined(_DEBUG)
	srand(0);			// 毎回変わるとデバッグに不便
#else
	srand( (unsigned)time( NULL ) );
#endif
	mpixelnum = 0;			// 1の数
	for(int i = 0 ; i < mheight ; i++) {
		unsigned char*		ps1 = mpmask + mwidth * i;
		for(int j = 0 ; j < mwidth ; j++,ps1++) {
			int		i1 = rand();
			double	d1 = (double)i1 / (double)RAND_MAX; 
			if(d1 <= threshold) {
				*ps1 = 1;
				mpixelnum++;
			}
			else {
				*ps1 = 0;
			}
		}
	}
#if defined(_DEBUG)
	if(1) {
		wchar_t		filename[FILENAME_MAX];
		// ホワイトノイズパターンの保存
		wcscpy(filename,L"g:/tmp/mpmask.bmp");
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
	return(0);
}

// void-and-cluster法でホワイトノイズマスクをブルーノイズマスクに変換
// 2009高速化の検討:初期化部分で関係ない
int CBNMask::minitvoidandcluster()
{
	mpgauss1 = (double*)malloc(sizeof(double) * mwidth * mheight * 2);
	if(mpgauss1 == NULL) return(MEMORY_SHORTAGE);
	mpgausswk = mpgauss1 + mwidth * mheight;
	mpgauss0 = (double*)malloc(sizeof(double) * mwidth * mheight);
	if(mpgauss0 == NULL) return(MEMORY_SHORTAGE);
	mpgaussptr = (stPoint*)malloc(sizeof(stPoint) * mwidth * mheight);
	if(mpgaussptr == NULL) return(MEMORY_SHORTAGE);
	int i;
	for(i = 0 ; i < mheight ; i++) {
		double* pd0 = mpgauss0 + mwidth * i;
		double* pd1 = mpgauss1 + mwidth * i;
		double* pd2 = mpgausswk + mwidth * i;
		for(int j = 0 ; j < mwidth ; j++,pd0++,pd1++,pd2++) {
			*pd0 = 0.0;
			*pd1 = 0.0;
			*pd2 = 0.0;
		}
	}
	mcalculateweighttable();
	mgauss2passy(mwidth,mheight,mpmask,1,mpgausswk);
	mgauss2passx(mwidth,mheight,mpgausswk,mpgauss1);
	mgauss2passy(mwidth,mheight,mpmask,0,mpgausswk);
	mgauss2passx(mwidth,mheight,mpgausswk,mpgauss0);
	mpmaskinc = (unsigned char*)malloc((mgausstablesize * 2 - 1) * (mgausstablesize * 2 - 1));
	if(mpmaskinc == NULL) return(MEMORY_SHORTAGE);
	memset(mpmaskinc,0,(mgausstablesize * 2 - 1) * (mgausstablesize * 2 - 1));
	// 中心に点を打つ
	mpmaskinc[(mgausstablesize * 2 - 1) * (mgausstablesize - 1) + (mgausstablesize - 1)] = 1;
	mpgaussinc = (double *)malloc(sizeof(double) * (mgausstablesize * 2 - 1) * (mgausstablesize * 2 - 1) * 2);
	if(mpgaussinc == NULL) return(MEMORY_SHORTAGE);
	mpgaussincwk = mpgaussinc + (mgausstablesize * 2 - 1) * (mgausstablesize * 2 - 1);
	for(i = 0 ; i < (mgausstablesize * 2 - 1) ; i++) {
		double* pd = mpgaussinc + (mgausstablesize * 2 - 1) * i;
		double* pdwk = mpgaussincwk + (mgausstablesize * 2 - 1) * i;
		for(int j = 0 ; j < mgausstablesize * 2 - 1 ; j++,pd++,pdwk++) {
			*pd = 0.0;
			*pdwk = 0.0;
		}
	}
	mgauss2passy((mgausstablesize * 2 - 1),(mgausstablesize * 2 - 1),mpmaskinc,1,mpgaussincwk);
	mgauss2passx((mgausstablesize * 2 - 1),(mgausstablesize * 2 - 1),mpgaussincwk,mpgaussinc);
#if defined(_DEBUG)
	// 循環畳み込みテスト
	if(0) mconvolutiontest();
#endif
	return(0);
}
// 2009高速化の検討:この関数の高速化が必要
int CBNMask::mvoidandcluster()
{
	int i1 = minitvoidandcluster();
	if(i1 < 0) return(i1);
#if defined(_DEBUG)
	if(1) {
		wchar_t		filename[FILENAME_MAX];
		// インクリメントパターンの保存
		wcscpy(filename,L"g:/tmp/mpgaussinc.bmp");
		int				width = (mgausstablesize * 2 - 1);
		int				height = (mgausstablesize * 2 - 1);
		int				scanlinesize = (width / 4 + (width % 4 > 0)) * 4;
		unsigned char*	pdata = (unsigned char*)malloc(scanlinesize * height);
		if(pdata) {
			memset(pdata,0,scanlinesize * height);
			for(int i = 0 ; i < height ; i++) {
				double* pd1 = mpgaussinc + width * i;
				unsigned char* ps2 = pdata  + scanlinesize * i;
				for(int j = 0 ; j < width ; j++,pd1++,ps2++) {
					int			i1 = (int)(*pd1 * 255.0 + 0.5);
					if(i1 > 255) i1 = 255;
					*ps2 = i1;
				}
			}
			msavebmp(filename,width,height,8,scanlinesize,1,300,pdata);
			free(pdata);
		}
	}
	if(1) {
		wchar_t		filename[FILENAME_MAX];
		// ガウスフィルタパターンの保存
		wcscpy(filename,L"g:/tmp/mpgauss0.bmp");
		int				scanlinesize = (mwidth / 4 + (mwidth % 4 > 0)) * 4;
		unsigned char*	pdata = (unsigned char*)malloc(scanlinesize * mheight);
		if(pdata) {
			memset(pdata,0,scanlinesize * mheight);
			for(int i = 0 ; i < mheight ; i++) {
				double* pd1 = mpgauss0 + mwidth * i;
				unsigned char* ps2 = pdata  + scanlinesize * i;
				for(int j = 0 ; j < mwidth ; j++,pd1++,ps2++) {
					int			i1 = (int)(*pd1 * 255.0 + 0.5);
					if(i1 > 255) i1 = 255;
					*ps2 = i1;
				}
			}
			msavebmp(filename,mwidth,mheight,8,scanlinesize,1,300,pdata);
			free(pdata);
		}
	}
	if(1) {
		wchar_t		filename[FILENAME_MAX];
		// ガウスフィルタパターンの保存
		wcscpy(filename,L"g:/tmp/mpgauss1.bmp");
		int				scanlinesize = (mwidth / 4 + (mwidth % 4 > 0)) * 4;
		unsigned char*	pdata = (unsigned char*)malloc(scanlinesize * mheight);
		if(pdata) {
			memset(pdata,0,scanlinesize * mheight);
			for(int i = 0 ; i < mheight ; i++) {
				double* pd1 = mpgauss1 + mwidth * i;
				unsigned char* ps2 = pdata  + scanlinesize * i;
				for(int j = 0 ; j < mwidth ; j++,pd1++,ps2++) {
					int			i1 = (int)(*pd1 * 255.0 + 0.5);
					if(i1 > 255) i1 = 255;
					*ps2 = i1;
				}
			}
			msavebmp(filename,mwidth,mheight,8,scanlinesize,1,300,pdata);
			free(pdata);
		}
	}
#endif
// 循環畳み込み(circular/cyclic convolution)
	int			loopnum = 0;
	while(1) {
		loopnum++;
		// mpgauss0の最大値(cluster)を計算する
		// 同じ値が複数ある場合はランダムにゲット
		int		xmax,ymax;
		i1 = mcalcmaxgauss(0,0,mwidth,mheight,mpmask,mpgauss1,xmax,ymax);
		if(i1 < 0) return(i1);
		mpmask[mwidth * ymax + xmax] = 0;
		mincdecgauss(0,1,mwidth,mheight,mpmask,mpgauss1,xmax,ymax);				// (x,y)の1が消えた分、mpgaussの値を調整
		mincdecgauss(1,0,mwidth,mheight,mpmask,mpgauss0,xmax,ymax);				// (x,y)の1が消えた分、mpgaussの値を調整
		// mpgauss1の最大値(void)を計算する(mpgauss0の最小値の代わり)
		// 同じ値が複数ある場合はランダムにゲット
		int		xmin,ymin;
		i1 = mcalcmaxgauss(1,0,mwidth,mheight,mpmask,mpgauss0,xmin,ymin);
		if(i1 < 0) return(i1);
		mpmask[mwidth * ymin + xmin] = 1;
		mincdecgauss(1,1,mwidth,mheight,mpmask,mpgauss1,xmin,ymin);				// (x,y)の1が増えた分、mpgaussの値を調整
		mincdecgauss(0,0,mwidth,mheight,mpmask,mpgauss0,xmin,ymin);				// (x,y)の1が増えた分、mpgaussの値を調整
		if(xmin == xmax && ymin == ymax) break;
	}
#if defined(_DEBUG)
	if(1) {
		wchar_t		filename[FILENAME_MAX];
		// ブルーノイズパターンの保存
		wcscpy(filename,L"g:/tmp/mpmaskBN.bmp");
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
	if(1) {
		wchar_t		filename[FILENAME_MAX];
		// ガウスフィルタパターンの保存
		wcscpy(filename,L"g:/tmp/mpgauss0BN.bmp");
		int				scanlinesize = (mwidth / 4 + (mwidth % 4 > 0)) * 4;
		unsigned char*	pdata = (unsigned char*)malloc(scanlinesize * mheight);
		if(pdata) {
			memset(pdata,0,scanlinesize * mheight);
			for(int i = 0 ; i < mheight ; i++) {
				double* pd1 = mpgauss0 + mwidth * i;
				unsigned char* ps2 = pdata  + scanlinesize * i;
				for(int j = 0 ; j < mwidth ; j++,pd1++,ps2++) {
					int			i1 = (int)(*pd1 * 255.0 + 0.5);
					if(i1 > 255) i1 = 255;
					*ps2 = i1;
				}
			}
			msavebmp(filename,mwidth,mheight,8,scanlinesize,1,300,pdata);
			free(pdata);
		}
	}
	if(1) {
		wchar_t		filename[FILENAME_MAX];
		// ガウスフィルタパターンの保存
		wcscpy(filename,L"g:/tmp/mpgauss1BN.bmp");
		int				scanlinesize = (mwidth / 4 + (mwidth % 4 > 0)) * 4;
		unsigned char*	pdata = (unsigned char*)malloc(scanlinesize * mheight);
		if(pdata) {
			memset(pdata,0,scanlinesize * mheight);
			for(int i = 0 ; i < mheight ; i++) {
				double* pd1 = mpgauss1 + mwidth * i;
				unsigned char* ps2 = pdata  + scanlinesize * i;
				for(int j = 0 ; j < mwidth ; j++,pd1++,ps2++) {
					int			i1 = (int)(*pd1 * 255.0 + 0.5);
					if(i1 > 255) i1 = 255;
					*ps2 = i1;
				}
			}
			msavebmp(filename,mwidth,mheight,8,scanlinesize,1,300,pdata);
			free(pdata);
		}
	}
#endif
	return(0);
}
void CBNMask::mincdecgauss(int incflag,int zeroone,int width,int height,unsigned char* pmask,double* pgauss,int x,int y)
{
	int			tablesize2 = mgausstablesize * 2 - 1;
	for(int i = 0,y1 = y - mgausstablesize + 1 ; i < tablesize2 ; i++,y1++) {
		int		y2 = y1;
		if(y2 < 0) y2 += height;
		if(height <= y2) y2 -= height;
		double*	pd1 = pgauss + width * y2;
		double*	pd2 = mpgaussinc + tablesize2 * i;
		for(int j = 0,x1 = x - mgausstablesize + 1 ; j < tablesize2 ; j++,x1++,pd2++) {
			int		x2 = x1;
			if(x2 < 0) x2 += width;
			if(width <= x2) x2 -= width;
			if(incflag)
				pd1[x2] += *pd2;
			else
				pd1[x2] -= *pd2;
		}
	}
}
// 同じ値が複数ある場合はランダムにゲット
static int __cdecl stgausscomp(const void *elem1, const void *elem2)
{
	stPoint* p1 = (stPoint *)elem1;
	stPoint* p2 = (stPoint *)elem2;

	if(p1->d > p2->d)
		return(-1);
	else if(p1->d < p2->d)
		return(1);
	else
		return 0;
}
// 入力
//  int			revflag;		0...clusterサーチ
//								1...voidサーチ
//  int			deleteonly;		CBNMaskクラスでは無視

// 2009高速化の検討:この関数の高速化がすべての鍵を握る
int CBNMask::mcalcmaxgauss(int revflag,int deleteonly,int width,int height,unsigned char* pmask,double* pgauss,int& x,int& y,FILE* fp)
{
	int		total = width * height;
	int     i;
	for(i = 0 ; i < height ; i++) {
		stPoint*	p1 = mpgaussptr + width * i;
		double*		pd1 = pgauss + width * i;
		unsigned char*	ps1 = pmask + width * i;
		for(int j = 0 ; j < width ; j++,p1++,pd1++,ps1++) {
			p1->x = j;
			p1->y = i;
			p1->d = *pd1;
			if(revflag == 0 && *ps1 == 0) {
				p1->d = 0;
			}
			if(revflag == 1 && *ps1 == 1) {
				p1->d = 0;
			}
		}
	}
//	qsort(mpgaussptr,total,sizeof(stPoint),stgausscomp);
	mpartialsort(width,height,mpgaussptr);
	int		samenum;
	for(i = 0,samenum = 1 ; i < total ; i++) {
		if(i > 0) {
			stPoint* p1 = &mpgaussptr[i - 1];
			stPoint* p2 = &mpgaussptr[i];
			if(p1->d == p2->d) {
				samenum++;
			}
			else break;
		}
	}
	// 上位samenum個が同じ値
	int		pickup = (rand() % samenum);
	x = mpgaussptr[pickup].x;
	y = mpgaussptr[pickup].y;
#if defined(_DEBUG)
	if(fp) {
		fwprintf(fp,L"(%d,%d) = %1.18lf : %d/%d\n",x,y,mpgaussptr[pickup].d,pickup,samenum);
	}
#endif
	return(0);
}
// 部分ソート
// qsortに置き換えてvoid-and-cluster処理を高速化
void CBNMask::mpartialsort(int width,int height,stPoint* ptr)
{
	// 行内パーシャルソート
	int		i;
	for(i = 0 ; i < height ; i++) {
		stPoint* p1 = ptr + width * i;
		mpartialsortrow1(width,p1);
	}
	// 最大値探索
	int		imax = 0;
	double	dmax = ptr[0].d;
	for(i = 0 ; i < height ; i++) {
		stPoint* p1 = ptr + width * i;
		if(p1->d > dmax) {
			dmax = p1->d;
			imax = i;
		}
	}
	// 同率最大値探索
	for(i = 0 ; i < height ; i++) {
		stPoint* p1 = ptr + width * i;
		if(p1->d == dmax) {
			mpartialsortrow2(width,p1,dmax);
		}
	}
	// ソート
	// トリッキー：場合によっては重ね書きされそうだが大丈夫
	stPoint*		phead = ptr;
	stPoint*		ptail = ptr + width * height - 1;
	for(i = 0 ; i < height ; i++) {
		stPoint* p1 = ptr + width * i;
		for(int j = 0 ; j < width ; j++,p1++) {
			if(p1->d == dmax && p1 != phead) {
				stPoint		p2 = *phead;
				*phead = *p1;
				*ptail = p2;
				phead++;
				ptail--;
			}
			else
				break;
		}
	}
}
// 1行の最大値を先頭に
void CBNMask::mpartialsortrow1(int width,stPoint* ptr)
{
	int			jmax = 0;
	double		dmax = ptr[0].d;
	stPoint*	p1 = ptr;
	for(int j = 0 ; j < width ; j++,p1++) {
		if(p1->d > dmax) {
			dmax = p1->d;
			jmax = j;
		}
	}
	if(jmax != 0) {
		stPoint p2 = *ptr;
		*ptr = ptr[jmax];
		ptr[jmax] = p2;
	}
}
// 1行の最大値と同じ値を前詰めに
void CBNMask::mpartialsortrow2(int width,stPoint* ptr,double dmax)
{
	stPoint*		phead = ptr;
	stPoint*		ptail = ptr + width - 1;
	stPoint*		p1 = ptr;
	for(int j = 0 ; j < width ; j++,p1++) {
		if(p1->d == dmax) {
			if(p1 != phead) {
				stPoint		p2 = *phead;
				*phead = *p1;
				*ptail = p2;
				phead++;
				ptail--;
			}
		}
		else
			break;
	}
}
// 入力
//  int		flag;		1...*ps1が1の場合
//                      0...*ps1が0の場合
void CBNMask::mgauss2passy(int width,int height,unsigned char* pmask,int flag,double* pgauss)
{
// パス1↓方向
	for(int i = 0 ; i < height ; i++) {
		double* pd20 = pgauss + width * i;
		for(int j = 0 ; j < mgausstablesize ; j++) {
			int		k1 = i - j;
			int		k2 = i + j;
			if(k1 < 0) k1 += height;			// 循環畳み込み
			if(height <= k2) k2 -= height;	// 循環畳み込み
			if(j == 0) {
				unsigned char*	ps1 = pmask + width * i;
				double*			pd2  = pd20;
				for(int k = 0 ; k < width ; k++,ps1++,pd2++) {
					*pd2 = mpweighttable[0] * (*ps1 == flag);
				}
			}
			else {
				unsigned char*	ps1 = pmask + width * k1;
				double*			pd2 = pd20;
				int             k;
				for(k = 0 ; k < width ; k++,ps1++,pd2++) {
					*pd2 += mpweighttable[j] * (*ps1 == flag);
				}
				ps1  = pmask + width * k2;
				pd2  = pd20;
				for(k = 0 ; k < width ; k++,ps1++,pd2++) {
					*pd2 += mpweighttable[j] * (*ps1 == flag);
				}
			}
		}
	}
}
void CBNMask::mgauss2passx(int width,int height,double* pgaussy,double* pgaussx)
{
	// パス→方向
	for(int i = 0 ; i < height ; i++) {
		double*			pd2 = pgaussy + width * i;
		double*			pd3 = pgaussx  + width * i;
		for(int k = 0 ; k < width ; k++,pd3++) {
			for(int j = 0 ; j < mgausstablesize ; j++) {
				int		k1 = k - j;
				int		k2 = k + j;
				if(k1 < 0) k1 += width;
				if(width <= k2) k2 -= width;
				if(j == 0) {
					*pd3 = mpweighttable[j] * pd2[k1];
				}
				else {
					*pd3 += mpweighttable[j] * pd2[k1];
					*pd3 += mpweighttable[j] * pd2[k2];
				}
			}
		}
	}
}

// mpmatrixrawがあって、正規化mpmatrixが無い場合には作る(一瞬)
int CBNMask::mconvertmatrixraw()
{
	if(mpmatrixraw != NULL && mpmatrix == NULL) {
		mpmatrix = (unsigned int*)malloc(sizeof(unsigned int) * mwidth * mheight);
		if(mpmatrix == NULL) return(MEMORY_SHORTAGE);
		// mlevel = 元画像の階調数で正規化
		double		rate = (double)mlevel / (double)(mwidth * mheight);
		for(int i = 0 ; i < mheight ; i++) {
			unsigned int*	pi1 = mpmatrixraw + mwidth * i;
			unsigned int*	pi2 = mpmatrix    + mwidth * i;
			for(int j = 0 ; j < mwidth ; j++,pi1++,pi2++) {
				int th = (int)((double)*pi1 * rate + 0.5);
				if(th >= mlevel) th = mlevel - 1;
				*pi2 = th;
			}
		}
	}
	return(0);
}
// ブルーノイズマスクをブルーノイズ行列に変換(ordered dither法のマトリクスに相当)
int CBNMask::mask2matrix()
{
	int i1 = minitmask2matrix();
	if(i1 < 0) return(i1);
		i1 = mask2matrix1();
	if(i1 < 0) return(i1);
		i1 = mask2matrix23();
	if(i1 < 0) return(i1);
// mlevel = 元画像の階調数で正規化
	double		rate = (double)mlevel / (double)(mwidth * mheight);
	for(int i = 0 ; i < mheight ; i++) {
		unsigned int*	pi1 = mpmatrixraw + mwidth * i;
		unsigned int*	pi2 = mpmatrix    + mwidth * i;
		for(int j = 0 ; j < mwidth ; j++,pi1++,pi2++) {
			int th = (int)((double)*pi1 * rate + 0.5);
			if(th >= mlevel) th = mlevel - 1;
			*pi2 = th;
		}
	}
#if defined(_DEBUG)
	if(1) {
		wchar_t		filename[FILENAME_MAX];
		// ブルーノイズパターンの保存
		wcscpy(filename,L"g:/tmp/matrix001.bmp");
		int			scanlinesize = (mwidth / 32 + (mwidth % 32 > 0)) * 32 / 8;
		unsigned char*		pdata = (unsigned char*)malloc(scanlinesize * mheight);
		if(pdata) {
			memset(pdata,0,scanlinesize * mheight);
			for(int i = 0 ; i < mheight ; i++) {
				unsigned int*	pi1 = mpmatrix + mwidth * i;
				unsigned char*	ps2 = pdata  + scanlinesize * i;
				for(int j = 0 ; j < mwidth ; j++,pi1++) {
					if(0<= *pi1 && *pi1 <= 2) {
						ps2[j / 8] |= (0x80 >> (j & 7));
					}
				}
			}
			msavebmp(filename,mwidth,mheight,1,scanlinesize,0,300,pdata);
			free(pdata);
		}
	}
#endif
	return(0);
}

// 原論文フェーズ1
int CBNMask::minitmask2matrix()
{
	mpmatrixraw = (unsigned int*)malloc(sizeof(unsigned int) * mwidth * mheight);
	if(mpmatrixraw == NULL) return(MEMORY_SHORTAGE);
	memset(mpmatrixraw,0,sizeof(unsigned int) * mwidth * mheight);
	mpmatrix = (unsigned int*)malloc(sizeof(unsigned int) * mwidth * mheight);
	if(mpmatrix == NULL) return(MEMORY_SHORTAGE);
	memset(mpmatrix,0,sizeof(unsigned int) * mwidth * mheight);
	mpmask_phase1 = (unsigned char*)malloc(sizeof(unsigned char) * mwidth * mheight);
	if(mpmask_phase1 == NULL) return(MEMORY_SHORTAGE);
	memcpy(mpmask_phase1,mpmask,sizeof(unsigned char) * mwidth * mheight);
	mpgauss1_phase1 = (double*)malloc(sizeof(double) * mwidth * mheight);
	if(mpgauss1_phase1 == NULL) return(MEMORY_SHORTAGE);
	memcpy(mpgauss1_phase1,mpgauss1,sizeof(double) * mwidth * mheight);
	mpmask_phase23 = (unsigned char*)malloc(sizeof(unsigned char) * mwidth * mheight);
	if(mpmask_phase23 == NULL) return(MEMORY_SHORTAGE);
	memcpy(mpmask_phase23,mpmask,sizeof(unsigned char) * mwidth * mheight);
	mpgauss0_phase23 = (double*)malloc(sizeof(double) * mwidth * mheight);
	if(mpgauss0_phase23 == NULL) return(MEMORY_SHORTAGE);
	memcpy(mpgauss0_phase23,mpgauss0,sizeof(double) * mwidth * mheight);
	return(0);
}
int CBNMask::mask2matrix1()
{
	int		rank = mpixelnum - 1;
	FILE*		fp = NULL;
	while(rank >= 0) {
#if defined(_DEBUG)
		if(rank == 256) {
			fp = _wfopen(L"g:/tmp/mask2matrix1.log",L"wt");
		}
		if(rank <= 256) {
			if(fp) fwprintf(fp,L"rank = %d\n",rank);
		}
#endif
		// mpgauss0の最大値(cluster)を計算する
		// 同じ値が複数ある場合はランダムにゲット
		int		xmax,ymax;
		int i1 = mcalcmaxgauss(0,1,mwidth,mheight,mpmask_phase1,mpgauss1_phase1,xmax,ymax,fp);
		if(i1 < 0) return(i1);
		mpmask_phase1[mwidth * ymax + xmax] = 0;
// #if defined(_DEBUG)
#if 0
		int		i2 = mcountpixel(1,mpmask_phase1);
#endif
		mincdecgauss(0,1,mwidth,mheight,mpmask_phase1,mpgauss1_phase1,xmax,ymax);				// (x,y)の1が消えた分、mpgaussの値を調整
		mpmatrixraw[mwidth * ymax + xmax] = rank;
		rank--;
	}
	if(fp) {
		fclose(fp);
	}
	return(0);
}
int CBNMask::mask2matrix23()
{
	int			rank = mpixelnum;
	int total = mwidth * mheight;
	while(rank < total) {
		// mpgauss0の最大値(cluster)を計算する
		// 同じ値が複数ある場合はランダムにゲット
		int		xmin,ymin;
		int i1 = mcalcmaxgauss(1,1,mwidth,mheight,mpmask_phase23,mpgauss0_phase23,xmin,ymin);
		if(i1 < 0) return(i1);
// #if defined(_DEBUG)
#if 0
		int		i2 = mcountpixel(0,mpmask_phase23);
#endif
		mpmask_phase23[mwidth * ymin + xmin] = 1;
		mincdecgauss(0,0,mwidth,mheight,mpmask_phase23,mpgauss0_phase23,xmin,ymin);				// (x,y)の1が消えた分、mpgaussの値を調整
		mpmatrixraw[mwidth * ymin + xmin] = rank;
		rank++;
	}
	return(0);
}
// ブルーノイズマトリクスを作成する
// 入力
//	int		width;		マスク幅(単位ピクセル)
//	int		height;		マスク高さ(単位ピクセル)
//  int     level;      元画像の階調数
//  int     ws;         ガウスフィルタウィンドウサイズ(デフォルト値7(7x7のウィンドウ))
//  double  sigma;      ガウスフィルタの標準偏差(デフォルト値1.5)
// 戻り値
//  0....正常終了
//	負...エラー
int CBNMask::makebluenoisematrix(int width,int height,int level,int ws,double sigma)
{
	mclearbluenoisemask();
	mcleargauss();
	mclearbluenoisematrix();
	mclearweighttable();
	mclearchild();
	mwidth = width;
	mheight = height;
	mlevel = level;
	msigma = sigma;					// σ原論文では1.5位がベストらしい
	mgausstablesize = ws / 2 + 1;	// ガウス窓サイズ：原論文では7(便宜のため必ず奇数)
	// mloadbnmでロードしなかったとき
	if(mpmask == NULL) {
		// ホワイトノイズマスクを生成
		// mpmask上にホワイトマスクを生成
		int i1 = makewhitenoisemask(WHITETHRESHOLD);
		if(i1 < 0) return(i1);
	}
	// mloadbnmでロードしなかったとき
	if(mpgauss0 == NULL) {
		// void-and-cluster法でホワイトノイズマスクをブルーノイズマスクに変換
		int i1 = mvoidandcluster();
		if(i1 < 0) return(i1);
	}
	// blue noise maskからblue noise matrixに変換
	// mloadbnmでロードしなかったとき
	if(mpmatrixraw == NULL) {
		int i1 = mask2matrix();
		if(i1 < 0) return(i1);
	}
	return(0);
}
// 外部に保存してあるmatrixを設定
int CBNMask::msetbluenoisematrixraw(int width,int height,unsigned int pmatrixraw[256][256])
{
	mclearbluenoisemask();
	mcleargauss();
	mclearbluenoisematrix();
	mclearweighttable();
	mclearchild();
	mwidth = width;
	mheight = height;

	mpmatrixraw = (unsigned int*)malloc(sizeof(unsigned int) * mwidth * mheight);
	if(mpmatrixraw == NULL) return(MEMORY_SHORTAGE);
	for(int i = 0 ; i < height ; i++) {
		unsigned int* pj1 = mpmatrixraw + width * i;
		unsigned int* pj2 = pmatrixraw[i];
		memcpy(pj1, pj2, sizeof(unsigned int) * mwidth);
	}
	return(0);
}
