#include<math.h>
#include<stdio.h>
#define pi 3.141592
int main()
{
	float angel = -30.0;
	int i = 0, j = 0, k = 0, l = 0;
	while (angel != 30)
	{
		int ax = 0;
		int px = 64 * 2;
		int ay = 191;
		int py = 64 * 3;
		ax = px + abs(py - ay) / tan((90 - angel) / 180 * pi);
		ay -= 64;
		ax += 64 / tan((90 - angel) / 180 * pi);
		//ay -= 64;
		//ax += 64 / tan((90 - angel) / 180 * pi);
		printf("%d\n", ax);
		ax = ax / 64;
		printf("%d\n", ax);
		printf("%d\n", ay);
		ay = ay / 64;
		printf("%d\n\n", ay);
		angel += 60.0 / 320;
		if (ax == 1)i++;
		if (ax == 2)j++;
		if (ax == 3)k++;
		if (ax == 0)l++;
	}
	printf("%d\n", i);
	printf("%d\n", j);
	printf("%d\n", k);
	printf("%d\n", l);
}	

//int main()
//{
//	int i = 0;
//	for (i = 0; i < 10; i++)
//	{
//		if (i < 5)continue;
//		printf("%d\n", i);
//	}
//}