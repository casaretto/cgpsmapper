/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/
#ifndef _SENDMAP_DLL
#define _SENDMAP_DLL

#ifdef __cplusplus
extern "C" {
#endif

#undef LIBSPEC
#undef CALL_API

#ifndef _WIN32
 #define LIBSPEC  
 #define CALL_API  
#else
  #ifdef _DLL_VERSION
	#define LIBSPEC __declspec(dllexport)
	#define CALL_API __stdcall
  #else
	#define LIBSPEC __declspec(dllimport)
	#define CALL_API __stdcall
  #endif

#endif

#define	_SE_OK						1
#define _SE_NOT_INITIATED			-1
#define _SE_COULD_NOT_CONNECT		-2
#define _SE_NOT_CONNECTED			-3
#define _SE_UPLOAD_CANCELLED		-4
#define _SE_NO_FILE					-5
#define _SE_OTHER_ERROR				-10

/**
  Initialisation of the send library - tries to connect to the GPS
  @param	port_type			0 - RS connection (COM port), 1 - USB (requires Garmin USB driver installed in the system), 2 - file will be created on disk, port_name will be treated as a filename
  @param	port_name			for RS connection - com port name e.g. "COM1", for USB should be "USB", for option 2 above - filename
  
  @return	_SE const			_SE_OK - connection established
*/
LIBSPEC int CALL_API se_connect(int port_type,const char* port_name,const char* copyright);

/**
  Close connection
*/
LIBSPEC int CALL_API se_disconnect();

/**
  Get the unique ID of the GPS
  @return	GPS ID
*/
LIBSPEC unsigned int CALL_API se_get_id();

/**
  Get the avail. memory size
  @return	GPS ID
*/
LIBSPEC int CALL_API se_get_memory();

/**
  Get the software version of the GPS
  @return	
*/
LIBSPEC const char* CALL_API se_get_version();

/**
  Erase GPS map memory
  @return	_SE_CONST
*/
LIBSPEC int CALL_API se_erase_memory();

/**
  Add unlock keys - required if maps are locked
  @return	_SE_CONST
*/
LIBSPEC int CALL_API se_add_key(const char* key);

/**
  Set the mapset name
  @return	_SE_CONST
*/
LIBSPEC int CALL_API se_set_mapset(const char* name);

/**
  Add single IMG file to be uploaded
  @param	file_name		name of the IMG file
  @param	region_name		name of the region to which belongs the map

  @return	_SE_CONST
*/
LIBSPEC int CALL_API se_add_file(const char* file_name,const char* region_name);

/**
  Add single internal file from IMG file 
  @param	file_name		name of the IMG file
  @param	selected_file	name of the internal file to be uploaded
  @param	selected_file_rename	new name of the internal file to be uploaded, if NULL, no change in name

  @return	_SE_CONST
*/
LIBSPEC int CALL_API se_add_file_internal(const char* file_name,const char* selected_file,const char* selected_file_rename);

/**
  Add single IMG file to be uploaded crypted using cgpsmapper DLL library
  @param	file_name		name of the IMG file
  @param	password		up to 256 characters, even number of chars
  @param	region_name		name of the region to which belongs the map

  @return	_SE_CONST
*/
LIBSPEC int CALL_API se_add_crypt_file(const char* file_name,const char* region_name,const char* password);


/**
  Add single IMG file to be uploaded - only specified part of the file will be upload - can be
  used if more than one IMG file exist in global file

  @param	file_name		name of the IMG file
  @param	start_data		offset in the file to the IMG data
  @param	len_data		length of the IMG file inside the file

  @return	_SE_CONST
*/
LIBSPEC int CALL_API se_add_raw_file(const char* file_name,int start_data,int len_data);

/**
  Add IMG file which has been merged with the DLL library

  @return	_SE_CONST
*/

LIBSPEC int CALL_API se_add_dll_file();

/**
  Upload process callback function
*/
typedef int (CALL_API *_UPLOADER)(int position,int total);

/**
  Uploads all the specified IMG files to the GPS

  @return	_SE_CONST
*/
LIBSPEC int CALL_API se_upload(_UPLOADER uploaderCallback,int speed,bool NT=false);

LIBSPEC int CALL_API se_upload_no_callback(int speed);

LIBSPEC int CALL_API se_upload_no_callback_NT(int speed);

LIBSPEC int CALL_API se_disable_mps();

LIBSPEC int CALL_API se_disable_output();

#undef LIBSPEC
#ifdef __cplusplus
}
#endif

#endif
