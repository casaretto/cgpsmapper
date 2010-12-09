/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

#ifndef __USBGPSH
#define __USBGPSH

#ifndef LINUX
#include <windows.h>
#endif

#include <iostream>
#include "gps.h"

namespace usb {
#include "usb.h"

using namespace std;

class USB_interface : public transmission_gps {
private:
	struct usb_bus *busses;
	struct usb_dev_handle* gps;
	int configuration, interf, altsetting;

	//pipes for comunication
	int		request_endpoint;
	int		retreive_endpoint;
	int		retreive_interrupt_endpoint;
	int		retreive_bulk_endpoint;

	bool	read_data_usb(int timeout = 5000);

protected:
	int	get_command_index() {return 4;};
	int	get_data_index() {return 12;};
	int	get_length_index() {return 8;};

public:
	USB_interface(bool silent = false);
	virtual ~USB_interface();

	bool connetcion_established();

	bool get_basic_info();
	bool get_memory_info();
	bool erase_memory();
	void turn_off();

	bool send_map_chunk(char* data,int size);
	bool finalize_upload();
};
};
#endif