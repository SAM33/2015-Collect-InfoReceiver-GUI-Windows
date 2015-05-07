#ifndef CON_H
#define CON_H

#define DgSize maxDgX*maxDgY
#define conv(x,y) x*maxDgX+y

#include <fstream>
#include "infoReceiver.h"
#include "diagram2.h"

using namespace std;

#define STE_NOT_START 0
#define STE_LISTEN 1
#define STE_CONNECT 2
#define STE_DISCONNECT 3

class controller {
	public:
		static const int maxDgX = 4;
		static const int maxDgY = 4;
		static const int nodeNum = 4;
		static const int startPort = 4000;
		static const int maxItem = 100;

		diagram2 *dgptr[DgSize];	
		string title[DgSize];
		int usingCol[DgSize];
		int usingID[DgSize];
		deque<float> *usingData[DgSize];
		
		infoReceiver rcv[nodeNum];
		string ipAddr[nodeNum];
		int connect_state[nodeNum];
	
		map<int,deque<float> > data[nodeNum];
		controller();
		void initDg(int,int,int,int,int,int);
		void setData(int,int,int,int);

		#ifdef ENABLE_OPTION
		void setOption(int,int,int,void *);
		#endif

		void printColumns(int);

		// call it to set title
		void trySetTitle();

		bool autoRegister(int);
	private:
		map<string,string> trans;
};

#endif
