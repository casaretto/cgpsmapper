/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/
#ifndef __PROGRESSH
#define __PROGRESSH
#ifdef _WIN32
#include <windows.h>
#endif
class progress {
public:
	static	int	percent;
	static	int	percent_m;
	static	bool gui;
#ifdef _WIN32
	static	HWND dialog;
#endif
	static	void show_gui(bool downloader = false);
	static	void hide_gui();
	void calculate(int position,int total,bool downloader = false,bool verbose = true);
};

#endif
