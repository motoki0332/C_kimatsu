#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Windows.h>
#include <curses.h>
#include <vector>
#include <algorithm>
#include <time.h>
#include "method.h"

std::vector<NumObject*> NumObjects;	// �������ꂽ�����I�u�W�F�N�g
int cell_coordinate[16][2] = { {0, 0}, {1, 0}, {2, 0}, {3, 0}, {0, 1}, {1, 1}, {2, 1}, {3, 1},
	{0, 2}, {1, 2}, {2, 2}, {3, 2}, {0, 3}, {1, 3}, {2, 3}, {3, 3} };	// �e�Z���̍��W

int numdata[11];	// �t�@�C���̓ǂݍ��ݗp
int fieldxmin = 0;
int fieldxmax = 48;
int fieldymin = 0;
int fieldymax = 24;

bool x_asc(const struct NumObject* p, const struct NumObject* q) {	// x�̏���
	return p->x < q->x;
}

bool x_desc(const struct NumObject* p, const struct NumObject* q) {	// x�̍~��
	return p->x > q->x;
}

bool y_asc(const struct NumObject* p, const struct NumObject* q) {	// y�̏���
	return p->y < q->y;
}

bool y_desc(const struct NumObject* p, const struct NumObject* q) {	// y�̍~��
	return p->y > q->y;
}

void getGurrentDirectory(char* currentDirectory) {
	GetCurrentDirectory(CHARBUFF, currentDirectory);
}

char* getini(char* sec, char* key) {
	char currentDirectory[CHARBUFF];
	char section[CHARBUFF];
	char keyWord[CHARBUFF];
	char settingFile[CHARBUFF];
	char keyValue[CHARBUFF];

	getGurrentDirectory(currentDirectory);
	sprintf_s(section, sec);
	sprintf_s(keyWord, key);
	sprintf_s(settingFile, "%s\\data.ini", currentDirectory);
	GetPrivateProfileString(section, keyWord, "none", keyValue, CHARBUFF, settingFile);

	return keyValue;
}

void CSVArray(const char* fileName) {	// �t�@�C���̓ǂݍ���
	FILE* fp;
	char s[BUFFSIZE];
	errno_t error;
	char* p1;
	char delim[] = ", ";
	char* ctx;
	int tmp;

	error = fopen_s(&fp, fileName, "r");

	if (error != 0)
		fprintf_s(stderr, "failed to open");
	else {
		while (fgets(s, BUFFSIZE, fp) != NULL) {
			p1 = strtok_s(s, delim, &ctx);
			for (int i = 0; i < 11; i++) {
				tmp = atoi(p1);
				numdata[i] = tmp;
				p1 = strtok_s(NULL, delim, &ctx);
			}
		}
		fclose(fp);
	}
}

void write_file(bool clearcheck, int count) {	// �t�@�C���ɏ����o��
	FILE* fp;
	char s[BUFFSIZE];
	errno_t error;

	error = fopen_s(&fp, "play_data.csv", "w");
	if (error != 0)
		fprintf_s(stderr, "failed to open");
	else {
		if (clearcheck) {
			fputs("Clear\n", fp);
		}
		else {
			fputs("Game Over\n", fp);
		}
		fprintf(fp, "�o�߃^�[��: %d\n", count);
		fclose(fp);
	}
}

NumObject* set_struct(NumObject* newNumObject) {	// �\���̂̃��X�g���Z�b�g
	NumObject* prevNumObject = NULL;
	for (int i = 0; i < 11; i++) {
		NumObject* tmp = new NumObject();

		tmp->num = numdata[i];
		tmp->nextNumObject = NULL;

		if (i == 0) {
			newNumObject = tmp;
		}
		else {
			prevNumObject->nextNumObject = tmp;
		}

		prevNumObject = tmp;
	}

	return newNumObject;
}

void create_struct(NumObject* newNumObject) {	// �V���������I�u�W�F�N�g�𐶐�
	int cell = find_empty_cellnum();
	if (cell != -1) {
		newNumObject = set_struct(newNumObject);
		newNumObject->cell = cell;
		newNumObject->x = cell_coordinate[newNumObject->cell][0];
		newNumObject->y = cell_coordinate[newNumObject->cell][1];
		NumObjects.push_back(newNumObject);
	}
}

void change_struct(int index) {	// �����I�u�W�F�N�g�̃��x������グ��
	int prevcell = NumObjects[index]->cell;
	NumObjects[index] = NumObjects[index]->nextNumObject;

	NumObjects[index]->cell = prevcell;
	NumObjects[index]->x = cell_coordinate[NumObjects[index]->cell][0];
	NumObjects[index]->y = cell_coordinate[NumObjects[index]->cell][1];
}

int find_empty_cellnum() {	// �󂢂Ă���Z����T��
	std::vector<int>empty_cellnum = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
	for (int i = 0; i < NumObjects.size(); i++) {
		empty_cellnum.erase(std::remove(empty_cellnum.begin(), empty_cellnum.end(), NumObjects[i]->cell), empty_cellnum.end());
	}

	srand(time(NULL));

	if (empty_cellnum.size() > 0) {
		int index = rand() % empty_cellnum.size();
		return empty_cellnum[index];
	}
	else {
		return -1;
	}
}

void draw_field() {	// �t�B�[���h�̐���

	for (int i = fieldxmin; i <= fieldxmax; i++) {
		attrset(COLOR_PAIR(23));
		mvaddstr(fieldymin, i, "-");
		mvaddstr(fieldymax, i, "-");
		mvaddstr(fieldymin + 6, i, "-");
		mvaddstr(fieldymin + 12, i, "-");
		mvaddstr(fieldymin + 18, i, "-");
	}

	for (int i = fieldymin; i <= fieldymax; i++) {
		attrset(COLOR_PAIR(23));
		mvaddstr(i, fieldxmin, "|");
		mvaddstr(i, fieldxmax, "|");
		mvaddstr(i, fieldxmin + 12, "|");
		mvaddstr(i, fieldxmin + 24, "|");
		mvaddstr(i, fieldxmin + 36, "|");
	}

}

void draw_NumObject(NumObject* nowNumObject) {	// �����I�u�W�F�N�g�̕\��
	int xline = 0;
	int yline = 0;
	if (nowNumObject->cell == 0 || nowNumObject->cell == 4 || nowNumObject->cell == 8 || nowNumObject->cell == 12) {
		xline = 1;
	}
	else if (nowNumObject->cell == 1 || nowNumObject->cell == 5 || nowNumObject->cell == 9 || nowNumObject->cell == 13) {
		xline = 2;
	}
	else if (nowNumObject->cell == 2 || nowNumObject->cell == 6 || nowNumObject->cell == 10 || nowNumObject->cell == 14) {
		xline = 3;
	}
	else if (nowNumObject->cell == 3 || nowNumObject->cell == 7 || nowNumObject->cell == 11 || nowNumObject->cell == 15) {
		xline = 4;
	}

	if (nowNumObject->cell == 0 || nowNumObject->cell == 1 || nowNumObject->cell == 2 || nowNumObject->cell == 3) {
		yline = 1;
	}
	else if (nowNumObject->cell == 4 || nowNumObject->cell == 5 || nowNumObject->cell == 6 || nowNumObject->cell == 7) {
		yline = 2;
	}
	else if (nowNumObject->cell == 8 || nowNumObject->cell == 9 || nowNumObject->cell == 10 || nowNumObject->cell == 11) {
		yline = 3;
	}
	else if (nowNumObject->cell == 12 || nowNumObject->cell == 13 || nowNumObject->cell == 14 || nowNumObject->cell == 15) {
		yline = 4;
	}

	int xtrue = fieldxmin + nowNumObject->x * 11 + xline;
	int ytrue = fieldymin + nowNumObject->y * 5 + yline;

	if (nowNumObject->num == 2) {	// �����I�u�W�F�N�g�̐������ɐF��ς���
		for (int i = xtrue; i < xtrue + 11; i++) {
			for (int j = ytrue; j < ytrue + 5; j++) {
				attrset(COLOR_PAIR(1));
				mvaddstr(j, i, " ");
			}
		}

		attrset(COLOR_PAIR(2));
		mvprintw(ytrue + 2, xtrue + 4, "%d", nowNumObject->num);
	}
	else if (nowNumObject->num == 4) {
		for (int i = xtrue; i < xtrue + 11; i++) {
			for (int j = ytrue; j < ytrue + 5; j++) {
				attrset(COLOR_PAIR(3));
				mvaddstr(j, i, " ");
			}
		}

		attrset(COLOR_PAIR(4));
		mvprintw(ytrue + 2, xtrue + 4, "%d", nowNumObject->num);
	}
	else if (nowNumObject->num == 8) {
		for (int i = xtrue; i < xtrue + 11; i++) {
			for (int j = ytrue; j < ytrue + 5; j++) {
				attrset(COLOR_PAIR(5));
				mvaddstr(j, i, " ");
			}
		}

		attrset(COLOR_PAIR(6));
		mvprintw(ytrue + 2, xtrue + 4, "%d", nowNumObject->num);
	}
	else if (nowNumObject->num == 16) {
		for (int i = xtrue; i < xtrue + 11; i++) {
			for (int j = ytrue; j < ytrue + 5; j++) {
				attrset(COLOR_PAIR(7));
				mvaddstr(j, i, " ");
			}
		}

		attrset(COLOR_PAIR(8));
		mvprintw(ytrue + 2, xtrue + 4, "%d", nowNumObject->num);
	}
	else if (nowNumObject->num == 32) {
		for (int i = xtrue; i < xtrue + 11; i++) {
			for (int j = ytrue; j < ytrue + 5; j++) {
				attrset(COLOR_PAIR(9));
				mvaddstr(j, i, " ");
			}
		}

		attrset(COLOR_PAIR(10));
		mvprintw(ytrue + 2, xtrue + 4, "%d", nowNumObject->num);
	}
	else if (nowNumObject->num == 64) {
		for (int i = xtrue; i < xtrue + 11; i++) {
			for (int j = ytrue; j < ytrue + 5; j++) {
				attrset(COLOR_PAIR(11));
				mvaddstr(j, i, " ");
			}
		}

		attrset(COLOR_PAIR(12));
		mvprintw(ytrue + 2, xtrue + 4, "%d", nowNumObject->num);
	}
	else if (nowNumObject->num == 128) {
		for (int i = xtrue; i < xtrue + 11; i++) {
			for (int j = ytrue; j < ytrue + 5; j++) {
				attrset(COLOR_PAIR(13));
				mvaddstr(j, i, " ");
			}
		}

		attrset(COLOR_PAIR(14));
		mvprintw(ytrue + 2, xtrue + 4, "%d", nowNumObject->num);
	}
	else if (nowNumObject->num == 256) {
		for (int i = xtrue; i < xtrue + 11; i++) {
			for (int j = ytrue; j < ytrue + 5; j++) {
				attrset(COLOR_PAIR(15));
				mvaddstr(j, i, " ");
			}
		}

		attrset(COLOR_PAIR(16));
		mvprintw(ytrue + 2, xtrue + 4, "%d", nowNumObject->num);
	}
	else if (nowNumObject->num == 512) {
		for (int i = xtrue; i < xtrue + 11; i++) {
			for (int j = ytrue; j < ytrue + 5; j++) {
				attrset(COLOR_PAIR(17));
				mvaddstr(j, i, " ");
			}
		}

		attrset(COLOR_PAIR(18));
		mvprintw(ytrue + 2, xtrue + 4, "%d", nowNumObject->num);
	}
	else if (nowNumObject->num == 1024) {
		for (int i = xtrue; i < xtrue + 11; i++) {
			for (int j = ytrue; j < ytrue + 5; j++) {
				attrset(COLOR_PAIR(19));
				mvaddstr(j, i, " ");
			}
		}

		attrset(COLOR_PAIR(20));
		mvprintw(ytrue + 2, xtrue + 4, "%d", nowNumObject->num);
	}
	else if (nowNumObject->num == 2048) {
		for (int i = xtrue; i < xtrue + 11; i++) {
			for (int j = ytrue; j < ytrue + 5; j++) {
				attrset(COLOR_PAIR(21));
				mvaddstr(j, i, " ");
			}
		}

		attrset(COLOR_PAIR(22));
		mvprintw(ytrue + 2, xtrue + 4, "%d", nowNumObject->num);
	}
}

void push_right() {	// �E���L�[�������ꂽ�ꍇ
	sort(NumObjects.begin(), NumObjects.end(), x_desc);	// x���W���傫����(���E�ɂ��鏇)�Ƀ\�[�g

	for (int i = 0; i < NumObjects.size(); i++) {	// �S�Ă̐����I�u�W�F�N�g�ɂ���
		while (NumObjects[i]->cell != 3 && NumObjects[i]->cell != 7 && NumObjects[i]->cell != 11 && NumObjects[i]->cell != 15) {	// �����I�u�W�F�N�g�̈ʒu���ŉE�[�ɂȂ�܂�
			NumObjects[i]->cell += 1;	// �����I�u�W�F�N�g���E��1�ړ�

			bool equal_location = false;
			int index = 0;
			for (int j = i - 1; j >= 0; j--) {
				if (NumObjects[i]->cell == NumObjects[j]->cell) {	// �ړ���ɕʂ̐����I�u�W�F�N�g������΃t���O��true��
					equal_location = true;
					index = j;
					break;
				}
			}

			if (equal_location) {	// �ړ���ɕʂ̐����I�u�W�F�N�g�����鎞
				if (NumObjects[i]->num == NumObjects[index]->num) {	// ���g�ƈړ���̐����I�u�W�F�N�g�̐����������Ȃ�
					change_struct(index);	// �������Ĉړ���̐����I�u�W�F�N�g�̃��x������グ��
					NumObjects.erase(NumObjects.begin() + i);	// ���g���폜
					i--;	// �폜�������C���f�b�N�X���������
					break;
				}
				else {
					NumObjects[i]->cell -= 1;	// �����ł��Ȃ������ꍇ1�O�̈ʒu�ɖ߂�
					break;
				}
			}
		}
		NumObjects[i]->x = cell_coordinate[NumObjects[i]->cell][0];	// �Z���ԍ��ɍ��킹���W���X�V
		NumObjects[i]->y = cell_coordinate[NumObjects[i]->cell][1];	// �Z���ԍ��ɍ��킹���W���X�V
	}
}

void push_left() {	// �����L�[�������ꂽ�ꍇ(push_right�Ɠ��l)
	sort(NumObjects.begin(), NumObjects.end(), x_asc);	// x���W����������(��荶�ɂ��鏇)�Ƀ\�[�g

	for (int i = 0; i < NumObjects.size(); i++) {
		while (NumObjects[i]->cell != 0 && NumObjects[i]->cell != 4 && NumObjects[i]->cell != 8 && NumObjects[i]->cell != 12) {	// �����I�u�W�F�N�g�̈ʒu���ō��[�ɂȂ�܂�
			NumObjects[i]->cell -= 1;

			bool equal_location = false;
			int index = 0;
			for (int j = i - 1; j >= 0; j--) {
				if (NumObjects[i]->cell == NumObjects[j]->cell) {
					equal_location = true;
					index = j;
					break;
				}
			}

			if (equal_location) {
				if (NumObjects[i]->num == NumObjects[index]->num) {	
					change_struct(index);
					NumObjects.erase(NumObjects.begin() + i);
					i--;
					break;
				}
				else {
					NumObjects[i]->cell += 1;
					break;
				}
			}
		}
		NumObjects[i]->x = cell_coordinate[NumObjects[i]->cell][0];
		NumObjects[i]->y = cell_coordinate[NumObjects[i]->cell][1];
	}
}

void push_up() {	// ����L�[�������ꂽ�ꍇ(push_right�Ɠ��l)
	sort(NumObjects.begin(), NumObjects.end(), y_asc);	// y���W����������(����ɂ��鏇)�Ƀ\�[�g

	for (int i = 0; i < NumObjects.size(); i++) {	// �S�Ă̐����I�u�W�F�N�g�ɂ���
		while (NumObjects[i]->cell != 0 && NumObjects[i]->cell != 1 && NumObjects[i]->cell != 2 && NumObjects[i]->cell != 3) {	// �����I�u�W�F�N�g�̈ʒu���ŏ�[�ɂȂ�܂�
			NumObjects[i]->cell -= 4;

			bool equal_location = false;
			int index = 0;
			for (int j = i - 1; j >= 0; j--) {
				if (NumObjects[i]->cell == NumObjects[j]->cell) {
					equal_location = true;
					index = j;
					break;
				}
			}

			if (equal_location) {
				if (NumObjects[i]->num == NumObjects[index]->num) {
					change_struct(index);	
					NumObjects.erase(NumObjects.begin() + i);
					i--;
					break;
				}
				else {
					NumObjects[i]->cell += 4;
					break;
				}
			}
		}
		NumObjects[i]->x = cell_coordinate[NumObjects[i]->cell][0];
		NumObjects[i]->y = cell_coordinate[NumObjects[i]->cell][1];
	}
}

void push_down() {	// �����L�[�������ꂽ�ꍇ(push_right�Ɠ��l)
	sort(NumObjects.begin(), NumObjects.end(), y_desc);	// y���W���傫����(��艺�ɂ��鏇)�Ƀ\�[�g

	for (int i = 0; i < NumObjects.size(); i++) {	// �S�Ă̐����I�u�W�F�N�g�ɂ���
		while (NumObjects[i]->cell != 12 && NumObjects[i]->cell != 13 && NumObjects[i]->cell != 14 && NumObjects[i]->cell != 15) {	// �����I�u�W�F�N�g�̈ʒu���ŉ��[�ɂȂ�܂�
			NumObjects[i]->cell += 4;

			bool equal_location = false;
			int index = 0;
			for (int j = i - 1; j >= 0; j--) {
				if (NumObjects[i]->cell == NumObjects[j]->cell) {
					equal_location = true;
					index = j;
					break;
				}
			}

			if (equal_location) {
				if (NumObjects[i]->num == NumObjects[index]->num) {
					change_struct(index);
					NumObjects.erase(NumObjects.begin() + i);
					i--;
					break;
				}
				else {
					NumObjects[i]->cell -= 4;
					break;
				}
			}
		}
		NumObjects[i]->x = cell_coordinate[NumObjects[i]->cell][0];
		NumObjects[i]->y = cell_coordinate[NumObjects[i]->cell][1];
	}
}