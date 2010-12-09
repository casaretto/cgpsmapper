/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

#ifndef TDBH
#define TDBH

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <stdexcept>
#include "filexor.h"

using namespace std;

//ms have a bug, if filenames <0x10000 , then after protection it cr ashed, in other case all is fine


class TDB_header    //50
{
public:
	int  recordLength;

	//1=CONT USA, 2=HAWAII, 5=WorldMap, 14=R&R Germany...
	unsigned int	 productCode;
	int		m_family,sub_family;
	int		version;
	int		mapVersion;
	int		codepage;
	string  productName;
	string  productType;
	unsigned char unk8; //nod bit size?
	unsigned char unk10_bg_color;


	unsigned char unlock_protect;
	unsigned char transparent;
	unsigned char fill_type;
	unsigned char routing_protect;
	unsigned char heigh_profile;
	int	maxN,maxE,maxS,maxW;

	TDB_header();
	void Write(xor_fstream* file,int m_Type);
};

class TDB_info_string
{
public:
	unsigned int code;
	string name;
};

class TDB_copyright //44
{
public:
	int  recordLength;
	vector<TDB_info_string> list_TDB_info_string;
	void Write(xor_fstream* file);
};

class TDB_map       //42
{
public:
	int  recordLength;
	unsigned int areaCode;       //const

	unsigned int map_0;

	int	    maxN,maxE,maxS,maxW;
	string  name;

	void ReadText(xor_fstream* file);
	void Write(xor_fstream* file);
};

class TDB_dca_region {
public:
	unsigned char m_sub_family;
	string name;
};

class TDB_dca //0x52
{
public:
	int  recordLength;
	vector<TDB_dca_region> regions;	

	void Write(xor_fstream* file);
};

class TDB_region //0x53
{
public:
	int  recordLength;
	unsigned char m_sub_family;
	int	 productCode;

	void Write(xor_fstream* file);
};

class TDB_img
{
public:
	int  recordLength;
	unsigned int  areaCode;  //same as in 0x42
	int		regionID;
	string	regionName;

	int	    IMGID;//bylo char
	int		maxN,maxE,maxS,maxW;
	string  img_name;
	int  img_sections; //RGN, LBL, TRE, UNNAMED = 0004, with NET = 5
	int  img_sections_real; //RGN, LBL, TRE = 0003, with NET = 4, without UNNAMED

	unsigned int RGN_length;
	unsigned int TRE_length;
	unsigned int LBL_length;
	unsigned int NET_length;
	unsigned int NOD_length;
	unsigned int GMP_length;
	//
	string	rgn_name;
	string	tre_name;
	string	lbl_name;
	string	net_name;
	string	nod_name;
	string	gmp_name;

	void Write(xor_fstream* file,int m_Type);
};

class TDB {
public:

	TDB(unsigned int productCode,string productName,string productType,int mapVersion,bool locked,bool marine);
	TDB_header    m_TDB_header;
	TDB_copyright m_TDB_copyright;
	TDB_map       m_TDB_map;
	TDB_dca       m_TDB_dca;
	TDB_region    m_TDB_region;
	int		m_Type;
	int		m_family;
	int		m_sub_family;

	//some extra data used by external objects
	string        mainIMGname;
	string        fileLocation;

	vector<TDB_img> m_TDB_img;

	xor_fstream*    file;

	void addRegion(int regionID,string name);
	void setCodePage(int codepage);
	void setFamily(int family);
	void setSubFamily(int sub_family);
	void setHeighProvile();
	void setMDR(bool status);

	bool WriteTDBfile(const char* tdb_file_name);

	void SignTDB(const char* tdb_file_name);
};

#endif
