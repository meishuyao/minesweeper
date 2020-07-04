#include "minemap.h"

Minemap::Minemap(int col,int row, Win_para* ww):opened_box(0),w(ww),mine_last(w->total_num)//输入初始点击坐标
{
	int total_box = w->box_m * w->box_n;
	map.resize(total_box);
	srand((int)time(0));
	std::vector<int> around=get_around_box(row,col);
	around.push_back(coo(row, col));
	int i=0;
	while (i< w->total_num) {
		int n=(rand()%(total_box));
		if(is_in_vector(around,n))
			continue;
		if(map[n]!=CLOSE_BOOM)
		{
			map[n]=CLOSE_BOOM;
			i++;
		}
	}
	start_time= clock();
}

int Minemap::get_mine_last()
{
	return mine_last;
}

int Minemap::get_time()
{
	clock_t end = clock();
	return (end - start_time ) / CLOCKS_PER_SEC;
}

double Minemap::get_time_precise()
{
	clock_t end = clock();
	return (double)(end - start_time) / CLOCKS_PER_SEC;
}


bool Minemap::leftClicked (int col,int row, Win_para* w)
{
	int num=coo(row,col);
	switch (map[num])
	{
		case CLOSE_BOOM:
			map[num]=BOOM;
			putimage(col * w->box_length, row * w->box_length + w->title_height, w->i_n + 11);	//draw boom
			return true;
		case CLOSE_NO_BOOM:
		{
			opened_box++;
			 std::vector<int> around=get_around_box(row,col);
			 int around_num=0;
			 for(int i:around)
				 if(map[i]==CLOSE_BOOM||map[i]==FLAG_BOOM)
					 around_num++;
			 map[num]=(STATE)(around_num+1);    //STATE中雷数与编号相差1
			 draw_box(col, row, map[num], w);
			 if(around_num==0)
				 for(int i:around)
					 leftClicked(i%w->box_n,i/ w->box_n, w);
			 return false;
		}
		case OPEN_ONE: case OPEN_TWO: case OPEN_THREE: case OPEN_FOUR:
		case OPEN_FIVE: case OPEN_SIX: case OPEN_SEVEN: case OPEN_EIGHT:
		{
			std::vector<int> around = get_around_box(row, col);
			int around_num = 0;
			for (int i : around)
				if (map[i] == FLAG_NO_BOOM || map[i] == FLAG_BOOM)
					around_num++;
			bool b = 0;
			if (around_num+1 == map[num])       //STATE中雷数与编号相差1
			{
				for (int i : around)
					if (map[i] == CLOSE_BOOM || map[i] == CLOSE_NO_BOOM) 
						b+=leftClicked(i % w->box_n, i / w->box_n, w);
			}
			return b;
		}
		default:
			return false;
	}

}

void Minemap::rightClicked(int col,int row, Win_para* w)
{
	int num=coo(row,col);
	switch (map[num])
	{
	case FLAG_BOOM:
		map[num]=CLOSE_BOOM;
		draw_box(col, row, CLOSE_BOOM, w);
		mine_last++;
		return;
	case FLAG_NO_BOOM:
		map[num]=CLOSE_NO_BOOM;
		draw_box(col, row, CLOSE_NO_BOOM, w);
		mine_last++;
		return;
	case CLOSE_BOOM:
		map[num]=FLAG_BOOM;
		draw_box(col, row, FLAG_BOOM, w);
		mine_last--;
		return;
	case CLOSE_NO_BOOM:
		map[num]=FLAG_NO_BOOM;
		draw_box(col, row, FLAG_NO_BOOM, w);
		mine_last--;
		return;
	default:
		return;
	}
}
STATE Minemap::get_state(int col,int row)
{
	return map[coo(row,col)];
}

bool Minemap::is_win()
{
	if(opened_box==w->box_m* w->box_n- w->total_num)
		return true;
	else
		return false;
}

int Minemap::coo(int row,int col)//返回编号
{
	return row* w->box_n+col;
}

std::vector<int> Minemap::get_around_box(int m,int n)//返回周围一圈编号,row,col
{
	std::vector<int> around;
	if(n>0)
		around.push_back(coo(m,n-1));
	if(n<w->box_n-1)
		around.push_back(coo(m,n+1));
	if(m>0)
	{
		around.push_back(coo(m-1,n));
		if(n>0)
			around.push_back(coo(m-1,n-1));
		if(n<w->box_n-1)
			around.push_back(coo(m-1,n+1));
	}
	if(m<w->box_m-1)
	{
		around.push_back(coo(m+1,n));
		if(n>0)
			around.push_back(coo(m+1,n-1));
		if(n<w->box_n-1)
			around.push_back(coo(m+1,n+1));
	}
	return around;
}

Win_para::Win_para ():box_n(30), box_m(16), total_num(99) , box_length(30),title_height(90),title_left(0)
{
	//std::ifstream settings("D:/settings", std::ios_base::binary);
	std::ifstream settings("./settings", std::ios_base::binary);
	if (settings) {
		settings.read(reinterpret_cast<char*>(&box_n), sizeof(int));
		settings.read(reinterpret_cast<char*>(&box_m), sizeof(int));
		settings.read(reinterpret_cast<char*>(&total_num), sizeof(int));
		settings.read(reinterpret_cast<char*>(&rank), sizeof(Rank));
	}

	loadimage(i_n+9, _T("./image/u.bmp"));			//release
	loadimage(i_n+10, _T("./image/f.bmp"));
	loadimage(i_n, _T("./image/b.bmp"));
	loadimage(i_n + 1, _T("./image/1.bmp"));
	loadimage(i_n + 2, _T("./image/2.bmp"));
	loadimage(i_n + 3, _T("./image/3.bmp"));
	loadimage(i_n + 4, _T("./image/4.bmp"));
	loadimage(i_n + 5, _T("./image/5.bmp"));
	loadimage(i_n + 6, _T("./image/6.bmp"));
	loadimage(i_n + 7, _T("./image/7.bmp"));
	loadimage(i_n + 8, _T("./image/8.bmp"));
	loadimage(i_n + 11, _T("./image/boom.bmp"));
	 /*loadimage(i_n + 9, _T("D:/image/u.bmp"));			//debug
	loadimage(i_n + 10, _T("D:/image/f.bmp"));
	loadimage(i_n, _T("D:/image/b.bmp"));
	loadimage(i_n + 1, _T("D:/image/1.bmp"));
	loadimage(i_n + 2, _T("D:/image/2.bmp"));
	loadimage(i_n + 3, _T("D:/image/3.bmp"));
	loadimage(i_n + 4, _T("D:/image/4.bmp"));
	loadimage(i_n + 5, _T("D:/image/5.bmp"));
	loadimage(i_n + 6, _T("D:/image/6.bmp"));
	loadimage(i_n + 7, _T("D:/image/7.bmp"));
	loadimage(i_n + 8, _T("D:/image/8.bmp"));
	loadimage(i_n + 11, _T("D:/image/boom.bmp"));*/

	if (box_n > 30)
	{
		title_height = (box_n * box_length) / 10;
		/*loadimage(i_n + 12, _T("D:/image/title.bmp"),box_n*box_length,title_height);
		loadimage(i_n + 14, _T("D:/image/no_music.bmp"), title_height / 3, title_height / 3);
		loadimage(i_n + 13, _T("D:/image/music.bmp"), title_height / 3, title_height / 3);*/
		loadimage(i_n + 12, _T("./image/title.bmp"), box_n * box_length, title_height);
		loadimage(i_n + 14, _T("./image/no_music.bmp"), title_height / 3, title_height / 3);
		loadimage(i_n + 13, _T("./image/music.bmp"), title_height / 3, title_height / 3);
	}
	else
	{
		title_left = (box_n * box_length - 900) / 2;
		/*loadimage(i_n + 12, _T("D:/image/title.bmp"));
		loadimage(i_n + 14, _T("D:/image/no_music.bmp"));
		loadimage(i_n + 13, _T("D:/image/music.bmp"));*/
		loadimage(i_n + 12, _T("./image/title.bmp"));
		loadimage(i_n + 14, _T("./image/no_music.bmp"));
		loadimage(i_n + 13, _T("./image/music.bmp"));
	}

	////时间雷数位置r1,r2
	if (title_left < -30)
	{
		if (title_left <= -270)
		{
			r1 = { 0, 20,75 ,70 };
			r2 = { box_n * box_length - r1.right, r1.top ,box_n * box_length - r1.left ,r1.bottom };
		}
		else
		{
			r1 = { 30, 20,105 ,70 };
			r2 = { box_n * box_length - r1.right, r1.top ,box_n * box_length - r1.left ,r1.bottom };
		}
	}
	else
	{
		r1 = { title_height * 2 / 3, title_height * 2 / 9,title_height * 3 / 2,title_height * 7 / 9 };
		r2 = { box_n * box_length - r1.right, r1.top ,box_n * box_length - r1.left ,r1.bottom };
	}

}


bool is_in_vector(const std::vector<int>& a, int n)
{
	for (int i : a)
		if (i == n)
			return true;
	return false;
}

void draw_box(int col, int row, STATE s, Win_para* w)
{
	int x = col * w->box_length;
	int y = row * w->box_length + w->title_height;
	switch (s)
	{
	case OPEN_NO: {
		putimage(x, y, w->i_n);
		break;
	}
	case OPEN_ONE: {
		putimage(x, y, w->i_n);
		putimage(x + 5, y + 5, w->i_n + 1);
		break;
	}
	case OPEN_TWO: {
		putimage(x, y, w->i_n);
		putimage(x + 5, y + 5, w->i_n + 2);
		break;
	}
	case OPEN_THREE: {
		putimage(x, y, w->i_n);
		putimage(x + 5, y + 5, w->i_n + 3);
		break;
	}
	case OPEN_FOUR: {
		putimage(x, y, w->i_n);
		putimage(x + 5, y + 5, w->i_n + 4);
		break;
	}
	case OPEN_FIVE: {
		putimage(x, y, w->i_n);
		putimage(x + 5, y + 5, w->i_n + 5);
		break;
	}
	case OPEN_SIX: {
		putimage(x, y, w->i_n);
		putimage(x + 5, y + 5, w->i_n + 6);
		break;
	}
	case OPEN_SEVEN: {
		putimage(x, y, w->i_n);
		putimage(x + 5, y + 5, w->i_n + 7);
		break;
	}
	case OPEN_EIGHT: {
		putimage(x, y, w->i_n);
		putimage(x + 5, y + 5, w->i_n + 8);
		break;
	}
	case FLAG_BOOM:case FLAG_NO_BOOM: {
		putimage(x + 5, y + 5, w->i_n + 10);
		break;
	}
	case CLOSE_BOOM: case CLOSE_NO_BOOM:
		putimage(x, y, w->i_n + 9);
	}
}

