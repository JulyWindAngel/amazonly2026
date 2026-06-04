/*
¿Ó‘• 2500012714
yu_li25@stu.pku.edu.cn

TIME:2025.11

*/

/*
11.19
Problems:
1.sometimes clicks are not sensative
2.when I move the queens or arrows,sometimes the graphics are not smooth
3.the queens can be pulled by pressing the left button continiously//solved in 2025.11.20
4.when I leave the game and play the new game,the arrows of the last game are remained.//solved in 2025.11.20

*/

/*
11.20
News:
1.
Queshtion:when I leave the game and play the new game,the arrows of the last game are remained.
Reason:the function "memset" is wrongly used.
BLOCKS*BLOCKS=64 equals to the quantitum of int ,while the third parameter of memset need the number of bites,which is BLOCKS*BLOCKS*64.

2.
I have mastered the use of git, and I upload the version 0.1 to GitHub.
Now I can change my program freely.

3.
Question:the queens can be pulled by pressing the left button continiously
Reason:the origin method to sense click,which is through checking the status of msg.lbutton, is not reasonable.
In fact, the true sign of CLICK is WM_LBUTTONUP
*/

/*
11.21
News:
1.
I have finished the gui,remaining CONTINUE and PAST GAMES

2.
I have written a stupid ai

3.
I have finished the storage of chess manual,but only one game.

Problems:
1.
CONTINUE and PAST GAMES haven't finished yet

2.
It's time to write a true AI
*/

#include<iostream>
#include<graphics.h>
#include<Windows.h>
#include<algorithm>
#include<cstring>
#include<fstream>
#include<cstdio>
#include<cstdlib>
#include<ctime>
#include<string>
#include<sstream>
#include<iomanip>

using namespace std;

const int WIDTH = 1000;
const int HEIGHT = 700;
const int BLOCKS = 8;
const int BLOCKLENGTH = 64;
const int HALFBLOCKLENGTH = BLOCKLENGTH / 2;
const int EDGELENGTH = (HEIGHT - BLOCKS * BLOCKLENGTH) / 2;

void DrawTheBoard();
void DrawTheMenu();
void InTheMenu();
bool ChooseYourRole();
void NewGame();
void ContinueGame();
void PastGames();
void QuitGame();
void InGame();
void TEMPORARY();
void isChosen(int xrecieve, int yrecieve);
void MoveQueen(bool queencolor, int queennum);
void SetArrow(int xrecieve, int yrecieve);
ExMessage MouseLeftButtonClick();
int AIRANDOM();
bool AbleToMove(int xofqueen, int yofqueen);
bool HumanLost();
void RenameFile();
int AIRecieve();
void temptest();
int ReadTheManual(char* FileName,int Maxstep);

int ArrowInBoard[BLOCKS][BLOCKS] = {};
//REMEMBER:the true arrows in board equals to the transposition of ArrowInBoard[][].
int gamenum = 1;
int step = 1;
int roleofplayer = 0;
int gamemode = 0;//0:NOW    1:LAST

//ofstream outFile("GAME.txt");

struct queen {
	int x;
	int y;
};

queen whitequeen[4];
queen blackqueen[4];

int main() {

	srand((unsigned)time(0));
	initgraph(WIDTH, HEIGHT);
	InTheMenu();

	return 0;
}

void DrawTheMenu() {
	setbkcolor(CYAN);
	cleardevice();
	setfillcolor(YELLOW);
	int ydrawmenu;
	int widthdrawmenu = WIDTH * 4 / 5;
	for (ydrawmenu = (HEIGHT - BLOCKS * BLOCKLENGTH) / 2; ydrawmenu < (HEIGHT + BLOCKS * BLOCKLENGTH) / 2; ydrawmenu += 2 * BLOCKLENGTH) {
		solidrectangle((WIDTH - widthdrawmenu) / 2, ydrawmenu, (WIDTH + widthdrawmenu) / 2, ydrawmenu + BLOCKLENGTH);
	}
	settextcolor(BLACK);
	setbkcolor(YELLOW);
	outtextxy((WIDTH - widthdrawmenu) / 2, (HEIGHT - BLOCKS * BLOCKLENGTH) / 2, _T("NEW GAME"));
	outtextxy((WIDTH - widthdrawmenu) / 2, (HEIGHT - BLOCKS * BLOCKLENGTH) / 2 + 2 * BLOCKLENGTH, _T("CONTINUE"));
	outtextxy((WIDTH - widthdrawmenu) / 2, (HEIGHT - BLOCKS * BLOCKLENGTH) / 2 + 4 * BLOCKLENGTH, _T("LAST GAME"));
	outtextxy((WIDTH - widthdrawmenu) / 2, (HEIGHT - BLOCKS * BLOCKLENGTH) / 2 + 6 * BLOCKLENGTH, _T("QUIT"));
}//DrawTheMEnu


void InTheMenu() {
	DrawTheMenu();
	flushmessage();
	ExMessage mouseclick;
	//BeginBatchDraw();
	while (1) {
		mouseclick = MouseLeftButtonClick();
		int widthdrawmenu = WIDTH * 4 / 5;
		if (mouseclick.x >= (WIDTH - widthdrawmenu) / 2 && mouseclick.x <= (WIDTH + widthdrawmenu) / 2) {
			switch ((mouseclick.y - BLOCKLENGTH) / BLOCKLENGTH) {
			case 0:NewGame(); return;
			case 2:ContinueGame(); return;
			case 4:PastGames(); return;
			case 6:QuitGame(); return;
			}
		}
		//FlushBatchDraw();
		//Sleep(10);
		//mouseclick.lbutton = 0;
		//EndBatchDraw();
	}
}//InTheMenu



void DrawTheBoard() {
	setbkcolor(CYAN);
	cleardevice();
	int xdrawboard, ydrawboard;
	int flagdrawboard = 0;
	for (ydrawboard = (HEIGHT - BLOCKS * BLOCKLENGTH) / 2; ydrawboard < (HEIGHT + BLOCKS * BLOCKLENGTH) / 2; ydrawboard += BLOCKLENGTH) {
		for (xdrawboard = (HEIGHT - BLOCKS * BLOCKLENGTH) / 2; xdrawboard < (HEIGHT + BLOCKS * BLOCKLENGTH) / 2; xdrawboard += BLOCKLENGTH) {
			if (flagdrawboard % 2) {
				setfillcolor(WHITE);
			}
			else {
				setfillcolor(LIGHTGRAY);
			}
			solidrectangle(xdrawboard, ydrawboard, xdrawboard + BLOCKLENGTH, ydrawboard + BLOCKLENGTH);
			flagdrawboard++;
		}
		flagdrawboard++;
	}
	char numbersOfBoard = '0';
	for (xdrawboard = (HEIGHT - BLOCKS * BLOCKLENGTH) / 2; xdrawboard < (HEIGHT + BLOCKS * BLOCKLENGTH) / 2; xdrawboard += BLOCKLENGTH) {
		outtextxy(xdrawboard + 30, (HEIGHT - BLOCKS * BLOCKLENGTH) / 2 - 32, numbersOfBoard);
		outtextxy((HEIGHT - BLOCKS * BLOCKLENGTH) / 2 - 32, xdrawboard + 30, numbersOfBoard);
		numbersOfBoard++;
	}
	//board

	setfillcolor(LIGHTGRAY);
	setbkcolor(LIGHTGRAY);
	settextcolor(BLACK);
	solidrectangle((HEIGHT - BLOCKS * BLOCKLENGTH) / 2 + 10 * BLOCKLENGTH, (HEIGHT - BLOCKS * BLOCKLENGTH) / 2, (HEIGHT - BLOCKS * BLOCKLENGTH) / 2 + 13 * BLOCKLENGTH, (HEIGHT - BLOCKS * BLOCKLENGTH) / 2 + BLOCKLENGTH);
	/*TCHAR gameinformation[20] = {'G','A','M','E'};
	gameinformation[4] = gamenum / 10 + '0';
	gameinformation[5] = gamenum % 10 + '0';
	gameinformation[6] = ' ';
	gameinformation[7] = ' ';
	gameinformation[8] = 'S';
	gameinformation[9] = 'T';
	gameinformation[10] = 'E';
	gameinformation[11] = 'P';
	gameinformation[12] = step / 10 + '0';
	gameinformation[13] = step % 10 + '0';*/
	step = step - roleofplayer;
	TCHAR gameinformation[] = { 'G','A','M','E','0','0',' ','R','O','U','N','D','0','0',0 };
	//gameinformation[4] = gamenum / 10 + '0';
	//gameinformation[5] = gamenum % 10 + '0';
	if (gamemode == 0) {
		gameinformation[0] = 'N';
		gameinformation[1] = 'O';
		gameinformation[2] = 'W';
		gameinformation[3] = ' ';
	}
	else if (gamemode == 1) {
		gameinformation[0] = 'L';
		gameinformation[1] = 'A';
		gameinformation[2] = 'S';
		gameinformation[3] = 'T';
	}
	gameinformation[4] = ' ';
	gameinformation[5] = ' ';
	gameinformation[12] = (step / 2 + 1) / 10 + '0';
	gameinformation[13] = (step / 2 + 1) % 10 + '0';
	step = step + roleofplayer;

	outtextxy((HEIGHT - BLOCKS * BLOCKLENGTH) / 2 + 10 * BLOCKLENGTH, (HEIGHT - BLOCKS * BLOCKLENGTH) / 2, gameinformation);
	solidrectangle((HEIGHT - BLOCKS * BLOCKLENGTH) / 2 + 10 * BLOCKLENGTH, (HEIGHT - BLOCKS * BLOCKLENGTH) / 2 + 2 * BLOCKLENGTH, (HEIGHT - BLOCKS * BLOCKLENGTH) / 2 + 13 * BLOCKLENGTH, (HEIGHT - BLOCKS * BLOCKLENGTH) / 2 + 3 * BLOCKLENGTH);
	outtextxy((HEIGHT - BLOCKS * BLOCKLENGTH) / 2 + 10 * BLOCKLENGTH, (HEIGHT - BLOCKS * BLOCKLENGTH) / 2 + 2 * BLOCKLENGTH, _T("LEAVE"));
	//solidrectangle((HEIGHT - BLOCKS * BLOCKLENGTH) / 2 + 10 * BLOCKLENGTH, (HEIGHT - BLOCKS * BLOCKLENGTH) / 2 + 4 * BLOCKLENGTH, (HEIGHT - BLOCKS * BLOCKLENGTH) / 2 + 13 * BLOCKLENGTH, (HEIGHT - BLOCKS * BLOCKLENGTH) / 2 + 5 * BLOCKLENGTH);
	//outtextxy((HEIGHT - BLOCKS * BLOCKLENGTH) / 2 + 10 * BLOCKLENGTH, (HEIGHT - BLOCKS * BLOCKLENGTH) / 2 + 4 * BLOCKLENGTH, _T("RESIGN"));
	solidrectangle((HEIGHT - BLOCKS * BLOCKLENGTH) / 2 + 10 * BLOCKLENGTH, (HEIGHT - BLOCKS * BLOCKLENGTH) / 2 + 6 * BLOCKLENGTH, (HEIGHT - BLOCKS * BLOCKLENGTH) / 2 + 13 * BLOCKLENGTH, (HEIGHT - BLOCKS * BLOCKLENGTH) / 2 + 7 * BLOCKLENGTH);
	if (step == 200) {
		outtextxy((HEIGHT - BLOCKS * BLOCKLENGTH) / 2 + 10 * BLOCKLENGTH, (HEIGHT - BLOCKS * BLOCKLENGTH) / 2 + 6 * BLOCKLENGTH, _T("BLACK WIN, CLICK TO LEAVE"));
	}
	else if (step == 201) {
		outtextxy((HEIGHT - BLOCKS * BLOCKLENGTH) / 2 + 10 * BLOCKLENGTH, (HEIGHT - BLOCKS * BLOCKLENGTH) / 2 + 6 * BLOCKLENGTH, _T("WHITE WIN, CLICK TO LEAVE"));
	}
	else if (step % 2 == 1) {
		outtextxy((HEIGHT - BLOCKS * BLOCKLENGTH) / 2 + 10 * BLOCKLENGTH, (HEIGHT - BLOCKS * BLOCKLENGTH) / 2 + 6 * BLOCKLENGTH, _T("BLACK'S TURN"));
	}
	else {
		outtextxy((HEIGHT - BLOCKS * BLOCKLENGTH) / 2 + 10 * BLOCKLENGTH, (HEIGHT - BLOCKS * BLOCKLENGTH) / 2 + 6 * BLOCKLENGTH, _T("WHITE'S TURN"));
	}
}//DrawTheBoard


void DrawQueenArrow() {
	DrawTheBoard();
	int xDrawQueenArrow, yDrawQueenArrow;

	//draw the arrows
	setfillcolor(BLUE);
	for (xDrawQueenArrow = 0; xDrawQueenArrow < BLOCKS; xDrawQueenArrow++) {
		for (yDrawQueenArrow = 0; yDrawQueenArrow < BLOCKS; yDrawQueenArrow++) {
			if (ArrowInBoard[xDrawQueenArrow][yDrawQueenArrow] == 1) {
				solidcircle(EDGELENGTH + (xDrawQueenArrow)*BLOCKLENGTH + HALFBLOCKLENGTH, EDGELENGTH + yDrawQueenArrow * BLOCKLENGTH + HALFBLOCKLENGTH, HALFBLOCKLENGTH / 2);
			}
		}
	}

	//draw the queens
	setlinecolor(BLACK);
	int iDrawQueenArrow;
	for (iDrawQueenArrow = 0; iDrawQueenArrow < 4; iDrawQueenArrow++) {
		setfillcolor(BLACK);
		fillcircle(blackqueen[iDrawQueenArrow].x * BLOCKLENGTH + EDGELENGTH + HALFBLOCKLENGTH, blackqueen[iDrawQueenArrow].y * BLOCKLENGTH + EDGELENGTH + HALFBLOCKLENGTH, HALFBLOCKLENGTH / 2);
		setfillcolor(WHITE);
		fillcircle(whitequeen[iDrawQueenArrow].x * BLOCKLENGTH + EDGELENGTH + HALFBLOCKLENGTH, whitequeen[iDrawQueenArrow].y * BLOCKLENGTH + EDGELENGTH + HALFBLOCKLENGTH, HALFBLOCKLENGTH / 2);
	}



}

bool ChooseYourRole() {
	setbkcolor(CYAN);
	cleardevice();
	setfillcolor(BLACK);
	solidrectangle(WIDTH / 4 - BLOCKLENGTH, HEIGHT / 2 - BLOCKLENGTH, WIDTH / 4 + BLOCKLENGTH, HEIGHT / 2 + BLOCKLENGTH);
	setfillcolor(WHITE);
	solidrectangle(WIDTH * 3 / 4 - BLOCKLENGTH, HEIGHT / 2 - BLOCKLENGTH, WIDTH * 3 / 4 + BLOCKLENGTH, HEIGHT / 2 + BLOCKLENGTH);
	setfillcolor(LIGHTGRAY);
	setbkcolor(LIGHTGRAY);
	solidrectangle(WIDTH / 2 - 2 * BLOCKLENGTH, HEIGHT - 3 * BLOCKLENGTH, WIDTH / 2 + 2 * BLOCKLENGTH, HEIGHT - 2 * BLOCKLENGTH);
	outtextxy(WIDTH / 2 - 2 * BLOCKLENGTH, 3 * BLOCKLENGTH, _T("CHOOSE YOUR ROLE"));
	outtextxy(WIDTH / 2 - 2 * BLOCKLENGTH, HEIGHT - 3 * BLOCKLENGTH, _T("BACK"));
	ExMessage msgchooseyourrole;
	while (1) {
		msgchooseyourrole = MouseLeftButtonClick();
		if (msgchooseyourrole.y >= HEIGHT / 2 - BLOCKLENGTH && msgchooseyourrole.y <= HEIGHT / 2 + BLOCKLENGTH) {
			if (msgchooseyourrole.x >= WIDTH / 4 - BLOCKLENGTH && msgchooseyourrole.x <= WIDTH / 4 + BLOCKLENGTH) {
				roleofplayer = 0;
				RenameFile();
				ofstream outFile("GAME.txt");
				outFile.close();
				return 0;
			}
			else if (msgchooseyourrole.x >= WIDTH * 3 / 4 - BLOCKLENGTH && msgchooseyourrole.x <= WIDTH * 3 / 4 + BLOCKLENGTH) {
				RenameFile();
				ofstream outFile("GAME.txt");
				outFile << "-1 -1 -1 -1 -1 -1" << endl;
				outFile.close();
				roleofplayer = 1;
				return 0;
			}
		}
		else if (msgchooseyourrole.x >= WIDTH / 2 - 2 * BLOCKLENGTH && msgchooseyourrole.x <= WIDTH / 2 + 2 * BLOCKLENGTH && msgchooseyourrole.y >= HEIGHT - 3 * BLOCKLENGTH && msgchooseyourrole.y <= HEIGHT - 2 * BLOCKLENGTH) {
			InTheMenu();
			return 1;
		}
	}

}

void NewGame() {

	if (ChooseYourRole()) {
		return;
	}

	step = 1;
	gamemode = 0;

	//initialize the arrows
	memset(ArrowInBoard, 0, sizeof(ArrowInBoard));

	//initialize the queens
	blackqueen[0].x = 0;
	blackqueen[0].y = 2;
	blackqueen[1].x = 2;
	blackqueen[1].y = 0;
	blackqueen[2].x = 5;
	blackqueen[2].y = 0;
	blackqueen[3].x = 7;
	blackqueen[3].y = 2;

	whitequeen[0].x = 7;
	whitequeen[0].y = 5;
	whitequeen[1].x = 5;
	whitequeen[1].y = 7;
	whitequeen[2].x = 2;
	whitequeen[2].y = 7;
	whitequeen[3].x = 0;
	whitequeen[3].y = 5;

	/*
	char newfilename[] = "GAME00.txt";
	newfilename[4] = gamenum / 10 + '0';
	newfilename[5] = gamenum % 10 + '0';

	std::ofstream outFile(newfilename);

	if (!outFile.is_open()) {
		cout << "File Open Error!";
		return;
	}*/
	/*
	char newfilename[] = "GAME00.txt";
	newfilename[4] = gamenum / 10 + '0';
	newfilename[5] = gamenum % 10 + '0';
	const char* old_name = "GAME.txt";
	const char* new_name = newfilename;
	if (rename(old_name, new_name) != 0) {
		cout << "RENAME ERROR!" << endl;
		return;
	}
	*/

	InGame();
}

void ContinueGame() {
	gamemode = 0;
	char GAMETXT[] = "GAME.txt";
	step = ReadTheManual(GAMETXT,57) - !(roleofplayer);
	InGame();
}

void PastGames() {
	gamemode = 1;
	char LastGame[] = "LASTGAME.txt";
	int TotalSteps = ReadTheManual(LastGame,57);
	step = 0;
	//cout << TotalSteps << endl;
	//int countSteps = 0;
	//solidrectangle((HEIGHT - BLOCKS * BLOCKLENGTH) / 2 + 10 * BLOCKLENGTH, (HEIGHT - BLOCKS * BLOCKLENGTH) / 2 + 6 * BLOCKLENGTH, (HEIGHT - BLOCKS * BLOCKLENGTH) / 2 + 13 * BLOCKLENGTH, (HEIGHT - BLOCKS * BLOCKLENGTH) / 2 + 7 * BLOCKLENGTH);
	while (step < TotalSteps) {
		//cout << countSteps << endl;
		ReadTheManual(LastGame, step);
		step++;
		DrawQueenArrow();
		//countSteps++;
		outtextxy((HEIGHT - BLOCKS * BLOCKLENGTH) / 2 + 10 * BLOCKLENGTH, (HEIGHT - BLOCKS * BLOCKLENGTH) / 2 + 6 * BLOCKLENGTH + 32, _T("NEXT->"));
		ExMessage msgPastGames;
		while (1) {
			msgPastGames = MouseLeftButtonClick();
			if (msgPastGames.x >= (HEIGHT - BLOCKS * BLOCKLENGTH) / 2 + 10 * BLOCKLENGTH && msgPastGames.x <= (HEIGHT - BLOCKS * BLOCKLENGTH) / 2 + 13 * BLOCKLENGTH
				&& msgPastGames.y >= (HEIGHT - BLOCKS * BLOCKLENGTH) / 2 + 6 * BLOCKLENGTH && msgPastGames.y <= (HEIGHT - BLOCKS * BLOCKLENGTH) / 2 + 7 * BLOCKLENGTH){
				DrawQueenArrow();
				if (step < TotalSteps) {
					outtextxy((HEIGHT - BLOCKS * BLOCKLENGTH) / 2 + 10 * BLOCKLENGTH, (HEIGHT - BLOCKS * BLOCKLENGTH) / 2 + 6 * BLOCKLENGTH + 32, _T("NEXT->"));
				}
				else {
					outtextxy((HEIGHT - BLOCKS * BLOCKLENGTH) / 2 + 10 * BLOCKLENGTH, (HEIGHT - BLOCKS * BLOCKLENGTH) / 2 + 6 * BLOCKLENGTH + 32, _T("END.CLICK TO LEAVE"));
					MouseLeftButtonClick();
					InTheMenu();
				}
				break;
				}
		}
	}



}

void QuitGame() {
	setbkcolor(CYAN);
	cleardevice();
	outtextxy(WIDTH / 2, HEIGHT / 2, _T("GOODBYE!"));
	Sleep(500);
	//TEMPORARY();
}

void InGame() {//the soul part of front-end

	ExMessage ingame;
	int flagingame = 0;
	DrawQueenArrow();
	while (!flagingame) {
		DrawQueenArrow();
		if ((step % 2 + roleofplayer) % 2 == 1) {//Human's turn
			ingame = MouseLeftButtonClick();
			if (ingame.x >= (HEIGHT - BLOCKS * BLOCKLENGTH) / 2 + 10 * BLOCKLENGTH && ingame.x <= (HEIGHT - BLOCKS * BLOCKLENGTH) / 2 + 13 * BLOCKLENGTH) {
				switch (ingame.y / BLOCKLENGTH) {
				case 3:flagingame = 3; break;
				//case 5:flagingame = 5; break;
				}
			}
			else if (ingame.x >= EDGELENGTH && ingame.x <= EDGELENGTH + BLOCKS * BLOCKLENGTH && ingame.y >= EDGELENGTH && ingame.y < +EDGELENGTH + BLOCKS * BLOCKLENGTH) {
				int xingame = (ingame.x - EDGELENGTH) / BLOCKLENGTH;
				int yingame = (ingame.y - EDGELENGTH) / BLOCKLENGTH;
				for (int iingame = 0; iingame < 4; iingame++) {
					if (roleofplayer == 0 && step % 2 == 1 && blackqueen[iingame].x == xingame && blackqueen[iingame].y == yingame) {
						ofstream outFile("GAME.txt", std::ios::app);
						outFile << xingame << " " << yingame << " ";
						outFile.close();
						
						setlinecolor(RED);
						isChosen(xingame, yingame);
						MoveQueen(0, iingame);
						step++;
						BeginBatchDraw();
						if (HumanLost()) {
							flagingame = 101;
						}
						DrawQueenArrow();
						FlushBatchDraw();
						EndBatchDraw();
						break;
					}
					else if (roleofplayer == 1 && step % 2 == 0 && whitequeen[iingame].x == xingame && whitequeen[iingame].y == yingame) {
						ofstream outFile("GAME.txt", std::ios::app);
						outFile << xingame << " " << yingame << " ";
						outFile.close();
						setlinecolor(RED);
						isChosen(xingame, yingame);
						MoveQueen(1, iingame);
						step++;
						BeginBatchDraw();
						if (HumanLost()) {
							flagingame = 100;
						}
						DrawQueenArrow();
						FlushBatchDraw();
						EndBatchDraw();
						break;
					}
				}
			}
		}
		else {//AI's turn
			flagingame = AIRecieve();
			step++;
			BeginBatchDraw();
			if (HumanLost()&&flagingame==0) {
				if (roleofplayer == 0) {
					flagingame = 101;
				}
				else {
					flagingame = 100;
				}
			}
			DrawQueenArrow();
			FlushBatchDraw();
			EndBatchDraw();

		}
		switch (flagingame) {
		case 3:InTheMenu(); return;
		//case 5:TEMPORARY(); return;
		case 100:step = 200; DrawQueenArrow(); ingame = MouseLeftButtonClick(); InTheMenu(); return;//BLACK WIN
		case 101:step = 201; DrawQueenArrow(); ingame = MouseLeftButtonClick(); InTheMenu(); return;//WHITE WIN
		}
	}
}

void isChosen(int xrecieve, int yrecieve) {
	int xisChosen, yisChosen;
	const int radiusisChosen = HALFBLOCKLENGTH * 2 / 3;
	//BeginBatchDraw();
	for (xisChosen = xrecieve + 1; xisChosen < BLOCKS; xisChosen++) {
		if (ArrowInBoard[xisChosen][yrecieve]) {
			break;
		}
		int flag = 0;
		for (int i = 0; i < 4; i++) {
			if (blackqueen[i].x == xisChosen && blackqueen[i].y == yrecieve || whitequeen[i].x == xisChosen && whitequeen[i].y == yrecieve) {
				flag = 1;
				break;
			}
		}
		if (flag) {
			break;
		}
		ArrowInBoard[xisChosen][yrecieve] = 2;//2->waiting
		circle(EDGELENGTH + xisChosen * BLOCKLENGTH + HALFBLOCKLENGTH, EDGELENGTH + yrecieve * BLOCKLENGTH + HALFBLOCKLENGTH, radiusisChosen);
	}
	for (xisChosen = xrecieve - 1; xisChosen >= 0; xisChosen--) {
		if (ArrowInBoard[xisChosen][yrecieve]) {
			break;
		}
		int flag = 0;
		for (int i = 0; i < 4; i++) {
			if (blackqueen[i].x == xisChosen && blackqueen[i].y == yrecieve || whitequeen[i].x == xisChosen && whitequeen[i].y == yrecieve) {
				flag = 1;
				break;
			}
		}
		if (flag) {
			break;
		}
		ArrowInBoard[xisChosen][yrecieve] = 2;
		circle(EDGELENGTH + xisChosen * BLOCKLENGTH + HALFBLOCKLENGTH, EDGELENGTH + yrecieve * BLOCKLENGTH + HALFBLOCKLENGTH, radiusisChosen);
	}
	for (yisChosen = yrecieve + 1; yisChosen < BLOCKS; yisChosen++) {
		if (ArrowInBoard[xrecieve][yisChosen]) {
			break;
		}
		int flag = 0;
		for (int i = 0; i < 4; i++) {
			if (blackqueen[i].x == xrecieve && blackqueen[i].y == yisChosen || whitequeen[i].x == xrecieve && whitequeen[i].y == yisChosen) {
				flag = 1;
				break;
			}
		}
		if (flag) {
			break;
		}
		ArrowInBoard[xrecieve][yisChosen] = 2;
		circle(EDGELENGTH + xrecieve * BLOCKLENGTH + HALFBLOCKLENGTH, EDGELENGTH + yisChosen * BLOCKLENGTH + HALFBLOCKLENGTH, radiusisChosen);
	}
	for (yisChosen = yrecieve - 1; yisChosen >= 0; yisChosen--) {
		if (ArrowInBoard[xrecieve][yisChosen]) {
			break;
		}
		int flag = 0;
		for (int i = 0; i < 4; i++) {
			if (blackqueen[i].x == xrecieve && blackqueen[i].y == yisChosen || whitequeen[i].x == xrecieve && whitequeen[i].y == yisChosen) {
				flag = 1;
				break;
			}
		}
		if (flag) {
			break;
		}
		ArrowInBoard[xrecieve][yisChosen] = 2;
		circle(EDGELENGTH + xrecieve * BLOCKLENGTH + HALFBLOCKLENGTH, EDGELENGTH + yisChosen * BLOCKLENGTH + HALFBLOCKLENGTH, radiusisChosen);
	}
	int diagsum = xrecieve + yrecieve;
	for (xisChosen = xrecieve + 1; xisChosen < BLOCKS && (diagsum - xisChosen) >= 0; xisChosen++) {
		if (ArrowInBoard[xisChosen][diagsum - xisChosen]) {
			break;
		}
		int flag = 0;
		for (int i = 0; i < 4; i++) {
			if (blackqueen[i].x == xisChosen && blackqueen[i].y == diagsum - xisChosen || whitequeen[i].x == xisChosen && whitequeen[i].y == diagsum - xisChosen) {
				flag = 1;
				break;
			}
		}
		if (flag) {
			break;
		}
		ArrowInBoard[xisChosen][diagsum - xisChosen] = 2;
		circle(EDGELENGTH + xisChosen * BLOCKLENGTH + HALFBLOCKLENGTH, EDGELENGTH + (diagsum - xisChosen) * BLOCKLENGTH + HALFBLOCKLENGTH, radiusisChosen);
	}
	for (xisChosen = xrecieve - 1; xisChosen >= 0 && (diagsum - xisChosen) < BLOCKS; xisChosen--) {
		if (ArrowInBoard[xisChosen][diagsum - xisChosen]) {
			break;
		}
		int flag = 0;
		for (int i = 0; i < 4; i++) {
			if (blackqueen[i].x == xisChosen && blackqueen[i].y == diagsum - xisChosen || whitequeen[i].x == xisChosen && whitequeen[i].y == diagsum - xisChosen) {
				flag = 1;
				break;
			}
		}
		if (flag) {
			break;
		}
		ArrowInBoard[xisChosen][diagsum - xisChosen] = 2;
		circle(EDGELENGTH + xisChosen * BLOCKLENGTH + HALFBLOCKLENGTH, EDGELENGTH + (diagsum - xisChosen) * BLOCKLENGTH + HALFBLOCKLENGTH, radiusisChosen);
	}
	int diagdif = xrecieve - yrecieve + BLOCKS - 1;
	for (xisChosen = xrecieve + 1; xisChosen < BLOCKS && (xisChosen + BLOCKS - 1 - diagdif) < BLOCKS; xisChosen++) {
		if (ArrowInBoard[xisChosen][xisChosen + BLOCKS - 1 - diagdif]) {
			break;
		}
		int flag = 0;
		for (int i = 0; i < 4; i++) {
			if (blackqueen[i].x == xisChosen && blackqueen[i].y == xisChosen + BLOCKS - 1 - diagdif || whitequeen[i].x == xisChosen && whitequeen[i].y == xisChosen + BLOCKS - 1 - diagdif) {
				flag = 1;
				break;
			}
		}
		if (flag) {
			break;
		}
		ArrowInBoard[xisChosen][xisChosen + BLOCKS - 1 - diagdif] = 2;
		circle(EDGELENGTH + xisChosen * BLOCKLENGTH + HALFBLOCKLENGTH, EDGELENGTH + (xisChosen + BLOCKS - 1 - diagdif) * BLOCKLENGTH + HALFBLOCKLENGTH, radiusisChosen);
	}
	for (xisChosen = xrecieve - 1; xisChosen >= 0 && (xisChosen + BLOCKS - 1 - diagdif) >= 0; xisChosen--) {
		if (ArrowInBoard[xisChosen][xisChosen + BLOCKS - 1 - diagdif]) {
			break;
		}
		int flag = 0;
		for (int i = 0; i < 4; i++) {
			if (blackqueen[i].x == xisChosen && blackqueen[i].y == xisChosen + BLOCKS - 1 - diagdif || whitequeen[i].x == xisChosen && whitequeen[i].y == xisChosen + BLOCKS - 1 - diagdif) {
				flag = 1;
				break;
			}
		}
		if (flag) {
			break;
		}
		ArrowInBoard[xisChosen][xisChosen + BLOCKS - 1 - diagdif] = 2;
		circle(EDGELENGTH + xisChosen * BLOCKLENGTH + HALFBLOCKLENGTH, EDGELENGTH + (xisChosen + BLOCKS - 1 - diagdif) * BLOCKLENGTH + HALFBLOCKLENGTH, radiusisChosen);
	}
	//FlushBatchDraw();
	//EndBatchDraw();
}

void MoveQueen(bool queencolor, int queennum) {
	ofstream outFile("GAME.txt", std::ios::app);
	ExMessage msgmovequeen;
	while (1) {
		msgmovequeen = MouseLeftButtonClick();
		int xmovequeen = (msgmovequeen.x - EDGELENGTH) / BLOCKLENGTH;
		int ymovequeen = (msgmovequeen.y - EDGELENGTH) / BLOCKLENGTH;
		if (ArrowInBoard[xmovequeen][ymovequeen] == 2) {
			if (queencolor == 0) {
				blackqueen[queennum].x = xmovequeen;
				blackqueen[queennum].y = ymovequeen;
			}
			else {
				whitequeen[queennum].x = xmovequeen;
				whitequeen[queennum].y = ymovequeen;
			}
			outFile << xmovequeen << " " << ymovequeen << " ";
			for (int i = 0; i < BLOCKS; i++) {
				for (int j = 0; j < BLOCKS; j++) {
					if (ArrowInBoard[i][j] == 2) {
						ArrowInBoard[i][j] = 0;
					}
				}
			}
			outFile.close();
			SetArrow(xmovequeen, ymovequeen);
			return;
		}
	}
}

void SetArrow(int xrecieve, int yrecieve) {
	ofstream outFile("GAME.txt", std::ios::app);
	DrawQueenArrow();
	setlinecolor(BLUE);
	isChosen(xrecieve, yrecieve);
	ExMessage msgsetarrow;
	while (1) {
		msgsetarrow = MouseLeftButtonClick();
		int xsetarrow = (msgsetarrow.x - EDGELENGTH) / BLOCKLENGTH;
		int ysetarrow = (msgsetarrow.y - EDGELENGTH) / BLOCKLENGTH;
		if (ArrowInBoard[xsetarrow][ysetarrow] == 2) {
			ArrowInBoard[xsetarrow][ysetarrow] = 1;
			outFile << xsetarrow << " " << ysetarrow << endl;
			for (int i = 0; i < BLOCKS; i++) {
				for (int j = 0; j < BLOCKS; j++) {
					if (ArrowInBoard[i][j] == 2) {
						ArrowInBoard[i][j] = 0;
					}
				}
			}
			outFile.close();
			return;
		}
	}
}

ExMessage MouseLeftButtonClick() {
	ExMessage msgsth;
	while (1) {
		msgsth = getmessage(EX_MOUSE);
		if (msgsth.message == WM_LBUTTONUP) {
			return msgsth;
		}
	}
}

void TEMPORARY() {
	setbkcolor(CYAN);
	settextcolor(BLACK);
	cleardevice();
	outtextxy(WIDTH / 2, HEIGHT / 2, _T("THE PART IS CONSTRUCTING,CLICK TO GO BACK"));
	ExMessage TEMPORARYwaiting;
	TEMPORARYwaiting = MouseLeftButtonClick();
	InTheMenu();
}

//useless
int AIRANDOM() {
	Sleep(500);
	int oriseedofai;
	oriseedofai = rand();
	ofstream outFile("GAME.txt", std::ios::app);
	if (roleofplayer == 0) {
		int count = 4;
		int seedofai = oriseedofai;
		while (count) {
			count--;
			seedofai = seedofai % 4 + 1;
			if (AbleToMove(whitequeen[seedofai % 4].x, whitequeen[seedofai % 4].y)) {
				outFile << whitequeen[seedofai % 4].x << " " << whitequeen[seedofai % 4].y << " ";
				int flag = 1;
				isChosen(whitequeen[seedofai % 4].x, whitequeen[seedofai % 4].y);
				for (int i = 0; i < BLOCKS; i++) {
					for (int j = 0; j < BLOCKS; j++) {
						if (ArrowInBoard[i][j] == 2) {
							if (flag == 1) {
								flag = 0;
								whitequeen[seedofai % 4].x = i;
								whitequeen[seedofai % 4].y = j;
								outFile << whitequeen[seedofai % 4].x << " " << whitequeen[seedofai % 4].y << " ";
							}
							ArrowInBoard[i][j] = 0;
						}
					}
				}
				isChosen(whitequeen[seedofai % 4].x, whitequeen[seedofai % 4].y);
				flag = 1;
				for (int i = 0; i < BLOCKS; i++) {
					for (int j = 0; j < BLOCKS; j++) {
						if (ArrowInBoard[i][j] == 2) {
							if (flag == 1) {
								flag = 0;
								ArrowInBoard[i][j] = 1;
								outFile << i << " " << j << endl;
							}
							else {
								ArrowInBoard[i][j] = 0;
							}
						}
					}
				}
				outFile.close();
				return 0;
			}
		}
		outFile.close();
		return 100;
	}
	else {
		int count = 4;
		int seedofai = oriseedofai;
		while (count) {
			count--;
			seedofai = seedofai % 4 + 1;
			if (AbleToMove(blackqueen[seedofai % 4].x, blackqueen[seedofai % 4].y)) {
				outFile << blackqueen[seedofai % 4].x << " " << blackqueen[seedofai % 4].y << " ";
				int flag = 1;
				isChosen(blackqueen[seedofai % 4].x, blackqueen[seedofai % 4].y);
				for (int i = 0; i < BLOCKS; i++) {
					for (int j = 0; j < BLOCKS; j++) {
						if (ArrowInBoard[i][j] == 2) {
							if (flag == 1) {
								flag = 0;
								blackqueen[seedofai % 4].x = i;
								blackqueen[seedofai % 4].y = j;
								outFile << blackqueen[seedofai % 4].x << " " << blackqueen[seedofai % 4].y << " ";
							}
							ArrowInBoard[i][j] = 0;
						}
					}
				}
				isChosen(blackqueen[seedofai % 4].x, blackqueen[seedofai % 4].y);
				flag = 1;
				for (int i = 0; i < BLOCKS; i++) {
					for (int j = 0; j < BLOCKS; j++) {
						if (ArrowInBoard[i][j] == 2) {
							if (flag == 1) {
								flag = 0;
								ArrowInBoard[i][j] = 1;
								outFile << i << " " << j << endl;
							}
							else {
								ArrowInBoard[i][j] = 0;
							}
						}
					}
				}
				outFile.close();
				return 0;
			}
		}
		outFile.close();
		return 101;
	}
}

bool AbleToMove(int xofqueen, int yofqueen) {
	isChosen(xofqueen, yofqueen);
	int flag = 0;
	for (int i = 0; i < BLOCKS; i++) {
		for (int j = 0; j < BLOCKS; j++) {
			if (ArrowInBoard[i][j] == 2) {
				flag = 1;
				ArrowInBoard[i][j] = 0;
			}
		}
	}
	if (flag) {
		return 1;
	}
	else {
		return 0;
	}
}
//useless

bool HumanLost() {
	if (roleofplayer == 0) {
		for (int i = 0; i < 4; i++) {
			if (AbleToMove(blackqueen[i].x, blackqueen[i].y)) {
				return 0;
			}
		}
	}
	else {
		for (int i = 0; i < 4; i++) {
			if (AbleToMove(whitequeen[i].x, whitequeen[i].y)) {
				return 0;
			}
		}
	}
	return 1;
}

void RenameFile() {
	const char* oldname = "GAME.txt";
	const char* newname = "LASTGAME.txt";
	remove(newname);
	if (rename(oldname, newname) != 0) {
		cout << "RENAME ERROR!";
		return;
	}
	

}

int AIRecieve() {
	WinExec("GUIamazonlybrain.exe", SW_SHOW);
	Sleep(1500);
	ifstream file("GAME.txt");
	string line, lastline;
	while (getline(file, line)) {
		lastline = line;
	}
	file.close();
	cout << lastline << endl;
	std::istringstream iss(lastline);
	int lastlineNums[6] = {};
	for (int i = 0; i < 6; i++) {
		iss>>lastlineNums[i];
	}


	if (lastlineNums[0] == -2) {
		return (100 + roleofplayer);
	}
	if (roleofplayer == 0) {
		for (int i = 0; i < 4; i++) {
			if (whitequeen[i].x == lastlineNums[0] && whitequeen[i].y == lastlineNums[1]) {
				whitequeen[i].x = lastlineNums[2];
				whitequeen[i].y = lastlineNums[3];
				break;
			}
		}
	}
	else {
		for (int i = 0; i < 4; i++) {
			if (blackqueen[i].x == lastlineNums[0] && blackqueen[i].y == lastlineNums[1]) {
				blackqueen[i].x = lastlineNums[2];
				blackqueen[i].y = lastlineNums[3];
				break;
			}
		}
	}
	ArrowInBoard[lastlineNums[4]][lastlineNums[5]] = 1;
	return 0;
}


//useless
void temptest() {
	cout << endl;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			cout << setw(4) << ArrowInBoard[j][i];
		}
		cout << endl;
	}
	cout << endl;
}
//useless


int ReadTheManual(char* FileName,int Maxstep) {
	ifstream file(FileName);

	//initialize the arrows
	memset(ArrowInBoard, 0, sizeof(ArrowInBoard));

	//initialize the queens
	blackqueen[0].x = 0;
	blackqueen[0].y = 2;
	blackqueen[1].x = 2;
	blackqueen[1].y = 0;
	blackqueen[2].x = 5;
	blackqueen[2].y = 0;
	blackqueen[3].x = 7;
	blackqueen[3].y = 2;

	whitequeen[0].x = 7;
	whitequeen[0].y = 5;
	whitequeen[1].x = 5;
	whitequeen[1].y = 7;
	whitequeen[2].x = 2;
	whitequeen[2].y = 7;
	whitequeen[3].x = 0;
	whitequeen[3].y = 5;

	string line;
	int steps = 0;
	roleofplayer = 0;
	int countMaxstep = Maxstep;
	while (getline(file, line)&&countMaxstep) {
		countMaxstep--;
		istringstream iss(line);
		int lineNums[6];
		for (int i = 0; i < 6; i++) {
			iss >> lineNums[i];
		}
		if (lineNums[0] == -1) {
			roleofplayer = 1;
			steps++;
		}
		else {
			steps++;
			/*
			queens[(steps + role) % 2][queenandrole / 10 - 10].x = lineNums[2];
			queens[(steps + role) % 2][queenandrole / 10 - 10].y = lineNums[3];
			*/
			for (int i = 0; i < 4; i++) {
				if (blackqueen[i].x == lineNums[0] && blackqueen[i].y == lineNums[1]) {
					blackqueen[i].x = lineNums[2];
					blackqueen[i].y = lineNums[3];
					break;
				}
				else if (whitequeen[i].x == lineNums[0] && whitequeen[i].y == lineNums[1]) {
					whitequeen[i].x = lineNums[2];
					whitequeen[i].y = lineNums[3];
					break;
				}
			}
			ArrowInBoard[lineNums[4]][lineNums[5]] = 1;
		}
	}
	file.close();
	return steps;
}
