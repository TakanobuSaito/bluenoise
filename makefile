TARGETNAME=bnmaskutf8
OUTDIR=.\Release
LINK=link.exe


ALL : $(OUTDIR)\$(TARGETNAME).dll

# -...コマンドがエラーでも中断しない
# @コマンド出力を抑制
# /Q ターゲットのタイムスタンプをチェック
CLEAN :
        -@erase /Q $(OUTDIR)\*

# $(OUTDIR)が無い場合
$(OUTDIR) :
        @if not exist $(OUTDIR) mkdir $(OUTDIR)

# /Zi                     Windows依存　デバッグ情報は プログラム データベース を使う
# /nologo                 著作権情報の非表示
# /W3                     コンパイラの警告レベル/W0～/W4
# /WX-                    警告をエラーとして扱う いいえ(-)
# /O2                     実行速度の最適化
# /Oi                     組み込み関数を利用する　はい
# /Oy-                    フレームポインタ無し いいえ(-)
# /GL                     プログラム全体の最適化 はい
# /Fo"$(OUTDIR)\\"\       オブジェクトファイル名の指定
# /Fd"$(OUTDIR)\\"\       プログラムデータベース(.pdb)ファイルの名前の指定
# /c                      コンパイルしてオブジェクトファイルを作る。リンクは行わない
# /D                      プリプロセッサ定義
# /Gm-                    最小リビルドを有効にする いいえ(-)
# /EHsc                   C++の例外を有効にする はい
# /GS                     バッファーセキュリティチェック はい
# /Gy                     関数レベルでリンクする はい
# /fp:precise             浮動小数点モデル
# /Zc:wchar_t             wchar_tをビルトイン型として使う
# /Zc:forScope            forループスコープの強制準拠 はい（古いプログラムはいいえのケースが多い）
# /Zc:forScope-           forループスコープの強制準拠 いいえ(-)（古いプログラムはいいえのケースが多い）
# /Gd                     呼び出しは _cdecl
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

# /nologo                                   著作権情報の非表示
# /pdb:"$(OUTDIR)\$(TARGETNAME).pdb"\       プログラムデータベース(.pdb)ファイルの名前の指定 
# /out:"$(OUTDIR)\$(TARGETNAME).dll"\       出力dllファイル名の指定 
# /DEBUG                                    デバッグ情報の生成 はい
# /INCREMENTAL:NO\                          インクリメンタルリンクを有効にする いいえ(NO)
# /DLL\                                     dll作成
# /SUBSYSTEM:WINDOWS\                       Windowsライブラリ・アプリケーション
# /OPT:REF\                                 参照されていない関数とデータを削除 はい
# /OPT:ICF\                                 同一のCOMDATを圧縮 はい
# /LTCG\                                    リンク時のコード生成を使用
# /TLBID:1\                                 タイプライブラリのリソースIDは1
# /DYNAMICBASE\                             ランダム化されたベースアドレス はい
# /NXCOMPAT\                                データ実行防止 はい
# /IMPLIB:"$(OUTDIR)\$(TARGETNAME).lib"\    インポートライブラリのファイル名の指定
# /MACHINE:X64                              64bitライブラリを作る
# /MACHINE:X86                              32bitライブラリを作る
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

# $@ ターゲットに置き換わる
# $? 更新された依存ファイルに置き換わる
# $^ 依存ファイルリストに置き換わる
# $< 先頭の依存ファイルに置き換わる
# $* 拡張子を除いたターゲット名に置き換わる

# 中括弧の意味
# {} は、make の「パターン置換ルール (suffix substitution rule)」で使用されます。
# {from}{to} の形式で指定されており、.cpp ファイルを基にして $(OUTDIR) ディレクトリ内に # .obj ファイルを生成する規則を表しています。
# 動作
# このルールでは、.cpp ファイルを対象にして、OUTDIR ディレクトリ内に対応する .obj ファイルを作成します。
# 具体的には、.cpp ファイルがたとえば source.cpp だった場合、$(OUTDIR)/source.obj という出力ファイルを生成する規則が定義されています。
# 変数の意味
# $< は「現在のターゲットに依存する最初の依存ファイル」を指します。ここでは元となる .cpp ファイル名になります。
# $(CPP) は C++ コンパイラのコマンドを指しており、たとえば g++ などが入ると仮定できます。
# $(CPPFLAGS) はコンパイラに渡すオプションです。

.cpp{$(OUTDIR)}.obj:
        $(CPP) $(CPPFLAGS) $<