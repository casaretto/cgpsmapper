/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/
#ifndef __UPLOADERGPSH
#define __UPLOADERGPSH

#pragma warning (disable: 1786) 

#ifndef _WIN32
#define MAX_PATH 3000
#endif

#include <string>
#include <algorithm>
#include <map>

#define FULL 1
#define GETFILE 1
#define TRIAL 0

#include "g_usb_gps.h"
#include "com_gps.h"
#include "sim_gps.h"
#include "single_map.h"

#pragma pack(push,1)
struct t_sendmap {
	char magic_string[10];
	int	start_data;
	int	len_data;
	int copyright_start_data;
	int copyright_len_data;
	int expired_start_data;
	int expired_len_data;
	int year,month,day;
	int	shareware;
	unsigned int gps_id;
	char copyright_string[120];
};
#pragma pack(pop)

using namespace std;

class map_uploader {
	bool	gps_found;
	bool	turn_off;
	bool	mps_enable;
	bool	typ_enable;
	bool	disable_out;
	bool	custom_flash;
	string	port;
	comm_medium_type port_type;

	transmission_gps* gps;

	//connection parameters
	int		upload_speed;
	int		send_delay;
	int		get_delay;
	int		speed_change_delay;

	//set of maps to upload
	IMG		imgs;
	IMG		gps_maps;
public:
	static map<string,int>	products_id;
	static int get_product_id(const char* region_name);	

	char	map_copyright[50];

	map_uploader(comm_medium_type port_type,const char* port,bool turn_off = false,bool custom_flash = false);
	~map_uploader();


	void	reconnect(comm_medium_type port_type,const char* port);
	bool	is_simulator() {return port_type == resSIM;};
	bool	connetcion_established() {return gps->connetcion_established();};

	void	disable_mps() {mps_enable = false;typ_enable = false;};
	void	disable_output() {disable_out = true;};
	unsigned int		id() {return gps->GPS_id;};
	int		memory();
	char*	get_gps_version() {return gps->get_gps_version();};
	void	connection_parameter(int upload_speed, int send_delay = 3, int get_delay = 3, int speed_change_delay = 100);
	bool	check_connection();
	void	add_keys(vector<string> key_list);

	bool	no_files();
	bool	add_img_file(const char* file_name,vector<internal_file> *TRE_file_list,const char* password="",unsigned int crypt_gps_id = 0,int crypt_start = 0,const char* region_name = NULL,const int current_product_id = CONST_FAMILY_ID,const char* selected_file=NULL,const char* selected_file_rename=NULL);
	bool	add_img_file_internal(const char* file_name,vector<internal_file> *TRE_file_list,const char* selected_file=NULL,const char* selected_file_rename=NULL);
	bool	remove_img_file(const char* file_name,const char* internal_file_name);
	void	remove_all();
	int		get_files_size();
	void	set_selfupload_size(int final_size) {imgs.set_final_size(final_size);};

	void	store_file_list(const char* file_name);

	void	add_raw_file(const char* file_name,int start_data = 0,int len_data = 0);
	void	set_mapset_name(const char* mapset_name);

	bool	upload(bool t_raw_data,bool crypted = false,unsigned int gps_id = 0,bool NT=false);
	bool	erase_only();
	void	turn_off_after_upload() {this->turn_off = true;};

	bool	download_directory(vector<internal_file> *TRE_file_list,bool image = false);
	bool	download_files(char* store_path,size_t store_path_size);
	void	mps_parser(const char* file_name,vector<internal_file> *TRE_file_list,IMG *maps);
};

#endif
