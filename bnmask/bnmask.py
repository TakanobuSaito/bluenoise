#!/usr/bin/env python
# coding: utf-8

# In[1]:


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


# In[ ]:




