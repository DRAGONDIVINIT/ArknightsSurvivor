#ifndef _HUD_H_
#define _HUD_H_

#include "Player.h"

extern IMAGE* current_avatar;
extern Player* current_player;

extern IMAGE img_F_btn_released;
extern IMAGE img_F_btn_pushed;

class HUD//状态栏位
{
public:
	HUD() = default;
	~HUD() = default;

	void Draw()
	{
		putimage_alpha(25, 25, current_avatar);

		setfillcolor(RGB(5, 5, 5));
		solidroundrect(125, 35, 125 + CONTENT_WIDTH + MARGIN_WIDTH * 2, 35 + CONTENT_HEIGHT + MARGIN_WIDTH * 2, 8, 8);
		solidroundrect(125, 70, 125 + CONTENT_WIDTH + MARGIN_WIDTH * 2, 70 + CONTENT_HEIGHT + MARGIN_WIDTH * 2, 8, 8);
		setfillcolor(RGB(67, 47, 47));
		solidroundrect(125, 35, 125 + CONTENT_WIDTH + MARGIN_WIDTH, 35 + CONTENT_HEIGHT + MARGIN_WIDTH, 8, 8);
		solidroundrect(125, 70, 125 + CONTENT_WIDTH + MARGIN_WIDTH, 70 + CONTENT_HEIGHT + MARGIN_WIDTH, 8, 8);

		double hp_bar_width = CONTENT_WIDTH * max(0, current_player->GetHP()) / 100.0;
		double mp_bar_width = CONTENT_WIDTH * min(100, current_player->GetMP()) / 100.0;
		setfillcolor(RGB(197, 61, 67));
		solidroundrect(125, 35, 125 + (int)hp_bar_width + MARGIN_WIDTH, 35 + CONTENT_HEIGHT + MARGIN_WIDTH, 8, 8);
		setfillcolor(RGB(83, 131, 195));
		solidroundrect(125, 70, 125 + (int)mp_bar_width + MARGIN_WIDTH, 70 + CONTENT_HEIGHT + MARGIN_WIDTH, 8, 8);

		static bool is_F_btn_pushed = false;//当前显示的F键状态
		static int F_btn_switch_counter = 0;//F键按下抬起图标切换动画计数器
		F_btn_switch_counter = (++F_btn_switch_counter) % 35;
		if (F_btn_switch_counter == 0) is_F_btn_pushed = !is_F_btn_pushed;
		if (current_player->GetMP() >= 100)
			putimage_alpha(125 + CONTENT_WIDTH + MARGIN_WIDTH * 2 + 20, 50, is_F_btn_pushed ? &img_F_btn_pushed : &img_F_btn_released);
	}

private:
	const int MARGIN_WIDTH = 3;
	const int CONTENT_WIDTH = 350;
	const int CONTENT_HEIGHT = 20;
};

#endif