#ifndef _ENEMY_H_
#define _ENEMY_H_

#include "Player.h"

extern const int WINDOW_WIDTH;
extern const int WINDOW_HEIGHT;

extern IMAGE img_shadow_enemy;

extern Player* current_player;

extern Frame* frame_insect_enemy_left;
extern Frame* frame_insect_enemy_right;
extern Frame* frame_boar_enemy_left;
extern Frame* frame_boar_enemy_right;

class Enemy//敌人基类
{
public:
	struct Point
	{
		double x = 0;
		double y = 0;
	};

public:
	Enemy(Frame* frame_left, Frame* frame_right, int interval = 35)
	{
		anim_left = new Animation(frame_left, interval);
		anim_right = new Animation(frame_right, interval);

		enum class SpawnEdge
		{
			Up = 0,
			Down,
			Left,
			Right
		};

		SpawnEdge edge = (SpawnEdge)(rand() % 4);//将敌人放置在地图外边界处的随机位置
		switch (edge)
		{
		case SpawnEdge::Up:
			position.x = rand() % WINDOW_WIDTH;
			position.y = -height;
			break;
		case SpawnEdge::Down:
			position.x = rand() % WINDOW_WIDTH;
			position.y = WINDOW_HEIGHT;
			break;
		case SpawnEdge::Left:
			position.x = -width;
			position.y = rand() % WINDOW_HEIGHT;
			break;
		case SpawnEdge::Right:
			position.x = WINDOW_WIDTH;
			position.y = rand() % WINDOW_HEIGHT;
			break;
		default:
			break;
		}
	}

	~Enemy()
	{
		delete anim_left;
		delete anim_right;
	}

	void CheckBulletCollision()//子弹等效为点，判断点是否在敌人矩形内
	{
		const std::vector<Bullet*>& bullet_list = current_player->GetBullets();
		for (const Bullet* bullet : bullet_list)
		{
			bool is_overlap_x = bullet->position.x >= position.x && bullet->position.x <= position.x + width;
			bool is_overlap_y = bullet->position.y >= position.y && bullet->position.y <= position.y + height;
			if (is_overlap_x && is_overlap_y) Hurt();
		}
	}

	void CheckPlayerCollision()//敌人中心位置等效为点，判断点是否在玩家矩形内
	{
		Point check_position = { position.x + width / 2, position.y + height / 2 };
		const POINT& player_position = current_player->GetPosition();
		bool is_overlap_x = check_position.x >= player_position.x && check_position.x <= player_position.x + current_player->GetWidth();
		bool is_overlap_y = check_position.y >= player_position.y && check_position.y <= player_position.y + current_player->GetHeight();
		if (is_overlap_x && is_overlap_y)
			current_player->Hurt();
	}

	virtual void Move()
	{
		const POINT& player_position = current_player->GetPosition();
		int dir_x = player_position.x - (int)position.x;
		int dir_y = player_position.y - (int)position.y;
		double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);
		if (len_dir != 0)
		{
			double normalized_x = dir_x / len_dir;
			double normalized_y = dir_y / len_dir;
			position.x += speed * normalized_x;
			position.y += speed * normalized_y;
		}
		if (dir_x < 0)
			facing_left = true;
		else if (dir_x > 0)
			facing_left = false;
	}

	virtual void Draw(int delta)
	{
		int pos_shadow_x = (int)position.x + (width / 2 - SHADOW_WIDTH / 2);
		int pos_shadow_y = (int)position.y + height - shadow_offset;
		putimage_alpha(pos_shadow_x, pos_shadow_y, &img_shadow_enemy);

		if (facing_left)
			anim_left->Play((int)position.x, (int)position.y, delta, invincible && is_show_sketch);
		else
			anim_right->Play((int)position.x, (int)position.y, delta, invincible && is_show_sketch);

		invincible_timer -= delta;//更新无敌时间计时器
		invincible = (invincible_timer > 0);

		sketch_switch_counter = (++sketch_switch_counter) % 15;
		if (sketch_switch_counter == 0) is_show_sketch = !is_show_sketch;
	}

	bool CheckAlive()
	{
		return hp > 0;
	}

protected:
	int hp = 100;//生命值
	double speed = 2;//速度
	int shadow_offset = 35;//阴影位置偏移
	bool invincible = false;//是否无敌状态
	int invincible_timer = 0;//无敌状态计时器
	bool is_show_sketch = false;//当前是否显示剪影
	int sketch_switch_counter = 0;//剪影闪烁效果计数器
	Point position = { 0, 0 };//位置
	int width = 96, height = 96;//宽度和高度
	const int SHADOW_WIDTH = 48;//阴影宽度

private:
	const int INVINCIBLE_DELAY = 1000;//敌人受击后无敌时长

private:
	bool alive = true;
	bool facing_left = false;
	Animation* anim_left = nullptr;
	Animation* anim_right = nullptr;

private:
	void Hurt()
	{
		if (invincible) return;
		hp -= 10;
		invincible = true;
		current_player->IncreaseMP();
		invincible_timer = INVINCIBLE_DELAY;
		mciSendString(_T("play hit from 0"), NULL, 0, NULL);
	}
};

class InsectEnemy : public Enemy//蜜蜂敌人
{
public:
	InsectEnemy() : Enemy(frame_insect_enemy_left, frame_insect_enemy_right)
	{
		hp = 10, speed = 3;
		shadow_offset = 10;
	}

	~InsectEnemy() = default;
};

class BoarEnemy : public Enemy//野猪敌人
{
public:
	BoarEnemy() : Enemy(frame_boar_enemy_left, frame_boar_enemy_right)
	{
		hp = 20, speed = 1;
		shadow_offset = 35;
	}

	~BoarEnemy() = default;
};

#endif