#ifndef _ANIMATION_H_
#define _ANIMATION_H_

#include "Frame.h"
#include "Tools.h"

class Animation//动画类
{
public:
	Animation(Frame* frame, int interval)
	{
		anim_frame = frame;
		interval_ms = interval;
		frame_count = anim_frame->normal_frame_list.size();
	}

	~Animation() = default;

	void Play(int x, int y, int delta, bool sketch = false)
	{
		if (frame_count > 1)
		{
			timer += delta;
			if (timer >= interval_ms)
			{
				idx_frame = (idx_frame + 1) % frame_count;
				timer = 0;
			}
		}
		
		putimage_alpha(x, y, sketch ? anim_frame->sketch_frame_list[idx_frame] : anim_frame->normal_frame_list[idx_frame]);//根据标识选择是否绘制剪影
	}

private:
	int timer = 0;
	int idx_frame = 0;
	int interval_ms = 0;
	int frame_count = 0;

private:
	Frame* anim_frame;
};

#endif 