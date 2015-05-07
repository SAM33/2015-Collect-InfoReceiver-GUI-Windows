#include "controller.h"

controller :: controller()
{
	for( int i = 0 ; i < DgSize ; ++i )
	{
		char chs[100] = {};
		sprintf(chs , "[%d] Not Set" , i );
		string tmp(chs);
		title[i] = tmp;
		usingCol[i] = -1;
		usingID[i] = -1;
		usingData[i] = NULL;
	}

	for( int i = 0 ; i < nodeNum ; ++i )
		connect_state[i] = STE_NOT_START;

	fstream fin( "trans.txt" , ios::in );

	string k;
	while(fin>>k)
		getline(fin,trans[k]);
}

void controller :: initDg(int boundx,int boundy,int width,int height,int x,int y)
{
	if( !( 0 <= x && x < maxDgX && 0 <= y && maxDgY ) )
	{
		puts("init diagram error at x,y pos");
		return ;
	}
	
	dgptr[conv(x,y)] = new diagram2(boundx,boundy,width,height); //x=50 y=50 width=400 height=150
	dgptr[conv(x,y)]->settitle( &title[conv(x,y)] );
		
}

void controller :: setData(int x,int y,int id,int col)
{
	if( col == 0 )
	{
		cout << "error:col 0 is machine name" << endl;
		return;
	}
	int index = conv(x,y);
	usingData[index] = &(data[id][col]);
	usingCol[index] = col;
	usingID[index] = id;
	char chs[100] = {};
	sprintf( chs , "[%d] Set to ID:%d\tcol:%d\n" ,index , id , col );
	string tmp(chs);
	title[index] = tmp;
}

#ifdef ENABLE_OPTION
void controller :: setOption(int x,int y,int optioncode,void * value)
{
	int index = conv(x,y);
	dgptr[conv(x,y)]->option(optioncode,value);
}
#endif

void controller :: printColumns(int id)
{
	auto cols = rcv[id].getColumns();
	for( size_t i = 0 ; i < cols.size() ; ++i )
	{
		auto iter = trans.find(cols[i]);
		if( iter == trans.end() )
			cout << i << ":" << cols[i] << endl;
		else
			cout << i << ":" << (iter->second) << endl;
	}
	cout << endl;
}

void controller :: trySetTitle()
{
	for( int i = 0 ; i < DgSize ; ++i )
	{
		if( usingCol[i] == -1 || usingID[i] == -1 )
			continue;

		auto cols = rcv[usingID[i]].getColumns();
		char chs[300] = {};
		if( usingCol[i] < cols.size() )
			sprintf( chs , "[%d] %s%d %s" ,i , ipAddr[usingID[i]].c_str() , startPort+usingID[i] , cols[usingCol[i]].c_str() );
		else
			sprintf( chs , "[%d] %s%d NO DATA" ,i , ipAddr[usingID[i]].c_str() , startPort+i );
		string tmp(chs);
		title[i] = tmp; 
	}
}

bool controller :: autoRegister(int id) 
{
	auto cols = rcv[id].getColumns();
	bool res = false;
	for( int i = 1 ; i < cols.size() ; ++i )
	{
		res = true;
		rcv[id].registerItem( &data[id][i] , i , 100 );	
	}

	return res;
}
