/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

//#include <boost/interprocess/managed_mapped_file.hpp>
#include <boost/exception.hpp>
#include <string>
#include <map>
#include <assert.h>
#include <iostream>
#include "img_internal.h"
#include "hcode.h"
#include "img.h"
#include "Globals.h"
#include "ITOA.H"

using namespace std;
using namespace map_read;

struct file_desc {
	unsigned long	file_start;
	unsigned long	file_start_nt;
	unsigned int	file_len;
	std::string		internal_name;
	bool			nt;
};

IMG_file::IMG_file() {

	for( int i = 0; i < 10; i++ ) {
		this->map_level.level[i] = 0;
		this->map_level.level_start_index[i] = 0;
	}

	this->_gmp_file = 0;
	this->_lbl_file = 0;
	this->_rgn_file = 0;
	this->_tre_file = 0;
	this->_net_file = 0;
	this->_nod_file = 0;

	this->lbl_size = 0;
	this->rgn_size = 0;
	this->tre_size = 0;
	this->net_size = 0;
	this->nod_size = 0;
	this->nod1_size = 0;

	this->tre8_data = 0;
	this->tre8_data_len = 0;
	this->tre8_rec_len = 0;
	this->tre8_flag = 0;

	this->rgn1_data = 0;
	this->rgn1_data_len = 0;
	this->rgn2_data = 0;
	this->rgn2_data_len = 0;

	this->rgn3_data = 0;
	this->rgn3_data_len = 0;

	this->rgn4_data = 0;
	this->rgn4_data_len = 0;
}

unsigned long	IMG_file::tre_file() {
	return _tre_file;
}

unsigned long	IMG_file::rgn_file() {
	return _rgn_file;
}
unsigned long	IMG_file::lbl_file() {
	return _lbl_file;
}
unsigned long	IMG_file::net_file() {
	return _net_file;
}
unsigned long	IMG_file::nod_file() {
	return _nod_file;
}

unsigned long	IMG_file::tre_file_data() {
	if( _gmp_file )
		return _gmp_file;
	return _tre_file;
}

unsigned long	IMG_file::rgn_file_data() {
	if( _gmp_file )
		return _gmp_file;
	return _rgn_file;
}
unsigned long	IMG_file::lbl_file_data() {
	if( _gmp_file )
		return _gmp_file;
	return _lbl_file;
}
unsigned long	IMG_file::net_file_data() {
	if( _gmp_file )
		return _gmp_file;
	return _net_file;
}
unsigned long	IMG_file::nod_file_data() {
	if( _gmp_file )
		return _gmp_file;
	return _nod_file;
}

/*************************************************************/
IMG_internal_list::IMG_internal_list(_HEADERDATA _headerFunction,_READDATA _dataFunction,_READDATA_INTEGER _dataFunctionInteger,_READPOI _poiFunction,_READNET _netFunction) {
	headerFunction = _headerFunction;
	dataFunction = _dataFunction;
	dataFunctionInteger = _dataFunctionInteger;
	poiFunction = _poiFunction;
	netFunction = _netFunction;

	poiFunction_i = NULL;
	netFunction_i = NULL;
	nag_label = true;
}

IMG_internal_list::~IMG_internal_list() {
	headerFunction = NULL;
	dataFunction = NULL;
	dataFunctionInteger = NULL;
	poiFunction = NULL;
	netFunction = NULL;

	poiFunction_i = NULL;
	netFunction_i = NULL;
}

void IMG_internal_list::setInternal(_READPOI_INTERNAL _poiInternal,_READNET_INTERNAL _netInternal) {
	poiFunction_i = _poiInternal;
	netFunction_i = _netInternal;
}

//Open IMG file
long IMG_internal_list::openMap(const char *file_name,int selected_file_no,int& sub_maps) {
	map<string,file_desc>	gmp_files;
	map<string,file_desc>	rgn_files;
	map<string,file_desc>	tre_files;
	map<string,file_desc>	lbl_files;
	map<string,file_desc>	net_files;
	map<string,file_desc>	nod_files;

	string			index_file;
	unsigned long	ret_data_start;
	unsigned int	ret_file_len;

	unsigned long	read_pos = 0x400;
	unsigned long	fat_size = 0;
	internal_file_type new_file_type;

	//Starting open procedure
	sub_maps = 0;
#ifdef _mapowanie
	try {
		img_file = new boost::interprocess::file_mapping(file_name,boost::interprocess::read_only);
	}  catch (exception &e) {
		std::cout<<flush<<e.what()<<endl;
		return -1;
	}
#else
	img_file = CreateFile(file_name,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	lastError = GetLastError();
	if( img_file == INVALID_HANDLE_VALUE ) {
		return lastError;
	}
	
	
	DWORD	t_value;
	img_size = GetFileSize(img_file,&t_value);
#endif
	this->file_name = file_name;

#ifdef _mapowanie
	try {
		img_handle = new boost::interprocess::mapped_region(*img_file,boost::interprocess::read_only);
		img_size = img_handle->get_size();
		img_data = (unsigned char*)img_handle->get_address();
	}  catch (exception &e) {
		cout<<flush<<e.what()<<endl;
		return -1;
	}
#else
	img_handle = CreateFileMapping(img_file,NULL,PAGE_READONLY,0,0,0);
	lastError = GetLastError();
	if( img_handle == NULL ) {
		CloseHandle(img_file);
		return lastError;
	}
	

	img_data = (unsigned char *) MapViewOfFile(img_handle,FILE_MAP_READ,0,0,0);
	lastError = GetLastError();
	if( img_file == NULL ) {
		CloseHandle(img_file);
		return lastError;
	}
#endif

	img_xor = img_data[0];

	//check if this is a normal IMG file - can be eventually DSKDEM
	readData(0x10,16);
	if( strncmp((char*)(read_buffer),"DSKIMG",6) ) {
		delete img_handle;
		delete img_file;
		return _MR_NOT_IMG;
	}

	memset(map_name,0,sizeof map_name);
	readData(0x49,20);
	memcpy(map_name,read_buffer,20);
	readData(0x65,20);
	memcpy((char*)(&map_name[20]),read_buffer,19);

	//basic FAT block size
	readData(0x61,2);
	block_size = (1 << (read_buffer[0])) << read_buffer[1];

	//address reading
	for(;;) {
		new_file_type = add_file_from_FAT(read_pos,fat_size,ret_data_start,index_file,ret_file_len);

		if( new_file_type == imgUNKNOWN )
			break;

		file_desc f_d;
		f_d.file_start = ret_data_start;
		f_d.file_len = ret_file_len;
		f_d.internal_name = index_file;
		f_d.nt = false;

		switch( new_file_type ) {
			case imgGMP:
				gmp_files[index_file] = f_d;
				break;
			case imgTRE:
				tre_files[index_file] = f_d;
				break;
			case imgLBL:
				lbl_files[index_file] = f_d;
				break;
			case imgRGN:
				rgn_files[index_file] = f_d;
				break;
			case imgNET:
				net_files[index_file] = f_d;
				break;
			case imgNOD:
				nod_files[index_file] = f_d;
				break;
		}
	}

	//everything is known now - initialise all the addresses of sub files

	unsigned short	gmp_header_len; 
	unsigned int	section_start;

	map<string,file_desc>::iterator gmp_iter_file = gmp_files.begin();
	for( ; gmp_iter_file != gmp_files.end(); ++gmp_iter_file ) {
		file_desc f_d_tre;
		file_desc f_d_rgn;
		file_desc f_d_lbl;
		file_desc f_d_net;
		file_desc f_d_nod;

	//0x19 - TRE
	//0x1d - RGN
	//0x21 - LBL
	//0x25 - NET
	//0x29 - NOD
	//0x2d - DEM

		if( gmp_files[gmp_iter_file->first].file_start < img_size )
		{
			readData(gmp_files[gmp_iter_file->first].file_start,0x32);
			memcpy(&gmp_header_len,read_buffer,2);

			memcpy(&section_start,&read_buffer[0x19],4);
			f_d_tre.file_start = gmp_files[gmp_iter_file->first].file_start + section_start;
			f_d_tre.file_len = gmp_files[gmp_iter_file->first].file_len;
			f_d_tre.internal_name = gmp_files[gmp_iter_file->first].internal_name;
			f_d_tre.file_start_nt = gmp_files[gmp_iter_file->first].file_start;
			f_d_tre.nt = true;
			tre_files[gmp_iter_file->first] = f_d_tre;

			memcpy(&section_start,&read_buffer[0x1d],4);
			f_d_rgn.file_start = gmp_files[gmp_iter_file->first].file_start + section_start;		
			f_d_rgn.file_len = gmp_files[gmp_iter_file->first].file_len;
			f_d_rgn.internal_name = gmp_files[gmp_iter_file->first].internal_name;
			f_d_rgn.file_start_nt = gmp_files[gmp_iter_file->first].file_start;
			f_d_rgn.nt = true;
			rgn_files[gmp_iter_file->first] = f_d_rgn;

			memcpy(&section_start,&read_buffer[0x21],4);
			f_d_lbl.file_start = gmp_files[gmp_iter_file->first].file_start + section_start;
			f_d_lbl.file_len = gmp_files[gmp_iter_file->first].file_len;
			f_d_lbl.internal_name = gmp_files[gmp_iter_file->first].internal_name;
			f_d_lbl.file_start_nt = gmp_files[gmp_iter_file->first].file_start;
			f_d_lbl.nt = true;
			lbl_files[gmp_iter_file->first] = f_d_lbl;

			memcpy(&section_start,&read_buffer[0x25],4);
			if( section_start && gmp_header_len > 0x25 ) {
				f_d_net.file_start = gmp_files[gmp_iter_file->first].file_start + section_start;
				f_d_net.file_len = gmp_files[gmp_iter_file->first].file_len;
				f_d_net.internal_name = gmp_files[gmp_iter_file->first].internal_name;
				f_d_net.file_start_nt = gmp_files[gmp_iter_file->first].file_start;
				f_d_net.nt = true;
				net_files[gmp_iter_file->first] = f_d_net;
			}

			memcpy(&section_start,&read_buffer[0x29],4);
			if( section_start && gmp_header_len > 0x29 ) {
				f_d_nod.file_start = gmp_files[gmp_iter_file->first].file_start + section_start;
				f_d_nod.file_len = gmp_files[gmp_iter_file->first].file_len;
				f_d_nod.internal_name = gmp_files[gmp_iter_file->first].internal_name;
				f_d_nod.file_start_nt = gmp_files[gmp_iter_file->first].file_start;
				f_d_nod.nt = true;
				nod_files[gmp_iter_file->first] = f_d_nod;
			}
		}		
	}

	map<string,file_desc>::iterator tre_iter_file = tre_files.begin();
	for( ; tre_iter_file != tre_files.end(); ++tre_iter_file ) {
		IMG_file	new_file;

		if( rgn_files.find(tre_iter_file->first) != rgn_files.end() &&
			lbl_files.find(tre_iter_file->first) != lbl_files.end() ) {

				if( tre_files[tre_iter_file->first].nt ) {
					new_file.gmp_file(tre_files[tre_iter_file->first].file_start_nt);

					new_file.gmp_internal_name = tre_files[tre_iter_file->first].internal_name;
					new_file.gmp_size = tre_files[tre_iter_file->first].file_len;
				} else {
					new_file.gmp_internal_name = "";
					new_file.gmp_size = 0;
				}

				new_file.tre_file(tre_files[tre_iter_file->first].file_start);
				new_file.tre_size = tre_files[tre_iter_file->first].file_len;
				new_file.tre_internal_name = tre_files[tre_iter_file->first].internal_name;

				new_file.rgn_file ( rgn_files[tre_iter_file->first].file_start);
				new_file.rgn_size = rgn_files[tre_iter_file->first].file_len;
				new_file.rgn_internal_name = rgn_files[tre_iter_file->first].internal_name;

				new_file.lbl_file ( lbl_files[tre_iter_file->first].file_start);
				new_file.lbl_size = lbl_files[tre_iter_file->first].file_len;
				new_file.lbl_internal_name = lbl_files[tre_iter_file->first].internal_name;

				if( net_files.find(tre_iter_file->first) != net_files.end() ) {
					new_file.net_file ( net_files[tre_iter_file->first].file_start);
					new_file.net_size = net_files[tre_iter_file->first].file_len;
					new_file.net_internal_name = net_files[tre_iter_file->first].internal_name;
				}

				if( nod_files.find(tre_iter_file->first) != nod_files.end() ) {
					new_file.nod_file ( nod_files[tre_iter_file->first].file_start);
					new_file.nod_size = nod_files[tre_iter_file->first].file_len;
					new_file.nod_internal_name = nod_files[tre_iter_file->first].internal_name;
					readData(new_file.nod_file(),0x200);
					memcpy(&new_file.nod1_data,&read_buffer[21],4);
					memcpy(&new_file.nod1_size,&read_buffer[25],4);
					memcpy(&new_file.nod2_data,&read_buffer[37],4);

					new_file.nod1_data += new_file.nod_file_data();
					new_file.nod2_data += new_file.nod_file_data();
				}

				readData(new_file.tre_file(),0x200);
				if( read_buffer[0] >= 0xbc ) {
					memcpy(&new_file.tre8_data,&read_buffer[0x7c],4);
					memcpy(&new_file.tre8_data_len,&read_buffer[0x80],4);
					memcpy(&new_file.tre8_rec_len,&read_buffer[0x84],2);
					memcpy(&new_file.tre8_flag,&read_buffer[0x86],4);

					new_file.tre8_data += new_file.tre_file_data();
				}

				readData(new_file.rgn_file(),0x200);
				memcpy(&new_file.rgn1_data,&read_buffer[21],4);
				memcpy(&new_file.rgn1_data_len,&read_buffer[25],4);
				
				new_file.rgn1_data += new_file.rgn_file_data();

				if( read_buffer[0] >= 0x7d ) {
					memcpy(&new_file.rgn2_data,&read_buffer[29],4);
					memcpy(&new_file.rgn2_data_len,&read_buffer[33],4);
					
					new_file.rgn2_data += new_file.rgn_file_data();

					memcpy(&new_file.rgn3_data,&read_buffer[57],4);
					memcpy(&new_file.rgn3_data_len,&read_buffer[61],4);
					new_file.rgn3_data += new_file.rgn_file_data();

					memcpy(&new_file.rgn4_data,&read_buffer[85],4);
					memcpy(&new_file.rgn4_data_len,&read_buffer[89],4);
					new_file.rgn4_data += new_file.rgn_file_data();
				}

				readData(new_file.lbl_file(),0x200);
				new_file.code_page = 0;
				if( read_buffer[0] > 0xab )
					memcpy(&new_file.code_page,&read_buffer[0xaa],2);				

				memcpy(&new_file.lbl1_data,&read_buffer[21],4);
				new_file.lbl1_data += new_file.lbl_file_data();

				memcpy(&new_file.lbl2_data,&read_buffer[31],4);
				memcpy(&new_file.lbl2_data_len,&read_buffer[35],4);
				memcpy(&new_file.lbl2_rec_len,&read_buffer[39],2);
				memcpy(&new_file.lbl2_flag,&read_buffer[41],2);//?
				new_file.lbl2_data += new_file.lbl_file_data();

				memcpy(&new_file.lbl3_data,&read_buffer[45],4);
				memcpy(&new_file.lbl3_data_len,&read_buffer[49],4);
				memcpy(&new_file.lbl3_rec_len,&read_buffer[53],2);
				memcpy(&new_file.lbl3_flag,&read_buffer[55],2); //?
				new_file.lbl3_data += new_file.lbl_file_data();

				memcpy(&new_file.lbl4_data,&read_buffer[59],4);
				memcpy(&new_file.lbl4_data_len,&read_buffer[63],4);
				new_file.lbl4_data += new_file.lbl_file_data();

				unsigned int	lbl4_len;
				memcpy(&lbl4_len,&read_buffer[63],4);
				memcpy(&new_file.lbl4_rec,&read_buffer[67],2);
				memcpy(&new_file.lbl4_flag,&read_buffer[69],2);//?
				new_file.lbl4_bits = 0;
				if( lbl4_len ) {
					int lbl4_rec_number = lbl4_len / new_file.lbl4_rec - 1;
					while(lbl4_rec_number) {
						new_file.lbl4_bits++;
						lbl4_rec_number>>=1;
					}
					//if( lbl4_len && !new_file.lbl4_bits ) new_file.lbl4_bits = 1;//? kazuistyka
				}

				new_file.lbl4_rec_len = 1;
				if(new_file.lbl4_rec > 0)
					while( static_cast<unsigned int>(0x1 << new_file.lbl4_rec_len * 8) < (lbl4_len / new_file.lbl4_rec))
						new_file.lbl4_rec_len++;

				memcpy(&new_file.lbl6_data,&read_buffer[87],4);
				new_file.lbl6_data += new_file.lbl_file_data();

				memcpy(&new_file.lbl6_flag,&read_buffer[96],1);

				unsigned int	lbl8_len;
				memcpy(&new_file.lbl8_data,&read_buffer[114],4);
				memcpy(&new_file.lbl8_data_len,&read_buffer[118],4);
				new_file.lbl8_data += new_file.lbl_file_data();

				memcpy(&lbl8_len,&read_buffer[118],4);
				memcpy(&new_file.lbl8_rec,&read_buffer[122],2);
				memcpy(&new_file.lbl8_flag,&read_buffer[124],2);//?
				new_file.lbl8_bits = 0;
				if( lbl8_len ) {
					int lbl8_rec_number = lbl8_len / new_file.lbl8_rec - 1;
					while(lbl8_rec_number) {
						new_file.lbl8_bits++;
						lbl8_rec_number>>=1;
					}
					//if( lbl8_len && !new_file.lbl8_bits ) new_file.lbl8_bits = 1;//? kazuistyka
				}
				new_file.lbl8_rec_len = 1;
				if(new_file.lbl8_rec > 0)
					while( static_cast<unsigned int>(0x1 << new_file.lbl8_rec_len * 8) < (lbl8_len / new_file.lbl8_rec))
						new_file.lbl8_rec_len++;

				new_file.lbl_shift = read_buffer[29];
				new_file.lbl_coding = read_buffer[30];

				if( new_file.net_file() ) {
					readData(new_file.net_file(),0x200);
					memcpy(&new_file.net1_data,&read_buffer[21],4);
					memcpy(&new_file.net1_modif,&read_buffer[29],1);					
					new_file.net1_data += new_file.net_file_data();

					memcpy(&new_file.net_base_bits,&read_buffer[54],1);
				}


				internal_files.push_back( new_file );
				selected_file = internal_files.end() - 1;
				if( initMap(selected_file_no,(int)internal_files.size()) != 0 )
					return _MR_CANNOT_INITIALISE;
			}
	}

	sub_maps = (int)internal_files.size();
	//UnmapViewOfFile(img_data);
	//CloseHandle(common_file->img_file);
	return 0;
}

//Initialise reading of the map
long IMG_internal_list::initMap(int selected_file_no,unsigned int selected_sub_map) {	
	//basic data from the IMG
	unsigned int	tre1,tre2,tre3;
	unsigned int	tre1_len,tre2_len,tre3_len;
	int				encrypted = 0;
	unsigned int	decrypt_value = 0;
	B2_t			b2_offset;
	unsigned int	offset;
	int				start_index = 1;
	unsigned int	lbl_address;
	unsigned int	id = 0;
	char			id_buffer[10];
	unsigned char	level_def[10];

	long			dx,dy;

	readData(selected_file->tre_file(),0xbc);
	memcpy(&tre_coord,&read_buffer[21],sizeof tre_coord);

	if( read_buffer[0] >= 0x78 ) {
		memcpy(&id,&read_buffer[0x74],4);
		//sprintf(id_buffer,"%d",id);
		itoa(id,id_buffer,10);
	} else {
		memcpy(id_buffer,&read_buffer[2],8);
		id_buffer[8] = 0;
	}

	//read basic info	
	if( read_buffer[0x0d] == 0x80 ) {
		encrypted = 1;
#if TRIAL == 1
		return -1;
#endif
		memcpy(&decrypt_value,&read_buffer[0xaa],4);
	}

	//read master TRE table & decode it
	memcpy(&tre1,&read_buffer[33],4);	
	memcpy(&tre1_len,&read_buffer[37],4);

	memcpy(&tre2,&read_buffer[41],4);	
	memcpy(&tre2_len,&read_buffer[45],4);

	memcpy(&tre3,&read_buffer[49],4);	
	memcpy(&tre3_len,&read_buffer[53],4);

	//TRE data are no longer avail. in the buffer
	readData(selected_file->tre_file_data()+tre1,tre1_len);

	//decode
	if( encrypted == 1 ) {
		//THIS PART OF CODE HAS BEEN REMOVED
		throw runtime_error("cannot read encrypted file");
		//decryptBuffer(decrypt_value,tre1_len);
	}

	//number of levels
	selected_file->map_level.map_levels = tre1_len/4 -1;
	int tmp_level = selected_file->map_level.map_levels;
	selected_file->map_level.level_address[selected_file->map_level.map_levels] = tre2;
	tre1 = 2;
	while( tmp_level>=0 ) {
		memcpy(b2_offset,&read_buffer[tre1],2);
		offset = ConvertB2ToUInt16(b2_offset);

		selected_file->map_level.level[tmp_level] = read_buffer[tre1-1];
		selected_file->map_level.empty[tmp_level] = (read_buffer[tre1-2] & 0x80) ? true : false;

		level_def[tmp_level] = read_buffer[tre1-1] & 0x7f;

		if( tmp_level > 0 )
			selected_file->map_level.level_address[tmp_level-1] = selected_file->map_level.level_address[tmp_level] + (offset * ( tmp_level ? 16 : 14 ));

		selected_file->map_level.level_start_index[tmp_level] = start_index;
		start_index += offset;

		selected_file->map_level.last_tre_idx = start_index-1;

		tre1+=4;
		tmp_level--;
	}

/*	selected_file->copyright1 = "";
	selected_file->copyright2 = "";
	selected_file->copyright3 = "";
*/
	if( tre3_len > 0 && tre3_len <= 12 ) {
		readData(selected_file->tre_file_data() + tre3,tre3_len);

		memcpy(&lbl_address,&read_buffer[0],3); lbl_address &= 0x3fffff;
		selected_file->copyright1 = readLBL(lbl_address);

		if( tre3_len > 3 ) {
			memcpy(&lbl_address,&read_buffer[3],3); lbl_address &= 0x3fffff;
			selected_file->copyright2 = readLBL(lbl_address);
		}
		if( tre3_len > 6 ) {
			memcpy(&lbl_address,&read_buffer[6],3); lbl_address &= 0x3fffff;
			selected_file->copyright3 = readLBL(lbl_address);
		}

	}


	unsigned long tre_address = selected_file->map_level.level_address[selected_file->map_level.map_levels];
	tre	tre_data;

	readData(tre_address + selected_file->tre_file_data(),sizeof tre_data);
	memcpy(&tre_data,read_buffer,sizeof tre_data);

	dy = ConvertB2ToUInt16(tre_data.y_span) & 0x7fff;
	dy <<= static_cast<long>(24-selected_file->map_level.level[selected_file->map_level.map_levels]);

	dx = ConvertB2ToUInt16(tre_data.x_span) & 0x7fff;
	dx <<= static_cast<long>(24-selected_file->map_level.level[selected_file->map_level.map_levels]);

	selected_file->map_bounds.y1 = (ConvertB3ToInt(tre_data.y) + dy)*c_fGarminUnit;
	selected_file->map_bounds.x1 = (ConvertB3ToInt(tre_data.x) + dx)*c_fGarminUnit;
	selected_file->map_bounds.y0 = (ConvertB3ToInt(tre_data.y) - dy)*c_fGarminUnit;
	selected_file->map_bounds.x0 = (ConvertB3ToInt(tre_data.x) - dx)*c_fGarminUnit;

	map_bounds.y1 = max(map_bounds.y1,selected_file->map_bounds.y1);
	map_bounds.x1 = max(map_bounds.x1,selected_file->map_bounds.x1);
	map_bounds.y0 = min(map_bounds.y0,selected_file->map_bounds.y0);
	map_bounds.x0 = min(map_bounds.x0,selected_file->map_bounds.x0);

	if( headerFunction ) {
		headerFunction(selected_file_no,
			selected_sub_map,
			id_buffer,map_name,
			selected_file->copyright1.c_str(),
			selected_file->copyright2.c_str(),
			selected_file->map_level.map_levels,
			selected_file->map_bounds.x0,selected_file->map_bounds.x1,
			selected_file->map_bounds.y0,selected_file->map_bounds.y1,
			encrypted,level_def);
	}

	return 0;
}

//decode locked map
void IMG_internal_list::decryptBuffer(unsigned int decrypt_value,size_t size)
{
}

internal_file_type IMG_internal_list::add_file_from_FAT(unsigned long &read_pos,unsigned long &fat_size,unsigned long &ret_data_start,string &ret_file_name,unsigned int &file_len) {
	char	file_name[9];
	char	file_type[4];
	char	t_file_type[4];	
	int		t_read_pos;
	unsigned short	t_secEndBlock;
	unsigned short	secStartBlock;
	unsigned short	secEndBlock;
	bool	t_end = false;
	internal_file_type	new_file_type = imgUNKNOWN;

	if( fat_size > 0 )
		if( read_pos >= fat_size )
			return new_file_type;

	readData(read_pos,0x200);

	memcpy(file_name,&read_buffer[1],8);
	//read(file_type,3);
	memcpy(file_type,&read_buffer[9],3);
	//read(&file_len,4);
	memcpy(&file_len,&read_buffer[12],4);

	file_name[8] = 0;
	file_type[3] = 0;

	if( file_type[0] == 0 )
		new_file_type = imgEMPTY;

	//check if known type
	if(		strncmp(file_type, "RGN", 3)
		&&	strncmp(file_type, "TRE", 3)
		&&	strncmp(file_type, "LBL", 3)
		&&	strncmp(file_type, "NET", 3)
		&&	strncmp(file_type, "SRT", 3)
		&&	strncmp(file_type, "MDR", 3)
		&&	strncmp(file_type, "TYP", 3)
		&&	strncmp(file_type, "   ", 3)
		&&	strncmp(file_type, "MPS", 3)
		&&	strncmp(file_type, "DEM", 3)
		&&	strncmp(file_type, "GMP", 3)
		&&	strncmp(file_type, "NOD", 3)) {
			read_pos += 0x200;
			return new_file_type;
		}
		if( !strncmp(file_type, "   ", 3) )
			fat_size = file_len;

		memcpy(&secStartBlock,&read_buffer[32],2);
		memcpy(&t_secEndBlock,&read_buffer[34],2);
		secEndBlock = secStartBlock;

		t_read_pos = 36;
		while(!t_end) {
			while(secEndBlock == (t_secEndBlock - 1) && t_read_pos < 0x200) {
				if(secEndBlock == (t_secEndBlock - 1))
					secEndBlock = t_secEndBlock;

				memcpy(&t_secEndBlock,&read_buffer[t_read_pos],2);
				t_read_pos+=2;

				if(secEndBlock != (t_secEndBlock - 1))
					t_secEndBlock = 0xffff;
			}

			if((t_secEndBlock != 0xffff) && t_read_pos >= 0x200) {
				// more than one block

				readData(read_pos+0x200,0x200);

				memcpy(t_file_type,&read_buffer[9],3);
				//read(t_file_type, 3);
				if( memcmp(file_type,t_file_type,3) == 0 ) {
					read_pos += 0x200;
					t_read_pos = 32;
				} else {
					t_end = true;
					read_pos += 0x200;
				}
			} else {
				t_end = true;
				read_pos += 0x200;
			}
		}

		ret_file_name = file_name;
		ret_data_start = secStartBlock * block_size;

		if( !strncmp(file_type, "TRE",3) ) new_file_type = imgTRE;
		if( !strncmp(file_type, "GMP",3) ) new_file_type = imgGMP;
		if( !strncmp(file_type, "LBL",3) ) new_file_type = imgLBL;
		if( !strncmp(file_type, "NET",3) ) new_file_type = imgNET;
		if( !strncmp(file_type, "RGN",3) ) new_file_type = imgRGN;
		if( !strncmp(file_type, "NOD",3) ) new_file_type = imgNOD;
		if( !strncmp(file_type, "SRT",3) ) new_file_type = imgSRT;
		if( !strncmp(file_type, "MDR",3) ) new_file_type = imgMDR;
		if( !strncmp(file_type, "MPS",3) ) new_file_type = imgMPS;
		if( !strncmp(file_type, "DEM",3) ) new_file_type = imgDEM;
		if( !strncmp(file_type, "   ",3) ) new_file_type = imgFAT;

		return new_file_type;
}

void IMG_internal_list::append6bitSymbol(int & _state, unsigned char _bt, string& _buf,bool& labelMPlike) {

    if (_state == 1) {
		//
		// Symbol after 0x1C.
		//
        if (_bt == 0)
			_buf += '@';
		else if (_bt == 2)
            _buf += '\'';
        else if (_bt >= 1 && _bt <= 0xF)     // !"#$%&'()*+,-./
            _buf += 0x20 + _bt;
        else if (_bt >= 0x1A && _bt <= 0x1F) // :;<=>?
            _buf += 0x20 + _bt;
        else if (_bt >= 0x2B && _bt <= 0x2F) // [\]^_
			_buf += 0x30 + _bt;
        
        _state = 0;
        return;
    } else if (_state == 2) {
		//
		// Symbol after 0x1B.
		//
        if (_bt >= 0 && _bt <= 0x1A) {            // `a-z, lowercase
			//_append_buf (_buf, _pos, "~[0x1b]");
			_buf += 0x60 + _bt;
		} else if (_bt >= 0x20 && _bt <= 0x29) {  // 0-9
			if( labelMPlike )
				_buf += "~[0x1b]";
			else
				_buf += char(0x1b);
			_buf += 0x10 + _bt;
		} else if (_bt == 0x2B) {
			// Separation: on the map visible only the second section (when over 1km), 
            // with the mouse sees displayed one the word completely, not separated.
			if( labelMPlike )
				_buf += "~[0x1b2b]";
			else {
				_buf += char(0x1b);
				_buf += char(0x2b);
			}
		} else if (_bt == 0x2C) {
			// Separation: on the map visible only the first section (when over 1km), 
            // with the mouse sees displayed one the word completely, not separated.
			if( labelMPlike )
				_buf += "~[0x1b2c]";
			else {
				_buf += char(0x1b);
				_buf += char(0x2c);
			}
		}
        _state = 0;
        return;
    }

	if (_bt >= 0x01 && _bt <= 0x1A)  // A-Z
		_buf += 0x40 + _bt;
    else switch (_bt) {
        case 0:
			_buf += ' ';
            return;
        case 0x1B: // Force lower case.
			_state = 2;
            return;
        case 0x1C: // The prefix code.
            _state = 1;
            return;
        case 0x1D:
			if( labelMPlike )
				_buf += "~[0x1d]";
			else
				_buf += char(0x1d);
            return;
        case 0x1E:
			// Separation: on the map visible only the second section (when over 1 km), 
            // with the mouse sees displayed one the word completely, by blank separated.
			if( labelMPlike )
				_buf += "~[0x1e]";
			else
				_buf += char(0x1e);
			return;
        case 0x1F:
			// Separation: on the map visible only the first section (when over 1 km), 
            // with the mouse sees displayed one the word completely, by blank separated.
            // Normaly used as a separator before elevation.
			if( labelMPlike )
				_buf += "~[0x1f]";
			else
				_buf += char(0x1f);
			return;
        case 0x20: // 0-9
		case 0x21:
		case 0x22:
		case 0x23:
		case 0x24:
		case 0x25:
		case 0x26:
		case 0x27:
		case 0x28:
		case 0x29:
            _buf +=  0x10 + _bt;
			return;
        case 0x2A: // Interstate highway
			if( labelMPlike )
				_buf += "~[0x2a]";
			else
				_buf += char(0x2a);
            return;
        case 0x2B: // Highway - shield
			if( labelMPlike )
				_buf += "~[0x2b]";
			else
				_buf += char(0x2b);
            return;
        case 0x2C: // Highway - round symbol
			if( labelMPlike )
				_buf += "~[0x2c]";
			else
				_buf += char(0x2c);
            return;
        case 0x2D: // Highway - small symbol - blue/red
			if( labelMPlike )
				_buf += "~[0x2d]";
			else
				_buf += char(0x2d);
            return;
        case 0x2E: // Highway - big symbol
			if( labelMPlike )
				_buf += "~[0x2e]";
			else
				_buf += char(0x2e);
            return;
        case 0x2F: // Highway - small symbol - white
			if( labelMPlike )
				_buf += "~[0x2f]";
			else
				_buf += char(0x2f);
            return;
    }
}

std::string IMG_internal_list::readLBL(const long& address) {
	//jesli 6bitowe - czytanie po 4 bajty, 8bitowe - po 1 bajcie
	std::string	t_return;
	//return string("a");

	unsigned long t_address = selected_file->lbl1_data;
	int			state = 0;

	t_address += (unsigned long)(address<<selected_file->lbl_shift);

	if( address == 0 )
		return t_return;

	if( selected_file->lbl_coding == 6 ) {
		for(;;) {
			readData(t_address,3,2);

			const unsigned char bt1 =  (read_buffer2[0]) >> 2;
			const unsigned char bt2 = ((read_buffer2[0] & 0x03) << 4) + ((read_buffer2[1]) >> 4);
			const unsigned char bt3 = ((read_buffer2[1] & 0x0F) << 2) + ((read_buffer2[2]) >> 6);
			const unsigned char bt4 =  (read_buffer2[2] & 0x3F);

			if (bt1 > 0x2F)	break;
			append6bitSymbol (state, bt1, t_return,labelMPlike);

			if (bt2 > 0x2F) break;
			append6bitSymbol (state, bt2, t_return,labelMPlike);

			if (bt3 > 0x2F) break;
			append6bitSymbol (state, bt3, t_return,labelMPlike);

			if (bt4 > 0x2F) break;
			append6bitSymbol (state, bt4, t_return,labelMPlike);

			t_address += 3;
		}

	} else {
		for(;;) {
			readData(t_address++,1,2);
			if( read_buffer2[0] == 0 ) break;

			switch (read_buffer2[0]) {
				case 0:
					return t_return;
					// Interstate highway
				case 0x01: 
					if( labelMPlike )
						t_return+="~[0x01]";
					else
						t_return+=char(0x01);
					break;
					// Highway - shield
				case 0x02: 
					if( labelMPlike )
						t_return+="~[0x02]";
					else
						t_return+=char(0x02);
					break;
					// Highway - round symbol
				case 0x03: 
					if( labelMPlike )
						t_return+="~[0x03]";
					else
						t_return+=char(0x03);
					break;
					// Highway - small symbol - blue/red (E-highway).
				case 0x04: 
					if( labelMPlike )
						t_return+="~[0x04]";
					else
						t_return+=char(0x04);
					break;
					// Highway - big symbol (D-highway).
				case 0x05: 
					if( labelMPlike )
						t_return+="~[0x05]";
					else
						t_return+=char(0x05);
					break;
					// Highway - small symbol - white (Internal highway).
				case 0x06: 
					if( labelMPlike )
						t_return+="~[0x06]";
					else
						t_return+=char(0x06);
					break;

					// Only the second part is shown; with the cursor you see the whole word.
				case 0x1b: 
					if( labelMPlike )
						t_return+="~[0x1b]";
					else
						t_return+=char(0x1b);
					break;
					// Only the first part is shown; with the cursor you see the whole word.
				case 0x1c: 
					if( labelMPlike )
						t_return+="~[0x1c]";
					else
						t_return+=char(0x1c);
					break;
					// In areas type 0x4A = <map_name>1D<number_of_detail_map>
					// In LBL2/LBL3 = <long_name>1D<short_name>
				case 0x1d: 
					if( labelMPlike )
						t_return+="~[0x1d]";
					else
						t_return+=char(0x1d);
					break;
					// Only the second part is shown; with the cursor you see the whole word, separated by a blank.
				case 0x1e: 
					if( labelMPlike )
						t_return+="~[0x1e]";
					else
						t_return+=char(0x1e);
					break;
					// Only the first part is shown; with the cursor you see the whole word, separated by a blank.
				case 0x1f: 
					if( labelMPlike )
						t_return+="~[0x1f]";
					else
						t_return+=char(0x1f);
					break;
				default:
					t_return += read_buffer2[0];
			}
		}
	}
#if TRIAL == 1
	if( t_return.size() > 10 )
		t_return = t_return.substr(10);
#endif

	return t_return;
}

inline 
void IMG_internal_list::readData(const unsigned long& address,size_t size,const unsigned int& buffer) {
	if( buffer == 1 ) {
		if( address > img_size )
			throw runtime_error("trying to read after file");
		if( address + size > img_size )
			size = img_size - address;

		if( size > READ_BUFFER_SIZE )
			size = READ_BUFFER_SIZE;

		memcpy(read_buffer,(void*)(&img_data[address]),size);
		if( img_xor ) {
			for( size_t t_s=0; t_s< size; t_s++ )
				read_buffer[t_s] ^= img_xor;
		}
	} else if( buffer == 2 ) {
		if( address > img_size )
			throw runtime_error("trying to read after file");
		if( address + size > img_size )
			size = img_size - address;

		if( size > READ_BUFFER_SIZE )
			size = READ_BUFFER_SIZE;

		memcpy(read_buffer2,(void*)(&img_data[address]),size);
		if( img_xor ) {
			for( size_t t_s=0; t_s< size; t_s++ )
				read_buffer2[t_s] ^= img_xor;
		}
	}
}

void IMG_internal_list::initialiseReadBits(const unsigned long& start_address,const unsigned int& total_number_of_bytes) {
	bit_position = 0;
	bit_stream_len = total_number_of_bytes;

	if( total_number_of_bytes > READ_BUFFER_SIZE-1 )
		throw runtime_error("too big chunk size for bit reading");

	readData(start_address,total_number_of_bytes + 20);
}

inline 
unsigned long IMG_internal_list::readBits(const unsigned short &number_of_bits) {

	//Remember what we have in the read buffer already
	unsigned int		bits_bs;
	unsigned long	bits_res;

	bits_res = 0;
	bits_bs = (bit_position >> 3);

	if( bit_stream_len >= bits_bs )		
		bits_res  = (read_buffer[bits_bs+0]);
	if( bit_stream_len >= bits_bs+1 ) 
		bits_res |= static_cast<long>(read_buffer[bits_bs+1])<<8L;
	if( bit_stream_len >= bits_bs+2 )
		bits_res |= static_cast<long>(read_buffer[bits_bs+2])<<16L;
	if( bit_stream_len >= bits_bs+3 )
		bits_res |= static_cast<long>(read_buffer[bits_bs+3])<<24L;

	if( bit_position & 7 ) {
		if( bit_stream_len > bits_bs+4 )
			bits_res = (bits_res >> static_cast<unsigned __int32>(bit_position & 7L)) | (static_cast<long>(read_buffer[bits_bs+4L]) << static_cast<unsigned __int32>(32L - (bit_position & 7L)));
		else
			bits_res = (bits_res >> static_cast<unsigned __int32>(bit_position & 7L)) | (0L << static_cast<unsigned __int32>(32 - (bit_position & 7L)));
	}

	bit_position += number_of_bits;
	return( (bits_res << static_cast<unsigned __int32>(32L-number_of_bits)) >> static_cast<unsigned __int32>(32L-number_of_bits) );
}


inline
long IMG_internal_list::readValue(const unsigned short &number_of_bits,const char &type) {
	long	Special;
	long	lDX;
	long	bits_res_value;

	Special = 1UL << (number_of_bits - 1L);

	//cout<<(int)number_of_bits<<" "<<(int)type<<":"<<(int)read_buffer[(bit_position >> 3)]<<"->";
	lDX = 0;
	if (type == 0) {
		// Variable.
		//long d;
		while (true) {
			bits_res_value = static_cast<long>( readBits(number_of_bits));//d = 0;
			//cout<<bit_position<<":"<<(long)bits_res_value<<" ";
			if (bits_res_value != Special)     // special case
				break;
			lDX += bits_res_value - 1;
		}
		if (bits_res_value < Special)          // positive
			lDX += bits_res_value;
		else {
			//assert (d > lXSpecial); // negative
			lDX = (bits_res_value - 2*Special) - lDX;
		}
	} else {
		// Fixed.
		lDX = static_cast<long>(readBits(number_of_bits));
		if (type == 1)
			lDX = -lDX;
	}
	//cout<<lDX<<" ";
	return lDX;
}

void IMG_internal_list::readRGNExtendedTable(const tre& tre_data,rgn_extended & rgn,const unsigned short& tre_idx) {	
	int				tre8_pos = 0;
	unsigned short	tre8_index = tre_idx-1;
	unsigned short	tre_first_valid = 1;
	unsigned long	tre8_data_start = selected_file->tre8_data;// + tre_data.tre_start;	
	bool			flag_exist = false;
	int				t_rec_len = selected_file->tre8_rec_len;

	memset(&rgn,0,sizeof rgn);

	if( t_rec_len < 4 )
		return;

	//zalozenie :
	// tre8_rec_len > 4+4+4 - wszystkie warstwy
	// tre8_rec_len < 4+4+4 - bez top ??

	if( t_rec_len < 0x0d ) {
		while( t_rec_len >= 4 )
			t_rec_len-=4;
		if( t_rec_len )
			flag_exist = true;
	} else
		flag_exist = true;

	if( flag_exist ) {
		tre_first_valid = 1;	
		assert( (selected_file->tre8_data_len / selected_file->tre8_rec_len - 1) == selected_file->map_level.last_tre_idx );
	} else if( selected_file->tre8_rec_len <= 0x0c && selected_file->tre8_rec_len > 4 ) {
		tre_first_valid = selected_file->map_level.level_start_index[selected_file->map_level.map_levels-1];
		assert( (selected_file->tre8_data_len / selected_file->tre8_rec_len - 1) == (selected_file->map_level.last_tre_idx - (tre_first_valid-1)) );
	} else
		return;
	if( tre_idx < tre_first_valid )
		return;

	tre8_index -= (tre_first_valid-1);

	tre8_data_start += (tre8_index * selected_file->tre8_rec_len);

	readData(tre8_data_start,selected_file->tre8_rec_len + 12);

	if( selected_file->tre8_flag & 1 ) {
		memcpy(&rgn.rgn2_address,&read_buffer[tre8_pos],4);
		tre8_pos+=4;
	}
	if( selected_file->tre8_flag & 2 ) {
		memcpy(&rgn.rgn3_address,&read_buffer[tre8_pos],4);
		tre8_pos+=4;
	}
	if( selected_file->tre8_flag & 4 ) {
		memcpy(&rgn.rgn4_address,&read_buffer[tre8_pos],4);
		tre8_pos+=4;
	}

	tre8_pos += (selected_file->tre8_rec_len - tre8_pos);

	if( selected_file->tre8_flag & 1 ) {
		memcpy(&rgn.rgn2_address_end,&read_buffer[tre8_pos],4);
		tre8_pos+=4;
	}
	if( selected_file->tre8_flag & 2 ) {
		memcpy(&rgn.rgn3_address_end,&read_buffer[tre8_pos],4);
		tre8_pos+=4;
	}
	if( selected_file->tre8_flag & 4 ) {
		memcpy(&rgn.rgn4_address_end,&read_buffer[tre8_pos],4);
		tre8_pos+=4;
	}

	rgn.rgn2_address += selected_file->rgn2_data;
	rgn.rgn2_address_end += selected_file->rgn2_data;

	rgn.rgn3_address += selected_file->rgn3_data;
	rgn.rgn3_address_end += selected_file->rgn3_data;

	rgn.rgn4_address += selected_file->rgn4_data;
	rgn.rgn4_address_end += selected_file->rgn4_data;
}

//RGN
void IMG_internal_list::readRGNTable(const tre& tre_data,rgn_table& rgn,const bool& parent) {
	char			rgn_type = 0;
	unsigned long	rgn_data_start = selected_file->rgn1_data + tre_data.rgn_start;

	rgn.rgn_address[0] = rgn_data_start;
	if( parent )
		rgn.rgn_address[1] = rgn.rgn_address[2] = rgn.rgn_address[3] = rgn.rgn_address[4] = selected_file->rgn1_data + tre_data.parent.rgn_end;
	else
		rgn.rgn_address[1] = rgn.rgn_address[2] = rgn.rgn_address[3] = rgn.rgn_address[4] = selected_file->rgn1_data + tre_data.child.rgn_end;

	readData(rgn.rgn_address[0],8);

	rgn.types = ((tre_data.types  & 0x1) ? 1 : 0 ) +
		((tre_data.types & 0x2) ? 1 : 0) +
		((tre_data.types & 0x4) ? 1 : 0) +
		((tre_data.types & 0x8) ? 1 : 0);
	if( rgn.types > 1 ) {
		rgn.rgn_address[rgn_type] += (rgn.types-1)*2;
		rgn.rgn_address[rgn_type+1] = rgn_data_start + ConvertB2ToUInt16(&read_buffer[rgn_type*2]);
		rgn_type++;
	}
	if( rgn.types > 2 ) {
		rgn.rgn_address[rgn_type+1] = rgn_data_start + ConvertB2ToUInt16(&read_buffer[rgn_type*2]);
		rgn_type++;
	}
	if( rgn.types > 3 ) {
		rgn.rgn_address[rgn_type+1] = rgn_data_start + ConvertB2ToUInt16(&read_buffer[rgn_type*2]);
		rgn_type++;
	}
}


void IMG_internal_list::closeMap() {
	if( /*file_name.size() &&*/ img_file != NULL ) {
		delete img_file;
		delete img_handle;
		//UnmapViewOfFile(img_data);
		//CloseHandle(img_file);
	}
	img_file = NULL;
}

bool IMG_internal_list::selectMap() {

	//if( file_name.size() == 0 ) return false;

#ifdef _mapowanie
	try {
		img_file = new boost::interprocess::file_mapping(file_name.c_str(),boost::interprocess::read_only);
	}  catch (exception &e) {
		cout<<flush<<e.what()<<endl;
		return -1;
	}
#else
	
	img_file = CreateFile(file_name.c_str(),GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	lastError = GetLastError();
	if( img_file == INVALID_HANDLE_VALUE ) {
		return false;
	}
#endif

#ifdef _mapowanie
	try {
		img_handle = new boost::interprocess::mapped_region(*img_file,boost::interprocess::read_only);
		img_size = img_handle->get_size();
		img_data = (unsigned char*)img_handle->get_address();
	}  catch (exception &e) {
		cout<<flush<<e.what()<<endl;
		return -1;
	}
#else
	img_handle = CreateFileMapping(img_file,NULL,PAGE_READONLY,0,0,0);
	lastError = GetLastError();
	if( img_handle == NULL ) {
		CloseHandle(img_file);
		return false;
	}

	img_data = (unsigned char *) MapViewOfFile(img_handle,FILE_MAP_READ,0,0,0);
	lastError = GetLastError();
	if( img_file == NULL ) {
		CloseHandle(img_file);
		return false;
	}
#endif
	return true;	
}

int	IMG_internal_list::getLabelCoding(int sub_map) {
	if( sub_map <= internal_files.size() ) {
		selected_file = internal_files.begin() + (sub_map-1);
		return (*selected_file).lbl_coding;
	}
	return 0;
}

int	IMG_internal_list::getCodePage(int sub_map) {
	if( sub_map <= internal_files.size() ) {
		selected_file = internal_files.begin() + (sub_map-1);
		return (*selected_file).code_page;
	}
	return 0;
}

void  IMG_internal_list::readMap_details(int sub_map,mapRead_rect bounds,mapRead_coord minimum_size,int details) {

	//convert bounds to long
	int	layer = 0;
	X0 = static_cast<long>(bounds.x0 / c_fGarminUnit);
	X1 = static_cast<long>(bounds.x1 / c_fGarminUnit);

	Y0 = static_cast<long>(bounds.y0 / c_fGarminUnit);
	Y1 = static_cast<long>(bounds.y1 / c_fGarminUnit);

	if( X0 > X1 ) std::swap(X0,X1);
	if( Y0 > Y1 ) std::swap(Y0,Y1);

	minX = long(minimum_size.ns / c_fGarminUnit);
	minY = long(minimum_size.we / c_fGarminUnit);


	interrupt = false;

	if( sub_map <= 0 ) {
		for( selected_file = internal_files.begin(); selected_file != internal_files.end(); ++selected_file ) {
			for( layer = 0; layer < (*selected_file).map_level.map_levels; ++layer ) {
				if( (*selected_file).map_level.level[layer] == details ) break;
				if( (*selected_file).map_level.level[layer] < details ) break;
			}
			active_layer = static_cast<char>(layer);
			redrawTRE(1);
			if( interrupt ) break;
		}
	}
	else if( sub_map <= internal_files.size() ) {
		selected_file = internal_files.begin() + (sub_map-1);
		for( layer = 0; layer < (*selected_file).map_level.map_levels; ++layer ) {
			if( (*selected_file).map_level.level[layer] == details ) break;
			if( (*selected_file).map_level.level[layer] < details ) break;
		}	
		active_layer = static_cast<char>(layer);
		redrawTRE(1);
	}
}

void IMG_internal_list::readMap(int sub_map,mapRead_rect bounds,mapRead_coord minimum_size,int layer) {

	//convert bounds to long
	X0 = static_cast<long>(bounds.x0 / c_fGarminUnit);
	X1 = static_cast<long>(bounds.x1 / c_fGarminUnit);

	Y0 = static_cast<long>(bounds.y0 / c_fGarminUnit);
	Y1 = static_cast<long>(bounds.y1 / c_fGarminUnit);

	if( X0 > X1 ) std::swap(X0,X1);
	if( Y0 > Y1 ) std::swap(Y0,Y1);

	minX = long(minimum_size.ns / c_fGarminUnit);
	minY = long(minimum_size.we / c_fGarminUnit);

	if( layer < 0 || layer > 9 )
		return;
	active_layer = static_cast<char>(layer);

	interrupt = false;

	if( sub_map <= 0 ) {
		for( selected_file = internal_files.begin(); selected_file != internal_files.end(); ++selected_file ) {
			redrawTRE(1);
			if( interrupt ) break;
		}
	}
	else if( sub_map <= internal_files.size() ) {
		selected_file = internal_files.begin() + (sub_map-1);
		redrawTRE(1);
	}
}

bool IMG_internal_list::readMap(int road_idx) {
	unsigned short tre_idx;
	short	rgn_idx;
	unsigned char layer;
	unsigned long address;
	long	bit_shift;
	long	start_x;
	long	start_y;

	tre_idx = (road_idx & 0xffff00) >> 8;
	rgn_idx = road_idx & 0xff;

	current_TRE = tre_idx;
	current_RGN = rgn_idx;

	X0 = static_cast<long>(-180.0/ c_fGarminUnit);
	X1 = static_cast<long>(180.0 / c_fGarminUnit);

	Y0 = static_cast<long>(-90.0 / c_fGarminUnit);
	Y1 = static_cast<long>(90.0 / c_fGarminUnit);

	if( locateRGN40(tre_idx,rgn_idx,layer,address,bit_shift,start_x,start_y) == false )
		return false;
	exportPolyline(layer,0x40,address,bit_shift,start_x,start_y);
	return true;
}

/**
* Sprawdzenie czy aktualny widoczny obszar pokrywa sie 
* z TRE wskazywanym przez tre_address
*/
bool IMG_internal_list::visibleTRE(const tre &tre_data,const char &layer) {
	long	x0,y0,dx,dy;

	x0 = ConvertB3ToInt(tre_data.x);
	y0 = ConvertB3ToInt(tre_data.y);

	dx = ConvertB2ToUInt16(tre_data.x_span) & 0x7fff;
	dx <<= static_cast<long>(24-selected_file->map_level.level[layer]);

	dy = ConvertB2ToUInt16(tre_data.y_span) & 0x7fff;
	dy <<= static_cast<long>(24-selected_file->map_level.level[layer]);

	if( X0 > x0+dx ) return false;
	if( X1 < x0-dx ) return false;
	if( Y0 > y0+dy ) return false;
	if( Y1 < y0-dy ) return false;
	return true;
}


/**
* Sprawdzenie czy aktualny widoczny obszar pokrywa sie 
* z TRE wskazywanym przez tre_idx
*/
bool IMG_internal_list::visibleTRE(const unsigned short &tre_idx) {
	unsigned long	tre_address;
	unsigned char	layer = 0;
	tre				tre_data;

	//wyszukanie w ktorej warstwie
	while( tre_idx < selected_file->map_level.level_start_index[layer] ) 
		layer++;

	tre_address = selected_file->map_level.level_address[layer] + (selected_file->map_level.level_start_index[layer] - tre_idx) * ( layer ? 16 : 14 );

	return visibleTRE(tre_data,layer);
}

/**
* Iterowanie TRE wraz z rysowaniem
* @return	false jezeli ostatnie dziecko
*/
bool IMG_internal_list::redrawTRE(const unsigned short &tre_idx) {
	//active_layer
	unsigned short	child_first;
	unsigned long	tre_address;
	unsigned char	layer = 0;
	tre				tre_data;

	//wyszukanie w ktorej warstwie
	while( tre_idx < selected_file->map_level.level_start_index[layer] ) 
		layer++;

	if( layer < active_layer )
		return false;	

	tre_address = selected_file->map_level.level_address[layer] + (tre_idx - selected_file->map_level.level_start_index[layer]) * ( layer ? 16 : 14 );

	readData(tre_address + selected_file->tre_file_data(),sizeof tre_data);
	memcpy(&tre_data,read_buffer,sizeof tre_data);

 	current_TRE = tre_idx;
	if( visibleTRE(tre_data,layer) ) {
		if( layer == active_layer ) {
			drawTRE(tre_data);
			if( interrupt == true )
				return false;
		} else {
			if( layer > active_layer ) {
				child_first = ConvertB2ToUInt16(tre_data.parent.tre_child);
				if( child_first ) {
					while( redrawTRE(child_first) == true ) {
						if( interrupt ) return false;
						child_first++;
					}
					if( interrupt ) return false;
				}
			}
		}
	}

	if( interrupt ) return false;
	if( (tre_data.x_span[1] & 0x80) == 0 )	
		return true;
	return false;
}

/** 
* Wlasciwe rysowanie regionu tutaj do zaimplementowania
* @return	false - przerywa proces odrysowywania
*/
void IMG_internal_list::drawTRE(const tre &tre_data) {
	unsigned long	rgn_address;
	unsigned long	rgn_address_end;
	unsigned int	object_id;
	unsigned int	rgn_id;

	const long bit_shift = 24 - selected_file->map_level.level[active_layer];
	rgn_table		rgn;
	rgn_extended 	rgn_extended;

	const long start_x = ConvertB3ToInt(tre_data.x);
	const long start_y = ConvertB3ToInt(tre_data.y);

	current_RGN = 1;
	readRGNTable(tre_data,rgn,active_layer != 0);
	readRGNExtendedTable(tre_data,rgn_extended,current_TRE);

	//ustawiam na ostatni
	int	rgn_type = 0;//rgn.types-1;
	//rysowanie - kolejnosc 10,20,40,80

	//marine
	if( rgn_extended.rgn4_address < rgn_extended.rgn4_address_end ) {
		rgn_address = rgn_extended.rgn4_address;
		rgn_address_end = rgn_extended.rgn4_address_end;
		while( rgn_address < rgn_address_end ) {
			rgn_address += exportPoint(active_layer,0x04,rgn_address,bit_shift,start_x,start_y);
			if( interrupt )	return;
		}	
	}

	//land
	if( tre_data.types & 0x2 ) {
		if( tre_data.types & 0x1 )
			rgn_type+=1;

		rgn_address = rgn.rgn_address[rgn_type];
		rgn_address_end = rgn.rgn_address[rgn_type+1];

		while( rgn_address < rgn_address_end ) {
			rgn_address += exportPoint(active_layer,0x20,rgn_address,bit_shift,start_x,start_y);
			current_RGN++;
			if( interrupt )	return;
		}	
	}

	if( tre_data.types & 0x1 ) {
		rgn_type = 0;
		rgn_address = rgn.rgn_address[rgn_type];
		rgn_address_end = rgn.rgn_address[rgn_type+1];

		while( rgn_address < rgn_address_end ) {
			rgn_address += exportPoint(active_layer,0x10,rgn_address,bit_shift,start_x,start_y);
			current_RGN++;
			if( interrupt )	return;
		}	
	}

	//marine - lines
	if( rgn_extended.rgn3_address < rgn_extended.rgn3_address_end ) {
		rgn_address = rgn_extended.rgn3_address;
		rgn_address_end = rgn_extended.rgn3_address_end;
		while( rgn_address < rgn_address_end ) {
			rgn_address += exportPolyline(active_layer,0x03,rgn_address,bit_shift,start_x,start_y);
			if( interrupt )	return;
		}	
	}

	//land - lines
	rgn_type = 0;
	if( tre_data.types & 0x1 )	rgn_type++;
	if( tre_data.types & 0x2 )	rgn_type++;

	current_RGN = 1;
	if( tre_data.types & 0x4 ) {
		rgn_address = rgn.rgn_address[rgn_type];
		rgn_address_end = rgn.rgn_address[rgn_type+1];
				
		while( rgn_address < rgn_address_end ) {
			rgn_address += exportPolyline(active_layer,0x40,rgn_address,bit_shift,start_x,start_y);
			current_RGN++;
			if( interrupt )	return;
		}	
		rgn_type++;
	}

	//marine - polygons
	if( rgn_extended.rgn2_address < rgn_extended.rgn2_address_end ) {
		rgn_address = rgn_extended.rgn2_address;
		rgn_address_end = rgn_extended.rgn2_address_end;
		while( rgn_address < rgn_address_end ) {
			rgn_address += exportPolyline(active_layer,0x02,rgn_address,bit_shift,start_x,start_y);
			if( interrupt )	return;
		}	
	}

	//land
	current_RGN = 1;
	if( (tre_data.types & 0x8) /*&& draw_rgn80*/ ) {
		rgn_address = rgn.rgn_address[rgn_type];
		rgn_address_end = rgn.rgn_address[rgn_type+1];

		while( rgn_address < rgn_address_end ) {
			rgn_address += exportPolyline(active_layer,0x80,rgn_address,bit_shift,start_x,start_y);
			current_RGN++;
			if( interrupt )	return;
		}			
	}
}

bool IMG_internal_list::locateRGN40(const unsigned short& tre_idx,const short& rgn_idx,unsigned char &layer,unsigned long& address,long& bit_shift,long& start_x,long& start_y) {
	//                                                    in                   in                     out               out              out             out           out
	unsigned long	tre_address;
	layer = 0;
	unsigned char	prev_active_layer = active_layer;
	unsigned long	rgn_address;
	unsigned long	rgn_address_end;
	tre				tre_data;
	rgn_table		rgn;
	int				rgn_type;
	short			local_rgn_idx = 0;

	if( !tre_idx || !rgn_idx || rgn_idx > 255 )
		return false;

	//1. locate TRE
	//wyszukanie w ktorej warstwie
	while( tre_idx < selected_file->map_level.level_start_index[layer] ) 
		layer++;
	active_layer = layer;

	//parametr
	bit_shift = 24 - selected_file->map_level.level[layer];

	tre_address = selected_file->map_level.level_address[layer] + (tre_idx - selected_file->map_level.level_start_index[layer]) * ( layer ? 16 : 14 );

	readData(tre_address + selected_file->tre_file_data(),sizeof tre_data);
	memcpy(&tre_data,read_buffer,sizeof tre_data);

	//2. tre_data zawiera dane TRE - lokalizacja RGN_IDX
	//parametry
	start_x = ConvertB3ToInt(tre_data.x);
	start_y = ConvertB3ToInt(tre_data.y);

	//3. zaladowac tabele RGN
	readRGNTable(tre_data,rgn,active_layer != 0);

	//wyszukanie rgn_idx
	rgn_type = 0;
	if( tre_data.types & 0x1 )	rgn_type++;
	if( tre_data.types & 0x2 )	rgn_type++;

	if( tre_data.types & 0x4 ) {
		rgn_address = rgn.rgn_address[rgn_type];
		rgn_address_end = rgn.rgn_address[rgn_type+1];

		while( rgn_address < rgn_address_end ) {
			++local_rgn_idx;

			if( local_rgn_idx == rgn_idx ) {
				address = rgn_address;
				active_layer = prev_active_layer;
				return true;
			} else {
				readData(rgn_address,10);
				if( read_buffer[0] & 0x80 ) {
					int stream_len = 0;
					memcpy(&stream_len,&read_buffer[8],2); 
					rgn_address = rgn_address + 11 + stream_len;
				} else {
					int stream_len = static_cast<unsigned int>(read_buffer[8]);
					rgn_address = rgn_address + stream_len + 10;
				}
			}
		}	
	}

	active_layer = prev_active_layer;
	return false;
}

/**
@return	true - is located, false - cannot be found
*/
bool IMG_internal_list::locateRGN1020(const unsigned short& tre_idx,const short& rgn_idx,t_coord& coordinates,std::string& label,unsigned long& lbl_address) {

	unsigned long	tre_address;
	unsigned char	layer = 0;
	unsigned char	prev_active_layer = active_layer;
	unsigned long	rgn_address;
	unsigned long	rgn_address_end;
	tre				tre_data;
	rgn_table		rgn;
	int				rgn_type;
	short			local_rgn_idx = 0;

	if( !tre_idx || !rgn_idx || rgn_idx > 255 )
		return false;

	//1. locate TRE
	//wyszukanie w ktorej warstwie
	while( tre_idx < selected_file->map_level.level_start_index[layer] ) 
		layer++;
	active_layer = layer;

	const long bit_shift = 24 - selected_file->map_level.level[layer];

	tre_address = selected_file->map_level.level_address[layer] + (tre_idx - selected_file->map_level.level_start_index[layer]) * ( layer ? 16 : 14 );

	readData(tre_address + selected_file->tre_file_data(),sizeof tre_data);
	memcpy(&tre_data,read_buffer,sizeof tre_data);

	//2. tre_data zawiera dane TRE - lokalizacja RGN_IDX
	const long start_x = ConvertB3ToInt(tre_data.x);
	const long start_y = ConvertB3ToInt(tre_data.y);

	//3. zaladowac tabele RGN
	readRGNTable(tre_data,rgn,active_layer != 0);

	//wyszukanie rgn_idx
	rgn_type = 0;
	if( tre_data.types & 0x1 )
		rgn_type = 1;

	if( tre_data.types & 0x2 ) {
		rgn_address = rgn.rgn_address[rgn_type];
		rgn_address_end = rgn.rgn_address[rgn_type+1];

		while( rgn_address < rgn_address_end ) {
			++local_rgn_idx;

			if( local_rgn_idx == rgn_idx ) {
				readData(rgn_address,8);
				memcpy(&lbl_address,&read_buffer[1],3);
				lbl_address &= 0x3fffff;
				coordinates.we = (start_x + (ConvertB2ToInt16(&read_buffer[4]) << bit_shift))*c_fGarminUnit;
				coordinates.ns = (start_y + (ConvertB2ToInt16(&read_buffer[6]) << bit_shift))*c_fGarminUnit;
				if( read_buffer[3] & 0x40 ) {
					readData(lbl_address + selected_file->lbl6_data,3);
					memcpy(&lbl_address,&read_buffer[0],3);
					lbl_address &= 0x3fffff;
				}
				label = readLBL(lbl_address);
				active_layer = prev_active_layer;
				return true;
			} else {
				readData(rgn_address,4);
				if( read_buffer[3] & 0x80 ) 
					rgn_address += 9;		
				else
					rgn_address += 8;	
			}			
		}	
		--rgn_type;
	}

	if( tre_data.types & 0x1 ) {
		rgn_address = rgn.rgn_address[rgn_type];
		rgn_address_end = rgn.rgn_address[rgn_type+1];

		while( rgn_address < rgn_address_end ) {
			++local_rgn_idx;

			if( local_rgn_idx == rgn_idx ) {
				readData(rgn_address,8);
				memcpy(&lbl_address,&read_buffer[1],3);
				lbl_address &= 0x3fffff;
				coordinates.we = (start_x + (ConvertB2ToInt16(&read_buffer[4]) << bit_shift))*c_fGarminUnit;
				coordinates.ns = (start_y + (ConvertB2ToInt16(&read_buffer[6]) << bit_shift))*c_fGarminUnit;
				if( read_buffer[3] & 0x40 ) {
					readData(lbl_address + selected_file->lbl6_data,3);
					memcpy(&lbl_address,&read_buffer[0],3);
					lbl_address &= 0x3fffff;
				}
				label = readLBL(lbl_address);
				active_layer = prev_active_layer;
				return true;
			} else {
				readData(rgn_address,4);
				if( read_buffer[3] & 0x80 ) 
					rgn_address += 9;		
				else
					rgn_address += 8;	
			}			
		}	
		++rgn_type;
	}

	active_layer = prev_active_layer;
	return false;
}

int IMG_internal_list::exportPolylineSegments(const int layer,const int type,const unsigned long& address,const long& bit_shift,const long& start_x,const long& start_y,mapRead_address segments[_MAX_SEGMENTS],unsigned int &t_address_size) {
	unsigned long	t_len = 10;
	unsigned int	rgn_type;
	unsigned int	lbl_address;
	unsigned int	net_address = 0;
	bool			net_element = false;
	bool			multi_segment = false;
	bool			segment_change;
	long			t_x0,t_x1,t_y0,t_y1;
	int				t_curr_segment = 0;

	//polyline decoding
	unsigned int	stream_len;
	unsigned long	t_address;
	long			x_total,y_total;
	unsigned short	x_len,y_len;
	char			x_type,y_type;
	unsigned int	coord_no;
	unsigned int	coord_no_i;

	memset(segments,0,sizeof segments);

	coord_no = 0;
	readData(address,10);
	if( read_buffer[0] & 0x80 ) {
		stream_len = 0;
		memcpy(&stream_len,&read_buffer[8],2); 
		t_len = 11 + stream_len;
		t_address = address + 10;
	} else {
		stream_len = static_cast<unsigned int>(read_buffer[8]);
		t_len = stream_len + 10;
		t_address = address + 9;
	}

	//czytanie danych
	memcpy(&lbl_address,&read_buffer[1],3);
	lbl_address &= 0x3fffff;

	if( type == 0x80 ) {
		rgn_type = static_cast<unsigned int>(read_buffer[0]) & 0x7f; 
	} else {
		if( read_buffer[3] & 0x80 )	{
			net_element = true;	
			net_address = lbl_address;
			net_address = (net_address<< selected_file->net1_modif);
		}
		if( read_buffer[3] & 0x40 )	multi_segment = true;	
		rgn_type = static_cast<unsigned int>(read_buffer[0]) & 0x3f; 
	}

	coords_int[0].we = (start_x + (ConvertB2ToInt16(&read_buffer[4]) << bit_shift));
	coords_int[0].ns = (start_y + (ConvertB2ToInt16(&read_buffer[6]) << bit_shift));

	t_x0 = t_x1 = coords_int[0].we;
	t_y0 = t_y1 = coords_int[0].ns;

	initialiseReadBits(t_address,stream_len);

	//typ kodowania
	x_len = static_cast<unsigned short>(readBits(4));
	y_len = static_cast<unsigned short>(readBits(4));

	x_len = x_len<=9 ? x_len+2 : (2 * x_len)-7;	
	y_len = y_len<=9 ? y_len+2 : (2 * y_len)-7;

	if( readBits(1) ) {
		if( readBits(1) ) 
			x_type = 1;
		else
			x_type = 2;
	} else {
		++x_len;
		x_type = 0;
	}

	if( readBits(1) ) {
		if( readBits(1) )
			y_type = 1;
		else
			y_type = 2;
	} else {
		++y_len;
		y_type = 0;
	}

	x_total = coords_int[0].we;
	y_total = coords_int[0].ns;

	//if( multi_segment ) {
	//zawsze conajmniej jeden segment!
	segments[t_curr_segment].segment = 0;
	t_curr_segment++;
	//}

	coord_no++;	
	for(;;) {
		if( multi_segment ) {
			if( readBits(1) != 0 ) {
				segments[t_curr_segment].segment = (coord_no-1);
				t_curr_segment++;
				segment_change = true;
			}
		} 

		//Ustawiany segment, ale koordynaty s¹ identyczne jak poprzednie...

		x_total += (readValue(x_len,x_type) << bit_shift);//we
		y_total += (readValue(y_len,y_type) << bit_shift);//ns

		if( coords_int[coord_no-1].we != x_total || coords_int[coord_no-1].ns != y_total ) {
			coords_int[coord_no].we = x_total;
			coords_int[coord_no].ns = y_total;

			t_x0 = min(t_x0,x_total);
			t_x1 = max(t_x1,x_total);

			t_y0 = min(t_y0,y_total);
			t_y1 = max(t_y1,y_total);
		
			++coord_no;
		} else if (segment_change) {
			t_curr_segment--;
		}

		if( (bit_position >> 3) > stream_len ) break;
		segment_change = false;
	}


/*
	if( X0 > t_x1 ) 
		return t_len;
	if( X1 < t_x0 ) 
		return t_len;
	if( Y0 > t_y1 ) 
		return t_len;
	if( Y1 < t_y0 ) 
		return t_len;
*/
	t_address_size = t_curr_segment;

	t_curr_segment = 0;
	for( coord_no_i = 0; coord_no_i <= coord_no; ++coord_no_i ) {
		coords[coord_no_i].we = static_cast<float>(coords_int[coord_no_i].we) * c_fGarminUnit;
		coords[coord_no_i].ns = static_cast<float>(coords_int[coord_no_i].ns) * c_fGarminUnit;

		if( segments[t_curr_segment].segment == coord_no_i ) {
			segments[t_curr_segment].segment_coord.ns = coords[coord_no_i].ns;
			segments[t_curr_segment].segment_coord.we = coords[coord_no_i].we;
			t_curr_segment++;
		}
	}
	return coord_no-1;
}

unsigned long IMG_internal_list::exportPolyline(const int layer,const int type,const unsigned long& address,const long& bit_shift,const long& start_x,const long& start_y) {
	unsigned long	t_len = 10;
	unsigned int	rgn_type;
	unsigned int	sub_type = 0;
	unsigned int	lbl_address = 0;
	unsigned int	net_address = 0;
	unsigned int	object_id = 0;
	bool			net_element = false;
	bool			multi_segment = false;
	long			t_x0,t_x1,t_y0,t_y1;
	unsigned char	marine_flag = 0;
#ifdef _DEBUG
	int				ttx,tty;
#endif

	//mapRead_segment	segments[_MAX_SEGMENTS];
	int				t_curr_segment = 0;

	//polyline decoding
	unsigned int	stream_len = 0;
	unsigned long	t_address = 0;
	long			x_total,y_total;
	unsigned short	x_len,y_len;
	char			x_type,y_type;
	unsigned int	coord_no;
	unsigned int	coord_no_i;

	//memset(segments,0,sizeof segments);

	object_id = current_TRE<<8 | current_RGN;

	coord_no = 0;
	//Czytanie marine typów
	if( type < 0x10 ) {
		object_id = 0;
		readData(address,11);

		rgn_type = static_cast<unsigned int>(read_buffer[0]);
		sub_type = static_cast<unsigned int>(read_buffer[1]) & 0x1f;
		marine_flag = static_cast<unsigned int>(read_buffer[1]) & 0xe0;

		coords_int[0].we = (start_x + (ConvertB2ToInt16(&read_buffer[2]) << bit_shift));
		coords_int[0].ns = (start_y + (ConvertB2ToInt16(&read_buffer[4]) << bit_shift));
	
		t_len = 6;

		if( read_buffer[6] & 1 ) {
			stream_len = static_cast<unsigned int>(read_buffer[6]);
			stream_len>>=1;
			t_len++;
			t_address = address + 7;
		} else if( (read_buffer[6] & 3) == 2 ) {
			stream_len = static_cast<unsigned int>(read_buffer[6]);
			stream_len>>=2;
			stream_len |= static_cast<unsigned int>( (read_buffer[7])<<6 );
			t_len+=2;
			t_address = address + 8;
		} else
			assert(false);

		t_len += stream_len;
	} else {
		//land
		readData(address,10);

		if( read_buffer[0] & 0x80 ) {
			stream_len = 0;
			memcpy(&stream_len,&read_buffer[8],2); 
			t_len = 11 + stream_len;
			t_address = address + 10;
		} else {
			stream_len = static_cast<unsigned int>(read_buffer[8]);
			t_len = stream_len + 10;
			t_address = address + 9;
		}

		//czytanie danych
		memcpy(&lbl_address,&read_buffer[1],3);
		lbl_address &= 0x3fffff;

		if( type == 0x80 ) {
			rgn_type = static_cast<unsigned int>(read_buffer[0]) & 0x7f; 
		} else {
			if( read_buffer[3] & 0x80 )	{
				net_element = true;	
				net_address = lbl_address;
				net_address = (net_address<< selected_file->net1_modif);
			}
			if( read_buffer[3] & 0x40 )	multi_segment = true;	
			rgn_type = static_cast<unsigned int>(read_buffer[0]) & 0x3f; 
		}
		coords_int[0].we = (start_x + (ConvertB2ToInt16(&read_buffer[4]) << bit_shift));
		coords_int[0].ns = (start_y + (ConvertB2ToInt16(&read_buffer[6]) << bit_shift));
	}

	t_x0 = t_x1 = coords_int[0].we;
	t_y0 = t_y1 = coords_int[0].ns;

	//dla marine o 20 bajtow wiecej
	initialiseReadBits(t_address,stream_len);

	//typ kodowania
	x_len = static_cast<unsigned short>(readBits(4));
	y_len = static_cast<unsigned short>(readBits(4));

	x_len = x_len<=9 ? x_len+2 : (2 * x_len)-7;	
	y_len = y_len<=9 ? y_len+2 : (2 * y_len)-7;

	if( readBits(1) ) {
		if( readBits(1) ) 
			x_type = 1;
		else
			x_type = 2;
	} else {
		++x_len;
		x_type = 0;
	}
	
	if( readBits(1) ) {
		if( readBits(1) )
			y_type = 1;
		else
			y_type = 2;
	} else {
		++y_len;
		y_type = 0;
	}


	x_total = coords_int[0].we;
	y_total = coords_int[0].ns;

	if( type < 0x10 ) {
		if( readBits(1) == 1 ) {

			//to jest wci¹¿ Ÿle...
			++x_len;
			++y_len;
		}
	}

	if( multi_segment ) {
		//segments[t_curr_segment].coordinate_number = 0;
		t_curr_segment++;
	}

	coord_no++;	
	for(;;) {
		if( multi_segment ) {
			if( readBits(1) != 0 ) {

				//segments[t_curr_segment].coordinate_number = (coord_no-1);
				//segment_change = true;
				t_curr_segment++;
			}
		}

#ifdef _DEBUG
		ttx = (readValue(x_len,x_type) << bit_shift);
		tty = (readValue(y_len,y_type) << bit_shift);

		x_total += ttx;
		y_total += tty;
#else
		x_total += (readValue(x_len,x_type) << bit_shift);//we
		y_total += (readValue(y_len,y_type) << bit_shift);//ns
#endif
		if( coords_int[coord_no-1].we != x_total || coords_int[coord_no-1].ns != y_total ) {
			coords_int[coord_no].we = x_total;
			coords_int[coord_no].ns = y_total;

			t_x0 = min(t_x0,x_total);
			t_x1 = max(t_x1,x_total);

			t_y0 = min(t_y0,y_total);
			t_y1 = max(t_y1,y_total);
		
		//coords[coord_no].we = we + static_cast<float>(x_total) * c_fGarminUnit;
		//coords[coord_no].ns = ns + static_cast<float>(y_total) * c_fGarminUnit;

			++coord_no;
		}

		if( (bit_position >> 3) > stream_len ) break;
		if( type < 0x10 && ( (bit_position+x_len+y_len) >> 3) >= stream_len ) break;
		
	}

	if( type < 0x10 && marine_flag ) {
		t_address = address + t_len;
		readData(t_address,20);

		if( marine_flag & 0x20 ) {
			memcpy(&lbl_address,&read_buffer[0],3);
			t_len+=3;//LBL
			lbl_address &= 0x3fffff;
		
			t_address = address + t_len;
			readData(t_address,20);
		}

		if( marine_flag & 0x40 ) {
			t_len+=4;
			assert((read_buffer[0] == 9)); //depth/elevation
			assert(( read_buffer[1] == 3 || read_buffer[1] == 5 || read_buffer[1] == 7 || read_buffer[1] == 0x15 ) ); //depth 3, elevation 5
			
			t_len += (( read_buffer[2] & 0x18 )>>3);
			readData(t_address,20);
		}

		if( marine_flag & 0x80 ) {
			if( rgn_type == 1 && sub_type == 8 ) {
				if( (read_buffer[0] & 0x60) == 0x60 ) {
					t_len += (read_buffer[1] >> 1);
					t_len += 2;
				} else {
					t_len += ( read_buffer[0] & 0x60 ) >> 5;
					t_len += 2;
				}
				//recommendedRoute.read(file);
			} else if( rgn_type == 1 && sub_type >= 5 && sub_type <= 7 ) {
				if( (read_buffer[0] & 0xe0) == 0xc0 )
					t_len += 4;
				else if( (read_buffer[0] & 0xe0) == 0xa0 )
					t_len += 3;
				else if( (read_buffer[0] & 0xe0) == 0x80 )
					t_len += 2;
				else
					t_len++;
				//depth
			} else if( rgn_type == 3 ) {
				if( (read_buffer[0] & 0xe0) == 0xc0 )
					t_len += 4;
				else if( (read_buffer[0] & 0xe0) == 0xa0 )
					t_len += 3;
				else if( (read_buffer[0] & 0xe0) == 0x80 )
					t_len += 2;
				else
					t_len++;
				//depth
			} else if( rgn_type >= 4 && rgn_type <= 6 ) {
				if( read_buffer[0] & 0x80 )
					t_len++;
				t_len++;
				//lineStyle.read(file);
			} else if( rgn_type == 7 ) {
				if( (read_buffer[0] & 0xe0) == 0x80 ) 
					t_len++;
				else if( (read_buffer[0] & 0xe0) == 0xa0 ) 
					t_len+=2;
				t_len++;
				//lbl17text.read(file,m_IMGfile);
			} else if( (rgn_type == 1 && sub_type == 4) || (rgn_type == 1 && sub_type == 9) ) {
				//LBL16 - chart data
				//cout<<"Chart data"<<endl;
				//file->Read(&value,1);//???
				t_len++;
			} else {
				assert(true);
			}
		}
	}

	if( X0 > t_x1 ) 
		return t_len;
	if( X1 < t_x0 ) 
		return t_len;
	if( Y0 > t_y1 ) 
		return t_len;
	if( Y1 < t_y0 ) 
		return t_len;

	for( coord_no_i = 0; coord_no_i <= coord_no; ++coord_no_i ) {
		coords[coord_no_i].we = static_cast<float>(coords_int[coord_no_i].we) * c_fGarminUnit;
		coords[coord_no_i].ns = static_cast<float>(coords_int[coord_no_i].ns) * c_fGarminUnit;
	}

	if( net_element ) {
		//retreive only LBL
		readData( net_address + selected_file->net1_data,3);
		memcpy(&lbl_address,&read_buffer[0],3);
		lbl_address &= 0x3fffff;
	}

	//--coord_no;
	if( dataFunction != NULL ) {
		char flag = net_element ? 0x02 : 0;

		if( !dataFunction(layer,(type < 0x10?(type==2?0x80:(type==3?0x40:0x10)):type),type<0x10?((rgn_type<<8) | sub_type | 0x010000 ):(rgn_type<<8 | sub_type),sub_type, 0, net_address,object_id,readLBL(lbl_address).c_str(),coords,coord_no,flag,lbl_address) )
			interrupt = true;
	}

	return t_len;
}

unsigned long IMG_internal_list::exportPoint(const int layer,const int type,const unsigned long& address,const long& bit_shift,const long& start_x,const long& start_y) {
	unsigned long	t_len = 8;
	unsigned int	rgn_type = 0;
	unsigned int	sub_type = 0;
	unsigned int	lbl_address;
	unsigned int	label_address = 0;
	unsigned int	poi_address = 0;
	unsigned int	net_address = 0;
	bool			poi_info = false;
	bool			nt_poi_info = false;

	long			t_x,t_y;
	unsigned char	marine_flag;
	
	static char		nt_string[512];
	unsigned short	t_i; // local

	//TODO
	//Czytanie marine typów
	if( type < 0x10 ) {
		readData(address,11);

		t_len = 6;
		rgn_type = static_cast<unsigned int>(read_buffer[0]);
		sub_type = static_cast<unsigned int>(read_buffer[1]) & 0x1f;
		marine_flag = static_cast<unsigned int>(read_buffer[1]) & 0xe0;

		t_x = (start_x + (ConvertB2ToInt16(&read_buffer[2]) << bit_shift));
		t_y = (start_y + (ConvertB2ToInt16(&read_buffer[4]) << bit_shift));

		lbl_address = 0;
		if( marine_flag & 0x20 ) {
			t_len+=3;//LBL
			memcpy(&lbl_address,&read_buffer[6],3);
			lbl_address &= 0x3fffff;
			label_address = lbl_address;
		}
		if( marine_flag & 0x80 ) {
			if( rgn_type == 1 ) {
				if( (read_buffer[t_len] & 0xe0) != 0xe0 )
					t_len++;
				else {					
					t_len += (read_buffer[t_len+1]>>1); //1-master flag, 2-extra flag (len)
					t_len +=2;
				}

				//navaid.setType(1);
				//navaid.read(file,m_IMGfile);
			} else if( rgn_type == 2 ) {
				if( (read_buffer[t_len] & 0xe0) != 0xe0 ) 
					t_len++;
				else
					t_len += (2 + (read_buffer[t_len+1]>>1)); //1-master flag, 2-extra flag (len)
				//navaid.setType(2);
				//navaid.read(file,m_IMGfile);
			} else if( rgn_type == 3 ) {
				if( (read_buffer[t_len] & 0xe0) == 0xc0 )
					t_len += 4;
				else if( (read_buffer[t_len] & 0xe0) == 0xa0 )
					t_len += 3;
				else if( (read_buffer[t_len] & 0xe0) == 0x80 )
					t_len += 2;
				else
					t_len++;
				//depth.read(file);
			} else if( rgn_type == 4 ) {
				if( read_buffer[t_len] & 0x80 ) {
					if( (read_buffer[t_len] & 0xe0) == 0x80 ) t_len++; 
					else if( (read_buffer[t_len] & 0xe0) == 0xa0 ) t_len+=2; 
					else if( (read_buffer[t_len] & 0xe0) == 0xc0 ) t_len+=3; 
				}
				t_len++;
				//obstruction.read(file);
			} else if( rgn_type == 5 ) {
				if( read_buffer[t_len] & 0x80 )
					t_len++;
				t_len++;
				//lineStyle.read(file);
			} else if( rgn_type == 8 || (rgn_type == 9 && sub_type == 2) ) {
				if( (read_buffer[t_len] & 0xe0) == 0x80 ) 
					t_len++;
				else if( (read_buffer[t_len] & 0xe0) == 0xa0 ) 
					t_len+=2;
				t_len++;
				//lbl17text.read(file,m_IMGfile);
			} else if( rgn_type == 9 && sub_type == 3 ) {
				if( (read_buffer[t_len] & 0xa0) == 0x80 ) 
					t_len++;
				else if( (read_buffer[t_len] & 0xa0) == 0xa0 ) 
					t_len+=2;
				t_len++;
				//facilities.read(file);
			} else if( rgn_type >= 0x0b && rgn_type <= 0x10 ) {
				nt_poi_info = true;
				assert( read_buffer[t_len] & 0xf0 );
				if( !(read_buffer[t_len] & 1) ) {
					//napis jako LBL
					memcpy(&lbl_address,&read_buffer[t_len+2],3);
					lbl_address &= 0x3fffff;
					label_address = lbl_address;
				}

				t_len++;
				t_len += (read_buffer[t_len]>>1);
				t_len++;
				
				readData(address,t_len);
				t_i = 8;
				while( t_i < t_len ) {
					nt_string[t_i-8] = (char)read_buffer[t_i];
					if( !read_buffer[t_i] )
						break;
					t_i++;
				}

				//TODO poi_address nie przechowa aktualnie pelnego adresu! Mam tam tylko 3 bajty - potrzebuje pelne 4
				poi_address = (address + 6);// - rgn_extended.rgn4_address;
			} else	{
				rgn_type = rgn_type;
				assert(false);
				//file->Read(&value,1);//???
				//if( value & 0x80 )
				//	file->Read(&value,1);//???
			}
		}
		if( marine_flag & 0x40 ) {
			if( rgn_type == 0x0e || rgn_type == 0x0f ) {
				readData(address+t_len,5);
				assert(read_buffer[0] < 10);
				assert(read_buffer[1] & 1);
				t_len+=2;
				t_len += (read_buffer[1]>>1);
				//multi language record
			} else
				assert(false);
		}

	} else {
		readData(address,9);
		if( read_buffer[3] & 0x80 ) 
			t_len = 9;

		rgn_type = static_cast<unsigned int>(read_buffer[0]);
		if( t_len == 9 ) {
			sub_type |= static_cast<unsigned int>(read_buffer[8]);
		}	
		memcpy(&lbl_address,&read_buffer[1],3);
		lbl_address &= 0x3fffff;
		label_address = lbl_address;

		t_x = (start_x + (ConvertB2ToInt16(&read_buffer[4]) << bit_shift));
		t_y = (start_y + (ConvertB2ToInt16(&read_buffer[6]) << bit_shift));
	}

	if( X0 > t_x ) return t_len;
	if( X1 < t_x ) return t_len;
	if( Y0 > t_y ) return t_len;
	if( Y1 < t_y ) return t_len;

	coords[0].we = t_x * c_fGarminUnit;
	coords[0].ns = t_y * c_fGarminUnit;

	if( read_buffer[3] & 0x40 && type >= 0x10 ) {
		net_address = static_cast<unsigned char>(selected_file - internal_files.begin());
		poi_info = true;
		poi_address = lbl_address;

		readData(lbl_address+selected_file->lbl6_data,3);
		memcpy(&lbl_address,&read_buffer[0],3);
		lbl_address &= 0x3fffff;
		label_address = lbl_address;
		//poi_address |= (unsigned int(selected_file_idx)) << (sizeof (unsigned int) - 8);
	}

	char flag = poi_info ? 0x01 : 0;
	if( nt_poi_info ) {
		flag = 4;
		net_address = static_cast<unsigned char>(selected_file - internal_files.begin());
	}

	if( nt_poi_info && !label_address ) {
		if( !dataFunction(layer,(type < 0x10?(type==2?0x80:(type==3?0x40:0x10)):type),type<0x10?((rgn_type<<8) | sub_type | 0x010000 ):(rgn_type<<8 | sub_type),sub_type, poi_address, net_address,0 ,nt_string,coords,1,flag,label_address)  )
			interrupt = true;
	} else {
		if( !dataFunction(layer,(type < 0x10?(type==2?0x80:(type==3?0x40:0x10)):type),type<0x10?((rgn_type<<8) | sub_type | 0x010000 ):(rgn_type<<8 | sub_type),sub_type, poi_address, net_address,0 ,readLBL(lbl_address).c_str(),coords,1,flag,label_address)  )
			interrupt = true;
	}
	return t_len;
}

void IMG_internal_list::readCityRegionCountry_i(const int& lbl4,std::string& city,unsigned long &Lcity,std::string& region,unsigned long &Lregion,std::string& country,unsigned long &Lcountry) {
	unsigned int	lbl_address;
	unsigned short	rgn_idx,tre_idx;
	unsigned int	region_idx;
	unsigned int	country_idx;
	t_coord			coordinates;

	rgn_idx = 0;
	tre_idx = 0;
	region_idx = 0;
	country_idx = 0;

	Lcity = 0;
	Lregion = 0;
	Lcountry = 0;

	if( !lbl4 )
		return;

	if( lbl4 > selected_file->lbl4_data_len / selected_file->lbl4_rec )
		throw runtime_error("lbl4 index out of range");
	readData(selected_file->lbl4_data + (lbl4-1)*selected_file->lbl4_rec,selected_file->lbl4_rec);

	if( read_buffer[4] & 0x80 ) {
		rgn_idx = read_buffer[0];
		memcpy(&tre_idx,&read_buffer[1],2);

		if( !locateRGN1020(tre_idx,rgn_idx,coordinates,city,Lcity) )
			city="";
	} else {
		memcpy(&lbl_address,&read_buffer[0],3);
		lbl_address &= 0x3fffff;
		Lcity = lbl_address;
		city = readLBL(lbl_address);
	}

	readData(selected_file->lbl4_data + (lbl4-1)*selected_file->lbl4_rec,selected_file->lbl4_rec);
	memcpy(&region_idx,&read_buffer[3],2);
	region_idx &= 0x7fff;

	if( region_idx & 0x4000 ) {
		country_idx = region_idx & 0x3fff;
		region_idx = 0;
	}

	if( region_idx ) {
		readData(selected_file->lbl3_data + (region_idx-1)*selected_file->lbl3_rec_len,5);
		memcpy(&country_idx,&read_buffer[0],2);
		memcpy(&lbl_address,&read_buffer[2],3);
		lbl_address &= 0x3fffff;
		Lregion = lbl_address;
		region = readLBL(lbl_address);
	}

	if( country_idx ) {
		readData(selected_file->lbl2_data + (country_idx-1)*selected_file->lbl2_rec_len,3);
		memcpy(&lbl_address,&read_buffer[0],3);
		lbl_address &= 0x3fffff;
		Lcountry = lbl_address;
		country = readLBL(lbl_address);
	}
}

void IMG_internal_list::readCityRegionCountry(const int& lbl4,std::string& city,std::string& region,std::string& country) {
	unsigned long l1,l2,l3;
	if( lbl4 )
		readCityRegionCountry_i(lbl4,city,l1,region,l2,country,l3);
}

void IMG_internal_list::readZip(unsigned int& lbl8,std::string& zip) {
	unsigned int lbl_address =0;
	string		t_ret;
	if( !lbl8 )
		return;
	readData(selected_file->lbl8_data + (lbl8-1)*selected_file->lbl8_rec,3,2);
	memcpy(&lbl_address,&read_buffer2[0],3);
	lbl_address &= 0x3fffff;
	zip = readLBL(lbl_address);
}

void IMG_internal_list::readZip_i(unsigned int& lbl8,std::string& zip,unsigned long& lbl_address) {
	lbl_address =0;
	string		t_ret;
	if( !lbl8 )
		return;
	readData(selected_file->lbl8_data + (lbl8-1)*selected_file->lbl8_rec,3,2);
	memcpy(&lbl_address,&read_buffer2[0],3);
	lbl_address &= 0x3fffff;
	zip = readLBL(lbl_address);
}

void IMG_internal_list::readIndex(_READINDEX indexFunction) {
	/*


	readData(selected_file->lbl4_data + (lbl4-1)*selected_file->lbl4_rec,selected_file->lbl4_rec);
	memcpy(&region_idx,&read_buffer[3],2);
	region_idx &= 0x7fff;

	if( region_idx & 0x4000 ) {
	country_idx = region_idx & 0x3fff;
	region_idx = 0;
	}
	*/
	int				index;
	int				master_idx;
	unsigned long	lbl_address;
	string			text;

	//czytanie krajow z LBL2
	for( index = 0; index*selected_file->lbl2_rec_len < selected_file->lbl2_data_len; ++index ) {
		readData(selected_file->lbl2_data + index*selected_file->lbl2_rec_len,3);
		memcpy(&lbl_address,&read_buffer[0],3);
		lbl_address &= 0x3fffff;		
		text = readLBL(lbl_address);
		indexFunction(index+1,text.c_str(),lbl_address,0,0,NULL,0,0,0,0,NULL,0,0,NULL,0);
	}

	//czytanie regionow z LBL3
	for( index = 0; index*selected_file->lbl3_rec_len < selected_file->lbl3_data_len; ++index ) {
		readData(selected_file->lbl3_data + index*selected_file->lbl3_rec_len,5);

		master_idx = 0;
		memcpy(&master_idx,&read_buffer[0],2);
		memcpy(&lbl_address,&read_buffer[2],3);

		lbl_address &= 0x3fffff;
		text = readLBL(lbl_address);
		indexFunction(0,NULL,0,index+1,master_idx,text.c_str(),lbl_address,0,0,0,NULL,0,0,NULL,0);
	}

	//czytanie miast z LBL4
	int	rgn_idx,tre_idx;
	t_coord	coordinates;
	unsigned short region_idx,country_idx;
	for( index = 0; index*selected_file->lbl4_rec < selected_file->lbl4_data_len; ++index ) {
		readData(selected_file->lbl4_data + index*selected_file->lbl4_rec,selected_file->lbl4_rec);

		rgn_idx = 0;
		tre_idx = 0;
		region_idx = 0;
		country_idx = 0;
		if( read_buffer[4] & 0x80 ) {
			rgn_idx = read_buffer[0];
			memcpy(&tre_idx,&read_buffer[1],2);

			if( !locateRGN1020(tre_idx,rgn_idx,coordinates,text,lbl_address) )
				text="";
		} else {
			memcpy(&lbl_address,&read_buffer[0],3);
			lbl_address &= 0x3fffff;
			text = readLBL(lbl_address);
		}

		readData(selected_file->lbl4_data + index*selected_file->lbl4_rec,selected_file->lbl4_rec);
		memcpy(&region_idx,&read_buffer[3],2);
		region_idx &= 0x7fff;

		if( region_idx & 0x4000 ) {
			country_idx = region_idx & 0x3fff;
			region_idx = 0;
		}


		indexFunction(0,NULL,0,0,0,NULL,0,index+1,region_idx,country_idx,text.c_str(),lbl_address,0,NULL,0);
	}

	//czytanie zip z LBL8
	for( index = 0; index*selected_file->lbl8_rec < selected_file->lbl8_data_len; ++index ) {
		readData(selected_file->lbl8_data + index*selected_file->lbl8_rec,3,2);
		memcpy(&lbl_address,&read_buffer2[0],3);
		lbl_address &= 0x3fffff;
		text = readLBL(lbl_address);
		indexFunction(0,NULL,0,0,NULL,0,0,0,NULL,0,0,0,index+1,text.c_str(),lbl_address);
	}
}

long IMG_internal_list::readNETroads(const int level,const unsigned long& net_address,int table_road[],int table_size) {
	unsigned int	lbl_address1 = 0;
	unsigned int	lbl_address2 = 0;
	unsigned int	lbl_address3 = 0;
	unsigned int	lbl_address4 = 0;
	unsigned int	nod_address  = 0;
	unsigned int	level_road_start;
	unsigned int	level_road_end;
	unsigned char	current_level;
	unsigned int	rgn_tre;
	bool			read_next = true;
	unsigned long	data_start = 3;
	unsigned long	flag;
	unsigned long	length;

	int				one_way,road_class,road_speed;

	one_way = 0;
	road_class = 0;
	road_speed = 0;

	memset(table_road,0,(sizeof (int)) * table_size);
	readData(net_address + selected_file->net1_data,120);
	memcpy(&lbl_address1,&read_buffer[0],3);
	if( lbl_address1 & 0x800000 ) read_next = false;
	lbl_address1 &= 0x3fffff;

	if( read_next ) {
		memcpy(&lbl_address2,&read_buffer[data_start],3);
		if( lbl_address2 & 0x800000 ) read_next = false;
		lbl_address2 &= 0x3fffff;
		data_start+=3;
	}

	if( read_next ) {
		memcpy(&lbl_address3,&read_buffer[data_start],3);
		if( lbl_address3 & 0x800000 ) read_next = false;
		lbl_address3 &= 0x3fffff;
		data_start+=3;
	}

	if( read_next ) {
		memcpy(&lbl_address4,&read_buffer[data_start],3);
		if( lbl_address4 & 0x800000 ) read_next = false;
		lbl_address4 &= 0x3fffff;
		data_start+=3;
	}
	while( read_next ) {
		if( read_buffer[data_start] & 0x80 ) read_next = false;
		data_start+=3;
	}

	//flag
	memset(&road_data,0,sizeof(t_road_data));
	flag = data_start;
	unsigned char t_flag = read_buffer[flag];

	one_way = (t_flag & 0x02)?1:0;

	data_start++; //flag
	length = 0;
	memcpy(&length,&read_buffer[data_start],2);
	data_start+=3; //length

	//skip first value (1 or 2 bytes)
	//TRE-RGN indexes
	current_level = 0;
	road_rgn_tre.clear();
	{
		bool	next_record = true;
		int		tre_rgn_count = 0;

		level_road_start = 0;
		level_road_end = 0;

		do {
			if( current_level == level )
				level_road_start = tre_rgn_count;

			if( !(t_flag & 1) ) {
				if(read_buffer[data_start] & 0x80) {
					next_record = false;
				}

				tre_rgn_count += (read_buffer[data_start] & 0x7f);
				data_start++;
			} else {
				unsigned short t_value = 0;
				memcpy(&t_value,&read_buffer[data_start],2);
				if(t_value & 0x8000) {
					next_record = false;
				}
				tre_rgn_count += ( t_value & 0x7fff);
				data_start+=2;
			}
			if( current_level == level )
				level_road_end = tre_rgn_count;

			current_level++;
		} while(next_record);

		//ominac indeksy 
		data_start+=3*level_road_start;

		//data_start+= 3*level0_road_start;
		readData(net_address + selected_file->net1_data,data_start+(level_road_end*3)+250);
		for( ; level_road_start < level_road_end; ++level_road_start, data_start+=3 ) {
			rgn_tre = 0;
			memcpy(&rgn_tre,&read_buffer[data_start],3);
			road_rgn_tre.push_back(rgn_tre);
		}
	}

	int t_counter = 0;
	for( vector<int>::iterator t_road = road_rgn_tre.begin(); t_road != road_rgn_tre.end(); t_road++ ) {
		if( t_counter < table_size )
			table_road[t_counter] = *t_road;
		t_counter++;
	}
	return (long)road_rgn_tre.size();
}

void IMG_internal_list::readNET(const unsigned long& net_address) {
	unsigned int	lbl_address1 = 0;
	unsigned int	lbl_address2 = 0;
	unsigned int	lbl_address3 = 0;
	unsigned int	lbl_address4 = 0;
	unsigned int	nod_address  = 0;
	unsigned int	level0_road_start;
	unsigned int	level0_road_end;
	unsigned int	rgn_tre;
	bool			read_next = true;
	unsigned long	data_start = 3;
	unsigned long	flag;
	unsigned long	length;

	int				one_way,road_class,road_speed;

	one_way = 0;
	road_class = 0;
	road_speed = 0;

	int					segments_size = 0;
	mapRead_address		segment_definition[_MAX_SEGMENTS];

	int					records = 0;

	memset(segment_definition,0,sizeof segment_definition);
	memset(address,0,sizeof address);
	memset(address_i,0,sizeof address_i);

	readData(net_address + selected_file->net1_data,120);
	memcpy(&lbl_address1,&read_buffer[0],3);
	if( lbl_address1 & 0x800000 ) read_next = false;
	lbl_address1 &= 0x3fffff;

	if( read_next ) {
		memcpy(&lbl_address2,&read_buffer[data_start],3);
		if( lbl_address2 & 0x800000 ) read_next = false;
		lbl_address2 &= 0x3fffff;
		data_start+=3;
	}

	if( read_next ) {
		memcpy(&lbl_address3,&read_buffer[data_start],3);
		if( lbl_address3 & 0x800000 ) read_next = false;
		lbl_address3 &= 0x3fffff;
		data_start+=3;
	}

	if( read_next ) {
		memcpy(&lbl_address4,&read_buffer[data_start],3);
		if( lbl_address4 & 0x800000 ) read_next = false;
		lbl_address4 &= 0x3fffff;
		data_start+=3;
	}
	while( read_next ) {
		if( read_buffer[data_start] & 0x80 ) read_next = false;
		data_start+=3;
	}

	//flag
	memset(&road_data,0,sizeof(t_road_data) );
	flag = data_start;
	unsigned char t_flag = read_buffer[flag];

	one_way = (t_flag & 0x02)?1:0;
	//tunnel  = (t_flag & 0x80)?1:0;

	data_start++; //flag
	length = 0;
	memcpy(&length,&read_buffer[data_start],2);
	data_start+=3; //length

	//skip first value (1 or 2 bytes)
	//TRE-RGN indexes
	road_rgn_tre.clear();
	{
		bool	next_record = true;
		int		tre_rgn_count = 0;

		level0_road_start = 0;
		level0_road_end = 0;

		do {
			if( !(t_flag & 1) ) {
				if(read_buffer[data_start] & 0x80) {
					next_record = false;
				}

				tre_rgn_count += (read_buffer[data_start] & 0x7f);
				data_start++;
				if( !level0_road_end )
					level0_road_end = tre_rgn_count;
			} else {
				unsigned short t_value = 0;
				memcpy(&t_value,&read_buffer[data_start],2);
				if(t_value & 0x8000) {
					next_record = false;
				}
				tre_rgn_count += ( t_value & 0x7fff);
				data_start+=2;
				if( !level0_road_end )
					level0_road_end = tre_rgn_count;
			}
		} while(next_record);

		//ominac indeksy POMIJAJAC 0 level
		//data_start+=3*tre_rgn_count;

		//data_start+= 3*level0_road_start;
		readData(net_address + selected_file->net1_data,data_start+(level0_road_end*3)+250);
		for( ; level0_road_start < level0_road_end; ++level0_road_start, data_start+=3 ) {
			rgn_tre = 0;
			memcpy(&rgn_tre,&read_buffer[data_start],3);
			road_rgn_tre.push_back(rgn_tre);
		}
		//1 - odczytanie wszystkich RGN40 nalezacych do NET -> wpisanie do tablicy segment_definition informacji
		//	-> index danego RGN (jako rgn_tre oraz numer porz¹dkowy)
		//  -> index punktu RGN dla ka¿dego segmentu
		
		//pominiêcie pozosta³ych warstw
		data_start+= 3*(tre_rgn_count-level0_road_end);

		//odczytanie wszystkich dróg z warstwy 0 i dodanie do 'segment_definition'
		for( vector<int>::iterator t_road = road_rgn_tre.begin(); t_road != road_rgn_tre.end(); t_road++ ) {
			short tre_idx, rgn_idx;
			unsigned char layer;
			unsigned long rgn_address;
			long bit_shift,start_x,start_y;
			unsigned int t_address_size;

			tre_idx = ((*t_road) & 0xffff00)>>8;
			rgn_idx = ((*t_road) & 0xff);

			if( locateRGN40(tre_idx,rgn_idx,layer,rgn_address,bit_shift,start_x,start_y) ) {
				const int points = exportPolylineSegments(layer,0x40,rgn_address,bit_shift,start_x,start_y,address,t_address_size);
				//skopiowaæ do docelowego
				for( int i = 0; i < t_address_size; segments_size++,i++ ) {
					if( address[i].segment != points ) { // ostatnie nalezy do nastepnej ulicy
						segment_definition[segments_size].segment = address[i].segment;
						segment_definition[segments_size].segment_coord = address[i].segment_coord;
						segment_definition[segments_size].road_idx = (*t_road);

						address_i[segments_size].segment  = address[i].segment;
						address_i[segments_size].road_idx = (*t_road);
					}
				}
			} else
				throw runtime_error("error in segment read");
		}

		//
		if( t_flag & 0x10 ) {
			unsigned short segments;
			readData(net_address + selected_file->net1_data,data_start+250);
			memcpy(&segments,&read_buffer[data_start],2);
			//memset(address,0,sizeof address);
			//memset(address_i,0,sizeof address_i);

			//numeracja i miasta!

			unsigned short	lbl8_flag = (segments & 0x0C00) >> 10;
			unsigned short	lbl4_flag = (segments & 0x3000) >> 12;
			unsigned short	numb_flag = (segments & 0xC000) >> 14;
			segments &= 0x3ff;
			records = segments;
			if( segments != segments_size ) {
				segments = segments;
			}
			data_start+=2;

			if( netFunction_i ) {
				readZipInfo(data_start,lbl8_flag,address_i);
				readCityInfo(net_address + selected_file->net1_data,data_start,lbl4_flag,address_i);	
			} else {
				readZipInfo(data_start,lbl8_flag,address);
				readCityInfo(net_address + selected_file->net1_data,data_start,lbl4_flag,address);	
			}
			readNumbers(net_address + selected_file->net1_data,data_start,numb_flag,t_flag,address,records);
			records++;

			//przeniesienie danych do segment_definition
			for( int i=0; i <= segments_size; ++i ) {
				memcpy(segment_definition[i].city_left,address[i].city_left,sizeof segment_definition[i].city_left);
				memcpy(segment_definition[i].city_right,address[i].city_right,sizeof segment_definition[i].city_right);
				memcpy(segment_definition[i].country_left,address[i].country_left,sizeof segment_definition[i].country_left);
				memcpy(segment_definition[i].country_right,address[i].country_right,sizeof segment_definition[i].country_right);
				memcpy(segment_definition[i].region_left,address[i].region_left,sizeof segment_definition[i].region_left);
				memcpy(segment_definition[i].region_right,address[i].region_right,sizeof segment_definition[i].region_right);

				segment_definition[i].left_type = address[i].left_type;
				segment_definition[i].left_start = address[i].left_start;
				segment_definition[i].left_end = address[i].left_end;

				segment_definition[i].right_type = address[i].right_type;
				segment_definition[i].right_start = address[i].right_start;
				segment_definition[i].right_end = address[i].right_end;

				memcpy(segment_definition[i].zip_left,address[i].zip_left,sizeof segment_definition[i].zip_left);
				memcpy(segment_definition[i].zip_right,address[i].zip_right,sizeof segment_definition[i].zip_right);
			}
/*
			for( int i=0; i <= segments_size; ++i ) {
				address[i].segment = segment_definition[i].coordinate_number;
			}*/
		}
		if( t_flag & 0x40 ) {
			unsigned char		t_nod_city_flag;
			
			readData(net_address + selected_file->net1_data,data_start+5);
			t_nod_city_flag = read_buffer[data_start];
			
			if( t_nod_city_flag & 3 ) {
				data_start++;
				memcpy(&nod_address,&read_buffer[data_start],(read_buffer[data_start-1] & 3)+1);
				data_start += (read_buffer[data_start-1] & 3)+1;

				//czytanie NOD2 tutaj
				readData(nod_address + selected_file->nod2_data,5);
				//flaga:

				struct _nod2Flag {
					unsigned char nod1:1;
					unsigned char speed:3;
					unsigned char roadClass:3;
					unsigned char extra:1;
				};
								
				_nod2Flag nodFlag;
				memcpy(&nodFlag,&read_buffer[0],1);
				road_class = nodFlag.roadClass;//read_buffer[1] >> 5;
				road_speed = nodFlag.speed;
			}

			//City - dodatkowe atrybuty
			readData(net_address + selected_file->net1_data,data_start+10);
			if( t_nod_city_flag & 0xfc ) {
				unsigned char	t_city;
				unsigned short	t_len;
				unsigned int	t_idx;
				string			city,region,country;	


				char t_counter = (t_nod_city_flag >> 5) & 3;
				char t_city_index = 0;

				while( t_counter-- ) {
					t_city_index++;					
					readData(net_address + selected_file->net1_data,data_start+10);

					t_city = read_buffer[data_start];
					data_start++;
					if( (t_city & 0x0c) != 0x0c && !(t_city & 0x04) ) {
						//jedna wartoœæ
						t_idx = t_city>>3;
						if( (t_city & 3) == 1 )	{
							t_idx |= (read_buffer[data_start])<<5;
							data_start++;
						}
						if( (t_city & 3) == 2 )	{
							t_idx |= ((read_buffer[data_start])<<5) | ((read_buffer[data_start+1])<<(5+8));
							data_start+=2;
						}
						if( (t_city & 3) == 3 )	{
							t_idx |= ((read_buffer[data_start])<<5) | ((read_buffer[data_start+1])<<(5+8)) | ((read_buffer[data_start+2])<<(5+16));
							data_start+=3;
						}
						t_idx++;

						if( t_city_index == 1 ) {
							readCityRegionCountry(t_idx,city,region,country);							

							strncpy(segment_definition[0].city_left_2,city.c_str(),sizeof segment_definition[0].city_left_2);
							strncpy(segment_definition[0].region_left_2,region.c_str(),sizeof segment_definition[0].region_left_2);
							strncpy(segment_definition[0].country_left_2,country.c_str(),sizeof segment_definition[0].country_left_2);

							address_i[0].city_left_2 = t_idx;
							address_i[0].city_right_2 = t_idx;

							memcpy(segment_definition[0].city_right_2,segment_definition[0].city_left_2,sizeof segment_definition[0].city_left_2);
							memcpy(segment_definition[0].region_right_2,segment_definition[0].region_left_2,sizeof segment_definition[0].region_left_2);
							memcpy(segment_definition[0].country_right_2,segment_definition[0].country_left_2,sizeof segment_definition[0].country_left_2);

							for( int i=1; i <= segments_size; ++i ) {
								memcpy(segment_definition[i].city_left_2,segment_definition[0].city_left_2,sizeof segment_definition[i].city_left);
								memcpy(segment_definition[i].city_right_2,segment_definition[0].city_right_2,sizeof segment_definition[i].city_right);
								memcpy(segment_definition[i].country_left_2,segment_definition[0].country_left_2,sizeof segment_definition[i].country_left);
								memcpy(segment_definition[i].country_right_2,segment_definition[0].country_right_2,sizeof segment_definition[i].country_right);
								memcpy(segment_definition[i].region_left_2,segment_definition[0].region_left_2,sizeof segment_definition[i].region_left);
								memcpy(segment_definition[i].region_right_2,segment_definition[0].region_right_2,sizeof segment_definition[i].region_right);

								address_i[i].city_left_2 = address_i[0].city_left_2;
								address_i[i].city_right_2 = address_i[0].city_right_2;
							}

						}
						if( t_city_index == 2 ) {
							readCityRegionCountry(t_idx,city,region,country);							

							strncpy(segment_definition[0].city_left_3,city.c_str(),sizeof segment_definition[0].city_left_3);
							strncpy(segment_definition[0].region_left_3,region.c_str(),sizeof segment_definition[0].region_left_3);
							strncpy(segment_definition[0].country_left_3,country.c_str(),sizeof segment_definition[0].country_left_3);

							address_i[0].city_left_3 = t_idx;
							address_i[0].city_right_3 = t_idx;

							memcpy(segment_definition[0].city_right_3,segment_definition[0].city_left_3,sizeof segment_definition[0].city_left_3);
							memcpy(segment_definition[0].region_right_3,segment_definition[0].region_left_3,sizeof segment_definition[0].region_left_3);
							memcpy(segment_definition[0].country_right_3,segment_definition[0].country_left_3,sizeof segment_definition[0].country_left_3);

							for( int i=1; i <= segments_size; ++i ) {
								memcpy(segment_definition[i].city_left_3,segment_definition[0].city_left_3,sizeof segment_definition[i].city_left);
								memcpy(segment_definition[i].city_right_3,segment_definition[0].city_right_3,sizeof segment_definition[i].city_right);
								memcpy(segment_definition[i].country_left_3,segment_definition[0].country_left_3,sizeof segment_definition[i].country_left);
								memcpy(segment_definition[i].country_right_3,segment_definition[0].country_right_3,sizeof segment_definition[i].country_right);
								memcpy(segment_definition[i].region_left_3,segment_definition[0].region_left_3,sizeof segment_definition[i].region_left);
								memcpy(segment_definition[i].region_right_3,segment_definition[0].region_right_3,sizeof segment_definition[i].region_right);

								address_i[i].city_left_3 = address_i[0].city_left_3;
								address_i[i].city_right_3 = address_i[0].city_right_3;
							}
						}
					} else {
						//ci¹g
						if( t_city & 0x08 ) {
							t_len = (t_city>>5) | (read_buffer[data_start]<<3);
							data_start++;
						} else {
							t_len = 1 + (t_city>>4);
						}
						
						//czytanie						
						memset(address,0,sizeof address);
						memset(address_i_tmp,0,sizeof address_i_tmp);

						if( netFunction_i ) {							
							readCityInfo(net_address + selected_file->net1_data,data_start,0,address_i_tmp,t_len);
							for( int i=0; i <= segments_size; ++i ) {
								if( t_city_index == 1 ) {
									address_i[i].city_left_2 = address_i_tmp[i].city_left;
									address_i[i].city_right_2 = address_i_tmp[i].city_right;
								} else if( t_city_index == 1 ) {
									address_i[i].city_left_3 = address_i_tmp[i].city_left;
									address_i[i].city_right_3 = address_i_tmp[i].city_right;
								}
							}
						} else {
							readCityInfo(net_address + selected_file->net1_data,data_start,0,address,t_len);	
							for( int i=0; i <= segments_size; ++i ) {
								if( t_city_index == 1 ) {
									memcpy(segment_definition[i].city_left_2,address[i].city_left_2,sizeof segment_definition[i].city_left_2);
									memcpy(segment_definition[i].city_right_2,address[i].city_right_2,sizeof segment_definition[i].city_right_2);
									memcpy(segment_definition[i].country_left_2,address[i].country_left_2,sizeof segment_definition[i].country_left_2);
									memcpy(segment_definition[i].country_right_2,address[i].country_right_2,sizeof segment_definition[i].country_right_2);
									memcpy(segment_definition[i].region_left_2,address[i].region_left_2,sizeof segment_definition[i].region_left_2);
									memcpy(segment_definition[i].region_right_2,address[i].region_right_2,sizeof segment_definition[i].region_right_2);
								} else if( t_city_index == 1 ) {
									memcpy(segment_definition[i].city_left_3,address[i].city_left_3,sizeof segment_definition[i].city_left_3);
									memcpy(segment_definition[i].city_right_3,address[i].city_right_3,sizeof segment_definition[i].city_right_3);
									memcpy(segment_definition[i].country_left_3,address[i].country_left_3,sizeof segment_definition[i].country_left_3);
									memcpy(segment_definition[i].country_right_3,address[i].country_right_3,sizeof segment_definition[i].country_right_3);
									memcpy(segment_definition[i].region_left_3,address[i].region_left_3,sizeof segment_definition[i].region_left_3);
									memcpy(segment_definition[i].region_right_3,address[i].region_right_3,sizeof segment_definition[i].region_right_3);
								}
							}
						}
						//data_start+=t_len;
					}
				}
			}
		}
	}

	if( netFunction_i ) {
		if( !netFunction_i(readLBL(lbl_address1).c_str(),lbl_address1,readLBL(lbl_address2).c_str(),lbl_address2,readLBL(lbl_address3).c_str(),lbl_address3,readLBL(lbl_address4).c_str(),lbl_address4,one_way,road_class,road_speed,segments_size,address_i)  )
			interrupt = true;
	} else {
		if( !netFunction(readLBL(lbl_address1).c_str(),readLBL(lbl_address2).c_str(),readLBL(lbl_address3).c_str(),readLBL(lbl_address4).c_str(),one_way,road_class,road_speed,segments_size,segment_definition)  )
			interrupt = true;
	}
}

void IMG_internal_list::readCityInfo(const unsigned long& net_address,unsigned long &data_start,unsigned short flag,mapRead_address address[_MAX_SEGMENTS],unsigned short t_data_len) {
	unsigned short	data_len = t_data_len;
	unsigned int	lbl4;
	//	unsigned long	lbl_address;
	string			city,region,country;	
	unsigned char	t_flag;
	unsigned short	t_segment;
	unsigned short	t_record = 0;

	if( data_len == 0 ) {
		//jesli nie jest przekazana d³ugoœæ danych - trzeba j¹ odczytaæ
		if( flag == 0 ) {
			memcpy(&data_len,&read_buffer[data_start],1);
			data_start++;
		}
		if( flag == 1 ) {
			memcpy(&data_len,&read_buffer[data_start],2);
			data_start+=2;
		}
		if( flag == 2 ) {
			lbl4=0;
			memcpy(&lbl4,&read_buffer[data_start],selected_file->lbl4_rec_len);
			data_start += selected_file->lbl4_rec_len;

			address[t_record].segment = 0;
			readCityRegionCountry(lbl4,city,region,country);
			//readZip(lbl8,zip);

			strncpy(address[t_record].city_left,city.c_str(),sizeof address[t_record].city_left);
			strncpy(address[t_record].region_left,region.c_str(),sizeof address[t_record].region_left);
			strncpy(address[t_record].country_left,country.c_str(),sizeof address[t_record].country_left);

			strncpy(address[t_record].city_right,city.c_str(),sizeof address[t_record].city_right);
			strncpy(address[t_record].region_right,region.c_str(),sizeof address[t_record].region_right);
			strncpy(address[t_record].country_right,country.c_str(),sizeof address[t_record].country_right);

			for(int i=1; i<_MAX_SEGMENTS; ++i) {
				strncpy(address[i].city_left,address[0].city_left,sizeof address[i].city_left);
				strncpy(address[i].region_left,address[0].region_left,sizeof address[i].region_left);
				strncpy(address[i].country_left,address[0].country_left,sizeof address[i].country_left);

				strncpy(address[i].city_right,address[0].city_right,sizeof address[i].city_right);
				strncpy(address[i].region_right,address[0].region_right,sizeof address[i].region_right);
				strncpy(address[i].country_right,address[0].country_right,sizeof address[i].country_right);

				//memcpy(&address[i],&address[0],sizeof address[i]);
			}

			//readCityRegionCountry destroys buffer!
			readData(net_address,data_start+250);
			return;
		}
	}

	while( data_len > 0 ) {

		if( t_record > 0 ) {
			memcpy(&address[t_record].city_left,&address[t_record-1].city_left,sizeof address[t_record].city_left);
			memcpy(&address[t_record].region_left,&address[t_record-1].region_left,sizeof address[t_record].region_left);
			memcpy(&address[t_record].country_left,&address[t_record-1].country_left,sizeof address[t_record].country_left);

			memcpy(&address[t_record].city_right,&address[t_record-1].city_right,sizeof address[t_record].city_right);
			memcpy(&address[t_record].region_right,&address[t_record-1].region_right,sizeof address[t_record].region_right);
			memcpy(&address[t_record].country_right,&address[t_record-1].country_right,sizeof address[t_record].country_right);
		}

		memcpy(&t_flag,&read_buffer[data_start],1);
		data_start++;data_len--;
		t_segment = (t_flag & 0x1f)+1;
		t_flag >>= 5;
		if(t_flag == 7) {

			t_segment |= ((read_buffer[data_start] & 0x1F) << 5);
			t_flag = read_buffer[data_start];
			data_start++;data_len--;
			t_flag >>= 5;
		}

		address[t_record].segment = t_segment;
		lbl4 = 0;

		switch(t_flag) {
			case 0://1=2
				memcpy(&lbl4,&read_buffer[data_start],selected_file->lbl4_rec_len);
				readCityRegionCountry(lbl4,city,region,country);

				strncpy(address[t_record].city_left,city.c_str(),80);
				strncpy(address[t_record].region_left,region.c_str(),80);
				strncpy(address[t_record].country_left,country.c_str(),80);

				strncpy(address[t_record].city_right,city.c_str(),80);
				strncpy(address[t_record].region_right,region.c_str(),80);
				strncpy(address[t_record].country_right,country.c_str(),80);

				data_start += selected_file->lbl4_rec_len;data_len-=selected_file->lbl4_rec_len;
				break;
			case 1://1
				memcpy(&lbl4,&read_buffer[data_start],selected_file->lbl4_rec_len);
				readCityRegionCountry(lbl4,city,region,country);

				strncpy(address[t_record].city_left,city.c_str(),80);
				strncpy(address[t_record].region_left,region.c_str(),80);
				strncpy(address[t_record].country_left,country.c_str(),80);

				data_start += selected_file->lbl4_rec_len;data_len-=selected_file->lbl4_rec_len;
				break;
			case 2://2
				memcpy(&lbl4,&read_buffer[data_start],selected_file->lbl4_rec_len);
				readCityRegionCountry(lbl4,city,region,country);

				strncpy(address[t_record].city_right,city.c_str(),80);
				strncpy(address[t_record].region_right,region.c_str(),80);
				strncpy(address[t_record].country_right,country.c_str(),80);

				data_start += selected_file->lbl4_rec_len;data_len-=selected_file->lbl4_rec_len;
				break;
			case 3://1,2
				memcpy(&lbl4,&read_buffer[data_start],selected_file->lbl4_rec_len);
				readCityRegionCountry(lbl4,city,region,country);

				strncpy(address[t_record].city_left,city.c_str(),80);
				strncpy(address[t_record].region_left,region.c_str(),80);
				strncpy(address[t_record].country_left,country.c_str(),80);
				
				readData(net_address,data_start+250);
				memcpy(&lbl4,&read_buffer[data_start]+selected_file->lbl4_rec_len,selected_file->lbl4_rec_len);
				readCityRegionCountry(lbl4,city,region,country);

				strncpy(address[t_record].city_right,city.c_str(),80);
				strncpy(address[t_record].region_right,region.c_str(),80);
				strncpy(address[t_record].country_right,country.c_str(),80);

				data_start += (selected_file->lbl4_rec_len*2);data_len-=(selected_file->lbl4_rec_len*2);
				break;
			case 4://	1=2 = -1
				address[t_record].city_left[0] = 0;
				address[t_record].region_left[0] = 0;
				address[t_record].country_left[0] = 0;

				address[t_record].city_right[0] = 0;
				address[t_record].region_right[0] = 0;
				address[t_record].country_right[0] = 0;
				break;
			case 5://1 -1
				address[t_record].city_left[0] = 0;
				address[t_record].region_left[0] = 0;
				address[t_record].country_left[0] = 0;
				break;
			case 6://2 -1
				address[t_record].city_right[0] = 0;
				address[t_record].region_right[0] = 0;
				address[t_record].country_right[0] = 0;
				break;
		}
		readData(net_address,data_start+250);
		t_record++;
	}
}

//internal
void IMG_internal_list::readCityInfo(const unsigned long& net_address,unsigned long &data_start,unsigned short flag,mapRead_address_i address[_MAX_SEGMENTS],unsigned short t_data_len) {
	unsigned short	data_len = t_data_len;
	unsigned int	lbl4;
	//	unsigned long	lbl_address;
	unsigned char	t_flag;
	unsigned short	t_segment;
	unsigned short	t_record = 0;

	if( data_len == 0 ) {
		if( flag == 0 ) {
			memcpy(&data_len,&read_buffer[data_start],1);
			data_start++;
		}
		if( flag == 1 ) {
			memcpy(&data_len,&read_buffer[data_start],2);
			data_start+=2;
		}
		if( flag == 2 ) {
			lbl4=0;
			memcpy(&lbl4,&read_buffer[data_start],selected_file->lbl4_rec_len);
			data_start += selected_file->lbl4_rec_len;

			address[t_record].segment = 0;
			address[t_record].city_left = lbl4;
			address[t_record].city_right = lbl4;

			for(int i=1; i<_MAX_SEGMENTS; ++i) {
				memcpy(&address[i].city_left,&address[0].city_left,sizeof address[i].city_left);
				memcpy(&address[i].city_right,&address[0].city_right,sizeof address[i].city_right);
			}

			//readCityRegionCountry destroys buffer!
			readData(net_address,data_start+250);

			return;
		}
	}

	while( data_len > 0 ) {

		//blad - do poprawienia! zle zczytywane dane?
		//if( data_len > 0xa000 )
		//	break;

		if( t_record > 0 ) {
			memcpy(&address[t_record].city_left,&address[t_record-1].city_left,sizeof address[t_record].city_left);
			memcpy(&address[t_record].city_right,&address[t_record-1].city_right,sizeof address[t_record].city_right);
		}

		memcpy(&t_flag,&read_buffer[data_start],1);
		data_start++;data_len--;
		t_segment = (t_flag & 0x1f)+1;
		t_flag >>= 5;
		if(t_flag == 7) {

			t_segment |= ((read_buffer[data_start] & 0x1F) << 5);
			t_flag = read_buffer[data_start];
			data_start++;data_len--;
			t_flag >>= 5;
		}

		address[t_record].segment = t_segment;
		lbl4 = 0;

		switch(t_flag) {
			case 0://1=2
				memcpy(&lbl4,&read_buffer[data_start],selected_file->lbl4_rec_len);
				address[t_record].city_left = lbl4;
				address[t_record].city_right = lbl4;

				data_start += selected_file->lbl4_rec_len;data_len-=selected_file->lbl4_rec_len;
				break;
			case 1://1
				memcpy(&lbl4,&read_buffer[data_start],selected_file->lbl4_rec_len);
				address[t_record].city_left = lbl4;

				data_start += selected_file->lbl4_rec_len;data_len-=selected_file->lbl4_rec_len;
				break;
			case 2://2
				memcpy(&lbl4,&read_buffer[data_start],selected_file->lbl4_rec_len);
				address[t_record].city_right = lbl4;

				data_start += selected_file->lbl4_rec_len;data_len-=selected_file->lbl4_rec_len;
				break;
			case 3://1,2
				memcpy(&lbl4,&read_buffer[data_start],selected_file->lbl4_rec_len);
				address[t_record].city_left = lbl4;

				memcpy(&lbl4,&read_buffer[data_start]+selected_file->lbl4_rec_len,selected_file->lbl4_rec_len);
				address[t_record].city_right = lbl4;

				data_start += (selected_file->lbl4_rec_len*2);data_len-=(selected_file->lbl4_rec_len*2);
				break;
			case 4://	1=2 = -1
				address[t_record].city_left = 0;
				address[t_record].city_right = 0;
				break;
			case 5://1 -1
				address[t_record].city_left = 0;
				break;
			case 6://2 -1
				address[t_record].city_right = 0;
				break;
		}
		readData(net_address,data_start+250);
		t_record++;
	}
}

void IMG_internal_list::readZipInfo(unsigned long &data_start,unsigned short flag,mapRead_address address[_MAX_SEGMENTS]) {
	unsigned short	data_len = 0;
	unsigned int	lbl8;
	//	unsigned long	lbl_address;
	string			zip;	
	unsigned char	t_flag;
	unsigned short	t_segment;
	unsigned short	t_record = 0;

	if( flag == 0 ) {
		memcpy(&data_len,&read_buffer[data_start],1);
		data_start++;
	}
	if( flag == 1 ) {
		memcpy(&data_len,&read_buffer[data_start],2);
		data_start+=2;
	}
	if( flag == 2 ) {
		lbl8=0;
		memcpy(&lbl8,&read_buffer[data_start],selected_file->lbl8_rec_len);
		data_start += selected_file->lbl8_rec_len;

		address[t_record].segment = 0;
		readZip(lbl8,zip);

		strncpy(address[t_record].zip_left,zip.c_str(),80);
		strncpy(address[t_record].zip_right,zip.c_str(),80);
		return;
	}

	while( data_len > 0 ) {

		if( t_record > 0 ) {
			memcpy(&address[t_record].zip_left,&address[t_record-1].zip_left,sizeof address[t_record].zip_left);
			memcpy(&address[t_record].zip_right,&address[t_record-1].zip_right,sizeof address[t_record].zip_right);
		}
		memcpy(&t_flag,&read_buffer[data_start],1);
		data_start++;data_len--;
		t_segment = (t_flag & 0x1f)+1;
		t_flag >>= 5;
		if(t_flag == 7) {

			t_segment |= ((read_buffer[data_start] & 0x1F) << 5);
			t_flag = read_buffer[data_start];
			data_start++;data_len--;
			t_flag >>= 5;
		}

		address[t_record].segment = t_segment;
		lbl8 = 0;

		switch(t_flag) {
			case 0://1=2
				memcpy(&lbl8,&read_buffer[data_start],selected_file->lbl8_rec_len);
				readZip(lbl8,zip);

				strncpy(address[t_record].zip_left,zip.c_str(),80);
				strncpy(address[t_record].zip_right,zip.c_str(),80);

				data_start += selected_file->lbl8_rec_len;data_len-=selected_file->lbl8_rec_len;
				break;
			case 1://1
				memcpy(&lbl8,&read_buffer[data_start],selected_file->lbl8_rec_len);
				readZip(lbl8,zip);

				strncpy(address[t_record].zip_left,zip.c_str(),80);

				data_start += selected_file->lbl8_rec_len;data_len-=selected_file->lbl8_rec_len;
				break;
			case 2://2
				memcpy(&lbl8,&read_buffer[data_start],selected_file->lbl8_rec_len);
				readZip(lbl8,zip);

				strncpy(address[t_record].zip_right,zip.c_str(),80);
				data_start += selected_file->lbl8_rec_len;data_len-=selected_file->lbl8_rec_len;
				break;
			case 3://1,2
				memcpy(&lbl8,&read_buffer[data_start],selected_file->lbl8_rec_len);
				readZip(lbl8,zip);
				strncpy(address[t_record].zip_left,zip.c_str(),80);

				memcpy(&lbl8,&read_buffer[data_start+selected_file->lbl8_rec_len],selected_file->lbl8_rec_len);
				readZip(lbl8,zip);
				strncpy(address[t_record].zip_right,zip.c_str(),80);

				data_start += (selected_file->lbl8_rec_len*2);data_len-=(selected_file->lbl8_rec_len*2);
				break;
			case 4://	1=2 = -1
				address[t_record].zip_left[0] = 0;
				address[t_record].zip_right[0] = 0;
				break;
			case 5://1 -1
				address[t_record].zip_left[0] = 0;
				break;
			case 6://2 -1
				address[t_record].zip_right[0] = 0;
				break;
		}

		t_record++;
	}
}

//internal
void IMG_internal_list::readZipInfo(unsigned long &data_start,unsigned short flag,mapRead_address_i address[_MAX_SEGMENTS]) {
	unsigned short	data_len = 0;
	unsigned int	lbl8;
	//	unsigned long	lbl_address;
	string			zip;	
	unsigned char	t_flag;
	unsigned short	t_segment;
	unsigned short	t_record = 0;

	if( flag == 0 ) {
		memcpy(&data_len,&read_buffer[data_start],1);
		data_start++;
	}
	if( flag == 1 ) {
		memcpy(&data_len,&read_buffer[data_start],2);
		data_start+=2;
	}
	if( flag == 2 ) {
		lbl8=0;
		memcpy(&lbl8,&read_buffer[data_start],selected_file->lbl8_rec_len);
		data_start+=selected_file->lbl8_rec_len;

		address[t_record].zip_left = lbl8;
		address[t_record].zip_right = lbl8;
		return;
	}

	while( data_len > 0 ) {

		if( t_record > 0 ) {
			address[t_record].zip_left = address[t_record-1].zip_left;
			address[t_record].zip_right = address[t_record-1].zip_right;
			//memcpy(&address[t_record],&address[t_record-1],sizeof mapRead_address);
		}

		memcpy(&t_flag,&read_buffer[data_start],1);
		data_start++;data_len--;
		t_segment = (t_flag & 0x1f)+1;
		t_flag >>= 5;
		if(t_flag == 7) {

			t_segment |= ((read_buffer[data_start] & 0x1F) << 5);
			t_flag = read_buffer[data_start];
			data_start++;data_len--;
			t_flag >>= 5;
		}

		address[t_record].segment = t_segment;
		lbl8 = 0;

		switch(t_flag) {
			case 0://1=2
				memcpy(&lbl8,&read_buffer[data_start],selected_file->lbl8_rec_len);
				address[t_record].zip_left = lbl8;
				address[t_record].zip_right = lbl8;

				data_start += selected_file->lbl8_rec_len;data_len-=selected_file->lbl8_rec_len;
				break;
			case 1://1
				memcpy(&lbl8,&read_buffer[data_start],selected_file->lbl8_rec_len);
				address[t_record].zip_left = lbl8;

				data_start += selected_file->lbl8_rec_len;data_len-=selected_file->lbl8_rec_len;
				break;
			case 2://2
				memcpy(&lbl8,&read_buffer[data_start],selected_file->lbl8_rec_len);
				address[t_record].zip_right = lbl8;

				data_start += selected_file->lbl8_rec_len;data_len-=selected_file->lbl8_rec_len;
				break;
			case 3://1,2
				memcpy(&lbl8,&read_buffer[data_start],selected_file->lbl8_rec_len);
				address[t_record].zip_left = lbl8;

				memcpy(&lbl8,&read_buffer[data_start+selected_file->lbl8_rec_len],selected_file->lbl8_rec_len);
				address[t_record].zip_right = lbl8;

				data_start += (selected_file->lbl8_rec_len*2);data_len-=(selected_file->lbl8_rec_len*2);
				break;
			case 4://	1=2 = -1
				address[t_record].zip_left = 0;
				address[t_record].zip_right = 0;
				break;
			case 5://1 -1
				address[t_record].zip_left = 0;
				break;
			case 6://2 -1
				address[t_record].zip_right = 0;
				break;
		}

		t_record++;
	}
}

void IMG_internal_list::readNumbers(const unsigned long& net_address,unsigned long &data_start,unsigned short flag,unsigned char net_flag,mapRead_address address[_MAX_SEGMENTS],int records) {
	unsigned short			data_len = 0;
	//unsigned short			t_poz = 0;

	if( flag == 3 ) return;
	if( flag == 0 ) {
		memcpy(&data_len,&read_buffer[data_start],1);
		data_start++;
	}
	if( flag == 1 ) {
		memcpy(&data_len,&read_buffer[data_start],2);
		data_start+=2;
	}
	//while( data_len > 0 ) {
	readData(net_address + data_start,data_len,2);
	//	numbering.push_back(read_buffer2[0]);

	data_start+=data_len;
	//data_len--;
	//}

	NumbersDecoder	*nd = new NumbersDecoder(
		read_buffer2,
		data_len,
		net_flag,
		selected_file->net_base_bits,
		address,
		records);
	delete nd;
}

/**
Reading of POI additional data on request - must be noticed that as
input I do require :
@param type        - type of object
@param sub_type    - sub_type of object
@param poi_address - value returned be _READDATA
*/

string	IMG_internal_list::read11Number(const unsigned char* buffer,const size_t& max_size,size_t& position) {
	string	t_return;
	char	ph_string[4];
	unsigned char	ph_char;
	bool	finish = false;
	bool	first = true;

	for(;;) {	
		ph_char = buffer[position++];

		if( !first )
			if( ph_char & 0x80 ) finish = true;

		ph_char &= 0x7f;
		itoa( ph_char, ph_string, 11);

		if(ph_string[0] == 'a' || ph_string[0] == 'A')
			ph_string[0] = ' ';
		if(ph_string[1] == 'a' || ph_string[1] == 'A')
			ph_string[1] = ' ';
		if(ph_string[1] == 0 ) {
			ph_string[1] = ph_string[0];
			ph_string[0] = '0';
		}
		ph_string[2] = 0;
		t_return += ph_string;

		first = false;
		if( finish ) break;
	} 

	return t_return;
}


void IMG_internal_list::readPOI(const unsigned int& i_type,const unsigned int& sub_type,const unsigned long& _poi_address,const unsigned long& selected_map) {
	unsigned char	t_flag = 0xff;
	unsigned char	t_bit = 1;
	unsigned int	lbl_address = 0;
	unsigned int	lbl4 = 0;
	unsigned int	lbl8 = 0;
	unsigned char	local_buffer[240];
	size_t			t_read_pos;
	size_t			t_read_size = 30;
	
	std::vector<IMG_file>::iterator	prev_selected_file = selected_file;
	std::string		name,street,house_number,city,region,country,zip,phone;
	unsigned long	poi_address = _poi_address;

	unsigned long	Lname = 0;
	unsigned long	Lstreet = 0;
	//unsigned long	Lhouse_number = 0;
	unsigned long	Lcity = 0;
	unsigned long	Lregion = 0;
	unsigned long	Lcountry = 0;
	//unsigned long	Lzip = 0;
	unsigned int	type = (i_type & 0xff00)>>8;

	//poi_address = _poi_address;

	/*if( poi_address == 0 )
		return;
	*/
	selected_file = internal_files.begin() + selected_map;
	lbl_address = 0;

	if( i_type < 0x010000 ) {
		poi_address += selected_file->lbl6_data;

		//EXIT
		if( type >= 0x20 && type <= 0x27 ) {
		}

		//przeczytany bufor skopiowac do zmiennej lokalnej!

		//POI
		if( type == 0x12 || (type >= 0x16 && type <= 0x1d) || type >= 0x29 ) {
			readData(poi_address,t_read_size);
			memcpy(local_buffer,read_buffer,t_read_size);

			memcpy(&lbl_address,&local_buffer[0],3);
			lbl_address &= 0x3fffff;
			Lname = lbl_address;
			name = readLBL(lbl_address);
			//if( name == "URZAD STATYSTYCZNY" )
			//	name = name;

			if( read_buffer[2] & 0x80 ) {
				memcpy(&t_flag,&local_buffer[3],1);
				t_read_pos = 4;
			} else
				t_read_pos = 3;

			//house number
			if( selected_file->lbl6_flag & 0x01 ) {
				if( t_flag & t_bit ) {
					if( !(local_buffer[t_read_pos] & 0x80) ) {
						unsigned char value[3];
						//string
						value[0] = local_buffer[t_read_pos+1];
						value[1] = local_buffer[t_read_pos+2];
						value[2] = local_buffer[t_read_pos];
						memcpy(&lbl_address,value,3);
						lbl_address &= 0x3fffff;
						//Lhouse_number = lbl_address;
						house_number = readLBL(lbl_address);
						t_read_pos+=3;
					} else {
						//number
						//Lhouse_number = 0;
						house_number = read11Number(local_buffer,t_read_size - t_read_pos,t_read_pos);
					}	
				}
				t_bit <<= 1;
			} 

			//street name
			if( selected_file->lbl6_flag & 0x02 ) {
				if( t_flag & t_bit ) {
					if( t_read_pos +3 > t_read_size ) {
						t_read_size *= 2;
						readData(poi_address,t_read_size);
						memcpy(local_buffer,read_buffer,t_read_size);
					}

					memcpy(&lbl_address,&local_buffer[t_read_pos],3);
					lbl_address &= 0x3fffff;
					Lstreet = lbl_address;
					street = readLBL(lbl_address);

					t_read_pos += 3;
				}
				t_bit <<= 1;
			}

			//city/region/country
			if( selected_file->lbl6_flag & 0x04 ) {
				if( t_flag & t_bit ) {
					if( t_read_pos +3 > t_read_size ) {
						t_read_size *= 2;
						readData(poi_address,t_read_size);
						memcpy(local_buffer,read_buffer,t_read_size);
					}

					lbl4 = 0;
					if( selected_file->lbl4_rec_len == 1 ) {
						lbl4 = local_buffer[t_read_pos];
						++t_read_pos;
					}
					else if( selected_file->lbl4_rec_len == 2 ) {
						memcpy(&lbl4,&local_buffer[t_read_pos],2);
						t_read_pos += 2;
					}

					if( poiFunction_i )
						readCityRegionCountry_i(lbl4,city,Lcity,region,Lregion,country,Lcountry);
					else
						readCityRegionCountry(lbl4,city,region,country);
				}
				t_bit <<= 1;
			}

			//zip
			if( selected_file->lbl6_flag & 0x08 ) {
				if( t_flag & t_bit ) {
					if( t_read_pos +3 > t_read_size ) {
						t_read_size *= 2;
						readData(poi_address,t_read_size);
						memcpy(local_buffer,read_buffer,t_read_size);
					}

					lbl8 = 0;
					if( selected_file->lbl8_rec_len == 1 ) {
						lbl8 = local_buffer[t_read_pos];
						++t_read_pos;
					}
					else if( selected_file->lbl8_rec_len == 2 ) {
						memcpy(&lbl8,&local_buffer[t_read_pos],2);
						t_read_pos += 2;
					}

					if( lbl8 ) {
						readData(selected_file->lbl8_data + (lbl8-1)*selected_file->lbl8_rec,3);
						memcpy(&lbl_address,&read_buffer[0],3);
						lbl_address &= 0x3fffff;
						//Lzip = lbl_address;
						zip = readLBL(lbl_address);
					}
				}
				t_bit <<= 1;
			}
			//phone number
			if( selected_file->lbl6_flag & 0x10 ) {
				if( t_flag & t_bit ) {
					if( t_read_pos + 11 > t_read_size ) {
						t_read_size *= 2;
						readData(poi_address,t_read_size);
						memcpy(local_buffer,read_buffer,t_read_size);
					}

					if( !(local_buffer[t_read_pos] & 0x80) ) {
						lbl_address = 0;
						lbl_address = local_buffer[t_read_pos+1];
						lbl_address |= (local_buffer[t_read_pos+2]<<8);
						lbl_address |= (local_buffer[t_read_pos]<<16);

						lbl_address &= 0x3fffff;
						phone = readLBL(lbl_address);
						t_read_pos +=3;
					} else
						phone = read11Number(local_buffer,t_read_size - t_read_pos-1,t_read_pos);
				}
				t_bit <<= 1;
			}
		}

	} else {
	//NT POI
		unsigned int t_total_len = 20;
		unsigned int t_start_pos = 1;
		bool	t_number = false;
		bool	t_phone = false;
		bool	t_text_phone = false;
		bool	t_lbl4 = false;
		bool	t_lbl8 = false;
		bool	t_label_lbl1 = false;
		unsigned short	t_zeros;

		readData(poi_address,t_total_len);
		if( !(read_buffer[0] & 1) )
			t_label_lbl1 = true;
		t_total_len = (read_buffer[1]>>1) + 1;
		if( t_total_len > 20 )
			readData(poi_address,t_total_len);

		if( !t_label_lbl1 ) {
			while( read_buffer[t_start_pos] && t_start_pos < t_total_len)
				t_start_pos++;
		} else //tylko dlugosc + lbl1 (3)
			t_start_pos+=2;
		t_start_pos++;

		t_total_len -= t_start_pos;
		poi_address += t_start_pos;
		initialiseReadBits(poi_address,t_total_len);
		if( t_label_lbl1 ) {
			//w ostatnim bajcie pominac 6 bitow
			readBits(6);
		}

		t_zeros = 0;
		while( (bit_position >> 3) < t_total_len ) {
			if( readBits(1) ) {
				if( t_zeros == 0 ) {
					if( readBits(1) )
						assert(false);
					else if( !t_phone ){
						lbl_address = readBits(20);
						phone = readLBL(lbl_address);
						t_phone = true;
					} else
						assert(false);					

					readData(poi_address,t_total_len);
					//////////////////
				} else if( t_zeros == 1 ) {
					if( !t_number ) {
						lbl_address = readBits(20);
						house_number = readLBL(lbl_address);

						t_number = true;
					} else
						assert(false);

					readData(poi_address,t_total_len);
					//////////////////
				} else if( t_zeros == 2 ) {
					assert( lbl4 == false );
					if( selected_file->lbl4_bits )
						lbl4 = readBits(selected_file->lbl4_bits) + 1;
					else
						lbl4 = 0;

					if( poiFunction_i )
						readCityRegionCountry_i(lbl4,city,Lcity,region,Lregion,country,Lcountry);
					else
						readCityRegionCountry(lbl4,city,region,country);

					t_lbl4 = true;
					readData(poi_address,t_total_len);

					if( readBits(1) ) {
						if( selected_file->lbl8_bits )
							lbl8 = readBits(selected_file->lbl8_bits) + 1;
						else
							lbl8 = 0;
						readData(selected_file->lbl8_data + (lbl8-1)*selected_file->lbl8_rec,3);
						memcpy(&lbl_address,&read_buffer[0],3);
						lbl_address &= 0x3fffff;
						zip = readLBL(lbl_address);

						t_lbl8 = true;
					} //else
						//assert(false);
				
					readData(poi_address,t_total_len);
				} else if( t_zeros == 3 ) {
					if( !t_lbl8 ) {
						lbl8 = readBits(selected_file->lbl8_bits) + 1;
						readData(selected_file->lbl8_data + (lbl8-1)*selected_file->lbl8_rec,3);
						memcpy(&lbl_address,&read_buffer[0],3);
						lbl_address &= 0x3fffff;
						zip = readLBL(lbl_address);

						t_lbl8 = true;
					} else
						assert(false);
				
					readData(poi_address,t_total_len);
				} else if( t_zeros == 4 ) {
					//sam numer telefonu
					if( !t_phone ){
						lbl_address = readBits(20);
						phone = readLBL(lbl_address);
						t_phone = true;
					} else
						assert(false);					

					readData(poi_address,t_total_len);
				} else if( t_zeros == 5 ) {
					//reprezentacja telefonu jako text
					if( !t_text_phone ){
						lbl_address = readBits(20);
						phone = readLBL(lbl_address);
						t_text_phone = true;
					} else
						assert(false);		
					readData(poi_address,t_total_len);
				} else if( t_zeros > 5 )
					assert(false);


				t_zeros = 0;
			} else
				t_zeros++;
		}
	}

	//(house_number,street_name,unsigned short city_idx,unsigned short zip_idx,const char* phone)
	if( poiFunction_i )
		poiFunction_i(name.c_str(),Lname,house_number.c_str(),street.c_str(),lbl4,lbl8,phone.c_str(),current_RGN,current_TRE);
	else
		poiFunction(house_number.c_str(),street.c_str(),city.c_str(),region.c_str(),country.c_str(),zip.c_str(),phone.c_str());
	selected_file = prev_selected_file;
	/*
	#ifdef _CONSOLE
	std::cout<<std::endl;
	if( name.size() ) std::cout<<"Label="<<name<<std::endl;
	if( house_number.size() ) std::cout<<"HouseNumber="<<house_number<<std::endl;
	#endif
	*/
}
