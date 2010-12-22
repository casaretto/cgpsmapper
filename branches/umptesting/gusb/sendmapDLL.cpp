/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/
#ifndef _WIN32
#define _DLL_VERSION
#endif

#ifdef _DLL_VERSION

#include "uploader.h"
#include "sendmapDLL.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include <string.h>
#include <vector>


extern	t_sendmap sendmap_data;
extern	int calculate_crypt_gps_id(char sendmap_exe[2024],unsigned int id,unsigned int &crypt_gps_id);
bool	se_module_init = false;
map_uploader*	se_uploader;
std::vector<internal_file>		se_TRE_file_list;
std::vector<string>			se_keys;
string						gps_version;

_UPLOADER					uploaderC;
#ifdef _WIN32
HINSTANCE					dll_hInstance;

BOOL CALL_API DllMain(HINSTANCE hinst, DWORD dwReason,LPVOID lpvReserved)
{
	  // Perform actions based on the reason for calling.
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}
#endif


int CALL_API se_disconnect() {
	if( se_module_init != true )
		return _SE_OK;

	delete se_uploader;
	se_module_init = false;
	return _SE_OK;
}

int CALL_API se_connect(int port_type,const char* port_name,const char* copyright) {
	if( se_module_init == true )
		return _SE_OK;

	se_uploader = new map_uploader(static_cast<comm_medium_type>(port_type),port_name);
	if( se_uploader->check_connection() == false ) {
		delete se_uploader;
		return _SE_COULD_NOT_CONNECT;
	}
	strncpy(se_uploader->map_copyright,copyright,50);

	se_module_init = true;
	return _SE_OK;
}

unsigned int CALL_API se_get_id() {
	if( !se_module_init ) return _SE_NOT_INITIATED;

	return se_uploader->id();
}

int CALL_API se_get_memory() {
	if( !se_module_init ) return _SE_NOT_INITIATED;

	return se_uploader->memory();
}

const char* CALL_API se_get_version() {
	if( !se_module_init ) return NULL;

	//if( !gps_version.size() )
	gps_version = se_uploader->get_gps_version();
	return gps_version.c_str();
}

int CALL_API se_add_key(const char* key) {
	if( !se_module_init ) return _SE_NOT_INITIATED;

	se_keys.push_back(key);	
	return _SE_OK;
}

int CALL_API se_set_mapset(const char* name) {
	if( !se_module_init ) return _SE_NOT_INITIATED;

	se_uploader->set_mapset_name(name);
	return _SE_OK;
}

int CALL_API se_erase_memory() {
	if( !se_module_init ) return _SE_NOT_INITIATED;

	if( se_uploader->erase_only() )
		return _SE_OK;
	return _SE_OTHER_ERROR;
}

int CALL_API se_add_file(const char* file_name,const char* region_name) {
	if( !se_module_init ) return _SE_NOT_INITIATED;

	if( se_uploader->add_img_file(file_name,&se_TRE_file_list,"",0,0,region_name) )
		return _SE_OK;
	return _SE_NO_FILE;
}

int CALL_API se_add_file_internal(const char* file_name,const char* selected_file,const char* selected_file_rename) {
	if( !se_module_init ) return _SE_NOT_INITIATED;

	if( se_uploader->add_img_file_internal(file_name,&se_TRE_file_list,selected_file,selected_file_rename) )
		return _SE_OK;
	return _SE_NO_FILE;
}

int CALL_API se_add_crypt_file(const char* file_name,const char* region_name,const char* password) {
	if( !se_module_init ) return _SE_NOT_INITIATED;

	if( se_uploader->add_img_file(file_name,&se_TRE_file_list,password,0,0,region_name) )
		return _SE_OK;
	return _SE_NO_FILE;
}

int CALL_API se_add_raw_file(const char* file_name,int start_data,int len_data) {
	if( !se_module_init ) return _SE_NOT_INITIATED;

	se_uploader->add_raw_file(file_name,start_data,len_data);
	return _SE_OK;
}

#ifdef _WIN32
int CALL_API se_add_dll_file() {
	if( !se_module_init ) return _SE_NOT_INITIATED;

	char	buffer_name[2024];
	unsigned int	gps_id;
	//bieze exe a nie DLL !!
	GetModuleFileName(dll_hInstance,buffer_name,1000);
	if( sendmap_data.start_data ) {
		if( sendmap_data.gps_id != 0 )
		calculate_crypt_gps_id(buffer_name,sendmap_data.gps_id,gps_id);

		se_uploader->add_img_file(buffer_name,NULL,"",gps_id,sendmap_data.start_data);
		return _SE_OK;
	}
	return _SE_OTHER_ERROR;
}
#endif

int CALL_API se_upload_no_callback(int speed) {
	return se_upload(NULL,speed);
}

int CALL_API se_upload_no_callback_NT(int speed) {
	return se_upload(NULL,speed,true);
}

int CALL_API se_upload(_UPLOADER uploaderCallback,int speed,bool NT) {
	if( !se_module_init ) return _SE_NOT_INITIATED;

	//if( uploaderCallback == NULL ) return _SE_OTHER_ERROR;

	uploaderC = uploaderCallback;

	if( se_keys.size() )
		se_uploader->add_keys(se_keys);

	if( speed != 0 )
		se_uploader->connection_parameter(speed);

	try {
		if( se_uploader->upload(false,false,0,NT) )
			return _SE_OK;
	} catch(exception &e) {
		if( e.what() == string("abort") )
			return _SE_UPLOAD_CANCELLED;
	}
	return _SE_OTHER_ERROR;
}

int CALL_API se_disable_mps() {
	if( !se_module_init ) return _SE_NOT_INITIATED;

	se_uploader->disable_mps();
	return _SE_OK;
}

int CALL_API se_disable_output() {
	if( !se_module_init ) return _SE_NOT_INITIATED;

	se_uploader->disable_output();
	return _SE_OK;
}

#endif

