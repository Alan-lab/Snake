//Author: Aruyuna
//2018.6.28
#include<stdio.h>
#include<stdlib.h>
#include<conio.h>
#include<windows.h>
#include<time.h>

#define region_x 30
#define region_y 30		//蛇的活动范围

static int Score=0;
static int straight=0;			//方向，0右 1下 2左 3上
static char gameRegion[region_x][region_y];	//游戏区域
static int food_x,food_y;		//食物的位置
int turned=0;		//转身信号量，防止换方向过快导致掉头吃尾巴的bug
int gameover=0;		//gameover信号量，为1则不能操作
int pause=0;		//暂停信号量

void printScreen();

void gotoxy(int x,int y)		//指定位置输出
{
    CONSOLE_SCREEN_BUFFER_INFO    csbiInfo;                            
    HANDLE    hConsoleOut;
    hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hConsoleOut,&csbiInfo);
    csbiInfo.dwCursorPosition.X = x;                                    
    csbiInfo.dwCursorPosition.Y = y;                                    
    SetConsoleCursorPosition(hConsoleOut,csbiInfo.dwCursorPosition);   
}

void putfood(){
	int x,y;
	srand(time(NULL));
	do{
		x=rand()%region_x;
		y=rand()%region_y;	//随机生成食物坐标
	}while(gameRegion[x][y]=='*');	//位置没被蛇身占据时结束
	gameRegion[x][y]='0';	//以字符0代表食物
	gotoxy(y+1,x+2);
	printf("0");
	food_x=x;
	food_y=y;
}

void regionInitial(){
	int i,j;
	for(i=0;i<region_x;i++){
		for(j=0;j<region_y;j++){
			gameRegion[i][j]=' ';	//置空白
		}
	}
	for(i=3;i<7;i++){				//画蛇，以字符*代表蛇身
		gameRegion[3][i]='*';
	}
	putfood();						//放食物，初始化结束
	printScreen();
}

class pos{
public:
	int x;
	int y;
	pos *pre;
	pos *next;
	pos(int xx,int yy){
	x=xx;
	y=yy;
	pre=NULL;
	next=NULL;
	}
};

class snake{
private:
	pos *head, *tail;
public:
	void createSnake(){
		head=new pos(3,6);
		head->next=new pos(3,5);
		head->next->pre=head;
		head->next->next=new pos(3,4);
		head->next->next->pre=head->next;
		head->next->next->next=new pos(3,3);
		head->next->next->next->pre=head->next->next;
		tail=head->next->next->next;
	}
	void destroySnake(){
		pos *tmp=head;
		while(head!=NULL){
			tmp=head;
			head=head->next;
			delete tmp;
			tmp=NULL;
		}
	}
	void printSnake(){
		pos *s=head;
		while(s!=NULL){
			printf("(%d,%d)",s->x,s->y);
			s=s->next;
		}
		printf("\n");
		s=tail;
		while(s!=NULL){
			printf("(%d,%d)",s->x,s->y);
			s=s->pre;
		}
		printf("\n");
	}
	int moveForward(){		//前进，通常返回0，gameover返回1
		int eatFood=0;
		switch(straight){
		case 0:if(gameRegion[head->x][head->y+1]=='0')eatFood=1;break;
		case 1:if(gameRegion[head->x+1][head->y]=='0')eatFood=1;break;
		case 2:if(gameRegion[head->x][head->y-1]=='0')eatFood=1;break;
		case 3:if(gameRegion[head->x-1][head->y]=='0')eatFood=1;break;
		}
		int x=0,y=0;
		if(eatFood==0){					//没吃
			//这里应该用尾巴结点坐标更改为头结点坐标，调一下新尾巴，旧头，新头指针方向。实现待定
			//去尾
			pos *tmp=tail;
			gameRegion[tmp->x][tmp->y]=' ';	//清空该位置
			gotoxy(tmp->y+1, tmp->x+2);		//从屏幕上清掉
			printf(" ");
			tail=tail->pre;
			tail->next=NULL;
			//加头
			switch(straight){
			case 0:
				x=head->x;
				y=head->y+1;
				break;
			case 1:
				x=head->x+1;
				y=head->y;
				break;
			case 2:
				x=head->x;
				y=head->y-1;
				break;
			case 3:
				x=head->x-1;
				y=head->y;
				break;
			}
			if(x>=region_x || y>=region_y || x<0 || y<0 || gameRegion[x][y]=='*'){
				gameover=1;
				return 1;
			}
			tmp->x=x;
			tmp->y=y;
			tmp->next=head;
			head->pre=tmp;
			head=tmp;
			tmp->pre=NULL;
			gameRegion[head->x][head->y]='*';
			gotoxy(head->y+1,head->x+2);
			printf("*");
		}
		else{
			//吃了食物，加头
			int new_x,new_y;
			switch(straight){
			case 0:
				new_x=head->x;
				new_y=head->y+1;
				break;
			case 1:
				new_x=head->x+1;
				new_y=head->y;
				break;
			case 2:
				new_x=head->x;
				new_y=head->y-1;
				break;
			case 3:
				new_x=head->x-1;
				new_y=head->y;
				break;
			}
			head->pre=new pos(new_x,new_y);
			head->pre->next=head;
			head=head->pre;
			gameRegion[head->x][head->y]='*';
			gotoxy(head->y+1,head->x+2);
			printf("*");
			Score++;
			gotoxy(6,0);
			printf("%d",Score);
			putfood();
		}
		return 0;
	}
};

snake s;

void printScreen(){			//画版面
	int i,j;
	system("cls");						//清除屏幕
	printf("Score 0\t\t操作提示：W向上，A向左，S向下，D向右，P暂停游戏\n");		//第一行写分数
	for(i=0;i<region_y+2;i++)printf("#");	//以#号代表边界
	printf("\n");
	for(i=0;i<region_x;i++){
		printf("#");
		for(j=0;j<region_y;j++)printf("%c",gameRegion[i][j]);
		printf("#\n");
	}										//画正体
	for(i=0;i<region_y+2;i++)printf("#");	//底部边界
	printf("\n");
}


int difficulty(){
	printf("选择游戏难度：\n");
	printf("1.Easy（蛇移动极慢）\n");
	printf("2.Normal（蛇移动速度中等）\n");
	printf("3.Hard（蛇移动较快）\n");
	printf("4.Expert（蛇移动极快）\n");
	printf("5.Master（蛇一直在动）\n");
	int t;
	while(1){
		scanf("%d",&t);
		switch(t){
		case 1:return 1000;
		case 2:return 500;
		case 3:return 250;
		case 4:return 100;
		case 5:return 50;
		default:break;
		}
		printf("重输：");
	}
}

//利用多线程表示游戏进展（接收按键转换蛇方向/蛇向前移动

DWORD WINAPI ChangeDirect(LPVOID lpParamter)
{
	while(gameover==0){
		int c=getch();
		switch(c){
		case 'w':
			if(turned==0 && pause==0){
				if(straight!=1){
					straight=3;
					turned=1;
				}
			}break;
		case 'a':
			if(turned==0 && pause==0){
				if(straight!=0){
					straight=2;
					turned=1;
				}
			}break;
		case 's':
			if(turned==0 && pause==0){
				if(straight!=3){
					straight=1;
					turned=1;
				}
			}break;
		case 'd':
			if(turned==0 && pause==0){
				if(straight!=2){
					straight=0;
					turned=1;
				}
			}break;
		case 'p':
			pause=1-pause;
			break;
		default:
			break;
		}
	}
    return 0L;
}
void HideCursor()//隐藏光标
{
 CONSOLE_CURSOR_INFO cursor_info = {1, 0}; 
 SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
}
int main()
{
	int diff=difficulty();
	HideCursor();
	s.createSnake();
	regionInitial();
    	HANDLE hThread = CreateThread(NULL, 0, ChangeDirect, NULL, 0, NULL);
    	CloseHandle(hThread); 
    	while(gameover==0){
		if(pause==0){
			if(s.moveForward()!=0)break;
			Sleep(diff);
		}
		if(turned==1)turned=0;
	}
	printf("GAME OVER，按ESC退出\n");
	s.destroySnake();
	int exit;
	do{
		exit=getch();
	}while(exit!=27);
    	return 0;
}
