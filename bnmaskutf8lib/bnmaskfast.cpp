/////////////////////////////////
// blue noiseマスク生成：高速化版
// bnmaskfast.cpp
// Blue Noise Mask Fast
// ブルーノイズ作成の性質上、登録データのランダム性が保障されているので単純な2分木
// でも平衡性が保たれる。それよりも、同順位のデータの大量発生に対応することが重要。
// 同順位のデータが大量にあると、平衡木でも平衡性が保たれない....さて、どうしましょうか
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <limits.h>
#include "bnmaskwcs.h"
#include "../errcode.h"


CBNMaskFast::CBNMaskFast()
{
	mproot0 = mproot1 = NULL;	// ルートノード
	mpnode = NULL;				// ノードインスタンス領域
	maxsamenum = 0;
}
CBNMaskFast::~CBNMaskFast()
{
	mclearnode();
}
void CBNMaskFast::mclearnode()
{
	if(mpnode) {
		free(mpnode);
		mpnode = NULL;
	}
	mproot0 = mproot1 = NULL;	// ルートノード
}
void CBNMaskFast::mclearwork()
{
	CBNMask::mclearwork();
	mclearnode();
}
// 二分木作成
// 入力
//  unsigned char*		pmask;		// マスク
//	double*				pdata1;		// 1に対応するガウス画像
//	double*				pdata0;		// 0に対応するガウス画像
int CBNMaskFast::maketree(unsigned char* pmask,double* pdata1,double* pdata0)
{
	mclearnode();
	// ノードインスタンスをmwidth * mheight個確保
	// 大量のメモリ(4k×4kで256M(32bit)/8k×8kで1.5G(64bit)/16k×16kで6G)なので注意
	size_t		width = mwidth;
	size_t		height = mheight;
	size_t		pixelnum = width * height;
	mpnode = (stBNode *)malloc(sizeof(stBNode) * pixelnum);
	if(mpnode == NULL) return(MEMORY_SHORTAGE);
	memset(mpnode,0,sizeof(stBNode) * pixelnum);
	// インスタンスデータを埋める
	for(int i = 0 ; i < mheight ; i++) {
		unsigned char*	ps1 = pmask  + mwidth * i;
		double*			pd1 = pdata1 + mwidth * i;
		double*			pd0 = pdata0 + mwidth * i;
		stBNode*		pnode = mpnode + mwidth * i;
		for(int j = 0 ; j < mwidth ; j++,ps1++,pd1++,pd0++,pnode++) {
			if(*ps1 == 1) {
				pnode->d = *pd1;
				// １木に登録
				minsertnode(&mproot1,pnode);
			}
			else {
				pnode->d = *pd0;
				// ０木に登録
				minsertnode(&mproot0,pnode);
			}
		}
	}
#if defined(_DEBUG)
	int		depth1 = mgettreedepth(mproot1);
	int		depth0 = mgettreedepth(mproot0);
#endif
	return(0);
}
// 再帰処理は最適化しそこなうと遅くなるので、繰り返し処理に書き換える
// ノード挿入：必ず葉(NULL値)に置き換わる
void CBNMaskFast::minsertnode(stBNode** pproot,stBNode* pnode)
{
	while(*pproot != NULL) {
		if((*pproot)->d <= pnode->d) {
			pproot = &(*pproot)->pright;
		}
		else {
			pproot = &(*pproot)->pleft;
		}
	}
	*pproot = pnode;
	pnode->pleft = pnode->pright = NULL;
}
// 再帰処理は最適化しそこなうと遅くなるので、繰り返し処理に書き換える
// ポインタの書き換えで処理することで高速化
// ノード削除
// 戻り値
//  0....pnodeが見つからなかった
//  1....pnodeが見つかった
// ２つ子供を持つ場合
//          pproot                           pproot
//            10             10を削除          12
//          ／  ＼                           ／  ＼
//         9     14                         9      14
//             ／  ＼                            ／  ＼
//    pprootmin      --                  pprootmin     --
//           12      16                         13     16
//             ＼
//               13
//  右の枝の最小値ノードを探し現在のノードと入れ替える。
//  右の枝の最小値ノードを削除する
//  右の枝の右の子は継承
//          pproot                           pproot
//            10             10を削除          14
//          ／  ＼                           ／  ＼
//        --     pprrootmin                --      pprootmin
//         9     14                         9      16
//                 ＼
//                   16
//          pproot                           pproot
//            10             10を削除          14
//          ／  ＼                           ／  ＼
//        --     pprrootmin                --      pprootmin
//         9     14                         9      NULL
int CBNMaskFast::mdeletenode(stBNode** pproot,stBNode* pnode)
{
	while(*pproot != NULL) {
		// 削除ノードが見つかった→削除処理
		if(*pproot == pnode) {
			// 右ノードのみ
			if((*pproot)->pleft == NULL) {
				*pproot = (*pproot)->pright;
			}
			// 左ノードのみ
			else if((*pproot)->pright == NULL) {
				*pproot = (*pproot)->pleft;
			}
			// 削除ノードが子供を２つ持つ場合
			else {
				// 削除ノードの右の子から最小値ノードpprootmin探索
				stBNode** pprootmin = &(*pproot)->pright;
				while((*pprootmin)->pleft != NULL) {
					pprootmin = &(*pprootmin)->pleft;
				} while((*pprootmin)->pleft != NULL);
				// 現在のノードと最小値ノードを入れ替え
				stBNode*	pleft  = (*pproot)->pleft;
				stBNode*	pright = (*pproot)->pright;
				*pproot = *pprootmin;
				stBNode*	prightmin = (*pprootmin)->pright;
				// proot==pnodeのリンクを受け継ぐ
				if((*pproot) != pleft) (*pproot)->pleft  = pleft;
				if((*pproot) != pright) (*pproot)->pright = pright;
				// 最小値ノードを削除==(pleftはNULL)のprightを上昇
				*pprootmin = prightmin;
			}
// #if defined(_DEBUG)
#if 0
			if(mcheckloopnode()) {
				int foo = 0;
			}
#endif
			return(1);
		}
		else if((*pproot)->d <= pnode->d) {
			pproot = &(*pproot)->pright;
		}
		else {
			pproot = &(*pproot)->pleft;
		}
	}
	return(0);		// 見つからなかった
}
// for _DEBUG only
// ループノードのチェック
int CBNMaskFast::mcheckloopnode()
{
#if defined(_DEBUG)
	for(int i = 0 ; i < mheight ; i++) {
		stBNode* pnode = mpnode + mwidth * i;
		for(int j = 0 ; j < mwidth ; j++,pnode++) {
			if(pnode == pnode->pright) {
				return(1);
			}
			if(pnode == pnode->pleft) {
				return(1);
			}
		}
	}
#endif
	return(0);
}
// 同順位の最大値データを巡回収集
// 高速化のためスタッティックな領域に収集する
// moverlayrand()によって、同順位データはめったに発生しないようになっているが、
// MAX_SAMENUMは大きめにとってある。
int CBNMaskFast::mtraversemaxdata(stBNode* proot)
{
	double		dmax = INT_MIN;
	while(proot != NULL) {
		if(dmax < proot->d) {
			dmax = proot->d;
			msamenum = 0;
			mptraverse[msamenum] = proot;
			msamenum++;
		}
		else {
			if(msamenum == MAX_SAMENUM) {
				return(BUFFER_OVERFLOW);
			}
			mptraverse[msamenum] = proot;
			msamenum++;
#if defined(_DEBUG)
			if(maxsamenum < msamenum) {
				maxsamenum = msamenum;
			}
#endif
		}
		proot = proot->pright;
	}
	return(0);
}
// デバッグ用途
// 深さを返す
int CBNMaskFast::mgettreedepth(stBNode* proot)
{
	if(proot == NULL) return(0);
	else {
		int left = mgettreedepth(proot->pleft);
		int right = mgettreedepth(proot->pright);
		return (left > right ? left + 1 : right + 1);
	}
}
// オーバーライド仮想関数
// 2009高速化の検討:初期化部分で関係ない
int CBNMaskFast::minitvoidandcluster()
{
	int i1 = CBNMask::minitvoidandcluster();
	if(i1 < 0) return(i1);
	// ランダム成分オーバーレイ
	// マスク→マトリクス変換の過程で、同順位のノードが大量発生する件に対応するため
	moverlayrand(mpgauss1,mpgauss0);
	// 二分木作成
	i1 = maketree(mpmask,mpgauss1,mpgauss0);
	if(i1 < 0) return(i1);
	return(0);
}
// マスク→マトリクス変換の過程で、同順位のノードが大量発生する件に対応するため
// 1...上記過程の最後の方で、"1"あるいは"0"が少なくなる(ガウス窓のサイズでの領域が重ならなくなる)
//     時に等値の最大値が大量発生する。
//     たとえば1024×1024でガウス窓が9×9の場合、残り10,000個くらいになると、等値の最大値が数千個となる。
//     Treap/赤黒木/AVL木のような平衡木の手法を使っても等値が大量にある場合は木のバランスが大きく崩れる。
// 2...上記の問題を解決するために、微小なホワイトノイズ成分をローパスフィルタ画像に加算して、等値を少な
//     くする。本来のvoid and cluster法に影響しないような微小値として
//     rand()値(0～32767) * 1.0e-18 0.000 000 000 000 000 001 ～ 0.000 000 000 000 032 767を採用
//     加算値は少し大きすぎるかもしれないので要チェック
void CBNMaskFast::moverlayrand(double* pgauss1,double* pgauss0)
{
	for(int i = 0 ; i < mheight ; i++) {
		double* pd1 = pgauss1 + mwidth * i;
		double*	pd0 = pgauss0 + mwidth * i;
		for(int j = 0 ; j < mwidth ; j++,pd1++,pd0++) {
			double r1 = (double)rand() * 1.e-18;
			*pd1 += r1;
			*pd0 += r1;
		}
	}
}
// 入力
//  int			revflag;		0...clusterサーチ
//								1...voidサーチ
//  int			deleteonly;		0...削除＋挿入
//                              1...削除のみ
// 2009高速化の検討:この関数の高速化がすべての鍵を握る
int CBNMaskFast::mcalcmaxgauss(int revflag,int deleteonly,int width,int height,unsigned char* pmask,double* pgauss,int& x,int& y,FILE* fp)
{
	// 最大値取得
	stBNode* pmax = NULL;
	if(revflag == 0) {
		// 同順位のデータを巡回収集
		msamenum = 0;
		int i1 = mtraversemaxdata(mproot1);
		if(i1 < 0) return(i1);
		// msamenum個が同じ値
		int		pickup = (rand() % msamenum);
		stBNode* p1 = mptraverse[pickup];
		size_t		offset = p1 - mpnode;
		x = (int)(offset % mwidth);						// x,yは実質16384までなので無問題
		y = (int)(offset / mwidth);
		// mpmask[mwidth * ymax + xmax] = 0;
		// １木から要素p1を削除
		i1 = mdeletenode(&mproot1,p1);
		if(i1 == 0) {
			int foo = 0;		// ありえない
		}
		// ０木に要素p1を挿入
		if(deleteonly == 0) {
			minsertnode(&mproot0,p1);
		}
	}
	else {
		// 同順位のデータを巡回収集
		msamenum = 0;
		int i1 = mtraversemaxdata(mproot0);
		if(i1 < 0) return(i1);
		// msamenum個が同じ値
		int		pickup = (rand() % msamenum);
		stBNode* p1 = mptraverse[pickup];
		size_t		offset = p1 - mpnode;
		x = (int)(offset % mwidth);						// x,yは実質16384までなので無問題
		y = (int)(offset / mwidth);
		// mpmask[mwidth * ymax + xmax] = 1;
		// ０木から要素p1を削除
		i1 = mdeletenode(&mproot0,p1);
		if(i1 == 0) {
			int foo = 0;		// ありえない
		}
		// １木に要素p1を挿入
		if(deleteonly == 0) {
			minsertnode(&mproot1,p1);
		}
	}
	return(0);
}
void CBNMaskFast::mincdecgauss(int incflag,int zeroone,int width,int height,unsigned char* pmask,double* pgauss,int x,int y)
{
	int			tablesize2 = mgausstablesize * 2 - 1;
	for(int i = 0,y1 = y - mgausstablesize + 1 ; i < tablesize2 ; i++,y1++) {
		int		y2 = y1;
		if(y2 < 0) y2 += height;
		if(height <= y2) y2 -= height;
		double*			pd1 = pgauss + width * y2;
		double*			pd2 = mpgaussinc + tablesize2 * i;
		stBNode*		pnode1 = mpnode + width * y2;
		unsigned char*	ps1 = pmask + width * y2;
		for(int j = 0,x1 = x - mgausstablesize + 1 ; j < tablesize2 ; j++,x1++,pd2++) {
			int		x2 = x1;
			if(x2 < 0) x2 += width;
			if(width <= x2) x2 -= width;
			if(incflag)
				pd1[x2] += *pd2;
			else
				pd1[x2] -= *pd2;
			// ０木に対する操作限定
			if(zeroone == 0) {
				if(ps1[x2] == 0) {
					stBNode* ptarget = pnode1 + x2;
					mdeletenode(&mproot0,ptarget);
					ptarget->d = pd1[x2];
					minsertnode(&mproot0,ptarget);
				}
			}
			// １木に対する操作限定
			else {
				if(ps1[x2] == 1) {
					stBNode* ptarget = pnode1 + x2;
					mdeletenode(&mproot1,ptarget);
					ptarget->d = pd1[x2];
					minsertnode(&mproot1,ptarget);
				}
			}
		}
	}
}
// 原論文フェーズ1,2,3
int CBNMaskFast::minitmask2matrix()
{
	int i1 = CBNMask::minitmask2matrix();
	if(i1 < 0) return(i1);
	// 二分木作成
	i1 = maketree(mpmask_phase1,mpgauss1_phase1,mpgauss0_phase23);
	if(i1 < 0) return(i1);
	return(0);
}
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
int CBNMaskFast::mseparatebluenoisematrix(int componentnum)
{
	if(mpmatrixraw == NULL) return(INITIALIZE_ERROR);
	mclearchild();
	if(componentnum <= 1) return(0);
	mppchild = (CBNMask**)malloc(sizeof(CBNMask*) * componentnum);
	if(mppchild == NULL) return(MEMORY_SHORTAGE);
	memset(mppchild,0,sizeof(CBNMask*) * componentnum);
	mcomponentnum = componentnum;
	for(int i = 0 ; i < componentnum ; i++) {
		mppchild[i] = new CBNMaskFast();
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
