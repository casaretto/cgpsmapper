/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

#ifndef __TRANSMISSIONGPSH
#define __TRANSMISSIONGPSH

#pragma warning (disable: 1786) 

#ifdef _WIN32
#include <windows.h>
#endif
#ifdef LINUX
#include <unistd.h>
#endif

#include <iostream>

using namespace std;

enum comm_medium_type { resCOM = 0, resUSB, resSIM, resGUSB };
enum panic_error_code_type { errNo, errTooManyNAK };

class internal_file;
class transmission_gps {
protected:
	bool	gps_found;
	bool	gps_communication_ok;
	comm_medium_type	comm_medium;
	char	buffer[0x1000];
	char	receive_buffer[0x1000];
	char	send_buffer[0x1000];
	int		map_address; //destination address 
	int		buffer_pos;
	int		received_length;
	bool	silent;
	panic_error_code_type	error_code;

	unsigned int convert_to_int(int index);//convert chars from buffer to int from index
	unsigned int received_buffer_convert_to_int(int index);//convert chars from buffer to int from index
	unsigned int convert_to_short(int index);//convert chars from buffer to int from index
	void delay(long ms);
	union {
		unsigned int value;
		char value_c[4];
	} int_conv;

	virtual int	get_command_index() = 0;
	virtual int	get_data_index() = 0;
	virtual int	get_length_index() = 0;

	void	parse_protocol();
	virtual void process_speed_change() {;};

	int		parse_message();

//
	virtual int read_data() {return -1;};

//GPS specyfic
	int		flash_id;
	bool	GPS_map_capable;
	bool	GPS_routable;
	bool	GPS_unlock_capable;

//status :
public:
	bool	got_protocol;
	bool	got_memory;
	bool	got_memory_erased;
	bool	got_speed_change;
	bool	got_error_speed_change;
public:
	unsigned int GPS_id;
	unsigned int GPS_packet_size; // ?????
	unsigned int GPS_software;
	unsigned int GPS_memory;
	unsigned int GPS_max_maps;
	char	GPS_version[255];

public:
	transmission_gps(bool silent = false);
	virtual ~transmission_gps() {;};

	panic_error_code_type get_panic_error() {return error_code;};

	int	get_flash_id() {return flash_id;};
	char*	get_gps_version() {return GPS_version;};
	virtual bool connetcion_established() {return gps_communication_ok;};
	virtual bool get_basic_info() {return false;};
	virtual bool get_memory_info() {return false;};
	virtual bool erase_memory() {return false;};
	virtual void turn_off() {;};
	virtual bool send_map_chunk(char*,int) {return false;};
	virtual bool finalize_upload() {return false;};
	virtual bool set_speed(int) {return true;};
	virtual bool set_async() {return true;};

	virtual bool open_com(const char*) {return true;};
	virtual void close_com() {;};
	virtual int  get_preffered_chunk_size() {return 0xf00;};

	virtual bool request_file(char* file_name,char* tmp_file,internal_file* i_file,int &total_size,int &position,bool buffer_data = false,int subfile_start=-1);
	virtual bool request_file_packet(char*) {return false;};
};

#endif
