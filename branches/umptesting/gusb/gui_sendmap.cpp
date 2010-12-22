/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/
#ifdef _WIN32
#include "ListView.h"
#include "language.h"
#include "exe_create.h"
#endif
#include "gui_sendmap.h"

#ifndef _DLL_VERSION
#ifdef _WIN32

GUI_manager	gui_manager;
WNDPROC IMG_list_original_proc;
WNDPROC PORT_list_original_proc;

HCURSOR g_hWaitCursor;
HCURSOR g_hArrowCursor;

HWND	exe_dialog = NULL;
HWND	key_dialog = NULL;
HWND	exe_expiry_check;
HWND	exe_lock_check;
HWND	exe_copyright;
HWND	exe_expiry;
HWND	exe_lock;
HWND	exe_file_lic;
HWND	exe_file_exp;

extern t_sendmap sendmap_data;

/*************** EXE create *******************************************************/
void GUI_manager::create_exe_file(HWND hwndDlg) {
	OPENFILENAME ofn;
	char strFilesName[MAX_PATH + 1];
	char sendmap_exe[1000];
	int exp_day=0;
	int exp_month=0;
	int exp_year=0;
	unsigned int gps_id = 0;
	char	copyright_string[2048];
	char	licence_file[2048];
	char	expired_file[2048];
	char	temp_path[MAX_PATH];
	char	temp_file[MAX_PATH];
	char	temp[2000];
	char	temp_d[2000];
	int		result;

	strcpy(strFilesName,"MyMaps.exe");

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = strFilesName;

	//ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(strFilesName);
	ofn.lpstrFilter = "EXE files\0*.EXE\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	//ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT | OFN_EXPLORER;

	GetModuleFileName(NULL,sendmap_exe,1000);

	strcpy(copyright_string,sendmap_data.copyright_string);

	if (GetSaveFileName(&ofn)==TRUE) {
		memset(expired_file,0,400);
		memset(licence_file,0,400);

#if FULL == 1
		if( SendMessage(exe_expiry_check,BM_GETCHECK,0,0) == BST_CHECKED ) {
			GetWindowText(exe_expiry,temp,11);
			if( strlen(temp) == 10 ) {
				if( temp[2] == '/' && temp[5] == '/' ) {
					memset(temp_d,0,200);
					strncpy(temp_d,temp,2);
					exp_day = atoi(temp_d);

					strncpy(temp_d,&temp[3],2);
					exp_month = atoi(temp_d);

					strncpy(temp_d,&temp[6],4);
					exp_year = atoi(temp_d);
				}
			}

			if( !exp_day && !exp_month && !exp_year )
				MessageBox(NULL,"Date format must be DD/MM/YYYY !","Creating EXE uploader",MB_ICONINFORMATION | MB_OK);
		} 

		if( SendMessage(exe_lock_check,BM_GETCHECK,0,0) == BST_CHECKED ) {
			GetWindowText(exe_lock,temp,20);
			gps_id = strtoul(temp,NULL,10);
		} 

		GetWindowText(exe_file_lic,temp,400);
		if( strlen(temp) ) {
			if( temp[0] != '*' )
				strncpy(licence_file,temp,400);
		}

		GetWindowText(exe_file_exp,temp,400);
		if( strlen(temp) ) {
			if( temp[0] != '*' )
				strncpy(expired_file,temp,400);
		}

		GetWindowText(exe_copyright,temp,400);
		if( strlen(temp) ) {
			strncpy(copyright_string,temp,400);
		}
#endif

		GetTempPath(sizeof(temp_path),temp_path);
		GetTempFileName(temp_path,"~fr",0,temp_file);

		uploader->reconnect(resSIM,temp_file);
		set_mapset_name();
		if( key_list.size() ) 
			uploader->add_keys(key_list);
		uploader->upload(false);

		result = create_exe_package(gps_id,exp_day,exp_month,exp_year,sendmap_exe,
			copyright_string,licence_file,expired_file,ofn.lpstrFile,temp_file);

		if( result == 0 )
			MessageBox(NULL,"Done with success!","Creating EXE uploader",MB_ICONINFORMATION | MB_OK);
		else
			MessageBox(NULL,"Errors creating uploader - check trace window for details.","Creating EXE uploader",MB_ICONERROR | MB_OK);

		DeleteFile(temp_file);
	}
}

int CALLBACK EXE_Dialog(HWND hwndDlg, UINT uMsg,WPARAM wParam, LPARAM lParam) {

	char	sendmap_exe[1000];
	char	drive[10],dir[1024],fname[255],ext[255];
	string	t_name;

	switch (uMsg) {
		case WM_INITDIALOG:
			exe_expiry_check = GetDlgItem (hwndDlg, IDC_CHECK1);
			exe_lock_check = GetDlgItem (hwndDlg, IDC_CHECK2);
			exe_copyright = GetDlgItem( hwndDlg, IDC_EDIT_KEY5);
			exe_expiry = GetDlgItem( hwndDlg, IDC_EDIT_KEY);
			exe_lock = GetDlgItem( hwndDlg, IDC_EDIT_KEY2);
			exe_file_lic = GetDlgItem( hwndDlg, IDC_EDIT_KEY6);
			exe_file_exp = GetDlgItem( hwndDlg, IDC_EDIT_KEY7);

			GetModuleFileName(NULL,sendmap_exe,1000);
			_splitpath(sendmap_exe,drive,dir,fname,ext);

			SendMessage(exe_expiry, WM_SETTEXT, -1, LPARAM("01/12/2007")); 
			SendMessage(exe_lock, WM_SETTEXT, -1, LPARAM("0")); 

			t_name = drive;
			t_name += dir;
			t_name += "licence.txt";
			SendMessage(exe_file_lic, WM_SETTEXT, -1, LPARAM(t_name.c_str())); 

			t_name = drive;
			t_name += dir;
			t_name += "expired.txt";
			SendMessage(exe_file_exp, WM_SETTEXT, -1, LPARAM(t_name.c_str())); 

#if FULL == 1
			EnableWindow(exe_expiry,TRUE);
			EnableWindow(exe_expiry_check,TRUE);
			EnableWindow(exe_lock,TRUE);
			EnableWindow(exe_lock_check,TRUE);
			EnableWindow(exe_copyright,TRUE);
			EnableWindow(exe_file_lic,TRUE);
			EnableWindow(exe_file_exp,TRUE);
/*

			SendMessage(exe_expiry, WM_ENABLE,0,0);
			SendMessage(exe_expiry_check, WM_ENABLE,0,0);
			SendMessage(exe_lock, WM_ENABLE,0,0);
			SendMessage(exe_lock_check, WM_ENABLE,0,0);
			SendMessage(exe_copyright, WM_ENABLE,0,0);
			SendMessage(exe_file_lic, WM_ENABLE,0,0);
			SendMessage(exe_file_exp, WM_ENABLE,0,0);*/
#endif
			break;
        case WM_COMMAND:
			switch(LOWORD(wParam)) {
				case IDC_BUTTON_GET_ID:
					if( gui_manager.connect(true) ) {
						char buffer[30];
						unsigned int t_id = gui_manager.uploader->id();
						sprintf(buffer,"%u",t_id);
						SendMessage(exe_lock, WM_SETTEXT, -1, LPARAM(buffer)); 
					}
					break;
				case IDC_EXE_CREATE:
					gui_manager.create_exe_file(hwndDlg);
					break;
				case IDC_EXIT:
				case 2:
					EndDialog(hwndDlg,0);
					exe_dialog = NULL;
					return 0;
			}
			break;
		case WM_DESTROY: 
			PostQuitMessage(0); 
			return 0;
	}

	return 0;
}


/**********************************************************************************/

GUI_manager::GUI_manager() {
	sync_mode = false;
	uploader = new map_uploader(resSIM,"SIM");
	gmap_create = false;
}

void GUI_manager::create_exe() {
	
	int exp_day=0;
	int exp_month=0;
	int exp_year=0;
	unsigned int gps_id = 0;
	char sendmap_exe[2024];

	if( uploader->no_files() ) return;

	if( exe_dialog != NULL ) return;

	exe_dialog = CreateDialog(NULL,MAKEINTRESOURCE(ID_EXE_CREATE),gui_manager.dialog,EXE_Dialog);

	GetModuleFileName(NULL,sendmap_exe,1000);

//	create_exe_package(gps_id,exp_day,exp_month,exp_year,sendmap_exe,
//						std::string copyright_string,std::string licence_file,std::string expired_file,std::string sendmap_exe_img);
}

int CALLBACK KEY_Dialog(HWND hwndDlg, UINT uMsg,WPARAM wParam, LPARAM lParam) {

	switch (uMsg) {
		case WM_INITDIALOG:
			SendMessage(GetDlgItem (hwndDlg, IDC_EDIT_ENTER_KEY),WM_SETTEXT,0,LPARAM("enter unlock key here"));
			break;
        case WM_COMMAND:
			switch(LOWORD(wParam)) {
				case IDC_ADD:
				{
					char    tmp[1024];
					char	b_title1[500];
					char	b_title2[500];
					GetWindowText(GetDlgItem (hwndDlg, IDC_EDIT_ENTER_KEY),tmp,1000);
					string t_key = tmp;
					while( t_key.find("-") < t_key.size() )
						t_key.erase(t_key.find("-"),1);

					if( t_key.size() != 25 ) {
						loadString(IDS_WRONG_KEY,b_title1,sizeof(b_title1));
						loadString(IDS_WRONG_KEY_TITLE,b_title2,sizeof(b_title2));

						MessageBox(NULL,b_title1,b_title2,MB_ICONERROR | MB_OK);
						return 1;
					} else
						gui_manager.key_list.push_back(t_key.c_str());
					EndDialog(hwndDlg,0);
					key_dialog = NULL;
				}
				break;
				case IDC_EXIT:
				case 2:
					EndDialog(hwndDlg,0);
					key_dialog = NULL;
					return 0;
			}
			break;
		case WM_DESTROY: 
			PostQuitMessage(0); 
			return 0;
	}

	return 0;
}

void GUI_manager::add_unlock_key() {
	if( key_dialog != NULL ) return;

	key_dialog = CreateDialog( NULL,MAKEINTRESOURCE(ID_KEY), gui_manager.dialog ,KEY_Dialog);

	/*
	char tmp[1024];
	char	b_title1[500];
	char	b_title2[500];
	GetWindowText(key_value,tmp,1000);

	if( strlen(tmp) != 25 ) {
		loadString(IDS_WRONG_KEY,b_title1,sizeof(b_title1));
		loadString(IDS_WRONG_KEY_TITLE,b_title2,sizeof(b_title2));

		MessageBox(NULL,b_title1,b_title2,MB_ICONERROR | MB_OK);
		SetWindowText(key_value,"");
		return;
	}
	key_list.push_back(tmp);
	SetWindowText(key_value,"");
*/
}

void GUI_manager::set_mapset_name() {
	char tmp[1024];
	bool old_header_value = false;
	GetWindowText(mapset_name,tmp,1000);
	uploader->set_mapset_name(tmp);

	if( SendMessage(old_header,BM_GETCHECK,0,0) == BST_CHECKED ) {
		old_header_value = true;

	} else 
		old_header_value = false;

	//IMG::set_old_header(old_header_value);
}

void GUI_manager::enter_sync_mode(bool verbose) {
	char	b_title1[500];
	char	b_title2[500];
	int		item = ListView_GetItemCount(IMG_list);
	vector<internal_file> TRE_file_list;

	sync_mode = false;
	if( connect(true) == false )
		return;

	SetSelection(IMG_list,0);
	while( remove_IMG() )
		;

	loadString(IDS_SYNC_START,b_title1,sizeof(b_title1));
	loadString(IDS_SYNC_TITLE,b_title2,sizeof(b_title2));

	if( verbose ) {
		if( MessageBox(NULL,b_title1,b_title2,MB_ICONERROR | MB_YESNO) == IDYES ) {
			sync_mode = true;
		}
	} else 
		sync_mode = true;
	
	if( sync_mode ) {
		EnableWindow(GetDlgItem (gui_hwndDlg, IDC_IMG_FILE),FALSE);
		EnableWindow(GetDlgItem (gui_hwndDlg, IDC_EXE_FILE),FALSE);
		EnableWindow(GetDlgItem (gui_hwndDlg, IDC_STORE_FILES),FALSE);
		

		SetCursor (g_hWaitCursor);
			
		if( uploader->download_directory(&TRE_file_list) == true ) {
			item = ListView_GetItemCount(IMG_list);
			for( vector<internal_file>::iterator f = TRE_file_list.begin(); f < TRE_file_list.end(); f++ ) {
				InsertItem(IMG_list, item , LPARAM("%s"),(*f).region_name.c_str());
				SetSubItemText (IMG_list, item, 1, "%s", (*f).TRE_map_name.c_str());
				SetSubItemText (IMG_list, item, 2, "%s", (*f).file_name.c_str());
				SetSubItemText (IMG_list, item, 3, "%s", (*f).get_internal_short_name());
			}
			AutoSizeColumns(IMG_list);
			show_size();
		}
		SetCursor (g_hArrowCursor);
	}
}

bool GUI_manager::connect(bool detect) {
	bool	t_ret = false;

	SetCursor (g_hWaitCursor);
	t_ret = connect_to_gps(uploader,detect);
	SetCursor (g_hArrowCursor);

	if( t_ret ) {
		//EnableWindow(GetDlgItem (dialog, ID_CONNECT),FALSE);
		//EnableWindow(gui_manager.PORT_list,FALSE);
		SendDlgItemMessage(dialog,IDC_GPS_TEXT,WM_SETTEXT,0,LPARAM(uploader->get_gps_version()));
	} else
		SendDlgItemMessage(dialog,IDC_GPS_TEXT,WM_SETTEXT,0,LPARAM("NO GPS FOUND"));
	
	return t_ret;
}

void GUI_manager::store_files() {
	OPENFILENAME ofn;
	char strFilesName[MAX_PATH + 1];

	if( gmap_create ) return;
	if( uploader->no_files() ) return;
	strcpy(strFilesName,"filelist.txt");

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = strFilesName;

	//ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(strFilesName);
	ofn.lpstrFilter = "TXT files\0*.TXT\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;

	gmap_create = true;
	if (GetSaveFileName(&ofn)==TRUE) {
		uploader->store_file_list(ofn.lpstrFile);
	}
	gmap_create = false;
}

void GUI_manager::create_GMAPSUPP() {
	OPENFILENAME ofn;
	char strFilesName[MAX_PATH + 1];

	if( gmap_create ) return;

	if( uploader->no_files() ) return;

	strcpy(strFilesName,"GMAPSUPP.IMG");

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = strFilesName;

	//ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(strFilesName);
	ofn.lpstrFilter = "IMG files\0*.IMG\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	//ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT | OFN_EXPLORER;

	gmap_create = true;
	if (GetSaveFileName(&ofn)==TRUE) {
		uploader->reconnect(resSIM,ofn.lpstrFile);
		
		set_mapset_name();
		if( key_list.size() ) uploader->add_keys(key_list);

		bool nt = SendMessage(gui_manager.nt_format, BM_GETCHECK , 0, 0);
		uploader->upload(false,false,0,nt);
	}
	gmap_create = false;
}

void GUI_manager::open_file_add_IMG() {
	OPENFILENAME ofn;
	char strFilesName[(MAX_PATH + 1)*100 + 1];

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = strFilesName;

	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(strFilesName);
	ofn.lpstrFilter = "Supported files\0*.IMG;*.TYP;*.RGN;*.LBL;*.TRE;*.NET;*.NOD;*.MDR;*.TXT\0All files\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT | OFN_EXPLORER;

	if (GetOpenFileName(&ofn)==TRUE) {
		string	strPath;
		bool	one_file = TRUE;

		const char * p = strFilesName;
		while (* p) {
			const char * pEnd = p + ::strlen (p);

			string strFileName (p, pEnd);
			if (strPath.empty ())
				strPath.swap (strFileName);
			else {
				one_file = FALSE;
				add_IMG(string(strPath + "\\" + strFileName).c_str());
			}

			p = pEnd + 1;
		}
		if( one_file ) {
			add_IMG(strPath.c_str());
		}

	}
}

void GUI_manager::add_IMG(const char* full_path_file_name) {
	int				item = ListView_GetItemCount(IMG_list);
	vector<internal_file>	TRE_file_list;
	char	tmp[1024];
	char	local_file_name[1024];
	char	t_file_type[255];
	char	drive[5];
	char	dir[1024];

	GetWindowText(region_name,tmp,1000);

	_splitpath(full_path_file_name,drive,dir,local_file_name,t_file_type);

	if( !strcmp(strupr( t_file_type ),".TXT" ) ) {
		FILE*	list = fopen(full_path_file_name,"r");
		char	file_name[1025];
		strcpy(tmp,"map");
		if( list ) {
			while( fgets(file_name,1024,list) != NULL ) {
				//remove 'non asci' chars!
				while( file_name[strlen(file_name)-1] < 32 && strlen(file_name))
					file_name[strlen(file_name)-1] = 0;
				if( strlen(file_name) ) {
					if( file_name[0] == ':' ) {
						strcpy(tmp,&file_name[1]);
					} else {
						uploader->add_img_file(file_name,&TRE_file_list,"",0,0,tmp,uploader->get_product_id(tmp));
					}
				}
			}
			fclose(list);
		}
	} else {
		uploader->add_img_file(full_path_file_name,&TRE_file_list,"",0,0,tmp,uploader->get_product_id(tmp));
	}
	
	for( vector<internal_file>::iterator f = TRE_file_list.begin(); f < TRE_file_list.end(); f++ ) {
		InsertItem(IMG_list, item , LPARAM("%s"),(*f).region_name.c_str());
		SetSubItemText (IMG_list, item, 1, "%s", (*f).TRE_map_name.c_str());
		SetSubItemText (IMG_list, item, 2, "%s", (*f).file_name.c_str());
		SetSubItemText (IMG_list, item, 3, "%s", (*f).get_internal_short_name());
	}
	AutoSizeColumns(IMG_list);
	show_size();
	
}

void GUI_manager::show_size() {
	float	size = static_cast<float>((uploader->get_files_size()) / (1024.0*1024.0));//Mb
	char	b_size[16];

	sprintf(b_size,"%.2f Mb",size);

	SendDlgItemMessage(dialog,IDC_SIZE_TOTAL,WM_SETTEXT,0,LPARAM(b_size));
}

bool GUI_manager::remove_IMG() {
	char	buffer[MAX_PATH+1];
	char	internal_name[20];
	int		item = ListView_GetSelectionMark(IMG_list);
	int		count = ListView_GetItemCount(IMG_list);
	bool	removed = false;
	
	if( item > -1 ) {
		ListView_GetItemText(IMG_list,item,2,buffer,MAX_PATH);
		ListView_GetItemText(IMG_list,item,3,internal_name,sizeof internal_name);
		if( strlen(buffer) && strlen(internal_name) ) {
			if( uploader->remove_img_file(buffer,internal_name) && !removed ) 
				removed = true;
		}
		if( ListView_DeleteItem(IMG_list,item) ) {
			removed = true;
			if( (item+1) >= count )
				item--;
		}
		SetSelection(IMG_list,item);
		show_size();
	}
	uploader->remove_all();
	return removed;
}

void GUI_manager::upload() {
	if( uploader->no_files() ) 
		return;

	set_mapset_name();
	if( key_list.size() ) uploader->add_keys(key_list);

	if( connect(false) ) {
		progress::show_gui(sync_mode);		
			
		bool nt = SendMessage(gui_manager.nt_format, BM_GETCHECK , 0, 0);
		uploader->upload(false,false,0,nt);
		if( sync_mode == true ) {
			sync_mode = false;

			EnableWindow(GetDlgItem (gui_hwndDlg, IDC_IMG_FILE),TRUE);
			EnableWindow(GetDlgItem (gui_hwndDlg, IDC_EXE_FILE),TRUE);
			EnableWindow(GetDlgItem (gui_hwndDlg, IDC_STORE_FILES),TRUE);

			enter_sync_mode(false);
		}
		progress::hide_gui();
	}
}

void GUI_manager::erase() {
	if( connect(true) ) {
		SetCursor (g_hWaitCursor);
		if(	uploader->erase_only() ) {
			SetCursor (g_hArrowCursor);
			return; 
		}
	}
	SetCursor (g_hArrowCursor);
}

LRESULT CALLBACK IMG_list_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) { 
        case WM_KEYDOWN: 
			switch(wParam) {
				case VK_DELETE:
					gui_manager.remove_IMG();
					break;
			}
	}
	return CallWindowProc(IMG_list_original_proc, hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK PORT_list_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) { 
		case WM_COMMAND :
			switch(HIWORD(wParam)) {
                case CBN_SELCHANGE : 
					if( SendMessage(gui_manager.PORT_list,CB_GETCURSEL , 0, 0) > 1 )
						EnableWindow(gui_manager.SPEED_list,TRUE);
					else
						EnableWindow(gui_manager.SPEED_list,FALSE);
					break;
			}
	}
	return CallWindowProc(PORT_list_original_proc, hwnd, msg, wParam, lParam);
}

int CALLBACK GUI_Dialog(HWND hwndDlg, UINT uMsg,WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_INITDIALOG:
			
			gui_manager.gui_hwndDlg = hwndDlg;
			//Initialisation of GUI here
			DragAcceptFiles(hwndDlg, true); 

			gui_manager.EXE_check = GetDlgItem (hwndDlg, IDC_EXE_FILE);
			gui_manager.GMAPSUPP_check = GetDlgItem (hwndDlg, IDC_IMG_FILE);
			gui_manager.nt_format = GetDlgItem (hwndDlg, IDC_NT_FORMAT);

			//ports			
			gui_manager.PORT_list = GetDlgItem (hwndDlg, IDC_PORTS);			

			//mapset
			gui_manager.mapset_name = GetDlgItem(hwndDlg, IDC_EDIT_MAPSET);

			//old header
			gui_manager.old_header = GetDlgItem(hwndDlg, IDC_OLD_HEADER);
#if FULL != 1
			EnableWindow(gui_manager.mapset_name,FALSE);
#endif

			SendMessage(gui_manager.old_header,BM_SETCHECK,0,0);

			//region
			gui_manager.region_name = GetDlgItem(hwndDlg, IDC_EDIT_REGION);

			//unlock key
			//gui_manager.key_value = GetDlgItem(hwndDlg, IDC_EDIT_KEY);

			SendMessage(gui_manager.PORT_list, CB_INSERTSTRING, -1, LPARAM("Auto")); 
			SendMessage(gui_manager.PORT_list, CB_INSERTSTRING, -1, LPARAM("usb")); 
			SendMessage(gui_manager.PORT_list, CB_INSERTSTRING, -1, LPARAM("com1")); 
			SendMessage(gui_manager.PORT_list, CB_INSERTSTRING, -1, LPARAM("com2")); 
			SendMessage(gui_manager.PORT_list, CB_INSERTSTRING, -1, LPARAM("com3")); 
			SendMessage(gui_manager.PORT_list, CB_INSERTSTRING, -1, LPARAM("com4")); 
			SendMessage(gui_manager.PORT_list, CB_INSERTSTRING, -1, LPARAM("com5")); 
			SendMessage(gui_manager.PORT_list, CB_INSERTSTRING, -1, LPARAM("com6")); 
			SendMessage(gui_manager.PORT_list, CB_INSERTSTRING, -1, LPARAM("com7")); 
			SendMessage(gui_manager.PORT_list, CB_INSERTSTRING, -1, LPARAM("com8")); 
			SendMessage(gui_manager.PORT_list, CB_INSERTSTRING, -1, LPARAM("com9")); 
			SendMessage(gui_manager.PORT_list, CB_SETCURSEL, 0, 0); 
			PORT_list_original_proc = (WNDPROC) SetWindowLong(gui_manager.PORT_list , GWL_WNDPROC, (DWORD) PORT_list_proc); 

			//speeds
			gui_manager.SPEED_list = GetDlgItem (hwndDlg, IDC_SPEEDS);

			SendMessage(gui_manager.SPEED_list, CB_INSERTSTRING, -1, LPARAM("115200")); 
			SendMessage(gui_manager.SPEED_list, CB_INSERTSTRING, -1, LPARAM("57600")); 
			SendMessage(gui_manager.SPEED_list, CB_INSERTSTRING, -1, LPARAM("38400")); 
			SendMessage(gui_manager.SPEED_list, CB_INSERTSTRING, -1, LPARAM("19200")); 
			SendMessage(gui_manager.SPEED_list, CB_INSERTSTRING, -1, LPARAM("9600")); 
			SendMessage(gui_manager.SPEED_list, CB_SETCURSEL, 1, 0); 

			EnableWindow(gui_manager.SPEED_list,FALSE);

			//IMG list
			gui_manager.IMG_list = GetDlgItem (hwndDlg, IDC_IMG_LIST);
			ListView_SetExtendedListViewStyleEx (gui_manager.IMG_list, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
			AddColumn (gui_manager.IMG_list, 0, "Region");
			AddColumn (gui_manager.IMG_list, 1, "Name");
			AddColumn (gui_manager.IMG_list, 2, "File");
			AddColumn (gui_manager.IMG_list, 3, "Internal");
			AutoSizeColumns(gui_manager.IMG_list);

			IMG_list_original_proc = (WNDPROC) SetWindowLong(gui_manager.IMG_list , GWL_WNDPROC, (DWORD) IMG_list_proc); 

			//Language - polski ;)
			changeTitle(hwndDlg,ID_CONNECT,IDS_CONNECT);
			changeTitle(hwndDlg,IDC_REMOVE_MAPS,IDS_REMOVE_MAPS);
			changeTitle(hwndDlg,IDC_ADD_MAPS,IDS_ADD_MAPS);
			changeTitle(hwndDlg,IDC_EXIT,IDS_EXIT);
			changeTitle(hwndDlg,IDC_UPLOAD,IDS_UPLOAD);
			changeTitle(hwndDlg,IDC_ERASE,IDS_ERASE);
			changeTitle(hwndDlg,IDC_MAPS_STATIC,IDS_MAPS_STATIC);
			changeTitle(hwndDlg,IDC_SIZE_STATIC,IDS_SIZE_STATIC);
			changeTitle(hwndDlg,IDC_DEVICE_STATIC,IDS_DEVICE_STATIC);
			changeTitle(hwndDlg,IDC_SPEED_STATIC,IDS_SPEED_STATIC);
			//changeTitle(hwndDlg,IDC_FOUND_STATIC,IDS_FOUND_STATIC);
			changeTitle(hwndDlg,IDC_SYNC_GPS,IDS_SYNC_GPS);

			changeTitle(hwndDlg,IDC_IMG_FILE,IDS_IMG_FILE);
			changeTitle(hwndDlg,IDC_EXE_FILE,IDS_EXE_FILE);
			
			changeTitle(hwndDlg,IDC_ADD_UNLOCK,IDS_ADD_UNLOCK);

			SendMessage(GetDlgItem (hwndDlg, IDC_EDIT_MAPSET),WM_SETTEXT,0,LPARAM("Maps uploaded with SendMap"));
			//SendMessage(GetDlgItem (hwndDlg, IDC_EDIT_KEY),WM_SETTEXT,0,LPARAM("enter unlock key here"));
			SendMessage(GetDlgItem (hwndDlg, IDC_EDIT_REGION),WM_SETTEXT,0,LPARAM("mapset"));
			break;
		case WM_DROPFILES: {
			const HDROP hDrop = reinterpret_cast <HDROP> (wParam);
			UINT c = 0;
			while (true) {
				char strFileName [MAX_PATH + 1];
				if (::DragQueryFile (hDrop, c ++, strFileName, MAX_PATH) == 0)
					break;

				gui_manager.add_IMG(strFileName);
			}
			::DragFinish (hDrop);
			break;
			}
        case WM_COMMAND:
			switch(LOWORD(wParam)) {
				case IDC_STORE_FILES:
					gui_manager.store_files();
					break;
				case IDC_EXE_FILE:
					gui_manager.create_exe();
					break;
				case IDC_IMG_FILE:
					gui_manager.create_GMAPSUPP();
					break;
				case IDC_ADD_UNLOCK:
					gui_manager.add_unlock_key();
					break;
				case IDC_SYNC_GPS:
					gui_manager.enter_sync_mode();
					break;
				case IDC_ADD_MAPS:
					gui_manager.open_file_add_IMG();
					break;
				case IDC_EXIT:
				case 2:
					DestroyWindow(gui_manager.dialog );
					return 0;
				case IDC_ERASE:
					gui_manager.erase();
					break;
				case ID_CONNECT:
					gui_manager.connect(true);
					break;
				case IDC_REMOVE_MAPS:
					gui_manager.remove_IMG();
					break;
				case IDC_UPLOAD:
					gui_manager.upload();
					break;
			}
			break;
		case WM_DESTROY: 
			PostQuitMessage(0); 
			return 0;
	}

	return 0;
}
#endif
#endif

//helpers
bool detect_port(map_uploader* uploader,string port,bool detect) {
	comm_medium_type port_type;

	cout<<"Trying: "<<port<<endl;
	if( port == "usb" ) {
		port_type = resUSB;
	} else if (port =="sim" ) {
		port_type = resSIM;
		port = "GMAPSUPP.IMG"; 
		if( detect )
			port = "CHECKCONNECTION";
	} else {
		port_type = resCOM;
	}
	uploader->reconnect(port_type,port.c_str());
	if( uploader->check_connection() == false )
		return false;
	return true;
}

map_uploader*	detect_port(string port,bool detect,bool t_flash_id) {
	comm_medium_type port_type;
	map_uploader*	uploader = NULL;

	cout<<"Trying: "<<port<<endl;
	if( port == "usb" ) {
		port_type = resUSB;
	} else if (port == "sim" ) {
		port_type = resSIM;
		port = "GMAPSUPP.IMG";
		if( detect )
			port = "CHECKCONNECTION";
	} else {
		port_type = resCOM;
	}

	uploader = new map_uploader(port_type,port.c_str(),false,t_flash_id);
	if( uploader->check_connection() == false ) {
		delete uploader;
		uploader = NULL;
	}

	return uploader;
}

#ifndef _DLL_VERSION

#ifdef _WIN32
bool connect_to_gps(map_uploader* uploader,bool detect) {
	LRESULT	port;
	LRESULT	speed;
	char	b_port[10];
	char	b_title1[200];
	char	b_title2[200];
	//if( uploader->connetcion_established() && !uploader->is_simulator() )
	//	return true;

	speed = SendMessage(gui_manager.SPEED_list,CB_GETCURSEL , 0, 0);
	if( speed > 0 ) {
		switch( speed ) {
			case 1: uploader->connection_parameter(57600);break;
			case 2: uploader->connection_parameter(38400);break;
			case 3: uploader->connection_parameter(19200);break;
			default: uploader->connection_parameter(9600);break;
		}
	}

	port = SendMessage(gui_manager.PORT_list,CB_GETCURSEL , 0, 0);
	if( port > 0 ) {
		SendMessage(gui_manager.PORT_list,CB_GETLBTEXT , port, LPARAM(b_port));
		if( detect_port(uploader,b_port) ) 
			return true;
		return false;
	}

	if( detect_port(uploader,"usb") ) return true;
	if( detect_port(uploader,"sim",detect) ) return true;
	if( detect_port(uploader,"com1") ) return true;
	if( detect_port(uploader,"com2") ) return true;
	if( detect_port(uploader,"com3") ) return true;
	if( detect_port(uploader,"com4") ) return true;
	if( detect_port(uploader,"com5") ) return true;
	if( detect_port(uploader,"com6") ) return true;
	if( detect_port(uploader,"com7") ) return true;
	if( detect_port(uploader,"com8") ) return true;
	if( detect_port(uploader,"com9") ) return true;

	loadString(IDS_NO_GPS,b_title1,sizeof(b_title1));
	loadString(IDS_NO_GPS_TITLE,b_title2,sizeof(b_title2));

	MessageBox(NULL,b_title1,b_title2,MB_ICONERROR | MB_OK);
	return false;
}
#endif


map_uploader*	connect_to_gps(bool t_flash_id) {
	map_uploader* uploader = NULL;
	
	uploader = detect_port("usb",false,t_flash_id);
	if( uploader ) return uploader;

	uploader = detect_port("sim");
	if( uploader ) return uploader;

	uploader = detect_port("com1");
	if( uploader ) return uploader;
	
	uploader = detect_port("com2");
	if( uploader ) return uploader;
	
	uploader = detect_port("com3");
	if( uploader ) return uploader;
	
	uploader = detect_port("com4");
	if( uploader ) return uploader;
	
	uploader = detect_port("com5");
	if( uploader ) return uploader;
	
	uploader = detect_port("com6");
	if( uploader ) return uploader;
	
	uploader = detect_port("com7");
	if( uploader ) return uploader;
	
	uploader = detect_port("com8");
	if( uploader ) return uploader;
	
	uploader = detect_port("com9");
	if( uploader ) return uploader;
#ifdef _WIN32
	if( !uploader ) {
		MessageBox(NULL,
			"Check if your GPS is turned ON and connected to the computer.\nIf you are using USB port - be sure to use latest Garmin USB drivers!",
			"Cannot detect GPS",MB_ICONERROR | MB_OK);
			return NULL;
		}
#endif
	return uploader;
}

#ifdef _WIN32
int process_GUI() {
	MSG	msg;
	g_hArrowCursor = ::LoadCursor (NULL, IDC_ARROW);
	g_hWaitCursor  = ::LoadCursor (NULL, IDC_WAIT);

	gui_manager.dialog = CreateDialog(NULL,MAKEINTRESOURCE(ID_MAIN),NULL,GUI_Dialog);
	
	BOOL bRet;

	while ( (bRet = GetMessage(&msg, NULL, 0, 0)) != 0 ) { 
		if (bRet == -1 ) {
        // handle the error and possibly exit
		} else if (!IsWindow(gui_manager.dialog) || !IsDialogMessage(gui_manager.dialog, &msg)) { 
			TranslateMessage(&msg); 
			DispatchMessage(&msg); 
		}
	}
	return 0;
}

#endif

#endif