#ifndef _FRAME_H_
#define _FRAME_H_

#include <bits/stdc++.h>
#include <graphics.h>

class Frame//动画帧类
{
public:
	Frame(LPCTSTR path, int num)
	{
		TCHAR path_file[256];//存储构建的文件路径
		for (size_t i = 0; i < num; i++)
		{
			_stprintf_s(path_file, path, i);//构建文件路径

			IMAGE* normal_frame = new IMAGE();
			loadimage(normal_frame, path_file);
			normal_frame_list.push_back(normal_frame);

			IMAGE* sketch_frame = new IMAGE(*normal_frame);//生成用于表示无敌状态的纯白色剪影
			DWORD* color_buffer = GetImageBuffer(sketch_frame);
			int width = sketch_frame->getwidth();
			int height = sketch_frame->getheight();
			for (int y = 0; y < height; ++y)
			{
				for (int x = 0; x < width; ++x)
				{
					int idx = y * width + x;
					if ((color_buffer[idx] & 0xFF000000) >> 24)//将透明度不为 0 的像素处理为白色
						color_buffer[idx] = BGR(RGB(255, 255, 255)) | (((DWORD)(BYTE)(255)) << 24);
				}
			}
			sketch_frame_list.push_back(sketch_frame);
		}
	}

	~Frame()
	{
		for (size_t i = 0; i < normal_frame_list.size(); i++)
		{
			delete sketch_frame_list[i];
			delete normal_frame_list[i];
		}
	}

public:
	std::vector<IMAGE*> normal_frame_list;
	std::vector<IMAGE*> sketch_frame_list;
};

#endif