/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

#include "typ_header.h"
#include "imfile.h"
#include "importgps.h"

using namespace std;

int	TYPPoint::DATA_START = 0;
int	TYPPoint::TYP_START = 0;

int	TYPLine::DATA_START = 0;
int	TYPLine::TYP_START = 0;

int	TYPPolygon::DATA_START = 0;
int	TYPPolygon::TYP_START = 0;

int TYPBase::calculateTypAddress(int& address,int typ_address_size) {
	this->typ_address = address;
	address += typ_address_size;
	return address;
}

/*************************************/

bool TYPPolygonTypes::operator()(const TYPPolygonTypes& e1,const TYPPolygonTypes& e2) {
	int e2t = e2.rgnType;
	int e1t = e1.rgnType;

	if( e2t & 0x010000 ) {
		e2t = (e2.rgnType & 0xff00)>>8;
	}

	if( e1t & 0x010000 ) {
		e1t = (e1.rgnType & 0xff00)>>8;
	}

	if( e2.order == e1.order ) {
			return e2t > e1t;
	} else
		return e2.order > e1.order;
}

bool TYPLine::operator()(const TYPLine& e1,const TYPLine& e2) {
	if( e2.rgnType == e1.rgnType )
		return e2.rgnSubType > e1.rgnSubType;
	return e2.rgnType > e1.rgnType;
}

void TYPLine::write(xor_fstream* file) {

	//TODO - antyalias

	file->seekg(data_address+DATA_START,SEEK_SET);
	file->Write(&flag,2);
	file->Write(&day_foreground,3);
	if( day_color == 2 ) file->Write(&day_background,3);

	if( night_color > 0 )file->Write(&night_foreground,3);
	if( night_color == 2 ) file->Write(&night_background,3);

	if( antyalias )
		border_width--;

	if( pattern == false ) {
		file->Write(&line_width,1);
		file->Write(&border_width,1);
	} else {
		for( int i = 0; i < array_lines; ++i ) {
			file->Write(&array[i],4);
		}
	}

	if( flag & 0x0100 ) {
		//language
		short t_size = (lang1.size() ? lang1.size()+2:0) + (lang2.size() ? lang2.size()+2:0) + (lang3.size() ? lang3.size()+2:0) + (lang4.size() ? lang4.size()+2:0);
		if( t_size < 0x7f ) {
			t_size<<=1;
			t_size |=1;
			file->Write(&t_size,1);
		} else {
			short temp = (t_size<<2) | 2;
			short t_value = temp & 0xff;
			file->Write(&t_value,1);
			t_value = (temp & 0xff00)>>8;
			file->Write(&t_value,1);
		}
		if( lang1.size() ) {
			file->Write(&lang1_code,1);
			file->WriteString(lang1);
		}
		if( lang2.size() ) {
			file->Write(&lang2_code,1);
			file->WriteString(lang2);
		}
		if( lang3.size() ) {
			file->Write(&lang3_code,1);
			file->WriteString(lang3);
		}
		if( lang4.size() ) {
			file->Write(&lang4_code,1);
			file->WriteString(lang4);
		}
	}

	if( flag & 0x0400 ) {
		unsigned char t_dat = 0;
		t_dat = (extended_font & 0x07);
		if( day_font > -1 ) t_dat |= 0x08;
		if( night_font > -1 ) t_dat |= 0x10;
		file->Write(&t_dat,1);

		if( day_font > 0 ) file->Write(&day_font,3);
		if( night_font > 0 ) file->Write(&night_font,3);
	}
}

void TYPLine::writeTypAddress(xor_fstream* file,int typ_address_size) {
	unsigned char	tmp;
	file->seekg(typ_address+TYP_START,SEEK_SET);

	tmp = (rgnType << 5) | rgnSubType;
	file->Write(&tmp,1);

	tmp = rgnType >> 3;
	file->Write(&tmp,1);
	file->Write(&data_address,typ_address_size-2);
}

int TYPLine::calculateSize(int &data_address) {
	size = 2; //flaga
	flag = 0;
	day_color = 2;
	night_color = 0;

	size += 6; //2 kolory
	if( pattern == false ) {
		if( day_foreground < 0 ) day_foreground = 0xffffff;
		if( day_background < 0 ) day_background = 0;

		if( (night_foreground > 0 && night_foreground != day_foreground) || (night_background > 0 && night_background != day_background) ) {
			night_color = 2;
			size += 6;
			if( night_foreground < 0 ) night_foreground = 0;
			if( night_background < 0 ) night_background = 0xffffff;
		}

		if( night_color == 0 )
			flag = 0x00;
		else if( night_color == 2 )
			flag = 0x01;
	} else {
		flag = 0x01; //dla dnia i nocy osobno

		day_color = 1;
		night_color = 1;

		if( day_foreground < 0 ) day_foreground = 0xffffff;		
		if( night_foreground < 0 ) day_foreground = day_foreground;

		if( day_background < 0 )
			flag |= 0x04;
		else {
			size +=3;
			day_color = 2;
		}

		if( night_background < 0 )
			flag |= 0x02;
		else {
			size +=3;
			night_color = 2;
		}
	}

	if( extended_font || day_font > -1 || night_font > -1 ) {
		flag |= 0x0400;
		size ++;

		if( day_font > -1 ) size+=3;
		if( night_font > -1 ) size+=3;
	}

	if( wide )
		flag |= 0x0200;
	//if( antyalias )
	//	flag |= 0x0100;

	if( pattern == false ) {
		size +=2;
	} else {
		if( array_lines > 31 ) array_lines = 31;
		flag |= array_lines << 3;
		size += 4*array_lines;
	}

	if( lang1_code || lang2_code || lang3_code || lang4_code || lang1.size() || lang2.size() || lang3.size() || lang4.size() ) {
		flag |= 0x0100;
		if( lang1.size() ) size += (int)lang1.size() + 2;
		if( lang2.size() ) size += (int)lang2.size() + 2;
		if( lang3.size() ) size += (int)lang3.size() + 2;
		if( lang4.size() ) size += (int)lang4.size() + 2;

		if( (lang1.size() ? lang1.size()+2:0) + (lang2.size() ? lang2.size()+2:0) + (lang3.size() ? lang3.size()+2:0) + (lang4.size() ? lang4.size()+2:0) >= 0x7f )
			size += 2;
		else
			size++;
	}

	this->data_address = data_address;

	data_address+=size;
	return size;
}

/************************************************************************/

bool TYPPoint::operator()(const TYPPoint& e1,const TYPPoint& e2) {
	if( e2.rgnType == e1.rgnType )
		return e2.rgnSubType > e1.rgnSubType;
	return e2.rgnType > e1.rgnType;
}

void TYPPoint::write(xor_fstream* file) {
	unsigned char	t_tmp;
	unsigned char	t_size;
	unsigned char	t_bit;

	file->seekg(data_address+DATA_START,SEEK_SET);
	file->Write(&flag,1);
	file->Write(&columns,1);
	file->Write(&lines,1);
	file->Write(&colors,1);

	t_tmp=0x10;
	file->Write(&t_tmp,1);

	for( int i = 0; i < colors; ++i )
		file->Write(&color[i],3);

	//size += (columns*lines*( colors<3 ? 2 : (colors < 16 ? 4 : 8)))/8;
	if( colors < 3 ) t_size = 2;
	else if( colors < 15 ) t_size = 4;
	else t_size = 8;

	t_bit = 0;
	t_tmp = 0;
	for( int i=0; i<lines; ++i ) {
		t_bit = 0;
		t_tmp = 0;
		for( int j=0; j<columns; ++j ) {
			//array[i][j] -=1;
			//cout<<(int)array[i][j]<<" ";
			if( array[i][j] >= colors || array[i][j] < 0 )
				array[i][j] = colors;
			
			//cout<<(int)array[i][j]<<" ";

			t_tmp |= ((array[i][j])<<t_bit);
			t_bit+=t_size;
			if( j+1 == columns )
				t_bit = 8;
			if( t_bit == 8 ) {
				file->Write(&t_tmp,1);
				t_bit = 0;
				t_tmp = 0;
			}
		}
	}
	if( flag & 0x02 ) {
		//night bitmap!
		file->Write(&night_colors,1);

		t_tmp=0x10;
		file->Write(&t_tmp,1);

		for( int i = 0; i < night_colors; ++i )
			file->Write(&night_color[i],3);

		if( night_colors < 3 ) t_size = 2;
		else if( night_colors < 15 ) t_size = 4;
		else t_size = 8;

		t_bit = 0;
		t_tmp = 0;
		for( int i=0; i<lines; ++i ) {
			t_bit = 0;
			t_tmp = 0;
			for( int j=0; j<columns; ++j ) {
				//array[i][j] -=1;
				//cout<<(int)array[i][j]<<" ";
				if( night_array[i][j] >= night_colors || night_array[i][j] < 0 )
					night_array[i][j] = night_colors;

				//cout<<(int)array[i][j]<<" ";

				t_tmp |= ((night_array[i][j])<<t_bit);
				t_bit+=t_size;
				if( j+1 == columns )
					t_bit = 8;
				if( t_bit == 8 ) {
					file->Write(&t_tmp,1);
					t_bit = 0;
					t_tmp = 0;
				}
			}
		}

	}
	if( flag & 0x04 ) {
		//language
		short t_size = (lang1.size() ? lang1.size()+2:0) + (lang2.size() ? lang2.size()+2:0) + (lang3.size() ? lang3.size()+2:0) + (lang4.size() ? lang4.size()+2:0);
		if( t_size < 0x7f ) {
			t_size<<=1;
			t_size |=1;
			file->Write(&t_size,1);
		} else {
			short temp = (t_size<<2) | 2;
			short t_value = temp & 0xff;
			file->Write(&t_value,1);
			t_value = (temp & 0xff00)>>8;
			file->Write(&t_value,1);
		}
		if( lang1.size() ) {
			file->Write(&lang1_code,1);
			file->WriteString(lang1);
		}
		if( lang2.size() ) {
			file->Write(&lang2_code,1);
			file->WriteString(lang2);
		}
		if( lang3.size() ) {
			file->Write(&lang3_code,1);
			file->WriteString(lang3);
		}
		if( lang4.size() ) {
			file->Write(&lang4_code,1);
			file->WriteString(lang4);
		}
	}

	if( flag & 0x08 ) {
		unsigned char t_dat = 0;
		t_dat = (extended_font & 0x07);
		if( day_font > -1 ) t_dat |= 0x08;
		if( night_font > -1 ) t_dat |= 0x10;
		file->Write(&t_dat,1);

		if( day_font > 0 ) file->Write(&day_font,3);
		if( night_font > 0 ) file->Write(&night_font,3);
	}
}

void TYPPoint::writeTypAddress(xor_fstream* file,int typ_address_size) {
	unsigned char	tmp;
	file->seekg(typ_address+TYP_START,SEEK_SET);

	tmp = (rgnType << 5) | rgnSubType;
	file->Write(&tmp,1);

	tmp = rgnType >> 3;
	file->Write(&tmp,1);
	file->Write(&data_address,typ_address_size-2);
}

int TYPPoint::calculateSize(int &data_address) {
	size = 5;
	flag = 0x01;

	size += colors*3;

	if( colors<3 ) {
		short t_one_line = ((columns * 2) / 8) + (((columns * 2) % 8)?1:0);
		size += t_one_line*lines;
	} else if( colors < 15 ) {
		short t_one_line = ((columns * 4) / 8) + (((columns * 4) % 8)?1:0);
		size += t_one_line*lines;
	} else {
		size += columns*lines;
	}

	if( night_colors > 0 ) {
		flag |= 0x02;
		size += 2;
		size += night_colors*3;

		if( night_colors<3 ) {
			short t_one_line = ((columns * 2) / 8) + (((columns * 2) % 8)?1:0);
			size += t_one_line*lines;
		} else if( night_colors < 15 ) {
			short t_one_line = ((columns * 4) / 8) + (((columns * 4) % 8)?1:0);
			size += t_one_line*lines;
		} else {
			size += columns*lines;
		}
	}

	if( lang1_code || lang2_code || lang3_code || lang4_code || lang1.size() || lang2.size() || lang3.size() || lang4.size() ) {
		flag |= 0x04;
		if( lang1.size() ) size += (int)lang1.size() + 2;
		if( lang2.size() ) size += (int)lang2.size() + 2;
		if( lang3.size() ) size += (int)lang3.size() + 2;
		if( lang4.size() ) size += (int)lang4.size() + 2;

		if( (lang1.size() ? lang1.size()+2:0) + (lang2.size() ? lang2.size()+2:0) + (lang3.size() ? lang3.size()+2:0) + (lang4.size() ? lang4.size()+2:0) >= 0x7f )
			size += 2;
		else
			size++;
	}

	if( extended_font || day_font > -1 || night_font > -1 ) {
		flag |= 0x08;
		size ++;

		if( day_font > -1 ) size+=3;
		if( night_font > -1 ) size+=3;
	}


	this->data_address = data_address;

	data_address+=size;
	return size;
}

/************************************************************************/

void TYPPolygon::write(xor_fstream* file) {
	int	t_value,j;
	file->seekg(data_address+DATA_START,SEEK_SET);
	file->Write(&flag,1);
	file->Write(&day_foreground,3);
	if( day_color == 2 ) file->Write(&day_background,3);

	if( night_color > 0 )file->Write(&night_foreground,3);
	if( night_color == 2 ) file->Write(&night_background,3);

	for( int i = 0; i < 32; ++i ) {
		t_value = 0;
		for( j = 0; j<32; j++ ) {			
			t_value |= ((array[i] & (1<<(31-j))) >> (31-j)) << j;
		}
		file->Write(&t_value,4);
	}

	if( flag & 0x10 ) {
		//language
		short t_size = (lang1.size() ? lang1.size()+2:0) + (lang2.size() ? lang2.size()+2:0) + (lang3.size() ? lang3.size()+2:0) + (lang4.size() ? lang4.size()+2:0);
		if( t_size < 0x7f ) {
			t_size<<=1;
			t_size |=1;
			file->Write(&t_size,1);
		} else {
			short temp = (t_size<<2) | 2;
			short t_value = temp & 0xff;
			file->Write(&t_value,1);
			t_value = (temp & 0xff00)>>8;
			file->Write(&t_value,1);
		}
		if( lang1.size() ) {
			file->Write(&lang1_code,1);
			file->WriteString(lang1);
		}
		if( lang2.size() ) {
			file->Write(&lang2_code,1);
			file->WriteString(lang2);
		}
		if( lang3.size() ) {
			file->Write(&lang3_code,1);
			file->WriteString(lang3);
		}
		if( lang4.size() ) {
			file->Write(&lang4_code,1);
			file->WriteString(lang4);
		}
	}

	if( flag & 0x20 ) {
		unsigned char t_dat = 0;
		t_dat = (extended_font & 0x07);
		if( day_font > -1 ) t_dat |= 0x08;
		if( night_font > -1 ) t_dat |= 0x10;
		file->Write(&t_dat,1);

		if( day_font > 0 ) file->Write(&day_font,3);
		if( night_font > 0 ) file->Write(&night_font,3);
	}
}

void TYPPolygon::writeTypAddress(xor_fstream* file,int typ_address_size) {
	unsigned char	tmp;
	file->seekg(typ_address+TYP_START,SEEK_SET);

	tmp = (rgnType << 5) | rgnSubType;
	file->Write(&tmp,1);

	tmp = rgnType >> 3;
	file->Write(&tmp,1);
	file->Write(&data_address,typ_address_size-2);
}

int TYPPolygon::calculateSize(int &data_address) {
	size = 1; //flaga
	flag = 0x08;
	day_color = 2;
	night_color = 0;

	size += 6; //2 kolory

	if( day_foreground < 0 ) day_foreground = 0xffffff;
	if( day_background < 0 || night_foreground >= 0 ) {
		day_color = 1;
		night_color = 1;
		flag |= 0x01; //osobno dla dzien i noc
		if( day_background < 0 ) 
			flag |= 0x04;
		else {
			day_color = 2;
			size +=3;
		}
		if( night_background < 0 ) 
			flag |= 0x02;
		else {
			night_color = 2;
			size +=3;
		}
	}

	size += 4*32;

	if( lang1_code || lang2_code || lang3_code || lang4_code || lang1.size() || lang2.size() || lang3.size() || lang4.size() ) {
		flag |= 0x10;
		if( lang1.size() ) size += (int)lang1.size() + 2;
		if( lang2.size() ) size += (int)lang2.size() + 2;
		if( lang3.size() ) size += (int)lang3.size() + 2;
		if( lang4.size() ) size += (int)lang4.size() + 2;

		if( (lang1.size() ? lang1.size()+2:0) + (lang2.size() ? lang2.size()+2:0) + (lang3.size() ? lang3.size()+2:0) + (lang4.size() ? lang4.size()+2:0) >= 0x7f )
			size += 2;
		else
			size++;
	}

	if( extended_font || day_font > -1 || night_font > -1 ) {
		flag |= 0x20;
		size ++;

		if( day_font > -1 ) size+=3;
		if( night_font > -1 ) size+=3;
	}

	this->data_address = data_address;

	data_address+=size;
	return size;
}

bool TYPPolygon::operator()(const TYPPolygon& e1,const TYPPolygon& e2) {
	if( e2.rgnType == e1.rgnType )
		return e2.rgnSubType > e1.rgnSubType;
	return e2.rgnType > e1.rgnType;
}

//***********************************************************************//

TYP_Header::TYP_Header() {
	typTYP3start = 0;
	typTYP3rec_len = 0;
	typTYP3length = 0;

	//lines table
	typTYP4start = 0;
	typTYP4rec_len = 0;
	typTYP4length = 0;

	//areas table
	typTYP7start = 0;
	typTYP7rec_len = 0;
	typTYP7length = 0;
	
	//const table?
	typTYP5start = 0;
	typTYP5rec_len = 5;
	typTYP5length = 0;

	typTYP1start = 0; //4 bajty
	typTYP1length = 0;

	//lines
	typTYP2start = 0;
	typTYP2length = 0;

	//areas
	typTYP6start = 0;
	typTYP6length = 0;

	product_id = 0;
	codepage = 1252;
}

void TYP_Header::readPoint(xor_fstream* file) {
	string		key,value;
	int			t_read;

	TYPPoint	typ_point;

	for( int i=0; i<32; ++i ) {
		for( int j=0; j<32; ++j )
			typ_point.array[i][j] = 0xff;
	}

	t_read = file->ReadInput(key,value);
	while( t_read != 0 && key != "[END]")
	{
		if( key == "FONT" ) typ_point.extended_font = strtol(value.c_str(),NULL,0);
		if( key == "DAYFONTCOLOR" ) typ_point.day_font = readColor(value);
		if( key == "NIGHTFONTCOLOR" ) typ_point.night_font = readColor(value);

		if( key == "TYPE" && t_read == 2) typ_point.rgnType =  strtol(value.c_str(),NULL,0);
		if( key == "SUBTYPE" && t_read == 2) typ_point.rgnSubType =  strtol(value.c_str(),NULL,0);
		if( (key == "CUSTOM" || key == "MARINE") && t_read == 2) {
			if( value == "1" || value == "Y" )
				typ_point.rgnMarine = true;
		}
		if( typ_point.rgnType & 0x10000 ) {
			typ_point.rgnSubType = (typ_point.rgnType & 0xff);
			typ_point.rgnType = (typ_point.rgnType & 0xff00)>>8;
			typ_point.rgnMarine = true;
		}
/*
		if( key.substr(0,5) == "COLOR" && t_read == 2) {
			short t_line = strtol(key.substr(5).c_str(),NULL,10);
			if( t_line > 0 && t_line < 256 ) {
				typ_point.color[t_line-1] = readColor(value);
				typ_point.colors = max(typ_point.colors,t_line);
			}
		}
*/
		if( key == "STRING1" && t_read == 2) {
			typ_point.lang1_code = (char)strtol(value.substr(0,value.find(",")).c_str(),NULL,0);
			typ_point.lang1 = value.substr(value.find(",") + 1, value.length() - value.find(","));
		}
		if( key == "STRING2" && t_read == 2) {
			typ_point.lang2_code = (char)strtol(value.substr(0,value.find(",")).c_str(),NULL,0);
			typ_point.lang2 = value.substr(value.find(",") + 1, value.length() - value.find(","));
		}
		if( key == "STRING3" && t_read == 2) {
			typ_point.lang3_code = (char)strtol(value.substr(0,value.find(",")).c_str(),NULL,0);
			typ_point.lang3 = value.substr(value.find(",") + 1, value.length() - value.find(","));
		}
		if( key == "STRING4" && t_read == 2) {
			typ_point.lang4_code = (char)strtol(value.substr(0,value.find(",")).c_str(),NULL,0);
			typ_point.lang4 = value.substr(value.find(",") + 1, value.length() - value.find(","));
		}

		if( (key == "DAYXPM" || key == "XPM") && t_read == 2 ) {
			int oryginal_colors;
			readXPM(file,typ_point.array,typ_point.color,value,typ_point.colors,oryginal_colors,typ_point.lines,typ_point.columns);
			/*for(int i =0; i<32; ++i) {
				for(int j=0; j<32; ++j) {
					typ_point.array[i][j]++;
				}
			}*/
			if( typ_point.lines > 24 ) typ_point.lines = 24;
			if( typ_point.columns > 24 ) typ_point.columns = 24;
		}

		if( key == "NIGHTXPM" && t_read == 2 ) {
			int oryginal_colors;
			int lines,columns;
			readXPM(file,typ_point.night_array,typ_point.night_color,value,typ_point.night_colors,oryginal_colors,lines,columns);
		}

		/*
		if( key.substr(0,6) == "BITMAP" && t_read == 2) {
			short t_line = strtol(key.substr(6).c_str(),NULL,10);
			short t_column = 1;
			if( t_line>0 && t_line <= 24 ) {
				while( value.size() && t_column <= 24 ) {
					typ_point.array[t_line-1][t_column-1] = strtoul(value.substr(0,value.find(",")).c_str(),NULL,16);
					if( value.find(",") >= value.size() ) 
						value ="";
					else {
						value = value.substr(value.find(",") + 1, value.length() - value.find(","));
						t_column++;
					}
					typ_point.columns = max(typ_point.columns,t_column);
				}
				typ_point.lines = max(typ_point.lines,t_line);
			}
		}
*/
		t_read = file->ReadInput(key,value);
	}

	if( typ_point.rgnType > 255 ) {
		typ_point.rgnSubType = typ_point.rgnType & 0xff;
		typ_point.rgnType >>= 8;
	}

	if( typ_point.rgnMarine )
		typ_point.rgnType |= 0x100;
	points.push_back(typ_point);
}

int	TYP_Header::readColor(string value) {
	int	t_color = 0;
	if( value.find(",") < value.size() ) {
		t_color = strtol(value.substr(0,value.find(",")).c_str(),NULL,0)<<16;
		value = value.substr(value.find(",") + 1, value.length() - value.find(","));

		t_color |= strtol(value.substr(0,value.find(",")).c_str(),NULL,0)<<8;
		value = value.substr(value.find(",") + 1, value.length() - value.find(","));

		t_color |= strtol(value.substr(0,value.find(",")).c_str(),NULL,0);

		return t_color;
	} else
		return strtol(value.c_str(),NULL,0);
}

void TYP_Header::readLine(xor_fstream* file) {
	string		key,value;
	int			t_read;

	TYPLine		typ_line;

	t_read = file->ReadInput(key,value);
	while( t_read != 0 && key != "[END]")
	{

		if( key == "FONT" ) typ_line.extended_font = strtol(value.c_str(),NULL,0);
		if( key == "DAYFONTCOLOR" ) typ_line.day_font = readColor(value);
		if( key == "NIGHTFONTCOLOR" ) typ_line.night_font = readColor(value);

		if( key == "TYPE" && t_read == 2) typ_line.rgnType =  strtol(value.c_str(),NULL,0);
		if( key == "ANTYALIAS" && t_read == 2) {
			if( value == "Y" || value == "y" || value =="1" ) typ_line.antyalias = true;
		}
		if( key == "SUBTYPE" && t_read == 2) typ_line.rgnSubType =  strtol(value.c_str(),NULL,0);
		if( key == "USEORIENTATION" && t_read == 2) {
			if( value == "Y" || value == "y" || value =="1" ) typ_line.wide = false;
		}
		if( (key == "CUSTOM" || key == "MARINE") && t_read == 2) {
			if( value == "1" || value == "Y" )
				typ_line.rgnMarine = true;
		}

		if( typ_line.rgnType & 0x10000 ) {
			typ_line.rgnSubType = typ_line.rgnType & 0xff;
			typ_line.rgnType = (typ_line.rgnType & 0xff00)>>8;
			typ_line.rgnMarine = true;
		}

/*
		if( key == "DAYFCOLOR" && t_read == 2) typ_line.day_foreground = readColor(value);
		if( key == "DAYBCOLOR" && t_read == 2) typ_line.day_background = readColor(value);
		if( key == "NIGHTFCOLOR" && t_read == 2) typ_line.night_foreground  = readColor(value);
		if( key == "NIGHTBCOLOR" && t_read == 2) typ_line.night_background  = readColor(value);
*/
		if( key == "LINEWIDTH" && t_read == 2) typ_line.line_width  = strtol(value.c_str(),NULL,0);
		if( key == "BORDERWIDTH" && t_read == 2) typ_line.border_width  = strtol(value.c_str(),NULL,0);

		if( key == "STRING1" && t_read == 2) {
			typ_line.lang1_code = strtol(value.substr(0,value.find(",")).c_str(),NULL,0);
			typ_line.lang1 = value.substr(value.find(",") + 1, value.length() - value.find(","));
		}
		if( key == "STRING2" && t_read == 2) {
			typ_line.lang2_code = strtol(value.substr(0,value.find(",")).c_str(),NULL,0);
			typ_line.lang2 = value.substr(value.find(",") + 1, value.length() - value.find(","));
		}
		if( key == "STRING3" && t_read == 2) {
			typ_line.lang3_code = strtol(value.substr(0,value.find(",")).c_str(),NULL,0);
			typ_line.lang3 = value.substr(value.find(",") + 1, value.length() - value.find(","));
		}
		if( key == "STRING4" && t_read == 2) {
			typ_line.lang4_code = strtol(value.substr(0,value.find(",")).c_str(),NULL,0);
			typ_line.lang4 = value.substr(value.find(",") + 1, value.length() - value.find(","));
		}
/*		if( key.substr(0,7) == "PATTERN" && t_read == 2) {
			short t_line = strtol(key.substr(7).c_str(),NULL,0);
			if( t_line>0 && t_line <33 ) {				
				typ_line.pattern = true;
				typ_line.array[t_line-1] = strtoul(value.c_str(),NULL,2);
				typ_line.array_lines = t_line;
			}
		}
*/
		if( key == "XPM" && t_read == 2 ) {
			int array[32][32];
			int a_colors[255];
			int lines, columns, colors,oryginal_colors;
			readXPM(file,array,a_colors,value,colors,oryginal_colors,typ_line.array_lines,columns);
			if( typ_line.array_lines )
				typ_line.pattern = true;
			if( oryginal_colors == 4 ) {
				switch( colors ) {
					case 2:	typ_line.day_foreground = a_colors[0];
							typ_line.night_foreground = a_colors[1];
						break;
					default:
						typ_line.day_foreground = a_colors[0];
						typ_line.day_background = a_colors[1];
						typ_line.night_foreground = a_colors[2];
						typ_line.night_background = a_colors[3];
				}
			} else if( oryginal_colors == 2 ) {
				switch( colors ) {
					case 1:	typ_line.day_foreground = a_colors[0];break;
					case 2:	
						typ_line.day_foreground = a_colors[0];
						typ_line.day_background = a_colors[1];
						break;
				}
			} else if( colors >0 ) typ_line.day_foreground = a_colors[0];

			memset(typ_line.array,0,sizeof typ_line.array);

			for( int i=0; i<32; ++i ) {
				for( int j=0; j<32; ++j ) {
					typ_line.array[i] |= ((int)( array[i][j] == 0 ? 1 : 0 ) << (int)j);
				}
			}
		}

		t_read = file->ReadInput(key,value);
	}

	if( typ_line.rgnType > 255 ) {
		typ_line.rgnSubType = typ_line.rgnType & 0xff;
		typ_line.rgnType >>= 8;
	}

	if( typ_line.line_width > 32 ) typ_line.line_width = 32;
	//przeliczam border width na calkowita szerokosc
	typ_line.border_width = typ_line.border_width*2 + typ_line.line_width;
	if( typ_line.border_width > 32 ) typ_line.border_width = 32;


	if( typ_line.rgnMarine )
		typ_line.rgnType |= 0x100;
	lines.push_back(typ_line);
}

int	 TYP_Header::colorXPM(string code) {
	/*code = upper_case(code);
	if( code == "NONE" ) return -1;

	*/
	return -1;
}

bool TYP_Header::readXPM(xor_fstream* file,int array[32][32],int color[255],string value,int &colors,int &oryginal_colors,int &lines,int &columns) {
	int				n_column,n_lines,n_colors,n_chars;
	map<string,int>	color_map;
	string			n_value;
	string			n_code;
	string			n_name;

	int				n_color;

	memset(array,-1,sizeof array);

	if( value.size() < 8 ) return false;

	while( value[0] != '"' ) value = value.substr(1);
	value = value.substr(1);

	n_column = atoi(value.c_str());
	while( value[0] == ' ' ) value = value.substr(1);
	value = value.substr(value.find(" "));

	n_lines = atoi(value.c_str());
	while( value[0] == ' ' ) value = value.substr(1);
	value = value.substr(value.find(" "));

	n_colors = atoi(value.c_str());
	while( value[0] == ' ' ) value = value.substr(1);
	value = value.substr(value.find(" "));

	n_chars = atoi(value.c_str());

	lines = n_lines;
	columns = n_column;
	oryginal_colors = colors = n_colors;

	for( int i =0; i<n_colors; ++i ) {
		n_value = file->ReadLine();
		if( n_value.size() < 3 )
            ImportSHP::ShowWarning(file,E035);

		n_value = n_value.substr(1);
		n_code = n_value.substr(0,n_chars);

		n_value = n_value.substr(n_value.find("c")+1);
		while( n_value[0] == ' ' ) n_value = n_value.substr(1);
		
		n_name = "";
		while( n_value[0] != '"' && n_value.size() ) {
			n_name += n_value[0];
			n_value = n_value.substr(1);
		}
		if( n_name[0] == '#' ) {
			n_name = n_name.substr(1);
			n_name = string("0x") + n_name;
			n_color = strtol(n_name.c_str(),NULL,16);
		} else {
			n_color = -1;
		}
		if( n_color >= 0 ) {
			color_map[n_code] = color_map.size();
			color[color_map[n_code]] = n_color;
		} else
			colors--;
	}

	for(int t_a = 0; t_a < 32; t_a++)
		memset(array[t_a],0,sizeof array[t_a]);

	for( int i = 0; i<n_lines; ++i ) {
		n_value = file->ReadLine();
		if( n_value.size() < 3 ) return false;

		while( n_value[0] != '"' ) n_value = n_value.substr(1);
		n_value = n_value.substr(1);

		if( n_value.size() < n_column )
			ImportSHP::ShowWarning(file,E034);

		for( int j = 0; j<n_column; ++j ) {
			n_code = n_value.substr(0,n_chars);
			n_value = n_value.substr(n_chars);
			//cout<<color_map.find(n_code)->second<<" ";
			if( color_map.find(n_code) != color_map.end())
				array[i][j] = color_map.find(n_code)->second;
			else
				array[i][j] = -1;
		}
		//cout<<endl;
	}
	return true;
}

void TYP_Header::readPolygon(xor_fstream* file) {
	string		key,value;
	int			t_read;

	TYPPolygon	typ_polygon;

	for( int i=0; i<32; ++i ) 
		typ_polygon.array[i] = 0;

	t_read = file->ReadInput(key,value);
	while( t_read != 0 && key != "[END]")
	{
		if( key == "FONT" ) typ_polygon.extended_font = strtol(value.c_str(),NULL,0);
		if( key == "DAYFONTCOLOR" ) typ_polygon.day_font = readColor(value);
		if( key == "NIGHTFONTCOLOR" ) typ_polygon.night_font = readColor(value);

		if( key == "TYPE" && t_read == 2) typ_polygon.rgnType =  strtol(value.c_str(),NULL,0);
		if( (key == "CUSTOM" || key == "MARINE") && t_read == 2) {
			if( value == "1" || value == "Y" )
				typ_polygon.rgnMarine = true;
		}
		if( key == "SUBTYPE" && t_read == 2) typ_polygon.rgnSubType =  strtol(value.c_str(),NULL,0);

		if( typ_polygon.rgnType & 0x10000 ) {
			typ_polygon.rgnSubType = (typ_polygon.rgnType & 0xff);
			typ_polygon.rgnType = (typ_polygon.rgnType & 0xff00)>>8;
			typ_polygon.rgnMarine = true;
		}

		/*
		if( key == "DAYFCOLOR" && t_read == 2) typ_polygon.day_foreground = readColor(value);
		if( key == "DAYBCOLOR" && t_read == 2) typ_polygon.day_background = readColor(value);
		if( key == "NIGHTFCOLOR" && t_read == 2) typ_polygon.night_foreground  = readColor(value);
		if( key == "NIGHTBCOLOR" && t_read == 2) typ_polygon.night_background  = readColor(value);
		*/

		if( key == "XPM" && t_read == 2 ) {
			int array[32][32];
			int a_colors[255];
			int lines, columns, colors,oryginal_colors;
			readXPM(file,array,a_colors,value,colors,oryginal_colors,lines,columns);
			if( oryginal_colors == 4 ) {
				switch( colors ) {
					case 2:	typ_polygon.day_foreground = a_colors[0];
							typ_polygon.night_foreground = a_colors[1];
						break;
					default:
						typ_polygon.day_foreground = a_colors[0];
						typ_polygon.day_background = a_colors[1];
						typ_polygon.night_foreground = a_colors[2];
						typ_polygon.night_background = a_colors[3];
				}
			} else if( oryginal_colors == 2 ) {
				switch( colors ) {
					case 1:	typ_polygon.day_foreground = a_colors[0];break;
					case 2:	
						typ_polygon.day_foreground = a_colors[0];
						typ_polygon.day_background = a_colors[1];
						break;
				}
			} else if( colors >0 ) typ_polygon.day_foreground = a_colors[0];

			for( int i=0; i<32; ++i ) {
				for( int j=0; j<32; ++j ) {
					typ_polygon.array[i] |= ( array[i][j] == 0 ? 1 : 0 ) << (31-j);
				}
			}
		}

		if( key == "STRING1" && t_read == 2) {
			typ_polygon.lang1_code = strtol(value.substr(0,value.find(",")).c_str(),NULL,0);
			typ_polygon.lang1 = value.substr(value.find(",") + 1, value.length() - value.find(","));
		}
		if( key == "STRING2" && t_read == 2) {
			typ_polygon.lang2_code = strtol(value.substr(0,value.find(",")).c_str(),NULL,0);
			typ_polygon.lang2 = value.substr(value.find(",") + 1, value.length() - value.find(","));
		}
		if( key == "STRING3" && t_read == 2) {
			typ_polygon.lang3_code = strtol(value.substr(0,value.find(",")).c_str(),NULL,0);
			typ_polygon.lang3 = value.substr(value.find(",") + 1, value.length() - value.find(","));
		}
		if( key == "STRING4" && t_read == 2) {
			typ_polygon.lang4_code = strtol(value.substr(0,value.find(",")).c_str(),NULL,0);
			typ_polygon.lang4 = value.substr(value.find(",") + 1, value.length() - value.find(","));
		}
		/*
		if( key.substr(0,7) == "PATTERN" && t_read == 2) {
			short t_line = strtol(key.substr(7).c_str(),NULL,10);
			if( t_line>0 && t_line <33 ) {				
				typ_polygon.array[t_line-1] = strtoul(value.c_str(),NULL,2);
			}
		}*/


		t_read = file->ReadInput(key,value);
	}

	if( typ_polygon.rgnType > 255 ) {
		typ_polygon.rgnSubType = typ_polygon.rgnType & 0xff;
		typ_polygon.rgnType >>= 8;
	}
	
	if( typ_polygon.rgnMarine )
		typ_polygon.rgnType |= 0x100;
	polygons.push_back(typ_polygon);
}

void TYP_Header::readId(xor_fstream* file) {
	string		key,value;
	int			t_read;

	product_id = 0;

	t_read = file->ReadInput(key,value);
	while( t_read != 0 && key != "[END]")
	{
		if( key == "FID" && t_read == 2) product_id |= strtol(value.c_str(),NULL,0);
		if( key == "PRODUCTCODE" && t_read == 2) product_id |= (strtol(value.c_str(),NULL,0))<<16;
		if( key == "CODEPAGE" && t_read == 2) codepage = (strtol(value.c_str(),NULL,0));

		t_read = file->ReadInput(key,value);
	}
}

void TYP_Header::readDrawOrder(xor_fstream* file) {
	string		key,value;
	int			t_read;
	int			t_major_type,t_sub_type,t_order;
	bool		t_nt;

	TYPPolygonTypes	polygon_order;

	t_read = file->ReadInput(key,value);
	while( t_read != 0 && key != "[END]")
	{
		//01 02 00 00 00  -> typ marine 0x101 01
		//do sprawdzenia
		if( key == "TYPE" && t_read == 2) {
			t_major_type = strtol(value.substr(0,value.find(",")).c_str(),NULL,0);
			if( t_major_type > 0x10000 ) {
				t_nt = true;
				t_sub_type = (t_major_type & 0xff);
				t_major_type = (t_major_type & 0xff00)>>8;
			} else {
				t_sub_type = 0;
				t_nt = false;
			}

			t_order = strtol(value.substr(value.find(",") + 1, value.length() - value.find(",")).c_str(),NULL,0);
			polygon_order.value = 0;

			if( t_order > 12 ) t_order = 12;
			if( t_order < 1 ) t_order = 1;

			for( vector<TYPPolygonTypes>::iterator i_int = polygons_table.begin(); i_int != polygons_table.end(); i_int++ ) {
				if( (*i_int).rgnType == t_major_type && (*i_int).order == t_order ) {
					if( t_nt )
						(*i_int).rgnSubType |= (1 << t_sub_type);
					t_order = -1;
				}
			}

			if( t_order > 0 ) {
				polygon_order.rgnType = t_major_type;
				if( t_nt )
					polygon_order.rgnSubType = (1 << t_sub_type);
				else
					polygon_order.rgnSubType = 0;
				polygon_order.order = t_order;

				polygons_table.push_back(polygon_order);
			}

		}

		t_read = file->ReadInput(key,value);
	}
}

bool TYP_Header::readTYPdefinition(const char* file_name) {
	string	key,value;
	int		t_read;
	bool	t_result = true;
	bool	t_draworder = false;

	xor_fstream* file = new xor_fstream(file_name,"rb");	
	if( file->error )
		return false;
	file->SetXorMask(0);

	t_read = file->ReadInput(key,value);
	while( t_read == 1 || t_read == 3 || t_read == 2 ||t_read == 5 )
	{
		if( key == string("[_POINT]") )		readPoint(file);
		if( key == string("[_LINE]") )		readLine(file);
		if( key == string("[_POLYGON]") )	readPolygon(file);
		if( key == string("[_DRAWORDER]") )	 {
			readDrawOrder(file);
			t_draworder = true;
		}
		if( key == string("[_ID]") )		readId(file);

		t_read = file->ReadInput(key,value);
	}

	if( t_draworder == false )
        ImportSHP::ShowWarning(file,W021);

	delete file;
	return t_result;
}

void TYP_Header::process(const char* file_name,const char* out_file) {

	char	buffer[10];
	string	out_name;

	cout<<"Processing custom type definition file"<<endl;

	if( readTYPdefinition(file_name) ) {
		out_name = itoa(product_id,buffer,16);
		out_name += ".TYP";
		calculate();
		if( strlen(out_file ) )
			out_name = out_file;
		out_name = upper_case(out_name);
		if( out_name.size() > 12 )
			cout<<"Custom file definition is not working correctly if the filename is longer than 8+3 characters!"<<endl;

		if( createTYP(out_name.c_str()) )
			cout<<"File: "<<out_name<<" is created"<<endl;
		else
			cout<<"Error creating custom type definition file"<<endl;
	}
}

void TYP_Header::calculate() {

	if( polygons_table.size() == 0 ) {
		polygons_table.push_back(TYPPolygonTypes(1,1));
		polygons_table.push_back(TYPPolygonTypes(2,1));
		polygons_table.push_back(TYPPolygonTypes(3,1));
		polygons_table.push_back(TYPPolygonTypes(4,1));
		polygons_table.push_back(TYPPolygonTypes(10,1));
		polygons_table.push_back(TYPPolygonTypes(0x3c,1));
		polygons_table.push_back(TYPPolygonTypes(0x3d,1));
		polygons_table.push_back(TYPPolygonTypes(0x3e,1));
		polygons_table.push_back(TYPPolygonTypes(0x3f,1));
		polygons_table.push_back(TYPPolygonTypes(0x40,1));
		polygons_table.push_back(TYPPolygonTypes(0x41,1));
		polygons_table.push_back(TYPPolygonTypes(0x46,1));
	}

	sort(lines.begin(),lines.end(),TYPLine());
	sort(points.begin(),points.end(),TYPPoint());
	sort(polygons.begin(),polygons.end(),TYPPolygon());
	sort(polygons_table.begin(),polygons_table.end(),TYPPolygonTypes());

	int	t_order = -1;
	for( vector<TYPPolygonTypes>::iterator i_int = polygons_table.begin(); i_int != polygons_table.end(); i_int++ ) {
		if( t_order != -1 ) {
			if( t_order != (*i_int).order ) {
				TYPPolygonTypes typ_polygon;
				typ_polygon.rgnType = 0;
				typ_polygon.value = 0;
				typ_polygon.order = t_order;

				i_int = polygons_table.insert(i_int,typ_polygon);
				i_int++;
			}
		}
		t_order = (*i_int).order;
	}


	typTYP1length = 0;
	typTYP2length = 0;
	typTYP3length = 0;
	typTYP4length = 0;
	typTYP5length = 0; //?

	typTYP3rec_len = 3;
	typTYP4rec_len = 3;
	typTYP7rec_len = 3;

	//Lines
	for( vector<TYPLine>::iterator i_line = lines.begin(); i_line != lines.end(); i_line++ ) {
		(*i_line).calculateSize(typTYP2length);
	}

	if( typTYP2length >=0xff )		typTYP4rec_len = 4;
	if( typTYP2length >=0xffff )	typTYP4rec_len = 5;
	if( typTYP2length >=0xffffff )	typTYP4rec_len = 6;
	for( vector<TYPLine>::iterator i_line = lines.begin(); i_line != lines.end(); i_line++ ) {
		(*i_line).calculateTypAddress(typTYP4length,typTYP4rec_len);
	}
	//Points
	for( vector<TYPPoint>::iterator i_point = points.begin(); i_point != points.end(); i_point++ ) {
		(*i_point).calculateSize(typTYP1length);
	}

	if( typTYP1length >=0xff )		typTYP3rec_len = 4;
	if( typTYP1length >=0xffff )	typTYP3rec_len = 5;
	if( typTYP1length >=0xffffff )	typTYP3rec_len = 6;
	for( vector<TYPPoint>::iterator i_point = points.begin(); i_point != points.end(); i_point++ ) {
		(*i_point).calculateTypAddress(typTYP3length,typTYP3rec_len);
	}

	//Polygons
	for( vector<TYPPolygon>::iterator i_polygon = polygons.begin(); i_polygon != polygons.end(); i_polygon++ ) {
		(*i_polygon).calculateSize(typTYP6length);
	}

	if( typTYP6length >=0xff )		typTYP7rec_len = 4;
	if( typTYP6length >=0xffff )	typTYP7rec_len = 5;
	if( typTYP6length >=0xffffff )	typTYP7rec_len = 6;
	for( vector<TYPPolygon>::iterator i_polygon = polygons.begin(); i_polygon != polygons.end(); i_polygon++ ) {
		(*i_polygon).calculateTypAddress(typTYP7length,typTYP7rec_len);
	}

	/*
	for( int i = 1; i < 0x53; i++ ) {
		TYPPolygonTypes poly_used;
		poly_used.rgnType = i;
		poly_used.value = 0;
		polygons_table.push_back(poly_used);
	}*/


	typTYP5length = polygons_table.size() * 5;

	//ustawienie adresow startowych
	typTYP6start = 0x6e;// 0x5b;

	typTYP7start = typTYP6start + typTYP6length;

	typTYP2start = typTYP7start + typTYP7length;

	typTYP4start = typTYP2start + typTYP2length;

	typTYP1start = typTYP4start + typTYP4length;

	typTYP3start = typTYP1start + typTYP1length;
	
	typTYP5start = typTYP3start + typTYP3length;

	TYPPoint::DATA_START = typTYP1start;
	TYPPoint::TYP_START = typTYP3start;

	TYPLine::DATA_START = typTYP2start;
	TYPLine::TYP_START = typTYP4start;

	TYPPolygon::DATA_START = typTYP6start;
	TYPPolygon::TYP_START = typTYP7start;

	if( !typTYP1length ) typTYP1start = 0;
	if( !typTYP2length ) typTYP2start = 0;
	if( !typTYP3length ) typTYP3start = 0;
	if( !typTYP4length ) typTYP4start = 0;
	if( !typTYP6length ) typTYP6start = 0;
	if( !typTYP7length ) typTYP7start = 0;
}

bool TYP_Header::createTYP(const char* out_file_name) {
	//1 - write header
	unsigned char	header_buffer[0x6f];
	unsigned int	t_tmp;
	xor_fstream		*file = new xor_fstream(out_file_name,"w+b");

	time_t		timer;
	struct tm	*tblock;

	if( file->error ) {
		delete file;
		//error message
		return false;
	}

	timer = time(NULL);
	tblock = localtime(&timer);

	typYear = tblock->tm_year + 1900;	// today_date.da_year - 2000;

	typMonth = tblock->tm_mon;
	typDay = tblock->tm_mday;
	typTime[0] = tblock->tm_hour;
	typTime[1] = tblock->tm_min;
	typTime[2] = tblock->tm_sec;

	memcpy(typName,"GARMIN TYP",10);
	typTyp0[0] = 1;
	typTyp0[1] = 0;
	typUnknown[0] = 0xe4; //language (primary?)
	typUnknown[1] = 0x04;

	memset(header_buffer,0,sizeof header_buffer);
	header_buffer[0] = 0x6e;//0x5b;
	memcpy(&header_buffer[2],typName,10);
	memcpy(&header_buffer[12],typTyp0,2);
	memcpy(&header_buffer[14],&typYear,2);
	memcpy(&header_buffer[16],&typMonth,1);
	memcpy(&header_buffer[17],&typDay,1);
	memcpy(&header_buffer[18],&typTime,3);
	memcpy(&header_buffer[21],&codepage,2);

	memcpy(&header_buffer[0x17],&typTYP1start,4);
	memcpy(&header_buffer[0x1b],&typTYP1length,4);

	memcpy(&header_buffer[0x1f],&typTYP2start,4);
	memcpy(&header_buffer[0x23],&typTYP2length,4);

	memcpy(&header_buffer[0x27],&typTYP6start,4);
	memcpy(&header_buffer[0x2b],&typTYP6length,4);

	//fid id, sub id
	//0x2f - 0x32
	memcpy(&header_buffer[0x2f],&product_id,4);

	memcpy(&header_buffer[0x33],&typTYP3start,4); //punkty
	memcpy(&header_buffer[0x37],&typTYP3rec_len,4);
	memcpy(&header_buffer[0x39],&typTYP3length,4);

	memcpy(&header_buffer[0x3d],&typTYP4start,4); //linie
	memcpy(&header_buffer[0x41],&typTYP4rec_len,4);
	memcpy(&header_buffer[0x43],&typTYP4length,4);

	memcpy(&header_buffer[0x47],&typTYP7start,4); //polygony
	memcpy(&header_buffer[0x4b],&typTYP7rec_len,4);
	memcpy(&header_buffer[0x4d],&typTYP7length,4);

	memcpy(&header_buffer[0x51],&typTYP5start,4); //polygon order table
	memcpy(&header_buffer[0x55],&typTYP5rec_len,4);
	memcpy(&header_buffer[0x57],&typTYP5length,4);

	header_buffer[0x65] = 0x1f;

	file->Write(header_buffer,0x6e);
	//2 - write data

	//points
	for( vector<TYPPoint>::iterator i_point = points.begin(); i_point != points.end(); i_point++ ) {
		(*i_point).write(file);
		(*i_point).writeTypAddress(file,typTYP3rec_len);
	}

	//lines
	for( vector<TYPLine>::iterator i_line = lines.begin(); i_line != lines.end(); i_line++ ) {
		(*i_line).write(file);
		(*i_line).writeTypAddress(file,typTYP4rec_len);
	}

	//polygons
	for( vector<TYPPolygon>::iterator i_polygon = polygons.begin(); i_polygon != polygons.end(); i_polygon++ ) {
		(*i_polygon).write(file);
		(*i_polygon).writeTypAddress(file,typTYP7rec_len);
	}

	//polygon order table
	file->seekg(typTYP5start,SEEK_SET);
	for( vector<TYPPolygonTypes>::iterator i_int = polygons_table.begin(); i_int != polygons_table.end(); i_int++ ) {
		
		file->Write(&(*i_int).rgnType,1);
		file->Write(&(*i_int).rgnSubType,4);
	}


	//3 - end

	delete file;
	return true;
}
