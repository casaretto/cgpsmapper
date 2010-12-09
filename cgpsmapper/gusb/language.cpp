/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/
#include "language.h"

void	changeTitle(HWND dialog,int item,int resource) {
	char	buffer[2000];
	int		r_diff = -1000;

	if( (GetUserDefaultLangID() & 0x3ff ) == LANG_POLISH )
		r_diff = 0;
	else if( (GetUserDefaultLangID() & 0x3ff ) == LANG_FRENCH )
		r_diff = 1000;
	
	if( LoadString(NULL,resource + r_diff,buffer,sizeof buffer) )
		SendMessage(GetDlgItem (dialog, item),WM_SETTEXT,0,LPARAM(buffer));
}

void	loadString(int resource,char* buffer,size_t b_size) {
	int		r_diff = -1000;

	buffer[0] = 0;

	if( (GetUserDefaultLangID() & 0x3ff ) == LANG_POLISH )
		r_diff = 0;
	else if( (GetUserDefaultLangID() & 0x3ff ) == LANG_FRENCH )
		r_diff = 1000;
	
	LoadString(NULL,resource + r_diff,buffer,b_size);		
}
