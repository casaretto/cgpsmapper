/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

#ifndef __RGN_H
#define __RGN_H

#include "img_struct.h"

namespace RGN {

	unsigned long	readBits(const unsigned char img_c[],const unsigned long& start_address,const unsigned short &number_of_bits);
	long			readValue(const long &Special,const unsigned char img_c[],const unsigned long& start_address,const unsigned short &number_of_bits,const char &type);

	bool			Visible(const long &x0,const long &y0,const long &x1,const long &y1);
	void			DrawRGN(const tre &tre_data,const long &bit_shift,bool parent);

	void			DrawPoint(const unsigned char c_img[],const char &shift);
	void			DrawPolyline(const unsigned char c_img[],const char &shift);
}
#endif