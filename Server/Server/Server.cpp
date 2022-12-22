#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <graphics.h>
#include <conio.h>
#include <easyx.h>
using namespace std;

#pragma comment (lib,"ws2_32.lib")


// 引用 Windows Multimedia API
#pragma comment(lib,"Winmm.lib")

#include <graphics.h>
#include <math.h>
#include <string>


/////////////////////////////////////////////////////////全局变量的定义
char* ip; //定义IP地址变量 

int list[15][15] = { 0 };//15*15的棋盘
int result = 1;

int chat = 0;//控制聊天框

char s1[50];//发送的字符串
char r1[50];//接收的字符串

char s2[200];//聊天要发送的
char r2[200];//聊天要接收的

void StartMenu();

int lx = -1, ly = -1;//记录打红点子，到时候擦除

MOUSEMSG m;		// 定义鼠标消息

SOCKET sockSer;
SOCKET sockConn;

SOCKET sockSer2;
SOCKET sockConn2;

int D = COLORREF RGB(148, 162, 175);
//int D = COLORREF RGB(250, 224, 150);//调制棋盘的颜色为代号D

///////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////
// 提取指定模块中的资源文件
// 参数：
//		strDstFile:		目标文件名。提取的资源将保存在这里；
//		strResType:		资源类型；
//		strResName:		资源名称；
// 返回值：
//		true: 执行成功；
//		false: 执行失败。
bool ExtractResource(LPCTSTR strDstFile, LPCTSTR strResType, LPCTSTR strResName)
{
	// 创建文件
	HANDLE hFile = ::CreateFile(strDstFile, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;

	// 查找资源文件中、加载资源到内存、得到资源大小
	HRSRC	hRes = ::FindResource(NULL, strResName, strResType);
	HGLOBAL	hMem = ::LoadResource(NULL, hRes);
	DWORD	dwSize = ::SizeofResource(NULL, hRes);

	// 写入文件
	DWORD dwWrite = 0;  	// 写入大小
	::WriteFile(hFile, hMem, dwSize, &dwWrite, NULL);
	::CloseHandle(hFile);

	return true;
}

void judge2(int x, int y, int c)//判断是否赢了,五子连算法
{
	int i, j, k;
	int flag = 0;
	int xp, yp;//胜利时要划红线的坐标

	for (i = -1; i <= 1; i++)//这两个for循环是确定方向
		for (j = -1; j <= 1; j++)
		{
			if (!(i == 0 && j == 0))
			{
				for (k = 1; k < 5; k++)//在正方向上，取几个计入个数
				{
					flag = 0;//不同方向了，计数器归0
					for (int h1 = k; h1 > 0; h1--)//正向所取。不能直接用k，会把k运算的，导致出错
					{
						if (list[x + h1 * i][y + h1 * j] == c)
							flag++;
						else
							break;//只要有一个断了，就不用计数了
					}

					for (int h2 = 1; h2 <= (4 - k); h2++)//反向要取几个
					{
						if (list[x - h2 * i][y - h2 * j] == c)
							flag++;
						else
							break;//只要有一个断了，就不用计数了
					}
					if (flag == 4)
					{
						printf("\n%d\n", flag);
						setlinecolor(RED);
						setlinestyle(NULL, 5, NULL, NULL);
						xp = 50 + 25 * x, yp = 100 + 25 * y;
						line(xp + 25 * k * i, yp + 25 * k * j, xp + 25 * (k - 4) * i, yp + 25 * (k - 4) * j);
						result = 0;//这局游戏已结束
						goto judge_over;
					}
				}
			}
		}
judge_over:
	flag = 0;//没有实际意义，只是为了编译通过
}

int judge1(int x, int y)//下棋确定点的函数
{
	int flag = 0;//为后面是否成功下棋做标记
	int i;
	int a, b;//记录在二维数组内的地址
	for (i = 0; i < 15; i++)//有15个格子一个个检索
	{
		if (abs(50 + 25 * i - x) <= 10)
		{
			x = 50 + 25 * i;
			a = i;//记录下下子在二维数组的行位置
			flag++;
			break;
		}
	}
	for (i = 0; i < 15; i++)//有15个格子一个个检索
	{
		if (abs(100 + 25 * i - y) <= 10)
		{
			y = 100 + 25 * i;
			b = i;//记录下下子在二维数组的列位置
			flag++;
			break;
		}
	}
	if (flag == 2 && list[a][b] == 0)//落子正确，且那个点还没有下过子
	{
		setfillcolor(BLACK);//规定颜色
		fillcircle(x, y, 10);//下子


		setfillcolor(WHITE);//规定颜色
		solidcircle(lx, ly, 2);//擦除上一个红点

		lx = x, ly = y;//记录下打红点的子
		setfillcolor(RED);//规定颜色
		solidcircle(lx, ly, 2);//记录下刚刚自己下的点（打红点）
		setfillcolor(D);//上上步改了填充色，这里把填充色还原


		list[a][b] = 1;
		judge2(a, b, 1);
		sprintf(s1, "%d,%d", a, b);
		return 1;
	}
	else
		return 0;
}

void get_you()
{
	int flag = 0;
	while (result)
	{
		switch (m.uMsg)
		{
		case WM_MOUSEMOVE:
			// 鼠标移动的时候画红色的小点
			//putpixel(m.x, m.y, RED);
			break;

		case WM_LBUTTONDOWN:
			flag = judge1(m.x, m.y);//判断是否下棋
			break;
		}
		if (flag == 1)
			break;
	}
}

void initial()
{
	result = 1;
	chat = 0;

	initgraph(800, 600);

	setbkcolor(D);
	cleardevice();
	setlinecolor(BLACK);
	int add = 100;
	for (; add <= 450; add += 25)
	{
		line(50, add, 400, add);
	}
	for (add = 50; add <= 400; add += 25)
	{
		line(add, 100, add, 450);
	}

	settextcolor(BLUE);
	settextstyle(50, 0, _T("隶书"));
	outtextxy(450, 50, _T("五子棋大战"));

	settextstyle(20, 0, _T("隶书"));
	outtextxy(480, 130, _T("1.服务器端:黑子先手"));
	outtextxy(480, 160, _T("2.祝您游戏愉快，旗开得胜！"));
	outtextxy(480, 190, _T("3.以下框为对方发言框："));

	setfillcolor(D);
	fillrectangle(450, 230, 770, 450);//消息框

	outtextxy(450, 470, _T("点击右边框开始发送对话:"));



	fillrectangle(680, 460, 770, 500);//点击发话的按钮

	outtextxy(700, 470, _T("Click!"));
}

void translate()//把接收到的x,y形式消息转化可实现
{
	int i, l, a, b;
	l = strlen(r1);
	for (i = 0; i < l; i++)
	{
		if (r1[i] == ',')
			break;
	}
	if (i == 2)
		a = 10 * (r1[0] - '0') + (r1[1] - '0');
	else
		a = r1[0] - '0';

	if ((l - i) == 3)
		b = 10 * (r1[l - 2] - '0') + (r1[l - 1] - '0');
	else
		b = (r1[l - 1] - '0');

	setfillcolor(WHITE);
	fillcircle(50 + 25 * a, 100 + 25 * b, 10);

	setfillcolor(BLACK);//规定颜色
	solidcircle(lx, ly, 2);//擦除红点

	lx = 50 + 25 * a, ly = 100 + 25 * b;//记录下打红点的子
	setfillcolor(RED);//规定颜色
	solidcircle(lx, ly, 2);//记录下刚刚自己下的点（打红点）
	setfillcolor(D);//上上步改了填充色，这里把填充色还原

	list[a][b] = 2;
	judge2(a, b, 2);
}

void show(int k)
{
	if (chat == 10)
	{
		fillrectangle(450, 230, 770, 450);//重画消息框以覆盖
		chat = 0;
	}
	if (k == 1)
	{
		outtextxy(460, 240 + 20 * chat, _T("我:"));
		outtextxy(485, 240 + 20 * chat, s2);
	}
	else
	{
		outtextxy(460, 240 + 20 * chat, _T("Ta说:"));
		outtextxy(505, 240 + 20 * chat, r2);
	}
	chat++;
}

DWORD WINAPI R3(LPVOID lpParamter) //接收的线程
{
	while (1)
	{
		while (MouseHit())
		{
			m = GetMouseMsg();
		}
	}
}

DWORD WINAPI R2(LPVOID lpParamter) //接收的线程
{
	strcpy(r2, "标记字符串标记字符串");//为了防止同一个消息，不断的播出，这是检测消息是否更新了
	char flag[200] = "标记字符串标记字符串";
	while (1)
	{
		int p = recv(sockConn2, r2, 200, 0);//如果对方中断了程序就会返回SOCKET_ERROR

		if (p == SOCKET_ERROR)//连接中断
		{
			HWND wnd = GetHWnd();
			MessageBox(wnd, "对方已中断程序或掉线,请重启程序", "连接中断", MB_OK | MB_ICONWARNING);
			exit(0);
		}

		if (strcmp(r2, flag) != 0)
		{
			show(2);
			flag[200] = r2[200];
			fflush(stdin);
		}
	}
}

DWORD WINAPI R1(LPVOID lpParamter) //聊天的线程
{

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(1, 1);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		/* Tell the user that we couldn't find a useable */
		/* winsock.dll. */
		return 0;
	}
	/* Confirm that the Windows Sockets DLL supports 1.1.*/
	/* Note that if the DLL supports versions greater */
	/* than 1.1 in addition to 1.1, it will still return */
	/* 1.1 in wVersion since that is the version we */
	/* requested. */
	if (LOBYTE(wsaData.wVersion) != 1 ||
		HIBYTE(wsaData.wVersion) != 1) {
		/* Tell the user that we couldn't find a useable */
		/* winsock.dll. */
		WSACleanup();
		return 0;
	}
	/* The Windows Sockets DLL is acceptable. Proceed. */
	/////////////////////////////


	sockSer2 = socket(AF_INET, SOCK_STREAM, 0);
	SOCKADDR_IN addrSer2, addrCli2;
	addrSer2.sin_family = AF_INET;
	addrSer2.sin_port = htons(5080);
	addrSer2.sin_addr.S_un.S_addr = inet_addr(ip);

	bind(sockSer2, (SOCKADDR*)&addrSer2, sizeof(SOCKADDR));

	listen(sockSer2, 5);


	int len = sizeof(SOCKADDR);

	cout << "等待连接..." << endl;
	sockConn2 = accept(sockSer2, (SOCKADDR*)&addrCli2, &len);
	if (sockConn2 == INVALID_SOCKET)
	{
		cout << "R1连接失败！" << endl;

		return 0;
		//closegraph();
	}
	else
	{
		CreateThread(NULL, 0, R2, NULL, 0, NULL);
	}

	while (1)
	{
		if (m.x >= 680 && m.x <= 770 && m.y >= 460 && m.y <= 500)//680,460,770,500
		{
			if (m.uMsg == WM_LBUTTONDOWN)
			{
				InputBox(s2, 250, "请输入要说的话●-●：", "输入框", NULL, 400, 150);
				if (strlen(s2) != 0)
				{
					show(1);
					send(sockConn2, s2, strlen(s2) + 1, 0);
				}
			}
		}
		Sleep(50);
	}
}

void Get_ip() /*定义CheckIP（）函数，用于获取本机IP地址*/
{
	WSADATA wsaData;
	char name[255]; //定义用于存放获得的主机名的变量 
	PHOSTENT hostinfo; //调用MAKEWORD（）获得Winsock版本的正确值，用于加载Winsock库 
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) == 0)  /*在是加载Winsock库，如果WSAStartup（）函数返回值为0，说明加载成功，程序可以继续*/
	{
		if (gethostname(name, sizeof(name)) == 0)  /*如果成功地将本地主机名存放入由name参数指定的缓冲区中*/
		{
			if ((hostinfo = gethostbyname(name)) != NULL) //这是获取主机名，如果获得主机名成功的话，将返回一个指针，指向hostinfo，hostinfo  
				//为PHOSTENT型的变量，下面即将用到这个结构体
			{
				ip = inet_ntoa(*(struct in_addr*)*hostinfo->h_addr_list);   //调用inet_ntoa（）函数，将hostinfo结构变量中的h_addr_list转化为标准的点分表示的IP
					//地址（如192.168.0.1） 


				/*cout << "\n\n\t\t\t   五子棋大战服务器创建\n" << endl;
				cout << "\n\n\t\t\t1.请告诉客户端你的IP地址:" << ip << endl;

				cout << "\n\t\t\t2.同时确保您的防火墙已关闭\n\n" << endl;*/
			}
		}
		WSACleanup(); //卸载Winsock库，并释放所有资源 
	}
}

void getIPs()
{
	WORD v = MAKEWORD(1, 1);
	WSADATA wsaData;
	WSAStartup(v, &wsaData);
	// 加载套接字库     
	int i = 0;
	struct hostent* phostinfo = gethostbyname("");
	for (i = 0; NULL != phostinfo && NULL != phostinfo->h_addr_list[i]; ++i)
	{
		char* pszAddr = inet_ntoa(*(struct in_addr*)phostinfo->h_addr_list[i]);
		//printf("%s\n", pszAddr);  //显示ip地址

	}
	/*cout << "\n\n\t\t\t   五子棋大战服务器创建\n" << endl;
	cout << "\n\n\t\t\t1.请告诉客户端你的IP地址:" << ip << endl;
	cout << "\n\t\t\t2.同时确保您的防火墙已关闭\n\n" << endl;*/
	initgraph(1000, 750);
	cleardevice();
	IMAGE bgp;
	loadimage(&bgp, "bgp.jpg");
	BeginBatchDraw();
	putimage(0, 0, &bgp, SRCCOPY);
	EndBatchDraw();
	settextstyle(40, 20, _T("楷体"));
	settextcolor(RED);
	setbkmode(TRANSPARENT);
	setlinecolor(BLUE);
	setfillstyle(BS_NULL);
	fillroundrect(295, 90, 705, 150, 20, 20);
	outtextxy(300, 100, "五子棋大战服务器创建");
	settextcolor(DARKGRAY);
	outtextxy(260, 310, "1.请告诉客户端你的IP地址:");
	settextcolor(BLUE);
	outtextxy(320, 350, ip);
	settextcolor(DARKGRAY);
	outtextxy(260, 400, "2.同时确保您的防火墙已关闭");

	/*FlushBatchDraw();
	int isConnect=1;
	while (isConnect) {
		int num = getchar();
		if ((num != 'q') && (num != 'Q'))
		{
			settextstyle(40, 20, _T("楷体"));
			settextcolor(RED);
			setbkmode(TRANSPARENT);
			outtextxy(250, 200, "请等待！或者按Q并回车返回");
		}
		else if (num=='q'||num=='Q')
		{
			closegraph();
			StartMenu();
			WSACleanup();
			isConnect = 0;
		}
		if (sockConn == INVALID_SOCKET)isConnect=0;
	}*/
	FlushBatchDraw();
	WSACleanup();
}



void main1()
{

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(1, 1);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		/* Tell the user that we couldn't find a useable */
		/* winsock.dll. */
		return;
	}
	/* Confirm that the Windows Sockets DLL supports 1.1.*/
	/* Note that if the DLL supports versions greater */
	/* than 1.1 in addition to 1.1, it will still return */
	/* 1.1 in wVersion since that is the version we */
	/* requested. */
	if (LOBYTE(wsaData.wVersion) != 1 ||
		HIBYTE(wsaData.wVersion) != 1) {
		/* Tell the user that we couldn't find a useable */
		/* winsock.dll. */
		WSACleanup();
		return;
	}
	/* The Windows Sockets DLL is acceptable. Proceed. */
	/////////////////////////////

	Get_ip();
	getIPs();

	//播放背景音乐
	//TCHAR bgm[_MAX_PATH];
	//char sk1[300];
	//// 产生临时文件的文件名

	//::GetTempPath(_MAX_PATH, bgm);
	//strcat(bgm, "playmp3_bg.MP3");

	//// 将 MP3 资源提取为临时文件
	//ExtractResource(bgm, _T("MP3"), _T("bgm.MP3"));

	//// 打开音乐
	//sprintf(sk1, "open \"%s\" alias bg1", bgm);
	//mciSendString(sk1, NULL, 0, NULL);

	//// 播放音乐
	//mciSendString(_T("play bg1 repeat"), NULL, 0, NULL);
	mciSendString("open bgm.mp3 alias bgm", NULL, 0, NULL);	//打开读取音乐文件
	mciSendString("play bgm repeat", NULL, 0, NULL);	//播放音乐文件

	////////////////////下面开始创建连接

	sockSer = socket(AF_INET, SOCK_STREAM, 0);
	SOCKADDR_IN addrSer, addrCli;
	addrSer.sin_family = AF_INET;
	addrSer.sin_port = htons(5050);
	addrSer.sin_addr.S_un.S_addr = inet_addr(ip);

	bind(sockSer, (SOCKADDR*)&addrSer, sizeof(SOCKADDR));

	listen(sockSer, 5);

	int len = sizeof(SOCKADDR);

	//cout << "\t\t\t服务器已创建，等待连接中..." << endl;
	settextstyle(40, 20, _T("楷体"));
	settextcolor(RED);
	setbkmode(TRANSPARENT);
	outtextxy(250, 200, "服务器已创建，等待连接中...");
	sockConn = accept(sockSer, (SOCKADDR*)&addrCli, &len);

	CreateThread(NULL, 0, R1, NULL, 0, NULL);
	CreateThread(NULL, 0, R3, NULL, 0, NULL);

	if (sockConn == INVALID_SOCKET)
	{
		settextstyle(40, 20, _T("楷体"));
		settextcolor(RED);
		setbkmode(TRANSPARENT);
		outtextxy(250, 250, "连接失败！");
		//cout << "连接失败！" << endl;
		return;
	}
	else
	{
		settextstyle(40, 20, _T("楷体"));
		settextcolor(RED);
		setbkmode(TRANSPARENT);
		outtextxy(250, 250, "连接成功！");
		//cout << "连接成功！" << endl;
	}

start:
	initial();



	while (result)
	{
		get_you();
		send(sockConn, s1, strlen(s1) + 1, 0);
		recv(sockConn, r1, 256, 0);
		translate();
	}
	char c[20] = "1";
	InputBox(c, 250, "战局已定！再来一把？确定则输入1确认,否则输入其它数：", "结束", NULL, 400, 150);
	if ((c[0] - '0') == 1)
	{
		for (int i = 0; i < 15; i++)
			for (int j = 0; j < 15; j++)
			{
				list[i][j] = 0;
			}
		goto start;
	}
	mciSendString("stop bgm", NULL, 0, NULL);
	mciSendString("close bgm", NULL, 0, NULL);
	//DeleteFile(bgm);

	closegraph();
	closesocket(sockSer);
	WSACleanup();
}

void StartMenu()
{
	char* input;
	char s[] = "游戏说明";
	initgraph(1000, 750);
	//setbkcolor(WHITE);
	cleardevice();
	IMAGE bgp;
	loadimage(&bgp, "bgp2.jpg");
	BeginBatchDraw();
	putimage(0, 0, &bgp, SRCCOPY);
	EndBatchDraw();
	settextstyle(60, 30, _T("隶书"));
	settextcolor(DARKGRAY);
	setbkmode(TRANSPARENT);
	outtextxy(200, 100, "欢迎来到妙妙五子棋!");
	settextstyle(40, 20, _T("楷体"));
	setlinecolor(LIGHTGRAY);
	setfillstyle(BS_NULL);
	fillroundrect(330, 245, 610, 330, 20, 20);
	outtextxy(370, 270, "1.开始游戏");
	fillroundrect(330, 345, 610, 430, 20, 20);
	outtextxy(370, 370, "2.游戏说明");
	fillroundrect(330, 445, 610, 530, 20, 20);
	outtextxy(370, 470, "3.退出游戏");
	FlushBatchDraw();

	char num = _getch();
	//scanf("%d", &num);
	settextstyle(40, 20, _T("微软雅黑"));
	settextcolor(RED);
	setbkmode(TRANSPARENT);
	//outtextxy(380, 380, num);

	if (num == 'a' || num == 'A' || num == '1') {
		closegraph();
		main1();
	}
	else if (num == 's' || num == 'S' || num == '2')
	{
		IMAGE bgp;
		loadimage(&bgp, "yxsm.jpg");
		BeginBatchDraw();
		putimage(0, 0, &bgp, SRCCOPY);
		EndBatchDraw();
		settextstyle(20, 15, _T("微软雅黑"));
		settextcolor(RED);
		setbkcolor(DARKGRAY);
		outtextxy(10, 10, "按任意键返回");
		FlushBatchDraw();
		if (_getch())
		{
			closegraph();
			StartMenu();
			num = '4';
		}
	}
	else if (num == 'd' || num == 'D' || num == '3')
	{
		exit(0);
	}
	else
	{
		if (num != '4') {
			settextstyle(25, 12, _T("楷体"));
			settextcolor(RED);
			setbkmode(TRANSPARENT);
			setbkcolor(DARKGRAY);
			outtextxy(350, 190, "选择有误，请重新选择！");
			//FlushBatchDraw();
			Sleep(500);
			StartMenu();
		}
	}
	closegraph();
}

int main() {
	StartMenu();
	main1();
	return 0;
}