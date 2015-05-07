#include "diagram2.h"
//#define ENABLE_DEBUG
#ifdef __APPLE__
#include <string.h>
#include <stdio.h>
inline void glutBitmapString(void *font , const unsigned char *chars)
{
    int l = strlen((char*)chars);
    for(int i=0 ; i<l ; i++)
    {
        glutBitmapCharacter(font,chars[i]);
    }
    
    
}
#endif



void diagram2 :: drawtext(float x, float y,string text, int size)
{
	glRasterPos2f(x, y);
	switch(size)
	{
		case TEXT_SMALL:
			glutBitmapString(GLUT_BITMAP_HELVETICA_10, (const unsigned char *)text.c_str());
			break;

		case TEXT_MIDDLE:
			glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char *)text.c_str());
			break;

		case TEXT_LARGE:
			glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char *)text.c_str());
			break;

		default:
			glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char *)text.c_str());
	}
}

void diagram2 :: drawline(float X1, float Y1, float X2, float Y2)
{
	glBegin(GL_LINES);
	glVertex2d(X1, Y1);
	glVertex2d(X2, Y2);
	glEnd();
}

diagram2 :: diagram2(int _x,int _y,int _width,int _height)
{
	x = _x;
	y = _y;
	width = _width;
	height = _height;
	maxvalue = 100;
	maxitem = 10;
	title = NULL;
#ifdef ENABLE_OPTION
	initoption();
#endif
}

void diagram2 :: settitle(string *_title)
{
	title = _title;
}

void diagram2 :: draw(deque<float> *dequeptr , int _maxitem)
{
	maxitem = _maxitem;
	glColor3f(0,1,0);
	if( title!=NULL )drawtext(x,y+height+20,*title,TEXT_MIDDLE);
	drawGrid();
	/* 畫出資料 */
	if(dequeptr==NULL)
	{
		//cout<<"no data to draw"<<endl;
		return;
	}

	glColor3f(1,0,0); //R G B
	int size = dequeptr->size();
	if(size<2) //無資料時,或是資料不足連成一條線時
	{
		//cout<<"too few data to draw"<<endl;
		return;
	}

	if(size<=maxitem && size>=2) //一般情況
	{

		maxvalue = *std::max_element( dequeptr->begin() , dequeptr->end() );
		float X1,X2,Y1,Y2;
		float Value1,Value2;
		for(int index=0 ; index<size-1 ; index++)
		{
			Value1=dequeptr->at(index);
			Value2=dequeptr->at(index+1);
			/* 第一個繪圖點 */
			float X1 = x+(float)((float)index/(float)maxitem)*(float)width;
			float Y1 = y+(float)((float)Value1/(float)maxvalue)*(float)height;
			/* 第二個繪圖點 */
			float X2 = x+(float)((float)(index+1)/(float)maxitem)*(float)width;
			float Y2 = y+(float)((float)Value2/(float)maxvalue)*(float)height;
			/* 畫出相連的線 */
			drawline(X1,Y1,X2,Y2);
		}
	}
	else	//發生錯誤,註冊近來的deque和x軸的顯示框對不上
	{
		cout<<"Error on diagram2::draw"<<endl;
		return;
	}
}



int diagram2 :: drawGrid()
{
	char temp[20];
	/* 畫出X軸 */
	glColor3f(1,1,1); //R G B
	drawline(x,y,x+width,y);
	for(int i=1 ; i<=maxitem ; i++)
	{
		if(maxitem>=20)
		{
			if(i%((int)maxitem/20)==0)
			{
				float X = x+(float)((float)i/(float)maxitem)*(float)width;
				drawline(X,y+5,X,y-5);
				memset(temp,0x0,20);
				sprintf(temp,"%d",i);
				drawtext(X-5,y-20,string(temp),TEXT_SMALL);
			}
		}
		else
		{
			float X = x+(float)((float)i/(float)maxitem)*(float)width;
			drawline(X,y+5,X,y-5);
			memset(temp,0x0,20);
			sprintf(temp,"%d",i);
			drawtext(X-5,y-20,string(temp),TEXT_SMALL);
		}
	}

	/* 畫出Y軸 */
	glColor3f(1,1,1); //R G B
	drawline(x,y,x,y+height);
	for(int i=1 ; i<=maxvalue ; i++)
	{
		if(maxvalue>10)
		{
			if(i%((int)maxvalue/10)==0)
			{
				float Y = y+(float)((float)i/(float)maxvalue)*(float)height;
				drawline(x+5,Y,x-5,Y);
				memset(temp,0x0,20);
				double value = (double)i;
				#ifdef ENABLE_OPTION
				if(Enable_GridDiv)
				{
					value/=GridDiv;
					sprintf(temp+strlen(temp),"%.2lf",value);
				}
				if(Enable_GridUnit)
				{
					sprintf(temp+strlen(temp),"%s",GridUnit);
				}
				else
				{
					sprintf(temp+strlen(temp),"%d",(int)value);
				}
				drawtext(x-20,Y-5,string(temp),TEXT_SMALL);
				#else
				sprintf(temp,"%d",i);
				drawtext(x-20,Y-5,string(temp),TEXT_SMALL);
				#endif
			}
		}
		else
		{
			float Y = y+(float)((float)i/(float)maxvalue)*(float)height;
			drawline(x+5,Y,x-5,Y);
			memset(temp,0x0,20);
			sprintf(temp,"%d",i);
			drawtext(x-20,Y-5,string(temp),TEXT_SMALL);
		}
	}

	return 0;
}


#ifdef ENABLE_OPTION
void diagram2 :: option(int optioncode,void * value)
{
	switch(optioncode)
	{
		case Option_SetGridDiv:
		GridDiv = *(int*)value;
		printf("GridDiv set to %d\n",GridDiv);
		break;

		case Option_SetGridUnit:
		strcpy(GridUnit,(char*)value);
		printf("GridUnit set to %s\n",GridUnit);
		break;

		case Option_EnableGridDiv:
		Enable_GridDiv = true;
		printf("Enable GridDiv\n");
		break;

		case Option_DisableGridDiv:
		Enable_GridDiv = false;
		printf("Disable GridDiv\n");
		break;

		case Option_EnableGridUnit:
		Enable_GridUnit = true;
		printf("Enable GridUnit\n");
		break;

		case Option_DisableGridUnit:
		Enable_GridUnit = false;
		printf("Disable GridUnit\n");
		break;
	}	
}


void diagram2 :: initoption()
{
	GridDiv = 0;
	memset(GridUnit,0x0,5);
	Enable_GridDiv = false;
	Enable_GridUnit = false;
}

#endif

