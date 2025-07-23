///////////////////////////////////
// bluenosedll ���C�u�����R�[���o�b�N�錾
#pragma once
#pragma pack(8)

#	ifdef __cplusplus
extern "C" {
#	endif
///////////////////////////////
// �u���[�m�C�Y�n��̃V�[�h�ݒ�
//  unsigned int		seed;		seed�l   c�W�����C�u������  void srand(unsigned int seed); �Ɠ����g����
// �Ԃ�l
//�@�@��....�G���[(INITIALIZE_ERROR...�������G���[(�N���X�̃C���X�^���X�����Ɏ��s:�������s��))
typedef int (__stdcall* SBLUENOISE)(unsigned int seed);
extern SBLUENOISE sbluenoise;
/////////////////////////
// �u���[�m�C�Y�n��̎擾(0�`RAND_MAX)       c�W�����C�u������  int rand(void); �Ɠ����g����
// RAND_MAX��0x7fff(32767)�Ȃ̂őe��
// �Ԃ�l
//�@�@��....�G���[(INITIALIZE_ERROR...�������G���[(�N���X�̃C���X�^���X�����Ɏ��s:�������s��))
//�@�@�u���[�m�C�Y�n��
typedef int (__stdcall* BLUENOISE)();
extern BLUENOISE bluenoise;
/////////////////////////
// ���ׂȃu���[�m�C�Y�n��̎擾(0�`size*size)   size��setbluenoisematrix�Őݒ肵���l
// �t�@�C�����g��Ȃ��T�C�Y(256*256)�ł�0�`65535�ƍׂ���
//   256*  256       0�`0x0000ffff(0�`     65,536-1)      �t�@�C�����g��Ȃ�(���C�u��������256K�o�C�g�̕\)
//   512*  512       0�`0x0003ffff(0�`    262,144-1)�@�@     1M�o�C�g
//  1024* 1024       0�`0x000fffff(0�`  1,048,576-1)         4M�o�C�g
//  2048* 2048       0�`0x003fffff(0�`  4,194,304-1)        16M�o�C�g
//  4096* 4096       0�`0x00ffffff(0�` 16,777,216-1)        64M�o�C�g
//  8192* 8192       0�`0x03ffffff(0�` 67,108,864-1)       256M�o�C�g
// 16384*16384       0�`0x0fffffff(0�`268,435,456-1)      1024M�o�C�g(1G�o�C�g)
// 32768*32768       0�`0x3fffffff(0�`1,073,741,824-1)    4096M�o�C�g(4G�o�C�g)
// �Ԃ�l
//�@�@��....�G���[(INITIALIZE_ERROR...�������G���[(�N���X�̃C���X�^���X�����Ɏ��s:�������s��))
//�@�@�u���[�m�C�Y�n��
typedef int (__stdcall* BLUENOISEEX)();
extern BLUENOISEEX bluenoiseex;
/////////////////////////
// �u���[�m�C�Y�}�g���N�X�T�C�Y�̎擾
// bluenoiseex��0�`matrixsize() - 1�͈̔͂̐��l��Ԃ�
// �Ԃ�l
//�@�@��....���݂̃u���[�m�C�Y�}�g���N�X�T�C�Y
//�@�@�u���[�m�C�Y�n��
typedef int (__stdcall* GETMATRIXSIZE)();
extern GETMATRIXSIZE getmatrixsize;
/////////////////////////
// �u���[�m�C�Y �}�g���N�X�ۑ��E�Ǎ��t�H���_�̐ݒ�A�}�g���N�X�T�C�Y�̐ݒ�
// �ʏ�(0�`RAND_MAX)�͈̔͂̃u���[�m�C�Y�̐����ł͐ݒ肵�Ȃ�
// bluenoiseex()�Ő��ׂȃu���[�m�C�Y���擾����ړI������ꍇ�ɐݒ肷��
// ����
//    const char*	pfolder;	�}�g���N�X�t�@�C���̑��݂���/�𐶐�����t�H���_��
//                              NULL���邢��""���w���ꍇ��256�~256�T�C�Y�̑g�ݍ��݃}�g���N�X�𗘗p����
//    int           size;		�}�g���N�X�T�C�Y(512/1024/2048/4096/8192/16384)
//                              1024�̏ꍇ�A1024�~1024�̃u���[�m�C�Y�}�g���N�X�𐶐�����B�t�@�C���T�C�Y��4���K�o�C�g
// �Ԃ�l
//�@�@��....�G���[(INITIALIZE_ERROR...�������G���[(�N���X�̃C���X�^���X�����Ɏ��s:�������s��))
//�@�@��....�G���[(FILE_OPEN_ERROR....�ۑ��ł��Ȃ��A�ǂݍ��݂ł��Ȃ�)
//                (MEMORY_SHORTAGE....�������s��)
// �w�肵���t�H���_��sizexsizeraw.dat�Ƃ����t�@�C����ǂݍ���
//                   size��1024�̏ꍇ��1024x1024raw.dat�Ƃ����t�@�C�����ɂȂ�
//                   �t�@�C���������ꍇ�̓t�@�C�����쐬����
//                   size���傫���Ȃ�قǍ��i����bluenoise�𐶐�����
typedef int (__stdcall* SETBLUENOISEMATRIX)(const wchar_t* pfolder,int size);
extern SETBLUENOISEMATRIX setbluenoisematrix;

// 1...�g�D�I�f�B�U���[�e�B���e�B
// ���������}�g���N�X���g����ordered dither�����s����
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
