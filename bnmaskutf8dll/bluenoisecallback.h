///////////////////////////////////
// bluenosedll ライブラリコールバック宣言
#pragma once
#pragma pack(8)

#	ifdef __cplusplus
extern "C" {
#	endif
///////////////////////////////
// ブルーノイズ系列のシード設定
//  unsigned int		seed;		seed値   c標準ライブラリの  void srand(unsigned int seed); と同じ使い方
// 返り値
//　　負....エラー(INITIALIZE_ERROR...初期化エラー(クラスのインスタンス生成に失敗:メモリ不足))
typedef int (__stdcall* SBLUENOISE)(unsigned int seed);
extern SBLUENOISE sbluenoise;
/////////////////////////
// ブルーノイズ系列の取得(0～RAND_MAX)       c標準ライブラリの  int rand(void); と同じ使い方
// RAND_MAXは0x7fff(32767)なので粗い
// 返り値
//　　負....エラー(INITIALIZE_ERROR...初期化エラー(クラスのインスタンス生成に失敗:メモリ不足))
//　　ブルーノイズ系列
typedef int (__stdcall* BLUENOISE)();
extern BLUENOISE bluenoise;
/////////////////////////
// 精細なブルーノイズ系列の取得(0～size*size)   sizeはsetbluenoisematrixで設定した値
// ファイルを使わないサイズ(256*256)でも0～65535と細かい
//   256*  256       0～0x0000ffff(0～     65,536-1)      ファイルを使わない(ライブラリ内の256Kバイトの表)
//   512*  512       0～0x0003ffff(0～    262,144-1)　　     1Mバイト
//  1024* 1024       0～0x000fffff(0～  1,048,576-1)         4Mバイト
//  2048* 2048       0～0x003fffff(0～  4,194,304-1)        16Mバイト
//  4096* 4096       0～0x00ffffff(0～ 16,777,216-1)        64Mバイト
//  8192* 8192       0～0x03ffffff(0～ 67,108,864-1)       256Mバイト
// 16384*16384       0～0x0fffffff(0～268,435,456-1)      1024Mバイト(1Gバイト)
// 32768*32768       0～0x3fffffff(0～1,073,741,824-1)    4096Mバイト(4Gバイト)
// 返り値
//　　負....エラー(INITIALIZE_ERROR...初期化エラー(クラスのインスタンス生成に失敗:メモリ不足))
//　　ブルーノイズ系列
typedef int (__stdcall* BLUENOISEEX)();
extern BLUENOISEEX bluenoiseex;
/////////////////////////
// ブルーノイズマトリクスサイズの取得
// bluenoiseexは0～matrixsize() - 1の範囲の数値を返す
// 返り値
//　　負....現在のブルーノイズマトリクスサイズ
//　　ブルーノイズ系列
typedef int (__stdcall* GETMATRIXSIZE)();
extern GETMATRIXSIZE getmatrixsize;
/////////////////////////
// ブルーノイズ マトリクス保存・読込フォルダの設定、マトリクスサイズの設定
// 通常(0～RAND_MAX)の範囲のブルーノイズの生成では設定しない
// bluenoiseex()で精細なブルーノイズを取得する目的がある場合に設定する
// 入力
//    const char*	pfolder;	マトリクスファイルの存在する/を生成するフォルダ名
//                              NULLあるいは""を指す場合は256×256サイズの組み込みマトリクスを利用する
//    int           size;		マトリクスサイズ(512/1024/2048/4096/8192/16384)
//                              1024の場合、1024×1024のブルーノイズマトリクスを生成する。ファイルサイズは4メガバイト
// 返り値
//　　負....エラー(INITIALIZE_ERROR...初期化エラー(クラスのインスタンス生成に失敗:メモリ不足))
//　　負....エラー(FILE_OPEN_ERROR....保存できない、読み込みできない)
//                (MEMORY_SHORTAGE....メモリ不足)
// 指定したフォルダのsizexsizeraw.datというファイルを読み込む
//                   sizeが1024の場合は1024x1024raw.datというファイル名になる
//                   ファイルが無い場合はファイルを作成する
//                   sizeが大きくなるほど高品質のbluenoiseを生成する
typedef int (__stdcall* SETBLUENOISEMATRIX)(const wchar_t* pfolder,int size);
extern SETBLUENOISEMATRIX setbluenoisematrix;

// 1...組織的ディザユーティリティ
// 生成したマトリクスを使ってordered ditherを実行する
#ifdef BLUENOISEMBCS
typedef int (__stdcall* ORDEREDDITHER2)(int no,int width,int height,int scanlinesize1,unsigned char* pdata1,int scanlinesize2,unsigned char* pdata2);
#else
typedef int (__stdcall* ORDEREDDITHER2)(int no,int width,int height,int scanlinesize1,unsigned char* pdata1,int scanlinesize2,unsigned char* pdata2);
#endif
extern ORDEREDDITHER2 ordereddither2;

#ifdef BLUENOISEMBCS
typedef int (__stdcall* ORDEREDDITHER4)(int no,int width,int height,int scanlinesize1,unsigned char* pdata1,int scanlinesize2,unsigned char* pdata2);
#else
typedef int (__stdcall* ORDEREDDITHER4)(int no,int width,int height,int scanlinesize1,unsigned char* pdata1,int scanlinesize2,unsigned char* pdata2);
#endif
extern ORDEREDDITHER4 ordereddither4;

#ifdef BLUENOISEMBCS
typedef int (__stdcall* ORDEREDDITHERLINE2)(int no,int width,int y,unsigned char* pdata1,unsigned char* pdata2);
#else
typedef int (__stdcall* ORDEREDDITHERLINE2)(int no,int width,int y,unsigned char* pdata1,unsigned char* pdata2);
#endif
extern ORDEREDDITHERLINE2 orderedditherline2;

#ifdef BLUENOISEMBCS
typedef int (__stdcall* ORDEREDDITHERLINE4)(int no,int width,int y,unsigned char* pdata1,unsigned char* pdata2);
#else
typedef int (__stdcall* ORDEREDDITHERLINE4)(int no,int width,int y,unsigned char* pdata1,unsigned char* pdata2);
#endif
extern ORDEREDDITHERLINE4 orderedditherline4;

#ifdef BLUENOISEMBCS
typedef int (__stdcall* ORDEREDDITHER2WHITE)(int no,int width,int height,int scanlinesize1,unsigned char* pdata1,int scanlinesize2,unsigned char* pdata2);
#else
typedef int (__stdcall* ORDEREDDITHER2WHITE)(int no,int width,int height,int scanlinesize1,unsigned char* pdata1,int scanlinesize2,unsigned char* pdata2);
#endif
extern ORDEREDDITHER2WHITE ordereddither2white;

#ifdef BLUENOISEMBCS
typedef int (__stdcall* ORDEREDDITHER4WHITE)(int no,int width,int height,int scanlinesize1,unsigned char* pdata1,int scanlinesize2,unsigned char* pdata2);
#else
typedef int (__stdcall* ORDEREDDITHER4WHITE)(int no,int width,int height,int scanlinesize1,unsigned char* pdata1,int scanlinesize2,unsigned char* pdata2);
#endif
extern ORDEREDDITHER4WHITE ordereddither4white;

#ifdef BLUENOISEMBCS
typedef int (__stdcall* FLOYDSTEINBERG2)(int width,int height,int scanlinesize1,unsigned char* pdata1,unsigned char* pdata2);
#else
typedef int (__stdcall* FLOYDSTEINBERG2)(int width,int height,int scanlinesize1,unsigned char* pdata1,unsigned char* pdata2);
#endif
extern FLOYDSTEINBERG2 floydsteinberg2;

#ifdef BLUENOISEMBCS
typedef int (__stdcall* FLOYDSTEINBERG4)(int width,int height,int scanlinesize1,unsigned char* pdata1,unsigned char* pdata2);
#else
typedef int (__stdcall* FLOYDSTEINBERG4)(int width,int height,int scanlinesize1,unsigned char* pdata1,unsigned char* pdata2);
#endif
extern FLOYDSTEINBERG4 floydsteinberg4;

#	ifdef __cplusplus
}
#	endif
#pragma pack()
