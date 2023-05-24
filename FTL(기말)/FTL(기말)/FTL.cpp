#include <windows.h> //--- ������ ��� ����
#include <tchar.h>
#include<stdio.h>
#include <cmath>
#include <sstream> // stringstream�� ����ϱ� ���� �߰�
#include <iomanip> // �Ҽ��� �ڸ����� �����ϱ� ���� �߰�
#include <atlimage.h>
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Game";
LPCTSTR lpszWindowName = L"FTL";
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

#define WINDOWWID 1600
#define WINDOWHEI 1000

typedef struct Ships
{
	int x, y;		//x,y��ǥ��
	int sx, sy;		//�Լ� x,y ������ ��

};
typedef struct Missile
{
	int x, y;         //�̻��� xy��ǥ ��
	int sx, sy;      //�̻��� x,y������ ��
	int dead;       //dead�� 1�̸� �̻��� ������
	int place;
	RECT rt;        //�߰�
}missile;
typedef struct room
{
	RECT rt;
	int hp;
	char name;
};
void attakToEnemy_down(int* m2count, int* m1count, missile* player_msl2, missile* player_msl1, int* eselectplace, RECT* eplace);
void attakToEnemy_up(int* m2count, int* m1count, missile* player_msl1, RECT* eplace, int* ebooms, int* eboomx, int* eboomy);
void attakToPlayer_up(int* em1count, int* em2count, missile* enemy_msl1, missile* enemy_msl2);
void attakToPlayer_down(int* em2count, missile* enemy_msl2);
void attaking(int* Mattak, int* expls, int* m2count);
void goMissileAni(int* expls, int* explnum);
void attackMissileAni(int* ebooms, int* eboomx, int* eboomy, int* eboomsx, int* eboomsy, int* eboomnum);
void makePlayerRoom(room* playerRoom, int* playerRoomC, int left, int top, int right, int bottom, int hp, char name);
void makeEnemyRoom(room* enemyRoom, int* enemyRoomC, int left, int top, int right, int bottom, int hp, char name);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASSEX WndClass;
	g_hInst = hInstance;
	WndClass.cbSize = sizeof(WndClass);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = lpszClass;
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&WndClass);
	hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, WINDOWWID, WINDOWHEI, NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	while (GetMessage(&Message, 0, 0, 0))
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}
#define MISSLECOUNT 1000

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc, mdc;
	HBITMAP hBitmap;
	HBRUSH hbr, oldbr;
	HPEN hPen, oldPen;
	HFONT hFont, hOldFont;

	static RECT rt;
	static RECT notrt;// //�߰�
	static Ships ship = { 0 };
	static CImage player1, player2, player3;
	static CImage enemy1, enemy2;
	static CImage laser, backGround;
	//�Լ��� ��
	static room playerRoom[100], enemyRoom[100];
	static int playerRoomC = 0, enemyRoomC = 0;
	//UI
	static TCHAR CplayerHP[100], CEnemyHP[100];//ü�� UI 
	static int playerMaxHP, playerHP;//ü��
	static int enemyMaxHP, enemyHP;//�ִ�ü��
	//����
	static int fontPlayerSize, fontEnemySize;//��Ʈ �ٲٱ�
	static COLORREF textPlayerColor, textEnemyColor;//�÷� �ٲٱ�

	static CImage missileimg1, missileimg2;// //�߰�
	static CImage boom[6];//
	static int boomnum = 0;// //�߰�
	static int booms = 0;// //�߰�

	static int eboomnum = 0;// //�߰�
	static int ebooms = 0;// //�߰�
	static int eboomx, eboomy, eboomsx=50, eboomsy=50;
	static CImage explosionM[9];// //�߰�
	static int explnum = 0;// //�߰�
	static int expls = 0;// //�߰�
	static int eexplnum = 0;// //�߰�
	static int eexpls = 0;// //�߰�
	static int w, h;
	static int mx, my;
	//�Ʊ��̻���
	static missile player_msl2[MISSLECOUNT];//�Ʒ��� �������� �̻��� //�߰�
	static missile player_msl1[MISSLECOUNT];//���� ���� �̻��� //�߰�
	static int m2count;// //�߰�
	static int m1count;// //�߰�
	static int Mattak=1;//1�̸� �̻��� �߻����
	//�����̻���
	static missile enemy_msl2[MISSLECOUNT];//�Ʒ��� �������� �̻��� //�߰�
	static missile enemy_msl1[MISSLECOUNT];//���� ���� �̻��� //�߰�
	static int em2count;// //�߰�
	static int em1count;// //�߰�
	static int eMattak = 1;//1�̸� �̻��� �߻���� //�߰�
	//�̻��� �߻� ��ư
	static RECT mslbutton = {0,0,50,30}; //�߰�
	//place
	static RECT eplace[5];//cnrk
	static int eselectplace=6;//cnrk
	//static RECT place = {1400};
	//static missile;
	//--- �޽��� ó���ϱ�
	switch (uMsg)
	{
	case WM_CREATE:
		eplace[4].left = 1345;
		eplace[4].right = 1380;
		eplace[4].top = 500;
		eplace[4].bottom = 580;
		eplace[3].left = 1260;
		eplace[3].right = 1340;
		eplace[3].top = 545;
		eplace[3].bottom = 580;
		eplace[2].left = 1260;
		eplace[2].right = 1340;
		eplace[2].top = 500;
		eplace[2].bottom = 540;
		eplace[1].left = 1260;
		eplace[1].right = 1340;
		eplace[1].top = 410;
		eplace[1].bottom = 490;
		eplace[0].left = 1260;
		eplace[0].right = 1340;
		eplace[0].top = 365;
		eplace[0].bottom = 405;
	{
		
		//�̹��� �ε�
		boom[0].Load(TEXT("Pictures/boom/����1.PNG"));
		boom[1].Load(TEXT("Pictures/boom/����2.PNG"));
		boom[2].Load(TEXT("Pictures/boom/����3.PNG"));
		boom[3].Load(TEXT("Pictures/boom/����4.PNG"));
		boom[4].Load(TEXT("Pictures/boom/����5.PNG"));
		boom[5].Load(TEXT("Pictures/boom/����6.PNG"));
		explosionM[0].Load(TEXT("Pictures/expl/expl1.PNG")); //�߰�
		explosionM[1].Load(TEXT("Pictures/expl/expl2.PNG"));
		explosionM[2].Load(TEXT("Pictures/expl/expl3.PNG"));
		explosionM[3].Load(TEXT("Pictures/expl/expl4.PNG"));
		explosionM[4].Load(TEXT("Pictures/expl/expl5.PNG"));
		explosionM[5].Load(TEXT("Pictures/expl/expl6.PNG"));
		explosionM[6].Load(TEXT("Pictures/expl/expl7.PNG"));
		explosionM[7].Load(TEXT("Pictures/expl/expl8.PNG"));
		explosionM[8].Load(TEXT("Pictures/expl/expl9.PNG"));
		missileimg1.Load(TEXT("Pictures/�̻���1.PNG"));//����
		missileimg2.Load(TEXT("Pictures/�̻���2.png"));//�Ʒ��� //�߰�
		player1.Load(TEXT("Pictures/player1.png"));
		player2.Load(TEXT("Pictures/player2.png"));
		player3.Load(TEXT("Pictures/player3.png"));
		enemy1.Load(TEXT("Pictures/enemy1.png"));
		enemy2.Load(TEXT("Pictures/enemy2.png"));
		laser.Load(TEXT("Pictures/Laser.png"));
		backGround.Load(TEXT("Pictures/BackGround.png")); 
	}
	{
		//Ÿ�̸�
		SetTimer(hwnd, 1, 45, NULL); //�߰�
		SetTimer(hwnd, 2, 2000, NULL); //�߰�
		SetTimer(hwnd, 3, 70, NULL); //�߰�
		//�÷��̾� ��ġ
		ship.x = 100, ship.y = 200, ship.sx = 700, ship.sy = 400;
		//��� RECT ũ�� �޾ƿ���
		GetClientRect(hwnd, &rt);
		//��...
		{
			//�÷��̾� ��
			{
				//TODO : ���̸��� �Է¹ް� ����
				makePlayerRoom(&playerRoom[playerRoomC], &playerRoomC, 300, 450, 350, 550, 'e', 0);
				makePlayerRoom(&playerRoom[playerRoomC], &playerRoomC, 345, 450, 425, 550, 'E', 3);//����
				makePlayerRoom(&playerRoom[playerRoomC], &playerRoomC, 345, 400, 425, 450, 'O', 1);//���
				makePlayerRoom(&playerRoom[playerRoomC], &playerRoomC, 345, 550, 425, 600, 'e', 0);

				makePlayerRoom(&playerRoom[playerRoomC], &playerRoomC, 430, 400, 510, 450, 'e', 0);
				makePlayerRoom(&playerRoom[playerRoomC], &playerRoomC, 430, 550, 510, 600, 'e', 0);
				makePlayerRoom(&playerRoom[playerRoomC], &playerRoomC, 480, 450, 560, 550, 'W', 1);//����

				makePlayerRoom(&playerRoom[playerRoomC], &playerRoomC, 550, 350, 640, 400, 'e', 0);
				makePlayerRoom(&playerRoom[playerRoomC], &playerRoomC, 550, 400, 640, 500, 'e', 0);
				makePlayerRoom(&playerRoom[playerRoomC], &playerRoomC, 550, 500, 640, 600, 'e', 0);
				makePlayerRoom(&playerRoom[playerRoomC], &playerRoomC, 550, 600, 640, 650, 'e', 0);

				makePlayerRoom(&playerRoom[playerRoomC], &playerRoomC, 640, 400, 730, 500, 'H', 1);//ȸ����
				makePlayerRoom(&playerRoom[playerRoomC], &playerRoomC, 640, 500, 740, 600, 'S', 2);//�����

				makePlayerRoom(&playerRoom[playerRoomC], &playerRoomC, 730, 450, 810, 500, 'D', 1);//�� �����, �ڷ���Ʈ
				makePlayerRoom(&playerRoom[playerRoomC], &playerRoomC, 730, 500, 810, 550, 'C', 1);//ī�޶� ��, �¹��� ��ó�� ī�޶� ���̴ٰ�, ���� ������ �� ��ü�� ���� or ��ü ����

				makePlayerRoom(&playerRoom[playerRoomC], &playerRoomC, 810, 450, 900, 550, 'e', 0);
				makePlayerRoom(&playerRoom[playerRoomC], &playerRoomC, 900, 450, 945, 550, 'K', 1);//��Ÿ ��
			}
			//���� ��
			{
				makeEnemyRoom(&enemyRoom[enemyRoomC], &enemyRoomC, 1255, 360, 1335, 405, 1, 'K');//��Ÿ��
				makeEnemyRoom(&enemyRoom[enemyRoomC], &enemyRoomC, 1255, 405, 1335, 495, 2, 'S');//�����
				makeEnemyRoom(&enemyRoom[enemyRoomC], &enemyRoomC, 1255, 495, 1335, 540, 3, 'W');//�����
				makeEnemyRoom(&enemyRoom[enemyRoomC], &enemyRoomC, 1255, 540, 1335, 620, 3, 'E');//������

				makeEnemyRoom(&enemyRoom[enemyRoomC], &enemyRoomC, 1215, 495, 1255, 585, 0, 'e');
				makeEnemyRoom(&enemyRoom[enemyRoomC], &enemyRoomC, 1340, 495, 1380, 585, 1, 'O');//��ҽ�
			}
		}
		//�ʱ� ����
		{
			playerMaxHP = 30;
			playerHP = 30;
		}
		//UI
		{
			textPlayerColor = RGB(0, 255, 0);//�÷� ����
			fontPlayerSize = 50; // ���� ũ��
			textEnemyColor = RGB(0, 0, 255);//�÷� ����
			fontEnemySize = 30; // ���� ũ��
		}
	
		//�̻��� �ʱ�
		for (int i = 0; i < MISSLECOUNT; i++)
		{
			player_msl2[i].x=900;
			player_msl2[i].y = 400;
			player_msl2[i].dead = 0;
			player_msl2[i].sx = 25;
			player_msl2[i].sy = 40;
		}
		m2count = 0;
		for (int i = 0; i < MISSLECOUNT; i++)
		{
			player_msl1[i].x = 1290;
			player_msl1[i].y = 700;
			player_msl1[i].dead = 0;
			player_msl1[i].sx = 25;
			player_msl1[i].sy = 40;
		}
		m1count = 0;
		for (int i = 0; i < MISSLECOUNT; i++)
		{
			enemy_msl2[i].x = 500;
			enemy_msl2[i].y = -40;
			enemy_msl2[i].dead = 0;
			enemy_msl2[i].sx = 25;
			enemy_msl2[i].sy = 40;
		}
		em2count = 0;
		for (int i = 0; i < MISSLECOUNT; i++)
		{
			enemy_msl1[i].x = 1290;
			enemy_msl1[i].y = 250;
			enemy_msl1[i].dead = 0;
			enemy_msl1[i].sx = 25;
			enemy_msl1[i].sy = 40;
		}
		em1count = 0;
		
	}
	break;
	case WM_PAINT:
		printf("x : %d y : %d sx : %d sy : %d", ship.x, ship.y, ship.sx, ship.sy);
		hdc = BeginPaint(hwnd, &ps);
		mdc = CreateCompatibleDC(hdc);
		hBitmap = CreateCompatibleBitmap(hdc, rt.right, rt.bottom);
		SelectObject(mdc, hBitmap);
		SetBkMode(mdc, TRANSPARENT);
		//���
		{
			w = backGround.GetWidth();
			h = backGround.GetHeight();

			backGround.Draw(mdc, 0, 0, rt.right, rt.bottom, 0, 0, w, h); //--- �޸� DC�� ��� �׸���
		}
		//�÷��̾�
		{
			player1.TransparentBlt(mdc, ship.x, ship.y, 1.5 * ship.sx, 1.5 * ship.sy, 0, 0, 719, 340, RGB(0, 0, 0));
			player2.TransparentBlt(mdc, 2.4 * ship.x, 1.5 * ship.y, ship.sx, ship.sy, 0, 0, 300, 140, RGB(224, 224, 224));
			player3.TransparentBlt(mdc, 2.4 * ship.x + 13, 1.5 * ship.y, ship.sx, ship.sy, 0, 0, 300, 140, RGB(224, 224, 224));
		}
		//��
		{
			enemy1.TransparentBlt(mdc, 1000, 250, 600, 500, 0, 0, 500, 500, RGB(0, 0, 0));
			enemy2.TransparentBlt(mdc, 1100, 300, 400, 400, 0, 0, 700, 700, RGB(31, 31, 31));
		}
		//������
		{
			//�÷��̾�
			//laser.TransparentBlt(mdc, 300, 450, 45, 100, 403, 970, 50, 19, RGB(0, 0, 0));
			//laser.TransparentBlt(mdc, 345, 450, 80, 100, 403, 970, 50, 19, RGB(0, 0, 0));
			//laser.TransparentBlt(mdc, 345, 400, 80, 50, 403, 970, 50, 19, RGB(0, 0, 0));
			//
			//laser.TransparentBlt(mdc, 430, 400, 80, 50, 403, 970, 50, 19, RGB(0, 0, 0));
			//laser.TransparentBlt(mdc, 350, 550, 80, 50, 403, 970, 50, 19, RGB(0, 0, 0));
			//laser.TransparentBlt(mdc, 430, 550, 80, 50, 403, 970, 50, 19, RGB(0, 0, 0));
			//laser.TransparentBlt(mdc, 480, 450, 80, 100, 403, 970, 50, 19, RGB(0, 0, 0));
			//
			//laser.TransparentBlt(mdc, 550, 350, 90, 50, 403, 970, 50, 19, RGB(0, 0, 0));
			//laser.TransparentBlt(mdc, 550, 400, 90, 100, 403, 970, 50, 19, RGB(0, 0, 0));
			//laser.TransparentBlt(mdc, 550, 500, 90, 100, 403, 970, 50, 19, RGB(0, 0, 0));
			//laser.TransparentBlt(mdc, 550, 600, 90, 50, 403, 970, 50, 19, RGB(0, 0, 0));
			//
			//laser.TransparentBlt(mdc, 640, 400, 90, 100, 403, 970, 50, 19, RGB(0, 0, 0));
			//laser.TransparentBlt(mdc, 640, 500, 90, 100, 403, 970, 50, 19, RGB(0, 0, 0));
			//
			//laser.TransparentBlt(mdc, 730, 450, 80, 50, 403, 970, 50, 19, RGB(0, 0, 0));
			//laser.TransparentBlt(mdc, 730, 500, 80, 50, 403, 970, 50, 19, RGB(0, 0, 0));
			//
			//laser.TransparentBlt(mdc, 810, 450, 90, 100, 403, 970, 50, 19, RGB(0, 0, 0));
			//laser.TransparentBlt(mdc, 900, 450, 45, 100, 403, 970, 50, 19, RGB(0, 0, 0));
			//
			////��
			//laser.TransparentBlt(mdc, 1255, 360, 80, 45, 403, 970, 50, 19, RGB(0, 0, 0));
			//laser.TransparentBlt(mdc, 1255, 405, 80, 90, 403, 970, 50, 19, RGB(0, 0, 0));
			//laser.TransparentBlt(mdc, 1255, 495, 80, 45, 403, 970, 50, 19, RGB(0, 0, 0));
			//laser.TransparentBlt(mdc, 1255, 540, 80, 45, 403, 970, 50, 19, RGB(0, 0, 0));
			//
			//laser.TransparentBlt(mdc, 1215, 495, 40, 90, 403, 970, 50, 19, RGB(0, 0, 0));
			//laser.TransparentBlt(mdc, 1340, 495, 40, 90, 403, 970, 50, 19, RGB(0, 0, 0));
		}
		//UI
		{
			_stprintf_s(CplayerHP, L"ü�� : %d / %d", playerHP, playerMaxHP);
			hFont = CreateFont(-fontPlayerSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");
			hOldFont = (HFONT)SelectObject(mdc, hFont);// �۲� ����

			SetTextColor(mdc, textPlayerColor);// ���� ���� ���� (���������� ����)
			TextOut(mdc, 50, 50, CplayerHP, lstrlen(CplayerHP));// �ؽ�Ʈ ���

			SelectObject(mdc, hOldFont);// ���� �۲� ����
			DeleteObject(hFont);// �۲� ����

			//_stprintf_s(CplayerHP, L"ü�� : %d / %d", playerHP, playerMaxHP);
			//hFont = CreateFont(-fontEnemySize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");
			//hOldFont = (HFONT)SelectObject(mdc, hFont);// �۲� ����
			//SetTextColor(mdc, textPlayerColor);// ���� ���� ���� (���������� ����)
			//TextOut(mdc, 50, 50, CplayerHP, lstrlen(CplayerHP));// �ؽ�Ʈ ���
			//SelectObject(mdc, hOldFont);// ���� �۲� ����
			//DeleteObject(hFont);// �۲� ����
		}
		//player�̻���
		{
			for (int i = 0; i < m2count; i++){
				if (player_msl2[i].dead == 0)
				{
					missileimg2.TransparentBlt(mdc, player_msl2[i].x, player_msl2[i].y, player_msl2[i].sx, player_msl2[i].sy ,
						1, 1, 25, 40, RGB(224, 224, 224));
				}
			}
			for (int i = 0; i < m1count; i++)
			{
				if (player_msl1[i].dead == 0)
				{
					missileimg1.TransparentBlt(mdc, player_msl1[i].x, player_msl1[i].y, player_msl1[i].sx, player_msl1[i].sy,
						0, 0, 25, 40, RGB(224, 224, 224));
				}
			}
		}
		//ememy�̻���
		{
			for (int i = 0; i < em1count; i++)
			{
				if (enemy_msl1[i].dead == 0)
				{
					missileimg1.TransparentBlt(mdc, enemy_msl1[i].x, enemy_msl1[i].y, enemy_msl1[i].sx, enemy_msl1[i].sy,
						0, 0, 25, 40, RGB(224, 224, 224));
				}
			}
			for (int i = 0; i < em2count; i++)
			{
				if (enemy_msl2[i].dead == 0)
				{
					missileimg2.TransparentBlt(mdc, enemy_msl2[i].x, enemy_msl2[i].y, enemy_msl2[i].sx, enemy_msl2[i].sy,
						1, 1, 25, 40, RGB(224, 224, 224));
				}
			}
		}
		//expl
		if (expls == 1)
		{
			explosionM[explnum].TransparentBlt(mdc, 900, 400, 40, 40, 0, 0, 90, 89, RGB(31, 31, 31));
		}
		if (eexpls == 1)
		{
			explosionM[eexplnum].TransparentBlt(mdc, 1290, 250, 40, 40, 0, 0, 90, 89, RGB(31, 31, 31));
		}
		//��ư
		hbr = CreateSolidBrush(RGB(255, 0, 0)); // GDI: �귯�� �����
		oldbr = (HBRUSH)SelectObject(hdc, hbr);
		FillRect(mdc, &mslbutton, hbr);
		SelectObject(hdc, oldbr); // ������ �귯�÷� ���ư���
		DeleteObject(hbr);
		//place
			hbr = CreateSolidBrush(RGB(255, 0, 0)); // GDI: �귯�� �����
			oldbr = (HBRUSH)SelectObject(hdc, hbr);
			FrameRect(mdc, &eplace[eselectplace], hbr);
			SelectObject(hdc, oldbr); // ������ �귯�÷� ���ư���
			DeleteObject(hbr);
		//boom
			if (ebooms == 1)
			{
				boom[eboomnum].TransparentBlt(mdc, eboomx, eboomy, eboomsx, eboomsy, 0, 0, 70, 70, RGB(31, 31, 31));
		    }
		//���� ���� ����
		BitBlt(hdc, 0, 0, rt.right, rt.bottom, mdc, 0, 0, SRCCOPY); //--- �޸� DC�� �׸��� ȭ�� DC�� ����
		DeleteObject(hBitmap);
		DeleteDC(mdc);
		EndPaint(hwnd, &ps);
		break;
	case WM_TIMER:
		switch (wParam)
		{
		case 1:
			//�Ʊ� �̻���
			attakToEnemy_down(&m2count, &m1count, player_msl2, player_msl1, &eselectplace, eplace);
			attakToEnemy_up(&m2count, &m1count, player_msl1, eplace, &ebooms, &eboomx, &eboomy);
			
			//ememy�̻���
			attakToPlayer_up(&em1count, &em2count, enemy_msl1, enemy_msl2);
			attakToPlayer_down(&em2count, enemy_msl2);
			break;
		case 2:
			//�Ʊ����ݻ���(Mattak==1�϶�)(�̻��� ������ �ִϸ��̼��� ���� expls=1 )
			attaking(&Mattak, &expls, &m2count);
			//�������ݻ���(eMattak==1�϶�)(�̻��� ������ �ִϸ��̼��� ���� expls=1 )
			attaking(&eMattak, &eexpls, &em1count);
			break;
		case 3:
			//�̻��� ������ �ִϸ��̼�(expls == 1�϶�)
			goMissileAni(&expls, &explnum);
			//�̻��� ������ �ִϸ��̼�(eexpls == 1�϶�)
			goMissileAni(&eexpls, &eexplnum);
			//�������� �ִϸ��̼� 
			attackMissileAni(&ebooms, &eboomx, &eboomy, &eboomsx, &eboomsy, &eboomnum);
			
			break;
		default:
			break;
		}
		InvalidateRect(hwnd, NULL, false);
		break;
	case WM_LBUTTONDOWN:
		mx = LOWORD(lParam);
		my = HIWORD(lParam);
		if (mslbutton.left <= mx && mx <= mslbutton.right && mslbutton.top <= my && my <= mslbutton.bottom)
		{
			if (Mattak == 1)
			{
				Mattak = 0;
			}
			else
			{
				Mattak = 1;
			}
			eMattak = 0;//�����ֱ�
		}
		for (int i = 0; i < 5; i++)
		{
			if (eplace[i].left <= mx && mx <= eplace[i].right && eplace[i].top <= my && my <= eplace[i].bottom)
			{
				eselectplace = i;
			}
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam); //--- ���� �� �޽��� ���� ������ �޽����� OS��
}
void makePlayerRoom(room* playerRoom, int* playerRoomC, int left, int top, int right, int bottom, int hp, char name)
{
	playerRoom->rt.left = left;
	playerRoom->rt.top = top;
	playerRoom->rt.right = right;
	playerRoom->rt.bottom = bottom;
	playerRoom->hp = hp;
	playerRoom->name = name;
	*playerRoomC += 1;
}
void makeEnemyRoom(room* enemyRoom, int* enemyRoomC, int left, int top, int right, int bottom, int hp, char name)
{
	enemyRoom->rt.left = left;
	enemyRoom->rt.top = top;
	enemyRoom->rt.right = right;
	enemyRoom->rt.bottom = bottom;
	enemyRoom->hp = hp;
	enemyRoom->name = name;
	*enemyRoomC += 1;
}
void attackMissileAni(int *ebooms,int* eboomx, int* eboomy, int* eboomsx, int* eboomsy,int* eboomnum)
{
	if (*ebooms == 1)
	{
		(*eboomnum)++;
		*eboomx -= 3;
		*eboomy -= 3;
		*eboomsx += 6;
		*eboomsy += 6;
		if (*eboomnum >= 6)
		{
			*eboomnum = 0;
			*ebooms = 0;
			*eboomsx = 50;
			*eboomsy = 50;
		}
	}
}
void goMissileAni(int*expls,int*explnum)
{
	if (*expls == 1)
	{
		(*explnum)++;
		if ((*explnum) >= 9)
		{
			(*explnum) = 0;
			*expls = 0;
		}
	}
}
void attaking(int * Mattak,int*expls,int*m2count)
{
	if (*Mattak == 1)
	{
		*expls = 1;
		(*m2count)++;
		if ((*m2count) >= MISSLECOUNT)
		{
			Mattak = 0;//����
		}
	}
}
void attakToPlayer_down(int*em2count,missile*enemy_msl2)
{
	for (int i = 0; i < *em2count; i++)
	{
		enemy_msl2[i].y += 4;
	}
}
void attakToPlayer_up(int *em1count,int *em2count,missile*enemy_msl1,missile* enemy_msl2)
{
	for (int i = 0; i < *em1count; i++)
	{
		if (enemy_msl1[i].dead == 0)
		{
			enemy_msl1[i].x -= 1;
			enemy_msl1[i].y -= 4;
			if (enemy_msl1[i].y <= -45)
			{
				enemy_msl1[i].dead = 1;
				enemy_msl1[i].y = -50;
				enemy_msl2[i].x;//��� ��ġ���� ����� ��ǥ �־��ָ�� place ��ġ
				(*em2count)++;
			}
		}
	}
}

void attakToEnemy_up(int* m2count, int* m1count, missile* player_msl1,RECT*eplace,int *ebooms,int *eboomx,int*eboomy)
{
	RECT notrt;
	for (int i = 0; i < *m1count; i++)
	{
		if (player_msl1[i].dead == 0)
		{
			player_msl1[i].y -= 4;//�浹üũ�� �������
			player_msl1[i].rt.left = player_msl1[i].x;
			player_msl1[i].rt.top = player_msl1[i].y;
			player_msl1[i].rt.right = player_msl1[i].x + player_msl1[i].sx;
			player_msl1[i].rt.bottom = player_msl1[i].y + player_msl1[i].sy;
			if (IntersectRect(&notrt, &player_msl1[i].rt, &eplace[player_msl1[i].place]))//
			{
				player_msl1[i].dead = 1;
				*ebooms = 1;
				*eboomx = (eplace[player_msl1[i].place].left + eplace[player_msl1[i].place].right) / 2 - 25;//�պ���
				*eboomy = (eplace[player_msl1[i].place].top + eplace[player_msl1[i].place].bottom) / 2 - 25;//�պ���
			}
		}
	}
}
void attakToEnemy_down(int* m2count, int* m1count, missile* player_msl2, missile* player_msl1, int* eselectplace, RECT* eplace)
{
	for (int i = 0; i < *m2count; i++)
	{
		if (player_msl2[i].dead == 0)
		{
			player_msl2[i].x += 1;
			player_msl2[i].y += 4;
			if (player_msl2[i].y >= 1000)
			{
				player_msl2[i].dead = 1;
				player_msl2[i].y = 1000 + 100;
				(*m1count)++;
				if (*eselectplace == 4)
				{
					player_msl1[i].x = (eplace[4].left + eplace[4].right) / 2 - 15;//��� ��ġ���� ����� ��ǥ �־��ָ�� place ��ġ
					player_msl1[i].place = 4;
				}
				else if (*eselectplace == 3)
				{
					player_msl1[i].x = (eplace[0].left + eplace[0].right) / 2 - 15;
					player_msl1[i].place = 3;
				}
				else if (*eselectplace == 2)
				{
					player_msl1[i].x = (eplace[0].left + eplace[0].right) / 2 - 15;
					player_msl1[i].place = 2;
				}
				else if (*eselectplace == 1)
				{
					player_msl1[i].x = (eplace[0].left + eplace[0].right) / 2 - 15;
					player_msl1[i].place = 1;
				}
				else if (*eselectplace == 0)
				{
					player_msl1[i].x = (eplace[0].left + eplace[0].right) / 2 - 15;
					player_msl1[i].place = 0;
				}

			}
		}
	}
}