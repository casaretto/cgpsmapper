#ifndef __MP_POLYGONS
#define __MP_POLYGONS

#include "point.h"
#include "mp_process.h"
#include ".\..\sqlite-source\sqlite3.h"


class PolyProcess {
private:
	sqlite3		*MPbase;
public:
	void	setDB(sqlite3* _MPbase) { MPbase = _MPbase;};

	void	detectMasterAndHoles(MPPoly* poly);

//			DetectMasterAndHoles(list_ImportRGN[level_import]);
//			PolygonProcess(rgnType,list_ImportRGN[level_import],level_import,true,verbose);

};



#endif