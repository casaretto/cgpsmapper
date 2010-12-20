#ifndef _SQL_HELPERS
#define _SQL_HELPERS

#include <string>
#include "wx\wx.h"
#include ".\..\sqlite-source\sqlite3.h"

int	getSelectCount(sqlite3* _MPbase,const char* s);
__int64	getSelectCount64(sqlite3* _MPbase,const char* s);
bool getSelectString3(sqlite3* _MPbase,const char* s,std::string &s1,std::string &s2,std::string &s3);
bool getSelectString(sqlite3* _MPbase,const char* s,std::string &s1);
bool getSelectString(sqlite3* _MPbase,const char* s,wxString &s1);
bool getSelectStringLong(sqlite3* _MPbase,const char* s,std::string &s1,int& value);

#endif