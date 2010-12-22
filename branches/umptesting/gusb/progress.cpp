/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#include <commctrl.h>
#endif
#include <stdexcept>
#include "progress.h"
#include "resource.h"
#include "language.h"

using namespace std;

int progress::percent = 0;
int progress::percent_m = 0;
bool progress::gui = false;
#ifdef _WIN32
HWND progress::dialog = NULL;

#ifdef _DLL_VERSION
#ifndef _SENDMAP_LIB
#include "sendmapDLL.h"
extern _UPLOADER uploaderC;
#endif
#endif
#endif

void progress::show_gui(bool downloader) {
	gui = true;
#ifndef _SENDMAP_LIB
#ifdef _WIN32
	if( downloader ) {
		dialog = CreateDialog(NULL,MAKEINTRESOURCE(ID_PROGRESS_SYNC),NULL,NULL);
		SendMessage(GetDlgItem (dialog, IDC_PROGRESS_DOWN),PBM_SETSTEP,1,0);
		SendMessage(GetDlgItem (dialog, IDC_PROGRESS_DOWN),PBM_SETRANGE,0,MAKELPARAM (0, 1000));
	} else
		dialog = CreateDialog(NULL,MAKEINTRESOURCE(ID_PROGRESS),NULL,NULL);
	ShowWindow(dialog,SW_SHOW);
	changeTitle(dialog,ID_PROGRESS,IDS_SENDING); 	
	SendMessage(GetDlgItem (dialog, IDC_PROGRESS),PBM_SETSTEP,1,0);
#endif
#endif
}

void progress::hide_gui() {
#ifndef _SENDMAP_LIB
#ifdef _WIN32
	if( gui && dialog )
		DestroyWindow(dialog);
#endif
#endif
	gui = false;
}

void progress::calculate(int position,int total,bool downloader, bool verbose) {
	float p = (float)position;
	float t = (float)total;
	float di = 100.0;

#ifndef _SENDMAP_LIB
#ifdef _DLL_VERSION
	if( uploaderC == NULL )
		return;
	if( uploaderC(position,total) == 0 )
		throw runtime_error("abort");
	return;
#endif

	if( downloader ) {
		if( int((p / t) * 1000.0) != percent_m ) {
			percent_m = int((p / t)*1000.0);
#ifdef _WIN32
			SendMessage(GetDlgItem (dialog, IDC_PROGRESS_DOWN),PBM_SETPOS,int((p / t) * 1000.0),0);
#endif
		}
	}

	if( int((p / t) * di) != percent ) {
		percent = int((p / t)*di);
		if( verbose )
			cout<<"\r"<<percent<<"%"<<flush;
#ifdef _WIN32
		if( gui && dialog ) {
			if( !downloader ) {
				SendMessage(GetDlgItem (dialog, IDC_PROGRESS),PBM_SETPOS,percent,0);
			}
		}
#endif
	}
#endif
}
