#include "mp_info.h"
#include "sql_helpers.h"

wxString MPInfo::execQuery(const char* s) {
	wxString	t_ret;
	int rc = sqlite3_prepare_v2(MPbase,s,-1,&ppStmtExec,NULL);
	rcExecQuery = sqlite3_step( ppStmtExec );
	if( rcExecQuery == SQLITE_ROW ) {
		_hasNextLine = true;

		t_ret.Append(_T("<TR>"));

		for( int i = 0; i < sqlite3_column_count(ppStmtExec); i++ ) {
			t_ret.Append(_T("<TH>"));
			t_ret.Append(wxString::FromUTF8(sqlite3_column_name(ppStmtExec,i)));
			t_ret.Append(_T("</TH>"));
		}
		t_ret.Append(_T("</TR>"));
	} else {
		_hasNextLine = false;
		t_ret.Append( wxString::FromUTF8( sqlite3_errmsg(MPbase) ) );
	}
	return t_ret;
}

wxString	MPInfo::getNextLine() {
	wxString	t_ret;
	if( rcExecQuery == SQLITE_ROW ) {
		_hasNextLine = true;
		t_ret.Append(_T("<TR>"));

		for( int i = 0; i < sqlite3_column_count(ppStmtExec); i++ ) {
			t_ret.Append(_T("<TD>"));
			t_ret.Append(wxString::FromUTF8( (char*)sqlite3_column_text(ppStmtExec,i)));
			t_ret.Append(_T("</TD>"));
		}
		t_ret.Append(_T("</TR>"));
		rcExecQuery = sqlite3_step( ppStmtExec );
	} else {
		_hasNextLine = false;
		sqlite3_finalize( ppStmtExec );
	}
	return t_ret;
}

wxString	MPInfo::getDBinfo_IMG() {
	sqlite3_stmt*	ppStmt;
	wxString		t_ret;
	int rc = sqlite3_prepare_v2(MPbase,"SELECT name, id FROM MP_ID;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {
			t_ret.Append(_T("Map ID: "));
			t_ret.Append(wxString::FromUTF8((char*)sqlite3_column_text(ppStmt,1)));
			t_ret.Append(_T("; Map name: "));
			t_ret.Append(wxString::FromUTF8((char*)sqlite3_column_text(ppStmt,0)));
			t_ret.Append(_T("\n"));
			rc = sqlite3_step( ppStmt );
		}
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );
	return t_ret;

}

wxString	MPInfo::getName() {
	wxString t_ret;
	getSelectString(MPbase,"SELECT name FROM MP_ID;",t_ret);
	return t_ret;
}

wxString	MPInfo::getID(){
	wxString t_ret;
	getSelectString(MPbase,"SELECT id FROM MP_ID;",t_ret);
	return t_ret;
}

wxString	MPInfo::getPoints(){
	wxString t_ret;
	getSelectString(MPbase,"SELECT count(*) FROM MP_POI;",t_ret);
	return t_ret;
}

wxString	MPInfo::getLines(){
	wxString t_ret;
	getSelectString(MPbase,"SELECT count(*) FROM MP_LINE;",t_ret);
	return t_ret;
}

wxString	MPInfo::getPolygons(){
	wxString t_ret;
	getSelectString(MPbase,"SELECT count(*) FROM MP_POLY;",t_ret);
	return t_ret;
}
