// bnmaskutf8dll.h : bnmaskutf8dll.DLL �̃��C�� �w�b�_�[ �t�@�C��
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH �ɑ΂��Ă��̃t�@�C�����C���N���[�h����O�� 'stdafx.h' ���C���N���[�h���Ă�������"
#endif

#include "resource.h"		// ���C�� �V���{��


// Cbnmaskutf8dllApp
// ���̃N���X�̎����Ɋւ��Ă� bnmaskutf8dll.cpp ���Q�Ƃ��Ă��������B
//
class CInterBluenoise;			// CBNMask�N���X�Ƃ̋��n��

class Cbnmaskutf8dllApp : public CWinApp
{
public:
	CInterBluenoise*		mpinter;
public:
	Cbnmaskutf8dllApp();
	~Cbnmaskutf8dllApp();

// �I�[�o�[���C�h
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
