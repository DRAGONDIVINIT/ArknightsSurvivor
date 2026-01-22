#ifndef _TOOLS_H_
#define _TOOLS_H_

#include <bits/stdc++.h>
#include <graphics.h>

#pragma comment(lib, "Winmm.lib")//链接 Winmm.lib 库
#pragma comment(lib, "MSIMG32.LIB")//链接 MSIMG32.LIB 库

inline void putimage_alpha(int x, int y, IMAGE* img)//绘制剪影
{
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h,
		GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA });
}

#endif