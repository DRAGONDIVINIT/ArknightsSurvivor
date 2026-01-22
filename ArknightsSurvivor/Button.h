#ifndef _BUTTON_H_
#define _BUTTON_H_

#include "Stage.h"
#include "Player.h"
extern bool is_running;
extern Stage game_stage;

extern IMAGE* current_avatar;
extern Player* current_player;
extern IMAGE img_avatar_wisadel;
extern IMAGE img_avatar_mlynar;
extern Animation* anim_wisadel_player_right;
extern Animation* anim_mlynar_player_right;


class Button //按钮基类
{
public:
	Button(RECT rect) : region(rect) {}

	~Button() = default;

	void ProcessEvent(const ExMessage& msg)
	{
		switch (msg.message)
		{
		case WM_MOUSEMOVE:
			if (status == Status::Idle && CheckCursorHit(msg.x, msg.y))
				status = Status::Hovered;
			else if (status == Status::Hovered && !CheckCursorHit(msg.x, msg.y))
				status = Status::Idle;
			break;
		case WM_LBUTTONDOWN:
			if (CheckCursorHit(msg.x, msg.y))
				status = Status::Pushed;
			break;
		case WM_LBUTTONUP:
			if (status == Status::Pushed)
				OnClick();
			break;
		default:
			break;
		}
	}

	virtual void Draw(int delta) = 0;

	void Reset()
	{
		status = Status::Idle;
	}

protected:
	virtual void OnClick() = 0;

protected:
	enum class Status
	{
		Idle = 0,
		Hovered,
		Pushed
	};

protected:
	RECT region;
	Status status = Status::Idle;

private:
	bool CheckCursorHit(int x, int y)//检测鼠标点击
	{
		return x >= region.left && x <= region.right && y >= region.top && y <= region.bottom;
	}
};

class ImageButton : public Button//图片按钮
{
public:
	ImageButton(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed)
		: Button(rect)
	{
		loadimage(&img_idle, path_img_idle);
		loadimage(&img_hovered, path_img_hovered);
		loadimage(&img_pushed, path_img_pushed);
	}

	~ImageButton() = default;

	void Draw(int delta)
	{
		switch (status)
		{
		case Status::Idle:
			putimage(region.left, region.top, &img_idle);
			break;
		case Status::Hovered:
			putimage(region.left, region.top, &img_hovered);
			break;
		case Status::Pushed:
			putimage(region.left, region.top, &img_pushed);
			break;
		}
	}

private:
	IMAGE img_idle;
	IMAGE img_hovered;
	IMAGE img_pushed;
};


class StartGameButton : public ImageButton //开始游戏按钮
{
public:
	StartGameButton(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed)
		: ImageButton(rect, path_img_idle, path_img_hovered, path_img_pushed) {
	}
	~StartGameButton() = default;

protected:
	void OnClick() { game_stage = Stage::Selection; }
};


class QuitGameButton : public ImageButton//退出游戏按钮
{
public:
	QuitGameButton(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed)
		: ImageButton(rect, path_img_idle, path_img_hovered, path_img_pushed) {
	}
	~QuitGameButton() = default;

protected:
	void OnClick()
	{
		is_running = false;
	}
};

class SelectorButton : public Button//选择角色按钮
{
public:
	SelectorButton(RECT rect, Animation* player_anim, LPCTSTR text) : Button(rect)
	{
		tip_text = text;
		player_animation = player_anim;
	}

	~SelectorButton() = default;

	void Draw(int delta)
	{
		switch (status)
		{
		case Status::Idle:
			setfillcolor(RGB(215, 215, 215));
			fillrectangle(region.left, region.top, region.right, region.bottom);
			setlinecolor(RGB(255, 255, 255));
			line(region.left, region.top, region.right, region.top);
			line(region.left, region.top, region.left, region.bottom);
			setlinecolor(RGB(25, 25, 25));
			line(region.left, region.bottom, region.right, region.bottom);
			line(region.right, region.top, region.right, region.bottom);
			break;
		case Status::Hovered:
			setfillcolor(RGB(235, 235, 235));
			fillrectangle(region.left, region.top, region.right, region.bottom);
			setlinecolor(RGB(255, 255, 255));
			line(region.left, region.top, region.right, region.top);
			line(region.left, region.top, region.left, region.bottom);
			setlinecolor(RGB(25, 25, 25));
			line(region.left, region.bottom, region.right, region.bottom);
			line(region.right, region.top, region.right, region.bottom);
			break;
		case Status::Pushed:
			setfillcolor(RGB(195, 195, 195));
			fillrectangle(region.left, region.top, region.right, region.bottom);
			setlinecolor(RGB(25, 25, 25));
			line(region.left, region.top, region.right, region.top);
			line(region.left, region.top, region.left, region.bottom);
			setlinecolor(RGB(255, 255, 255));
			line(region.left, region.bottom, region.right, region.bottom);
			line(region.right, region.top, region.right, region.bottom);
			break;
		}

		player_animation->Play(region.left + 100, region.top + 25, delta);

		setbkmode(TRANSPARENT);
		static const int SHADOW_OFFSET_X = 12;
		static const int SHADOW_OFFSET_Y = 6;
		RECT rect_tip_text = { region.left, region.top + (region.bottom - region.top) / 2, region.right, region.bottom };
		RECT rect_tip_text_shadow = { rect_tip_text.left + SHADOW_OFFSET_X, rect_tip_text.top + SHADOW_OFFSET_Y,
				rect_tip_text.right + SHADOW_OFFSET_X, rect_tip_text.bottom + SHADOW_OFFSET_Y };
		settextcolor(RGB(0, 0, 0));
		drawtext(tip_text, &rect_tip_text, DT_CENTER);
	}

protected:
	void OnClick()
	{
		game_stage = Stage::Playing;
		mciSendString(_T("play bgm1 repeat from 0"), NULL, 0, NULL);
	}

private:
	LPCTSTR tip_text;
	Animation* player_animation;
};

class WisadelSelectorButton : public SelectorButton //维什戴尔按钮
{
public:
	WisadelSelectorButton(RECT rect)
		: SelectorButton(rect, anim_wisadel_player_right,
			_T("【维什戴尔】\n\n----------------------------------\n\n目光清澈的炸弹魔王庭之主\n\n身边围绕着死魂灵\n\n爆裂黎明：可以永久增加死魂灵数"))
	{
	}

	~WisadelSelectorButton() = default;

protected:
	void OnClick()
	{
		SelectorButton::OnClick();
		current_avatar = &img_avatar_wisadel;
		current_player = new WisadelPlayer();
	}
};

class MlynarSelectorButton : public SelectorButton//玛恩那按钮
{
public:
	MlynarSelectorButton(RECT rect)
		: SelectorButton(rect, anim_mlynar_player_right,
			_T("【玛恩那】\n\n----------------------------------\n\n卡西米尔无权为他授勋的天马骑士\n\n剑术专精\n\n未照耀的荣光：造成远距离的大面积伤害"))
	{
	}

	~MlynarSelectorButton() = default;

protected:
	void OnClick()
	{
		SelectorButton::OnClick();
		current_avatar = &img_avatar_mlynar;
		current_player = new MlynarPlayer();
	}
};

#endif