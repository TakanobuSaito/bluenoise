// bnmaskutf8dll.h : bnmaskutf8dll.DLL のメイン ヘッダー ファイル
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル


// Cbnmaskutf8dllApp
// このクラスの実装に関しては bnmaskutf8dll.cpp を参照してください。
//
class CInterBluenoise;			// CBNMaskクラスとの橋渡し

class Cbnmaskutf8dllApp : public CWinApp
{
public:
	CInterBluenoise*		mpinter;
public:
	Cbnmaskutf8dllApp();
	~Cbnmaskutf8dllApp();

// オーバーライド
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
