/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

#include <string>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "error.h"

using namespace std;

bool errorClass::show_message_error = true;
char errorClass::warn_messages[30][200] = 
{
	"NULL ERROR",
	"Warning PW01: Could not open input file.",
	"Error PE01: Could not open input file.",
	"Error PE02: Name of the file for preview must be composed always from 8 digits.",
	"Error PE03: Could not initialise temporary database - check disk for writing permission.",
	"Error PE04: Could not open IMG file.",
	"Error PE05: Could not open index file. Check if you are using compatible cGPSmapper version.",
	"Error PE06: Not consistent IMG ID value in the index file.",
	"Error PE07: Could not add sort file to the final MDR.IMG file - check if sort.img file is available.",
	"Error PE08: Wrong city index - error in IDX file or in IMG file.",
	"Error PE09: IDX file was created with incompatible version of cGPSmapper.",
	"Error PE10: None of the IMG detailed files can have same name as FileName value in the [MAP] section.",
	"Error PE11: Preview map has to be less detailed than any IMG file in the mapset. Change level definition in the [MAP] section to a less detailed.",
};

void errorClass::showError(int line,const char* file_name,int code)
{
	char	buffer[255];
	int		t_code = code;

	string	text;
	if( file_name ) {
		if( line >= 0 ) {
			text = sprintf("%s (%d) : %s",file_name,line,warn_messages[code]);
/*			text = file_name;
			text += "(";
			text += itoa(line,buffer,10);
			text += ") : ";
			text += warn_messages[code];*/
		} else {
			text = sprintf("%s %s",warn_messages[code],file_name);
/*
			text += warn_messages[code];
			text += " ";
			text += file_name;*/
		}
	}

	if( !file_name ) {
		cout<<warn_messages[code]<<endl;
		text = warn_messages[code];
	} else {
		if( line >= 0 )
			cout<<endl<<file_name<<"("<<line<<") : "<<warn_messages[code]<<endl;
		else
			cout<<warn_messages[code]<<" "<<file_name<<endl;
	}

	if( code >= PE01 )
	{
#ifdef _WIN32
		if( show_message_error )
			MessageBox(NULL,text.c_str(),"Input data ERROR",MB_ICONASTERISK | MB_OK);		
#endif
		exit(code);
	}
}
