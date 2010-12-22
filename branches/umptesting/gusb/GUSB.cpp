/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/
// GUSB.cpp : Defines the entry point for the console application.
//
/*
#include <iostream>
#include <stdio.h>
#include "usb_gps.h"
#include "com_gps.h"

int main(int argc, char* argv[])
{
	char buffer[0x1000];
	FILE* stream;
	int read;
	bool error = false;
	float file_size;
	float position = 0;
	int percent = 0;

	if( argc != 2 ) {
		cout << "GUSB file.img"<<endl;
		return 2;
	}

	//USB_interface gps;
	transmission_gps* gps;
	gps = new USB_interface();
	if( gps->connetcion_established() == false ) {
		cout<<"No USB GPS found or sendmap driver is not installed"<<endl;
		delete gps;
	
		gps = new COM_interface();
	}
	//com specyfic
	gps->open_com("COM2");

	//if( gps->connetcion_established() == false )
	//	cout<<"Cannot establish connection"<<endl;
	{
		if( gps->get_basic_info() ) {
			stream = fopen(argv[1],"rb");
			if( stream ) {
				fseek(stream,16,SEEK_SET);
				fread(buffer,1,0x6,stream);
				buffer[6] = 0;
				if( strcmp(buffer,"DSKIMG") != 0) {
					fclose(stream);
					cout<<"Not a valid IMG file"<<endl;
					return 4;
				}

				fseek(stream,0,SEEK_END);
				file_size = (float)ftell(stream);
				fseek(stream,0,SEEK_SET);

				gps->get_memory_info();
				cout << "Detected          : " << gps->GPS_version << endl;
				cout << "Available memory  : " << gps->GPS_memory << endl;
				cout << "Max number of maps: " << gps->GPS_max_maps << endl;

				gps->set_async();
				if( gps->set_speed(115200) != true ) {
					cout<<"\rCannot change speed, trying to use default speed"<<endl;
					if( gps->get_basic_info() != true )
						error = true;
				}
				if( error == false && gps->erase_memory() ) {
					//send map
					cout<<"\r0%";
					read = int(fread(buffer,1,gps->get_preffered_chunk_size(),stream));
					while( read == gps->get_preffered_chunk_size() ) {
						if( gps->send_map_chunk(buffer,read) == false ) {
							error = true;
							break;
						}
						position += read;
						if( int((position / file_size) * 100.0) != percent ) {
							percent = int((position / file_size)*100.0);
							cout<<"\r"<<percent<<"%"<<flush;
						}
						read = int(fread(buffer,1,gps->get_preffered_chunk_size(),stream));
					}

					if( !error ) {
						if( read > 0 ) {
							if( read % 2 )
								read++;
							if( gps->send_map_chunk(buffer,read) != true )
								error = true;
							cout<<"\r100%"<<flush;
						}
					}

					if( error != true ) {
						if( gps->finalize_upload() )
							cout<<"\rMap uploaded!"<<endl;
					} else {
						cout<<"\rMap uploade failed!"<<endl;
					}
					gps->set_speed(9600);
					gps->get_basic_info();
				} else
					cout<<"Cannot start upload map.."<<endl;

				fclose(stream);
			}
		}
		delete gps;
		return 0;
	}
	
	delete gps;
	return 1;
}
*/