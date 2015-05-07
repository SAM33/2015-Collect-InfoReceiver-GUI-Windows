#include "infoReceiver.h"
#ifdef __MINGW32__
#include <stdlib.h>  //atof
#endif
infoReceiver::infoReceiver()
{
	hasHeader = false;	
}

int infoReceiver::updateInfo( string str )
{
	if( str.empty() ) return 1;

	buffer += str;//將字串與上次切剩下的字串合併

	while(1)
	{
		try { 
			string res = tryCutString(buffer, '\n');  //把buffer中'\n'之前的字串切下來,且從buffer中除去
			lines.push_back(res);  //把切好的一行字串塞入lines deque
		} catch ( ... ) { break; }
	}

	if( !hasHeader && tryResolveHeader() == OK )
		hasHeader = true;
	else
		tryResolveInfo();

	return 1;
}

void infoReceiver :: registerItem( deque<float> *que ,int col ,int max )
{
	maxSize[col] = max;
	monitorVec[col] = que;
}

int infoReceiver :: sync()
{
	int res=0;
	for( auto it : monitorVec )
	{
		int col = it.first;
		res += tempData[col].size();
		//it.second->insert( it.second->begin()+it.second->size() , tempData[col].begin() , tempData[col].end() );
		for( size_t i = 0 ; i < tempData[col].size() ; ++i )
		{
			float val;
			#ifdef __MINGW32__
			try { val = atof(tempData[col][i].c_str()); }
			catch ( ... ) {		
				val = 0.0;
				return 0;
			}
			#else
			try { val = stof(tempData[col][i]); }
			catch ( ... ) {		
				val = 0.0;
				return 0;
			}
			
			#endif
			it.second->push_back(val);
		}

		tempData[col].clear();
		while( it.second->size() > maxSize[col] )
			it.second->pop_front();
	}
	
	return res;
}

vector<string> infoReceiver::getColumns()
{
	return columnName;
}

void infoReceiver::printColumns()
{
	vector<string> col = getColumns();
	for( size_t i = 0 ; i < col.size() ; ++i )
		cout << i << ":" << col[i] << "\n";
	cout << endl;
}


int infoReceiver::tryResolveHeader()
{
	if( lines.empty() ) return ERR;

	string info = retrieveOneLineFromQueue();

	host = "";
	try { 
		host = tryCutString(info, '#');
	} catch ( ... ) { return ERR; }

	stringstream ss(info);
	columnName.clear();
	columnName.push_back(host);
	string tmp;
	while( ss >> tmp )
		columnName.push_back(tmp);
	
	if( columnName.empty() )
		throw runtime_error("get hostname but not have any column.");

	return OK;
}

int infoReceiver::tryResolveInfo()
{
	while( !lines.empty() )
	{
		string line = retrieveOneLineFromQueue();
		if( ERR == parseOneLine(line) )
		{
			puts("warning: illegal line.");	
		}
	}

	return OK;
}

int infoReceiver::parseOneLine( string line )
{
	stringstream ss(line);
	vector<string> tmp;
	string s;
	while( ss >> s )	
		tmp.push_back(s);

	if( tmp.size() != columnName.size() )
		return ERR;
	
	for( auto it : monitorVec )
	{
		int col = it.first;
		tempData[col].push_back( tmp[col] );
	}

	return OK;
}

string infoReceiver::retrieveOneLineFromQueue()
{
	if( lines.empty() )
		throw runtime_error("queue is empty.");
	string info = lines.front();
	lines.pop_front();

	return info;
}

string infoReceiver::tryCutString(string& str, char delim) throw(int)
{
	size_t pos = str.find(delim);	
	if( pos == string::npos )
		throw NOT_FOUND;

	string ret = str.substr( 0, pos );
	str.erase( 0, pos+1 );

	return ret;
}



string infoReceiver::getcolumnName(int column)
{
	return columnName.at(column);
}
