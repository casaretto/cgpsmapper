/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/
#ifndef __SIMGPSH
#define __SIMGPSH

#include "gps.h"
#include <stdio.h>

using namespace std;

class SIM_interface : public transmission_gps {
protected:
	int	get_command_index() {return 0;};
	int	get_data_index() {return 0;};
	int	get_length_index() {return 0;};
	FILE* out_file;
public:
	SIM_interface(bool silent = false);

	bool get_basic_info() {return true;};
	bool get_memory_info() {return true;};
	bool erase_memory() {return true;};
	bool set_speed(int) {return true;};
	bool set_async() {return true;};
	bool finalize_upload() {return true;};

	bool send_map_chunk(char*,int);
	bool open_com(const char*);
	void close_com();
};

#endif

