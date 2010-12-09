/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/
#include "sim_gps.h"
#include "./../tinyxml/tinyxml.h"
#include <limits.h>
#include <vector>
#include <string>

SIM_interface::SIM_interface(bool silent)
: transmission_gps(silent) {

	got_protocol = true;
	got_memory = true;
	got_memory_erased = true;
	flash_id = 0x0a;
	GPS_map_capable = true;
	GPS_routable = true;
	GPS_unlock_capable = true;

	gps_found = true;
	gps_communication_ok = true;
	got_speed_change = true;

	GPS_id = 0;
	comm_medium = resSIM;
	error_code = errNo;

	strcpy(GPS_version,"Storage GPS");
	GPS_software = 0;
	GPS_memory = 2 * 1024 * 1024 * 1024;
	GPS_max_maps = 99999;
}

bool SIM_interface::send_map_chunk(char* buffer,int size) {
	fseek(out_file,map_address,SEEK_SET);
	if( fwrite(buffer,1,size,out_file) != size )
		return false;
	map_address += size;
	return true;
}

bool SIM_interface::open_com(const char* file_name) {
	char t_drives[1024];
	char t_supplementalMaps[1024];
	bool t_result = false;
	bool t_gps_found = false;
	bool t_usb_drive = false;
	std::string t_local_file_name;
	std::string t_local_drive_name;
	std::string t_local_drive_directory;
	std::vector<std::string>	t_garmin_drive;

#ifdef _WIN32
	ULARGE_INTEGER lpFreeBytesAvailable,lpTotalNumberOfBytes,lpTotalNumberOfFreeBytes;

	strcpy(t_supplementalMaps,"GARMIN");

	if( string(file_name) == "GMAPSUPP.IMG" || string(file_name) == "CHECKCONNECTION") {
		HANDLE hTmpDevice;
		if( GetLogicalDriveStrings(1024,t_drives) ) {
			int t_pos = 0;		
			char	t_temp_drive[30];
			//konczy sie dwoma zerami
			while( t_drives[t_pos] != 0 ) {
				strncpy(t_temp_drive,&t_drives[t_pos],30);

				if( GetDriveType(t_temp_drive) == 2 ) {
					if( GetVolumeInformation(t_temp_drive, NULL, NULL, NULL, NULL, NULL, NULL, NULL) ) {
					//hTmpDevice = CreateFile(t_temp_drive, 0, FILE_SHARE_READ , NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS , NULL);
					//if( hTmpDevice != INVALID_HANDLE_VALUE ) {
						t_garmin_drive.push_back(std::string(t_temp_drive));
						t_local_drive_name = std::string(t_temp_drive)  + "GARMIN\\" + string(file_name);
						t_usb_drive = true;

					//	CloseHandle(hTmpDevice);
					}
				}
				t_pos+= string(t_temp_drive).size()+1;
			}
		}

		if( t_garmin_drive.size() ) {
			/*		
			1. dla kazdego - otworzyc plik konfiguracyjny -> GarminDevice.xml
			2. jesli znaleziono - ustawic jako domyslny i przeczytac:
			gps_id
			3. jesli nie znaleziono - ustawic 'pierwszy'
			*/
			for( std::vector<string>::iterator t_g = t_garmin_drive.begin(); t_g != t_garmin_drive.end(); t_g++ ) {
				t_local_file_name = *t_g;
				t_local_file_name += "GARMIN\\GarminDevice.xml";

				if( !GetDiskFreeSpaceEx((*t_g).c_str(),&lpFreeBytesAvailable,&lpTotalNumberOfBytes,&lpTotalNumberOfFreeBytes ) )
					continue;

				GPS_memory = (unsigned long)lpFreeBytesAvailable.QuadPart;

				TiXmlDocument doc(t_local_file_name.c_str());
				if( doc.LoadFile() ) {
					char t_buff[20];
					char t_storageName[1024];
					
					TiXmlHandle hDoc(&doc);
					TiXmlHandle hRoot(0);
					TiXmlHandle hStorage(0);
					TiXmlElement* pElem;
					int	i_storage = 0;
					pElem = hDoc.FirstChildElement().Element();
					hRoot=TiXmlHandle(pElem);

					pElem = hRoot.FirstChild("Model").FirstChild("Description").Element();
					strncpy(GPS_version,pElem->GetText(),250);

					pElem = hRoot.FirstChild("Id").Element();
					strncpy(t_buff,pElem->GetText(),19);
					unsigned long t = strtoul(t_buff,NULL,10);
					//cout<<t;
					GPS_id = t;

					//supplemental maps
					hStorage = hRoot.FirstChild("MassStorageMode");
					while( true ) {
						pElem = hStorage.Child(i_storage).FirstChild("Name").Element();
						if( pElem ) {
							strncpy(t_storageName,pElem->GetText(),1000);
							for(int i = 0; i<strlen(t_storageName); ++i)
								t_storageName[i] = toupper(t_storageName[i]);
							
							if( !strcmp(t_storageName,"SUPPLEMENTALMAPS") ) {
								pElem = hStorage.Child(i_storage).FirstChild("File").FirstChild("Location").FirstChild("Path").Element();
								strncpy(t_supplementalMaps,pElem->GetText(),1000);
								break;
							}
						} else 
							break;
						i_storage++;
					}

					t_gps_found = true;

					/*
					HANDLE t_file = CreateFile(t_local_file_name.c_str(),GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
					if( t_file != INVALID_HANDLE_VALUE
					*/
					/*
					out_file = fopen(t_local_file_name.c_str(),"r+b");
					if( out_file != NULL ) {
					*/
					//OK - znaleziono!
					t_local_drive_name = *t_g + t_supplementalMaps;
					t_local_drive_directory = t_local_drive_name;

					t_local_drive_name += "\\";
					t_local_drive_name += string(file_name);
					t_result = true;
					break;

					//fclose(out_file);
				}

			}
		} else
			t_local_drive_name = file_name;
	} else
		t_local_drive_name = file_name; //nazwa to nie 'gmapsupp.img'
#else
	t_local_drive_name = file_name;
#endif

	map_address = 0;
	//return t_result;

	out_file = NULL;
	if( string(file_name) == "CHECKCONNECTION" )
		return true;

	//check directory if exist
	CreateDirectory(t_local_drive_directory.c_str(),NULL);

	out_file = fopen(t_local_drive_name.c_str(),"w+b");
	if(out_file == NULL ) {
		if( t_gps_found )
			cout<<"GPS found but upload is not possible."<<endl;
		else if(t_usb_drive) 
			cout<<"USB storage found but no GARMIN directory. Create manually GARMIN directory."<<endl;
		return false;
	}
	return true;
}

void SIM_interface::close_com() {
	if( out_file != NULL )
		fclose(out_file);
	out_file = NULL;
}

