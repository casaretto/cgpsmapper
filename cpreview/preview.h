/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

#ifndef __PREVIEW_H
#define __PREVIEW_H

#include <string>
#include <vector>
#include <iostream>
#include "filexor.h"
#include "tdb.h"
#include "mdr_creator.h"
#include "../mapRead/img.h"

/*
 //						(int map_index,int map_sub_index,const char* map_id,const char* map_name,const char* copyright1,const char* copyright2,int levels,float &x0,float &x1,float &y0,float &y1,int locked,unsigned char level_def[10])
int CALL_API readHeader_(int map_index,int map_sub_index,const char* map_id,const char* map_name,const char* copyright1,const char* copyright2,int levels,float &x0,float &x1,float &y0,float &y1,int locked,unsigned char level_def[10]);

//                     (int layer,int object_type,int type,int sub_type, unsigned int poi_address, unsigned int net_address,const char* label,mapRead_coord_integer* coords,int coord_size,const char flag,unsigned int reserved)
int CALL_API readDataI_(int layer,int object_type,int type,int sub_type, unsigned int poi_address, unsigned int net_address,const char* label,mapRead_coord_integer* coords,int coord_size,const char flag,unsigned int reserved);

//					  (int layer,int object_type,int type,int sub_type, unsigned int poi_address,unsigned int net_address,unsigned int object_id,const char* label,mapRead_coord* coords,int coord_size,const char flag,unsigned int reserved)
int CALL_API readData_(int layer,int object_type,int type,int sub_type, unsigned int poi_address,unsigned int net_address,unsigned int object_id,const char* label,mapRead_coord* coords,int coord_size,const char flag,unsigned int reserved);

//					 (const char* house_number,const char* street_name,const char* city, const char* region, const char* country,const char* zip,const char* phone)
int CALL_API poiData_(const char* house_number,const char* street_name,const char* city, const char* region, const char* country,const char* zip,const char* phone);
//					 (const char* name1,const char* name2,const char* name3,const char* name4,int one_way,int road_class,int road_speed,int &records,mapRead_address address[_MAX_SEGMENTS])
int CALL_API netData_(const char* name1,const char* name2,const char* name3,const char* name4,int one_way,int road_class,int road_speed,int &records,mapRead_address address[_MAX_SEGMENTS]);
*/

class img_item {
public:
	int region_id;
	int product_id;
	std::string id_name;
	std::string name;
	std::string region_name;

	img_item(std::string a_id_name,std::string a_name,int a_region_id,std::string a_region_name,int a_product_id) : name(a_name),id_name(a_id_name), region_id(a_region_id), region_name(a_region_name), product_id(a_product_id) {};
};

class MDX_element
{
public:

	unsigned long imgID;
	unsigned short familySubID;
	unsigned short familyID;
	unsigned long imgFileName;

	MDX_element(unsigned long imgID,unsigned short familySubID,unsigned short familyID,unsigned long imgFileName);
	MDX_element();
	bool operator()(const MDX_element &e1,const MDX_element &e2);
};

class MDX_manager
{
public:
	vector<MDX_element> mdx;

	void AddIMG(MDX_element element);
	void Write(const char* file_name);
};

class previewTDB  {
public:
	//MP parametry
	int		levels;
	unsigned char	zoom_def[10];
	unsigned char	level[10];
	int				TRE_ID; //ID for preview map
	std::vector<std::string> dictionary;

	//TDB
	std::string	fileName;

	int			FID;
	int			colour;
	bool		height;
	bool		locked;
	bool		marine;
	int			codepage;
	int			productCode;
	int			mapVersion;
	std::string	mapsourceName;
	std::string mapSetName;
	std::string CDSetName;
	std::string copy1,copy2,copy3;

	TDB*		TDB_file;
	MDX_manager	MDX_file;

	std::vector<img_item> list_img;

	void execute(bool &mdr,bool &pvx);
	previewTDB();
	~previewTDB() {
		list_img.clear();
		dictionary.clear();
	}
};

class previewMDR {
public:
	MDR_CREATOR::MdrCreator mdrCreator;

	//MDR
	std::string	fileName;
	int		codepage;
	bool	erase_temporary;
	bool	idx_ignore;
	std::string sortfile_location;

	std::vector<img_item> list_img;

	void execute(bool by_region,bool multibody);
	previewMDR(string base_name,bool idx_ignore);
	~previewMDR();
};

class preview {
public:
	previewMDR*	mdr;
	previewTDB*	tdb;
	//config reader
	xor_fstream* file;
	bool	erase_temporary;
	preview(string base_name,bool idx_ignore);
	~preview();

	bool	openConfig(const char* fileName);
	void	readMap();
	void	readFiles();
	void	readDictionary();
	void	setHomePath(const char* progPath);

	void	processFiles();
	void	run(bool by_region,bool multibody,bool pvx_mode);
};


#endif
