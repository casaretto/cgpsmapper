/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

#include "mapReadDLL.h"
#include "img.h"

map_read::IMG	*mapRead_IMG = NULL;
char	_buffer[256];
#ifdef _WIN32
typedef bool (__stdcall *InstallKeyFn)(const char *name, const char *code);
typedef bool (__stdcall *VerifyKeyFn)(const char *name, const char *code);  
#endif

long CALL_API mapRead_lastError() {
	if( mapRead_IMG == NULL ) {
		return mapRead_IMG->getLastError();
	}
	return _MR_NOT_INITIALISED;
}

void CALL_API mapRead_labelStyle(int style){
	if( mapRead_IMG != NULL ) {
		mapRead_IMG->setLableStyle(style);
	}
}

int	CALL_API mapRead_getLabelCoding(int map_index,int sub_map) {
	if( mapRead_IMG ) {
		if( mapRead_IMG->selectMap(map_index) == false )
			return 0;
		return mapRead_IMG->getLabelCoding(sub_map);
	}
	return 0;
}

int	CALL_API mapRead_getCodePage(int map_index,int sub_map) {
	if( mapRead_IMG ) {
		if( mapRead_IMG->selectMap(map_index) == false )
			return 0;
		return mapRead_IMG->getCodePage(sub_map);
	}
	return 0;
}

long CALL_API mapRead_openIMG(const char *file_name,int& sub_maps,_HEADERDATA _headerFunction,_READDATA _dataFunction,_READDATA_INTEGER _dataFunctionInteger,_READPOI _poiFunction,_READNET _netFunction) {
	
	long	t_ret = _MR_CANNOT_INITIALISE;

	if( mapRead_IMG == NULL ) {
		mapRead_IMG = new map_read::IMG(_headerFunction,_dataFunction,_dataFunctionInteger,_poiFunction,_netFunction);
	}

	t_ret = mapRead_IMG->openMap(file_name,sub_maps);

	if( t_ret >= 0 )
		return _MR_OK;

	return _MR_CANNOT_INITIALISE;
}

void CALL_API mapRead_setInternal(int map_index,_READPOI_INTERNAL _poiInternal,_READNET_INTERNAL _netInternal){
	if( mapRead_IMG ) {
		if( mapRead_IMG->selectMap(map_index) == false )
			return;
		mapRead_IMG->setInternal(_poiInternal,_netInternal);
	}
}

bool CALL_API mapRead_mapValid(int map_index) {
	if( mapRead_IMG != NULL ) {
		return mapRead_IMG->isMapValid(map_index);
	} else
		return false;
}

void CALL_API mapRead_closeAll() {
	if( mapRead_IMG != NULL ) {
		mapRead_IMG->closeAllMaps();
		delete mapRead_IMG;
		mapRead_IMG = NULL;
	}
}

/**
 Close previously open file
*/
void CALL_API mapRead_closeIMG(int map_index) {
	if( mapRead_IMG != NULL ) {
		if( mapRead_IMG->closeMap(map_index) == true ) {
			//all are closed!
			delete mapRead_IMG;
			mapRead_IMG = NULL;
		}
	}
}

void CALL_API mapRead_readMap_details(int map_index,int sub_map,float& x0,float& y0,float& x1,float& y1,float& min_ns,float& min_we,int details) {
	mapRead_rect bounds;
	mapRead_coord min_size;

	min_size.ns = min_ns;
	min_size.we = min_we;

	bounds.x0 = x0;
	bounds.x1 = x1;
	bounds.y0 = y0;
	bounds.y1 = y1;

	if( mapRead_IMG ) {
		if( mapRead_IMG->selectMap(map_index) == false )
			return;

		mapRead_IMG->readMap_details(sub_map,bounds,min_size,details);
	}
}

/**
 Read desired part of the map
*/
void CALL_API mapRead_readMap(int map_index,int sub_map,float& x0,float& y0,float& x1,float& y1,float& min_ns,float& min_we,int layer) {
	mapRead_rect bounds;
	mapRead_coord min_size;

	min_size.ns = min_ns;
	min_size.we = min_we;

	bounds.x0 = x0;
	bounds.x1 = x1;
	bounds.y0 = y0;
	bounds.y1 = y1;

	if( mapRead_IMG ) {
		if( mapRead_IMG->selectMap(map_index) == false )
			return;
		mapRead_IMG->readMap(sub_map,bounds,min_size,layer);
	}
}

int CALL_API mapRead_readMap_road(int map_index,int road_idx) {
	if( mapRead_IMG ) {
		if( mapRead_IMG->selectMap(map_index) == false )
			return 0;
		if( mapRead_IMG->readMap_road(road_idx) )
			return 1;
	}
	return 0;
}

/**
 Read of POI data
 @param	type		type of the object
 @param	sub_type	sub type of the object
 @param	poi_address	address of POI structure
*/
void CALL_API mapRead_readPOI(int map_index,const unsigned int& type,const unsigned int& sub_type,const unsigned long& poi_address, const unsigned long& poi_index) {
#if FREE == 1
	return;
#else
	if( mapRead_IMG ) {
		if( mapRead_IMG->selectMap(map_index) == false )
			return;
		mapRead_IMG->readPOI(type,sub_type,poi_address,poi_index);
	}
#endif
}

void CALL_API mapRead_readNET(int map_index,const unsigned long& net_address) {
#if FREE == 1
	return;
#else
	if( mapRead_IMG ) {
		if( mapRead_IMG->selectMap(map_index) == false )
			return;
		mapRead_IMG->readNET(net_address);
	}
#endif
}

long CALL_API mapRead_readNETroads(int map_index,const unsigned long& net_address,int layer,int table_road[],int table_road_size) {
#if FREE == 1
	return 0;
#else
	if( mapRead_IMG ) {
		if( mapRead_IMG->selectMap(map_index) == false )
			return 0;
		return mapRead_IMG->readNETroads(layer,net_address,table_road,table_road_size);
	}
	return 0;
#endif
}

void CALL_API mapRead_readCities(int map_index,_READINDEX indexFunction) {
	if( mapRead_IMG ) {
		if( mapRead_IMG->selectMap(map_index) == false )
			return;
		mapRead_IMG->readIndex(indexFunction);
	}
}

void CALL_API mapRead_readTRE(unsigned char& rgn,unsigned short& tre) {
	if( mapRead_IMG ) {
		mapRead_IMG->readTRE(rgn,tre);
	}
}
