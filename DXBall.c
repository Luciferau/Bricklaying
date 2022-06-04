// DXBall.c : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
#include "easyx_c.h"
//int 4字节 -‭2147483648‬~‭2147483647‬
//unsigned int 4字节 0~‭4294967295‬

void start();//启动页面函数声明
void draw_origin(int num[][10]);//游戏起始页面声明
void game();//
void draw_ball(int x, int y, int r, COLORREF color);
void draw_baffle(int x, int y, COLORREF color);
int/*返回类型*/ main/*函数名称*/()
{/*函数体*/
	UINT nID = mopen("背景音乐.mp3"/*音乐文件路径*/, "mpegvideo"/*音乐文件格式*/);
	start();
	game();
	getch(/*没有参数*/);//等待用户按键
	mclose(nID)/*关闭背景音乐*/;
	return 0;
}

void start()//定义/实现
{
	initgraph(400, 280);
	setbkcolor(CYAN);
	cleardevice();
	settextcolor(RED);
	setbkmode(TRANSPARENT);
	settextstyle(30, 0, "楷体");
	outtextsxy(140, 80, "打砖块");
	settextstyle(20, 0, "楷体");
	outtextsxy(130, 180, "版本 1.0");
	outtextsxy(130, 200, "制作 易小晴");
	settextcolor(DARKGRAY);
	outtextsxy(100, 240, "按任意键开始......");
	getch();//等待用户按键
}

int get_baffle_position(int xb)
{
	if (kbhit()) {
		int ch = getch();
		//char c = (char)ch;
		if (ch == 75) {
			xb = (xb - 20 > 0) ? (xb - 20) : 1;
			/*if (xb - 20 > 0) {
				xb -= 20;//没有越界
			}
			else {//发生了越界
				xb = 1;
			}*/
		}
		else if (ch == 77) {
			if (xb + 20 < 578) {
				xb += 20;
			}
			else {
				xb = 578;
			}
		}
		else if (ch == 27) {
			xb = -1;
		}
	}
	return xb;
}
void draw_brick(int i, int j, COLORREF line, COLORREF fill)
{
	setlinecolor(line);
	rectangle(j * 80 + 1, i * 20 + 1, (j + 1) * 80, (i + 1) * 20);
	setfillstyle(BS_SOLID);
	setfillcolor(fill);
	floodfill(j * 80 + 2, i * 20 + 2, line);
}
int check_baffle_collision_detection(
	int x,
	int y,
	int r,
	int num[][10]
)
{
	int i, j;
	int lf = x - r, rg = x + r, t = y - r, bt = y + r;
	for (i = 0; i < 7; i++) {
		for (j = 0; j < 8; j++) {
			if (num[i][j] == 0)continue;
			int left = 1 + j * 80, right = (j + 1) * 80, top = 1 + i * 20, bottom = (i + 1) * 20;
			int status = 0;
			if (((left <= lf) && (lf <= right)) && ((top <= t) && (bottom >= t))) status |= 1;
			if (((left <= rg) && (rg <= right)) && ((top <= t) && (bottom >= t))) status |= 2;
			if (((left <= rg) && (rg <= right)) && ((top <= bt) && (bottom >= bt))) status |= 4;
			if (((left <= lf) && (lf <= right)) && ((top <= bt) && (bottom >= bt))) status |= 8;
			if (status > 0) {
				num[i][j] = 0;
				draw_brick(i, j, LIGHTGREEN, LIGHTGREEN);
			}
			switch (status)
			{//255个
			case 3:
			case 12:
				return 1;
			case 9:
			case 6:
				return 2;
			case 1:
			case 2:
			case 4:
			case 8:
				return 3;
			default:
				break;
			}
		}
	}
	return 0;
}
int game_over_window(int score) {
	char s[64];
	initgraph(440, 280);
	setbkcolor(RED);
	cleardevice();
	settextcolor(BLUE);
	settextstyle(40, 0, "楷体");
	outtextsxy(120, 80, "游戏结束！");
	settextcolor(YELLOW);
	settextstyle(20, 0, "楷体");
	outtextsxy(150, 200, "是否继续？");
	sprintf(s, "您的分数是：%d", score);
	outtextsxy(140, 180, s);
	settextcolor(GREEN);
	outtextsxy(90, 230, "按任意键结束游戏，按y继续……");
	return getch() == 'y';
}
void game() {//游戏主函数
	int num[10][10];//砖块状态值（0表示已经消失1表示仍然存在)
	int x = 200, y = 400, r = 10, dx = -2, dy = -2;
	int xb = 320;
	int score = 0;//打砖块的积分
	draw_origin(num);
	do {
		draw_ball(x, y, r, LIGHTGREEN);
		draw_baffle(xb, 400, LIGHTGREEN);//清除挡板
		int position = check_baffle_collision_detection(x, y, r, num);
		if (position > 0) {
			PlaySound(".\\打击声.wav", NULL, SND_FILENAME | SND_ASYNC);
			score += 10;
			switch (position) {//int short long char unsigned前缀
				//float double void*
			case 1://垂直碰撞
				dy = -dy;
				break;
			case 3://角碰撞
				dy = -dy;
			case 2://水平碰撞
				dx = -dx;
				break;
			}
		}
		x += dx;//x = x+dx;
		y += dy;//y = y+dy;
		xb = get_baffle_position(xb);
		if (xb == -1) {
			int ret = game_over_window(score);
			if (ret == 1) {
				x = 200, y = 400, dx = -2, dy = -2;
				xb = 320, score = 0;
				draw_origin(num);
				continue;
			}
			break;
		}
		if ((y - r < 400) && (400 < y + r)) {
			if ((x - r > xb) && (xb < x + 60 + r)) {
				dy = -dy; //改变球的方向
				y = 400 - 1 - r;//将球移到挡板的上面
				PlaySound(".\\接球声.wav", NULL, SND_FILENAME | SND_ASYNC);//播放接球音效
			}
		}
		if ((x <= r + 4) || (x >= 636 - r)) {//与&& 或|| 非!
			dx = -dx;
		}
		if (y <= r + 4)dy = -dy;
		if (y > 440) {
			int ret = game_over_window(score);
			if (ret == 1) {
				x = 200, y = 400, dx = -2, dy = -2;
				xb = 320, score = 0;
				draw_origin(num);
				continue;
			}
			break;
		}
		draw_baffle(xb, 400, RED);
		draw_ball(x, y, r, BROWN);
		Sleep(15);
	} while (1);
}
void draw_baffle(int x, int y, COLORREF color)//形参是在实现的时候
{
	setlinecolor(color);
	rectangle(x, y, x + 60, y + 6);
	setfillstyle(BS_SOLID);
	setfillcolor(color);
	floodfill(x + 2, y + 2, color);
}
void draw_ball(int x, int y, int r, COLORREF color)
{
	setlinecolor(color);
	circle(x, y, r);
	setfillstyle(BS_SOLID);
	setfillcolor(color);
	floodfill(x + 2, y + 2, color);
}
void draw_origin(
	int num[/*10可填可不填*/][10/*必须填*/]
) {//绘制游戏起始页面
	int i, j;
	HWND hWnd = initgraph(640, 480);//图形系统初始化
	setbkcolor(LIGHTGREEN);
	cleardevice();
	//绘制游戏边界
	setlinecolor(BLUE);//设置线条颜色
	rectangle(0, 0, 639, 479);
	//绘制砖块
	for (j = 0; j < 7; j++) {
		for (i = 0; i < 8; i++)
		{//for//for 不要达到256次
			draw_brick(j, i, RED, LIGHTRED);
			num[j][i] = 1;//记录砖块的存在与否1表示存在
		}
	}
	//挡板
	draw_baffle(320, 400, RED);//实参是在调用的时候
	//小球
	draw_ball(200, 400, 10, BROWN);
}