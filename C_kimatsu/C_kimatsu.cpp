#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Windows.h>
#include <curses.h>
#include <vector>
#include "method.h"

extern std::vector<NumObject*> NumObjects;
extern int cell_coordinate[16][2];
extern int fieldxmin;
extern int fieldxmax;
extern int fieldymin;
extern int fieldymax;

bool clearcheck = false;
int count = 0;  // 経過ターン数

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
    init_pair(1, COLOR_RED, COLOR_RED);             // 数字オブジェクト2のカラー
    init_pair(2, COLOR_WHITE, COLOR_RED);

    init_pair(3, COLOR_GREEN, COLOR_GREEN);         // 数字オブジェクト4のカラー
    init_pair(4, COLOR_WHITE, COLOR_GREEN);

    init_pair(5, COLOR_BLUE, COLOR_BLUE);           // 数字オブジェクト8のカラー
    init_pair(6, COLOR_WHITE, COLOR_BLUE);

    init_pair(7, COLOR_YELLOW, COLOR_YELLOW);       // 数字オブジェクト16のカラー
    init_pair(8, COLOR_BLACK, COLOR_YELLOW);

    init_pair(9, COLOR_CYAN, COLOR_CYAN);           // 数字オブジェクト32のカラー
    init_pair(10, COLOR_BLACK, COLOR_CYAN);

    init_pair(11, COLOR_MAGENTA, COLOR_MAGENTA);    // 数字オブジェクト64のカラー
    init_pair(12, COLOR_WHITE, COLOR_MAGENTA);

    init_pair(13, COLOR_WHITE, COLOR_WHITE);        // 数字オブジェクト128のカラー
    init_pair(14, COLOR_BLACK, COLOR_WHITE);

    init_pair(17, COLOR_RED, COLOR_RED);            // 数字オブジェクト256のカラー
    init_pair(18, COLOR_BLACK, COLOR_RED);

    init_pair(19, COLOR_GREEN, COLOR_GREEN);        // 数字オブジェクト512のカラー
    init_pair(20, COLOR_BLACK, COLOR_GREEN);

    init_pair(15, COLOR_BLUE, COLOR_BLUE);          // 数字オブジェクト1024のカラー
    init_pair(16, COLOR_YELLOW, COLOR_BLUE);

    init_pair(21, COLOR_YELLOW, COLOR_YELLOW);      // 数字オブジェクト2048のカラー
    init_pair(22, COLOR_BLUE, COLOR_YELLOW);

    init_pair(23, COLOR_WHITE, COLOR_BLACK);        // フィールドのカラー

    init_pair(24, COLOR_RED, COLOR_BLACK);          // リザルトのカラー

    curs_set(0);    // カーソルを表示しない

    noecho();
    cbreak();
    keypad(stdscr, TRUE);

    for (int i = 0; i < 2; i++) {   // 最初の数字オブジェクトを生成
        create_struct(newNumObject);
    }

    while (true) {
        erase();

        draw_field();   // フィールドの生成

        attrset(COLOR_PAIR(23));
        attron(A_BOLD);
        mvaddstr(0, (fieldxmin + fieldxmax) / 2 - 8, "Welcome to 2048");    // タイトルの表示

        mvaddstr((fieldymin + fieldymax) / 2 - 2, fieldxmax + 15, "d");
        mvaddstr((fieldymin + fieldymax) / 2 - 1, fieldxmax + 13, "a");
        mvaddstr((fieldymin + fieldymax) / 2 - 1, fieldxmax + 15, "s");
        mvaddstr((fieldymin + fieldymax) / 2 - 1, fieldxmax + 17, "d");

        mvaddstr((fieldymin + fieldymax) / 2 + 1, fieldxmax + 15, "上");
        mvaddstr((fieldymin + fieldymax) / 2 + 2, fieldxmax + 13, "左");
        mvaddstr((fieldymin + fieldymax) / 2 + 2, fieldxmax + 15, "下");
        mvaddstr((fieldymin + fieldymax) / 2 + 2, fieldxmax + 17, "右");

        mvprintw((fieldymin + fieldymax) / 2, fieldxmin - 20, "turn: %d", count);  // 経過ターン数の表示

        for (int i = 0; i < NumObjects.size(); i++) {
            draw_NumObject(NumObjects[i]);
        }

        key = getch();
        if (key == 'd') { // dキーが押されたら
            move_right();
            create_struct(newNumObject);
            count++;
        }
        else if (key == 'a') { // aキーが押されたら
            move_left();
            create_struct(newNumObject);
            count++;
        }
        else if (key == 'w') {   // wキーが押されたら
            move_up();
            create_struct(newNumObject);
            count++;
        }
        else if (key == 's') { // sキーが押されたら
            move_down();
            create_struct(newNumObject);
            count++;
        }

        refresh();

        for (int i = 0; i < NumObjects.size(); i++) {
            if (NumObjects[i]->num == 2048) {   // 数字が2048の数字オブジェクトがあるか
                clearcheck = true;
            }
        }

        if (clearcheck) {   // クリア処理
            write_file(clearcheck, count);

            erase();
            draw_field();
            while (true) {
                for (int i = 0; i < NumObjects.size(); i++) {
                    draw_NumObject(NumObjects[i]);
                }

                attrset(COLOR_PAIR(24));
                attron(A_BOLD);
                mvaddstr(fieldymin - 2, (fieldxmin + fieldxmax) / 2 - 3, "Clear");

                mvprintw(fieldymin - 1, (fieldxmin + fieldxmax) / 2 - 3, "turn: %d", count);
                refresh();
            }
        }

        if (NumObjects.size() == 16) {  // ゲームオーバー処理
            write_file(clearcheck, count);

            erase();
            draw_field();
            while (true) {
                for (int i = 0; i < NumObjects.size(); i++) {
                    draw_NumObject(NumObjects[i]);
                }

                attrset(COLOR_PAIR(24));
                attron(A_BOLD);
                mvaddstr(fieldymin - 2, (fieldxmin + fieldxmax) / 2 - 4, "Game Over");

                mvprintw(fieldymin - 1, (fieldxmin + fieldxmax) / 2 - 4, "turn: %d", count);
                refresh();
            }
        }
    }

}