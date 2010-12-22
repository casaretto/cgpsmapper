#ifndef __MP_IMG_CREATE
#define __MP_IMG_CREATE

#include <string>
#include "typeTranslation.h"
#include ".\..\sqlite-source\sqlite3.h"
#include "mp_types.h"

/*

funkcje?
	zebranie wszystkich lbl

*/

class LBLCreate {
private:
	sqlite3		*MPbase;
public:
	void	setDB(sqlite3* _MPbase) {MPbase = _MPbase;};

	//
	void	createLBLstrings(int map_id);

	//dzielenie obiektow
	// - dzielenie linii


	// - dzielenie polygonow
};

#endif