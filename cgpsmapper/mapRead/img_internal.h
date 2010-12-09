/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

#ifndef _img_internal_h
#define _img_internal_h

#define _mapowanie

#ifdef _mapowanie
#define BOOST_DATE_TIME_NO_LIB
#include <boost/interprocess/managed_mapped_file.hpp>
#else
#include <windows.h>
#endif
#include <string>
#include <vector>
#include "img_struct.h"
#include "mapReadDLL.h"


enum	internal_file_type {imgUNKNOWN = 0, imgEMPTY, imgIMG, imgMPS, imgTRE, imgRGN, imgLBL, imgNET, imgNOD, imgSRT, imgMDR, imgFAT, imgDEM, imgTYP, imgGMP};

#define	READ_BUFFER_SIZE 20000
#define COORDS_BUFFER 20000

class IMG_file {
	//address table of the sub files
	unsigned long	_tre_file;
	unsigned long	_rgn_file;
	unsigned long	_lbl_file;
	unsigned long	_net_file;
	unsigned long	_nod_file;
	unsigned long	_gmp_file;

public:
	unsigned long	tre_file();
	unsigned long	rgn_file();
	unsigned long	lbl_file();
	unsigned long	net_file();
	unsigned long	nod_file();

	void	gmp_file(unsigned long val) {_gmp_file = val;};
	void	tre_file(unsigned long val) {_tre_file = val;};
	void	rgn_file(unsigned long val) {_rgn_file = val;};
	void	lbl_file(unsigned long val) {_lbl_file = val;};
	void	net_file(unsigned long val) {_net_file = val;};
	void	nod_file(unsigned long val) {_nod_file = val;};

	unsigned long	tre_file_data();
	unsigned long	rgn_file_data();
	unsigned long	lbl_file_data();
	unsigned long	net_file_data();
	unsigned long	nod_file_data();

	unsigned long	tre8_data;
	unsigned long	tre8_data_len;
	unsigned short	tre8_rec_len;
	unsigned long	tre8_flag;

	unsigned long	rgn1_data;
	unsigned long	rgn1_data_len;

	unsigned long	rgn2_data;
	unsigned long	rgn2_data_len;

	unsigned long	rgn3_data;
	unsigned long	rgn3_data_len;

	unsigned long	rgn4_data;
	unsigned long	rgn4_data_len;

	unsigned long	lbl1_data;
	unsigned long	lbl2_data;
	unsigned long	lbl2_data_len;
	unsigned long	lbl3_data;
	unsigned long	lbl3_data_len;
	unsigned long	lbl4_data;
	unsigned long	lbl4_data_len;
	unsigned long	lbl5_data;
	unsigned long	lbl6_data;
	unsigned char	lbl6_flag;
	unsigned long	lbl8_data;
	unsigned long	lbl8_data_len;
	unsigned int	lbl8_bits;

	unsigned short	lbl2_rec_len;
	unsigned short	lbl2_flag;
	unsigned short	lbl3_rec_len;
	unsigned short	lbl3_flag;
	unsigned short	lbl4_rec_len;
	unsigned int	lbl4_bits;
	unsigned short	lbl4_rec;
	unsigned short	lbl4_flag;
	unsigned int	lbl8_rec_len;
	unsigned short	lbl8_rec;
	unsigned short	lbl8_flag;

	unsigned long	net1_data;
	unsigned char	net1_modif;
	unsigned char	net_base_bits;

	unsigned long	nod1_data;
	unsigned long	nod2_data;

	//file size
	unsigned long	gmp_size;
	unsigned long	lbl_size;
	unsigned long	rgn_size;
	unsigned long	tre_size;
	unsigned long	net_size;
	unsigned long	nod_size;
	unsigned long	nod1_size;

	std::string		gmp_internal_name;
	std::string		lbl_internal_name;
	std::string		rgn_internal_name;
	std::string		tre_internal_name;
	std::string		net_internal_name;
	std::string		nod_internal_name;
	
	t_map_levels	map_level;

	//LBL coding format
	unsigned char	lbl_coding;
	unsigned char	lbl_shift;
	int				code_page;

	//map information
	mapRead_rect	map_bounds;
	//char			map_name[40];
	std::string	copyright1,copyright2,copyright3;

	IMG_file();
};

//One IMG file can have more than one map....
class IMG_internal_list {
private:
	//callbacks
	_HEADERDATA		headerFunction;
	_READDATA		dataFunction;
	_READDATA_INTEGER	dataFunctionInteger;
	_READPOI		poiFunction;
	_READNET		netFunction;

	_READPOI_INTERNAL		poiFunction_i;
	_READNET_INTERNAL		netFunction_i;

	//during reading of data - bounds of interest
	long			X0,X1,Y0,Y1;//bounds as integer
	long			minX,minY;

	//coords buffer
	mapRead_coord			coords[COORDS_BUFFER];
	mapRead_coord_integer	coords_int[COORDS_BUFFER];
	mapRead_address			address[_MAX_SEGMENTS];
	mapRead_address_i		address_i[_MAX_SEGMENTS];
	mapRead_address_i		address_i_tmp[_MAX_SEGMENTS];

	//additional road attributes
	t_road_data		road_data;

	//layer we want to process
	char			active_layer;

	bool			interrupt;
	bool			nag_label;
	unsigned char	current_RGN;
	unsigned short	current_TRE;

	internal_file_type add_file_from_FAT(unsigned long &read_pos,unsigned long &fat_size,unsigned long &ret_data_start,std::string &ret_file_name,unsigned int &file_len);
	void			drawTRE(const tre &tre_data);
	bool			redrawTRE(const unsigned short &tre_idx);//Processing TRE - up to reaching 

	//Check if TRE visible
	bool			visibleTRE(const tre &tre_data,const char &layer);
	bool			visibleTRE(const unsigned short &tre_idx);

	//Locate RGN10/20 - return name and coordinates
	bool			locateRGN1020(const unsigned short& tre_idx,const short& rgn_idx,t_coord& coordinates,std::string& label,unsigned long& lbl);
	//Locate RGN40
	bool			locateRGN40(const unsigned short& tre_idx,const short& rgn_idx,unsigned char &layer,unsigned long& address,long& bit_shift,long& start_x,long& start_y);
	//Read segment definitions - returns count of points
	int				exportPolylineSegments(const int layer,const int type,const unsigned long& address,const long& bit_shift,const long& start_x,const long& start_y,mapRead_address segments[_MAX_SEGMENTS],unsigned int &t_address_size);
	

	//Process single TRE - draw / export data
	unsigned long	exportPolyline(const int layer,const int type,const unsigned long& address,const long& bit_shift,const long& start_x,const long& start_y);
	unsigned long	exportPoint(const int layer,const int type,const unsigned long& address,const long& bit_shift,const long& start_x,const long& start_y);

//LBL
	void			readCityRegionCountry(const int& lbl4,std::string& city,std::string& region,std::string& country);
	void			readZip(unsigned int& lbl8,std::string& zip);
	void			readCityInfo(const unsigned long& net_address,unsigned long &data_start,unsigned short flag,mapRead_address address[_MAX_SEGMENTS],unsigned short t_data_len = 0);
	void			readZipInfo(unsigned long &data_start,unsigned short flag,mapRead_address address[_MAX_SEGMENTS]);
	void			readNumbers(const unsigned long& net_address,unsigned long &data_start,unsigned short flag,unsigned char net_flag,mapRead_address address[_MAX_SEGMENTS],int segments_count);

//internal - with lbl address
	void			readCityRegionCountry_i(const int& lbl4,std::string& city,unsigned long &Lcity,std::string& region,unsigned long &Lregion,std::string& country,unsigned long &Lcountry);
	void			readCityInfo(const unsigned long& net_address,unsigned long &data_start,unsigned short flag,mapRead_address_i address[120],unsigned short t_data_len = 0);
	void			readZipInfo(unsigned long &data_start,unsigned short flag,mapRead_address_i address[120]);
	void			readZip_i(unsigned int& lbl8,std::string& zip,unsigned long& lbl_address);

	void			append6bitSymbol(int & _state, unsigned char _bt, std::string& _buf,bool& labelMPlike);
	std::string		read11Number(const unsigned char* buffer,const size_t& max_size,size_t& position);

	//bool			translatePoiFlag(unsigned char img_flag,unsigned char flag,unsigned char field_element);

public:
	std::vector<IMG_file>			internal_files;
	std::vector<IMG_file>::iterator	selected_file;

	std::vector<int>				road_rgn_tre;

	bool			labelMPlike;
	//general IMG data and access
	std::string		file_name;
#ifdef _mapowanie
	boost::interprocess::file_mapping*		img_file;
	boost::interprocess::mapped_region*		img_handle;
#else
	HANDLE	img_file;
	HANDLE	img_handle;
#endif
	unsigned char	img_xor;
	unsigned long	img_size;
	unsigned long	block_size;

	//buffers
	unsigned char	read_buffer[READ_BUFFER_SIZE];
	unsigned char	read_buffer2[READ_BUFFER_SIZE];

	//bit reading
	unsigned int	bit_position;
	unsigned int	bit_stream_len;

	mapRead_rect	map_bounds; //sume of all bounds
	char			map_name[40];
	long			lastError;
	unsigned char*	img_data;

	t_tre_coords	tre_coord;

	//Methods
	//set callback functions
	IMG_internal_list(_HEADERDATA _headerFunction,_READDATA _dataFunction,_READDATA_INTEGER _dataFunctionInteger,_READPOI _poiFunction,_READNET _netFunction);
	~IMG_internal_list();

	//set additional callbacks (optional)
	void			setInternal(_READPOI_INTERNAL _poiInternal,_READNET_INTERNAL _netInternal);

	long			openMap(const char *file_name,int selected_file_no,int& sub_maps);
	long			initMap(int selected_file_no,unsigned int selected_sub_map); //wybrany zestaw map i wybrana podmapa
	void			closeMap();
	bool			selectMap();

	void			readMap(int sub_map,mapRead_rect bounds,mapRead_coord minimum_size,int layer);
	bool			readMap(int road_idx);
	void			readMap_details(int sub_map,mapRead_rect bounds,mapRead_coord minimum_size,int details);
	void			readPOI(const unsigned int& type,const unsigned int& sub_type,const unsigned long& poi_address,const unsigned long& net_address);
	void			readNET(const unsigned long& net_address);
	long			readNETroads(const int level,const unsigned long& net_address,int table_road[],int table_size);

	//reading functions
	//Reading from the mapped memory - can be replaced
	inline void		readData(const unsigned long& address,size_t size,const unsigned int& buffer=1);

	//bits reading
	void			initialiseReadBits(const unsigned long& start_address,const unsigned int& total_number_of_bytes);
	inline unsigned long readBits(const unsigned short &number_of_bits);

	//Read value from the bit stream
	//before use initialiseReadBits must be used!
	inline long		readValue(const unsigned short &number_of_bits,const char &type);
	void			decryptBuffer(unsigned int decrypt_value,size_t size);

	//reading of the RGN table
	void			readRGNTable(const tre& tre_data,rgn_table& rgn,const bool& parent);

	//reading of the extended address table
	void			readRGNExtendedTable(const tre& tre_data,rgn_extended & rgn,const unsigned short& tre_idx);


	//reading and decoding of LBL string
	std::string		readLBL(const long& address);

	void			readIndex(_READINDEX indexFunctioN);
	void			readTRE(unsigned char& rgn,unsigned short& tre) { rgn = current_RGN; tre = current_TRE;};

	int				getLabelCoding(int sub_map);
	int				getCodePage(int sub_map);
};

#endif