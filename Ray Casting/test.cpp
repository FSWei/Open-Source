//教程：https://permadi.com/1996/05/ray-casting-tutorial-table-of-contents/

//他妈的，math库里的三角函数作用对象是弧度制，害得我熬夜到凌晨两点找bug
//注：本文件的弧度制坐标系在弧度系数为0的时候角色面向y轴负方向，以顺时针为正方向
#define _CRT_SECURE_NO_WARNINGS 1

#include <graphics.h>
#include<conio.h>
#include<math.h>
#include<stdio.h>

using namespace std;

#define pi 3.141592

typedef struct pos//位置信息（物体左上角）
{
	int x;//横坐标
	int y;//纵坐标
}pos;

typedef struct cube//立方体
{
	int length = 64;//立方体长
	int width = 64;//立方体宽
	int height = 64;//立方体高
	pos pos;//立方体位置，也可以当做网格坐标
}cube;

typedef struct player//人物
{
	int height = 32;//人物高
	float FOV = 60;//人物视宽（field of view）：60度
	pos pos;//人物位置
	float angel = 0;//人物面朝方向
	int DPP = 255;//人物到投影平面的距离（Distance to the Projection Plane）
	int move_speed = 5;//移动速度
	int rotate_speed = 5;//旋转速度
}player;

typedef struct scene//投影场景
{
	int length = 320;//场景长度
	int height = 200;//场景高度
}scene;

#define LENGTH 10//地图长度（为几个立方体长）
#define WIDTH 8//地图宽度（为几个立方体宽）
typedef struct map//地图
{
	bool iscube[WIDTH][LENGTH];//某个位置是否有立方体（位置信息需要在构造函数里定义）

	map()//构造函数	
	{
		for (int i = 0; i < WIDTH; i++)
		{
			iscube[i][0] = 1;
			iscube[i][LENGTH - 1] = 1;
		}
		for (int j = 0; j < LENGTH; j++)
		{
			iscube[0][j] = 1;
			iscube[WIDTH - 1][j] = 1;
		}
		iscube[0][5] = 0;
		iscube[6][2] = 1;
		iscube[1][2] = 1;
		iscube[2][8] = 1;
		iscube[2][7] = 1;
		iscube[2][6] = 1;
		iscube[5][5] = 1;
		iscube[2][3] = 1;
	}
}map;

cube c;//创建一个立方体
map m;//创建一个地图
player p;//创建一个角色
scene s;//创建一个投影场景


float distance_cal(int x, int y, float angel)//计算距离
{
	float distance = sqrt((p.pos.x - x) * (p.pos.x - x) + (p.pos.y - y) * (p.pos.y - y));//勾股定理//这里之前用的cos，出了一堆问题，他妈的
	distance = distance * cos((-angel) / 180 * pi);//抵消鱼缸效应
	return distance;
}

float level_check2(int x, int y, float angel)//用于递归
{
	pos a = { x,y };
	pos tmp = {};
	pos tmp2 = {};
	//纵向变化距离
	if (p.angel + angel > 90 && p.angel + angel < 270)//光线朝下，往正方向走（注意由于坐标系不同，这里的范围也不同于正常情况，下面三个函数同理）
		tmp.y = c.width;
	else//光线朝上，往负方向走
		tmp.y = -c.width;

	tmp.x = - tmp.y / tan((90 - (p.angel + angel)) / 180 * pi);//横向变化距离
	//tmp.x = c.width / tan((90 - (p.angel+angel)) / 180 * pi);//横向变化距离
	a.x = a.x + tmp.x;
	a.y = a.y + tmp.y;
	tmp2.x = a.x;
	tmp2.y = a.y;
	a.x = a.x / c.length;//把a.x变成网格横坐标
	a.y = a.y / c.width;//把a.y变成网格纵坐标

	if (a.x >= 0 && a.x < LENGTH && a.y >= 0 && a.y < WIDTH)//如果没有超出地图边界
	{
		if (!m.iscube[a.y][a.x])//如果没有墙
		{
			return level_check2(tmp2.x, tmp2.y, angel);//去查找下一级
		}
		if (m.iscube[a.y][a.x])//如果有墙
		{
			return distance_cal(tmp2.x, tmp2.y, angel);//计算距离
		}
	}
	return -1;//如果光线已越过地图边界，则返回-1
}

//查找与水平网格线的交点：
//当射线的纵坐标为立方体的宽度（竖直长度）的倍数的时候，用if来检查是否遇到墙壁
//遇到墙壁则停下来计算距离，没有遇到则寻找下一个与水平网格线的交点
float level_check(float angel)
{
	pos a{};//定义一个点代表交点
	pos tmp = {};
	if (p.angel + angel > 90 && p.angel + angel < 270)//如果光线朝下
		a.y = p.pos.y / 64 * 64 + 64;//用人物纵坐标除以64再乘64是为了得到相对于人物最近的立方体
	else//如果光线朝上
		a.y = p.pos.y / 64 * 64 - 1;//-1是让a代表线以上的块

	if ((p.angel + angel) / 90 == 0)//因为tan pi/2 * 2n == 0，不能作为除数，tan pi/2 * n会自动取极值
		return -1;
	else
	a.x = p.pos.x + (p.pos.y - a.y) / tan((90 - (p.angel+angel)) / 180 * pi);//得到交点横坐标
	tmp.x = a.x;
	tmp.y = a.y;
	a.y = a.y / c.width;//把坐标转化为网格坐标
	a.x = a.x / c.length;

	if (a.x >= 0 && a.x < LENGTH && a.y >= 0 && a.y < WIDTH)//如果没有超出地图边界
	{
		if (!m.iscube[a.y][a.x])//如果没有墙
		{
			return level_check2(tmp.x, tmp.y, angel);//去查找下一级
		}
		if (m.iscube[a.y][a.x])//如果有墙
		{
			return distance_cal(tmp.x, tmp.y, angel);//计算距离
		}
	}
	return -1;//如果光线已越过地图边界，则返回-1

}


float vertical_check2(int x, int y, float angel)//用于递归
{
	pos a = { x,y };
	pos tmp = {};
	pos tmp2 = {};
	//横向变化距离
	if (p.angel + angel > 0 && p.angel + angel < 180)//光线朝右，往正方向走
		tmp.x = c.length;
	else//光线朝左，往负方向走
		tmp.x = -c.length;

	tmp.y =  - tmp.x * tan((90 - (p.angel + angel)) / 180 * pi);//纵向变化距离
	//tmp.y = -c.length * tan((90 - (p.angel + angel)) / 180 * pi);//纵向变化距离
	//tmp.y = c.length * tan((90 - (p.angel + angel)) / 180 * pi);//纵向变化距离
	a.x = a.x + tmp.x;
	a.y = a.y + tmp.y;
	tmp2.x = a.x;
	tmp2.y = a.y;
	a.x = a.x / c.length;//把a.x变成网格横坐标
	a.y = a.y / c.width;//把a.y变成网格纵坐标

	if (a.x >= 0 && a.x < LENGTH && a.y >= 0 && a.y < WIDTH)//如果没有超出地图边界
	{
		if (!m.iscube[a.y][a.x])//如果没有墙
		{
			return vertical_check2(tmp2.x, tmp2.y, angel);//去查找下一级
		}
		if (m.iscube[a.y][a.x])//如果有墙
		{
			return distance_cal(tmp2.x, tmp2.y, angel);//计算距离
		}
	}
	return -1;//如果光线已越过地图边界，则返回
}

//查找与垂直网格线的交点
float vertical_check(float angel)
{
	pos a{};//定义一个点代表交点
	pos tmp = {};
	if (p.angel + angel > 0 && p.angel + angel < 180)//如果光线朝右
		a.x = p.pos.x / 64 * 64 + 64;//用人物纵坐标除以64再乘64是为了得到相对于人物最近的立方体
	else//如果光线朝左
		a.x = p.pos.x / 64 * 64 - 1;//-1是让a代表线以左的块

	if ((p.angel + angel) / 90 == 0)//如果光线是角色正前后左右方，那么不用判断，直接给distance返回-1就好，不加这句程序会崩溃，因为tan 90会被自动赋成极值
		return -1;
	else
		a.y = p.pos.y + (p.pos.x - a.x) * tan((90 - (p.angel+angel)) / 180 * pi);//得到交点纵坐标
	tmp.x = a.x;
	tmp.y = a.y;
	a.x = a.x / c.length;//把a.x变成网格横坐标
	a.y = a.y / c.width;//把a.y变成网格纵坐标

	if (a.x >= 0 && a.x < LENGTH && a.y >= 0 && a.y < WIDTH)//如果没有超出地图边界
	{
		if (!m.iscube[a.y][a.x])//如果没有墙
		{
			return vertical_check2(tmp.x, tmp.y, angel);//去查找下一级
		}
		if (m.iscube[a.y][a.x])//如果有墙
		{
			return distance_cal(tmp.x, tmp.y, angel);//计算距离
		}
	}
	return -1;//如果光线已越过地图边界，则返回

}

void scene_draw()//绘制投影
{
	float angel = -p.FOV / 2;//角度初始值为 负的 视角范围的一半
	////p.pos.x = 239;//用于调试
	//p.pos.y = 420;//用于调试
	////p.angel = 30;//用于调试
	////angel = 6.9375;//用于调试
	//angel = 22.125;//用于调试
	////angel = 22.125+60/320;//用于调试
	//p.angel = 180;
	//angel = -26.625 + 60.0 / 320;


	setlinecolor(BLACK);//设置画线颜色
	for (int i = 0; i < s.length; i++, angel = angel + p.FOV / s.length)//循环次数为 投影场景的宽度的像素个数	角度增量为FOV/投影场景宽
		//for (int i = 0; i < s.length; i++)//用于调试
	{
		float tmp1 = level_check(angel);//tmp1存储水平面的检查值
		float tmp2 = vertical_check(angel);//tmp2存储竖直面的检查值
		float distance = -1;
		if (tmp1 != -1 || tmp2 != -1)//如果tmp1和tmp2两者有一个不等于-1
			distance = tmp1 > tmp2 ? tmp1 : tmp2;//就给distance赋两者中的较大值
		if (tmp1 != -1 && tmp2 != -1)//如果tmp1和tmp2都不等于-1
			distance = tmp1 < tmp2 ? tmp1 : tmp2;//就给distance赋两者中的较小值（把前面赋的较大值覆盖掉）
		if (tmp1 == -1 && tmp2 == -1)//如果没查到墙，不绘线，直接下次循环
			continue;

		//////////因为水平方向和竖直方向的绘图会有冲突（查竖直的一面墙的时候，先查到了水平有立方体，就不会查竖直了），所以舍弃以下方法
		//float distance = level_check(angel);//先检查水平方向
		//if (distance == -1)//如果水平方向没查到墙
		//	distance = vertical_check(angel);//就在竖直方向检查
		//if (distance == -1)//如果竖直方向也没查到墙
		//	continue;//不绘线，直接下次循环


		int height = c.height / distance * p.DPP;//立方体投影高度 = 立方体实际高度 / 人到立方体的距离 * 人到投影面的距离

		printf("x:%d,y:%d,p.angel:%f,angel:%f,distance:%f,%d,%d,%d,%d\n", p.pos.x,p.pos.y,p.angel, angel, distance, i, s.height / 2 - height / 2, i, s.height / 2 + height / 2);//用于调试

		line(i, s.height / 2 - height / 2, i, s.height / 2 + height / 2);//画线
		//线的横坐标是i，顶部纵坐标是投影平面中心 - 立方体投影高度的一半，底部纵坐标是投影平面中心 + 立方体投影高度的一半

	}
	FlushBatchDraw();//输出批量绘图
	//saveimage(_T("fsw.png"));//保存图片，用于调试
}


void player_move()//注：这里的大多移动代码和正常情况相反，因为本程序采用以y轴负方向为起点，以顺时针为正方向的坐标系
{
	char key = _getch();
	float tmpy = cos((p.angel) / 180 * pi) * p.move_speed;
	int movy = 0;
	if (tmpy != 0)
	{
		if (tmpy > 0.5)//如果朝正方向移动		//我发现这里cos(90)不等于0，所以这句的tmpy > 0改成tmpy > 0.5，不然在X轴水平方向移动时Y轴坐标仍然改变
			movy = ceil(tmpy);//ceil() - 向上取整		//就向上取整
		else//如果朝负方向移动
			movy = floor(tmpy);//就向下取整
	}
	float tmpx = sin((p.angel) / 180 * pi) * p.move_speed;
	int movx = 0;
	if (tmpx != 0)
	{
		if (tmpx > 0.5)//如果朝正方向移动
			movx = ceil(tmpx);//ceil() - 向上取整		//就向上取整
		else
			movx = floor(tmpx);
	}
	//上面取整是为了防止角色在相对于前后左右四个方向上移动距离的绝对值小于1的时候自动取0
	switch (key)
	{
	case 72:
	case 'w':
	case 'W':
		p.pos.x += movx;
		p.pos.y -= movy;
		break;
	case 80:
	case 's':
	case 'S':
		p.pos.x -= movx;
		p.pos.y += movy;
		break;
	case 77:
	case 'd':
	case 'D':
		p.pos.x += movy;
		p.pos.y += movx;
		break;
	case 75:
	case 'a':
	case 'A':
		p.pos.x -= movy;
		p.pos.y -= movx;
		break;
	case'q':
	case'Q':
		if (p.angel <= 0)
			p.angel = 360;
		p.angel -= p.rotate_speed;
		break;
	case'e':
	case'E':
		if (p.angel >= 360)
			p.angel = 0;
		p.angel += p.rotate_speed;
		break;
	}

	cleardevice();
	scene_draw();
}
void map_show()//展示地图
{
	initgraph(LENGTH * c.length, WIDTH * c.width);//初始化窗口，定义窗口长宽
	setbkcolor(WHITE);//设置背景颜色为白色
	cleardevice();//用当前背景色清空绘图设备

	setlinecolor(BLACK);//设置画线颜色为黑色
	setfillcolor(BLACK);//设置填充颜色为黑色
	for (int i = 0; i < WIDTH; i++)//开始绘制障碍物（立方体）
	{
		for (int j = 0; j < LENGTH; j++)
		{
			if (m.iscube[i][j] == 1)
			{
				fillrectangle(j * c.length,//矩形左部坐标 * 单位长度
					i * c.width,//矩形顶部坐标 * 单位宽度
					j * c.length + c.length,//矩形右部坐标 = 左部坐标 * 单位长度 + 单位长度
					i * c.width + c.width);//矩形底部坐标 = 顶部坐标 * 单位长度 + 单位长度
			}
		}
		
	}

	_getch();
	return;
}



void test()
{
	initgraph(s.length, s.height, SHOWCONSOLE);//同时显示控制台窗口，用于调试
	//initgraph(s.length, s.height);
	setbkcolor(WHITE);
	cleardevice();
	p.pos.x = c.length * LENGTH / 2;
	p.pos.y = c.width * WIDTH / 2;
	p.angel = 0;
	while (1)
	{
		BeginBatchDraw();//批量绘图
		//scene_draw();
		player_move();
		EndBatchDraw();//结束批量绘图
	}
}
int main()
{
	map_show();
	test();
}