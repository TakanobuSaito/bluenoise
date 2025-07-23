///////////////
// INTERBLUENOISEクラス
// C言語APIとCBNMASKクラスの橋渡し
#pragma once

#include "stdafx.h"
#include <stdio.h>
#include "../errcode.h"

class CBNMask;
class CBNMaskFast;

class CInterBluenoise {
public:
	CInterBluenoise();
	~CInterBluenoise();
private:
	unsigned int	mptr;				// 出力するブルーノイズの要素
	unsigned int	mwidth,mheight,msize;
	wchar_t			matrixfolder[FILENAME_MAX];
	CBNMaskFast*	mpbn;
	void			mclearbluenoise();
	int				minitbluenoise();
public:
	// mwidth mheight msizeの取得
	inline int		mgetwidth()
	{
		return mwidth;
	}
	inline int		mgetheight()
	{
		return mheight;
	}
	inline int		mgetmatrixsize()
	{
		return msize;
	}

	// seedの設定
	void			msetseed(unsigned int seed);
	// 0～RAND_MAXの範囲のbluenoise数列の取得
	int				mgetbluenoise();
	// 0～size*sizeの範囲のbluenoise数列の取得
	int				mgetbluenoiseex();
	// bluenoiseフォルダ、サイズの設定
	int				msetmatrixfolder(const wchar_t* pfolder,int size);

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
	int				mordereddither2(int no,int width,int height,int scanlinesize1,unsigned char* pdata1,
							int scanlinesize2,unsigned char* pdata2);
	int				mordereddither4(int no,int width,int height,int scanlinesize1,unsigned char* pdata1,
							int scanlinesize2,unsigned char* pdata2);
	// 2...組織的ディザユーティリティ
	//     1行単位で処理するバージョン
	// 入力
	//   int            y;               行番号
	int				morderedditherline2(int no,int width,int y,unsigned char* pdata1,unsigned char* pdata2);
	int				morderedditherline4(int no,int width,int y,unsigned char* pdata1,unsigned char* pdata2);

	// 3...比較用：ホワイトノイズによる組織的ディザユーティリティ
	//     1行単位で処理するバージョン
	// 入力
	int				mordereddither2white(int no,int width,int height,int scanlinesize1,unsigned char* pdata1,
							int scanlinesize2,unsigned char* pdata2);
	int				mordereddither4white(int no,int width,int height,int scanlinesize1,unsigned char* pdata1,
							int scanlinesize2,unsigned char* pdata2);

	// 4...比較用：Floyd & Steinberg方式による誤差拡散法ディザユーティリティ
	//     プレーンnoはサポートしていない
	//     pdata2の scanlinesize は、強制的に width となるので注意
	int				mfloydsteinberg2(int width,int height,int scanlinesize1,unsigned char* pdata1,unsigned char* pdata2);
	int				mfloydsteinberg4(int width,int height,int scanlinesize1,unsigned char* pdata1,unsigned char* pdata2);


};
