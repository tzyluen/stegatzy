#!/usr/bin/python
# Copyright (C) 2014 Ng Tzy Luen. All Rights Reserved.
# 
# Description: Get the bitmap padding size, pixel array size, and total memory required
#              to store bitmap pixel data.
#
# Reference: Bitmap pixel storage,
#            http://en.wikipedia.org/wiki/BMP_file_format#Pixel_storage
#
#            The bits representing the bitmap pixels are packed in rows. The size of
#            each row is rounded up to a multiple of 4 bytes (a 32-bit DWORD) by padding.
# 
#
# Therefore, in pseudo:
#
# let `n' be real size(in bits), `p' be padding size(in bits), `x' be row size(in bits),
# n = width * 24bit
#   if n % 4bytes is not equal 0 then
#       p = (4bytes - (n % 4bytes))
#   thus, x = n + p
#        
# And pixel array's size = x * height

### print in bits
def method_one():
    for width in range(1, 1024):
        #print "%d" % ((width * 24 + 31) / 32 * 4)
        n = width * 24
        r = n % 32
        if (r != 0):
            p = 32 - r
            x = n + p
            ps = x * width
            height = width  # assume height == width
            print "width->%d, height->%d, padding->%d bits, rowsize->%d bits, pixelstorage->%d bits" %(width, height, p, x, ps)
        else:
            print "width->%d, height->%d, padding->%d bits, rowsize->%d bits, pixelstorage->%d bits" %(width, height, 0, n, n * width)


### print in bytes
def method_two():
    for width in range(1, 1024):
        height = width  # assume height == width
        n = ((width * 24 + 31) / 32) * 4;
        p = n - width * 3;
        print "width->%d, padding->%d bytes, rowsize->%d bytes" %(width, p, n)


#method_one()
method_two()
