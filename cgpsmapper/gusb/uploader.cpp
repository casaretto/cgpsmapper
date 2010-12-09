/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

#include "uploader.h"
#include <set>
#include <stdexcept>
#ifdef LINUX
#include <stdio.h>
#include <string.h>
#endif

map<string,int>	map_uploader::products_id;

int map_uploader::get_product_id(const char* region_name) {
	if( products_id.find(string(region_name)) != products_id.end() )
		return products_id.find(string(region_name))->second;
	products_id[string(region_name)] = CONST_FAMILY_ID + (products_id.size() << 16);
	return products_id.find(string(region_name))->second;
}

map_uploader::map_uploader(comm_medium_type port_type,const char* port,bool turn_off,bool _custom_flash) {
	int	custom_flash_id = 0;
	strcpy(map_copyright,"maps");
	mps_enable = true;
	typ_enable = false;
	gps_found = false;
	disable_out = false;
 	this->turn_off = turn_off;
	this->port = port;
	this->port_type = port_type;
	upload_speed = 115200;
	send_delay = 3;
	get_delay = 3;
	speed_change_delay = 100;

	custom_flash = _custom_flash;
	if( custom_flash )
		custom_flash_id = 0x31;

	gps = NULL;
	if( port_type == resUSB ) {
		gps = new GUSB_interface(false,custom_flash_id);
	} else if( port_type == resCOM ) {
		gps = new COM_interface();
	} else {
		gps = new SIM_interface();
	}
}

void map_uploader::reconnect(comm_medium_type port_type,const char* port) {
	delete gps;

	int custom_flash_id = 0;
	if( custom_flash )
		custom_flash_id = 0x31;

	gps_found = false;
	this->port = port;
	this->port_type = port_type;
	if( port_type == resUSB ) {
		gps = new GUSB_interface(false,custom_flash_id);

	} else if( port_type == resCOM ) {
		gps = new COM_interface();
	} else {
		gps = new SIM_interface();
	}
}

map_uploader::~map_uploader() {
	if( gps )
		delete gps;
}

bool map_uploader::download_directory(vector<internal_file> *TRE_file_list,bool image) {
	char	temp_path[MAX_PATH];
	char	temp_file[MAX_PATH];
	int		total_size = 0;
	int		position = 0;
	internal_file*	internal_f;

	if( gps->open_com(this->port.c_str()) ) {

		gps->set_async();
		if( gps->set_speed(this->upload_speed) != true ) {
			if( disable_out == false ) {
				cout<<"\rCannot change speed, trying to use the default speed"<<endl;
			}
			if( gps->get_basic_info() != true )
				return false;
		}
		if( gps->get_memory_info() == false )
			return false;

		if( image ) {
			strcpy(temp_file,"GPS.IMG");
			strcpy(temp_path,"");
		} else {
#ifdef _WIN32
			GetTempPath(sizeof(temp_path),temp_path);
			GetTempFileName(temp_path,"~fr",0,temp_file);
#else
			strcpy(temp_file,"/tmp/senXXXXXX");
			mkstemp(temp_file);
#endif
		}
		if( gps->request_file("",temp_file,NULL,total_size,position) ) {
			if( gps_maps.add_IMG(temp_file,TRE_file_list,"",0,0,true) == false ) {
				if( gps->set_speed(9600) == true ) {
					gps->get_basic_info();
				}			
				return false;
			}
		}
		if( image ) {
			gps_maps.reset_download_files();
			while( internal_f = gps_maps.next_download_file() ) {
				if( !gps->request_file(temp_file,temp_file,internal_f,total_size,position,false,internal_f->internal_offset) )
					return false;
			}
			return true;
		}
#ifdef _WIN32
		DeleteFile(temp_file);
#else
		remove(temp_file);
#endif
		//check if MPS file exist - if so - use it
		gps_maps.reset_download_files();
		while( internal_f = gps_maps.next_download_file() ) {
			if( internal_f->file_type == imgMPS ) {
				internal_f->file_name = temp_file;
				if( gps->request_file("",temp_file,internal_f,total_size,position) ) {
					//use MPS file here!
					mps_parser(temp_file,TRE_file_list,&gps_maps);
#ifdef _WIN32
					DeleteFile(temp_file);
#else
					remove(temp_file);
#endif
				}
				gps_maps.remove_exact_IMG(internal_f->get_internal_filename().c_str());
			}
		}
	}

	if( gps->set_speed(9600) == true ) {
		if( gps->get_basic_info() != true )
			if( disable_out == false ){
				cout<<"Could not recover communication!"<<endl;
			}
	}
	gps->close_com();
	return true;
}

void map_uploader::mps_parser(const char* file_name,vector<internal_file> *TRE_file_list,IMG *maps) {
	FILE*			file;
	unsigned char	record;
	char			znak[2];
	unsigned short	size;
	size_t			pos;
	char			mapset_name[250];
	char			mapset_name_2[250];
	char			TRE_map_name[250];
	int				TRE_id;
	internal_file*	internal_f;
	vector<internal_file>::iterator tre_file;
	char			key[26];
	vector<string>	key_list;

	file = fopen(file_name,"rb");
	while( fread(&record,1,1,file) ) {
		pos = ftell(file) + 2;
		fread(&size,1,2,file);
		if( record == 0x55 ) {
			fread(&key,1,26,file);
			key_list.push_back(string(key));
		}
		if( record == 0x4c ) {
			memset(mapset_name,0,sizeof mapset_name);
			memset(mapset_name_2,0,sizeof mapset_name_2);
			memset(TRE_map_name,0,sizeof TRE_map_name);
			memset(znak,0,sizeof znak);

			fseek(file,8,SEEK_CUR);
			do {
				fread(&znak[0],1,1,file);
				strcat(mapset_name,znak);
			} while(znak[0]);

			do {
				fread(&znak[0],1,1,file);
				strcat(TRE_map_name,znak);
			} while(znak[0]);
			do {
				fread(&znak[0],1,1,file);
				strcat(mapset_name_2,znak);
			} while(znak[0]);

			fread(&TRE_id,1,4,file);

			maps->reset_download_files();
			while( internal_f = maps->next_download_file() ) {
				if( internal_f->file_type == imgTRE && internal_f->TRE_id == TRE_id) {
					internal_f->TRE_map_name = TRE_map_name;					
					internal_f->region_name = mapset_name;
				}
			}
			if( TRE_file_list ) {
				for( tre_file = TRE_file_list->begin(); tre_file < TRE_file_list->end(); tre_file++ ) {
					if( (*tre_file).TRE_id == TRE_id ) {
						(*tre_file).TRE_map_name = TRE_map_name;					
						(*tre_file).region_name = mapset_name;
					}
				}
			}
		} 
		//next record
		fseek(file,pos+size,SEEK_SET);
	}

	imgs.add_keys(key_list);
	fclose(file);
}

bool map_uploader::download_files(char* store_path,size_t store_path_size) {
	internal_file*	internal_f;
	char	temp_path[MAX_PATH];
	char	temp_file[MAX_PATH];
	int		total_size = 0;
	int		position = 0;
	internal_file	TRE_file;

	store_path[0] = 0;
	if( gps->open_com(this->port.c_str()) ) {

		gps->set_async();
		if( gps->set_speed(this->upload_speed) != true ) {
			if( disable_out == false ) {
				cout<<"\rCannot change speed, trying to use the default speed"<<endl;
			}
			if( gps->get_basic_info() != true )
				return false;
		}
		if( gps->get_memory_info() == false )
			return false;

#ifdef _WIN32
		GetTempPath(sizeof(temp_path),temp_path);
#else
		strcpy(temp_path,"/tmp/");
#endif

		cout<<"Relocating GPS data."<<endl<<"Still you can interrupt the whole process without the possibility of loosing"<<endl<<"of all the maps from GPS!"<<endl;
		cout<<"THIS IS THE LAST CHANCE - PRESS 'CTRL-C' OR TURN OFF GPS"<<endl<<"IF YOU WANT TO STOP THE PROCESS OF ADDING MAPS!"<<endl;
		cout<<endl<<endl<<"IMPORTANT!"<<endl;
		cout<<"Ensure that no other tasks are running during relocation process,"<<endl<<"or the process may fail."<<endl;
		cout<<"In case of failure of this phase you will NOT lose your maps from GPS."<<endl<<endl;
		//get file by file
		if( !gps_maps.no_IMG() ) {
			total_size = gps_maps.get_files_size();

			gps_maps.reset_download_files();
#ifdef _WIN32
			GetTempFileName(temp_path,"~fr",0,temp_file);
#else
			strcpy(temp_file,"/tmp/senXXXXXX");
			mkstemp(temp_file);
#endif

			strncpy(store_path,temp_file,store_path_size);
			while( internal_f = gps_maps.next_download_file() ) {
				if( gps->request_file("",temp_file,internal_f,total_size,position,true) == false )
					return false;
			}
		}

	}
	if( gps->set_speed(9600) == true ) {
		if( gps->get_basic_info() != true )
			if( disable_out == false ) {
				cout<<"Could not recover communication!"<<endl;
			}
	}
	gps->close_com();
	return true;
}

void map_uploader::add_keys(vector<string> key_list) {
	imgs.add_keys(key_list);
}

void map_uploader::add_raw_file(const char* file_name,int start_data,int len_data) {
	imgs.raw_file = file_name;
	imgs.raw_start_data = start_data;
	imgs.raw_len_data = len_data;
	imgs.set_final_size(len_data);
}

bool map_uploader::no_files() {
	if( imgs.no_IMG() && gps_maps.no_IMG() )
		return true;
	return false;
}

bool map_uploader::add_img_file_internal(const char* file_name,vector<internal_file> *TRE_file_list,const char* selected_file,const char* selected_file_rename) {
	return add_img_file(file_name,TRE_file_list,"",0,0,NULL,0,selected_file,selected_file_rename);
}

bool map_uploader::add_img_file(const char* file_name,vector<internal_file> *TRE_file_list,const char* password,unsigned int crypt_gps_id,int crypt_start,const char* region_name,const int current_product_id,const char* selected_file,const char* selected_file_rename) {
	char	temp_path[MAX_PATH];
	char	temp_file_mps[MAX_PATH];
#ifndef _WIN32
	int 	t_res = 0;
#endif

	if( crypt_start == 0 ) {
#ifdef _WIN32
		GetTempPath(sizeof(temp_path),temp_path);
		GetTempFileName(temp_path,"~fr",0,temp_file_mps);
#else
		memset(temp_file_mps,0,MAX_PATH-1);
		strcpy(temp_file_mps,"/tmp/senXXXXXX");
		t_res = mkstemp(temp_file_mps);
#endif
	} else
		temp_file_mps[0] = 0;

	bool result = imgs.add_IMG(file_name,TRE_file_list,password,crypt_gps_id,crypt_start,false,temp_file_mps,region_name,current_product_id,selected_file,selected_file_rename);
	if( result && TRE_file_list ) {
		mps_parser(temp_file_mps,TRE_file_list,&imgs);
#ifdef _WIN32
		DeleteFile(temp_file_mps);
#else
		remove(temp_file_mps);
#endif
	}
	return result;
}

void map_uploader::remove_all() {
	gps_maps.remove_all();
}

bool map_uploader::remove_img_file(const char* file_name,const char* internal_file_name) {
	if( strlen(file_name) >= 3 ) {
		if( file_name[0] == 'G' && file_name[1] == 'P' && file_name[2] == 'S' ) {
			return this->gps_maps.remove_IMG(file_name,internal_file_name);
		}
	}
	return imgs.remove_IMG(file_name,internal_file_name);
}

int	map_uploader::get_files_size() {
	return imgs.get_files_size() + gps_maps.get_files_size();
}


void map_uploader::set_mapset_name(const char* mapset_name) {

	imgs.set_mapset(mapset_name);
}

void map_uploader::connection_parameter(int upload_speed, int send_delay, int get_delay, int speed_change_delay) {
	this->upload_speed = upload_speed;
	this->send_delay = send_delay;
	this->get_delay = get_delay;
	this->speed_change_delay = speed_change_delay;
}

bool map_uploader::check_connection() {
	//try to connect
	try {
	if( gps->open_com(this->port.c_str()) ) {
#ifdef _DEBUG
		cout<<"Checking connection"<<endl;
#endif
		if( gps->connetcion_established() ) {
			if( gps->get_basic_info() ) {
				gps_found = true;
			}
		}
		gps->close_com();
	}
	} catch(...) {
	}
	return gps_found;
}

int map_uploader::memory() {
	if( gps->got_memory )
		return gps->GPS_memory;

	if( gps->open_com(this->port.c_str()) ) {
		if( gps->get_memory_info() == false )
			return -1;

		gps->close_com();
	}

	return gps->GPS_memory;
}

bool map_uploader::erase_only() {
	bool result = false;
	if( gps->open_com(this->port.c_str()) ) {
		gps->get_memory_info();
		gps->set_async();
		if( result = gps->erase_memory() ) {
			gps->finalize_upload();
		}
	}
	gps->close_com();
	return result;
}

void map_uploader::store_file_list(const char* file_name) {
	internal_file* i_file;
	FILE*			file;
	set<string>		file_names;

	file = fopen(file_name,"w");
	if( file == NULL )
		return;

	imgs.reset_download_files();
	while( i_file = imgs.next_download_file() ) {
		if( (*i_file).file_type == imgTRE ) {
			if( file_names.find( (*i_file).file_name ) == file_names.end() ) {
				file_names.insert((*i_file).file_name);
				fputs(":",file);
				fputs((*i_file).region_name.c_str(),file);
				fputs("\n",file);
				fputs((*i_file).file_name.c_str(),file);
				fputs("\n",file);
			}
		}
		if( (*i_file).file_type == imgTYP || (*i_file).file_type == imgMDR ) {
			fputs((*i_file).file_name.c_str(),file);
			fputs("\n",file);
		}
	}

	fclose(file);
}

bool map_uploader::upload(bool t_raw_data,bool crypted,unsigned int gps_id,bool NT) {
	bool	result = false;
	char	temp_file[MAX_PATH];

	gps_maps.map_copyright = imgs.map_copyright; //map_copyright;

	imgs.preserve_files();
	if( NT ) {
		imgs.calculate_final_GMP();
	}

	if( t_raw_data == false ) {
		if( !gps_maps.no_IMG() ) {
			//start with downloading maps from GPS
			if( download_files(temp_file,sizeof temp_file -1) == true ) {
				internal_file* i_file;
				//copy from gps_maps to imgs
				gps_maps.reset_download_files();
				while( i_file = gps_maps.next_download_file() ) {
					imgs+=(*i_file);
				}
				if( disable_out == false ) {
					cout<<endl;
				}
			} else {
				if( disable_out == false ) {
					cout<<endl<<"Relocation GPS data process failed."<<"You have to switch off GPS and then switch it on to restore the communication"<<endl;
				}
				gps_maps.remove_all();
				imgs.remove_all();
#ifdef _WIN32
				DeleteFile(temp_file);
#else
				remove(temp_file);
#endif
				return false;
			}
		}

		if( mps_enable == false )
			imgs.disable_mps();
		imgs.calculate_final_IMG();
		//reconnect(resSIM,"SIM.IMG");
	}
	try {
#ifdef _DEBUG
		cout<<"Starting upload process"<<endl;
#endif
		if( gps->open_com(this->port.c_str()) ) {
			if( gps->get_memory_info() ) /*gps->get_basic_info()*/ {
				if( disable_out == false ) {
					cout << "Detected          : " << gps->GPS_version << endl;
					cout << "Available memory  : ";
					if( gps->GPS_memory < 1024 * 1024 ) {
						cout << gps->GPS_memory / 1024<< " kB" << endl;
					} else if( gps->GPS_memory < 1024 * 1024 * 1024 ) {
						cout << gps->GPS_memory / (1024 * 1024)<< " MB" << endl;
					} else
						cout << gps->GPS_memory / (1024 * 1024 * 1024)<< " GB" << endl;
					cout << "Max number of maps: " << gps->GPS_max_maps << endl;
					cout << "Final map size    : ";

					if( imgs.get_final_size() < 1024 * 1024 ) {
						cout << imgs.get_final_size() / 1024<< " kB" << endl;
					} else {//if( imgs.get_final_size() < 1024 * 1024 * 1024 ) {
						cout << imgs.get_final_size() / (1024 * 1024)<< " MB" << endl;
					} //else
					//cout << imgs.get_final_size() / (1024 * 1024 * 1024)<< " GB" << endl;
				}
				if( imgs.get_final_size() > gps->GPS_memory && gps->get_flash_id() == 0 ) {
					if( disable_out == false ) {
						cout << "You have not enough memory to upload selected maps!" <<endl;
					}
					gps->close_com();
					imgs.after_upload();
					return false;
				}
				/*
				if( !IMG::get_old_header() && imgs.get_final_size() > 128 * 1024 * 1024 ) {
					cout << "You are in compatibility mode - you cannot upload more than 128Mb!" <<endl;
					gps->close_com();
					imgs.after_upload();
					return false;
				}*/

				gps->set_async();
				if( gps->set_speed(this->upload_speed) != true ) {
					if( disable_out == false ) {
						cout<<"\rCannot change speed, trying to use the default speed"<<endl;
					}
					if( gps->get_basic_info() != true ) {
						imgs.after_upload();
						return result;
					}
				}

				if( !gps_maps.no_IMG() ) {
					cout<<endl<<"DO NOT INTERRUPT THE UPLOAD PROCESS OR YOU WILL LOST ALL YOUR MAPS IN GPS!"<<endl;
				}

				if( gps->erase_memory() ) {
					//upload here!
					try {
					if( t_raw_data ) {						
						result = imgs.send_raw_data(gps,&progress::calculate,crypted,gps_id);
					} else
						//if( crypted )
						//	result = imgs.send_data(gps,&progress::calculate);
						//else
						result = imgs.send_data(gps,&progress::calculate);
					} catch (exception &e) {
#ifdef _DEBUG
						cout<<e.what()<<endl;
#endif
						gps->finalize_upload();
						gps->erase_memory();

						throw runtime_error("abort");
					}
					imgs.after_upload();

					if( result == true ) {
						if( gps->finalize_upload() ) {
							if( disable_out == false ) {
								cout<<"\rMaps uploaded!"<<endl;
							}
						} else {
							if( disable_out == false ) {
								cout<<"\rMaps uploade failed!"<<endl;
							}
							result = false;
						}
						if( this->turn_off ) {
							gps->turn_off();
							return result;
						}

						if( this->upload_speed > 9600 )
							gps->set_speed(9600);						
					}
				}
			}
			gps->close_com();
		}
	} catch(exception &e) {
#ifdef _DEBUG
		cout<<e.what()<<endl;
#endif
		if( e.what() == string("abort") )
			throw runtime_error("abort");			
	}
	if( !gps_maps.no_IMG() ) {
		gps_maps.remove_all();
		imgs.remove_all();
#ifdef _WIN32
		DeleteFile(temp_file);
#else
		remove(temp_file);
#endif
	}

	if( NT ) {
		imgs.delete_GMP();
	}

	imgs.restore_files();
	
	return result;
}
