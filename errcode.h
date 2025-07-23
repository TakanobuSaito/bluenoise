/*--------------------------------------------------------------
  NGK error code for open source
  this file include error code for ocr system.
   (C) ArtLogic Inc.
       https://www.artlogic.co.jp/index.html
   Everybody and every company can use this file for any purpose
--------------------------------------------------------------*/
#pragma once

/* memory shortage */
// 処理に必要なメモリが足りませんでした
#define		MEMORY_SHORTAGE		-1000
/* err exists in argument value */
// 関数の引数が変です
#define		ILLEAGAL_ARGUMENT	-1001
#define		ILLEGAL_ARGUMENT	-1001
/* buffset over flow : the contents exceed the buffer size */
// バッファオーバーフローです
#define		BUFFER_OVERFLOW		-1002
/* not used in open source */
// 書式が違っています
#define		PARSE_ERROR		-1003
/* some uninitialization error */
// ライブラリの初期化がなされていません
#define		INITIALIZE_ERROR	-1004
/* not used in open source */
// ハンドル領域のオーバーフローです
#define		HANDLE_OVERFLOW		-1005
/* not used in open source */
// 未使用
#define		UNKNOWN_HOST		-1006
/* not used in open source */
// 未使用
#define		ILLEGAL_FLAG		-1007
/* not used in open source */
// 未使用
#define		FORK_ERROR		-1008
/* not used in open source */
// 未使用
#define		DUP_ERROR		-1009
/* free space size in disk drive is too small */
// ディスク容量不足です
#define		FREESPACE_SHORTAGE	-1010
/* not used in open source */
// 未使用
#define		ILLEGAL_HANDLE		-1011
/* not used in open source */
// 未使用
#define		ILLEGAL_POSITION	-1012
// フォーマットエラー
#define		ILLEGAL_FORMAT	-1012

/* FILE not found / can not open by some reason */
// ファイルが見つかりません／ファイルを作成できません
#define		FILE_OPEN_ERROR		-2000
#define		FILE_NOT_FOUND		-2000
/* can not close file by some reason */
// ファイルがクローズできません
#define		FILE_CLOSE_ERROR	-2001
/* error occures in reading file */
// ファイルリードエラー
#define		FILE_READ_ERROR		-2002
/* error occures in fseek(file pointer seek) */
// ファイルシークエラー
#define		FILE_SEEK_ERROR		-2003
/* erro occures in writing file */
// ファイルライトエラー
#define		FILE_WRITE_ERROR	-2004
/* not used in open source */
// 未使用
#define		FILE_ALREADY_LOCKED	-2005
/* not used in open source */
// 未使用
#define		FILE_LOCK_ERROR		-2006
/* not used in open source */
// 未使用
#define		SOCKET_BIND_ERROR	-2007
/* not used in open source */
// 未使用
#define		SOCKET_ACCEPT_ERROR	-2008
/* not used in open source */
// 未使用
#define		SOCKET_CONNECT_ERROR	-2009
/* not used in open source */
// 未使用
#define		SOCKET_GETNAME_ERROR	-2010
/* not used in open source */
// 未使用
#define		PIPE_OPEN_ERROR		-2011
/* can not make/open temporary file */
// 一時ファイルが作成できませんでした
#define		TMPFILE_OPEN_ERROR	-2012
/* file name is too long */
// ファイル名が長すぎます
#define		FILE_NAME_TOO_LONG	-2013
/* not used in open source */
// 未使用
#define		EXHAUST_DICID		-2014
/* file rename error */
// ファイル名変更エラー
#define		FILE_RENAME_ERROR	-2015
/* file delete error */
// ファイル削除エラー
#define		FILE_DELETE_ERROR	-2016
/* not used in open source */
// 未使用
#define	FILE_INVALID_ERROR		-2017

/* time out error */
// タイムアウトエラー
#define		TIME_OUT_ERROR		-3000
/* stopped by user */
// ユーザーによるストップ
#define		USER_STOP		-3001

/* not used in open source */
// 未使用
#define		ILLEGAL_HEADER		-3002
/* not used in open source */
// 未使用
#define		ILLEGAL_VERSION		-3003
/* not used in open source */
// 未使用
#define		ILLEGAL_GRAPHIC		-3004

/* not used in open source */
// 未使用
#define		SENTENCE_TOO_LONG	-4000
/* not used in open source */
#define		PART_RANGE_ERROR	-4001
/* not used in open source */
// 未使用
#define		TRAN_RESULT_DIFF	-4002
/* not used in open source */
// 未使用
#define		TRAN_INFO_DIFF		-4003
/* not used in open source */
// 未使用
#define		NOT_YET_TRANSLATED	-4004
/* not used in open source */
// 未使用
#define		TEMPLATE_TAG_UNMATCH	-4005
/* not used in open source */
// 未使用
#define		WRONG_TEMPLATE_TAG	-4006
/* not used in open source */
// 未使用
#define		WORD_TOO_LONG		-4007
/* not used in ocr */
// 未使用
#define		TOO_MANY_TEMPLATE_TAG	-4008

/* not used in open source */
// 未使用
#define		BASE_DIC_ERROR		-4010
/* not used in open source */
// 未使用
#define		USER_DIC_ERROR		-4011
/* not used in open source */
// 未使用
#define		LEARN_DIC_ERROR		-4012
/* not used in open source */
// 未使用
#define		OPTION_DIC_ERROR	-4013
/* not used in open source */
// 未使用
#define		NOTUSR_DIC_ERROR	-4014
/* not used in open source */
// 未使用
#define		OLDUSR_DIC_ERROR	-4015
/* not used in open source */
// 未使用
#define		WORD_NOT_FOUND		-4016
/* not used in open source */
// 未使用
#define		DIC_READ_ERROR		-4017
/* not used in open source */
// 未使用
#define		DIC_WRITE_ERROR		-4018
/* not used in open source */
// 未使用
#define		OTHER_ERROR		-9998


/* fatal error:for example pattern dictionnary has no content */
// 想定外のエラーで処理中断
#define		FATAL_ERROR		-9999

