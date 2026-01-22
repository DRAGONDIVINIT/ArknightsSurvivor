#ifndef _BULLET_H_
#define _BULLET_H_

#include "Tools.h"
#include <graphics.h>

extern IMAGE img_blade_up;
extern IMAGE img_blade_down;
extern IMAGE img_blade_left;
extern IMAGE img_blade_right;

class Bullet//子弹基类
{
public:
	POINT position = { 0, 0 };

public:
	Bullet() = default;
	~Bullet() = default;
	virtual void Draw() = 0;
};


class PlayerBullet : public Bullet//玩家子弹类
{
public:
	PlayerBullet() = default;
	~PlayerBullet() = default;
	virtual void Draw() = 0;

protected:
	const int RADIUS = 10;
};

class WisadelPlayerBullet : public PlayerBullet//维什戴尔子弹类
{
public:
	WisadelPlayerBullet() = default;
	~WisadelPlayerBullet() = default;

	void Draw()
	{
		setlinecolor(RGB(255, 155, 50));
		setfillcolor(RGB(200, 75, 10));
		fillcircle(position.x, position.y, RADIUS);
	}
};

class MlynarPlayerBullet : public PlayerBullet//玛恩那子弹类
{
public:
	MlynarPlayerBullet() = default;
	~MlynarPlayerBullet() = default;

	void Draw()
	{
		setlinecolor(RGB(50, 155, 255));
		setfillcolor(RGB(10, 75, 200));
		fillcircle(position.x, position.y, RADIUS);
	}
};

class MlynarPlayerBlade : public PlayerBullet//未照耀的荣光子弹类
{
public:
	enum class Direction
	{
		Up = 0,
		Down,
		Left,
		Right
	};

public:
	MlynarPlayerBlade(Direction dir)
	{
		direction = dir;

		switch (dir)
		{
		case Direction::Up:
			img = &img_blade_up;
			break;
		case Direction::Down:
			img = &img_blade_down;
			break;
		case Direction::Left:
			img = &img_blade_left;
			break;
		case Direction::Right:
			img = &img_blade_right;
			break;
		}
	}
	~MlynarPlayerBlade() = default;

	void Draw()
	{
		putimage_alpha(position.x, position.y, img);
	}

public:
	Direction direction;
	IMAGE* img = nullptr;
};

#endif