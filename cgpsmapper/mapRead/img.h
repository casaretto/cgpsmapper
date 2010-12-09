/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

#ifndef __img_h
#define __img_h

#define TRIAL 0
#define FREE 0

//#include <windows.h>
#include <string>
#include <vector>
#include "img_internal.h"
#include "img_struct.h"
#include "mapReadDLL.h"

namespace map_read {

class IMG {
private:
	//callbacks
	_HEADERDATA		headerFunction;
	_READDATA		dataFunction;
	_READDATA_INTEGER	dataFunctionInteger;
	_READPOI		poiFunction;
	_READNET		netFunction;

	_READPOI_INTERNAL		poiFunction_i;
	_READNET_INTERNAL		netFunction_i;
	//pointers to regions structures
private:
	unsigned char*	img_data;

	int				selected_file_no;
	std::vector<IMG_internal_list*>::iterator	selected_file;
	std::vector<IMG_internal_list*>				open_files;

	t_tre_coords	tre_coord;

	long			lastError;
	bool			labelMPlike;
private:
	//internal_file_type add_file_from_FAT(unsigned long &read_pos,unsigned long &fat_size);

	//Init file to memory mapping
	long	initMap();

	//Finish memory mapping
	void	closeMap();	

public:

	// Initialise and open IMG
	IMG(_HEADERDATA _headerFunction,_READDATA _dataFunction,_READDATA_INTEGER _dataFunctionInteger,_READPOI _poiFunction,_READNET _netFunction);
	void	setInternal(_READPOI_INTERNAL _poiInternal,_READNET_INTERNAL _netInternal);

	// Close
	~IMG();
	bool	selectMap(int map_index);

	//Read basic data
	//return map_index, number of sub_maps
	long	openMap(const char *file_name,int& sub_maps);

	//Close selected map
	bool	closeMap(int map_index);
	
	//Check if map with given index is valid
	bool	isMapValid(int map_index);

	//Close all maps
	void	closeAllMaps();

	void	setLableStyle(int labelStyle) {
		if(labelStyle) labelMPlike = true; else labelMPlike = false;
	};
	bool	getLabelStyle() {
		return labelMPlike;
	};
	int		getLabelCoding(int sub_map);
	
	int		getCodePage(int sub_map);

	long	getLastError() {return lastError;};

	// Read the map data
	void	readMap(int sub_map,mapRead_rect bounds,mapRead_coord minimum_size,int layer);
	bool	readMap_road(int road_idx);

	void	readMap_details(int sub_map,mapRead_rect bounds,mapRead_coord minimum_size,int details);

	void	readPOI(const unsigned int& type,const unsigned int& sub_type,const unsigned long& poi_address,const unsigned long& selected_map);
	void	readNET(const unsigned long& net_address);
	long	readNETroads(const int level,const unsigned long& net_address,int table_road[],int table_size);

	void	readIndex(_READINDEX indexFunction);
	void	readTRE(unsigned char& rgn,unsigned short& tre);

	// Some internal functions required for TDB creation...
	int		getFileSize(int map_index,int sub_map,internal_file_type file_type,std::string &internal_name);
	int		getNOD1Size(int map_index,int sub_map);
};

}

#endif
