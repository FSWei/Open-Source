//�̳̣�https://permadi.com/1996/05/ray-casting-tutorial-table-of-contents/

//����ģ�math��������Ǻ������ö����ǻ����ƣ������Ұ�ҹ���賿������bug
//ע�����ļ��Ļ���������ϵ�ڻ���ϵ��Ϊ0��ʱ���ɫ����y�Ḻ������˳ʱ��Ϊ������
#define _CRT_SECURE_NO_WARNINGS 1

#include <graphics.h>
#include<conio.h>
#include<math.h>
#include<stdio.h>

using namespace std;

#define pi 3.141592

typedef struct pos//λ����Ϣ���������Ͻǣ�
{
	int x;//������
	int y;//������
}pos;

typedef struct cube//������
{
	int length = 64;//�����峤
	int width = 64;//�������
	int height = 64;//�������
	pos pos;//������λ�ã�Ҳ���Ե�����������
}cube;

typedef struct player//����
{
	int height = 32;//�����
	float FOV = 60;//�����ӿ�field of view����60��
	pos pos;//����λ��
	float angel = 0;//�����泯����
	int DPP = 255;//���ﵽͶӰƽ��ľ��루Distance to the Projection Plane��
	int move_speed = 5;//�ƶ��ٶ�
	int rotate_speed = 5;//��ת�ٶ�
}player;

typedef struct scene//ͶӰ����
{
	int length = 320;//��������
	int height = 200;//�����߶�
}scene;

#define LENGTH 10//��ͼ���ȣ�Ϊ���������峤��
#define WIDTH 8//��ͼ��ȣ�Ϊ�����������
typedef struct map//��ͼ
{
	bool iscube[WIDTH][LENGTH];//ĳ��λ���Ƿ��������壨λ����Ϣ��Ҫ�ڹ��캯���ﶨ�壩

	map()//���캯��	
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

cube c;//����һ��������
map m;//����һ����ͼ
player p;//����һ����ɫ
scene s;//����һ��ͶӰ����


float distance_cal(int x, int y, float angel)//�������
{
	float distance = sqrt((p.pos.x - x) * (p.pos.x - x) + (p.pos.y - y) * (p.pos.y - y));//���ɶ���//����֮ǰ�õ�cos������һ�����⣬�����
	distance = distance * cos((-angel) / 180 * pi);//�������ЧӦ
	return distance;
}

float level_check2(int x, int y, float angel)//���ڵݹ�
{
	pos a = { x,y };
	pos tmp = {};
	pos tmp2 = {};
	//����仯����
	if (p.angel + angel > 90 && p.angel + angel < 270)//���߳��£����������ߣ�ע����������ϵ��ͬ������ķ�ΧҲ��ͬ�����������������������ͬ��
		tmp.y = c.width;
	else//���߳��ϣ�����������
		tmp.y = -c.width;

	tmp.x = - tmp.y / tan((90 - (p.angel + angel)) / 180 * pi);//����仯����
	//tmp.x = c.width / tan((90 - (p.angel+angel)) / 180 * pi);//����仯����
	a.x = a.x + tmp.x;
	a.y = a.y + tmp.y;
	tmp2.x = a.x;
	tmp2.y = a.y;
	a.x = a.x / c.length;//��a.x������������
	a.y = a.y / c.width;//��a.y�������������

	if (a.x >= 0 && a.x < LENGTH && a.y >= 0 && a.y < WIDTH)//���û�г�����ͼ�߽�
	{
		if (!m.iscube[a.y][a.x])//���û��ǽ
		{
			return level_check2(tmp2.x, tmp2.y, angel);//ȥ������һ��
		}
		if (m.iscube[a.y][a.x])//�����ǽ
		{
			return distance_cal(tmp2.x, tmp2.y, angel);//�������
		}
	}
	return -1;//���������Խ����ͼ�߽磬�򷵻�-1
}

//������ˮƽ�����ߵĽ��㣺
//�����ߵ�������Ϊ������Ŀ�ȣ���ֱ���ȣ��ı�����ʱ����if������Ƿ�����ǽ��
//����ǽ����ͣ����������룬û��������Ѱ����һ����ˮƽ�����ߵĽ���
float level_check(float angel)
{
	pos a{};//����һ���������
	pos tmp = {};
	if (p.angel + angel > 90 && p.angel + angel < 270)//������߳���
		a.y = p.pos.y / 64 * 64 + 64;//���������������64�ٳ�64��Ϊ�˵õ���������������������
	else//������߳���
		a.y = p.pos.y / 64 * 64 - 1;//-1����a���������ϵĿ�

	if ((p.angel + angel) / 90 == 0)//��Ϊtan pi/2 * 2n == 0��������Ϊ������tan pi/2 * n���Զ�ȡ��ֵ
		return -1;
	else
	a.x = p.pos.x + (p.pos.y - a.y) / tan((90 - (p.angel+angel)) / 180 * pi);//�õ����������
	tmp.x = a.x;
	tmp.y = a.y;
	a.y = a.y / c.width;//������ת��Ϊ��������
	a.x = a.x / c.length;

	if (a.x >= 0 && a.x < LENGTH && a.y >= 0 && a.y < WIDTH)//���û�г�����ͼ�߽�
	{
		if (!m.iscube[a.y][a.x])//���û��ǽ
		{
			return level_check2(tmp.x, tmp.y, angel);//ȥ������һ��
		}
		if (m.iscube[a.y][a.x])//�����ǽ
		{
			return distance_cal(tmp.x, tmp.y, angel);//�������
		}
	}
	return -1;//���������Խ����ͼ�߽磬�򷵻�-1

}


float vertical_check2(int x, int y, float angel)//���ڵݹ�
{
	pos a = { x,y };
	pos tmp = {};
	pos tmp2 = {};
	//����仯����
	if (p.angel + angel > 0 && p.angel + angel < 180)//���߳��ң�����������
		tmp.x = c.length;
	else//���߳�������������
		tmp.x = -c.length;

	tmp.y =  - tmp.x * tan((90 - (p.angel + angel)) / 180 * pi);//����仯����
	//tmp.y = -c.length * tan((90 - (p.angel + angel)) / 180 * pi);//����仯����
	//tmp.y = c.length * tan((90 - (p.angel + angel)) / 180 * pi);//����仯����
	a.x = a.x + tmp.x;
	a.y = a.y + tmp.y;
	tmp2.x = a.x;
	tmp2.y = a.y;
	a.x = a.x / c.length;//��a.x������������
	a.y = a.y / c.width;//��a.y�������������

	if (a.x >= 0 && a.x < LENGTH && a.y >= 0 && a.y < WIDTH)//���û�г�����ͼ�߽�
	{
		if (!m.iscube[a.y][a.x])//���û��ǽ
		{
			return vertical_check2(tmp2.x, tmp2.y, angel);//ȥ������һ��
		}
		if (m.iscube[a.y][a.x])//�����ǽ
		{
			return distance_cal(tmp2.x, tmp2.y, angel);//�������
		}
	}
	return -1;//���������Խ����ͼ�߽磬�򷵻�
}

//�����봹ֱ�����ߵĽ���
float vertical_check(float angel)
{
	pos a{};//����һ���������
	pos tmp = {};
	if (p.angel + angel > 0 && p.angel + angel < 180)//������߳���
		a.x = p.pos.x / 64 * 64 + 64;//���������������64�ٳ�64��Ϊ�˵õ���������������������
	else//������߳���
		a.x = p.pos.x / 64 * 64 - 1;//-1����a����������Ŀ�

	if ((p.angel + angel) / 90 == 0)//��������ǽ�ɫ��ǰ�����ҷ�����ô�����жϣ�ֱ�Ӹ�distance����-1�ͺã�������������������Ϊtan 90�ᱻ�Զ����ɼ�ֵ
		return -1;
	else
		a.y = p.pos.y + (p.pos.x - a.x) * tan((90 - (p.angel+angel)) / 180 * pi);//�õ�����������
	tmp.x = a.x;
	tmp.y = a.y;
	a.x = a.x / c.length;//��a.x������������
	a.y = a.y / c.width;//��a.y�������������

	if (a.x >= 0 && a.x < LENGTH && a.y >= 0 && a.y < WIDTH)//���û�г�����ͼ�߽�
	{
		if (!m.iscube[a.y][a.x])//���û��ǽ
		{
			return vertical_check2(tmp.x, tmp.y, angel);//ȥ������һ��
		}
		if (m.iscube[a.y][a.x])//�����ǽ
		{
			return distance_cal(tmp.x, tmp.y, angel);//�������
		}
	}
	return -1;//���������Խ����ͼ�߽磬�򷵻�

}

void scene_draw()//����ͶӰ
{
	float angel = -p.FOV / 2;//�Ƕȳ�ʼֵΪ ���� �ӽǷ�Χ��һ��
	////p.pos.x = 239;//���ڵ���
	//p.pos.y = 420;//���ڵ���
	////p.angel = 30;//���ڵ���
	////angel = 6.9375;//���ڵ���
	//angel = 22.125;//���ڵ���
	////angel = 22.125+60/320;//���ڵ���
	//p.angel = 180;
	//angel = -26.625 + 60.0 / 320;


	setlinecolor(BLACK);//���û�����ɫ
	for (int i = 0; i < s.length; i++, angel = angel + p.FOV / s.length)//ѭ������Ϊ ͶӰ�����Ŀ�ȵ����ظ���	�Ƕ�����ΪFOV/ͶӰ������
		//for (int i = 0; i < s.length; i++)//���ڵ���
	{
		float tmp1 = level_check(angel);//tmp1�洢ˮƽ��ļ��ֵ
		float tmp2 = vertical_check(angel);//tmp2�洢��ֱ��ļ��ֵ
		float distance = -1;
		if (tmp1 != -1 || tmp2 != -1)//���tmp1��tmp2������һ��������-1
			distance = tmp1 > tmp2 ? tmp1 : tmp2;//�͸�distance�������еĽϴ�ֵ
		if (tmp1 != -1 && tmp2 != -1)//���tmp1��tmp2��������-1
			distance = tmp1 < tmp2 ? tmp1 : tmp2;//�͸�distance�������еĽ�Сֵ����ǰ�渳�Ľϴ�ֵ���ǵ���
		if (tmp1 == -1 && tmp2 == -1)//���û�鵽ǽ�������ߣ�ֱ���´�ѭ��
			continue;

		//////////��Ϊˮƽ�������ֱ����Ļ�ͼ���г�ͻ������ֱ��һ��ǽ��ʱ���Ȳ鵽��ˮƽ�������壬�Ͳ������ֱ�ˣ��������������·���
		//float distance = level_check(angel);//�ȼ��ˮƽ����
		//if (distance == -1)//���ˮƽ����û�鵽ǽ
		//	distance = vertical_check(angel);//������ֱ������
		//if (distance == -1)//�����ֱ����Ҳû�鵽ǽ
		//	continue;//�����ߣ�ֱ���´�ѭ��


		int height = c.height / distance * p.DPP;//������ͶӰ�߶� = ������ʵ�ʸ߶� / �˵�������ľ��� * �˵�ͶӰ��ľ���

		printf("x:%d,y:%d,p.angel:%f,angel:%f,distance:%f,%d,%d,%d,%d\n", p.pos.x,p.pos.y,p.angel, angel, distance, i, s.height / 2 - height / 2, i, s.height / 2 + height / 2);//���ڵ���

		line(i, s.height / 2 - height / 2, i, s.height / 2 + height / 2);//����
		//�ߵĺ�������i��������������ͶӰƽ������ - ������ͶӰ�߶ȵ�һ�룬�ײ���������ͶӰƽ������ + ������ͶӰ�߶ȵ�һ��

	}
	FlushBatchDraw();//���������ͼ
	//saveimage(_T("fsw.png"));//����ͼƬ�����ڵ���
}


void player_move()//ע������Ĵ���ƶ��������������෴����Ϊ�����������y�Ḻ����Ϊ��㣬��˳ʱ��Ϊ�����������ϵ
{
	char key = _getch();
	float tmpy = cos((p.angel) / 180 * pi) * p.move_speed;
	int movy = 0;
	if (tmpy != 0)
	{
		if (tmpy > 0.5)//������������ƶ�		//�ҷ�������cos(90)������0����������tmpy > 0�ĳ�tmpy > 0.5����Ȼ��X��ˮƽ�����ƶ�ʱY��������Ȼ�ı�
			movy = ceil(tmpy);//ceil() - ����ȡ��		//������ȡ��
		else//������������ƶ�
			movy = floor(tmpy);//������ȡ��
	}
	float tmpx = sin((p.angel) / 180 * pi) * p.move_speed;
	int movx = 0;
	if (tmpx != 0)
	{
		if (tmpx > 0.5)//������������ƶ�
			movx = ceil(tmpx);//ceil() - ����ȡ��		//������ȡ��
		else
			movx = floor(tmpx);
	}
	//����ȡ����Ϊ�˷�ֹ��ɫ�������ǰ�������ĸ��������ƶ�����ľ���ֵС��1��ʱ���Զ�ȡ0
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
void map_show()//չʾ��ͼ
{
	initgraph(LENGTH * c.length, WIDTH * c.width);//��ʼ�����ڣ����崰�ڳ���
	setbkcolor(WHITE);//���ñ�����ɫΪ��ɫ
	cleardevice();//�õ�ǰ����ɫ��ջ�ͼ�豸

	setlinecolor(BLACK);//���û�����ɫΪ��ɫ
	setfillcolor(BLACK);//���������ɫΪ��ɫ
	for (int i = 0; i < WIDTH; i++)//��ʼ�����ϰ�������壩
	{
		for (int j = 0; j < LENGTH; j++)
		{
			if (m.iscube[i][j] == 1)
			{
				fillrectangle(j * c.length,//���������� * ��λ����
					i * c.width,//���ζ������� * ��λ���
					j * c.length + c.length,//�����Ҳ����� = ������ * ��λ���� + ��λ����
					i * c.width + c.width);//���εײ����� = �������� * ��λ���� + ��λ����
			}
		}
		
	}

	_getch();
	return;
}



void test()
{
	initgraph(s.length, s.height, SHOWCONSOLE);//ͬʱ��ʾ����̨���ڣ����ڵ���
	//initgraph(s.length, s.height);
	setbkcolor(WHITE);
	cleardevice();
	p.pos.x = c.length * LENGTH / 2;
	p.pos.y = c.width * WIDTH / 2;
	p.angel = 0;
	while (1)
	{
		BeginBatchDraw();//������ͼ
		//scene_draw();
		player_move();
		EndBatchDraw();//����������ͼ
	}
}
int main()
{
	map_show();
	test();
}