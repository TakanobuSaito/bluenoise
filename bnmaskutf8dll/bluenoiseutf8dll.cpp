// bluenoiseutf8dll.cpp : DLL �̏��������[�`���ł��B
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
//TODO: ���� DLL �� MFC DLL �ɑ΂��ē��I�Ƀ����N�����ꍇ�A
//		MFC ���ŌĂяo����邱�� DLL ����G�N�X�|�[�g���ꂽ�ǂ̊֐���
//		�֐��̍ŏ��ɒǉ������ AFX_MANAGE_STATE �}�N����
//		�����Ȃ���΂Ȃ�܂���B
//
//		��:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// �ʏ�֐��̖{�̂͂��̈ʒu�ɂ���܂�
//		}
//
//		���̃}�N�����e�֐��Ɋ܂܂�Ă��邱�ƁAMFC ����
//		�ǂ̌Ăяo�����D�悷�邱�Ƃ͔��ɏd�v�ł��B
//		����͊֐����̍ŏ��̃X�e�[�g�����g�łȂ���΂� 
//		��Ȃ����Ƃ��Ӗ����܂��A�R���X�g���N�^�[�� MFC
//		DLL ���ւ̌Ăяo�����s���\��������̂ŁA�I�u
//		�W�F�N�g�ϐ��̐錾�����O�łȂ���΂Ȃ�܂���B
//
//		�ڍׂɂ��Ă� MFC �e�N�j�J�� �m�[�g 33 �����
//		58 ���Q�Ƃ��Ă��������B
//

// Cbluenoiseutf8dllApp

BEGIN_MESSAGE_MAP(Cbluenoiseutf8dllApp, CWinApp)
END_MESSAGE_MAP()

wchar_t	gmodulefilename[FILENAME_MAX];
wchar_t	gmodulepath[FILENAME_MAX];
wchar_t	ginipath[FILENAME_MAX];
BOOL	gstopflag;

// Cbluenoiseutf8dllApp �R���X�g���N�V����

Cbluenoiseutf8dllApp::Cbluenoiseutf8dllApp()
{
	//�����ݒ�t�@�C����
	GetModuleFileName(NULL,gmodulefilename,FILENAME_MAX);
	wcscpy(gmodulepath,gmodulefilename);
	wchar_t*	ps1 = wcsrchr(gmodulepath,L'\\');
	if(ps1) *(ps1 + 1) = 0;
	// �A�v���P�[�V�����ݒ�t�H���_
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

// �B��� Cbluenoiseutf8dllApp �I�u�W�F�N�g�ł��B

Cbluenoiseutf8dllApp theApp;


// Cbluenoiseutf8dllApp ������

BOOL Cbluenoiseutf8dllApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}
/////////////////////
// srand�ɑΉ�����sbluenoise
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
// rand�ɑΉ�����bluenoise
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
// ������bluenoise
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
// matrixsize�̎擾
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
// bluenoise �t�H���_�A�T�C�Y�ݒ�
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
// ����
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
