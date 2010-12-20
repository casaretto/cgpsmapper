#ifndef __MP_INFO
#define __MP_INFO

#include "wx/wx.h"
#include ".\..\sqlite-source\sqlite3.h"

class MPInfo {
private:
	sqlite3		*MPbase;

	int			rcExecQuery;
	bool		_hasNextLine;
	sqlite3_stmt*	ppStmtExec;

public:
	void	setDB(sqlite3* _MPbase) { MPbase = _MPbase; };

	wxString	getDBinfo_IMG();
	wxString	getName();
	wxString	getID();
	wxString	getPoints();
	wxString	getLines();
	wxString	getPolygons();

	wxString	execQuery(const char* s);
	bool		hasNextLine() { return _hasNextLine; };
	wxString	getNextLine();
};

#endif
