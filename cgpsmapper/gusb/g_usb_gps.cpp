/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/
#include "g_usb_gps.h"
#ifdef _WIN32
#include <initguid.h>
#endif

#include <string.h>

#define TRACE 0
#ifdef _WIN32
DEFINE_GUID(GUID_DEVINTERFACE_GRMNUSB, 0x2c9c45c2L, 0x8e7d, 0x4c08, 0xa1, 0x2d, 0x81, 0x6b, 0xba, 0xe7, 0x22, 0xc0);

#define GARMIN_USB_API_VERSION 1 
#define GARMIN_USB_MAX_BUFFER_SIZE 4096 
#define GARMIN_USB_INTERRUPT_DATA_SIZE 64

#define IOCTL_GARMIN_USB_API_VERSION CTL_CODE (FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_GARMIN_USB_INTERRUPT_IN CTL_CODE (FILE_DEVICE_UNKNOWN, 0x850, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_GARMIN_USB_BULK_OUT_PACKET_SIZE CTL_CODE (FILE_DEVICE_UNKNOWN, 0x851, METHOD_BUFFERED, FILE_ANY_ACCESS)
#endif

char gr_get_id[]			={0x00,0x00,0x00,0x00,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
char gr_get_packet_size[]	={0x00,0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
char gr_get_info[]			={0x14,0x00,0x00,0x00,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
char gr_async_set[]			={0x14,0x00,0x00,0x00,0x1c,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00}; //disable all
char gr_get_flash_id[]		={0x14,0x00,0x00,0x00,0x0a,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x3f,0x00};
char gr_erase_memory[]		={0x14,0x00,0x00,0x00,0x4b,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x0a,0x00};//0x000a flash desc
char gr_send_map[]			={0x14,0x00,0x00,0x00,0x24,0x00,0x00,0x00};
char gr_finalize_map[]		={0x14,0x00,0x00,0x00,0x2d,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x0a,0x00};
char gr_get_serial_gps[]	={0x14,0x00,0x00,0x00,0x0a,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x0e,0x00};
char gr_poweroff_gps[]		={0x14,0x00,0x00,0x00,0x0a,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x08,0x00};
char gr_get_file[]			={0x14,0x00,0x00,0x00,0x59,0x00,0x00,0x00,0x13,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0a,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

//unsigned char		mapflash[] ={ "\x0a\x02\x3f\x00" };		// MapFlash description...
//unsigned char		erasemap[] ={ "\x4b\x02\x00\x00" };		// erase
//unsigned char		getfile[] = { "\x59\x13\ x00\x00\x00\x00\xFF\xFF\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" };

GUSB_interface::GUSB_interface(bool silent,int _overwritte_flash)
  : transmission_gps(silent), overwritte_flash(_overwritte_flash) {
	gps = NULL;
	comm_medium = resUSB;
	bulk_in = false;

	if( overwritte_flash > 0 ) {
		gr_erase_memory[12] = overwritte_flash;
		gr_finalize_map[12] = overwritte_flash;
		gr_get_file[16] = overwritte_flash;
	}
}

GUSB_interface::~GUSB_interface() {
#ifdef _WIN32
	if( gps )
		CloseHandle(gps);
#endif
}

bool GUSB_interface::open_com(const char*) {
	unsigned long read_data = 0x0;
#ifdef _WIN32
	HDEVINFO hInfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_GRMNUSB, NULL, NULL, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);

	SP_INTERFACE_DEVICE_DATA Interface_Info;
	Interface_Info.cbSize = sizeof(Interface_Info);
	// Enumerate device
	if (!SetupDiEnumInterfaceDevice(hInfo, NULL, (LPGUID)&GUID_DEVINTERFACE_GRMNUSB,0, &Interface_Info)) {
		SetupDiDestroyDeviceInfoList(hInfo);
		gps_found = false;
		return false;
	}
	DWORD needed; // get the required lenght
	SetupDiGetInterfaceDeviceDetail(hInfo, &Interface_Info, NULL, 0, &needed, NULL);
	PSP_INTERFACE_DEVICE_DETAIL_DATA detail = (PSP_INTERFACE_DEVICE_DETAIL_DATA) malloc(needed);
	if (!detail) {
		SetupDiDestroyDeviceInfoList(hInfo);
		gps_found = false;
		return false;
	}

	// fill the device details
	detail->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);
	if (!SetupDiGetInterfaceDeviceDetail(hInfo,	&Interface_Info,detail, needed,NULL, NULL)) {
		free((PVOID) detail);
		SetupDiDestroyDeviceInfoList(hInfo);
		gps_found = false;
		return false;
	}

	strncpy(Device, detail->DevicePath, sizeof(Device));
	free((PVOID) detail);
	//Device = name; // keep a copy of each device name
	
	gps = CreateFile(Device, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0/*FILE_FLAG_OVERLAPPED*/, NULL);
	if( gps == NULL ) {
		gps_found = false;
		return false;
	}

	if( DeviceIoControl(gps, IOCTL_GARMIN_USB_API_VERSION,	NULL, 0, buffer, 4000, &read_data, NULL) ) {	
		gps_found = true;
		gps_communication_ok = true;
	}

	DeviceIoControl(gps, IOCTL_GARMIN_USB_BULK_OUT_PACKET_SIZE,	NULL, 0, &bulk_out_size, 4, &read_data, NULL);
	if( read_data == 2 ) {
		bulk_out_size = bulk_out_size & 0xffff;
	}
#endif
	bulk_in = false;
	return true;
}

void GUSB_interface::close_com() {
#ifdef _WIN32
	if( gps )
		CloseHandle(gps);
#endif
	gps = NULL;
}
int	GUSB_interface::parse_message() {
	int t_ret = transmission_gps::parse_message();
	if( overwritte_flash > 0 )
		flash_id = overwritte_flash;
	return t_ret;
}

bool GUSB_interface::connetcion_established() {
	return gps_communication_ok;
}

int	GUSB_interface::read_data() {
	if( read_data_usb() )
		return parse_message();
	return -1;
}

bool GUSB_interface::request_file_packet(char* file_name) {
	unsigned long	read_data = 0;

	for(int a = 18; a < 18 + 12; a++) gr_get_file[a] = 0;
	memcpy(&gr_get_file[18], file_name, strlen(file_name));

	if( send_data_usb_internal(gr_get_file, sizeof(gr_get_file), read_data) )
		return true;
	return false;
}

bool GUSB_interface::finalize_upload() {
	unsigned long read_data;
	if( send_data_usb_internal(gr_finalize_map, 14, read_data) < 0 )
		return false;
	return true;
}

bool GUSB_interface::send_map_chunk(char* data,int size) {
	unsigned long	read_data;
	if( size > 0x1000 - 16 )
		return false;
	memcpy(send_buffer,gr_send_map,8);
	//9-12 - rozmiar
	int_conv.value = size + 4;
	memcpy(&(send_buffer[8]),int_conv.value_c,4);
	//13-16 - adres docelowy
	int_conv.value = map_address;
	memcpy(&(send_buffer[12]),int_conv.value_c,4);
	map_address += size;
	
	memcpy(&(send_buffer[16]),data,size);
	if( !send_data_usb_internal(send_buffer, size + 16, read_data) )
		return false;	
	return true;
}

int GUSB_interface::send_data_usb_internal(char* command,int size,unsigned long &read_data) {
#if TRACE == 1
	cout<<"Sending: ";
	if( size > 5 ) {
		cout<<hex<<static_cast<unsigned short>(command[0])<<" "<<static_cast<unsigned short>(command[1])<<" "<<static_cast<unsigned short>(command[2])<<" "<<static_cast<unsigned short>(command[3])<<" "<<static_cast<unsigned short>(command[4])<<" " <<dec;
	} else
		cout<<"command";
	cout<<endl;
#endif

#ifdef _WIN32
	//bulk_in = false;
	return WriteFile(gps,command,size,&read_data, NULL);
#else
	return 0;
#endif
}

int GUSB_interface::read_data_usb_internal(int timeout) {
#ifdef _WIN32
	unsigned long	read_data = 0x0;
	bool			read_result = false;

redoit:
	if( bulk_in == false ) {
#if TRACE == 1
		cout<<"BULK in"<<endl;
#endif
		if( DeviceIoControl(gps, IOCTL_GARMIN_USB_INTERRUPT_IN,NULL, 0, &buffer[buffer_pos], GARMIN_USB_MAX_BUFFER_SIZE, &read_data, NULL) )
			read_result = true;
		
		if( read_result && buffer[4] == 2 && buffer[0] == 0 ) {
#if TRACE == 1
			cout<<"Switching to BULK IN interface (not existing in 60x)"<<endl;
#endif
			bulk_in = true;
			goto redoit;
		}
#if TRACE == 1
		cout<<"BULK in data read: "<<read_data<<endl;
#endif
	} else {
#if TRACE == 1
		cout<<"INTERRUPT in"<<endl;
#endif
		ReadFile(gps,&buffer[buffer_pos],GARMIN_USB_MAX_BUFFER_SIZE,&read_data,NULL);

		if( read_data == 0 && timeout != 0 /* no switch DURING ONE packet!! */) {
			//bulk_in = false;
#if TRACE == 1
			cout<<"Switching back to INTERRUPT IN interface "<<endl;
#endif
		} else {
			read_result = true;
#if TRACE == 1
			cout<<"INTERRUPT in data read: "<<read_data<<endl;
#endif
		}
		delay(timeout);
	}
	if( read_result ) {
#if TRACE == 1
		for( int a = 0; a < (read_data + buffer_pos); a++ ) {
			cout<<hex<<static_cast<unsigned short>(buffer[a])<<" ";
		}
		cout<<dec<<endl;
#endif
		return read_data;
	}
#endif
	return 0;
}

bool GUSB_interface::read_data_usb(int timeout) {
	int to_read = 0;
	int read;

	buffer_pos = 0;
	read = read_data_usb_internal(timeout);
	if( read ) {

		to_read = convert_to_int(8);
		received_length = to_read;
		to_read += 4*3; //to_read does not include header
		to_read -= read;
		buffer_pos = read;

		while(to_read>0) {
			read = read_data_usb_internal(0);
			buffer_pos += read;
			to_read -= read;

			if( buffer_pos > sizeof(buffer) ) {
				return false;
			}
		}
		received_length += get_data_index();
		return true;	
	}
	//never here
	bulk_in = false;
	return false;
}

bool GUSB_interface::initiate_transmission() {
	unsigned long read_data = 0x0;
	static bool	initiated = false;

	//quit only if first time initialisation fail
	if( send_data_usb_internal(gr_get_id, 12, read_data) ) {
		if( read_data_usb() == false ) {
			if( initiated == false )
				return false;
		} else
			parse_message();
	}

	if( send_data_usb_internal(gr_get_packet_size, 12, read_data) ) {
		if( read_data_usb() != false )
			parse_message();
	}
	initiated = true;
	return true;
}

bool GUSB_interface::set_async() {
	unsigned long read_data = 0x0;
	send_data_usb_internal(gr_async_set, 14, read_data);
	return true;
}

bool GUSB_interface::get_memory_info() {
	int		read = 0;
	unsigned long read_data = 0x0;
	int		tries = 0;

	if( overwritte_flash > 0 )
		flash_id = overwritte_flash;
	else
		flash_id = 0x0a; //0x31
	GPS_memory = 0;

#if TRACE == 1
	cout<<"Preparing for upload...."<<endl;
#endif

	bulk_in = false;
	if( initiate_transmission() == false )
		return false;

	//get_basic_info();
	
	buffer_pos = 0;
	if( send_data_usb_internal(gr_get_info, sizeof(gr_get_info), read_data) ) {
		if( read_data_usb() == false )
			return false;
		parse_message();

		tries = 0;
		while( tries < 5 ) {
			if( read_data_usb() == true )
				parse_message();
			else
				break;
			delay(150);
			tries++;
		}
	}

	tries = 0;
	//this will read all additional data...
	send_data_usb_internal(gr_async_set, 14, read_data);
	if( send_data_usb_internal(gr_get_flash_id, 14, read_data) ) {
		tries = 0;
//		bulk_in = true;
		while( tries < 5 ) {
			if( read_data_usb() == true )
				if( parse_message() == 0x5f ) {
#if TRACE == 1
					cout<<"Memory info got.."<<endl;
#endif
					break;
				}
			tries++;					
			delay(150);
		}
		return got_memory;
	} else
		return false;
	send_data_usb_internal(NULL,0,read_data);
	return true;
}

bool GUSB_interface::erase_memory() {
	int tries = 0;
	unsigned long read_data;
	map_address = 0;

	//erase memory here
	if( send_data_usb_internal(gr_erase_memory, sizeof(gr_erase_memory), read_data) ) {
		tries = 0;
		//bulk_in = true;
		while( tries < 100 ) {
			delay(150);
			read_data_usb();
			parse_message();
			if( got_memory_erased == true )
				return true;
			delay(150);
#ifndef _DLL_VERSION
			if( silent == false )
				cout<<".";
#endif
			tries++;
		}
	}
	return got_memory_erased;
}

void GUSB_interface::turn_off() {
	unsigned long read_data = 0;

	send_data_usb_internal(gr_poweroff_gps, sizeof(gr_poweroff_gps), read_data);
}

bool GUSB_interface::get_basic_info() {
	int		tries = 0;
	unsigned long read_data = 0x40;
#if TRACE == 1
	cout<<"Trying to get ID from GPS - if this fail - then we know that at least we open the connection but...."<<endl;
#endif

	GPS_version[0] = 0;
	bulk_in = false;
	if( initiate_transmission() == false )
		return false;
	//return true;

	delay(150);

	//GPS informations
	buffer_pos = 0;
	if( send_data_usb_internal(gr_get_info, sizeof(gr_get_info), read_data) ) {
		if( read_data_usb() == false )
			return false;
		parse_message();
	}
	if( !strlen(GPS_version) )
		strcpy(GPS_version,"USB GPS");
	//14 - version
	//16 - string
	//strncpy(GPS_version,&(buffer[16]),255);

	buffer_pos = 0;
	//...and protocol array

#if TRACE == 1
	cout<<"Protocol array received.."<<endl;
#endif
	if( read_data_usb() )		
		parse_message();
	
	//this will read ID..
	/*
	if( send_data_usb_internal(gr_get_serial_gps, sizeof(gr_get_serial_gps), read_data) ) {
		while( tries < 5 ) {
			if( read_data_usb() == true )
				if( parse_message() == 0x26 ) {
#if TRACE == 1
					cout<<"ID info got.."<<endl;
#endif
					break;
				}
			tries++;					
			delay(150);
		}
	}
	*/

	send_data_usb_internal(NULL,0,read_data);
	return true;
}


