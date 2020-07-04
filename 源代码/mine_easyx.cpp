#include "minemap.h"

void check_time(double t, Win_para* w, bool& music_played);      //检查排行榜
void show_record(Rank* r);					//打印排行榜
void change_settings(Win_para* w, Minemap*& game, bool& music_played);						//更改设置
void play_music(Win_para* w, AudioClip* m, bool& music_played, int& music_time);				//音乐
void open_window(Win_para* w, bool& music_played);
void close_window(Win_para* w);
void initgame(Win_para* w, bool music_played);          //初始化画图


int main()
{
	AudioClip ac;					//加载bgm
	bool music_played=false;
	int music_time = 0;
	//ac.load("D:/bgm.mp3");
	ac.load("./bgm.mp3");

	Win_para* w=new Win_para;
	//加载资源
	for (int i = 0; i < 15; i++)
		if ((w->i_n + i)->getheight() == 0)
		{
			std::cout << "Cannot read images!" << std::endl;	//检查资源是否加载
			char c;
			std::cin.get(c);
			return 1;
		}

	// 初始化图形窗口
	open_window(w,music_played);

	MOUSEMSG m;		// 定义鼠标消息
	Minemap* game=nullptr;

	while (true)
	{
		// 获取一条鼠标消息
		m = GetMouseMsg();

		switch (m.uMsg)
		{
		case WM_LBUTTONUP:
			//左键
		{
			if (m.y < w->title_height)
			{
				if (m.y > w->title_height / 3 && m.y < w->title_height * 2 / 3)
				{
					if (m.x > w->title_height * 29 / 6 + w->title_left && m.x < w->title_height * 31 / 6 + w->title_left)
						show_record(&(w->rank));
					else if (m.x > w->title_height * 13 / 3 + w->title_left && m.x < w->title_height * 14 / 3 + w->title_left)
						change_settings(w,game, music_played);
					else if (m.x > w->title_height * 16 / 3 + w->title_left && m.x < w->title_height * 17 / 3 + w->title_left)
						play_music(w,&ac,music_played,music_time);
				}
				break;
			}
			int col = m.x / w->box_length;
			int row = (m.y- w->title_height) / w->box_length;
			if (game == nullptr)
			{
				initgame(w,music_played);
				game = new Minemap(col,row,w);
			}
			if (game->leftClicked(col, row, w))		//lose
			{
				delete game;
				game = nullptr;
				MessageBox(NULL, _T("You lose!"), _T(""), MB_OK|MB_SYSTEMMODAL);
			}
			if (game!=nullptr&&game->is_win())
			{
				double total_time = game->get_time_precise();
				MessageBox(NULL, _T("You win!"), _T(""), MB_OK | MB_SYSTEMMODAL);
				check_time(total_time,w,music_played);
				delete game;
				game = nullptr;
			}
			break;
		}
			
		case WM_RBUTTONUP:
			// 右键
			if (game != nullptr)
				game->rightClicked(m.x / w->box_length,( m.y- w->title_height) / w->box_length, w);
			break;

		}
		//时间和雷数
		if (game != nullptr)
		{
			TCHAR t[5];
			_stprintf_s(t, _T("%03d"), game->get_mine_last());
			drawtext(t, &(w->r1), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			_stprintf_s(t, _T("%03d"), game->get_time());
			drawtext(t, &(w->r2), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}

		if (music_played == true && clock() - music_time > ac.milliseconds() + 1000)		//音乐循环
		{
			ac.stop();
			ac.play();
			music_time = clock();
		}

	}

	// 关闭图形窗口
	close_window(w);
}


void read_name(double t, double time[5], char name[30], Win_para* w, bool& music_played)		//破纪录后读姓名，保存记录
{
	for (int i = 0; i < 5; i++)
		if (t < time[i])
		{
			if (MessageBox(NULL, _T("You broke the record! Do you want to save it?"), _T("Congratulations!"), MB_YESNO | MB_SYSTEMMODAL) == IDNO)
			{
				return;
			}
			closegraph();
			for (int j = 4; j > i; j--)
			{
				(time)[j] = (time)[j - 1];
				strcpy_s((name + j * 30), 28, (name + (j - 1) * 30));
			}
			(time)[i] = t;
			std::cout << "You broke the record!\nplease enter your name:\n";
			std::string s;
			std::cin >> s;
			while (s.length() > 28)
			{
				std::cout << "too long name, less than 28 characters\nyour name:";
				std::cin >> s;
			}
			strcpy_s((name + i * 30), 28, s.c_str());
			//std::ofstream os("D:/settings", std::ios_base::out | std::ios_base::binary);
			std::ofstream os("./settings", std::ios_base::out | std::ios_base::binary);
			os.clear();
			os.write(reinterpret_cast<char*>(&(w->box_n)), sizeof(int));
			os.write(reinterpret_cast<char*>(&(w->box_m)), sizeof(int));
			os.write(reinterpret_cast<char*>(&(w->total_num)), sizeof(int));
			os.write(reinterpret_cast<char*>(&(w->rank)), sizeof(Rank));
			os.close();
			open_window(w, music_played);
			show_record(&(w->rank));
			break;
		}
}

void check_time(double t, Win_para* w, bool& music_played)					//检查是否破纪录
{
	if (w->box_n == 30 && w->box_m == 16 && w->total_num == 99)
	{
		read_name(t, w->rank.high_time, w->rank.high_name, w, music_played);
	}
	else if (w->box_n == 16 && w->box_m == 16 && w->total_num == 40)
	{
		read_name(t, w->rank.mid_time, w->rank.mid_name, w, music_played);
	}
	else if (w->box_n == 9 && w->box_m == 9 && w->total_num == 10)
	{
		read_name(t, w->rank.low_time, w->rank.low_name, w, music_played);
	}
}

void show_record(Rank* r)							//打印记录
{
	setlocale(LC_CTYPE, "chs");
	WCHAR t[400];
	swprintf_s(t, 400, L"初级:\t1\t%.3lf\t%S\n\t2\t%.3lf\t%S\n\t3\t%.3lf\t%S\n\t4\t%.3lf\t%S\n\t5\t%.3lf\t%S\n中级:\t1\t%.3lf\t%S\n\t2\t%.3lf\t%S\n\t3\t%.3lf\t%S\n\t4\t%.3lf\t%S\n\t5\t%.3lf\t%S\n高级:\t1\t%.3lf\t%S\n\t2\t%.3lf\t%S\n\t3\t%.3lf\t%S\n\t4\t%.3lf\t%S\n\t5\t%.3lf\t%S\n"
		, r->low_time[0], r->low_name, r->low_time[1], r->low_name + 30, r->low_time[2], r->low_name + 60, r->low_time[3], r->low_name + 90, r->low_time[4], r->low_name + 120
		, r->mid_time[0], r->mid_name, r->mid_time[1], r->mid_name + 30, r->mid_time[2], r->mid_name + 60, r->mid_time[3], r->mid_name + 90, r->mid_time[4], r->mid_name + 120
		, r->high_time[0], r->high_name, r->high_time[1], r->high_name + 30, r->high_time[2], r->high_name + 60, r->high_time[3], r->high_name + 90, r->high_time[4], r->high_name + 120);
	MessageBox(NULL, t, L"ranking list", MB_OK | MB_SYSTEMMODAL);
}

int read_int(int min, int max)						//设置，读数字
{
	int a;
	char c;
	std::cin.get(c);
	if (!isdigit(c))
	{
		std::cin.ignore(100, '\n');
		std::cout << "请输入范围正确的数字，最小" << min << "，最大" << max << "\n";
		return read_int(min, max);
	}
	std::cin.putback(c);
	std::cin >> a;
	std::cin.ignore(100, '\n');
	if (a<min || a>max)
	{
		std::cout << "请输入范围正确的数字，最小" << min << "，最大" << max << "\n";
		return read_int(min, max);
	}
	return a;
}


void change_settings(Win_para* w, Minemap*& game, bool& music_played)			//设置
{
	if (game != nullptr)
		if (MessageBox(NULL, _T("你想进入设置吗？这将终止正在进行的游戏。"), _T("warning"), MB_YESNO | MB_SYSTEMMODAL) == IDNO)
		{
			return;
		}
		else
		{
			delete game;
			game = nullptr;
		}
	closegraph();
	std::cout << "请选择难度:\nl\t初级\t9*9,10个雷\nm\t中级\t16*16,40个雷\nh\t高级\t30*16,99个雷\ns\t自定义\n";
	char c;
	std::cin.get(c);
	std::cin.ignore(100, '\n');
	while (c != 'l' && c != 'm' && c != 'h' && c != 's')
	{
		std::cout << "请输入正确的字符\n";
		std::cin.get(c);
		std::cin.ignore(100, '\n');
	}
	switch (c)
	{
	case 'l':
		w->box_m = 9;
		w->box_n = 9;
		w->total_num = 10;
		break;
	case 'm':
		w->box_m = 16;
		w->box_n = 16;
		w->total_num = 40;
		break;
	case 'h':
		w->box_n = 30;
		w->box_m = 16;
		w->total_num = 99;
		break;
	case 's':
	{
		std::cout << "宽度:";
		w->box_n = read_int(9, 100);
		std::cout << "高度:";
		w->box_m = read_int(9, 50);
		std::cout << "雷数:";
		w->total_num = read_int(1, w->box_m * w->box_n - 9);
		break;
	}
	}
	//std::ofstream os("D:/settings", std::ios_base::out | std::ios_base::binary);
	std::ofstream os("./settings", std::ios_base::out | std::ios_base::binary);
	os.clear();
	os.write(reinterpret_cast<char*>(&(w->box_n)), sizeof(int));
	os.write(reinterpret_cast<char*>(&(w->box_m)), sizeof(int));
	os.write(reinterpret_cast<char*>(&(w->total_num)), sizeof(int));
	os.write(reinterpret_cast<char*>(&(w->rank)), sizeof(Rank));
	os.close();
	delete w;
	w = new Win_para();
	open_window(w, music_played);
}


void play_music(Win_para* w, AudioClip* m, bool& music_played, int& music_time)		//音乐
{
	if (m->milliseconds() == 0)
	{
		MessageBox(NULL, _T("Can't load background music."), _T("error"), MB_OK | MB_SYSTEMMODAL);
		return;
	}

	if (music_played == false)
	{
		m->play();
		music_time = clock();
		putimage(w->title_height * 16 / 3 + w->title_left, w->title_height / 3, w->i_n + 13);
		music_played = true;
	}
	else
	{
		m->stop();
		putimage(w->title_height * 16 / 3 + w->title_left, w->title_height / 3, w->i_n + 14);
		music_played = false;
	}

}

void open_window(Win_para* w, bool& music_played)
{
	initgraph(w->box_n * w->box_length, w->box_m * w->box_length + w->title_height);
	initgame(w, music_played);
}
void close_window(Win_para* w)
{
	closegraph();
	delete w;
}

void initgame(Win_para* w, bool music_played)
{
	cleardevice();
	putimage(w->title_left, 0, w->i_n + 12);
	if (music_played == false)
		putimage(w->title_height * 16 / 3 + w->title_left, w->title_height / 3, w->i_n + 14);
	for (int i = 0; i < (w->box_m); i++)
		for (int j = 0; j < (w->box_n); j++)
		{
			putimage(j * (w->box_length), i * (w->box_length) + w->title_height, (w->i_n + 9));
		}
	settextcolor(WHITE);
	settextstyle(50, 0, _T("Consolas"));
	drawtext(_T("000"), &(w->r1), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	drawtext(_T("000"), &(w->r2), DT_CENTER | DT_VCENTER | DT_SINGLELINE);

}