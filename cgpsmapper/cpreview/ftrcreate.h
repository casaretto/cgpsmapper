/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

#ifndef _FTRCREATE_H
#define _FTRCREATE_H

#include <iostream>
#include <map>
#include <vector>
#include <set>
#include <string>
#include <algorithm>
#include "../sqlite-source/sqlite3.h"

//- pozniej do "odkomentowania"
#define _TEMP_REMOVE 0

//#define _MAPREAD_DLL
#include "./../mapRead/mapReadDLL.h"
#include "filexor.h"

bool	initMDR(string base_name);
bool	createTables();
bool	processIMG(std::string fileName,bool idx_ignore);
int		getCodePage();
void	setMultibody(bool multibody);
sqlite3	*getMDRbase();
bool	doneMDR(bool erase);

#endif
