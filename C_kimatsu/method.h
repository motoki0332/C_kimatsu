#pragma once
#define CHARBUFF 124
#define BUFFSIZE 1024

struct NumObject {
	int num;
	int cell;
	int x;
	int y;
	NumObject* nextNumObject;
};

bool x_asc(const struct NumObject*, const struct NumObject*);
bool x_desc(const struct NumObject*, const struct NumObject*);
bool y_asc(const struct NumObject*, const struct NumObject*);
bool y_desc(const struct NumObject*, const struct NumObject*);

void getGurrentDirectory(char*);
char* getini(char*, char*);
void CSVArray(const char*);
void write_file(bool, int);

NumObject* set_struct(NumObject*);
void create_struct(NumObject*);
void change_struct(int);

int find_empty_cellnum();
void draw_field();
void draw_NumObject(NumObject*);
void push_right();
void push_left();
void push_up();
void push_down();