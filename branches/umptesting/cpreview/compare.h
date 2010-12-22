/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

#ifndef __COMPARE_H
#define __COMPARE_H

namespace g_compare {
	void init(int codepage);
	int get_codepage();
	int get_lang_id();
	int compare(void*,int,const void*,int,const void*);
}


#endif