/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

#ifndef __GUSBGPSH
#define __GUSBGPSH

#ifdef _WIN32
#include <windows.h>
#include <setupapi.h>

#undef USB_ENDPOINT_TYPE_CONTROL
#undef USB_ENDPOINT_TYPE_ISOCHRONOUS
#undef USB_ENDPOINT_TYPE_BULK
#undef USB_ENDPOINT_TYPE_INTERRUPT

#include "./DDK/usb200.h"

#endif

#include <iostream>
#include "gps.h"

using namespace std;

class GUSB_interface : public transmission_gps {
private:
#ifdef _WIN32
	HDEVINFO hInfo;
	char	Device[MAX_PATH]; 
	HANDLE	gps;
#endif

#ifdef LINUX
	void*	gps;
#endif

	int		overwritte_flash;
	bool	bulk_in;
	int		bulk_out_size;

	int configuration, interf, altsetting;

	//pipes for comunication
	int	request_endpoint;
	int	retreive_endpoint;
	int	retreive_interrupt_endpoint;
	int	retreive_bulk_endpoint;

	bool	read_data_usb(int timeout=150);
	int	read_data_usb_internal(int timeout);
	int	send_data_usb_internal(char* command,int size,unsigned long &read_data);

protected:
	int	get_command_index() {return 4;};
	int	get_data_index() {return 12;};
	int	get_length_index() {return 8;};

	bool	initiate_transmission();
	int		read_data();
	int		parse_message();
public:
	GUSB_interface(bool silent = false,int overwritte_flash = 0);
	virtual ~GUSB_interface();

	bool open_com(const char*);
	void close_com();

	bool connetcion_established();

	bool get_basic_info();
	bool get_memory_info();
	bool erase_memory();
	void turn_off();
	bool set_async();

	bool send_map_chunk(char* data,int size);
	bool finalize_upload();

	bool request_file_packet(char* file_name);
};


#endif
