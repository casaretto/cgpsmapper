/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/
#include "usb_gps.h"
#define TRACE 0


char r_get_id[]				={0x00,0x00,0x00,0x00,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
char r_get_packet_size[]	={0x00,0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
char r_get_info[]			={0x14,0x00,0x00,0x00,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
char r_async_set[]			={0x14,0x00,0x00,0x00,0x1c,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00}; //disable all
char r_get_flash_id[]		={0x14,0x00,0x00,0x00,0x0a,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x3f,0x00};
char r_erase_memory[]		={0x14,0x00,0x00,0x00,0x4b,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x0a,0x00};//0x000a flash desc
char r_send_map[]			={0x14,0x00,0x00,0x00,0x24,0x00,0x00,0x00};
char r_finalize_map[]		={0x14,0x00,0x00,0x00,0x2d,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x0a,0x00};
char r_poweroff_gps[]		={0x14,0x00,0x00,0x00,0x0a,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x08,0x00};

using namespace usb;

USB_interface::USB_interface(bool silent)
  : transmission_gps(silent) {
	comm_medium = resUSB;
	request_endpoint=-1;
	retreive_bulk_endpoint=-1;
	retreive_interrupt_endpoint=-1;
	gps = NULL;
	GPS_packet_size = 0x1000;

	//show debug on screen - later to comment out
	usb_set_debug(0);

	usb_init();
	usb_find_busses();
	usb_find_devices();
	busses = usb_get_busses();

	struct usb_bus *bus;

	for (bus = busses; bus; bus = bus->next) {
		struct usb_device *dev;
		unsigned char dir;

		for (dev = bus->devices; dev; dev = dev->next) {
			if (dev->descriptor.idVendor == 0x091e && dev->descriptor.idProduct == 0x0003) {      // Garmin GPS
				gps_found = true;

				//char message[512];
				//cout<<"Found GPS with USB "<<dev->descriptor.idVendor<<"&"<< dev->descriptor.idProduct<<endl;

				gps = usb_open(dev);

				/* Walk through the possible configs, etc. */

				//printf("This device has %d possible configuration(s).\n", dev->descriptor.bNumConfigurations);
				for (int c = 0; c < dev->descriptor.bNumConfigurations; c++)
				{
					//printf("Looking at configuration %d...This configuration has %d interfaces.\n", c, dev->config[c].bNumInterfaces);
					for(int i=0; i<dev->config[c].bNumInterfaces; i++)
					{
						//printf("  Looking at interface %d...This interface has %d altsettings.\n", i, dev->config[c].interface[i].num_altsetting);
						for (int a=0; a < dev->config[c].interface[i].num_altsetting; a++)
						{
							//printf("    Looking at altsetting %d...This altsetting has %d endpoints.\n", a, dev->config[c].interface[i].altsetting[a].bNumEndpoints);
							for (int e=0; e < dev->config[c].interface[i].altsetting[a].bNumEndpoints; e++)
							{
								//printf("      Endpoint %d: Address %02xh, attributes %02xh ", e, dev->config[c].interface[i].altsetting[a].endpoint[e].bEndpointAddress, dev->config[c].interface[i].altsetting[a].endpoint[e].bmAttributes);
								char attribs = (dev->config[c].interface[i].altsetting[a].endpoint[e].bmAttributes & 3);
								switch (attribs)
								{
								case 0: //printf("(Control) Ignore this: ");
									break;
								case 1: //printf("(Isochronous) ");
									break;
								case 2: //printf("(Bulk) ");

									configuration = c;
									interf = i;
									altsetting = a;
									break;
								case 3: //printf("(Interrupt) ");
									//
									break;
								default: ;//printf("ERROR! Got an illegal value in endpoint bmAttributes\n");
								}

								dir = (dev->config[c].interface[i].altsetting[a].endpoint[e].bEndpointAddress & 0x80);
								switch (dir)
								{
								case 0x00: //printf("(Out)\n");
									// Do nothing in this case
									//request_endpoint=0x02;
									if ((dev->config[c].interface[i].altsetting[a].endpoint[e].bmAttributes & 0x03) == 2)
									{
										// Found the correct endpoint to use for bulk transfer; use its ADDRESS
										request_endpoint = dev->config[c].interface[i].altsetting[a].endpoint[e].bEndpointAddress;
									}
									break;

								case 0x80: //printf("(In)\n");
									//retreive_endpoint=0x81;
									if ((dev->config[c].interface[i].altsetting[a].endpoint[e].bmAttributes & 0x03) == USB_ENDPOINT_TYPE_BULK)
									{
										// Found the correct endpoint to use for bulk transfer; use its ADDRESS
										retreive_bulk_endpoint = dev->config[c].interface[i].altsetting[a].endpoint[e].bEndpointAddress;
									} else
										retreive_interrupt_endpoint = dev->config[c].interface[i].altsetting[a].endpoint[e].bEndpointAddress;
									break;
								default: ;//printf("ERROR! Got an illegal value in endpoint bEndpointAddress\n");
								}
							}
						}
					}
				}
			}
		}
	}

	if( gps_found == false )
		return;
#if TRACE == 1
	cout<<"INTERRUPT : "<<retreive_interrupt_endpoint<<endl;
	cout<<"BULK : "<<retreive_bulk_endpoint<<endl;
	cout<<"REQUEST : "<<request_endpoint<<endl;
#endif

	if( retreive_interrupt_endpoint > -1 && request_endpoint > -1 && retreive_bulk_endpoint > -1 )
		gps_communication_ok = true;

	if( usb_set_configuration(gps, configuration+1) < 0 )
		gps_communication_ok = false;
	if( usb_claim_interface(gps, interf) < 0 )
		gps_communication_ok = false;

	if( gps_communication_ok == false && gps != NULL ) {
		if( usb_set_configuration(gps, 0) < 0 && silent == false)
			cout<<"Closing USB without success?"<<endl;
		if( usb_reset(gps) < 0 && silent == false )
			cout<<"Cannot close USB gracefully"<<endl;
		usb_close(gps);
	}

	//let start with interrupt interface
	retreive_endpoint = retreive_interrupt_endpoint;
}

USB_interface::~USB_interface() {

	try {
		if( gps != NULL ) {
			if( usb_set_configuration(gps, 0) < 0 && silent == false )
				cout<<"Closing USB without success?"<<endl;
			if( usb_reset(gps) < 0 && silent == false )
				cout<<"Cannot close USB gracefully"<<endl;
		}
		usb_close(gps);
		gps_communication_ok = false;
	} catch(...) {
		gps_communication_ok = false;
	}
}

bool USB_interface::connetcion_established() {
	return gps_communication_ok;
}


bool USB_interface::finalize_upload() {
	if( usb_bulk_write(gps, request_endpoint, r_finalize_map, 14, 100 * 60 * 50) < 0 )
		return false;
	//return get_basic_info();
	return true;
}

bool USB_interface::send_map_chunk(char* data,int size) {
	if( size > 0x1000 - 16 )
		return false;
	memcpy(send_buffer,r_send_map,8);
	//9-12 - rozmiar
	int_conv.value = size + 4;
	memcpy(&(send_buffer[8]),int_conv.value_c,4);
	//13-16 - adres docelowy
	int_conv.value = map_address;
	memcpy(&(send_buffer[12]),int_conv.value_c,4);
	map_address += size;
	
	memcpy(&(send_buffer[16]),data,size);
	if( usb_bulk_write(gps, request_endpoint, send_buffer, size + 16, 100 * 60 * 50) < 0 )
		return false;	
	return true;
}

bool USB_interface::read_data_usb(int timeout) {
	bool bulk_first_read = false;
	int read;
	int to_read = 0;
	int read_data = 0x40;
	received_length = 0;
	buffer_pos = 0;

	/* I know - goto should never ever be in use, but this situation is 
	example of a good exception */
redoit:
	read = usb_interrupt_read(gps, retreive_endpoint, buffer, read_data, timeout);
	if( read == 0 && bulk_first_read == true ) {
		//just after swithing to BULK IN looks like GPS can return 0 (what normally mean
		// - switch back to INTERRUPT IN - but that wouldn't make sense) - so
		//let us make another try of reading
#if TRACE == 1
		cout<<"FIRST BULK READ EMPTY ";
#endif
		read = usb_interrupt_read(gps, retreive_endpoint, buffer, read_data, timeout);
	}
#if TRACE == 1
	cout<<"IN : "<<retreive_endpoint<<endl;
	cout<<"read : "<<read<<endl;
#endif
	bulk_first_read = false;

	if( retreive_endpoint == retreive_bulk_endpoint && read == 0) {
		//check if no more BULK IN
		//switch back to INTERRUPT
		retreive_endpoint = retreive_interrupt_endpoint;
#if TRACE == 1
		cout<<"Back to interrupt IN : "<<retreive_endpoint<<endl;
#endif
		goto redoit;
	} 

	if( read > 0 ) {
#if TRACE == 1
		cout<<"analising : 0 and 4 : "<<int(buffer[0])<<" "<<int(buffer[4])<<endl;
#endif
		//BULK IN swith
		if( buffer[4] == 2 && buffer[0] == 0 ) {
			bulk_first_read = true;
			retreive_endpoint = retreive_bulk_endpoint;
			goto redoit;
		}

		to_read = convert_to_int(8);
		received_length = to_read;
		to_read += 4*3; //to_read does not include header
		to_read -= read;
		buffer_pos = read;

		while(to_read>0) {
			read = usb_interrupt_read(gps, retreive_endpoint, &(buffer[buffer_pos]), read_data, 5000);
			buffer_pos += read;
			to_read -= read;
		}
		received_length += get_data_index();
		return true;
	}
	return false;
}

bool USB_interface::get_memory_info() {
	int read = 0;
	int read_data = 0x40;
	int tries = 0;
	flash_id = 0x0a;
	GPS_memory = 0;

	if( usb_bulk_write(gps, request_endpoint, r_async_set, 12, 5000) < 0 )
		return false;
	
	if( usb_bulk_write(gps, request_endpoint, r_get_flash_id, 14, 5000) >= 0 ) {
		tries = 0;
		while( tries < 50 ) {
			if( read_data_usb() == true )
				if( parse_message() == 0x5f )
					break;
			tries++;					
			delay(150);
		}
		return got_memory;
	} else
		return false;
	return true;
}

bool USB_interface::erase_memory() {
	int tries = 0;
	map_address = 0;

	//erase memory here
	if( usb_bulk_write(gps, request_endpoint, r_erase_memory, 14, 5000) >= 0 ) {
		tries = 0;
		while( tries < 100 ) {
			if( read_data_usb() == true )
				if( parse_message() == 0x4a )
					return true;
			delay(150);
			if( silent == false )
				cout<<".";
			tries++;
		}
	}
	return got_memory_erased;
}

void USB_interface::turn_off() {
	usb_bulk_write(gps, request_endpoint, r_poweroff_gps, 14, 5000);
}

bool USB_interface::get_basic_info() {
	bool	result = false;
	unsigned long read_data = 0x40;

	if( usb_bulk_write(gps, request_endpoint, r_get_id, 12, 5000) > 0 ) {
		if( read_data_usb() == false )
			return false;
		parse_message();
		result = true;
	}

	if( usb_bulk_write(gps, request_endpoint, r_get_packet_size, 12, 5000) > 0 ) {
		if( read_data_usb() == false )
			return false;
		parse_message();
	}

	//GPS informations
	buffer_pos = 0;
	if( usb_bulk_write(gps, request_endpoint, r_get_info, 12, 5000) > 0 ) {
		if( read_data_usb() == false )
			return false;
		parse_message();
	}
	//14 - version
	//16 - string
	strncpy(GPS_version,&(buffer[16]),255);

	buffer_pos = 0;
	//...and protocol array
	if( read_data_usb() == true )
		parse_message();

	if( read_data_usb(60*10) == true )
		parse_message();

	return result;
}