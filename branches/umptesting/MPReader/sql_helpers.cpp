#include "sql_helpers.h"

int	getSelectCount(sqlite3* _MPbase,const char* s) {
    sqlite3_stmt *ppStmt;
	int ret_val = -1;

	int	rc = sqlite3_prepare_v2(_MPbase,s,-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	if( rc == SQLITE_ROW ) 
		ret_val = sqlite3_column_int(ppStmt,0);
	sqlite3_finalize( ppStmt );
	return ret_val;
}

__int64	getSelectCount64(sqlite3* _MPbase,const char* s) {
    sqlite3_stmt *ppStmt;
	__int64 ret_val = -1;

	int	rc = sqlite3_prepare_v2(_MPbase,s,-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	if( rc == SQLITE_ROW ) 
		ret_val = sqlite3_column_int64(ppStmt,0);
	sqlite3_finalize( ppStmt );
	return ret_val;
}

bool getSelectStringLong(sqlite3* _MPbase,const char* s,std::string &s1,int& value) {
    sqlite3_stmt *ppStmt;
	bool ret_val = false;

	s1 = "";
	value = -1;

	int	rc = sqlite3_prepare_v2(_MPbase,s,-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	if( rc == SQLITE_ROW ) {
		s1 = (sqlite3_column_text(ppStmt,0)?(char*)sqlite3_column_text(ppStmt,0):"");
		value = sqlite3_column_int(ppStmt,1);
		ret_val = true;
	}
	sqlite3_finalize( ppStmt );
	return ret_val;
}

bool getSelectString(sqlite3* _MPbase,const char* s,wxString &s1) {
    sqlite3_stmt *ppStmt;
	bool ret_val = false;

	s1.clear();

	int	rc = sqlite3_prepare_v2(_MPbase,s,-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	if( rc == SQLITE_ROW ) {
		s1 = wxString::FromUTF8((sqlite3_column_text(ppStmt,0)?(char*)sqlite3_column_text(ppStmt,0):""));
		ret_val = true;
	}
	sqlite3_finalize( ppStmt );
	return ret_val;
}

bool getSelectString(sqlite3* _MPbase,const char* s,std::string &s1) {
    sqlite3_stmt *ppStmt;
	bool ret_val = false;

	s1 = "";

	int	rc = sqlite3_prepare_v2(_MPbase,s,-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	if( rc == SQLITE_ROW ) {
		s1 = (sqlite3_column_text(ppStmt,0)?(char*)sqlite3_column_text(ppStmt,0):"");
		ret_val = true;
	}
	sqlite3_finalize( ppStmt );
	return ret_val;
}

bool getSelectString3(sqlite3* _MPbase,const char* s,std::string &s1,std::string &s2,std::string &s3) {
    sqlite3_stmt *ppStmt;
	bool ret_val = false;

	s1 = "";
	s2 = "";
	s3 = "";

	int	rc = sqlite3_prepare_v2(_MPbase,s,-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	if( rc == SQLITE_ROW ) {
		s1 = (sqlite3_column_text(ppStmt,0)?(char*)sqlite3_column_text(ppStmt,0):"");
		s2 = (sqlite3_column_text(ppStmt,1)?(char*)sqlite3_column_text(ppStmt,1):"");
		s3 = (sqlite3_column_text(ppStmt,2)?(char*)sqlite3_column_text(ppStmt,2):"");
		ret_val = true;
	}
	sqlite3_finalize( ppStmt );
	return ret_val;
}
