TARGETNAME=bnmaskutf8
OUTDIR=.\Release
LINK=link.exe


ALL : $(OUTDIR)\$(TARGETNAME).dll

# -...�R�}���h���G���[�ł����f���Ȃ�
# @�R�}���h�o�͂�}��
# /Q �^�[�Q�b�g�̃^�C���X�^���v���`�F�b�N
CLEAN :
        -@erase /Q $(OUTDIR)\*

# $(OUTDIR)�������ꍇ
$(OUTDIR) :
        @if not exist $(OUTDIR) mkdir $(OUTDIR)

# /Zi                     Windows�ˑ��@�f�o�b�O���� �v���O���� �f�[�^�x�[�X ���g��
# /nologo                 ���쌠���̔�\��
# /W3                     �R���p�C���̌x�����x��/W0�`/W4
# /WX-                    �x�����G���[�Ƃ��Ĉ��� ������(-)
# /O2                     ���s���x�̍œK��
# /Oi                     �g�ݍ��݊֐��𗘗p����@�͂�
# /Oy-                    �t���[���|�C���^���� ������(-)
# /GL                     �v���O�����S�̂̍œK�� �͂�
# /Fo"$(OUTDIR)\\"\       �I�u�W�F�N�g�t�@�C�����̎w��
# /Fd"$(OUTDIR)\\"\       �v���O�����f�[�^�x�[�X(.pdb)�t�@�C���̖��O�̎w��
# /c                      �R���p�C�����ăI�u�W�F�N�g�t�@�C�������B�����N�͍s��Ȃ�
# /D                      �v���v���Z�b�T��`
# /Gm-                    �ŏ����r���h��L���ɂ��� ������(-)
# /EHsc                   C++�̗�O��L���ɂ��� �͂�
# /GS                     �o�b�t�@�[�Z�L�����e�B�`�F�b�N �͂�
# /Gy                     �֐����x���Ń����N���� �͂�
# /fp:precise             ���������_���f��
# /Zc:wchar_t             wchar_t���r���g�C���^�Ƃ��Ďg��
# /Zc:forScope            for���[�v�X�R�[�v�̋������� �͂��i�Â��v���O�����͂������̃P�[�X�������j
# /Zc:forScope-           for���[�v�X�R�[�v�̋������� ������(-)�i�Â��v���O�����͂������̃P�[�X�������j
# /Gd                     �Ăяo���� _cdecl
CPPFLAGS=\
		/Zi\
        /nologo\
        /W3\
		/WX-\
		/O2\
		/Oi\
		/Oy-\
		/GL\
        /Fo"$(OUTDIR)\\"\
        /Fd"$(OUTDIR)\\"\
        /c\
		/D "WIN32"\
		/D "NDEBUG"\
		/D "_WINDOWS"\
		/D "_USRDLL"\
		/D "BNMASKUTF8_EXPORTS"\
		/D "_WINDLL"\
        /D "UNICODE"\
        /D "_UNICODE"\
		/Gm-\
		/EHsc\
		/GS\
		/Gy\
		/fp:precise\
		/Zc:wchar_t\
		/Zc:forScope\
		/Gd

# /nologo                                   ���쌠���̔�\��
# /pdb:"$(OUTDIR)\$(TARGETNAME).pdb"\       �v���O�����f�[�^�x�[�X(.pdb)�t�@�C���̖��O�̎w�� 
# /out:"$(OUTDIR)\$(TARGETNAME).dll"\       �o��dll�t�@�C�����̎w�� 
# /DEBUG                                    �f�o�b�O���̐��� �͂�
# /INCREMENTAL:NO\                          �C���N�������^�������N��L���ɂ��� ������(NO)
# /DLL\                                     dll�쐬
# /SUBSYSTEM:WINDOWS\                       Windows���C�u�����E�A�v���P�[�V����
# /OPT:REF\                                 �Q�Ƃ���Ă��Ȃ��֐��ƃf�[�^���폜 �͂�
# /OPT:ICF\                                 �����COMDAT�����k �͂�
# /LTCG\                                    �����N���̃R�[�h�������g�p
# /TLBID:1\                                 �^�C�v���C�u�����̃��\�[�XID��1
# /DYNAMICBASE\                             �����_�������ꂽ�x�[�X�A�h���X �͂�
# /NXCOMPAT\                                �f�[�^���s�h�~ �͂�
# /IMPLIB:"$(OUTDIR)\$(TARGETNAME).lib"\    �C���|�[�g���C�u�����̃t�@�C�����̎w��
# /MACHINE:X64                              64bit���C�u���������
# /MACHINE:X86                              32bit���C�u���������
LINK_FLAGS=\
        /nologo\
        /pdb:"$(OUTDIR)\$(TARGETNAME).pdb"\
        /out:"$(OUTDIR)\$(TARGETNAME).dll"\
		/DEBUG\
		/INCREMENTAL:NO\
		/DLL\
		/SUBSYSTEM:WINDOWS\
		/OPT:REF\
		/OPT:ICF\
		/LTCG\
		/TLBID:1\
		/DYNAMICBASE\
		/NXCOMPAT\
		/IMPLIB:"$(OUTDIR)\$(TARGETNAME).lib"\
		/MACHINE:X64 

LINK_OBJS=\
        $(OUTDIR)\bnmask.obj\
        $(OUTDIR)\bnmaskdebug.obj\
        $(OUTDIR)\bnmaskfast.obj\
		$(OUTDIR)\bnmaskmulti.obj


$(OUTDIR)\$(TARGETNAME).dll : $(OUTDIR) $(LINK_OBJS)
        $(LINK) $(LINK_FLAGS) $(LINK_OBJS)

# $@ �^�[�Q�b�g�ɒu�������
# $? �X�V���ꂽ�ˑ��t�@�C���ɒu�������
# $^ �ˑ��t�@�C�����X�g�ɒu�������
# $< �擪�̈ˑ��t�@�C���ɒu�������
# $* �g���q���������^�[�Q�b�g���ɒu�������

# �����ʂ̈Ӗ�
# {} �́Amake �́u�p�^�[���u�����[�� (suffix substitution rule)�v�Ŏg�p����܂��B
# {from}{to} �̌`���Ŏw�肳��Ă���A.cpp �t�@�C������ɂ��� $(OUTDIR) �f�B���N�g������ # .obj �t�@�C���𐶐�����K����\���Ă��܂��B
# ����
# ���̃��[���ł́A.cpp �t�@�C����Ώۂɂ��āAOUTDIR �f�B���N�g�����ɑΉ����� .obj �t�@�C�����쐬���܂��B
# ��̓I�ɂ́A.cpp �t�@�C�������Ƃ��� source.cpp �������ꍇ�A$(OUTDIR)/source.obj �Ƃ����o�̓t�@�C���𐶐�����K������`����Ă��܂��B
# �ϐ��̈Ӗ�
# $< �́u���݂̃^�[�Q�b�g�Ɉˑ�����ŏ��̈ˑ��t�@�C���v���w���܂��B�����ł͌��ƂȂ� .cpp �t�@�C�����ɂȂ�܂��B
# $(CPP) �� C++ �R���p�C���̃R�}���h���w���Ă���A���Ƃ��� g++ �Ȃǂ�����Ɖ���ł��܂��B
# $(CPPFLAGS) �̓R���p�C���ɓn���I�v�V�����ł��B

.cpp{$(OUTDIR)}.obj:
        $(CPP) $(CPPFLAGS) $<