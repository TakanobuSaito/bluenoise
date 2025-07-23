#!/usr/bin/env python
# coding: utf-8

# In[3]:


get_ipython().run_line_magic('matplotlib', 'inline')
import cv2
import matplotlib.pyplot as plt
import numpy as np
from bnmask.bnmask import CBluenoise

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

# flg全体をX*Yに分割し、plot位置に画像を配置する。
X = 4
Y = 2

# grayimage の表示
imgplot = 1
ax1 = fig.add_subplot(X, Y, imgplot)
# タイトルの設定
ax1.set_title("original image", fontsize=20)

plt.imshow(grayimage, cmap = "gray")

# dither2image の表示
img2plot =  3
ax2 = fig.add_subplot(X, Y, img2plot)
# タイトルの設定
ax2.set_title(" 2 level bluenoise dither",fontsize=20)
plt.imshow(dither2image, cmap = "gray")

# dither4image の表示
img2plot =  4
ax2 = fig.add_subplot(X, Y, img2plot)
# タイトルの設定
ax2.set_title(" 4 level bluenoise dither",fontsize=20)
plt.imshow(dither4image, cmap = "gray")

# dither2imagewhite の表示
img2plot =  5
ax2 = fig.add_subplot(X, Y, img2plot)
# タイトルの設定
ax2.set_title(" 2 level whitenoise dither",fontsize=20)
plt.imshow(dither2imagewhite, cmap = "gray")

# dither4imagewhite の表示
img2plot =  6
ax2 = fig.add_subplot(X, Y, img2plot)
# タイトルの設定
ax2.set_title(" 4 level whitenoise dither",fontsize=20)
plt.imshow(dither4imagewhite, cmap = "gray")

# floydsteinberg2 の表示
img2plot =  7
ax2 = fig.add_subplot(X, Y, img2plot)
# タイトルの設定
ax2.set_title("2 level floydsteinberg dither",fontsize=20)
plt.imshow(floydsteinberg2image, cmap = "gray")

# floydsteinberg4 の表示
img2plot =  8
ax2 = fig.add_subplot(X, Y, img2plot)
# タイトルの設定
ax2.set_title(" 4 level floydsteinberg dither",fontsize=20)
plt.imshow(floydsteinberg4image, cmap = "gray")

#plt.show() #なくても表示された。


# In[ ]:




