/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

#include <stdexcept>
#include <string.h>
#include "img_manager.h"

img_manager::img_manager(std::string _main_file,unsigned int _file_offset,internal_file_type _file_type,unsigned char _xor_file,unsigned int _internal_size) {
	main_file = _main_file;
	file_offset = _file_offset;
	file_type = _file_type;
	xor_file = _xor_file;
	internal_size = _internal_size;
}

img_manager::~img_manager() {
	fclose(input_file);
}

size_t	img_manager::read_data(unsigned char* buffer,int size) {
	size_t t_read = fread(buffer,1,size,input_file);
	if( xor_file )
		for( int t_pos=0; t_pos < (int)t_read; t_pos++ )
			buffer[t_pos] ^= xor_file;
	return t_read;
}

void img_manager::read_const_part() {
	read_data(const_header,0x15);
}

void img_manager::write_const_part() {
	fwrite(&const_header,1,0x15,output_file);
	//fwrite(&const_header,0x14);
}

void img_manager::read_header() {
	input_file = fopen(main_file.c_str(),"rb");
	if( input_file == NULL )
		throw std::runtime_error("abort");

	fseek(input_file,file_offset,SEEK_SET);

	read_const_part();

	if( const_header[2] != 'G' && const_header[3] != 'A' )
		throw std::runtime_error("not a valid file");

	memcpy(&header_size,const_header,2);
}

void img_manager::set_output_file(FILE* _output_file) {
	output_file = _output_file;
}

void img_manager::set_header_offset(unsigned int _offset) {
	header_offset = _offset;
}

unsigned int img_manager::get_header_end() {
	return (header_offset + get_header_size());
}

unsigned int img_manager::get_header_size() {
	return (unsigned int)header_size;
}

unsigned int img_manager::get_data_size() {
	return (internal_size - header_size);
}

void img_manager::set_data_start_offset(unsigned int _offset) {
	data_offset = _offset;
	end_data_offset = get_data_size() + data_offset;
}

unsigned int img_manager::get_data_end_offset() {
	return end_data_offset;
}

void img_manager::write_header() {
	fseek(output_file,header_offset,SEEK_SET);	
}

void img_manager::write_data() {
	unsigned char	buffer[1024];
	unsigned int	data_size = get_data_size();
	fseek(output_file,data_offset,SEEK_SET);
	fseek(input_file,file_offset+get_header_size(),SEEK_SET);	

	for( unsigned int t_s = 0; t_s < data_size; ) {
		if( data_size - t_s > 1024 ) {
			fread(buffer,1,1024,input_file);
			fwrite(buffer,1,1024,output_file);
			t_s += 1024;
		} else {
			fread(buffer,1,(data_size - t_s),input_file);
			fwrite(buffer,1,(data_size - t_s),output_file);
			t_s = data_size;
		}
	}
}

//*************
void rgn_manager::set_data_start_offset(unsigned int _offset) {
	img_manager::set_data_start_offset(_offset);

	rgn_header.rgn1_start += (_offset - get_header_size());
	rgn_header.rgn2_start += (_offset - get_header_size());
	rgn_header.rgn3_start += (_offset - get_header_size());
	rgn_header.rgn4_start += (_offset - get_header_size());
	rgn_header.rgn5_start += (_offset - get_header_size());
}

void rgn_manager::read_header() {
	img_manager::read_header();

	memset(&rgn_header,0,sizeof rgn_header);

	read_data((unsigned char*)&rgn_header,header_size - 0x15);
	if( internal_size < header_size)
		internal_size = header_size;
}

void rgn_manager::write_header() {
	img_manager::write_header();
	img_manager::write_const_part();

	fwrite(&rgn_header,1,header_size - 0x15,output_file);
}

//*************

void tre_manager::set_data_start_offset(unsigned int _offset) {
	img_manager::set_data_start_offset(_offset);

	//zmiana 'start-ow'
	tre_header.tre1_start += (_offset - get_header_size());
	tre_header.tre2_start += (_offset - get_header_size());
	tre_header.tre3_start += (_offset - get_header_size());
	tre_header.tre4_start += (_offset - get_header_size());
	tre_header.tre5_start += (_offset - get_header_size());
	tre_header.tre6_start += (_offset - get_header_size());
	tre_header.tre8_start += (_offset - get_header_size());
	tre_header.tre9_start += (_offset - get_header_size());
	tre_header.tre10_start += (_offset - get_header_size());

//	end_data_offset = size + data_offset ;
}

void tre_manager::read_header() {
	img_manager::read_header();

	memset(&tre_header,0,sizeof tre_header);

	read_data((unsigned char*)&tre_header,header_size - 0x21);
	//odczytanie copyright strings - od koñca nag³ówka do tre3 start
	tre_copyright_len = tre_header.tre3_start - header_size;
	if( tre_copyright_len > 1024 )
		tre_copyright_len = 1024;
	read_data((unsigned char*)&tre_copyright_strings,tre_copyright_len);
	if( internal_size < header_size)
		internal_size = header_size;
}

unsigned int tre_manager::get_data_size() {
	return (internal_size - header_size - tre_copyright_len);
}

unsigned int tre_manager::get_header_size() {
	return (unsigned int)header_size + tre_copyright_len;
}

unsigned int tre_manager::get_header_end() {
	return (header_offset + get_header_size());
}

void tre_manager::write_const_part() {
	fwrite(&tre_const_header,1,0x21,output_file);
	//fwrite(&const_header,0x14);
}

void tre_manager::read_const_part() {
	read_data(tre_const_header,0x21);
	memcpy(&const_header,&tre_const_header,0x15);
}

void tre_manager::write_header() {
	img_manager::write_header();
	
	fwrite(&tre_const_header,1,0x21,output_file);
	fwrite(&tre_header,1,header_size - 0x21,output_file);
	fwrite(&tre_copyright_strings,1,tre_copyright_len,output_file);
}

//*************
void lbl_manager::set_data_start_offset(unsigned int _offset) {
	img_manager::set_data_start_offset(_offset);

	lbl_header.lbl1_start += (_offset - get_header_size());
	lbl_header.lbl2_start += (_offset - get_header_size());
	lbl_header.lbl3_start += (_offset - get_header_size());
	lbl_header.lbl4_start += (_offset - get_header_size());
	lbl_header.lbl5_start += (_offset - get_header_size());
	lbl_header.lbl6_start += (_offset - get_header_size());
	lbl_header.lbl7_start += (_offset - get_header_size());
	lbl_header.lbl8_start += (_offset - get_header_size());
	lbl_header.lbl9_start += (_offset - get_header_size());
	lbl_header.lbl10_start += (_offset - get_header_size());
	lbl_header.lbl11_start += (_offset - get_header_size());

	lbl_header.lbl13_start += (_offset - get_header_size());
	lbl_header.lbl14_start += (_offset - get_header_size());
	lbl_header.lbl15_start += (_offset - get_header_size());
	lbl_header.lbl16_start += (_offset - get_header_size());
	lbl_header.lbl17_start += (_offset - get_header_size());
}

void lbl_manager::read_header() {
	img_manager::read_header();

	memset(&lbl_header,0,sizeof lbl_header);

	read_data((unsigned char*)&lbl_header,header_size - 0x15);
	if( internal_size < header_size)
		internal_size = header_size;
}

void lbl_manager::write_header() {
	img_manager::write_header();
	img_manager::write_const_part();

	fwrite(&lbl_header,1,header_size - 0x15,output_file);
}

//*************

void net_manager::set_data_start_offset(unsigned int _offset) {
	img_manager::set_data_start_offset(_offset);

	//zmiana 'start-ow'
	net_header.net1_start += (_offset - get_header_size());
	net_header.net2_start += (_offset - get_header_size());
	net_header.net3_start += (_offset - get_header_size());
}

void net_manager::read_header() {
	img_manager::read_header();
	memset(&net_header,0,sizeof net_header);

	read_data((unsigned char*)&net_header,header_size - 0x15);
	if( internal_size < header_size)
		internal_size = header_size;
}

void net_manager::write_header() {
	img_manager::write_header();
	img_manager::write_const_part();

	fwrite(&net_header,1,header_size - 0x15,output_file);
}

//************************

void nod_manager::set_data_start_offset(unsigned int _offset) {
	img_manager::set_data_start_offset(_offset);

	//zmiana 'start-ow'
	nod_header.nod1_start += (_offset - get_header_size());
	nod_header.nod2_start += (_offset - get_header_size());
	nod_header.nod3_start += (_offset - get_header_size());
	nod_header.nod4_start += (_offset - get_header_size());
}

void nod_manager::read_header() {
	img_manager::read_header();
	memset(&nod_header,0,sizeof nod_header);

	read_data((unsigned char*)&nod_header,header_size - 0x15);
	if( internal_size < header_size)
		internal_size = header_size;
}

void nod_manager::write_header() {
	img_manager::write_header();
	img_manager::write_const_part();

	fwrite(&nod_header,1,header_size - 0x15,output_file);
}
