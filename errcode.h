/*--------------------------------------------------------------
  NGK error code for open source
  this file include error code for ocr system.
   (C) ArtLogic Inc.
       https://www.artlogic.co.jp/index.html
   Everybody and every company can use this file for any purpose
--------------------------------------------------------------*/
#pragma once

/* memory shortage */
// �����ɕK�v�ȃ�����������܂���ł���
#define		MEMORY_SHORTAGE		-1000
/* err exists in argument value */
// �֐��̈������ςł�
#define		ILLEAGAL_ARGUMENT	-1001
#define		ILLEGAL_ARGUMENT	-1001
/* buffset over flow : the contents exceed the buffer size */
// �o�b�t�@�I�[�o�[�t���[�ł�
#define		BUFFER_OVERFLOW		-1002
/* not used in open source */
// ����������Ă��܂�
#define		PARSE_ERROR		-1003
/* some uninitialization error */
// ���C�u�����̏��������Ȃ���Ă��܂���
#define		INITIALIZE_ERROR	-1004
/* not used in open source */
// �n���h���̈�̃I�[�o�[�t���[�ł�
#define		HANDLE_OVERFLOW		-1005
/* not used in open source */
// ���g�p
#define		UNKNOWN_HOST		-1006
/* not used in open source */
// ���g�p
#define		ILLEGAL_FLAG		-1007
/* not used in open source */
// ���g�p
#define		FORK_ERROR		-1008
/* not used in open source */
// ���g�p
#define		DUP_ERROR		-1009
/* free space size in disk drive is too small */
// �f�B�X�N�e�ʕs���ł�
#define		FREESPACE_SHORTAGE	-1010
/* not used in open source */
// ���g�p
#define		ILLEGAL_HANDLE		-1011
/* not used in open source */
// ���g�p
#define		ILLEGAL_POSITION	-1012
// �t�H�[�}�b�g�G���[
#define		ILLEGAL_FORMAT	-1012

/* FILE not found / can not open by some reason */
// �t�@�C����������܂���^�t�@�C�����쐬�ł��܂���
#define		FILE_OPEN_ERROR		-2000
#define		FILE_NOT_FOUND		-2000
/* can not close file by some reason */
// �t�@�C�����N���[�Y�ł��܂���
#define		FILE_CLOSE_ERROR	-2001
/* error occures in reading file */
// �t�@�C�����[�h�G���[
#define		FILE_READ_ERROR		-2002
/* error occures in fseek(file pointer seek) */
// �t�@�C���V�[�N�G���[
#define		FILE_SEEK_ERROR		-2003
/* erro occures in writing file */
// �t�@�C�����C�g�G���[
#define		FILE_WRITE_ERROR	-2004
/* not used in open source */
// ���g�p
#define		FILE_ALREADY_LOCKED	-2005
/* not used in open source */
// ���g�p
#define		FILE_LOCK_ERROR		-2006
/* not used in open source */
// ���g�p
#define		SOCKET_BIND_ERROR	-2007
/* not used in open source */
// ���g�p
#define		SOCKET_ACCEPT_ERROR	-2008
/* not used in open source */
// ���g�p
#define		SOCKET_CONNECT_ERROR	-2009
/* not used in open source */
// ���g�p
#define		SOCKET_GETNAME_ERROR	-2010
/* not used in open source */
// ���g�p
#define		PIPE_OPEN_ERROR		-2011
/* can not make/open temporary file */
// �ꎞ�t�@�C�����쐬�ł��܂���ł���
#define		TMPFILE_OPEN_ERROR	-2012
/* file name is too long */
// �t�@�C�������������܂�
#define		FILE_NAME_TOO_LONG	-2013
/* not used in open source */
// ���g�p
#define		EXHAUST_DICID		-2014
/* file rename error */
// �t�@�C�����ύX�G���[
#define		FILE_RENAME_ERROR	-2015
/* file delete error */
// �t�@�C���폜�G���[
#define		FILE_DELETE_ERROR	-2016
/* not used in open source */
// ���g�p
#define	FILE_INVALID_ERROR		-2017

/* time out error */
// �^�C���A�E�g�G���[
#define		TIME_OUT_ERROR		-3000
/* stopped by user */
// ���[�U�[�ɂ��X�g�b�v
#define		USER_STOP		-3001

/* not used in open source */
// ���g�p
#define		ILLEGAL_HEADER		-3002
/* not used in open source */
// ���g�p
#define		ILLEGAL_VERSION		-3003
/* not used in open source */
// ���g�p
#define		ILLEGAL_GRAPHIC		-3004

/* not used in open source */
// ���g�p
#define		SENTENCE_TOO_LONG	-4000
/* not used in open source */
#define		PART_RANGE_ERROR	-4001
/* not used in open source */
// ���g�p
#define		TRAN_RESULT_DIFF	-4002
/* not used in open source */
// ���g�p
#define		TRAN_INFO_DIFF		-4003
/* not used in open source */
// ���g�p
#define		NOT_YET_TRANSLATED	-4004
/* not used in open source */
// ���g�p
#define		TEMPLATE_TAG_UNMATCH	-4005
/* not used in open source */
// ���g�p
#define		WRONG_TEMPLATE_TAG	-4006
/* not used in open source */
// ���g�p
#define		WORD_TOO_LONG		-4007
/* not used in ocr */
// ���g�p
#define		TOO_MANY_TEMPLATE_TAG	-4008

/* not used in open source */
// ���g�p
#define		BASE_DIC_ERROR		-4010
/* not used in open source */
// ���g�p
#define		USER_DIC_ERROR		-4011
/* not used in open source */
// ���g�p
#define		LEARN_DIC_ERROR		-4012
/* not used in open source */
// ���g�p
#define		OPTION_DIC_ERROR	-4013
/* not used in open source */
// ���g�p
#define		NOTUSR_DIC_ERROR	-4014
/* not used in open source */
// ���g�p
#define		OLDUSR_DIC_ERROR	-4015
/* not used in open source */
// ���g�p
#define		WORD_NOT_FOUND		-4016
/* not used in open source */
// ���g�p
#define		DIC_READ_ERROR		-4017
/* not used in open source */
// ���g�p
#define		DIC_WRITE_ERROR		-4018
/* not used in open source */
// ���g�p
#define		OTHER_ERROR		-9998


/* fatal error:for example pattern dictionnary has no content */
// �z��O�̃G���[�ŏ������f
#define		FATAL_ERROR		-9999

