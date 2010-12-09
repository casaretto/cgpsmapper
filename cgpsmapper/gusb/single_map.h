/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/
#ifndef __SINGLEMAPH
#define __SINGLEMAPH

// mlodsze 2 bajty - sub ID, 2 starsze - family ID
#define CONST_FAMILY_ID 0x03200001

#include <string>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <map>
#include "global_const.h"
#include "gps.h"
#include "progress.h"

#ifdef _WIN32
#include "Fish/blowfish.h"
#endif

#include "img_manager.h"

using namespace std;
extern bool	long_IMG_header;

class IMG;

class internal_file {
protected:
	string			internal_name;//internal name name, i.e. 12345678.RGN
	FILE*			input_file;
#ifdef _WIN32
	CBlowFish*		fish;
#endif
	bool			c_fish;
	unsigned char	f_buffer[0x201];
	char			f_password[256];


	int		size;//size of this one internal file
	char	xor_value;//0 or...

	//przewidziane podczas 'dodawania' map do GPS'a
	char	protected_buffer[0x200];//copy of first 0x200 bytes from the file - use when download from GPS to disc!
	bool	gps_file;
	
	unsigned int crypt_gps_id;      //if set then decrypt on the fly of the file
	bool	self_exe;

	bool	protected_buffer_use;
	size_t	read(void* buffer, int len);
	void	dexor(void* tmp_buffer,int len);

	friend	class IMG;
public:
	int		internal_offset;//offset in IMG file to this file
	string	file_name;//IMG file name
	char*	FAT; //only for imgIMF type!
	char	password[256];
	
	internal_file_type	file_type;
	int		TRE_id; //for imgTRE file only
	int		TRE_file_name; //name of the file (number)
	int		TRE_header_size;
	string	TRE_map_name;//map name to be show in GPS
	string	region_name;

	int		product_id; //used by imgTYP file only!!! if not set to 0 then will automatically update TYP file! Dangerous?

	//calculated positions of each subfile in the final IMG file
public:
	int		fat_entry_offset_start; //offsets for fat entries
	int		fat_entry_offset_end;
	int		fat_blocks; //number of occupied blocks by the file
	int		fat_first; //number of first occupied block
	int		file_start; //offsets for the file itself
	int		file_end;
public:
	internal_file() {product_id = 0; self_exe = false;gps_file = false;crypt_gps_id = 0;protected_buffer_use = false;};//not for use!
	internal_file(char xor_value,const char* file_name,const char* internal_file_name, internal_file_type file_type, int size);
	virtual	~internal_file();
	void	set_offset(int offset) {internal_offset = offset;};
	int		get_offset() {return internal_offset;};
	void	set_tre(int TRE_id,int TRE_file_name,int TRE_header_size,string TRE_map_name);
	void	set_gps_buffer(const char* buffer);
	void	set_self_exe() { self_exe = true;};

	virtual int		get_file_size() {return size;};
	void	set_blocks(int block_size);
	void	set_crypt_gps_id(unsigned int crypt_gps_id) {this->crypt_gps_id = crypt_gps_id;};
	int		get_blocks() {return fat_blocks;}; //return number of occupied blocks

	int		set_file_position(int first_free_block,int block_size);
	void	set_fat_position(int last_fat_offset);

	bool	get_fat(int file_pos,char* buffer); //returns one fat entry of size 0x200 or false
	virtual int		get_file(int file_pos,char* buffer); //returns number of bytes
	string	get_internal_filename();
	const char*	get_filename() const {return file_name.c_str();};
	const char* get_internal_short_name() const {return internal_name.c_str();};
	char	get_xor() {return xor_value;};

};

class internal_file_NT  {
public:
		internal_file		rgn_file;
		internal_file		tre_file;
		internal_file		lbl_file;
		internal_file		net_file;
		internal_file		nod_file;
};

class mps_internal_file : public virtual internal_file {
private:
	vector<internal_file>	tre_files;
	map<int,string>			region_list; //rec 0x46 MPS
	vector<internal_file>::iterator	tre_file_i;

	string	mapset_name;
	bool	initialised;
	int		product_id;
	char*	mps_buffer;
	vector<string> key_list;
public:
	mps_internal_file();
	virtual	~mps_internal_file();
	void	add_tre_file(internal_file tre_file);
	void	set_mapset_name(string name);
	void	set_product_id(int product_id);
	void	add_keys(vector<string> key_list);
	void	clear();
	virtual	int		get_file_size();
	virtual int		get_file(int file_pos,char* buffer);
};

//builder of one global IMG file
class IMG {
private:
	char	read_buffer[0x201];
	FILE*	input_file;
#ifdef _WIN32
	CBlowFish*		fish;
#endif
	bool			c_fish;
	unsigned char	f_buffer[0x200];
	char			f_password[256];
	char	xor_value;
	vector<internal_file>	files;
	vector<internal_file>	copy_files;
	vector<internal_file*>	files_fat;
	vector<internal_file>::iterator file_i;
	vector<internal_file*>::iterator file_f;
	//vector<internal_file>::iterator file_typ;

	//NT files
	map<string,internal_file_NT>	nt_files;//key - nazwa pliku

	mutable	vector<internal_file>::const_iterator file_const_i;
	void	dexor(void* tmp_buffer,int len);
	size_t	read(void* buffer, int len);

	//output file parameters
	int		block_size;
	int		size_of_final_img; //size of final IMG
	int		sent_bytes_img; //already sent bytes
	int		last_fat_entry;

	//one MPS global file
	mps_internal_file	MPS;
	bool	mps_enable;
	//bool	typ_enable;

	//create IMG header
	void	create_img_header(const char* mapset_name);

	//calculate all files - to know the block_size and all details for FAT
	void	calculate_files();

	//send preprepared data to gps
	bool	send_data(transmission_gps* gps,char* buffer,int size);

	void	create_header(char* buffer,const char* copyright);

	void	add_to_TRE_list(internal_file_type new_file_type,vector<internal_file> *TRE_file_list,const char* region_name);

public:
	IMG();

	static	void set_old_header(bool old) {long_IMG_header = !old;};
	static	bool get_old_header() {return long_IMG_header;};
	bool	operator()(const internal_file e1,const internal_file e2);
	IMG&	operator+=(const internal_file &i_file);
	IMG&	operator-=(const internal_file &i_file);

	void	set_mapset(const char* mapset_name);
	void	disable_mps();
	void	add_keys(vector<string> key_list);
	bool	no_IMG() {return (files.empty() && (raw_file.size()==0));};
	bool	add_IMG(const char* file_name,vector<internal_file> *TRE_file_list,
				const char* password="",unsigned int crypt_gps_id = 0,int crypt_start = 0,
				bool gps_file=false,const char* mps_file=NULL,const char* region_name=NULL,
				const int current_product_id = CONST_FAMILY_ID,
				const char* selected_file = NULL,const char* selected_file_rename = NULL); //returns TRE file

	bool	add_IMG_internal(const char* file_name,vector<internal_file> *TRE_file_list,
				const char* selected_file = NULL,
				const char* selected_file_rename = NULL);
	bool	remove_IMG(const char* file_name,const char* internal_file_name);
	bool	remove_exact_IMG(const char* internal_file_name);
	void	remove_all();

	/**
	selected_file - jesli nie null, tylko plik o podanej nazwie (niezaleznie od typu) zostanie dodany do listy plików do zaladowania
	selected_file_rename - tylko jesli ustawione powyzsze, wybranemu plikowi zostanie zmieniona nazwa
	*/
	internal_file_type	add_file_from_FAT(const char* file_name,string map_name,int block_size,int & read_pos,int &fat_size,unsigned int crypt_gps_id,int crypt_start,bool gps_file,const char* mps_file,const int current_product_id,const char* selected_file = NULL,const char* selected_file_rename = NULL);
	int		get_files_size();

	int		get_final_size() { return size_of_final_img; };
	void	set_final_size(int final_size) { size_of_final_img = final_size; };

	//make a local copy of 'files'
	void	preserve_files() {copy_files = files;};
	void	restore_files() {files = copy_files;};

	//prepare mapset
	void	calculate_final_IMG();

	//prepare GDB file from mapset
	void	calculate_final_GMP();
	void	delete_GMP();
	string	create_GMP(map<string,internal_file_NT>::iterator file_set);

	//re-initialise
	void	after_upload();

	//mapset
	bool	send_data(transmission_gps* gps,void (progress::*calculate)(int,int,bool,bool));

	//only ONE file sent to GPS
	bool	send_raw_data(transmission_gps* gps,void (progress::*calculate)(int,int,bool,bool),bool crypted = false,unsigned int gps_id = 0);
	static void	xor_data(char* buffer,int size,unsigned int gps_id);

	//download IMG functions
	void	reset_download_files() ;
	internal_file*	next_download_file() ;

	string	raw_file;
	string	map_copyright;
	int		raw_start_data;
	int		raw_len_data;
};

#endif
