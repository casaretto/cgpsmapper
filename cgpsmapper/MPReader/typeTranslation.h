#ifndef __TYPE_TRANSLATION
#define __TYPE_TRANSLATION

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cctype>

class RGN_names
{
public:
	std::string name;
	int  start;
	int  range;

	RGN_names(std::string name,int start,int range) 
	{	this->name = name; 
		this->start = start; 
		this->range = range;
	};
	RGN_names(const RGN_names& t) {
		this->name = t.name;
		this->start = t.start;
		this->range = t.range;
	};
	RGN_names() {
		start = 0;
		range = 0;
	};
};


class typeReader {
public:
	bool	dataReady;
	typeReader() { dataReady = false;};
	void Read(const char* fileName);
	void InterpreteRGNTypeLine(std::string line,int &rgn_start,int &rgn_end,std::string &name);

	int  findPolyline(const char* rgn_name);
	int  findPoi(const char* rgn_name);
	int  findPolygon(const char* rgn_name);

	std::map<std::string,RGN_names>::iterator	c_I;
	std::map<std::string,RGN_names> c_RGN_names_1020; //poi
	std::map<std::string,RGN_names> c_RGN_names_40; //line
	std::map<std::string,RGN_names> c_RGN_names_80; //poly
};

#endif