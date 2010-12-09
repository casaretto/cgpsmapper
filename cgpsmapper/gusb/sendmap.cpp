/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/
#include <string>
#include <vector>
#ifdef _WIN23
#include <windows.h>
#include "resource.h"
#endif
#include "uploader.h"
#include "crccheck.h"
#include "gui_sendmap.h"
#include "exe_create.h"

using namespace std;

//#define DEBUG

#define TEMP_STR_LEN 40
char temp_key[TEMP_STR_LEN];

#pragma pack(push,1)
#if FULL == 1
t_sendmap sendmap_data = {{"USB_MAGIC"},0,0,0,0,0,0,0,0,0,0,0,"Sendmap20 Pro rev 6.7 by Stanislaw Kozicki - cgpsmapper@gmail.com"};
#else
t_sendmap sendmap_data = {{"USB_MAGIC"},0,0,0,0,0,0,0,0,0,1,0,"Sendmap20 rev 6.7 by Stanislaw Kozicki - cgpsmapper@gmail.com"};
#endif
char*	global_licence_file_txt;
char*	global_expiration_file_txt;

#pragma pack(pop)


#ifdef _DLL_VERSION
#ifndef _SENDMAP_LIB
#include "sendmapDLL.h"
#endif
#endif


/** 
 * calc crc value
 */
long crc_calc(char* buffer,int size,unsigned long gps_id) {
	long crc = 0;
	char id[4];
	memcpy(id,&gps_id,4);
	for( int a=0; a < size; ++a )
		crc = UPDC32(buffer[a], crc);
//cout<<crc<<endl;
	crc = UPDC32(id[0], crc);
	crc = UPDC32(id[1], crc);
	crc = UPDC32(id[2], crc);
	crc = UPDC32(id[3], crc);
	return crc;
}

/**
 * calculate crypt key using gps_id and input file
 */

int calculate_crypt_gps_id(char sendmap_exe[2024],unsigned int id,unsigned int &crypt_gps_id) {
	char* file_exe;
	FILE*	prog_file = fopen(sendmap_exe,"rb");
	file_exe = new char[sendmap_data.copyright_start_data];
	fseek(prog_file,0,SEEK_SET);
	if( fread(file_exe,1,sendmap_data.copyright_start_data,prog_file) != sendmap_data.copyright_start_data ) {
#ifdef _DEBUG
		cout<<"Fatal error checking integrity of the file!"<<endl;
#endif
		return 1;
	}
	fclose(prog_file);
	crypt_gps_id = crc_calc(file_exe,sendmap_data.copyright_start_data,id);
	delete []file_exe;
	return 0;
}

/*
	-tCOM_PORT or
	-tUSB

*/

#ifdef _WIN32
int CALLBACK DialogProcedureLic(HWND hwndDlg, UINT uMsg,WPARAM wParam, LPARAM lParam) {

	size_t text_len;// = strlen(global_licence_file_txt);
	size_t poz = 0;
	string	licence;
	string	global_yes;
	string	global_no;
	string	global_heading;

	switch (uMsg) {
		case WM_INITDIALOG:
			//prepare buttons
			text_len = strlen(global_licence_file_txt);
			licence = global_licence_file_txt;
			global_yes[0] = 0;
			global_no[0] = 0;

			while( poz < text_len ) {
				if(licence[poz] == 10) {
					if( global_heading.size() == 0 ) {
						global_heading = licence.substr(0,poz);
						if( poz ) {
							if( global_heading[global_heading.size()-1] < 32 )
								global_heading = global_heading.substr(0,global_heading.size()-1);
						}			
						licence = licence.substr(poz);
						if( licence.size() )
							if( licence[0] < 32 )
								licence = licence.substr(1);

						poz = 0;
					}else if( global_yes.size() == 0 ) {
						global_yes = licence.substr(0,poz);						
						licence = licence.substr(poz);
						if( poz ) {
							if( global_yes[global_yes.size()-1] < 32 )
								global_yes = global_yes.substr(0,global_yes.size()-1);
						}			
						if( licence.size() )
							if( licence[0] < 32 )
								licence = licence.substr(1);
						poz = 0;						
					} else if( global_no.size() == 0 ) {
						global_no = licence.substr(0,poz);						
						if( poz ) {
							if( global_no[global_no.size()-1] < 32 )
								global_no = global_no.substr(0,global_no.size()-1);
						}			
						licence = licence.substr(poz);
						if( licence.size() )
							if( licence[0] < 32 )
								licence = licence.substr(1);
						break;
					}
				}
				poz++;
			}
			
			SetWindowText(hwndDlg,global_heading.c_str());
			SetWindowText(GetDlgItem(hwndDlg,IDYES),global_yes.c_str());
			SetWindowText(GetDlgItem(hwndDlg,IDNO),global_no.c_str());
			SetWindowText(GetDlgItem(hwndDlg,IDC_RICHEDIT22),licence.c_str());
			break;
        case WM_COMMAND:
			switch(LOWORD(wParam)) {
			case IDYES:
				EndDialog(hwndDlg,IDYES);
				return IDYES;
			case IDNO:
				EndDialog(hwndDlg,IDNO);				
				return IDNO;
			}
			break;
	}
  return 0L;
}

int CALLBACK DialogProcedureExp(HWND hwndDlg, UINT uMsg,WPARAM wParam, LPARAM lParam) {

	size_t text_len;// = strlen(global_licence_file_txt);
	size_t poz = 0;
	string	licence;
	string	global_yes;
	string	global_heading;

	switch (uMsg) {
		case WM_INITDIALOG:
			//prepare buttons
			text_len = strlen(global_expiration_file_txt);
			licence = global_expiration_file_txt;
			global_yes[0] = 0;

			while( poz < text_len ) {
				if(licence[poz] == 10) {
					if( global_heading.size() == 0 ) {
						global_heading = licence.substr(0,poz);						
						licence = licence.substr(poz);
						if( poz ) {
							if( global_heading[global_heading.size()-1] < 32 )
								global_heading = global_heading.substr(0,global_heading.size()-1);
						}			
						if( licence.size() )
							if( licence[0] < 32 )
								licence = licence.substr(1);
						poz = 0;
					} else if(licence[poz] == 10) {
						global_yes = licence.substr(0,poz);						
						if( poz ) {
							if( global_yes[global_yes.size()-1] < 32 )
								global_yes = global_yes.substr(0,global_yes.size()-1);
						}			
						licence = licence.substr(poz);
						if( licence.size() )
							if( licence[0] < 32 )
								licence = licence.substr(1);
						break;
					}
				}
				poz++;
			}
			SetWindowText(hwndDlg,global_heading.c_str());
			SetWindowText(GetDlgItem(hwndDlg,IDYES),global_yes.c_str());
			//SetWindowText(GetDlgItem(hwndDlg,IDC_RICHEDIT21),licence.c_str());
			SetWindowText(GetDlgItem(hwndDlg,IDC_EDIT2),licence.c_str());
			break;
        case WM_COMMAND:
			switch(LOWORD(wParam)) {
			case IDYES:
				EndDialog(hwndDlg,IDYES);
				return IDYES;
			case IDNO:
				EndDialog(hwndDlg,IDNO);				
				return IDNO;
			}
			break;
	}
  return 0L;
}

int CALLBACK DialogProcedureInfo(HWND hwndDlg, UINT uMsg,WPARAM wParam, LPARAM lParam) {

	//int text_len;// = strlen(global_licence_file_txt);
	int poz = 0;
	string	licence;
	string	global_yes;
	string	global_heading;

	switch (uMsg) {
        case WM_COMMAND:
			switch(LOWORD(wParam)) {
			case IDNO:
				EndDialog(hwndDlg,IDNO);				
				return IDNO;
			}
			break;
	}
  return 0L;
}
#endif

void xor_data(char* buffer,int size,unsigned int gps_id) {
	if( gps_id == 0 )
		return;
	int t=0;
	char id[4];
	int p;

	memcpy(id,&gps_id,4);

	for( p = 0; p < size; ++p ) {
		buffer[p] = buffer[p] ^ id[t];
		t++;
		if( t > 3 ) {
			t=0;
		}
	}
}

#ifndef _DLL_VERSION
#ifndef _SENDMAP_LIB

int main(int argc,char* argv[]) {
	int			t_argument = 1;
	int			t_custom_speed = -1;
	map_uploader*	uploader = NULL;
	comm_medium_type port_type;
	bool		t_raw_data = false;
	bool		t_simulation = false;
	bool		t_create_exe = false;
	bool		t_self_img = false;
	bool		t_erase_only = false;
	bool		t_custom_port = false;
	bool		t_expired = false;
	bool		t_turn_off = false;
	bool		t_flash_id = false;
	bool		t_mps = true;
	char		temp_path[2048];
	char		temp_file[2048];
#if TRIAL == 1
	string		mapset_name = "Trial SendMap";
	bool		one_file_trial = false;
#else
	string		mapset_name = "Maps uploaded with SendMap";
#endif
	char		sendmap_exe[2024];
	string		licence_file;
	string		expired_file;
	string		sendmap_exe_img;
	string		copyright_string;
	unsigned int	gps_id = 0;
	vector<string>	key_list;
	int			exp_year,exp_month,exp_day;
#ifdef _WIN32
	SYSTEMTIME	sys_time;
	HINSTANCE hInst = LoadLibrary("RICHED20.DLL");

	exp_year = exp_month = exp_day = 0;

	if( sendmap_data.len_data != 0 ) {
		t_self_img = true;
		t_raw_data = false;
	}

	GetModuleFileName(NULL,sendmap_exe,1000);
	GetSystemTime(&sys_time);
	cout<<sendmap_data.copyright_string<<endl;

#if TRIAL == 1
	cout<<"Trial version - one IMG can be processed only, '-m','-r' options disabled"<<endl;
#endif
	if( t_self_img && sendmap_data.copyright_len_data ) {
		FILE*	lic_file = fopen(sendmap_exe,"rb");

		string help;

		if( lic_file != NULL ) {
			fseek(lic_file,sendmap_data.copyright_start_data,SEEK_SET);
			global_licence_file_txt = new char[sendmap_data.copyright_len_data+2];
			memset(global_licence_file_txt,0,sendmap_data.copyright_len_data+1);
			fread(global_licence_file_txt,1,sendmap_data.copyright_len_data,lic_file);
			fclose(lic_file);
			if( DialogBox(NULL,MAKEINTRESOURCE(ID_LICENCE),NULL,DialogProcedureLic) != IDYES ) {
				FreeLibrary(hInst);
				return 1;
			}
			delete []global_licence_file_txt;
		}
		
	}

	//expired check
	if( t_self_img && sendmap_data.year > 0) {
		if(sendmap_data.year < sys_time.wYear )
			t_expired = true;
		if(sendmap_data.year == sys_time.wYear && sendmap_data.month < sys_time.wMonth )
			t_expired = true;
		if(sendmap_data.year == sys_time.wYear && sendmap_data.month == sys_time.wMonth && sendmap_data.day < sys_time.wDay )
			t_expired = true;
	}

	if( t_self_img && sendmap_data.shareware == 1 ) {
		cout<<endl<<endl;
		cout<<"*******************************************************************************"<<endl;
		cout<<"                    Thank you for using sendmap20 program."<<endl;
		cout<<" cGPSmapper free version and sendmap20 free version can be used to create and "<<endl;
		cout<<"                     distribute ONLY NON COMMERCIAL maps."<<endl;
		cout<<"     If you want to know more - visit sendmap20 and cGPSmapper homepage :"<<endl;
		cout<<"                              http://cgpsmapper.com"<<endl;
		cout<<"*******************************************************************************"<<endl;
		//DialogBox(NULL,MAKEINTRESOURCE(ID_INFO),NULL,DialogProcedureInfo);
	}

#endif
	if( argc < 2 && !t_self_img) {
		string help;// = "Sendmap20 rev 5.0 by gps_mapper, gps@cgpsmapper.com\n\n";
#ifdef LINUX
		help = sendmap_data.copyright_string;
		help += "\n";
#endif
		help += "Homepage: http://cgpsmapper.com\n\n";
		help += "Send maps to GARMIN GPS device.\n\n";
#ifdef _WIN32
		help += "Simplest map sending: drop your selected IMG files on sendmap20.exe !\n\n";
#endif
		help += "Command line use :\n\n";
		help += "sendmap20 file1.img file2.img file3.img ....\n";
		help += " sendmap20 will try to detect to which port your GPS is connected\n";
		help += " USB, COM1, COM2, COM3, COM4, COM5 and COM6 are checked.\n\n";
		help += "sendmap20 -y test_file.exe file1.img file2.img file3.img ....\n";
		help += " sendmap20 will create a 'test_file.exe' distribution program with maps included\n\n";
		help += "sendmap20 -tUSB file1.img file2.img file3.img ...\n";
		help += " sends maps directly to USB port\n you have to install GARMIN USB driver before!!\n\n";
		help += "sendmap20 -tCOM2 -s9600 file1.img file2.img file3.img ...\n";
		help += " sends maps directly to COM2 port with custom speed (default speed is 115200)\n\n";
		help += "sendmap20 -r -tCOM2 one_map_file.img\n";
		help += " sends maps directly to COM2 as raw data - USE WITH CAUTION!\n";
		help += " NO CHECK OF INTERNAL DATA IS DONE!!\n\n";
		help += "Use option -h to see full list of available parameters\n";
#ifdef _WIN32
		return process_GUI();
#else
		cout<<help<<endl;
#endif		/*
		MessageBox(NULL,
			help.c_str(),
			sendmap_data.copyright_string,MB_ICONASTERISK | MB_OK);
		
		return 0;*/
	}

#ifdef _WIN32
	if( t_expired && sendmap_data.year > 0 ) {
		bool t_bypass = false;
		if( argc > 1 ) {
			if( (argv[t_argument][0] == '-' && argv[t_argument][1] == 'e') )
				t_bypass = true;
		}

		if( !t_bypass ) {
			FILE*	exp_file = fopen(sendmap_exe,"rb");

			string help;

			if( exp_file != NULL ) {
				fseek(exp_file,sendmap_data.expired_start_data,SEEK_SET);
				global_expiration_file_txt = new char[sendmap_data.expired_len_data+2];
				memset(global_expiration_file_txt,0,sendmap_data.expired_len_data+1);
				fread(global_expiration_file_txt,1,sendmap_data.expired_len_data,exp_file);
				fclose(exp_file);
				DialogBox(NULL,MAKEINTRESOURCE(ID_EXPIRATION),NULL,DialogProcedureExp);
				delete []global_expiration_file_txt;
			}
			FreeLibrary(hInst);
			return 1;
		}
	}
	FreeLibrary(hInst);
#endif
	string com_port = ""; //SIM, USB, comX or unix style

	while(t_argument < argc || t_self_img) {
		if( t_self_img  ||
		   (!t_self_img && argv[t_argument][0] != '-' )) {
			if( uploader == NULL ) {
				uploader = connect_to_gps(t_flash_id);

				if( uploader == NULL ) {
					return 1;
				}

				if( t_turn_off )
					uploader->turn_off_after_upload();
			}			
#ifdef _WIN32
			if( t_self_img && t_argument >= argc ) {
				unsigned int crypt_gps_id = 0;
				if( sendmap_data.gps_id > 0 ) {
					if( calculate_crypt_gps_id(sendmap_exe,uploader->id(),crypt_gps_id) > 0 )
						return 1;
				} else
					crypt_gps_id = 0;

				//expired code START
				GetSystemTime(&sys_time);
				bool expired = false;
				if( sendmap_data.year > 0 ) {
					if(sendmap_data.year < sys_time.wYear ) expired = true;
					if(sendmap_data.year == sys_time.wYear && sendmap_data.month < sys_time.wMonth ) expired = true;
					if(sendmap_data.year == sys_time.wYear && sendmap_data.month == sys_time.wMonth && sendmap_data.day < sys_time.wDay ) expired = true;
				}
				//expired code STOP

				if( expired == false )
					//uploader->add_img_file(sendmap_exe,NULL,crypt_gps_id,sendmap_data.start_data);
					uploader->add_raw_file(sendmap_exe,sendmap_data.start_data,sendmap_data.len_data);
				break;
			}	
#endif
			if( t_argument < argc ) {
				if( t_raw_data ) {
					uploader->add_raw_file(argv[t_argument]);
				} else {
#if TRIAL == 1
					if( one_file_trial == false )
#endif
						if( uploader->add_img_file(argv[t_argument],NULL) == false ) {
							delete uploader;
							cout<<"Cannot open file "<<argv[t_argument]<<" or file not valid"<<endl;
							return 2;
						}
#if TRIAL == 1
				one_file_trial = true;		
#endif
				}
			}
		} else if(t_argument < argc) {
			if(argv[t_argument][1] == 'h') {
				if(argv[t_argument][2] == 'x' && !t_self_img) {
					cout<<"-x new_file.exe copyright_string   create executable (Pro version only)"<<endl;
					cout<<" If copyright_string includes blank character then must be enclosed in "<<endl;
					cout<<" quotation-marks"<<endl<<endl;
					cout<<"-c file_name            file with EULA text"<<endl;
					cout<<" file_name is a text file where first line is used as a caption of "<<endl;
					cout<<" the window, second line as a caption for the confirmation button and "<<endl;
					cout<<" third line as a caption of the decline button"<<endl<<endl;
					cout<<"-d ddmmyyyy file_name   set expired date and text"<<endl;
					cout<<" file_name is a text file where first line is used as a caption of "<<endl;
					cout<<" the window, second line as a caption for the close button "<<endl<<endl;
					cout<<"-j gps_id               upload will be possible only to specified "<<endl;
					cout<<" GPS with given gps_id - gps_id can be determined by option '-i' "<<endl;
					cout<<" if gps_id for this option is set to 0 during exe creation then "<<endl;
					cout<<" sendmap20 will get gps_id of the connected receiver "<<endl;
					cout<<" GPS must be connected to the computer and turned on - NO UPLOAD IS DONE"<<endl;
					cout<<" ONLY gps_id IS TAKEN FROM GPS!"<<endl;
					return 0;
				} 
				cout<<"Full list of available parameters for sendmap20:"<<endl;
				if( !t_self_img ) {					
					cout<<"-y new_file.exe    create executable"<<endl;
					cout<<"-l                 create file instead of the upload"<<endl;
					cout<<"-r                 raw data upload"<<endl;
					cout<<"-o                 turn off after upload"<<endl;
					cout<<"-i                 show ID of the connected receiver"<<endl;
					cout<<"-n                 do not create maps index"<<endl;
					//cout<<"-z                 compatibility mode if old receivers (32Mb limit)"<<endl<<endl;
					cout<<" Sendmap20 Pro version only:"<<endl;					
					cout<<"-x new_file.exe copyright_string   create executable (Pro version only)"<<endl;
					cout<<"   to get more information about '-x' please use '-hx' option"<<endl;
					cout<<"-c file_name          (only apply when -x is used) file with EULA text"<<endl;
					cout<<"-d ddmmyyyy file_name (only apply when -x is used) set expired date and text"<<endl;
					cout<<"-j gps_id             (only apply when -x is used) lock final exe to GPS"<<endl;

				}
				cout<<"-e                 erase all the maps from GPS / fix corrupted GPS"<<endl;
				if( !t_self_img ) {
					cout<<"-f file_name       list of IMG files stored in a text file (one name in a line)"<<endl;
					cout<<"-m map_set_name    set the mapset name visible in the list of maps"<<endl;
					cout<<"-k 25_character_unlock_key"<<endl;
				}
				cout<<"-sCustom_speed"<<endl;
				cout<<"-tPort"<<endl;
				return 0;
			}
			
			/*
			if(argv[t_argument][1] == 'z') {
				IMG::set_old_header(true);
				++t_argument;
				continue;
			}*/

			if(argv[t_argument][1] == 't') {
				t_custom_port = true;
				com_port = &argv[t_argument][2];
				if( com_port == "usb" || com_port == "USB" )
					port_type = resUSB;
				else
					port_type = resCOM;
				if( com_port == "sim" ) {
					port_type = resSIM;
					com_port == "GMAPSUPP.IMG";
				}
				uploader = new map_uploader(port_type,com_port.c_str(),false,t_flash_id);
				if( uploader->check_connection() == false ) {
					cout<<"Cannot connect to GPS - check if cable is connected and GPS is turned ON"<<endl;
					delete uploader;
					return 1;
				}
				++t_argument;
				continue;
			}
#if FULL == 1
			if( (argv[t_argument][1] == 'x' || argv[t_argument][1] == 'w') && !t_self_img ) {
				if( t_argument > 1 ) {
					cout<<"Parameter '-x' must be the first one"<<endl;
					return 1;
				}
				if( t_argument+2 >= argc ) {
					cout<<"Wrong parameters for option '-x' - correct format is:"<<endl;
					cout<<"-x new_filename.exe copyright_string"<<endl;
					return 1;
				}
				if( t_argument+3 >= argc && argv[t_argument][1] == 'w') {
					cout<<"Wrong parameters for option '-w' - correct format is:"<<endl;
					cout<<"-w new_filename.exe copyright_string original_exe_name"<<endl;
					return 1;
				}

				t_create_exe = true;
				com_port = "SIM";
				port_type = resSIM;
				sendmap_exe_img = argv[++t_argument];
				if( sendmap_exe_img[sendmap_exe_img.size()-4] != '.' )
					sendmap_exe_img += ".exe";

				GetTempPath(sizeof(temp_path),temp_path);
				GetTempFileName(temp_path,"~fr",0,temp_file);

				uploader = new map_uploader(port_type,temp_file,false,t_flash_id);
				copyright_string = argv[++t_argument];

				if( argv[t_argument-2][1] == 'w') {
					strncpy(sendmap_exe,argv[++t_argument],1000);
				}

				++t_argument;
				continue;
			}
#endif
#ifdef _WIN32
			if(argv[t_argument][1] == 'y' && !t_self_img ) {
				if( t_argument > 1 ) {
					cout<<"Parameter '-y' must be the first one"<<endl;
					return 1;
				}
				if( t_argument+1 >= argc ) {
					cout<<"Wrong parameters for option '-y' - correct format is:"<<endl;
					cout<<"-y new_filename.exe"<<endl;
					return 1;
				}

				t_create_exe = true;
				com_port = "SIM";
				port_type = resSIM;
				sendmap_exe_img = argv[++t_argument];
				if( sendmap_exe_img[sendmap_exe_img.size()-4] != '.' )
					sendmap_exe_img += ".exe";

				GetTempPath(sizeof(temp_path),temp_path);
				GetTempFileName(temp_path,"~fr",0,temp_file);

				uploader = new map_uploader(port_type,temp_file,false,t_flash_id);
				copyright_string = sendmap_data.copyright_string;
				++t_argument;
				continue;
			}
#endif
#if FULL == 1
			if(argv[t_argument][1] == 'j') {
				if( t_create_exe == false ) {
					cout<<"Wrong options : '-j' can be use only together with '-x'"<<endl;
					return 1;
				}
				if( t_argument+1 >= argc ) {
					cout<<"Wrong parameters for option '-c' - correct format is:"<<endl;
					cout<<"-c gps_id"<<endl;
					return 1;
				}
				gps_id = strtoul(argv[++t_argument],NULL,10);
				//gps_id = atoi(argv[++t_argument]);
				++t_argument;
				if( gps_id == 0 ) {
					map_uploader* tmp_uploader = connect_to_gps(t_flash_id);
					if( tmp_uploader == NULL ) {
						cout<<"Could not determine gps_id of the connected GPS - make sure that GPS is turned on and connected to the computer"<<endl;
						return 1;
					}
					gps_id = tmp_uploader->id();
					delete tmp_uploader;
				}
				continue;
			}
#endif
#ifdef _WIN32
			if(argv[t_argument][1] == 'c') {
				if( t_create_exe == false ) {
					cout<<"Wrong options : '-c' can be use only together with '-x'"<<endl;
					return 1;
				}
				if( t_argument+1 >= argc ) {
					cout<<"Wrong parameters for option '-c' - correct format is:"<<endl;
					cout<<"-c file_name"<<endl;
					return 1;
				}
				licence_file = argv[++t_argument];
				++t_argument;
				continue;
			}
#endif
			if(argv[t_argument][1] == 'n') {
				t_mps = false;
				++t_argument;
				continue;
			}

			//flash id inny!
			if(argv[t_argument][1] == 'z') {
				t_flash_id = true;
				++t_argument;
				continue;
			}

#ifdef _WIN32
			if(argv[t_argument][1] == 'd') {
				if( t_create_exe == false ) {
					cout<<"Wrong options : '-d' can be use only together with '-x'"<<endl;
					return 1;
				}
				if( t_argument+2 >= argc ) {
					cout<<"Wrong parameters for option '-d' - correct format is:"<<endl;
					cout<<"-d ddmmyyyy expiration_message_file"<<endl;
					return 1;
				}
				string t_date = argv[++t_argument];
				exp_day		= atoi(t_date.substr(0,2).c_str());
				exp_month	= atoi(t_date.substr(2,2).c_str());
				exp_year	= atoi(t_date.substr(4,4).c_str());

				if( exp_day == 0 || exp_month == 0 || exp_year == 0 || exp_month > 12 || exp_day > 31) {
					cout<<"Wrong expiration date - correct format is: -d ddmmyyyy expiration_message_file"<<endl;
					return 1;
				}

				expired_file = argv[++t_argument];
				++t_argument;
				continue;
			}
#endif
			if(argv[t_argument][1] == 'o') {
				t_turn_off = true;
				++t_argument;
				continue;
			};

			if(argv[t_argument][1] == 'l' 
#ifdef _DEBUG
				&& !t_self_img
#endif
				) {
				t_simulation = true;
				com_port = "SIM";
				port_type = resSIM;
				uploader = new map_uploader(port_type,"GMAPSUPP.IMG");
				++t_argument;
				continue;
			}
#if TRIAL != 1
			if(argv[t_argument][1] == 'r' && !t_self_img) {
				t_raw_data = true;
				++t_argument;
				continue;
			}
#endif
			if(argv[t_argument][1] == 's') {
				memcpy(temp_key, &argv[t_argument][2], strlen(argv[t_argument]) - 2 >= TEMP_STR_LEN ? TEMP_STR_LEN-1 : strlen(argv[t_argument]) - 2);
				t_custom_speed = atoi(temp_key);
				if( t_custom_speed < 9600 || t_custom_speed > 115200 ) {
					cout<<"Wrong parameters for option '-s' - correct format is:"<<endl;
					cout<<"-sXXXXXX"<<endl;
					cout<<"Valid values for XXXXXX are :"<<endl;
					cout<<" 9600, 19200, 38400, 57600, 115200"<<endl;
					cout<<"If you have problems uploading maps using RS (COM) interface, it is recommended"<<endl;
					cout<<"to use slowes baud rate which is 9600"<<endl;

					return 1;
				}

				++t_argument;
				continue;
			}
			if(argv[t_argument][1] == 'k' && !t_self_img) {
				string new_key;
				bool error = false;
				if( t_argument+1 >= argc )
					error = true;

				new_key = argv[++t_argument];				
				while( new_key.find("-") < new_key.size() )
					new_key = new_key.substr(0,new_key.find("-")) + new_key.substr(new_key.find("-")+1);

				if(new_key.size() != 25)
					error = true;
				if( error ) {
					cout<<"Wrong parameters for option '-k' - correct format is:"<<endl;
					cout<<"-k 25_character_long_unlock_key"<<endl;
					return 1;
				}
				key_list.push_back(new_key);				
				++t_argument;
				continue;
			}
			if(argv[t_argument][1] == 'e') {
				if( uploader == NULL ) 
					uploader = connect_to_gps(t_flash_id);
				if( uploader ) {
					if(	uploader->erase_only() ) {
						delete uploader;
						return 0;
					}
					delete uploader;
				}
				return 1;
			}
#if TRIAL != 1
			if(argv[t_argument][1] == 'm' && !t_self_img) {
				if( t_argument+1 >= argc ) {
					cout<<"Wrong parameters for option '-m' - correct format is:"<<endl;
					cout<<"-m mapset_name"<<endl;
					return 1;
				}
				mapset_name = argv[++t_argument];
				++t_argument;
				continue;
			}
#endif
#if GETFILE == 1
			if( argv[t_argument][1] == 'g' && !t_self_img) {
				char	temp_file[MAX_PATH];

				if( uploader == NULL ) {
					uploader = connect_to_gps(t_flash_id);
					if( uploader == NULL )
						return 1;
				}
				if( uploader->download_directory(NULL,true) == false) {
                    delete uploader;
					return 1;
				}
				//uploader->download_files(temp_file,sizeof temp_file);

				delete uploader;
				return 0;
			}

#endif
			if( argv[t_argument][1] == 'i' && !t_self_img) {
				if( uploader == NULL ) {
					uploader = connect_to_gps(t_flash_id);
					if( uploader == NULL )
						return 1;
				}
				cout<<"GPS ID: "<<uploader->id()<<endl;
				delete uploader;
				return 0;
			}
			if(argv[t_argument][1] == 'f' && !t_self_img) {
				string list_file = argv[++t_argument];
				if( uploader == NULL ) {
					uploader = connect_to_gps(t_flash_id);
					if( uploader == NULL )
						return 1;
				}
				FILE*	list = fopen(list_file.c_str(),"r");
				char	file_name[1025];
				char	region_name[1025];
				strcpy(region_name,"map");
				vector<internal_file>	TRE_file_list;
				if( list ) {
					while( fgets(file_name,1024,list) != NULL ) {
						//remove 'non asci' chars!
						while( file_name[strlen(file_name)-1] < 32 && strlen(file_name))
							file_name[strlen(file_name)-1] = 0;
						if( strlen(file_name) ) {
							if( file_name[0] == ':' ) {
								strcpy(region_name,&file_name[1]);
							} else {
								if( uploader->add_img_file(file_name,&TRE_file_list,"",0,0,region_name,uploader->get_product_id(region_name)) == false ) {
									delete uploader;
									fclose(list);
									cout<<"Cannot open file or file not valid"<<file_name<<endl;
									return 2;
								}
							}
						}
					}
					fclose(list);
				}
				++t_argument;
				continue;
			}

			/*
			Command list :
			-x copyright_string				create executable
			-c licence_file					(only apply when -x is used) 
			-d ddmmyyyy message_file		(only apply when -x is used) 

			-l								create file instead of the upload
			-r								raw data upload
			-f file_name					list of IMG files stored in text file - one line - one IMG filename
			-m map_set_name

			-sCustom_speed
			-tPort
			*/

/*
			if(argv[t_argument][1] == 'k') {
				if(strlen(argv[t_poz]) != 27) {
					cout << "UNLOCK_KEY must be 25 characters long!!" << endl;
					delete garminComm;
					return 1;
				}
				memcpy(tempKey, &argv[t_poz][2], 25);
				tempKey[25] = 0;
				garminComm->keysGPS.push_back(KeysGPSTransferInfo(tempKey, 1)); //FIXME - always product1?
			}


			if(argv[t_poz][1] == 'v') {
				garminComm->SetVerbose();
			}

			if(argv[t_poz][1] == 'x') {
				garminComm->SetNoMPS();
			}

			if(argv[t_poz][1] == 'u') {
				t_unlocked_maps = true;
			}

			if(argv[t_poz][1] == 'g') {
				t_get_maps = true;
			}

			if(argv[t_poz][1] == 'p') {
				t_pause = true;
			}


			if(argv[t_poz][1] == 'b') {
				memcpy(tempKey, &argv[t_poz][2], strlen(argv[t_poz]) - 2 >= TEMP_STR_LEN ? TEMP_STR_LEN-1 : strlen(argv[t_poz]) - 2);
				customBlockSize = atoi(tempKey);
				if(customBlockSize != 512 && customBlockSize != 1024 && customBlockSize != 2048)
					customBlockSize = 0;
				garminComm->SetCustomBlockSize(customBlockSize);
			}

			if(argv[t_poz][1] == 'c') {
				strncpy(mapSetName, &argv[t_poz][2], MAP_SET_NAME_LEN-1);
				garminComm->SetSimulation(resCrypted,mapSetName);
			}
			*/
		}
		t_argument++;
	}

	//Sprawdzanie parametrow !!!
	if( t_create_exe ) { 
		if( t_erase_only ) {
			cout<<"Wrong options : cannot use '-e' together with '-x'"<<endl;
			return 1;
		}
		if( t_simulation ) {
			cout<<"Wrong options : cannot use '-l' together with '-x'"<<endl;
			return 1;
		}
		if( t_custom_speed > 0 ) {
			cout<<"Wrong options : cannot use '-s' together with '-x'"<<endl;
			return 1;
		}
		if( t_custom_port ) {
			cout<<"Wrong options : cannot use '-t' together with '-x'"<<endl;
			return 1;
		}
	}

	if(uploader) {
		if( uploader->no_files() && !t_self_img ) {
			cout<<"No files to process"<<endl;
			delete uploader;
			return 1;
		}

		if( key_list.size() ) 
			uploader->add_keys(key_list);

		if( t_custom_speed > 0 )
			uploader->connection_parameter(t_custom_speed);

#if TRIAL == 1
		mapset_name = "             ";
		mapset_name = "Trial SendMap";
		uploader->set_mapset_name(mapset_name.c_str());
#else
		if( !t_self_img )
			uploader->set_mapset_name(mapset_name.c_str());
#endif
		if( t_mps == false )
			uploader->disable_mps();
		if( t_self_img ) {
			uploader->set_selfupload_size(sendmap_data.len_data);
			if( calculate_crypt_gps_id(sendmap_exe,uploader->id(),gps_id) > 0 )
				return 1;
			if( sendmap_data.gps_id != 0 ) {
				if( uploader->id() != sendmap_data.gps_id ) {
					cout<<"Unable to upload map - wrong GPS ID"<<endl;
					delete uploader;
					return 1;
				}
				if( uploader->upload(true,true,gps_id) == false ) {
					cout<<"Error during data transfer - CRC calculation failed or error in data integrity"<<endl;
				}
			} else
				uploader->upload(true,false);
		} else
			uploader->upload(t_raw_data);
#ifdef _DEBUG
		cout<<"Done"<<endl;
#endif
		delete uploader;
#ifdef _WIN32
		if( t_create_exe ) {
#ifdef _DEBUG
			cout<<temp_file<<endl;
#endif
			create_exe_package(gps_id,exp_day,exp_month,exp_year,sendmap_exe,copyright_string,licence_file,expired_file,sendmap_exe_img,temp_file);
			DeleteFile(temp_file);
		}
#endif
	}
	return 0;
}

#endif
#endif