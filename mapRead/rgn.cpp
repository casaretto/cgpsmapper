/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

#include "rgn.h"
#include "img.h"

namespace RGN {

//global start coordinate
long	start_x,start_y;
rgn_table rgn;

//polyline draw
float x,last_x;
float y,last_y;
long screen_x;
long screen_y;
char type;
unsigned long local_address;
bool long_type;

//polyline decoder
unsigned short byte_position;
unsigned short x_len,y_len;
char	x_type,y_type; //0,1,2
long	x_total,y_total;
long	x_last_total,y_last_total;

unsigned long	bits_res;
unsigned long	bits_tmp;

long lXSpecial;// = 1UL << (cBitsPerX - 1);
long lYSpecial;// = 1UL << (cBitsPerY - 1);

//global clip
//Coord	a0,b0,a1,b1;
bool	last_available;
point_t	a,b,last;

//wspolrzedne do rysowania
point_i	RGNcoords[256];
int		RGNcoords_size;
int		RGNcoords_curr;

/**
 * Glowna funkcja rusujaca
 * @param	tre_data	parametry regionu
 * @param	bit_shift	mnoznik dokladnosci
 * @param	parent		true jezeli posiada childs
 */
void DrawRGN(const tre &tre_data,const long &bit_shift,bool parent) {
	char	rgn_type;
	unsigned long	rgn_address,rgn_address_end;
		
	start_x = ConvertB3ToInt(tre_data.x);
	start_y = ConvertB3ToInt(tre_data.y);
	img_database->ReadRGNTable(tre_data,rgn,parent);

	//ustawiam na ostatni
	rgn_type = rgn.types-1;
	//rysowanie - kolejnosc 80, 40, 20, 10
	if( (tre_data.types & 0x80) /*&& draw_rgn80*/ ) {
		rgn_address = rgn.rgn_address[rgn_type];
		rgn_address_end = rgn.rgn_address[rgn_type+1];
		
		while( rgn_address < rgn_address_end ) {
			rgn_address += img_database->ReadRGN40(rgn_address);
			DrawPolyline(img_database->rgn_buffer,bit_shift);
		}
			
		rgn_type--;
	}

	if( tre_data.types & 0x40 ) {
		rgn_address = rgn.rgn_address[rgn_type];
		rgn_address_end = rgn.rgn_address[rgn_type+1];

		while( rgn_address < rgn_address_end ) {
			rgn_address += img_database->ReadRGN40(rgn_address);
			DrawPolyline(img_database->rgn_buffer,bit_shift);
		}	
		rgn_type--;
	}

	if( tre_data.types & 0x20 ) {
		rgn_address = rgn.rgn_address[rgn_type];
		rgn_address_end = rgn.rgn_address[rgn_type+1];

		while( rgn_address < rgn_address_end ) {
			rgn_address += img_database->ReadRGN10(rgn_address);
			DrawPoint(img_database->rgn_buffer,bit_shift);
		}	
		rgn_type--;
	}

	if( tre_data.types & 0x10 ) {
		rgn_address = rgn.rgn_address[rgn_type];
		rgn_address_end = rgn.rgn_address[rgn_type+1];

		while( rgn_address < rgn_address_end ) {
			rgn_address += img_database->ReadRGN10(rgn_address);
			DrawPoint(img_database->rgn_buffer,bit_shift);
		}	
	}
}

void DrawPoint(const unsigned char img_c[],const char &shift) {
	type = img_c[0];
	long_type = (img_c[3] & 0x80 ? true : false);
		
	//coords
	//local_address+=4;
	screen_x = (start_x + (ConvertB2ToInt16(&img_c[4]) << shift));// *c_fGarminUnit;
	//local_address+=2;
	screen_y = (start_y + (ConvertB2ToInt16(&img_c[6]) << shift));//*c_fGarminUnit;
	//local_address+=2;

	if(screen_x >= g_img->X0 && screen_x <= g_img->X1 && screen_y >= g_img->Y0 && screen_y <= g_img->Y1 ) {
		x = screen_x * c_fGarminUnit;
		y = screen_y * c_fGarminUnit;
		screen_x = Position::XToScreen(x);
		screen_y = Position::YToScreen(y);
		WinDrawPixel(screen_x,screen_y);
	}

	/*
	if( long_type )
		return 9;
	return 8;
	*/
}

void DrawPolyline(const unsigned char img_c[],const char &shift) {
	type = img_c[0];
	long_type = type & 0x80 ? true : false;
	type = type & 0x7f; 
	//label
	
	//coords
	local_address=4;
	screen_x = ConvertB2ToInt16(&img_c[local_address]);
	screen_x <<= static_cast<long>(shift);// *c_fGarminUnit;
	screen_x += start_x;
	local_address+=2;

	screen_y = ConvertB2ToInt16(&img_c[local_address]);
	screen_y <<= static_cast<long>(shift);
	screen_y += start_y;
	local_address+=2;

	if( long_type ) {
		bit_stream_len = ConvertB2ToUInt16(&img_c[local_address]);
		local_address+=2;
	} else {
		bit_stream_len = img_c[local_address];	
		++local_address;
	}
	bit_position = 0;
	x_total = screen_x;
	y_total = screen_y;
	x_last_total = x_total;
	y_last_total = y_total;

	RGNcoords_size = 1;
	RGNcoords[0] = point_i(screen_x,screen_y);
	
	//last_x = screen_x * c_fGarminUnit;
	//last_y = screen_y * c_fGarminUnit;

	x_len = ReadBits(img_c,local_address,4);	
	y_len = ReadBits(img_c,local_address,4);
	
	x_len = x_len<=9 ? x_len+2 : (2 * x_len)-7;	
	y_len = y_len<=9 ? y_len+2 : (2 * y_len)-7;
	
	if( ReadBits(img_c,local_address,1) ) {
		if( ReadBits(img_c,local_address,1) ) 
			x_type = 1;
		else
			x_type = 2;
	} else {
		++x_len;
		x_type = 0;
	}

	if( ReadBits(img_c,local_address,1) ) {
		if( ReadBits(img_c,local_address,1) )
			y_type = 1;
		else
			y_type = 2;
	} else {
		++y_len;
		y_type = 0;
	}

	lXSpecial = 1UL << (x_len - 1L);
	lYSpecial = 1UL << (y_len - 1L);
	last_available = false;
	
	for(;;) {
		screen_x = ReadValue(lXSpecial,img_c,local_address,x_len,x_type);//we
		screen_y = ReadValue(lYSpecial,img_c,local_address,y_len,y_type);//ns
		
		screen_x<<=static_cast<long>(shift);
		screen_y<<=static_cast<long>(shift);
	
		x_total += screen_x;
		y_total += screen_y;
		/*
		if( Visible(x_last_total,y_last_total,x_total,y_total) ) {

			if( last_available == false ) {
				last_x = x_last_total * c_fGarminUnit;
				last_y = y_last_total * c_fGarminUnit;

				a.x = Position::XToScreen(last_x);
				a.y = Position::YToScreen(last_y);
			} else {
				a = last;
			}

			x = x_total * c_fGarminUnit;
			y = y_total * c_fGarminUnit;

			b.x = Position::XToScreen(x);
			b.y = Position::YToScreen(y);
			
			ClipLineToWindow(0,160,0,160);
			WinDrawLine(a0,b0,a1,b1);
			
			last = b;
			last_available = true;
		} else
			last_available = false;
		*/
		RGNcoords[RGNcoords_size++]=point_i(x_total,y_total);
		
		//x_last_total = x_total;	
		//y_last_total = y_total;	
		
		if( (bit_position >> 3) > bit_stream_len ) break;
	}
	
	//Rysowanie
	for( RGNcoords_curr=0; RGNcoords_curr < RGNcoords_size-1; ++RGNcoords_curr ) {
		if( Visible(RGNcoords[RGNcoords_curr].x,RGNcoords[RGNcoords_curr].y,RGNcoords[RGNcoords_curr+1].x,RGNcoords[RGNcoords_curr+1].y) ) {			
			if( last_available == false ) {
				last_x = RGNcoords[RGNcoords_curr].x * c_fGarminUnit;
				last_y = RGNcoords[RGNcoords_curr].y * c_fGarminUnit;

				a.x = Position::XToScreen(last_x);
				a.y = Position::YToScreen(last_y);
			} else {
				a = last;
			}

			x = RGNcoords[RGNcoords_curr+1].x * c_fGarminUnit;
			y = RGNcoords[RGNcoords_curr+1].y * c_fGarminUnit;

			b.x = Position::XToScreen(x);
			b.y = Position::YToScreen(y);
			/*
			
			 tutaj rysowanie linii

			if( ClipLineToWindow(0,100,0,100) ) {
				WinDrawLine(a0,b0,a1,b1);
			
				last = b;
				last_available = true;
			}
			*/
		} else
			last_available = false;
	}
}

bool Visible(const long &x0,const long &y0,const long &x1,const long &y1) {
	if( 
		(x0 < g_img->X0 && x1 < g_img->X0) ||
		(x0 > g_img->X1 && x1 > g_img->X1) ||
		(y0 < g_img->Y0 && y1 < g_img->Y0) ||
		(y0 > g_img->Y1 && y1 > g_img->Y1) ) return false;
	return true;
}

/**
 * Bit stream reading
 */





}