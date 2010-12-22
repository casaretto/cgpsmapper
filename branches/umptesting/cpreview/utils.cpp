/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/


#include <stdio.h>

#include "utils.h"

//copy from LBL_header
const char LBL_valid_chars[] =
{' ','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o',
 'p','q','r','s','t','u','v','w','x','y','z','0','1','2','3','4',
 '5','6','7','8','9','@','!','"','#','$','%','&',39 ,'(',')','*',
 '+',',','-','.','/',':',';','<','=','>','?',91 ,92 ,93 ,30 ,'~'};

string format_double(string format,double value)
{
  char buffer[120];
  sprintf(buffer,format.c_str(),value);
  return string(buffer);
}

string filename_id(string text) {
	if( text.find(".") < text.size() ) {
		//extension
		text = text.substr(0,text.find("."));
	}

	while( text.find("\\") < text.size() ) {
		text = text.substr(text.find("\\")+1);
	}
	while( text.find("/") < text.size() ) {
		text = text.substr(text.find("/")+1);
	}
	return text;
}

string trim(string text)
{
  string::iterator t_pos = text.begin();
  string t_ret = "";
  while( t_pos != text.end() )
  {
    if( *t_pos != ' ' )
      t_ret += *t_pos;
    t_pos++;
  }
  return t_ret;
}

string trimString(string text)
{
  int t_pos = static_cast<int>(text.size())-1;
  string t_ret = "";

  while( t_pos >= 0 && text[t_pos] == ' ' )
    t_pos--;

  t_ret = text.substr(0,t_pos+1);
  return t_ret;
}

string legalize_to_lbl(string text)
{
  int t_poz = 0;
  text = lower_case(text);
  string::iterator t_pos = text.begin();
  while( t_pos != text.end() )
  {
    while( LBL_valid_chars[t_poz] != *t_pos && LBL_valid_chars[t_poz] != '~' )
      t_poz++;
    if( LBL_valid_chars[t_poz] != *t_pos )
      *t_pos = '-';
    t_poz=0;
    t_pos++;
  }
  return text;
}

string lower_case(string text)
{
  string::iterator t_pos = text.begin();
  while( t_pos != text.end() )
  {
    if( *t_pos >='A' && *t_pos <= 'Z' )
      *t_pos = 'a' + (*t_pos-'A');
    t_pos++;
  }
  return text;
}

string from6to8(string text) {
	text = upper_case(text);
	bool found = true;
	while( found ) {
		found = false;
		if( text.find("~[0X2B]") < text.length()) {
			found = true;
			text = text.replace(text.find("~[0X2B]"),7,"~[0X02]");
		}
		if( text.find("~[0X2C]") < text.length() ){
			found = true;
			text = text.replace(text.find("~[0X2C]"),7,"~[0X03]");
		}
		if( text.find("~[0X2D]")< text.length()){
			found = true;
			text = text.replace(text.find("~[0X2D]"),7,"~[0X04]");
		}
		if( text.find("~[0X2E]") < text.length()){
			found = true;
			text = text.replace(text.find("~[0X2E]"),7,"~[0X05]");
		}
		if( text.find("~[0X2F]") < text.length() ){
			found = true;
			text = text.replace(text.find("~[0X2F]"),7,"~[0X06]");
		}
	}
	return text;
}

string upper_case(string text)
{
	unsigned char z;
	string::iterator t_pos = text.begin();
	while( t_pos != text.end() )
	{
		z = *t_pos;
		if( z >='a' && z <= 'z' )
			*t_pos = 'A' + (*t_pos-'a');
		if( z >=0xe0 && z <= 0xff )
			*t_pos = *t_pos-0x20;
		t_pos++;
	}
	return text;
}

string remove_enter(string text)
{
  string::iterator t_pos = text.begin();
  while( t_pos != text.end() )
  {
    if( *t_pos =='\n' )
      *t_pos = ' ';
    t_pos++;
  }
  return text;
}

string first_up(string text)
{
  char prev=' ';
  string::iterator t_pos = text.begin();
  while( t_pos != text.end() )
  {
    if( prev == ' ' )
      if( *t_pos >='a' && *t_pos <= 'z' )
	*t_pos = 'A' + (*t_pos-'a');
    else
      if( *t_pos >='A' && *t_pos <= 'Z' )
	*t_pos = 'a' + (*t_pos-'A');
    prev = *t_pos;
    t_pos++;
  }
  return text;

}

string return_file_id(string file)
{
  //jezeli w 'file' wystepuje / lub \ to zwrocic caly string od \/ do konca,
  //w przeciwnym wypadku calosc
  while( file.find('\\') < file.length() )
    file = file.substr(file.find('\\')+1,file.find('\\') - (file.find('\\')+1) );

  while( file.find('/') < file.length() )
    file = file.substr(file.find('/')+1,file.find('/') - (file.find('/')+1) );

  while( file.find('.') < file.length() )
    file = file.substr(0, file.find('.') );

  return file;
}
