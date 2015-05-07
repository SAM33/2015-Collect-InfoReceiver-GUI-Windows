//NTCU TSCC TEAM , 2015.03
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <vector>
#include <cstdio>
#include <sstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "infoReceiver.h"
#include "diagram2.h"
#include "controller.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>	
#include <arpa/inet.h>
#endif
#define MS 1000
// collectl -A 127.0.0.1:4000 -scm -P

controller ctl;

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

/* mutex */
pthread_mutex_t guimutex;
/*  this function will automatic call Draw  */
static void Repaint()
{
	glutPostRedisplay();
}
/*  opengl critical sectiong  */
static void Draw()
{
	glClear(GL_COLOR_BUFFER_BIT);

	pthread_mutex_lock(&guimutex);
	for( int i = 0 ; i < controller::maxDgX * controller::maxDgY ; ++i )
	{
		ctl.dgptr[i]->draw(ctl.usingData[i],controller::maxItem);
	}
	pthread_mutex_unlock(&guimutex);
	glutSwapBuffers();
}

// thread only connect with client and receiver their data
void* run(void* param)
{
	int sockfd, newsockfd, portno;
	#ifdef _WIN32
	WSADATA wsa;
    printf("Initialising Winsock...\n");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        printf("Failed. Error Code : %d\n",WSAGetLastError());
        exit(-1);
    }
    printf("Initialised.\n");
	#endif	
	//custom
	int id = *((int*)param);	
	portno = controller::startPort + id;
	printf("init socket with id:%d at port:%d\n",id,portno);
	#ifdef _WIN32
	int clilen;
	#else
	socklen_t clilen;
	#endif
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket");
	memset((char *) &serv_addr, 0x0,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	clilen = sizeof(cli_addr);
    
#ifdef __APPLE__
    //mac os will auto import some function in c++11 when we use std=c++11 param
    //we use :: operator to tell c++11 , don't use its 'std::bin' to
    //replace our c language 'bind' function
    int r  = ::bind(sockfd, (struct sockaddr *)&serv_addr,sizeof(serv_addr));
    if (r < 0)
        
        error("ERROR on binding");
    
#else
    if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    
#endif
    //bind(sockfd,&serv_addr,sizeof(serv_addr));
	ctl.connect_state[id] = STE_LISTEN;
	listen(sockfd,5);
	newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0)
		error("ERROR on accept");
	int n;

	// get ip address
	ctl.ipAddr[id] = string(inet_ntoa(cli_addr.sin_addr))+":";

	bool isReg = false;
	cout << "port:" << portno << " is connect with " << ctl.ipAddr[id] << endl;
	ctl.connect_state[id] = STE_CONNECT;
	while( 1 )
	{
		memset(buffer,0x0,256);
		n = read(newsockfd,buffer,255);
		if (n < 0) {
			 ctl.connect_state[id] = STE_DISCONNECT;
			 error("ERROR reading from socket");
		}
		string tmp( buffer , buffer+n );

		ctl.rcv[id].updateInfo(tmp);

		if( !isReg )
		{
			isReg = ctl.autoRegister(id);
			ctl.trySetTitle();
		}

		pthread_mutex_lock(&guimutex);
		int comingSize = ctl.rcv[id].sync();
		pthread_mutex_unlock(&guimutex);

		if( comingSize == 0 )
			usleep(200*MS);
		else
			usleep(900*MS);
	}

	close(newsockfd);
	close(sockfd);

	pthread_exit(NULL);
	return NULL;
}

void* key(void* param)
{
	int n;
	int dgID;
	int rcvID;
	int col;
	while(1)
	{
		do {
			puts("<STEP1>choose diagram:");
		} while( 1 != scanf(" %d",&dgID) );
		
		if( dgID >= controller::maxDgX * controller::maxDgY )
		{
			puts("error diagram ID");
			continue;
		}

		do {
			puts("<STEP2>choose node:");
		} while( 1 != scanf(" %d",&rcvID) );
		
		if( rcvID >= controller::nodeNum )
		{
			puts("error node ID");
			continue;
		}

		if( ctl.connect_state[rcvID] != STE_CONNECT )
		{
			puts("not connecting");
			continue;
		}

		do {
			ctl.printColumns(rcvID);
			puts("<STEP3>choose col:");
		} while( 1 != scanf(" %d",&col) );

		if( col >= ctl.rcv[rcvID].getColumns().size() )
		{
			puts("error column ID");
			continue;
		}

		ctl.setData( dgID/controller::maxDgX , dgID%controller::maxDgY , rcvID , col );
		ctl.trySetTitle();
		#ifdef ENABLE_OPTION
		int op;
		bool input=true;
		while(input)
		{
			printf("0 : no more option\n");
			printf("1 : Enable_MemoryFromat\n");
			//collect input is "KB" we want to change it to MB or GB
			printf("2 : Disable_MemoryFromat\n");
			puts("<STEP4>set option:");
			int s = scanf("%d",&op);
			int u;
			int arg_int;
			int one = 1;		
			int to_KB = 1;
			int to_MB = 1000;
			int to_GB = 1000000;
			char Unit[5];	
			switch(op)
			{
				case 0:
				input=false;
				break;

				case 1:
				printf("0 : KB(default)\n");
				printf("1 : MB\n");
				printf("2 : GB\n");
				printf("Unit:\n");
				memset(Unit,0x0,5);
				s = scanf("%d",&u);
				if(u==0)
				{
					ctl.setOption(dgID/controller::maxDgX,dgID%controller::maxDgY,Option_SetGridDiv,(void*)&to_KB);
					strcpy(Unit,"KB");
				}
				else if(u==1)
				{
					ctl.setOption(dgID/controller::maxDgX,dgID%controller::maxDgY,Option_SetGridDiv,(void*)&to_MB);
					strcpy(Unit,"MB");
				}
				else if(u==2)
				{
					ctl.setOption(dgID/controller::maxDgX,dgID%controller::maxDgY,Option_SetGridDiv,(void*)&to_GB);
					strcpy(Unit,"GB");
				}
				else
				{
					;
				}
				ctl.setOption(dgID/controller::maxDgX,dgID%controller::maxDgY,Option_SetGridUnit,(void*)Unit);
				ctl.setOption(dgID/controller::maxDgX,dgID%controller::maxDgY,Option_EnableGridDiv,(void*)&one);
				ctl.setOption(dgID/controller::maxDgX,dgID%controller::maxDgY,Option_EnableGridUnit,(void*)&one);
				break;

				case 2:
				ctl.setOption(dgID/controller::maxDgX,dgID%controller::maxDgY,Option_DisableGridDiv,(void*)&one);
				ctl.setOption(dgID/controller::maxDgX,dgID%controller::maxDgY,Option_DisableGridUnit,(void*)&one);
				break;

				default:
				break;

			}
		}
		#endif
	}
	pthread_exit(NULL);
	return NULL;
}

/*  main function  */
int main(int argc, char** argv)
{
	const int w = 400;
	const int h = 150;
	const int xB = 80;
	const int yB = 80;
	const int xN = controller::maxDgX;
	const int yN = controller::maxDgY;
	const int xT = 10 + xN*(xB+w);
	const int yT = 10 + yN*(yB+h);

	pthread_t datareceiver[controller::nodeNum+1];
	int param[controller::nodeNum];
	pthread_mutex_init(&guimutex, NULL);

	for( int x = 0 ; x < xN ; ++x )
		for( int y = 0 ; y < yN ; ++y )
			ctl.initDg( 40 + x*(xB+w) , 40 + y*(yB+h) ,w,h,x,y); 

	pthread_create(&datareceiver[0], NULL, key, NULL);
	for( int i = 0 ; i < controller::nodeNum ; ++i )
	{
		param[i] = i;
		pthread_create(&datareceiver[i+1], NULL, run, (void *)&param[i]);
	}

	/*  init opengl (glu,glut...)  */
	glutInit(&argc, argv);
	glutInitWindowPosition(10,10);
	glutInitWindowSize(xT,yT);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);
	glutCreateWindow("TSCC2015");
	glClearColor(0.0,0.0,0.0,0.0);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0,xT,0,yT);
	glutIdleFunc(Repaint);
	glutDisplayFunc(Draw);
	glutMainLoop(); //nerver return
	pthread_mutex_destroy(&guimutex);
	return 0;
}

