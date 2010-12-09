/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

#ifndef __ERROR_H
#define __ERROR_H

#ifdef _WIN32
#include <windows.h>
#endif

class errorClass {
	static char		warn_messages[30][200];

public:
	static const	int PW01 = 1;
	static const	int PE01 = PW01+1;
	static const	int PE02 = PW01+2;
	static const	int PE03 = PW01+3;
	static const	int PE04 = PW01+4;
	static const	int PE05 = PW01+5;
	static const	int PE06 = PW01+6;
	static const	int PE07 = PW01+7;
	static const	int PE08 = PW01+8;
	static const	int PE09 = PW01+9;
	static const	int PE10 = PW01+10;
	static const	int PE11 = PW01+11;
	static bool		show_message_error;

	void static showError(int line,const char* file_name,int code);
};

#endif
