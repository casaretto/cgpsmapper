#ifndef __MP_EXPORT
#define __MP_EXPORT

#include <string>
#include <iostream>
#include <fstream>
#include ".\..\sqlite-source\sqlite3.h"
#include "mp_types.h"

class MPExport {
protected:
	sqlite3		*MPbase;

	bool	exportLines(const char* _fileName);

public:
	bool	exportSHP(sqlite3* _MPbase,_mp_section section,const char* _fileName);

	bool	exportMP(sqlite3* _MPbase,const char* _fileName);
	bool	exportRestrictionsMP(sqlite3* _MPbase,const char* _fileName);
};

#endif