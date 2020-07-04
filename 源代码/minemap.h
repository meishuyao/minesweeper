#ifndef MINEMAP_H
#define MINEMAP_H

#include<stdlib.h>
#include<time.h>
#include<vector>
#include <graphics.h>
#include <conio.h>
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<iomanip>
#include"AudioClip.h"   //音乐库

enum STATE          //格子状态
{
    CLOSE_NO_BOOM= 0,OPEN_NO , OPEN_ONE, OPEN_TWO, OPEN_THREE, OPEN_FOUR, OPEN_FIVE, OPEN_SIX, OPEN_SEVEN, OPEN_EIGHT,
    BOOM, FLAG_BOOM, FLAG_NO_BOOM, CLOSE_BOOM
};

struct Rank             //排行榜
{
    double low_time[5];
    char low_name[150];
    double mid_time[5];
    char mid_name[150];
    double high_time[5];
    char high_name[150];
};

struct Win_para             //窗口参数
{
    Win_para();

    IMAGE i_n[15];
    int box_n;
    int box_m;
    int total_num;
    int box_length;
    int title_height;
    int title_left;
    Rank rank;
    RECT r1, r2;    //时间雷数位置
};


struct Minemap       //行列编号从0开始
{
    Minemap(int col,int row,Win_para* w);
    bool leftClicked(int col, int row, Win_para* w);
    void rightClicked(int col, int row, Win_para* w);
    STATE get_state(int col,int row);
    bool is_win();
    int coo(int row, int col);      //返回编号
    std::vector<int> get_around_box(int m, int n);//返回周围的格子编号
    int get_mine_last();
    int get_time();
    double get_time_precise();

    //private
    std::vector<STATE> map;
    int opened_box;
    Win_para* w;
    clock_t start_time;
    int mine_last;
};

bool is_in_vector(const std::vector<int>& a,int n);     //辅助函数，查找
void draw_box(int col, int row, STATE s, Win_para* w);  //画一个格子



#endif // MINEMAP_H
