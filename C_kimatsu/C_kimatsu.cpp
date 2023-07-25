#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Windows.h>
#include <curses.h>
#include <vector>
#include "method.h"

extern std::vector<NumObject*> NumObjects;
extern int cell_coordinate[16][2];
bool clearcheck = false;
int count = 0;	// 経過ターン数

int main()
{
	NumObject* newNumObject = new NumObject();
	int key;

	char section[] = "section1";
	char keyword[] = "keyword1";
	char* filename = getini(section, keyword);
	CSVArray(filename);

	initscr();

	start_color();
	init_color(8, 1000, 753, 796);	// Pink
	init_color(9, 1000, 647, 0);	// Orange
	init_color(10, 500, 500, 500);	// Gray
	init_color(11, 1000, 1000, 878);	// LightYellow
	init_color(12, 1000, 871, 678);	// NavajoWhite

	init_pair(1, COLOR_RED, COLOR_RED);				// 数字オブジェクト2のカラー
	init_pair(2, COLOR_WHITE, COLOR_RED);

	init_pair(3, COLOR_GREEN, COLOR_GREEN);			// 数字オブジェクト4のカラー
	init_pair(4, COLOR_WHITE, COLOR_GREEN);

	init_pair(5, COLOR_BLUE, COLOR_BLUE);			// 数字オブジェクト8のカラー
	init_pair(6, COLOR_WHITE, COLOR_BLUE);

	init_pair(7, COLOR_YELLOW, COLOR_YELLOW);		// 数字オブジェクト16のカラー
	init_pair(8, COLOR_BLACK, COLOR_YELLOW);

	init_pair(9, COLOR_CYAN, COLOR_CYAN);			// 数字オブジェクト32のカラー
	init_pair(10, COLOR_BLACK, COLOR_CYAN);

	init_pair(11, COLOR_MAGENTA, COLOR_MAGENTA);	// 数字オブジェクト64のカラー
	init_pair(12, COLOR_WHITE, COLOR_MAGENTA);

	init_pair(13, COLOR_BLACK, COLOR_BLACK);		// 数字オブジェクト128のカラー
	init_pair(14, COLOR_WHITE, COLOR_BLACK);

	init_pair(15, COLOR_WHITE, COLOR_WHITE);		// 数字オブジェクト256のカラー
	init_pair(16, COLOR_BLACK, COLOR_WHITE);

	init_pair(17, 8, 8);							// 数字オブジェクト512のカラー
	init_pair(18, COLOR_WHITE, 8);

	init_pair(19, 9, 9);							// 数字オブジェクト1024のカラー
	init_pair(20, COLOR_WHITE, 9);

	init_pair(21, 10, 10);							// 数字オブジェクト2048のカラー
	init_pair(22, COLOR_WHITE, 10);

	init_pair(23, 12, 12);							// フィールドのカラー

	init_pair(24, COLOR_BLACK, 11);					// リザルトのカラー

	init_pair(25, 12, 12);							// 背景色

	noecho();
	cbreak();
	keypad(stdscr, TRUE);

	for (int i = 0; i < 2; i++) {	// 最初の数字オブジェクトを生成
		create_struct(newNumObject);
	}

	while (true) {
		erase();

		draw_field();	// フィールドの生成

		for (int i = 0; i < NumObjects.size(); i++) {
			draw_NumObject(NumObjects[i]);
		}

		key = getch();
		if (key == KEY_RIGHT) {	// 右矢印キーが押されたら
			push_right();
			create_struct(newNumObject);
			count++;
		}
		else if (key == KEY_LEFT) {	// 左矢印キーが押されたら
			push_left();
			create_struct(newNumObject);
			count++;
		}
		else if (key == KEY_UP) {	// 上矢印キーが押されたら
			push_up();
			create_struct(newNumObject);
			count++;
		}
		else if (key == KEY_DOWN) {	// 下矢印キーが押されたら
			push_down();
			create_struct(newNumObject);
			count++;
		}

		refresh();

		for (int i = 0; i < NumObjects.size(); i++) {
			if (NumObjects[i]->num == 2048) {	// 数字が2048の数字オブジェクトがあるか
				clearcheck = true;
			}
		}

		if (clearcheck) {	// クリア処理
			write_file(clearcheck, count);

			erase();
			draw_field();
			while (true) {
				for (int i = 0; i < NumObjects.size(); i++) {
					draw_NumObject(NumObjects[i]);
				}

				attrset(COLOR_PAIR(24));
				attron(A_BOLD);
				mvaddstr(8, 55, "Clear");

				mvprintw(10, 55, "経過ターン: %d", count);
				refresh();
			}
		}

		if (NumObjects.size() == 16) {	// ゲームオーバー処理
			write_file(clearcheck, count);

			erase();
			draw_field();
			while (true) {
				for (int i = 0; i < NumObjects.size(); i++) {
					draw_NumObject(NumObjects[i]);
				}

				attrset(COLOR_PAIR(24));
				attron(A_BOLD);
				mvaddstr(8, 55, "Game Over");

				mvprintw(10, 55, "経過ターン: %d", count);
				refresh();
			}
		}
	}

}