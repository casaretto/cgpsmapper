/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

#ifndef __TYPE_TRANSLATION
#define __TYPE_TRANSLATION

#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

class RGN_names
{
public:
	std::string name;
	int  start;
	int  range;
	//byte max_visible_zoom; //np 1 - widoczne w 0 i 1
	//byte type;    //tylko dla RGN10
                //0x01 - dodatkowy bajt typu
                //0x10 - opis typu EXIT
                //ox20 - opis typu POI
	RGN_names(const char* name,int start,int range) 
	{	this->name = name; 
		this->start = start; 
		this->range = range;};
};


class typeReader {
public:
	bool	dataReady;
	typeReader() { dataReady = false;};
	void Read(const char* fileName);
	void InterpreteRGNTypeLine(std::string line,int &rgn_start,int &rgn_end,std::string &name);

	int  SearchRGNName40(const char* rgn_name);

	std::vector<RGN_names> c_RGN_names_1020;
	std::vector<RGN_names> c_RGN_names_40;
	std::vector<RGN_names> c_RGN_names_80;
};

#endif