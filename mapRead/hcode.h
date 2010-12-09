/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

#ifndef __HCODEHEADER
#define __HCODEHEADER

#include "mapReadDLL.h"

/*
 * *_flag - 0 - brak numeru
 *          1 - parzyste
 *          2 - nieparzyste
 */

class NumbersDecoder {
public:
	NumbersDecoder(unsigned char* buf,unsigned short buff_size,unsigned char net_flag, unsigned char net_36,mapRead_address address[_MAX_SEGMENTS],int segments);

	//unsigned char reading_net_36;
private:
	void mainLoop(int segments);

	unsigned int read_N_bits(int n);
	int read_bwf_val(unsigned int flsz);
	unsigned int read_bwf(void);
	void hn_fill_1(void);
	void hn_fill_2(void);
	void hn_round(int fl, int *s, int *e);

	unsigned char* buf;
	unsigned int  buf_size;
	unsigned char net_36;
	mapRead_address* address;
	unsigned int  current_segment;

	unsigned int p_reloaded;
	struct {
		unsigned int start;   // offset of hn data from start of NET1
		unsigned int bitsize; // size of hn data in bytes * 8
		unsigned int cbit;    // number of bits readed
		//
		unsigned int p1;      // bitwidth of the first part  (and flags)
		unsigned int p2;      // bitwidth of the second part (and flags)
		unsigned int rp1;      // bitwidth of the first part  (and flags)
		unsigned int rp2;      // bitwidth of the second part (and flags)

		//unsigned
		int p2v_left;
		int p2v_right;

		unsigned int l_base;
		unsigned int r_base;

		unsigned int skip;
		int l_order;
		int r_order;

		int l_start;
		int l_end;

		int r_start;
		int r_end;
	} hn;

};

#endif
