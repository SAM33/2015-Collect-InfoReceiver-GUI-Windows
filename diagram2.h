//NTCU TSCC TEAM , 2015.03
#ifndef DIAGRAM2_H
#define DIAGRAM2_H

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include <deque>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string.h>
#include <iostream>
#include <algorithm>
using namespace std;
/* 繪圖相關集成 */
#define TEXT_SMALL  0
#define TEXT_MIDDLE 1
#define TEXT_LARGE  2

#define ENABLE_OPTION


#ifdef ENABLE_OPTION
#define Option_SetGridDiv 0x01
#define Option_SetGridUnit 0x02
#define Option_EnableGridDiv 0x03
#define Option_DisableGridDiv 0x04
#define Option_EnableGridUnit 0x05
#define Option_DisableGridUnit 0x06

#define GridDiv_G 1000000000
#define GridDiv_M 1000000
#define GridDiv_K 1000


#endif

class diagram2
{
 private:
	int x;
	int y;
	string *title;
	int width;
	int height;
	float maxvalue;   //y軸的最大值
	float maxitem;
	/* 繪圖相關集成 */
	void drawline(float X1, float Y1, float X2, float Y2);
	void drawtext(float X, float y,string, int size);
	int drawGrid();
#ifdef ENABLE_OPTION
	int GridDiv;
	char GridUnit[5];
	bool Enable_GridDiv;
	bool Enable_GridUnit;
#endif


public:
	int port;  //無相關,但方便pthread傳參數使用,特別加過來
	diagram2(int _x,int _y,int _width,int _height);  //建構子
	void settitle(string *_title);                 //設定標題
	void draw(deque<float>*,int);  //繪圖區,請使用OpenGL主迴圈來調用
#ifdef ENABLE_OPTION
	void option(int optioncode,void * value);
	void initoption();
#endif

};

#endif
