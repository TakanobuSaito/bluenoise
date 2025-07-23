#!/usr/bin/env python
# coding: utf-8

# In[7]:


# get_ipython().run_line_magic('matplotlib', 'inline')
import cv2
import matplotlib.pyplot as plt
import numpy as np
import ctypes

class CBluenoise:
    def __init__(self):
        self.dllhandle = ctypes.windll.LoadLibrary('bnmaskutf8dll.dll')
        
        # エントリーポイントの取得
        self.c_sbluenoise = self.dllhandle.sbluenoise
        # 戻り値の型を指定
        self.c_sbluenoise.restype = ctypes.c_int32
        # 引数の型を指定
        self.c_sbluenoise.argtypes =  (ctypes.c_uint32,)

        self.c_bluenoise = self.dllhandle.bluenoise
        self.c_bluenoise.restype = ctypes.c_int32
        self.c_bluenoise.argtypes = (None)

        self.c_bluenoiseex = self.dllhandle.bluenoiseex
        self.c_bluenoiseex.restype = ctypes.c_int32
        self.c_bluenoiseex.argtypes = (None)

        self.c_getmatrixsize = self.dllhandle.getmatrixsize
        self.c_getmatrixsize.restype = ctypes.c_int32
        self.c_getmatrixsize.argtypes = (None)

        self.c_setbluenoisematrix = self.dllhandle.setbluenoisematrix
        self.c_setbluenoisematrix.restype = ctypes.c_int32
        self.c_setbluenoisematrix.argtypes = (ctypes.c_wchar_p, ctypes.c_int32)
        
        self.c_ordereddither2 = self.dllhandle.ordereddither2
        self.c_ordereddither2.restype = ctypes.c_int32
        self.c_ordereddither2.argtypes = (ctypes.c_int32, 
                                   ctypes.c_int32, 
                                   ctypes.c_int32, 
                                   ctypes.c_int32, 
                                   ctypes.c_void_p,
                                   ctypes.c_int32, 
                                   ctypes.c_void_p)
        
        self.c_ordereddither4 = self.dllhandle.ordereddither4
        self.c_ordereddither4.restype = ctypes.c_int32
        self.c_ordereddither4.argtypes = (ctypes.c_int32, 
                                   ctypes.c_int32, 
                                   ctypes.c_int32, 
                                   ctypes.c_int32, 
                                   ctypes.c_void_p,
                                   ctypes.c_int32, 
                                   ctypes.c_void_p)
        
        self.c_orderedditherline2 = self.dllhandle.orderedditherline2
        self.c_orderedditherline2.restype = ctypes.c_int32
        self.c_orderedditherline2.argtypes = (ctypes.c_int32, 
                                   ctypes.c_int32, 
                                   ctypes.c_int32, 
                                   ctypes.c_void_p,
                                   ctypes.c_void_p)
        
        self.c_orderedditherline4 = self.dllhandle.orderedditherline4
        self.c_orderedditherline4.restype = ctypes.c_int32
        self.c_orderedditherline4.argtypes = (ctypes.c_int32, 
                                   ctypes.c_int32, 
                                   ctypes.c_int32, 
                                   ctypes.c_void_p,
                                   ctypes.c_void_p)
        
        self.c_ordereddither2white = self.dllhandle.ordereddither2white
        self.c_ordereddither2white.restype = ctypes.c_int32
        self.c_ordereddither2white.argtypes = (ctypes.c_int32, 
                                   ctypes.c_int32, 
                                   ctypes.c_int32, 
                                   ctypes.c_int32, 
                                   ctypes.c_void_p,
                                   ctypes.c_int32, 
                                   ctypes.c_void_p)
        
        self.c_ordereddither4white = self.dllhandle.ordereddither4white
        self.c_ordereddither4white.restype = ctypes.c_int32
        self.c_ordereddither4white.argtypes = (ctypes.c_int32, 
                                   ctypes.c_int32, 
                                   ctypes.c_int32, 
                                   ctypes.c_int32, 
                                   ctypes.c_void_p,
                                   ctypes.c_int32, 
                                   ctypes.c_void_p)
        
        self.c_floydsteinberg2 = self.dllhandle.floydsteinberg2
        self.c_floydsteinberg2.restype = ctypes.c_int32
        self.c_floydsteinberg2.argtypes = (ctypes.c_int32, 
                                   ctypes.c_int32, 
                                   ctypes.c_int32, 
                                   ctypes.c_void_p,
                                   ctypes.c_void_p)
        
        self.c_floydsteinberg4 = self.dllhandle.floydsteinberg4
        self.c_floydsteinberg4.restype = ctypes.c_int32
        self.c_floydsteinberg4.argtypes = (ctypes.c_int32, 
                                   ctypes.c_int32, 
                                   ctypes.c_int32, 
                                   ctypes.c_void_p,
                                   ctypes.c_void_p)
    def sbluenoise(self, seed):
#       c_seed = ctypes.c_uint32(seed)
        return self.c_sbluenoise(seed)
    
    def bluenoise(self):
        return self.c_bluenoise()
    
    def bluenoiseex(self):
        return self.c_bluenoiseex()
    
    # 0.0～1.0
    def random(self):
        ival = self.c_bluenoiseex()
        # 0～imaxval-1 の整数値を 0.0～1.0にマッピング
        imaxval = self.c_getmatrixsize()
        return ival / (imaxval - 1)
    
    def setbluenoisematrix(self, folder, size):
        return self.c_setbluenoisematrix(folder, size)
    
    def ordereddither2(self, planeno, width, height, scanlinesize1, srcimage, scanlinesize2):
#       c_planeno = ctypes.c_int32(planeno)
#       c_width = ctypes.c_int32(width)
#       c_height = ctypes.c_int32(height)
#       c_scanlinesize1 = ctypes.c_int32(scanlinesize1)
#       c_srcimage = srcimage.tobytes()
#       c_scanlinesize2 = ctypes.c_int32(scanlinesize2)
        _size = width * height
        _retimage = np.zeros(_size, dtype=np.uint8)
        _retdata = _retimage.tobytes()
#       self.c_ordereddither2(c_planeno, c_width, c_height, c_scanlinesize1, c_srcimage, c_scanlinesize2, _retdata)
        self.c_ordereddither2(planeno, width, height, scanlinesize1, srcimage.tobytes(), scanlinesize2, _retdata)
        _buffer = ctypes.cast(_retdata, ctypes.POINTER(ctypes.c_uint8 * _size))
        _retimage = np.frombuffer(_buffer.contents, dtype=np.uint8, count=_size)
        _dither2image = np.copy(_retimage).reshape([height, width])
        return _dither2image
    
    def ordereddither4(self, planeno, width, height, scanlinesize1, srcimage, scanlinesize2):
        _size = width * height
        _retimage = np.zeros(_size, dtype=np.uint8)
        _retdata = _retimage.tobytes()
        self.c_ordereddither4(planeno, width, height, scanlinesize1, srcimage.tobytes(), scanlinesize2, _retdata)
        _buffer = ctypes.cast(_retdata, ctypes.POINTER(ctypes.c_uint8 * _size))
        _retimage = np.frombuffer(_buffer.contents, dtype=np.uint8, count=_size)
        _dither4image = np.copy(_retimage).reshape([height, width])
        return _dither4image
    
    def ordereddither2white(self, planeno, width, height, scanlinesize1, srcimage, scanlinesize2):
        _size = width * height
        _retimage = np.zeros(_size, dtype=np.uint8)
        _retdata = _retimage.tobytes()
        self.c_ordereddither2white(planeno, width, height, scanlinesize1, srcimage.tobytes(), scanlinesize2, _retdata)
        _buffer = ctypes.cast(_retdata, ctypes.POINTER(ctypes.c_uint8 * _size))
        _retimage = np.frombuffer(_buffer.contents, dtype=np.uint8, count=_size)
        _dither2image = np.copy(_retimage).reshape([height, width])
        return _dither2image
    
    def ordereddither4white(self, planeno, width, height, scanlinesize1, srcimage, scanlinesize2):
        _size = width * height
        _retimage = np.zeros(_size, dtype=np.uint8)
        _retdata = _retimage.tobytes()
        self.c_ordereddither4white(planeno, width, height, scanlinesize1, srcimage.tobytes(), scanlinesize2, _retdata)
        _buffer = ctypes.cast(_retdata, ctypes.POINTER(ctypes.c_uint8 * _size))
        _retimage = np.frombuffer(_buffer.contents, dtype=np.uint8, count=_size)
        _dither4image = np.copy(_retimage).reshape([height, width])
        return _dither4image

    def floydsteinberg2(self, width, height, scanlinesize1, srcimage):
        _size = width * height
        _retimage = np.zeros(_size, dtype=np.uint8)
        _retdata = _retimage.tobytes()
        self.c_floydsteinberg2(width, height, scanlinesize1, srcimage.tobytes(), _retdata)
        _buffer = ctypes.cast(_retdata, ctypes.POINTER(ctypes.c_uint8 * _size))
        _retimage = np.frombuffer(_buffer.contents, dtype=np.uint8, count=_size)
        _dither2image = np.copy(_retimage).reshape([height, width])
        return _dither2image
    
    def floydsteinberg4(self, width, height, scanlinesize1, srcimage):
        _size = width * height
        _retimage = np.zeros(_size, dtype=np.uint8)
        _retdata = _retimage.tobytes()
        self.c_floydsteinberg4(width, height, scanlinesize1, srcimage.tobytes(), _retdata)
        _buffer = ctypes.cast(_retdata, ctypes.POINTER(ctypes.c_uint8 * _size))
        _retimage = np.frombuffer(_buffer.contents, dtype=np.uint8, count=_size)
        _dither4image = np.copy(_retimage).reshape([height, width])
        return _dither4image

bn = CBluenoise()

# 関数 bluenoise を実行
print('bluenoise API test')
for i in range(100):
    ret = bn.bluenoise()
    print('ret[%02d] = %d\t' % (i, ret), end="")
    if(i % 5 == 4):
        print('')

# 関数 sbluenoise を実行 2 だけずらす
print('')
print('sbluenoise API test')
bn.sbluenoise(2)
for i in range(100):
    ret = bn.bluenoise()
    print('ret[%02d] = %d\t' % (i, ret), end="")
    if(i % 5 == 4):
        print('')

# 数列シードを0に
print('')
print('bluenoiseex API test')
bn.sbluenoise(0)
# 関数 bluenoiseex を実行
for i in range(100):
    ret = bn.bluenoiseex()
    print('ret[%02d] = %d\t' % (i, ret), end="")
    if(i % 5 == 4):
        print('')

# 数列シードを0に
print('')
print('getmatrixsize & bluenoiseex API test')
bn.sbluenoise(0)
# 関数 bluenoiseex を実行
for i in range(100):
    ret = bn.random()
    print('ret[%02d] = %f\t' % (i, ret), end="")
    if(i % 5 == 4):
        print('')

'''
# 関数 setbluenoisematrix を実行
bnsize = 512
bn.setbluenoisematrix("d:/ocr/2025/bluenoise/", bnsize)
# 関数 bluenoiseex を実行
for i in range(100):
    ret = bn.bluenoiseex()
    if 16384 <= bnsize:
        print('ret[%02d] = %09d\t' % (i, ret), end="")
    elif 4096 <= bnsize:
        print('ret[%02d] = %08d\t' % (i, ret), end="")
    elif 1024 <= bnsize:
        print('ret[%02d] = %07d\t' % (i, ret), end="")
    elif 512 <= bnsize:
        print('ret[%02d] = %06d\t' % (i, ret), end="")
    else:
        print('ret[%02d] = %05d\t' % (i, ret), end="")
    if(i % 5 == 4):
        print('')
'''
# 元画像のロード
grayimage = cv2.imread('D:/ocr/2025/bluenoise/umigray.jpg', 0)
graywidth = grayimage.shape[1]
grayheight = grayimage.shape[0]
grayscanlinesize = graywidth

# 数列シードを2に(0以外にすると画像が黒くなるバグ 原因不明)
# bn.sbluenoise(5)

# 関数 ordereddither2/ordereddither4 を実行

dither2image = bn.ordereddither2(0, graywidth, grayheight, grayscanlinesize, np.copy(grayimage), grayscanlinesize)
dither4image = bn.ordereddither4(0, graywidth, grayheight, grayscanlinesize, np.copy(grayimage), grayscanlinesize)

# 関数 ordereddither2white/ordereddither4white を実行
dither2imagewhite = bn.ordereddither2white(0, graywidth, grayheight, grayscanlinesize, np.copy(grayimage), grayscanlinesize)
dither4imagewhite = bn.ordereddither4white(0, graywidth, grayheight, grayscanlinesize, np.copy(grayimage), grayscanlinesize)

# 関数 floydsteinberg2/floydsteinberg4 を実行
floydsteinberg2image = bn.floydsteinberg2(graywidth, grayheight, grayscanlinesize, np.copy(grayimage))
floydsteinberg4image = bn.floydsteinberg4(graywidth, grayheight, grayscanlinesize, np.copy(grayimage))
# 新規ウインドウ作成
fig = plt.figure(figsize=(12, 15))
plt.subplots_adjust(hspace=0.6)

# flg全体をX*Yに分割し、plot位置に画像を配置する。
X = 4
Y = 2

# grayimage の表示
imgplot = 1
ax1 = fig.add_subplot(X, Y, imgplot)
# タイトルの設定
ax1.set_title("original image", fontsize=10)

plt.imshow(grayimage, cmap = "gray")

# dither2image の表示
img2plot =  3
ax2 = fig.add_subplot(X, Y, img2plot)
# タイトルの設定
ax2.set_title(" 2 level bluenoise dither",fontsize=10)
plt.imshow(dither2image, cmap = "gray")

# dither4image の表示
img2plot =  4
ax2 = fig.add_subplot(X, Y, img2plot)
# タイトルの設定
ax2.set_title(" 4 level bluenoise dither",fontsize=10)
plt.imshow(dither4image, cmap = "gray")

# dither2imagewhite の表示
img2plot =  5
ax2 = fig.add_subplot(X, Y, img2plot)
# タイトルの設定
ax2.set_title(" 2 level whitenoise dither",fontsize=10)
plt.imshow(dither2imagewhite, cmap = "gray")

# dither4imagewhite の表示
img2plot =  6
ax2 = fig.add_subplot(X, Y, img2plot)
# タイトルの設定
ax2.set_title(" 4 level whitenoise dither",fontsize=10)
plt.imshow(dither4imagewhite, cmap = "gray")

# floydsteinberg2 の表示
img2plot =  7
ax2 = fig.add_subplot(X, Y, img2plot)
# タイトルの設定
ax2.set_title("2 level floydsteinberg dither",fontsize=10)
plt.imshow(floydsteinberg2image, cmap = "gray")

# floydsteinberg4 の表示
img2plot =  8
ax2 = fig.add_subplot(X, Y, img2plot)
# タイトルの設定
ax2.set_title(" 4 level floydsteinberg dither",fontsize=10)
plt.imshow(floydsteinberg4image, cmap = "gray")

plt.show()


# In[1]:


i1 = 123
i2 = 3456
d1 = i1 / i2
print('d1 = ',d1)
print('i1 = ',i1)


# In[ ]:




