#include "Animation.h"
#include "Bullet.h"
#include "Button.h"
#include "Enemy.h"
#include "Frame.h"
#include "HUD.h"
#include "Player.h"
#include "Stage.h"
using namespace std;

static int enemy_spawn_timer = 0;
static int game_time = 0;

static const int WINDOW_WIDTH = 1280;//窗口宽度
static const int WINDOW_HEIGHT = 720;//窗口高度
static const int FPS = 60;// 游戏帧率

bool is_running = true;//游戏是否运行
Stage game_stage = Stage::Menu;//初始化游戏阶段至菜单

Frame* frame_insect_enemy_left = nullptr;//虫敌人向左动画图集
Frame* frame_insect_enemy_right = nullptr;//虫敌人向右动画图集
Frame* frame_boar_enemy_left = nullptr;//野猪敌人向左动画图集
Frame* frame_boar_enemy_right = nullptr;//野猪敌人向右动画图集

Frame* frame_wisadel_player_left = nullptr;//维什戴尔向左动画图集
Frame* frame_wisadel_player_right = nullptr;//维什戴尔向右动画图集

Frame* frame_mlynar_player_left = nullptr;//玛恩那向左动画图集
Frame* frame_mlynar_player_right = nullptr;//玛恩那向右动画图集

Animation* anim_wisadel_player_right = nullptr;//派维什戴尔向右动画
Animation* anim_mlynar_player_right = nullptr;//玛恩那向右动画

vector<Enemy*> enemy_list;//敌人列表

IMAGE img_shadow_enemy;//敌人脚底阴影
IMAGE img_shadow_player;//玩家脚底阴影
IMAGE img_avatar_wisadel;//维什戴尔头像
IMAGE img_avatar_mlynar;//玛恩那头像
IMAGE img_blade_up;//向上的未照耀的荣光
IMAGE img_blade_down;//向下的未照耀的荣光
IMAGE img_blade_left;//向左的未照耀的荣光
IMAGE img_blade_right;//向右的未照耀的荣光
IMAGE img_menu;//主菜单背景图
IMAGE img_background;//游戏内背景图
IMAGE img_F_btn_released;//F键按下图标
IMAGE img_F_btn_pushed;//F键抬起图标

IMAGE* current_avatar = nullptr;//当前头像
Player* current_player = nullptr;//当前玩家

void LoadResources()//加载资源
{
	loadimage(&img_shadow_enemy, _T("img/shadow_enemy.png"));
	loadimage(&img_shadow_player, _T("img/shadow_player.png"));
	loadimage(&img_avatar_wisadel, _T("img/avatar_wisadel.png"));
	loadimage(&img_avatar_mlynar, _T("img/avatar_mlynar.png"));
	loadimage(&img_blade_up, _T("img/attack_up.png"));
	loadimage(&img_blade_down, _T("img/attack_down.png"));
	loadimage(&img_blade_left, _T("img/attack_left.png"));
	loadimage(&img_blade_right, _T("img/attack_right.png"));
	loadimage(&img_menu, _T("img/menu.png"));
	loadimage(&img_background, _T("img/background.png"));
	loadimage(&img_F_btn_released, _T("img/F_btn_released.png"));
	loadimage(&img_F_btn_pushed, _T("img/F_btn_pushed.png"));

	frame_insect_enemy_left = new Frame(_T("img/insect_left_%d.png"), 1);
	frame_insect_enemy_right = new Frame(_T("img/insect_right_%d.png"), 1);

	frame_boar_enemy_left = new Frame(_T("img/boar_left_%d.png"), 6);
	frame_boar_enemy_right = new Frame(_T("img/boar_right_%d.png"), 6);

	frame_wisadel_player_left = new Frame(_T("img/wisadel_left_%d.png"), 1);
	frame_wisadel_player_right = new Frame(_T("img/wisadel_right_%d.png"), 1);

	frame_mlynar_player_left = new Frame(_T("img/mlynar_left_%d.png"), 1);
	frame_mlynar_player_right = new Frame(_T("img/mlynar_right_%d.png"), 1);

	anim_wisadel_player_right = new Animation(frame_wisadel_player_right, 0);
	anim_mlynar_player_right = new Animation(frame_mlynar_player_right, 0);

	mciSendString(_T("open mus/bgm0.mp3 alias bgm0"), NULL, 0, NULL);
	mciSendString(_T("open mus/bgm1.mp3 alias bgm1"), NULL, 0, NULL);
	mciSendString(_T("open mus/hit.wav alias hit"), NULL, 0, NULL);
	mciSendString(_T("open mus/hurt.wav alias hurt"), NULL, 0, NULL);

	enemy_list.clear();
	static int enemy_spawn_timer = 0;
}

void UnloadResources()//卸载资源
{
	delete frame_insect_enemy_left;
	frame_insect_enemy_left = nullptr;
	delete frame_insect_enemy_right;
	frame_insect_enemy_right = nullptr;
	delete frame_boar_enemy_left;
	frame_boar_enemy_left = nullptr;
	delete frame_boar_enemy_right;
	frame_boar_enemy_right = nullptr;

	delete frame_wisadel_player_left;
	frame_wisadel_player_left = nullptr;
	delete frame_wisadel_player_right;
	frame_wisadel_player_right = nullptr;
	delete frame_mlynar_player_left;
	frame_mlynar_player_left = nullptr;
	delete frame_mlynar_player_right;
	frame_mlynar_player_right = nullptr;

	delete anim_wisadel_player_right;
	anim_wisadel_player_right = nullptr;
	delete anim_mlynar_player_right;
	anim_mlynar_player_right = nullptr;

	delete current_player;
	current_player = nullptr;

	for (Enemy* enemy : enemy_list)
		delete enemy;
	enemy_list.clear();
}

void TryGenerateEnemy(int delta)
{
	game_time += delta;//更新游戏时间

	int spawn_interval = 1500;
	int max_enemies = 1;

	if (game_time > 30000) { // 30秒后
		spawn_interval = 1200;
		max_enemies = 2;
	}
	if (game_time > 60000) { // 60秒后
		spawn_interval = 1000;
		max_enemies = 3;
	}
	if (game_time > 90000) { // 90秒后
		spawn_interval = 800;
		max_enemies = 4;
	}

	enemy_spawn_timer += delta;
	if (enemy_spawn_timer >= spawn_interval)
	{
		int rand_val = rand();
		for (int i = 0; i < max_enemies; ++i)
		{
			if (rand_val % 4 == 0) enemy_list.push_back(new BoarEnemy());
			if (rand_val % 2 == 0) enemy_list.push_back(new InsectEnemy());
		}
		enemy_spawn_timer = 0;
	}
}

int main()
{
	LoadResources();
	srand((unsigned int)time(NULL));
	HWND hwnd = initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);//创建窗口
	SetWindowText(hwnd, _T("方舟幸存者 -  大二高程项目试做版"));
	HUD hud;
	ExMessage msg;
	StartGameButton btn_start_game = StartGameButton({ 544, 430, 736, 505 },
		_T("img/ui_start_idle.png"), _T("img/ui_start_hovered.png"), _T("img/ui_start_pushed.png"));
	QuitGameButton btn_quit_game = QuitGameButton({ 544, 550, 736, 742 },
		_T("img/ui_quit_idle.png"), _T("img/ui_quit_hovered.png"), _T("img/ui_quit_pushed.png"));
	WisadelSelectorButton btn_wisadel_selector = WisadelSelectorButton({ 240, 160, 540, 560 });
	MlynarSelectorButton btn_mlynar_selector = MlynarSelectorButton({ 740, 160, 1040, 560 });

	mciSendString(_T("play bgm0 repeat from 0"), NULL, 0, NULL);

	BeginBatchDraw();

	while (is_running)
	{
		DWORD start_time = GetTickCount();
		while (peekmessage(&msg))
		{
			switch (game_stage)
			{
			case Stage::Menu:
				btn_start_game.ProcessEvent(msg);
				btn_quit_game.ProcessEvent(msg);
				break;
			case Stage::Selection:
				btn_wisadel_selector.ProcessEvent(msg);
				btn_mlynar_selector.ProcessEvent(msg);
				break;
			case Stage::Playing:
				current_player->ProcessEvent(msg);
				break;
			}
		}

		static DWORD last_tick = GetTickCount();
		DWORD current_tick = GetTickCount();
		DWORD delta = current_tick - last_tick;
		last_tick = current_tick;

		cleardevice();

		switch (game_stage)
		{
		case Stage::Menu:
			putimage(0, 0, &img_menu);
			btn_start_game.Draw(delta);
			btn_quit_game.Draw(delta);
			break;
		case Stage::Selection:
			putimage(0, 0, &img_background);
			btn_wisadel_selector.Draw(delta);
			btn_mlynar_selector.Draw(delta);
			break;
		case Stage::Playing:
			mciSendString(_T("stop bgm0"), 0, 0, 0);
			current_player->Move();
			TryGenerateEnemy(delta);
			for (Enemy* enemy : enemy_list)
				enemy->Move();
			for (Enemy* enemy : enemy_list)
			{
				enemy->CheckBulletCollision();
				enemy->CheckPlayerCollision();
			}
			for (size_t i = 0; i < enemy_list.size(); i++)
			{
				Enemy* enemy = enemy_list[i];
				if (!enemy->CheckAlive())
				{
					swap(enemy_list[i], enemy_list.back());
					enemy_list.pop_back();
					delete enemy;
				}
			}
			putimage(0, 0, &img_background);
			for (Enemy* enemy : enemy_list)
				enemy->Draw(delta);
			current_player->Draw(delta);
			hud.Draw();
			break;
		}

		FlushBatchDraw();

		if (game_stage == Stage::Playing)
		{
			static DWORD total_time = 0;
			total_time += delta;
			if (current_player->GetHP() <= 0)
			{
				TCHAR text[128];
				_stprintf_s(text, _T("一共坚持了：%d 秒 !"), total_time / 1000);
				MessageBox(GetHWnd(), text, _T("游戏结束"), MB_OK);
				mciSendString(_T("stop bgm1"), 0, 0, 0);
				total_time = 0;
				enemy_spawn_timer = 0;
				game_time = 0;
				for (Enemy* enemy : enemy_list)
				{
					delete enemy;
				}
				enemy_list.clear();
				game_stage = Stage::Menu;
				btn_start_game.Reset();
				btn_quit_game.Reset();
				btn_wisadel_selector.Reset();
				btn_mlynar_selector.Reset();
				mciSendString(_T("play bgm0 repeat from 0"), NULL, 0, NULL);
			}
		}

		DWORD end_time = GetTickCount();
		DWORD delta_time = end_time - start_time;
		if (delta_time < 1000 / FPS)
			Sleep(1000 / FPS - delta_time);
	}

	EndBatchDraw();
	UnloadResources();
}