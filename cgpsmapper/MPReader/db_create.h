#ifndef __MP_DB_CREATE
#define __MP_DB_CREATE

#include <string>
#include "typeTranslation.h"
#include ".\..\sqlite-source\sqlite3.h"
#include "mp_types.h"
#include "mp_process.h"
#include "wx\wx.h"
#include "wx\tokenzr.h"


class DBCreate {
private:
	sqlite3		*MPbase;
	bool		_constructor_error;
public:
	DBCreate::DBCreate(const char* _databaseName);
	~DBCreate();

	bool	constructorError() { return _constructor_error; };

	//utworzenie czesci bazy odpowiadajacej za przechowywanie danych wejsciowych
	void	createInputDB();

	//baza musi juz istniec, tworzy tabele konieczne do przetwarzania do IMG
	void	createProcessDB();

	sqlite3* getDB();
};

#endif