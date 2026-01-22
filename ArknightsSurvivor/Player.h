#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "Bullet.h"
#include "Animation.h"

extern const int WINDOW_WIDTH;
extern const int WINDOW_HEIGHT;

extern IMAGE img_shadow_player;

extern Frame* frame_wisadel_player_left;
extern Frame* frame_wisadel_player_right;

extern Frame* frame_mlynar_player_left;
extern Frame* frame_mlynar_player_right;

class Player//玩家基类
{
public:
	Player() = default;

	~Player()
	{
		for (Bullet* bullet : bullet_list)
			delete bullet;
	}

	virtual void Draw(int delta)
	{
		for (Bullet* bullet : bullet_list)
			bullet->Draw();

		static int sketch_switch_counter = 0;//剪影闪烁效果计数器
		sketch_switch_counter = (++sketch_switch_counter) % 15;
		if (sketch_switch_counter == 0) is_show_sketch = !is_show_sketch;
	}

	virtual void OnReleaseSkill() = 0;

	void Move()
	{
		int dir_x = is_move_right - is_move_left;
		int dir_y = is_move_down - is_move_up;
		double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);
		if (len_dir != 0)
		{
			double normalized_x = dir_x / len_dir;
			double normalized_y = dir_y / len_dir;
			position.x += (int)(SPEED * normalized_x);
			position.y += (int)(SPEED * normalized_y);
		}

		if (position.x < 0) position.x = 0;
		if (position.y < 0) position.y = 0;
		if (position.x + width > WINDOW_WIDTH) position.x = WINDOW_WIDTH - width;
		if (position.y + height > WINDOW_HEIGHT) position.y = WINDOW_HEIGHT - height;

		UpdateBullets();
	}
	void ProcessEvent(const ExMessage& msg)
	{
		switch (msg.message)
		{
		case WM_KEYDOWN:
			switch (msg.vkcode)
			{
			case VK_UP:
				is_move_up = true;
				break;
			case VK_DOWN:
				is_move_down = true;
				break;
			case VK_LEFT:
				is_move_left = true;
				break;
			case VK_RIGHT:
				is_move_right = true;
				break;
			case 0x46://F键
				if (mp >= 100)
				{
					OnReleaseSkill();
					mp = 0;
				}
				break;
			}
			break;

		case WM_KEYUP:
			switch (msg.vkcode)
			{
			case VK_UP:
				is_move_up = false;
				break;
			case VK_DOWN:
				is_move_down = false;
				break;
			case VK_LEFT:
				is_move_left = false;
				break;
			case VK_RIGHT:
				is_move_right = false;
				break;
			}
			break;
		}
	}

	void Hurt()
	{
		if (invincible) return;

		hp -= 10;
		invincible = true;
		invincible_timer = INVINCIBLE_DELAY;
		mciSendString(_T("play hurt from 0"), NULL, 0, NULL);
	}

	int GetHP() { return hp; }
	int GetMP() { return mp; }
	void IncreaseMP() { mp += mp_delta; }
	int GetWidth() const { return width; }
	int GetHeight() const { return height; }
	const POINT& GetPosition() const { return position; }
	const std::vector<Bullet*>& GetBullets() const { return bullet_list; }

protected:
	const int SPEED = 3;// 玩家移动速度
	const int SHADOW_WIDTH = 32;// 玩家脚底阴影宽度
	const int INVINCIBLE_DELAY = 1000;// 玩家受击后无敌时长

protected:
	int mp_delta = 1;// 击中敌人单次能量增长
	int hp = 100, mp = 0;// 当前生命值和能量值
	bool invincible = false;// 是否处于无敌状态
	bool is_move_up = false;// 是否向上移动
	bool is_move_down = false;// 是否向下移动
	bool is_move_left = false;// 是否向左移动
	bool is_move_right = false;// 是否向右移动
	bool is_show_sketch = false;// 当前是否显示剪影
	int invincible_timer = 0;// 无敌时间计时器
	int width = 0, height = 0;// 玩家宽度和高度
	POINT position = { 640, 360 };// 玩家位置
	std::vector<Bullet*> bullet_list;// 玩家拥有的子弹列表

protected:
	virtual void UpdateBullets() = 0;
};

class WisadelPlayer : public Player//高贵的维什戴尔玩家
{
public:
	WisadelPlayer()
	{
		width = 100, height = 100, mp_delta = 5;
		anim_left = new Animation(frame_wisadel_player_left, 0);
		anim_right = new Animation(frame_wisadel_player_right, 0);

		for (size_t i = 0; i < 3; i++)
			bullet_list.push_back(new WisadelPlayerBullet());
	}

	~WisadelPlayer()
	{
		delete anim_left;
		delete anim_right;
	}

	void Draw(int delta)
	{
		Player::Draw(delta);

		int pos_shadow_x = position.x + (width / 2 - SHADOW_WIDTH / 2);
		int pos_shadow_y = position.y + height - 8;
		putimage_alpha(pos_shadow_x, pos_shadow_y, &img_shadow_player);

		static bool facing_left = false;
		int dir_x = is_move_right - is_move_left;
		if (dir_x < 0)
			facing_left = true;
		else if (dir_x > 0)
			facing_left = false;

		if (facing_left)
			anim_left->Play(position.x, position.y, delta, invincible && is_show_sketch);
		else
			anim_right->Play(position.x, position.y, delta, invincible && is_show_sketch);

		invincible_timer -= delta;// 更新无敌时间计时器
		invincible = (invincible_timer > 0);
	}

	void OnReleaseSkill()
	{
		bullet_list.push_back(new WisadelPlayerBullet());//释放技能时增加玩家周围的死魂灵数量
	}

protected:
	void UpdateBullets()
	{
		const double RADIAL_SPEED = 0.0045;
		const double RADIUS = 100;
		for (size_t i = 0; i < bullet_list.size(); i++)
		{
			double radian = GetTickCount() * RADIAL_SPEED + 2 * 3.14159 * i / bullet_list.size();
			bullet_list[i]->position.x = position.x + width / 2 + (int)(RADIUS * cos(radian));
			bullet_list[i]->position.y = position.y + height / 2 + (int)(RADIUS * sin(radian));
		}
	}

private:
	Animation* anim_left;
	Animation* anim_right;
};

class MlynarPlayer : public Player//玛恩那玩家
{
public:
	MlynarPlayer()
	{
		width = 100, height = 100, mp_delta = 20;
		anim_left = new Animation(frame_mlynar_player_left, 0);
		anim_right = new Animation(frame_mlynar_player_right, 0);

		for (size_t i = 0; i < 3; i++)
			bullet_list.push_back(new MlynarPlayerBullet());

		blade_bullets.push_back(new MlynarPlayerBlade(MlynarPlayerBlade::Direction::Up));
		blade_bullets.push_back(new MlynarPlayerBlade(MlynarPlayerBlade::Direction::Down));
		blade_bullets.push_back(new MlynarPlayerBlade(MlynarPlayerBlade::Direction::Left));
		blade_bullets.push_back(new MlynarPlayerBlade(MlynarPlayerBlade::Direction::Right));
		for (Bullet* bullet : blade_bullets)//初始化未照耀的荣光
		{
			bullet->position.x = -10000;
			bullet->position.y = -10000;
			bullet_list.push_back(bullet);
		}
	}

	~MlynarPlayer()
	{
		delete anim_left;
		delete anim_right;
	}

	void Draw(int delta)
	{
		Player::Draw(delta);

		static bool facing_left = false;
		int dir_x = is_move_right - is_move_left;
		if (dir_x < 0)
			facing_left = true;
		else if (dir_x > 0)
			facing_left = false;

		int pos_shadow_x = position.x + (width / 2 - SHADOW_WIDTH / 2) + (facing_left ? 10 : -10);
		int pos_shadow_y = position.y + height - 15;
		putimage_alpha(pos_shadow_x, pos_shadow_y, &img_shadow_player);

		if (facing_left)
			anim_left->Play(position.x, position.y, delta, invincible && is_show_sketch);
		else
			anim_right->Play(position.x, position.y, delta, invincible && is_show_sketch);

		invincible_timer -= delta;//更新无敌时间计时器
		invincible = (invincible_timer > 0);
	}

	void OnReleaseSkill()
	{
		for (MlynarPlayerBlade* blade : blade_bullets)// 释放技能时重置未照耀的荣光位置到玩家周围
		{
			switch (blade->direction)
			{
			case MlynarPlayerBlade::Direction::Up:
				blade->position.x = position.x + (width - BLADE_WIDTH) / 2;
				blade->position.y = position.y - BLADE_HEIGHT;
				break;
			case MlynarPlayerBlade::Direction::Down:
				blade->position.x = position.x + (width - BLADE_WIDTH) / 2;
				blade->position.y = position.y + height;
				break;
			case MlynarPlayerBlade::Direction::Left:
				blade->position.x = position.x - BLADE_WIDTH;
				blade->position.y = position.y + (height - BLADE_HEIGHT) / 2;
				break;
			case MlynarPlayerBlade::Direction::Right:
				blade->position.x = position.x + width;
				blade->position.y = position.y + (height - BLADE_HEIGHT) / 2;
				break;
			}
		}
	}

protected:
	void UpdateBullets()
	{
		const double RADIAL_SPEED = 0.0045;
		const double TANGENT_SPEED = 0.0055;
		double radian_interval = 2 * 3.14159 / 3;
		double radius = 100 + 25 * sin(GetTickCount() * RADIAL_SPEED);
		for (size_t i = 0; i < 3; i++)
		{
			double radian = GetTickCount() * TANGENT_SPEED + radian_interval * i;
			bullet_list[i]->position.x = position.x + width / 2 + (int)(radius * sin(radian));
			bullet_list[i]->position.y = position.y + height / 2 + (int)(radius * cos(radian));
		}

		const int BLADE_SPEED = 6;
		for (MlynarPlayerBlade* blade : blade_bullets)
		{
			switch (blade->direction)
			{
			case MlynarPlayerBlade::Direction::Up:
				blade->position.y -= BLADE_SPEED;
				break;
			case MlynarPlayerBlade::Direction::Down:
				blade->position.y += BLADE_SPEED;
				break;
			case MlynarPlayerBlade::Direction::Left:
				blade->position.x -= BLADE_SPEED;
				break;
			case MlynarPlayerBlade::Direction::Right:
				blade->position.x += BLADE_SPEED;
				break;
			}
		}
	}

private:
	const int BLADE_WIDTH = 48;
	const int BLADE_HEIGHT = 48;

private:
	Animation* anim_left;
	Animation* anim_right;
	std::vector<MlynarPlayerBlade*> blade_bullets;
};

#endif