/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

#include <string>
//#include "lbl.h"
#include "img.h"
#include "img_internal.h"
//#include "rgn.h"
#include "Globals.h"

using namespace map_read;

IMG::IMG(_HEADERDATA _headerFunction,_READDATA _dataFunction,_READDATA_INTEGER _dataFunctionInteger,_READPOI _poiFunction,_READNET _netFunction) {
	labelMPlike = true;
	headerFunction = _headerFunction;
	dataFunction = _dataFunction;
	dataFunctionInteger = _dataFunctionInteger;
	poiFunction = _poiFunction;
	netFunction = _netFunction;
	poiFunction_i = NULL;
	netFunction_i = NULL;
}

void IMG::setInternal(_READPOI_INTERNAL _poiInternal,_READNET_INTERNAL _netInternal) {
	poiFunction_i = _poiInternal;
	netFunction_i = _netInternal;
	(*selected_file)->setInternal(poiFunction_i,netFunction_i);
}

long IMG::openMap(const char *file_name,int& sub_maps) {
	long				t_ret;
	IMG_internal_list*	new_map = new IMG_internal_list(headerFunction,dataFunction,dataFunctionInteger,poiFunction,netFunction);

	sub_maps = 0;

	if( selected_file_no >= 0 && open_files.size() ) {
		(*selected_file)->closeMap();
	}
/*
	char _name[256];
	GetEnvironmentVariable("ALTUSERNAME", _name, 255);
	if (!stricmp(_name, "DEFAULT")  ) { 
		if( open_files.size() > 1 ) {
#ifndef _DEBUG
			return -1;
#endif
		}
	}
	GetEnvironmentVariable("VERSION", _name, 255);
	if (!stricmp(_name, "FREE")  ) { 
		if( open_files.size() > 1 ){
#ifndef _DEBUG
			return -1;
#endif
		}
	}
*/
	selected_file_no = (int)open_files.size();
	t_ret = new_map->openMap(file_name,selected_file_no,sub_maps);
	if( t_ret == 0 ) {
		open_files.push_back(new_map);
	} else {
		selected_file_no = -1;
		return selected_file_no;
	}

	selected_file = open_files.end()-1;
	selected_file_no = (int)(open_files.size()-1);

	lastError = 0;
	return selected_file_no;
}


bool IMG::isMapValid(int map_index) {
	if( map_index >= 0 && size_t(map_index) < open_files.size() ) {
		return true;
	}
	return false;
}

void IMG::closeMap() {
	if( open_files.size() && selected_file_no > -1 ) {
		(*selected_file)->closeMap();
		selected_file_no = -1;
	}
}

IMG::~IMG() {
	closeMap();
//	open_files.clear();
/*
	for( std::vector<IMG_internal_list*>::iterator delete_file = open_files.begin(); delete_file != open_files.end(); delete_file++ ) {
		//(*delete_file)->closeMap();
		delete *delete_file;
	}
	open_files.clear();
*/
}

void IMG::closeAllMaps() {
	closeMap();
}

bool IMG::closeMap(int map_index) {
	if( map_index == selected_file_no ) {
		closeMap();
		delete( *selected_file );
		selected_file = open_files.erase(selected_file);
		selected_file_no = -1;
	} else {
		if( map_index >= 0 && size_t(map_index) < open_files.size() ) {
			std::vector<IMG_internal_list*>::iterator delete_file = open_files.begin();
			delete_file += map_index;
			delete *delete_file;
			open_files.erase(delete_file);
			selected_file_no = -1;
		}
	}

	return	open_files.size() == 0;
}

bool IMG::selectMap(int map_index) {
	lastError = 0;

	if( map_index != selected_file_no ) {
		if( map_index < 0 ||  size_t(map_index) > open_files.size()-1)
			return false;

		closeMap();

		selected_file = open_files.begin() + map_index;
		selected_file_no = map_index;

		return (*selected_file)->selectMap();
	}
	return true;
}


int IMG::getFileSize(int map_index,int sub_map,internal_file_type file_type,std::string &internal_name) {
	internal_name = "";
	if( selectMap(map_index) ) {
		if( sub_map > (int)(*selected_file)->internal_files.size() || sub_map < 0 )
			return 0;
		if( (*selected_file)->internal_files.size() == 1 )
			sub_map = 1;
		if( sub_map == 0 ) 
			sub_map = 1;
		else{
			switch( file_type ) {
				case imgGMP:
					internal_name = (*selected_file)->internal_files[sub_map-1].gmp_internal_name;
					return (*selected_file)->internal_files[sub_map-1].gmp_size; 
				case imgTRE:
					internal_name = (*selected_file)->internal_files[sub_map-1].tre_internal_name;
					return (*selected_file)->internal_files[sub_map-1].tre_size; 
				case imgLBL:
					internal_name = (*selected_file)->internal_files[sub_map-1].lbl_internal_name;
					return (*selected_file)->internal_files[sub_map-1].lbl_size; 
				case imgRGN:
					internal_name = (*selected_file)->internal_files[sub_map-1].rgn_internal_name;
					return (*selected_file)->internal_files[sub_map-1].rgn_size; 
				case imgNET:
					internal_name = (*selected_file)->internal_files[sub_map-1].net_internal_name;
					return (*selected_file)->internal_files[sub_map-1].net_size; 
				case imgNOD:
					internal_name = (*selected_file)->internal_files[sub_map-1].nod_internal_name;
					return (*selected_file)->internal_files[sub_map-1].nod_size; 
			}
		}
	}
	return 0;
}

int IMG::getNOD1Size(int map_index,int sub_map) {
	if( selectMap(map_index) ) {
		if( sub_map > (int)(*selected_file)->internal_files.size() || sub_map < 0 )
			return 0;
		if( (*selected_file)->internal_files.size() == 1 )
			sub_map = 1;
		if( sub_map == 0 ) 
			sub_map = 1;
		else{
			return (*selected_file)->internal_files[sub_map-1].nod1_size; 
		}
	}
	return 0;
}

int IMG::getLabelCoding(int sub_map) {
	return (*selected_file)->getLabelCoding(sub_map);
}

int IMG::getCodePage(int sub_map) {
	return (*selected_file)->getCodePage(sub_map);
}

void IMG::readMap_details(int sub_map,mapRead_rect bounds,mapRead_coord minimum_size,int details) {
	(*selected_file)->labelMPlike = labelMPlike;
	(*selected_file)->readMap_details(sub_map,bounds,minimum_size,details);
}

void IMG::readMap(int sub_map,mapRead_rect bounds,mapRead_coord minimum_size,int layer) {
	(*selected_file)->labelMPlike = labelMPlike;
	(*selected_file)->readMap(sub_map,bounds,minimum_size,layer);
}

bool IMG::readMap_road(int road_idx) {
	return (*selected_file)->readMap(road_idx);
}

void IMG::readPOI(const unsigned int& type,const unsigned int& sub_type,const unsigned long& poi_address,const unsigned long& selected_map) {
	(*selected_file)->labelMPlike = labelMPlike;
	(*selected_file)->readPOI(type,sub_type,poi_address,selected_map);
}

void IMG::readNET(const unsigned long& net_address) {
	(*selected_file)->labelMPlike = labelMPlike;
	(*selected_file)->readNET(net_address);
}

long IMG::readNETroads(const int level,const unsigned long& net_address,int table_road[],int table_size) {
	return (*selected_file)->readNETroads(level,net_address,table_road,table_size);
}

void IMG::readIndex(_READINDEX indexFunction) {
	(*selected_file)->labelMPlike = labelMPlike;
	(*selected_file)->readIndex(indexFunction);
}

void IMG::readTRE(unsigned char& rgn,unsigned short& tre) {
	(*selected_file)->labelMPlike = labelMPlike;
	(*selected_file)->readTRE(rgn,tre);
}
