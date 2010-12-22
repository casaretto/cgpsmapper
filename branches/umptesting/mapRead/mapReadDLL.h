/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/
#ifndef __MAPREADDLL_H
#define __MAPREADDLL_H

#ifdef __cplusplus
extern "C" {
#endif

/*
There are 2 version of the library available

FREE
	- basic functionality - no limitations for distribute for non commercial use, dialog box visible at the initialization time
	- not able to read POI details
	- not able to read road deatails
	- no limitations for number of maps open
COMMERCIAL
	- can be used for commercial purpose (only developer needs to buy one licence then library can be redistributed)
	- can read POI details
	- can read road details
	- can read locked maps (might be limited to defined list of FIDs)
*/

#define _MR_OK                          0
#define _MR_NOT_IMG						-1
#define _MR_CANNOT_INITIALISE			-2
#define _MR_NOT_INITIALISED				-3
#define _MR_BAD_PARAMS                  -4
#define _MR_NOT_REGISTERED              -5
#define	_MAX_SEGMENTS					512

//#ifdef _MAPREAD_DLL
#ifdef _DLL_VERSION
	#define LIBSPEC __declspec(dllexport)
	#define CALL_API __stdcall
#else
	#ifdef _MAPREAD_STATIC
		#define LIBSPEC
		#define CALL_API
	#else
		#define LIBSPEC __declspec(dllimport)
		#define CALL_API __stdcall
	#endif
#endif
/*#else
	#define LIBSPEC
	#define CALL_API
#endif*/


struct mapRead_coord {
	float we, ns;
};

struct mapRead_coord_integer {
	int we, ns;
};

struct mapRead_rect {
	// In degrees.
	float x0, y0, x1, y1;
};

struct mapRead_segment {
	unsigned int	coordinate_number;
};

struct mapRead_address {
	char	city_left[80];
	char	city_right[80];

	char	region_left[80];
	char	region_right[80];

	char	country_left[80];
	char	country_right[80];

	int		segment; //point index where segment definition start - it is valid till next definition (not necessary next point!)
	mapRead_coord	segment_coord;//segment coordinate
	int		road_idx;//unique ID of road - returned by data read function - to match array of segment data with specific road

	int		left_type;
	int		left_start;
	int		left_end;

	int		right_type;
	int		right_start;
	int		right_end;

	char	zip_left[80];
	char	zip_right[80];

	char	city_left_2[80];
	char	city_right_2[80];

	char	region_left_2[80];
	char	region_right_2[80];

	char	country_left_2[80];
	char	country_right_2[80];

	char	city_left_3[80];
	char	city_right_3[80];

	char	region_left_3[80];
	char	region_right_3[80];

	char	country_left_3[80];
	char	country_right_3[80];
};

struct mapRead_address_i {
	unsigned short	city_left;
	unsigned short	city_right;

	unsigned short	city_left_2;
	unsigned short	city_right_2;

	unsigned short	city_left_3;
	unsigned short	city_right_3;

	int		segment;
	int		road_idx;

	unsigned short	zip_left;
	unsigned short	zip_right;
};


//Callback function definitions
/**
 Function returning basic information about map
 @map_index	- index of open map (to be used for select map, close map..)
 @map_sub_index - sub index of map (in case IMG contains more than one map)
 @map_id
 @map_name
 @copyright1
 @copyright2
 @levels	- number of levels in map
 @x0, @x1, @y0, @y1 - bounds of map
 @locked	- 1 if map is locked
 @level_def	- bit resolution of levels
*/
typedef int (CALL_API *_HEADERDATA)(int map_index,int map_sub_index,const char* map_id,const char* map_name,const char* copyright1,const char* copyright2,int levels,float &x0,float &x1,float &y0,float &y1,int locked,unsigned char level_def[10]);
/**
 Function returning bounds data of map
*/
typedef int (CALL_API *_HEADERDATA_BOUNDS)(float &x0,float &x1,float &y0,float &y1);
/**
 Function returning single object from map
 @layer			- on which layer object is present
 @object_type	- 0x10 (city), 0x20 (POI), 0x40 (line), 0x80 (polygon)
 @type
 @sub_type
 @poi_address	- if flag & 0x01 then poi_address and net_address (needs to be passed as poi_index) contains address to be used with function mapRead_readPOI
 @net_address	- if flag & 0x02 then net_address contains address to be used with function mapRead_readNET
 @road_idx      - used only for roads, unigue ID of road, to be used when reading NET data - in array mapRead_address ID is returned - as array may cover more than single road object
 @label,
 @coords		- table with coordinates
 @coord_size	- size of table with coordinates
 @flag			- used for poi_address and net_address
				1 - poi exist
				2 - net exist
				4 - extended poi
 @selected_map	- used only when poi_address and net_address, has to be passed to function for reading additional data
 @reserved
*/
typedef int (CALL_API *_READDATA)(int layer,int object_type,int type,int sub_type, unsigned int poi_address,unsigned int net_address,unsigned int object_id,const char* label,mapRead_coord* coords,int coord_size,const char flag,unsigned int reserved);
/**
 Function returning single object from map - coordinates are integer
*/
typedef int (CALL_API *_READDATA_INTEGER)(int layer,int object_type,int type,int sub_type, unsigned int poi_address, unsigned int net_address,const char* label,mapRead_coord_integer* coords,int coord_size,const char flag,unsigned int reserved);
/**
 Function returning single POI object - called by mapRead_readPOI
*/
typedef int (CALL_API *_READPOI)(const char* house_number,const char* street_name,const char* city, const char* region, const char* country,const char* zip,const char* phone);
/**
 Function returning single road object - called by mapRead_readNET
*/
typedef int (CALL_API *_READNET)(const char* name1,const char* name2,const char* name3,const char* name4,int one_way,int road_class,int road_speed,int &records,mapRead_address address[_MAX_SEGMENTS]);

/**
 Open IMG file
 @param	file_name
 @param	_headerFunction
 @param	_dataFunction
 @param _dataFunctionInteger - only one function should exist - dataFunction or dataFunctionInteger
 @param	_poiFunction
 @param	_netFunction
 
 @param  sub_maps - return number of sub maps - to be used by mapRead_readMap
 @return 0 for success or _MR_CANNOT_INITIALISE

*/
long CALL_API mapRead_openIMG(const char *file_name,int& sub_maps,_HEADERDATA _headerFunction,_READDATA _dataFunction,_READDATA_INTEGER _dataFunctionInteger,_READPOI _poiFunction,_READNET _netFunction);

/**
 Close previously open file
 @param	map_index	- map to be closed
*/
void CALL_API mapRead_closeIMG(int map_index);

/**
 Change reading of labels from MP like style to real codes
 @param	style		- 1 MP like, 0 - real codes
*/
void CALL_API mapRead_labelStyle(int style);
int	CALL_API mapRead_getLabelCoding(int map_index,int sub_map);

/**
 @return			- map codepage
 */
int	CALL_API mapRead_getCodePage(int map_index,int sub_map);

/**
 Close all open files
*/
void CALL_API mapRead_closeAll();

/**
 Check if map_index is still valid
 @param map_index
 @return			- true if map is still open
*/
bool CALL_API mapRead_mapValid(int map_index);

/**
 Return last win32 api error
*/
long CALL_API mapRead_lastError();

/**
 Read desired part of the map

 @param	map_index - map to be processed
 @param	sub_map - if a single file is built from more than one map - then this parameter can be used to process only one map
				if set to 0 - then all maps from the file are processed
 @param x0,x1,y0,y1 - bounds to be processed
 @param min_ns,min_we - if object will be smaller than min_ns,min_we - it will not be returned by the procedure
 @param layer - map layer to be processed - header callback receives list of layers of the map (0 is most detailed layer)
*/
void CALL_API mapRead_readMap(int map_index,int sub_map,float& x0,float& y0,float& x1,float& y1,float& min_ns,float& min_we,int layer);

/**
 Read desired part of the map

 @param	map_index - map to be processed
 @param	sub_map - if a single file is built from more than one map - then this parameter can be used to process only one map
				if set to 0 - then all maps from the file are processed
 @param x0,x1,y0,y1 - bounds to be processed
 @param min_ns,min_we - if object will be smaller than min_ns,min_we - it will not be returned by the procedure
 @param details - layers of the map equal or higher will be processed - most detailed is always 24
*/

/**
 Read of POI data
 NOT AVAILABLE IN THE FREE VERSION
 @param	type		type of the object
 @param	sub_type	sub type of the object
 @param	poi_address	address of POI structure
 @param net_address 2nd part of address
*/
void CALL_API mapRead_readPOI(int map_index,const unsigned int& type,const unsigned int& sub_type,const unsigned long& poi_address, const unsigned long& net_address);

/**
 Read of NET additional data - street citi/region/country - calls callback _READNET function
 NOT AVAILABLE IN THE FREE VERSION
 @param	net_address	address of NET structure
*/
void CALL_API mapRead_readNET(int map_index,const unsigned long& net_address);

/**
 Read of NET additional data - returns all road (in order) which compose single NET structure
 For layer 0 single NET structure can be described with segment information (returned in array of mapRead_address data)
 NET structure is composed from list of single road object - separate for each layer (if road exist in more than one layer) - segment description covers only 0 layer

 NOT AVAILABLE IN THE FREE VERSION


 @param level		- level for which return list of roads in NET structure
 @param net_address	- address of NET structure
 @param table_road  - table of road_idx (int)
 @param table_size	- size of table_road (if will be not big enough - some road_idx will be not stored)
 @return			- number of road_idx (if more than size of table_road - not all road_idx are in table_road)
 */
long CALL_API mapRead_readNETroads(int map_index,const unsigned long& net_address,int layer,int table_road[],int table_road_size);

/**
 Read only selected road - by road_idx
 @param map_index
 @param road_idx
 @return 0 - not a valid road_idx
*/
int CALL_API mapRead_readMap_road(int map_index,int road_idx);

/**
 Interface to sorted by name roads - returning NET address - then mapRead_readNETroads can be used to retreive road_idx values and then real objects on map 
 with mapRead_readMap_road
*/

/**
 Read size of NET sorted array
 @param	map_index
 */


//For internal use only!
typedef int (CALL_API *_READPOI_INTERNAL)(const char* name,unsigned long l1,const char* house_number,const char* street_name,unsigned short city_idx,unsigned short zip_idx,const char* phone,unsigned char RGN,unsigned short TRE);
typedef int (CALL_API *_READNET_INTERNAL)(const char* name1,unsigned long l1,const char* name2,unsigned long l2,const char* name3,unsigned long l3,const char* name4,unsigned long l4,int one_way,int road_class,int road_speed,int &records,mapRead_address_i address[_MAX_SEGMENTS]);
typedef int (CALL_API *_READINDEX)(int country_idx,const char* country,unsigned long l1,int region_idx,int region_country_idx,const char* region,unsigned long l2,int city_idx,int city_region_idx,int city_country_idx,const char* city,unsigned long l3,int zip_idx,const char* zip,unsigned long l4);
void CALL_API mapRead_setInternal(int map_index,_READPOI_INTERNAL _poiInternal,_READNET_INTERNAL _netInternal);
void CALL_API mapRead_readCities(int map_index,_READINDEX indexFunction);
void CALL_API mapRead_readTRE(unsigned char& rgn,unsigned short& tre);

#ifdef __cplusplus
}
#endif

#endif
