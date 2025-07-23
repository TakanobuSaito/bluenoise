// bluenoiseutf8dll.cpp : DLL の初期化ルーチンです。
//

#include "stdafx.h"
#include "bluenoiseutf8dll.h"
#include "interbluenoise.h"
#include "../myerrcode.h"
#include <float.h>
#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: この DLL が MFC DLL に対して動的にリンクされる場合、
//		MFC 内で呼び出されるこの DLL からエクスポートされたどの関数も
//		関数の最初に追加される AFX_MANAGE_STATE マクロを
//		持たなければなりません。
//
//		例:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 通常関数の本体はこの位置にあります
//		}
//
//		このマクロが各関数に含まれていること、MFC 内の
//		どの呼び出しより優先することは非常に重要です。
//		これは関数内の最初のステートメントでなければな 
//		らないことを意味します、コンストラクターが MFC
//		DLL 内への呼び出しを行う可能性があるので、オブ
//		ジェクト変数の宣言よりも前でなければなりません。
//
//		詳細については MFC テクニカル ノート 33 および
//		58 を参照してください。
//

// Cbluenoiseutf8dllApp

BEGIN_MESSAGE_MAP(Cbluenoiseutf8dllApp, CWinApp)
END_MESSAGE_MAP()

wchar_t	gmodulefilename[FILENAME_MAX];
wchar_t	gmodulepath[FILENAME_MAX];
wchar_t	ginipath[FILENAME_MAX];
BOOL	gstopflag;

// Cbluenoiseutf8dllApp コンストラクション

Cbluenoiseutf8dllApp::Cbluenoiseutf8dllApp()
{
	//初期設定ファイル名
	GetModuleFileName(NULL,gmodulefilename,FILENAME_MAX);
	wcscpy(gmodulepath,gmodulefilename);
	wchar_t*	ps1 = wcsrchr(gmodulepath,L'\\');
	if(ps1) *(ps1 + 1) = 0;
	// アプリケーション設定フォルダ
	wchar_t	appfolder[FILENAME_MAX];
	if(SUCCEEDED(SHGetSpecialFolderPath(NULL,appfolder,CSIDL_APPDATA,1))) {
		_swprintf(ginipath,L"%s\\bluenosedll\\",appfolder);
		_wmkdir(ginipath);
	}
	else {
		wcscpy(ginipath,gmodulepath);
	}
	mpinter = new CInterBluenoise();
	if(mpinter) {
		mpinter->msetmatrixfolder(NULL,256);
		mpinter->msetseed(0);
	}
}

Cbluenoiseutf8dllApp::~Cbluenoiseutf8dllApp()
{
	if(mpinter) delete mpinter;
}

// 唯一の Cbluenoiseutf8dllApp オブジェクトです。

Cbluenoiseutf8dllApp theApp;


// Cbluenoiseutf8dllApp 初期化

BOOL Cbluenoiseutf8dllApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}
/////////////////////
// srandに対応するsbluenoise
extern "C" __declspec( dllexport ) int  __stdcall sbluenoise(unsigned int seed)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(theApp.mpinter) {
_fpreset();
		theApp.mpinter->msetseed(seed);
		return(0);
	}
	return(INITIALIZE_ERROR);
}

/////////////////////
// randに対応するbluenoise
extern "C" __declspec( dllexport ) int  __stdcall bluenoise()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(theApp.mpinter) {
_fpreset();
		return theApp.mpinter->mgetbluenoise();
	}
	return(INITIALIZE_ERROR);
}

//////////////////
// 高精細bluenoise
extern "C" __declspec( dllexport ) int  __stdcall bluenoiseex()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(theApp.mpinter) {
_fpreset();
		return theApp.mpinter->mgetbluenoiseex();
	}
	return(INITIALIZE_ERROR);
}

///////////////////
// matrixsizeの取得
extern "C" __declspec( dllexport ) int  __stdcall getmatrixsize()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(theApp.mpinter) {
_fpreset();
		return theApp.mpinter->mgetmatrixsize();
	}
	return(INITIALIZE_ERROR);
}

/////////////////////////
// bluenoise フォルダ、サイズ設定
extern "C" __declspec( dllexport ) int  __stdcall setbluenoisematrix(const wchar_t* folder,int size)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(theApp.mpinter) {
_fpreset();
		int i1 = theApp.mpinter->msetmatrixfolder(folder,size);
		return(i1);
	}
	return(INITIALIZE_ERROR);
}

/////////////////////////
// ordered dither utility
// input image is to be 256 level one plane image
// grayscale image uses 0 plane only
// RGB uses 0,1,2 planes
// CMYK used 0,1,2,3 planes
// max plane number is 8
//  int             no;             plane no (0,1,2,3,4,5,6,7 / 0 for grayscale/ 0,1,2 for RGB / 0,1,2,3 for CMYK)
//                                  if you specify same plane no for different image, you can get dot-on-dot dithering image
//	int				width;			image width(pixel)
//	int				height;			image height(pixel)
//	int				scanlinesize1;	scanline size(byte count) of one line image
//	unsigned char*	pdata1;			256 level image
//	int				scanlinesize2;	scanline size(byte count) of 2 or 4 level image
// output
//  unsigned char*	pdata2;			2 or 4 level image(one pixel is one byte)
//                                  2 level has value(0,255)
//                                  4 level has value(0,85,170,255)

extern "C" __declspec( dllexport ) int  __stdcall ordereddither2(int no,int width,int height,int scanlinesize1,unsigned char* pdata1,
						int scanlinesize2,unsigned char* pdata2)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(theApp.mpinter) {
_fpreset();
		int i1 = theApp.mpinter->mordereddither2(no,width,height,scanlinesize1,pdata1,scanlinesize2,pdata2);
		return(i1);
	}
	return(INITIALIZE_ERROR);
}
extern "C" __declspec( dllexport ) int  __stdcall ordereddither4(int no,int width,int height,int scanlinesize1,unsigned char* pdata1,
						int scanlinesize2,unsigned char* pdata2)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(theApp.mpinter) {
_fpreset();
		int i1 = theApp.mpinter->mordereddither4(no,width,height,scanlinesize1,pdata1,scanlinesize2,pdata2);
		return(i1);
	}
	return(INITIALIZE_ERROR);
}
// ordered dither utility
//     line by line process
// 入力
//   int            y;               line no(0,...height - 1)
extern "C" __declspec( dllexport ) int  __stdcall orderedditherline2(int no,int width,int y,unsigned char* pdata1,unsigned char* pdata2)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(theApp.mpinter) {
_fpreset();
		int i1 = theApp.mpinter->morderedditherline2(no,width,y,pdata1,pdata2);
		return(i1);
	}
	return(INITIALIZE_ERROR);
}
extern "C" __declspec( dllexport ) int  __stdcall orderedditherline4(int no,int width,int y,unsigned char* pdata1,unsigned char* pdata2)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(theApp.mpinter) {
_fpreset();
		int i1 = theApp.mpinter->morderedditherline4(no,width,y,pdata1,pdata2);
		return(i1);
	}
	return(INITIALIZE_ERROR);
}

/////////////////////////
// ordered dither(white noise) utility

extern "C" __declspec( dllexport ) int  __stdcall ordereddither2white(int no,int width,int height,int scanlinesize1,unsigned char* pdata1,
						int scanlinesize2,unsigned char* pdata2)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(theApp.mpinter) {
_fpreset();
		int i1 = theApp.mpinter->mordereddither2white(no,width,height,scanlinesize1,pdata1,scanlinesize2,pdata2);
		return(i1);
	}
	return(INITIALIZE_ERROR);
}
extern "C" __declspec( dllexport ) int  __stdcall ordereddither4white(int no,int width,int height,int scanlinesize1,unsigned char* pdata1,
						int scanlinesize2,unsigned char* pdata2)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(theApp.mpinter) {
_fpreset();
		int i1 = theApp.mpinter->mordereddither4white(no,width,height,scanlinesize1,pdata1,scanlinesize2,pdata2);
		return(i1);
	}
	return(INITIALIZE_ERROR);
}

//////////////////////////////////////
// Floyd & Steinberg error diffusion utility

extern "C" __declspec( dllexport ) int  __stdcall floydsteinberg2(int width,int height,int scanlinesize1,unsigned char* pdata1,unsigned char* pdata2)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(theApp.mpinter) {
_fpreset();
		int i1 = theApp.mpinter->mfloydsteinberg2(width,height,scanlinesize1,pdata1,pdata2);
		return(i1);
	}
	return(INITIALIZE_ERROR);
}
extern "C" __declspec( dllexport ) int  __stdcall floydsteinberg4(int width,int height,int scanlinesize1,unsigned char* pdata1,unsigned char* pdata2)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(theApp.mpinter) {
_fpreset();
		int i1 = theApp.mpinter->mfloydsteinberg4(width,height,scanlinesize1,pdata1,pdata2);
		return(i1);
	}
	return(INITIALIZE_ERROR);
}
