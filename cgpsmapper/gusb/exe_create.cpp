/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/
#include "exe_create.h"
#ifdef _WIN32
#include <vector>
#include <iostream>
#include "uploader.h"

using namespace std;

extern t_sendmap sendmap_data;

#ifndef _DLL_VERSION

int create_exe_package(unsigned int gps_id,int exp_day,int exp_month,int exp_year,
						char sendmap_exe[2024],string copyright_string,string licence_file,string expired_file,string sendmap_exe_img,char* temp_file) {

	char* file_exe;
	char* licence_file_txt;
	char* expired_file_txt;
	char buffer[0x1000];
	size_t data_start;
	t_sendmap* sendmap_data;
	size_t prog_size,data_size,lic_size,exp_size;

	//1 - skopiowanie sendmap_exe do nowego pliku

	//2 - dolaczenie licencji

	//2a- dolaczenie gmapsupp.img do pliku

	//3 - wpisanie wartosci do nowego pliku

	FILE*	prog_file = fopen(sendmap_exe,"rb");
	FILE*	lic_file = NULL;
	if( licence_file.size() )
		lic_file = fopen(licence_file.c_str(),"rb");
	if( lic_file == NULL && licence_file.size() ) {
		cout<<"Warning - could not open licence file!"<<endl;
		lic_file = NULL;
	}
	FILE*	exp_file = NULL;
	if( expired_file.size() )
		exp_file = fopen(expired_file.c_str(),"rb");
	if( exp_file == NULL && exp_year != 0 )
		cout<<"Warning - could not open expired message file!"<<endl;


	FILE*	data_file;
	if( temp_file != NULL )
		data_file = fopen(temp_file,"rb");
	else
		data_file = fopen("GMAPSUPP.IMG","rb");
	FILE*	out_file = fopen(sendmap_exe_img.c_str(),"w+b");

	if( prog_file == NULL ) {
		cout<<"Fatal error - cannot find sendmap20 executable"<<endl;
		return 10;
	}
	if( data_file == NULL ) {
		cout<<"Fatal error - cannot find temporary IMG file"<<endl;
		fclose(prog_file);
		DeleteFile(sendmap_exe);
		return 10;
	}

	if( lic_file != NULL ) {
		fseek(lic_file,0,SEEK_END);
		lic_size = ftell(lic_file);
		fseek(lic_file,0,SEEK_SET);

		licence_file_txt = new char[lic_size];
		lic_size = fread(licence_file_txt,1,lic_size,lic_file);
		fclose(lic_file);
	} else
		lic_size = 0;

	if( exp_file != NULL ) {
		fseek(exp_file,0,SEEK_END);
		exp_size = ftell(exp_file);
		fseek(exp_file,0,SEEK_SET);

		expired_file_txt = new char[exp_size];
		exp_size = fread(expired_file_txt,1,exp_size,exp_file);
		fclose(exp_file);
	} else
		exp_size = 0;

	fseek(data_file,0,SEEK_END);
	data_size = ftell(data_file);
	fseek(data_file,0,SEEK_SET);

	fseek(prog_file,0,SEEK_END);
	prog_size = ftell(prog_file);
	file_exe = new char[prog_size];
	fseek(prog_file,0,SEEK_SET);
	prog_size = fread(file_exe,1,prog_size,prog_file);
	fclose(prog_file);

	//zapisanie parametrow
	for( data_start =0; data_start < prog_size; data_start++ ) {
		if( file_exe[data_start] == 'U' && file_exe[data_start+1] == 'S' && file_exe[data_start+2] == 'B' && file_exe[data_start+3] == '_' && file_exe[data_start+4] == 'M' && file_exe[data_start+5] == 'A' ) {
			break;
		}
	}
	//data set
	sendmap_data = (t_sendmap*)(&file_exe[data_start]);
	strncpy(sendmap_data->copyright_string,copyright_string.c_str(),110);

	sendmap_data->copyright_start_data = int(prog_size);
	sendmap_data->copyright_len_data = int(lic_size);

	sendmap_data->expired_start_data = int(prog_size + lic_size);
	sendmap_data->expired_len_data = int(exp_size);

	sendmap_data->start_data = int(prog_size+lic_size+exp_size);
	sendmap_data->len_data	= int(data_size);

	sendmap_data->year		= exp_year;
	sendmap_data->month		= exp_month;
	sendmap_data->day		= exp_day;
	sendmap_data->shareware = 0;
	sendmap_data->gps_id	= gps_id;
#if FULL == 0
	sendmap_data->shareware = 1;
#endif
	//zapisane programu
	if( gps_id != 0 )
		gps_id = static_cast<int>(crc_calc(file_exe,prog_size,gps_id));
	fwrite(file_exe,1,prog_size,out_file);
	delete []file_exe;

	//zapisanie licencji
	if( lic_size ) {
		fwrite(licence_file_txt,1,lic_size,out_file);
		delete []licence_file_txt;
	}

	//zapisanie expired message
	if( exp_size ) {
		fwrite(expired_file_txt,1,exp_size,out_file);
		delete []expired_file_txt;
	}

	//zapisanie IMG
	fseek(out_file,prog_size+lic_size+exp_size,SEEK_SET);
	while( data_size > 0x1000 ) {
		fread(buffer,1,0x1000,data_file);
		if( gps_id != 0 )
			xor_data(buffer,0x1000,gps_id);
		fwrite(buffer,1,0x1000,out_file);
		data_size-=0x1000;
	}
	fread(buffer,1,data_size,data_file);
	if( gps_id != 0 )
		xor_data(buffer,static_cast<int>(data_size),gps_id);
	fwrite(buffer,1,data_size,out_file);
	fclose(data_file);

	///

	fclose(out_file);
	DeleteFile("GMAPSUPP.IMG");

	cout<<"Map installation program : "<<sendmap_exe_img<<" has been created."<<endl;
	//copy itself
	//merge GMAPSUPP.IMG
	//fill start,len
	return 0;
}
#endif
#endif
