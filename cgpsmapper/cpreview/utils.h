/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

#ifndef UtilsH
#define UtilsH

#include <string>
#ifdef LINUX
//#include "ITOA.H"
#endif

using namespace std;

string format_double(string format,double value);
string trim(string text);
string trimString(string text);
string upper_case(string text);
string lower_case(string text);
string first_up(string text);
string remove_enter(string text);
string legalize_to_lbl(string text);
string from6to8(string text);
string filename_id(string text);
/*return file id*/
string return_file_id(string file);

//---------------------------------------------------------------------------
#endif
