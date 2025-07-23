/////////////////////////////
// bnmaskutf8.dll �e�X�g�v���O����
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

#define		SEED 0		// �����̎�

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
// �w�i�r�b�g����
// bit 1�������ȏ�̏ꍇ��1��Ԃ��B
// RV�ϊ��ł�1��Ԃ����ꍇ�́A�O�i��0/�w�i��1
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
// �������]
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

HINSTANCE bn_dllinstance;			// dll�C���X�^���X
// DLL �G���g���[�|�C���g
// �u���[�m�C�Y�n��̃V�[�h�ݒ�
SBLUENOISE sbluenoise;
// �u���[�m�C�Y�n��̎擾(0�`RAND_MAX)       c�W�����C�u������  int rand(void); �Ɠ����g����
BLUENOISE bluenoise;
// ���ׂȃu���[�m�C�Y�n��̎擾(0�`size*size)   size��setbluenoisematrix�Őݒ肵���l
// �t�@�C�����g��Ȃ��T�C�Y(256*256)�ł�0�`65535�ƍׂ���
BLUENOISEEX bluenoiseex;
// �u���[�m�C�Y�}�g���N�X�T�C�Y�̎擾
// bluenoiseex��0�`matrixsize() - 1�͈̔͂̐��l��Ԃ�
GETMATRIXSIZE getmatrixsize;
// �u���[�m�C�Y �}�g���N�X�ۑ��E�Ǎ��t�H���_�̐ݒ�A�}�g���N�X�T�C�Y�̐ݒ�
// �ʏ�(0�`RAND_MAX)�͈̔͂̃u���[�m�C�Y�̐����ł͐ݒ肵�Ȃ�
// bluenoiseex()�Ő��ׂȃu���[�m�C�Y���擾����ړI������ꍇ�ɐݒ肷��
SETBLUENOISEMATRIX setbluenoisematrix;

// 1...�g�D�I�f�B�U���[�e�B���e�B
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
	HINSTANCE bn_dllinstance;			// dll�C���X�^���X
#if defined(_DEBUG)
	bn_dllinstance = LoadLibrary(L"d:/ocr/src/bnmaskutf8/x64/Debug/bnmaskutf8dll.dll");
#else
	bn_dllinstance = LoadLibrary(L"bnmaskutf8dll.dll");
#endif
	if(bn_dllinstance == NULL) {
		fwprintf(stderr,L"DLĻ�� bnmaskutf8dll.dll ��۰�ނɎ��s���܂���\n");
		return(0);		
	}
	sbluenoise = (SBLUENOISE)GetProcAddress(bn_dllinstance,"sbluenoise");
	bluenoise = (BLUENOISE)GetProcAddress(bn_dllinstance,"bluenoise");
	bluenoiseex = (BLUENOISEEX)GetProcAddress(bn_dllinstance,"bluenoiseex");
	getmatrixsize = (GETMATRIXSIZE)GetProcAddress(bn_dllinstance,"getmatrixsize");
	setbluenoisematrix = (SETBLUENOISEMATRIX)GetProcAddress(bn_dllinstance,"setbluenoisematrix");
	if(sbluenoise == NULL || bluenoise == NULL || bluenoiseex == NULL || getmatrixsize == NULL || setbluenoisematrix == NULL) {
		fwprintf(stderr,L"DLĻ�� bnmaskutf8dll.dll ���ް�ޮ݂��قȂ�܂�(1)\n");
		FreeLibrary(bn_dllinstance);
		return(1);		
	}
	ordereddither2 = (ORDEREDDITHER2)GetProcAddress(bn_dllinstance,"ordereddither2");
	ordereddither4 = (ORDEREDDITHER2)GetProcAddress(bn_dllinstance,"ordereddither4");
	orderedditherline2 = (ORDEREDDITHERLINE2)GetProcAddress(bn_dllinstance,"orderedditherline2");
	orderedditherline4 = (ORDEREDDITHERLINE4)GetProcAddress(bn_dllinstance,"orderedditherline4");
	if(ordereddither2 == NULL || ordereddither4 == NULL || orderedditherline2 == NULL || orderedditherline4 == NULL) {
		fwprintf(stderr,L"DLĻ�� bnmaskutf8dll.dll ���ް�ޮ݂��قȂ�܂�(1)\n");
		FreeLibrary(bn_dllinstance);
		return(1);		
	}
	ordereddither2white = (ORDEREDDITHER2)GetProcAddress(bn_dllinstance,"ordereddither2white");
	ordereddither4white = (ORDEREDDITHER2)GetProcAddress(bn_dllinstance,"ordereddither4white");
	floydsteinberg2 = (FLOYDSTEINBERG2)GetProcAddress(bn_dllinstance,"floydsteinberg2");
	floydsteinberg4 = (FLOYDSTEINBERG4)GetProcAddress(bn_dllinstance,"floydsteinberg4");
	if(ordereddither2white == NULL || ordereddither4white == NULL || floydsteinberg2 == NULL || floydsteinberg4 == NULL) {
		fwprintf(stderr,L"DLĻ�� bnmaskutf8dll.dll ���ް�ޮ݂��قȂ�܂�(1)\n");
		FreeLibrary(bn_dllinstance);
		return(1);		
	}
	// C�����rand() 0�`RAND_MAX�܂ł̐�����Ԃ�/srand(unsigned int seed)�֐��݊�
	sbluenoise(SEED);		// seed = SEED
	for(int i = 0 ; i < 240 ; i++) {
		int ival = bluenoise();
		printf("bn[%03d] = %05d ", i, ival);
		if(i % 6 == 5) puts("");
	}

	printf("\n----------------------------------------------------------------------------------\n");
	// 0�`RAND_MAX�ł͂Ȃ��A0�`getmatrixsize()-1�܂ł̐�����Ԃ�(
	// �W����256�~256�ł́A0�`2^16-1(65535)
	// 1024�~1024�̃u���[�m�C�Y�}�g���N�X���g���΁A0�`2^20-1(1,048,576)�͈̔͂̐������Ԃ�
	// 32768�~32768�̃u���[�m�C�Y�}�g���N�X���g���΁A0�`2^30-1(1,073,741,823)�͈̔͂̐������Ԃ�
	sbluenoise(SEED);		// seed = SEED
	for(int i = 0 ; i < 240 ; i++) {
		int ival = bluenoiseex();
		printf("bn[%03d] = %05d ", i, ival);
		if(i % 6 == 5) puts("");
	}

	// 256�~256�ȊO�̃}�g���N�X(512�~512)�̐���
	int i1 = setbluenoisematrix(L"./test/", 512);
	if(i1 < 0) {
		FreeLibrary(bn_dllinstance);
		return(i1);
	}

	// 0�`getmatrixsize() - 1�܂ł̐�����0.0�`1.0�Ƀ}�b�s���O����
	int		amatrixsize = getmatrixsize();
	sbluenoise(SEED);		// seed = SEED
	for(int i = 0 ; i < 240 ; i++) {
		int ival = bluenoiseex();
		printf("bn[%03d] = %0.6lf ", i, (double)ival / (double)(amatrixsize - 1));
		if(i % 6 == 5) puts("");
	}

	// �摜�o�b�t�@�A�덷�g�U�@�̃��[�N�o�b�t�@�̊m�ہ{�摜�̓ǂݍ���
	//�����ݒ�t�@�C����
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

	// white noise�ɂ��ordered dither
	// 2 level
	ordereddither2white(0,rawwidth,rawheight,rawscanlinesize, prawdata, rawwidth, pdata);
	gsavebmp(L"./test/umigranWN2dll.bmp",rawwidth,rawheight,8,rawscanlinesize,0,300,pdata);
	// 4 level
	ordereddither4white(0,rawwidth,rawheight,rawscanlinesize, prawdata, rawwidth, pdata);
	gsavebmp(L"./test/umigranWN4dll.bmp",rawwidth,rawheight,8,rawscanlinesize,0,300,pdata);

	// blue noise�ɂ��ordered dither
	// 2 level
	ordereddither2(0,rawwidth,rawheight,rawscanlinesize, prawdata, rawwidth, pdata);
	gsavebmp(L"./test/umigranBN2dll.bmp",rawwidth,rawheight,8,rawscanlinesize,0,300,pdata);
	// 4 level
	ordereddither4(0,rawwidth,rawheight,rawscanlinesize, prawdata, rawwidth, pdata);
	gsavebmp(L"./test/umigranBN4dll.bmp",rawwidth,rawheight,8,rawscanlinesize,0,300,pdata);

	// error diffusion(Floyed & Steinberg)�ɂ��ordered dither
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
	//�����ݒ�t�@�C����
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
	// �X�^�e�B�b�N�����N�łł�
	// (1)256�~256��matrix���쐬����
	// (2)���ւ̂���(����)�n��=blue noise �n��擾(���̂��̂�API�͖����̂Ńv���O�������\��)
	// (3)2/4 level whitenoise/bluenoise/error diffusion�摜�A���v6�����쐬����̂�
	CBNMaskFast* pbn = new CBNMaskFast();       // �R���X�g���N�^
	// 256�~256��matrix����
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
	// matrix�ۑ�
	wchar_t	amatrixfilepath[FILENAME_MAX];
	_swprintf(amatrixfilepath,L"./test/bnmatrix%d_%d.dat",width,height);
	i1 = pbn->msavebnmatrixraw(amatrixfilepath);
	if(i1 < 0) {
		free(prawdata);
		delete pbn;
		return(i1);
	}
	delete pbn;                    // �f�B�X�g���N�^

// for time measurement
#if 0
	// 512x512�̃}�g���N�X�쐬
	pbn = new CBNMaskFast();       // �R���X�g���N�^
	// 512�~512��matrix����
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
	// matrix�ۑ�
	_swprintf(amatrixfilepath,L"./test/bnmatrix%d_%d.dat",width,height);
	i1 = pbn->msavebnmatrixraw(amatrixfilepath);
	if(i1 < 0) {
		free(prawdata);
		delete pbn;
		return(i1);
	}
	delete pbn;                    // �f�B�X�g���N�^

	pbn = new CBNMaskFast();       // �R���X�g���N�^
	// 1024x1024��matrix����
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
	// matrix�ۑ�
	_swprintf(amatrixfilepath,L"./test/bnmatrix%d_%d.dat",width,height);
	i1 = pbn->msavebnmatrixraw(amatrixfilepath);
	if(i1 < 0) {
		free(prawdata);
		delete pbn;
		return(i1);
	}
	delete pbn;                    // �f�B�X�g���N�^

	pbn = new CBNMaskFast();       // �R���X�g���N�^
	// 2048x2048��matrix����
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
	// matrix�ۑ�
	_swprintf(amatrixfilepath,L"./test/bnmatrix%d_%d.dat",width,height);
	i1 = pbn->msavebnmatrixraw(amatrixfilepath);
	if(i1 < 0) {
		free(prawdata);
		delete pbn;
		return(i1);
	}
	delete pbn;                    // �f�B�X�g���N�^

	pbn = new CBNMaskFast();       // �R���X�g���N�^
	// 4096x4096��matrix����
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
	// matrix�ۑ�
	_swprintf(amatrixfilepath,L"./test/bnmatrix%d_%d.dat",width,height);
	i1 = pbn->msavebnmatrixraw(amatrixfilepath);
	if(i1 < 0) {
		free(prawdata);
		delete pbn;
		return(i1);
	}
	delete pbn;                    // �f�B�X�g���N�^

	pbn = new CBNMaskFast();       // �R���X�g���N�^
	// 8192x8192��matrix����
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
	// matrix�ۑ�
	_swprintf(amatrixfilepath,L"./test/bnmatrix%d_%d.dat",width,height);
	i1 = pbn->msavebnmatrixraw(amatrixfilepath);
	if(i1 < 0) {
		free(prawdata);
		delete pbn;
		return(i1);
	}
	delete pbn;                    // �f�B�X�g���N�^
#endif

	pbn = new CBNMaskFast();       // �R���X�g���N�^
	// matrix�̃��[�h
	i1 = pbn->mloadbnmatrixraw(amatrixfilepath);
	if(i1 < 0) {
		free(prawdata);
		delete pbn;
		return(i1);
	}
	// blue noise�[�������n��̍쐬
	int		size = width * height;
	int		seed = SEED % size;		// ������seed 0�` width * height - 1�͈̔�
	for(int ptr = seed,i = 0 ; i < 240 ; i++) {
		// 0�`(msize - 1)�͈̔͂̐�����0�` RAND_MAX�Ɏʑ����ĕԂ�
		unsigned int val = pbn->mgetpmatrixraw()[ptr];
		double		rate = (double)val / (size - 1);
		int ival = (int)(RAND_MAX * rate + 0.5);
		printf("bn[%04d] = %05d ", i, ival);
		if(i % 6 == 5) puts("");
		ptr = (ptr + 1) % size;
	}

	// gif�A�j���pbmp�摜�쐬
	pbn->msavematrixrawasbmp(L"./test/bnanime",20);
	pbn->msavewhitenoiseasbmp(L"./test/wnanime",20);


	///////////////////////////////////////////////////////////////////
	// ordered dither�摜�o�b�t�@(1�s�N�Z��1�o�C�g�̃O���C�X�P�[���摜)
	unsigned char* pdata = (unsigned char*)malloc(rawwidth * rawheight);
	if(pdata == NULL) {
		free(prawdata);
		delete pbn;
		return(MEMORY_SHORTAGE);
	}
	// error diffusion���[�N�o�b�t�@(sizeof(float) * rawwidth * rawheight);
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
	delete pbn;                    // �f�B�X�g���N�^

	return(0);
}
#endif



