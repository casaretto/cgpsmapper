/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

#include <boost/exception.hpp>
#include <stdexcept>
#include "hcode.h"

//unsigned char NumbersDecoder::reading_net_36 = 0;

NumbersDecoder::NumbersDecoder(unsigned char* buf,unsigned short buff_size,unsigned char net_flag, unsigned char net_36,mapRead_address address[_MAX_SEGMENTS],int segments) {
	unsigned int t, i, j, tb;

	p_reloaded = 0;
	buf_size = buff_size;
	this->buf = buf;
	this->net_36 = net_36;
	this->address = address;
	current_segment = 0;

	if( net_flag & 0x20 ) {
		hn.l_order = 1;
		hn.r_order = 2;
	} else {
		hn.l_order = 2;
		hn.r_order = 1;
	}
	hn.start   = 0;//offset;
	hn.bitsize = buff_size << 3;
	hn.cbit    = 0;

	hn.rp1 = 5 + read_bwf();
	hn.rp2 = 2 + read_bwf();
	if(read_N_bits(1)) {
		hn.l_base = hn.r_base = read_N_bits(net_36 + 5);
	} else {
		t = read_N_bits(4);
		hn.l_base = hn.r_base = read_N_bits(t + 5);
	}
	while( 1 ) {
		mainLoop(segments);
		if( current_segment > segments || hn.bitsize < hn.cbit )
			break;
	}
}

void NumbersDecoder::mainLoop(int segments) {
	int t_skip;


	if( p_reloaded == 0 ) {
		hn.p1 = hn.rp1;
		hn.p2 = hn.rp2;
	}

	if(read_N_bits(1)) {
		// calculate new housenumbers
		if((hn.l_order == 0) || (hn.r_order == 0))
			hn_fill_2();
		else
			hn_fill_1();
		p_reloaded = 0;
	} else { // change settings
		if(read_N_bits(1)) {
			if(read_N_bits(1)) { // set skip counter
				if(read_N_bits(1) == 0) {
					hn.skip = 1 + read_N_bits(5);
				} else {
					hn.skip = 1 + read_N_bits(10);
				}
				if( hn.skip > segments )
					throw std::runtime_error("critical error in numbering, bad IMG");
				hn.l_start = hn.r_start = hn.r_end = hn.l_end = 0;
				for( t_skip = 0; t_skip < hn.skip; t_skip++ ) {
					address[current_segment].left_start = 0;
					address[current_segment].left_end = 0;
					address[current_segment].left_type = 0;

					address[current_segment].right_start = 0;
					address[current_segment].right_end = 0;
					address[current_segment].right_type = 0;
					current_segment++;
				}
			} else {              // reload p1 or p2
				p_reloaded = 1;
				if(read_N_bits(1) == 0) {
					hn.p1 = 5 + read_bwf();
				} else {
					hn.p2 = 2 + read_bwf();
				}
			}
		} else { /* read order bits */
			hn.l_order = read_N_bits(2);
			hn.r_order = read_N_bits(2);
		}
	}
}

unsigned int NumbersDecoder::read_N_bits(int n) {
	unsigned int res = 0;
	unsigned int bs = (hn.cbit >> 3) + hn.start;

	if( buf_size > bs )
		res  = buf[bs+0];
	if( buf_size > bs+1 )
		res |= buf[bs+1]<<8;
	if( buf_size > bs+2 )
		res |= buf[bs+2]<<16;
	if( buf_size > bs+3 )
		res |= buf[bs+3]<<24;
	if( hn.cbit & 7 ) {
		if( buf_size > bs+4 )
			res = (res >> (hn.cbit & 7)) | (buf[bs+4] << (32 - (hn.cbit & 7)));
		else
			res = (res >> (hn.cbit & 7)) | (0 << (32 - (hn.cbit & 7)));
	}

	hn.cbit += n;
	return( (res << (32-n)) >> (32-n) );
}

int NumbersDecoder::read_bwf_val(unsigned int flsz) {
	int res = read_N_bits(flsz & 0x3F);
	if(flsz & 0x40) return(-res);

	if(flsz & 0x80) {
		int t = -1;
		t <<= (flsz & 0x3f) - 1;
		if(res & t) res |= t;
	}

	return(res);
}


unsigned int NumbersDecoder::read_bwf(void) {
	unsigned int t, r;

	t = read_N_bits(2);
	r = read_N_bits(4);

	if(t == 1) return(r | 0x40); // negative values
	if(t == 2) return(++r | 0x80); // signed valuse

	return(r);
}

/* ----------------------------------------------------------------- */
void NumbersDecoder::hn_fill_1(void) {
	//SegmentNo segmentNo(hn.l_order,hn.r_order);
	unsigned int eqb, n8, n10, n20;
	unsigned int z1, z2;

	int tp1 = 0, tp2;

	address[current_segment].left_type = hn.l_order;
	address[current_segment].right_type = hn.r_order;

	if( (eqb = read_N_bits(1)) == 1) {/* need to change base */
		if(read_N_bits(1)) {
			hn.r_base = hn.l_base;
		} else {
			hn.l_base = hn.r_base;
		}
	}

	n8  = read_N_bits(1);
	n10 = read_N_bits(1);
	n20 = read_N_bits(1);

	if(n10 == 0)
		tp1 = read_bwf_val(hn.p1);

        if(n20 == 0)
		tp2 = read_bwf_val(hn.p2);
	else
		tp2 = hn.p2v_left;

	hn.l_start  = hn.l_base + tp1;
	hn.l_end    = hn.l_start + tp2;

	hn.l_base   = hn.l_end;
	hn.p2v_left = tp2;

	hn_round(hn.l_order, &hn.l_start, &hn.l_end);


	address[current_segment].left_start = hn.l_start;
	address[current_segment].left_end = hn.l_end;

/* right side */
	z1 = z2 = 0;

	if(eqb == 0) /* bases are not equal */
		z1 = read_N_bits(1);
	else
		z1 = 1;
	if(n8 == 0)
		z2 = read_N_bits(1);
	else
		z2 = 1;
	tp1 = 0;

	if((eqb == 0) && (z1 == 0))
		tp1 = read_bwf_val(hn.p1);

	if(z2 == 0)
		tp2 = read_bwf_val(hn.p2);
	else if((n8 == 0) || (n20 == 1))
		tp2 = hn.p2v_right;

	hn.r_start  = hn.r_base + tp1;
	hn.r_end    = hn.r_start + tp2;

	hn.r_base   = hn.r_end;
	hn.p2v_right= tp2;


	hn_round(hn.r_order, &hn.r_start, &hn.r_end);

	address[current_segment].right_start = hn.r_start;
	address[current_segment].right_end = hn.r_end;
	current_segment++;
}

void NumbersDecoder::hn_fill_2(void) {
	int tp1 = 0, tp2 = 0;
	unsigned int f1, f2;

	address[current_segment].left_type = hn.l_order;
	address[current_segment].right_type = hn.r_order;

	if(hn.l_order == 0) {
		hn.l_base   = hn.r_base;
		hn.p2v_left = hn.p2v_right;
	} else {
		hn.r_base   = hn.l_base;
		hn.p2v_right= hn.p2v_left;
	}

	f1 = read_N_bits(1);
	f2 = read_N_bits(1);

	if(f1 == 0)
		tp1 = read_bwf_val(hn.p1);
	if(f2 == 0)
		tp2 = read_bwf_val(hn.p2);
	else
		tp2 = hn.p2v_left;

	hn.l_start = hn.l_base + tp1;
	hn.l_end   = hn.l_start + tp2;
	hn.r_start = hn.r_base + tp1;
	hn.r_end   = hn.r_start + tp2;

	hn.p2v_left = hn.p2v_right = tp2;
	hn.l_base = hn.l_end;
	hn.r_base = hn.r_end;

	hn_round(hn.l_order, &hn.l_start, &hn.l_end);
	hn_round(hn.r_order, &hn.r_start, &hn.r_end);

	if(hn.l_order == 0) {
		address[current_segment].right_start = hn.r_start;
		address[current_segment].right_end = hn.r_end;
	} else {
		address[current_segment].left_start = hn.l_start;
		address[current_segment].left_end = hn.l_end;
	}
	current_segment++;
}

void NumbersDecoder::hn_round(int fl, int *s, int *e){
	int *t;

	if(*s > *e) {
		(*e)++;
		t = s;
		s = e;
		e = t;
	} else if (*s < *e)
		(*e)--;

	if(fl == 1) {
		if(*s & 1) (*s)++;
		if(*e & 1) (*e)--;
	} else if(fl == 2) {
		if((*s & 1) == 0) (*s)++;
		if((*e & 1) == 0) (*e)--;
	} else if(fl == 0) {
		*s = 0;
		*e = 0;
	}

// last check
	if(*s > *e) {
		*s = 0;
		*e = 0;
	}
}
