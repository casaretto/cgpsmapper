/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/
#ifndef __GUI_SENDMAP
#define __GUI_SENDMAP
#ifdef _WIN32

#include <windows.h>
#endif
#include <string>
#include <vector>
#include "resource.h"
#include "uploader.h"

#ifndef _DLL_VERSION
#ifdef _WIN32
using namespace std;

class GUI_manager {
public: 
	map_uploader*	uploader;
	HWND			dialog;
	HWND			IMG_list;
	HWND			PORT_list;
	HWND			SPEED_list;
	HWND			GMAPSUPP_check;
	HWND			EXE_check;
	HWND			mapset_name;
	HWND			region_name;
	HWND			key_value;

	HWND			gui_hwndDlg;
	HWND			old_header;
	HWND			nt_format;

	vector<string>	selected_img;
	vector<string>	key_list;

	bool	sync_mode;
	bool	gmap_create;

	GUI_manager();
	
	bool	connect(bool detect);
	void	open_file_add_IMG();
	void	store_files();
	void	add_IMG(const char* full_path_file_name);
	bool	remove_IMG();
	void	erase();
	void	upload();
	void	show_size();
	void	create_exe_file(HWND hwndDlg);
	void	create_GMAPSUPP();
	void	create_exe();
	void	add_unlock_key();
	void	set_mapset_name();

	void	enter_sync_mode(bool verbose = true);
};

int CALLBACK GUI_Dialog(HWND hwndDlg, UINT uMsg,WPARAM wParam, LPARAM lParam);
#endif

map_uploader*	connect_to_gps(bool t_flash_id = false);
bool			connect_to_gps(map_uploader* uploader,bool detect = false);
bool			detect_port(map_uploader* uploader,string port,bool detect = false);
map_uploader*	detect_port(string port,bool detect = false,bool t_flash_id = false);
int				process_GUI();

#endif
#endif
