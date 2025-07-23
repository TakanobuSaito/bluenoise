/////////////////////////////
// bnmaskutf8.dll テストプログラム
#include <malloc.h>
#include <stdio.h>
#include <memory.h>
#include <math.h>

// #define DYNAMIC_LINK

#if !defined(DYNAMIC_LINK)
#	include "bnmaskutf8lib/bnmaskwcs.h"
#else
#	include <windows.h>
#	include "bnmaskutf8dll/bluenoisecallback.h"
#endif
#include "errcode.h"

#include "bmpsupport.h"

#define		SEED 0		// 乱数の種

static	unsigned char bit_on_table[256] = {
			0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,	// 0x00 -> 0x0f
			1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,	// 0x10 -> 0x1f
			1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,	// 0x20 -> 0x2f
			2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,	// 0x30 -> 0x3f
			1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,	// 0x40 -> 0x4f
			2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,	// 0x50 -> 0x5f
			2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,	// 0x60 -> 0x6f
			3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,	// 0x70 -> 0x7f
			1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,	// 0x80 -> 0x8f
			2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,	// 0x90 -> 0x9f
			2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,	// 0xa0 -> 0xaf
			3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,	// 0xb0 -> 0xbf
			2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,	// 0xc0 -> 0xcf
			3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,	// 0xd0 -> 0xdf
			3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,	// 0xe0 -> 0xef
			4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8		// 0xf0 -> 0xff
};
// 背景ビット判定
// bit 1が半分以上の場合に1を返す。
// RV変換では1を返した場合は、前景が0/背景が1
int gcheckbackground(int width,int scanlinesize, int height, unsigned char* pdata)
{
	unsigned long	bitcount = 0UL;
	for(int i = 0 ; i < height ; i++) {
		unsigned char* ps1 = pdata + scanlinesize * i;
		int		j;
		for(j = 0 ; j < scanlinesize - 1 ; j++,ps1++) {
			bitcount += bit_on_table[*ps1];
		}
		for(int k = j * 8 ; k < width ; k++) {
			if(*ps1 & (0x80 >> (k & 7))) {
				bitcount++;
			}
		}
	}
	return (bitcount > (unsigned long)(width * height / 2));
}
// 白黒反転
void greverse(int width,int scanlinesize, int height, unsigned char* pdata)
{
	for(int i = 0 ; i < height ; i++) {
		unsigned char* ps1 = pdata + scanlinesize * i;
		for(int j = 0 ; j < scanlinesize ; j++,ps1++) {
			*ps1 ^= 0xff;
		}
	}
}

#if defined(DYNAMIC_LINK)

HINSTANCE bn_dllinstance;			// dllインスタンス
// DLL エントリーポイント
// ブルーノイズ系列のシード設定
SBLUENOISE sbluenoise;
// ブルーノイズ系列の取得(0～RAND_MAX)       c標準ライブラリの  int rand(void); と同じ使い方
BLUENOISE bluenoise;
// 精細なブルーノイズ系列の取得(0～size*size)   sizeはsetbluenoisematrixで設定した値
// ファイルを使わないサイズ(256*256)でも0～65535と細かい
BLUENOISEEX bluenoiseex;
// ブルーノイズマトリクスサイズの取得
// bluenoiseexは0～matrixsize() - 1の範囲の数値を返す
GETMATRIXSIZE getmatrixsize;
// ブルーノイズ マトリクス保存・読込フォルダの設定、マトリクスサイズの設定
// 通常(0～RAND_MAX)の範囲のブルーノイズの生成では設定しない
// bluenoiseex()で精細なブルーノイズを取得する目的がある場合に設定する
SETBLUENOISEMATRIX setbluenoisematrix;

// 1...組織的ディザユーティリティ
ORDEREDDITHER2 ordereddither2;
ORDEREDDITHER4 ordereddither4;

ORDEREDDITHERLINE2 orderedditherline2;
ORDEREDDITHERLINE4 orderedditherline4;

ORDEREDDITHER2WHITE ordereddither2white;
ORDEREDDITHER4WHITE ordereddither4white;

FLOYDSTEINBERG2 floydsteinberg2;
FLOYDSTEINBERG4 floydsteinberg4;

int main()
{
	HINSTANCE bn_dllinstance;			// dllインスタンス
#if defined(_DEBUG)
	bn_dllinstance = LoadLibrary(L"d:/ocr/src/bnmaskutf8/x64/Debug/bnmaskutf8dll.dll");
#else
	bn_dllinstance = LoadLibrary(L"bnmaskutf8dll.dll");
#endif
	if(bn_dllinstance == NULL) {
		fwprintf(stderr,L"DLLﾌｧｲﾙ bnmaskutf8dll.dll のﾛｰﾄﾞに失敗しました\n");
		return(0);		
	}
	sbluenoise = (SBLUENOISE)GetProcAddress(bn_dllinstance,"sbluenoise");
	bluenoise = (BLUENOISE)GetProcAddress(bn_dllinstance,"bluenoise");
	bluenoiseex = (BLUENOISEEX)GetProcAddress(bn_dllinstance,"bluenoiseex");
	getmatrixsize = (GETMATRIXSIZE)GetProcAddress(bn_dllinstance,"getmatrixsize");
	setbluenoisematrix = (SETBLUENOISEMATRIX)GetProcAddress(bn_dllinstance,"setbluenoisematrix");
	if(sbluenoise == NULL || bluenoise == NULL || bluenoiseex == NULL || getmatrixsize == NULL || setbluenoisematrix == NULL) {
		fwprintf(stderr,L"DLLﾌｧｲﾙ bnmaskutf8dll.dll のﾊﾞｰｼﾞｮﾝが異なります(1)\n");
		FreeLibrary(bn_dllinstance);
		return(1);		
	}
	ordereddither2 = (ORDEREDDITHER2)GetProcAddress(bn_dllinstance,"ordereddither2");
	ordereddither4 = (ORDEREDDITHER2)GetProcAddress(bn_dllinstance,"ordereddither4");
	orderedditherline2 = (ORDEREDDITHERLINE2)GetProcAddress(bn_dllinstance,"orderedditherline2");
	orderedditherline4 = (ORDEREDDITHERLINE4)GetProcAddress(bn_dllinstance,"orderedditherline4");
	if(ordereddither2 == NULL || ordereddither4 == NULL || orderedditherline2 == NULL || orderedditherline4 == NULL) {
		fwprintf(stderr,L"DLLﾌｧｲﾙ bnmaskutf8dll.dll のﾊﾞｰｼﾞｮﾝが異なります(1)\n");
		FreeLibrary(bn_dllinstance);
		return(1);		
	}
	ordereddither2white = (ORDEREDDITHER2)GetProcAddress(bn_dllinstance,"ordereddither2white");
	ordereddither4white = (ORDEREDDITHER2)GetProcAddress(bn_dllinstance,"ordereddither4white");
	floydsteinberg2 = (FLOYDSTEINBERG2)GetProcAddress(bn_dllinstance,"floydsteinberg2");
	floydsteinberg4 = (FLOYDSTEINBERG4)GetProcAddress(bn_dllinstance,"floydsteinberg4");
	if(ordereddither2white == NULL || ordereddither4white == NULL || floydsteinberg2 == NULL || floydsteinberg4 == NULL) {
		fwprintf(stderr,L"DLLﾌｧｲﾙ bnmaskutf8dll.dll のﾊﾞｰｼﾞｮﾝが異なります(1)\n");
		FreeLibrary(bn_dllinstance);
		return(1);		
	}
	// C言語のrand() 0～RAND_MAXまでの整数を返す/srand(unsigned int seed)関数互換
	sbluenoise(SEED);		// seed = SEED
	for(int i = 0 ; i < 240 ; i++) {
		int ival = bluenoise();
		printf("bn[%03d] = %05d ", i, ival);
		if(i % 6 == 5) puts("");
	}

	printf("\n----------------------------------------------------------------------------------\n");
	// 0～RAND_MAXではなく、0～getmatrixsize()-1までの整数を返す(
	// 標準の256×256では、0～2^16-1(65535)
	// 1024×1024のブルーノイズマトリクスを使えば、0～2^20-1(1,048,576)の範囲の整数が返る
	// 32768×32768のブルーノイズマトリクスを使えば、0～2^30-1(1,073,741,823)の範囲の整数が返る
	sbluenoise(SEED);		// seed = SEED
	for(int i = 0 ; i < 240 ; i++) {
		int ival = bluenoiseex();
		printf("bn[%03d] = %05d ", i, ival);
		if(i % 6 == 5) puts("");
	}

	// 256×256以外のマトリクス(512×512)の生成
	int i1 = setbluenoisematrix(L"./test/", 512);
	if(i1 < 0) {
		FreeLibrary(bn_dllinstance);
		return(i1);
	}

	// 0～getmatrixsize() - 1までの整数を0.0～1.0にマッピングする
	int		amatrixsize = getmatrixsize();
	sbluenoise(SEED);		// seed = SEED
	for(int i = 0 ; i < 240 ; i++) {
		int ival = bluenoiseex();
		printf("bn[%03d] = %0.6lf ", i, (double)ival / (double)(amatrixsize - 1));
		if(i % 6 == 5) puts("");
	}

	// 画像バッファ、誤差拡散法のワークバッファの確保＋画像の読み込み
	//初期設定ファイル名
	int		rawwidth,rawheight,depth,rawscanlinesize,dpi,photometric,topdownflag;
	i1 = gopenbmp("./test/umigray.bmp",rawwidth,rawheight,depth,rawscanlinesize,photometric,dpi,topdownflag);
	if(i1 < 0) return(i1);
	if(depth != 8) {
		fprintf(stderr,"this test program is for gray scale image");
		FreeLibrary(bn_dllinstance);
		return(ILLEGAL_GRAPHIC);
	}
	unsigned char* prawdata = (unsigned char*)malloc(rawscanlinesize * rawheight);
	if(prawdata == NULL) return(MEMORY_SHORTAGE);
	i1 = gloadbmp(rawwidth,rawscanlinesize,rawheight,topdownflag,prawdata);
	if(gfp) {
		fclose(gfp);
	}
	if(i1 < 0) {
		free(prawdata);
		FreeLibrary(bn_dllinstance);
		return(i1);
	}
	unsigned char* pdata = (unsigned char*)malloc(rawwidth * rawheight);
	if(pdata == NULL) {
		free(prawdata);
		FreeLibrary(bn_dllinstance);
		return(MEMORY_SHORTAGE);
	}

	// white noiseによるordered dither
	// 2 level
	ordereddither2white(0,rawwidth,rawheight,rawscanlinesize, prawdata, rawwidth, pdata);
	gsavebmp(L"./test/umigranWN2dll.bmp",rawwidth,rawheight,8,rawscanlinesize,0,300,pdata);
	// 4 level
	ordereddither4white(0,rawwidth,rawheight,rawscanlinesize, prawdata, rawwidth, pdata);
	gsavebmp(L"./test/umigranWN4dll.bmp",rawwidth,rawheight,8,rawscanlinesize,0,300,pdata);

	// blue noiseによるordered dither
	// 2 level
	ordereddither2(0,rawwidth,rawheight,rawscanlinesize, prawdata, rawwidth, pdata);
	gsavebmp(L"./test/umigranBN2dll.bmp",rawwidth,rawheight,8,rawscanlinesize,0,300,pdata);
	// 4 level
	ordereddither4(0,rawwidth,rawheight,rawscanlinesize, prawdata, rawwidth, pdata);
	gsavebmp(L"./test/umigranBN4dll.bmp",rawwidth,rawheight,8,rawscanlinesize,0,300,pdata);

	// error diffusion(Floyed & Steinberg)によるordered dither
	// 2 level
	i1 = floydsteinberg2(rawwidth, rawheight, rawscanlinesize, prawdata, pdata);
	if(i1 < 0) {
		free(pdata);
		free(prawdata);
		FreeLibrary(bn_dllinstance);
		return(i1);
	}
	gsavebmp(L"./test/umigranFS2dll.bmp",rawwidth,rawheight,8,rawscanlinesize,0,300,pdata);
	// 2 level
	i1 = floydsteinberg4(rawwidth, rawheight, rawscanlinesize, prawdata, pdata);
	if(i1 < 0) {
		free(pdata);
		free(prawdata);
		FreeLibrary(bn_dllinstance);
		return(i1);
	}
	gsavebmp(L"./test/umigranFS4dll.bmp",rawwidth,rawheight,8,rawscanlinesize,0,300,pdata);

	free(prawdata);
	free(pdata);
	FreeLibrary(bn_dllinstance);
	return(0);
}

#else

int main()
{
	//初期設定ファイル名
	int		rawwidth,rawheight,depth,rawscanlinesize,dpi,photometric,topdownflag;
	int		i1 = gopenbmp("./test/umigray.bmp",rawwidth,rawheight,depth,rawscanlinesize,photometric,dpi,topdownflag);
	if(i1 < 0) return(i1);
	if(depth != 8) {
		fprintf(stderr,"this test program is for gray scale image");
		return(ILLEGAL_GRAPHIC);
	}
	unsigned char* prawdata = (unsigned char*)malloc(rawscanlinesize * rawheight);
	if(prawdata == NULL) return(MEMORY_SHORTAGE);
	i1 = gloadbmp(rawwidth,rawscanlinesize,rawheight,topdownflag,prawdata);
	if(gfp) {
		fclose(gfp);
	}
	if(i1 < 0) {
		free(prawdata);
		return(i1);
	}

	SYSTEMTIME			t1,t2;
	int		mili256,mili512,mili1024,mili2048,mili4096,mili8192;
	// スタティックリンク版では
	// (1)256×256のmatrixを作成する
	// (2)相関のある(乱数)系列=blue noise 系列取得(そのもののAPIは無いのでプログラムを構成)
	// (3)2/4 level whitenoise/bluenoise/error diffusion画像、合計6枚を作成するのみ
	CBNMaskFast* pbn = new CBNMaskFast();       // コンストラクタ
	// 256×256のmatrix生成
	int		width = 256;
	int		height = 256;
	::GetSystemTime(&t1);
	i1 = pbn->makebluenoisematrix(width,height);
	::GetSystemTime(&t2);
	mili256 = (t2.wHour - t1.wHour) * 3600 * 1000 + (t2.wMinute - t1.wMinute) * 60 * 1000 + (t2.wSecond - t1.wSecond) * 1000 + (t2.wMilliseconds - t1.wMilliseconds);
	if(i1 < 0) {
		free(prawdata);
		delete pbn;
		return(i1);
	}
	// matrix保存
	wchar_t	amatrixfilepath[FILENAME_MAX];
	_swprintf(amatrixfilepath,L"./test/bnmatrix%d_%d.dat",width,height);
	i1 = pbn->msavebnmatrixraw(amatrixfilepath);
	if(i1 < 0) {
		free(prawdata);
		delete pbn;
		return(i1);
	}
	delete pbn;                    // ディストラクタ

// for time measurement
#if 0
	// 512x512のマトリクス作成
	pbn = new CBNMaskFast();       // コンストラクタ
	// 512×512のmatrix生成
	width = 512;
	height = 512;
	::GetSystemTime(&t1);
	i1 = pbn->makebluenoisematrix(width,height);
	::GetSystemTime(&t2);
	mili512 = (t2.wHour - t1.wHour) * 3600 * 1000 + (t2.wMinute - t1.wMinute) * 60 * 1000 + (t2.wSecond - t1.wSecond) * 1000 + (t2.wMilliseconds - t1.wMilliseconds);
	if(i1 < 0) {
		free(prawdata);
		delete pbn;
		return(i1);
	}
	// matrix保存
	_swprintf(amatrixfilepath,L"./test/bnmatrix%d_%d.dat",width,height);
	i1 = pbn->msavebnmatrixraw(amatrixfilepath);
	if(i1 < 0) {
		free(prawdata);
		delete pbn;
		return(i1);
	}
	delete pbn;                    // ディストラクタ

	pbn = new CBNMaskFast();       // コンストラクタ
	// 1024x1024のmatrix生成
	width = 1024;
	height = 1024;
	::GetSystemTime(&t1);
	i1 = pbn->makebluenoisematrix(width,height);
	::GetSystemTime(&t2);
	mili1024 = (t2.wHour - t1.wHour) * 3600 * 1000 + (t2.wMinute - t1.wMinute) * 60 * 1000 + (t2.wSecond - t1.wSecond) * 1000 + (t2.wMilliseconds - t1.wMilliseconds);
	if(i1 < 0) {
		free(prawdata);
		delete pbn;
		return(i1);
	}
	// matrix保存
	_swprintf(amatrixfilepath,L"./test/bnmatrix%d_%d.dat",width,height);
	i1 = pbn->msavebnmatrixraw(amatrixfilepath);
	if(i1 < 0) {
		free(prawdata);
		delete pbn;
		return(i1);
	}
	delete pbn;                    // ディストラクタ

	pbn = new CBNMaskFast();       // コンストラクタ
	// 2048x2048のmatrix生成
	width = 2048;
	height = 2048;
	::GetSystemTime(&t1);
	i1 = pbn->makebluenoisematrix(width,height);
	::GetSystemTime(&t2);
	mili2048 = (t2.wHour - t1.wHour) * 3600 * 1000 + (t2.wMinute - t1.wMinute) * 60 * 1000 + (t2.wSecond - t1.wSecond) * 1000 + (t2.wMilliseconds - t1.wMilliseconds);
	if(i1 < 0) {
		free(prawdata);
		delete pbn;
		return(i1);
	}
	// matrix保存
	_swprintf(amatrixfilepath,L"./test/bnmatrix%d_%d.dat",width,height);
	i1 = pbn->msavebnmatrixraw(amatrixfilepath);
	if(i1 < 0) {
		free(prawdata);
		delete pbn;
		return(i1);
	}
	delete pbn;                    // ディストラクタ

	pbn = new CBNMaskFast();       // コンストラクタ
	// 4096x4096のmatrix生成
	width = 4096;
	height = 4096;
	::GetSystemTime(&t1);
	i1 = pbn->makebluenoisematrix(width,height);
	::GetSystemTime(&t2);
	mili4096 = (t2.wHour - t1.wHour) * 3600 * 1000 + (t2.wMinute - t1.wMinute) * 60 * 1000 + (t2.wSecond - t1.wSecond) * 1000 + (t2.wMilliseconds - t1.wMilliseconds);
	if(i1 < 0) {
		free(prawdata);
		delete pbn;
		return(i1);
	}
	// matrix保存
	_swprintf(amatrixfilepath,L"./test/bnmatrix%d_%d.dat",width,height);
	i1 = pbn->msavebnmatrixraw(amatrixfilepath);
	if(i1 < 0) {
		free(prawdata);
		delete pbn;
		return(i1);
	}
	delete pbn;                    // ディストラクタ

	pbn = new CBNMaskFast();       // コンストラクタ
	// 8192x8192のmatrix生成
	width = 8192;
	height = 8192;
	::GetSystemTime(&t1);
	i1 = pbn->makebluenoisematrix(width,height);
	::GetSystemTime(&t2);
	mili8192 = (t2.wHour - t1.wHour) * 3600 * 1000 + (t2.wMinute - t1.wMinute) * 60 * 1000 + (t2.wSecond - t1.wSecond) * 1000 + (t2.wMilliseconds - t1.wMilliseconds);
	if(i1 < 0) {
		free(prawdata);
		delete pbn;
		return(i1);
	}
	// matrix保存
	_swprintf(amatrixfilepath,L"./test/bnmatrix%d_%d.dat",width,height);
	i1 = pbn->msavebnmatrixraw(amatrixfilepath);
	if(i1 < 0) {
		free(prawdata);
		delete pbn;
		return(i1);
	}
	delete pbn;                    // ディストラクタ
#endif

	pbn = new CBNMaskFast();       // コンストラクタ
	// matrixのロード
	i1 = pbn->mloadbnmatrixraw(amatrixfilepath);
	if(i1 < 0) {
		free(prawdata);
		delete pbn;
		return(i1);
	}
	// blue noise擬似乱数系列の作成
	int		size = width * height;
	int		seed = SEED % size;		// 乱数のseed 0～ width * height - 1の範囲
	for(int ptr = seed,i = 0 ; i < 240 ; i++) {
		// 0～(msize - 1)の範囲の正数を0～ RAND_MAXに写像して返す
		unsigned int val = pbn->mgetpmatrixraw()[ptr];
		double		rate = (double)val / (size - 1);
		int ival = (int)(RAND_MAX * rate + 0.5);
		printf("bn[%04d] = %05d ", i, ival);
		if(i % 6 == 5) puts("");
		ptr = (ptr + 1) % size;
	}

	// gifアニメ用bmp画像作成
	pbn->msavematrixrawasbmp(L"./test/bnanime",20);
	pbn->msavewhitenoiseasbmp(L"./test/wnanime",20);


	///////////////////////////////////////////////////////////////////
	// ordered dither画像バッファ(1ピクセル1バイトのグレイスケール画像)
	unsigned char* pdata = (unsigned char*)malloc(rawwidth * rawheight);
	if(pdata == NULL) {
		free(prawdata);
		delete pbn;
		return(MEMORY_SHORTAGE);
	}
	// error diffusionワークバッファ(sizeof(float) * rawwidth * rawheight);
	float* pf = (float *)malloc(sizeof(float) * rawwidth * rawheight);
	if(pf == NULL) {
		free(pdata);
		free(prawdata);
		delete pbn;
		return(MEMORY_SHORTAGE);
	}

	// white noise
	// 2level
	pbn->mordereddither2white(0,rawwidth,rawheight,rawscanlinesize,prawdata,rawwidth,pdata);
	gsavebmp(L"./test/umigranWN2.bmp",rawwidth,rawheight,8,rawscanlinesize,0,300,pdata);
	// 4level
	pbn->mordereddither4white(0,rawwidth,rawheight,rawscanlinesize,prawdata,rawwidth,pdata);
	gsavebmp(L"./test/umigranWN4.bmp",rawwidth,rawheight,8,rawscanlinesize,0,300,pdata);

	// blue noise
	// 2level
	pbn->mordereddither2(0,rawwidth,rawheight,rawscanlinesize,prawdata,rawwidth,pdata);
	gsavebmp(L"./test/umigranBN2.bmp",rawwidth,rawheight,8,rawscanlinesize,0,300,pdata);
	// 4level
	pbn->mordereddither4(0,rawwidth,rawheight,rawscanlinesize,prawdata,rawwidth,pdata);
	gsavebmp(L"./test/umigranBN4.bmp",rawwidth,rawheight,8,rawscanlinesize,0,300,pdata);

	// 	Floyd & Steinberg error diffusion
	// 2level
	pbn->mfloydsteinberg2(rawwidth,rawheight,rawscanlinesize,prawdata,pf,pdata);
	gsavebmp(L"./test/umigranFS2.bmp",rawwidth,rawheight,8,rawscanlinesize,0,300,pdata);
	// 4level
	pbn->mfloydsteinberg4(rawwidth,rawheight,rawscanlinesize,prawdata,pf,pdata);
	gsavebmp(L"./test/umigranFS4.bmp",rawwidth,rawheight,8,rawscanlinesize,0,300,pdata);

	free(prawdata);
	free(pdata);
	free(pf);
	delete pbn;                    // ディストラクタ

	return(0);
}
#endif



