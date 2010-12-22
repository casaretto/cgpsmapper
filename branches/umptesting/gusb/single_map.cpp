/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/
#include <time.h>
#include "single_map.h"
#ifdef _WIN32
#include "Fish/blowfish.h"
#endif
#include <string.h>

bool	long_IMG_header = false;

//imageSize = 11 (max w starym formacie)
unsigned char	c_imgDskimg2_128m[] =       { 0x20, 0x00, 0x20, 0x00, 0x00, 0x04, 0x00, 0x00 }; // 0x18 - 0x1f
unsigned char	c_imgDskimg5_128m[] =       { 0x20, 0x00, 0x20, 0x00, 0x09, 0x02, 0xFF, 0xFF };// 0x5d - 0x64 - (block power jest to 2, musi sie zmieniac, 0x40 - musi byc wyliczone)
unsigned char	c_imgDskimg7_128m[] =       { 0x01, 0x00, 0x00, 0x1f, 0xE0, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10 };// 0x1c0

//14 - maksymalny rozmiar 1GB
unsigned char	c_imgDskimg2_1g[] =       { 0x20, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00 }; // 0x18 - 0x1f
unsigned char	c_imgDskimg5_1g[] =       { 0x00, 0x01, 0x20, 0x00, 0x09, 0x02, 0xFF, 0xFF };// 0x5d - 0x64 - (block power jest to 2, musi sie zmieniac, 0x40 - musi byc wyliczone)
unsigned char	c_imgDskimg7_1g[] =       { 0x01, 0x00, 0x00, 0xff, 0xE0, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80 };// 0x1c0

unsigned char	c_imgDskimg2_512[] =       { 0x04, 0x00, 0x10, 0x00, 0x10, 0x00, 0x00, 0x00 };
unsigned char	c_imgDskimg5_512[] =       { 0x10, 0x00, 0x04, 0x00, 0x09, 0x00, 0x00, 0x04 };
unsigned char	c_imgDskimg7_512[] =       { 0x01, 0x00, 0x00, 0x0f, 0x04, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00 };																									//   ^^^^^^^^^^^^^^^^^^^^rozmiar	

unsigned char	c_imgDskimg2_long_512[] =  { 0x20, 0x00, 0x02, 0x00, 0x11, 0x03, 0x00, 0x00 };
unsigned char	c_imgDskimg5_long_512[] =  { 0x02, 0x00, 0x20, 0x00, 0x09, 0x00, 0x18, 0x00 };
unsigned char	c_imgDskimg7_long_512[] =  { 0x01, 0x00, 0x00, 0x00, 0x11, 0x05, 0x00, 0x00, 0x00, 0x00, 0x51, 0x01, 0x00 };

unsigned char	c_imgDskimg2_1024[] =      { 0x10, 0x00, 0x10, 0x00, 0x20, 0x00, 0x00, 0x00 };
unsigned char	c_imgDskimg5_1024[] =      { 0x10, 0x00, 0x10, 0x00, 0x09, 0x01, 0x00, 0x10 };
unsigned char	c_imgDskimg7_1024[] =      { 0x01, 0x00, 0x00, 0x0f, 0x10, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00 };

unsigned char	c_imgDskimg2_long_1024[] = { 0x20, 0x00, 0x02, 0x00, 0x11, 0x03, 0x00, 0x00 };
unsigned char	c_imgDskimg5_long_1024[] = { 0x02, 0x00, 0x20, 0x00, 0x09, 0x01, 0x77, 0x08 };
unsigned char	c_imgDskimg7_long_1024[] = { 0x01, 0x00, 0x00, 0x00, 0x50, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x90, 0x5f, 0x00  };

unsigned char	c_imgDskimg2_2048[] =	   { 0x10, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x00 };
unsigned char	c_imgDskimg5_2048[] =      { 0x10, 0x00, 0x10, 0x00, 0x09, 0x02, 0x00, 0x20 };
unsigned char	c_imgDskimg7_2048[] =      { 0x01, 0x00, 0x00, 0x0F, 0x10, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00  };

unsigned char	c_imgDskimg2_long_2048[] = { 0x20, 0x00, 0x02, 0x00, 0x11, 0x03, 0x00, 0x00 };
unsigned char	c_imgDskimg5_long_2048[] = { 0x02, 0x00, 0x20, 0x00, 0x09, 0x02, 0x09, 0x98 };
unsigned char	c_imgDskimg7_long_2048[] = { 0x01, 0x00, 0x00, 0x01, 0x50, 0xDD, 0x00, 0x00, 0x00, 0x00, 0x70, 0x77, 0x00  };

unsigned char	c_imgDskimg2_4096[] =      { 0x20, 0x00, 0x40, 0x00, 0xCD, 0x03, 0x00, 0x00 };	// 0x18 - 0x1f
unsigned char	c_imgDskimg5_4096[] =      { 0x40, 0x00, 0x20, 0x00, 0x09, 0x03, 0xed, 0x84 };	// 0x5d - 0x64
unsigned char	c_imgDskimg7_4096[] =      { 0x01, 0x00, 0x00, 0x0c, 0x58, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x68, 0x27, 0x04 };	// 0x1c0

unsigned char	c_imgDskimg2_long_4096[] = { 0x10, 0x00, 0x10, 0x00, 0xC7, 0x03, 0x00, 0x00 };
unsigned char	c_imgDskimg5_long_4096[] = { 0x10, 0x00, 0x10, 0x00, 0x09, 0x03, 0x36, 0xA6 };
unsigned char	c_imgDskimg7_long_4096[] = { 0x01, 0x00, 0x00, 0x0F, 0x10, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00  };

unsigned char	c_imgDskimg2_long_8192[] = { 0x20, 0x00, 0x20, 0x00, 0xC7, 0x03, 0x00, 0x00 };	// 0x18 - 0x1f
unsigned char	c_imgDskimg5_long_8192[] = { 0x20, 0x00, 0x20, 0x00, 0x09, 0x04, 0xCF, 0x9C };	// 0x5d - 0x64
unsigned char	c_imgDskimg7_long_8192[] = { 0x01, 0x00, 0x00, 0x0e, 0xd0, 0x64, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xCC, 0x09 };	// 0x1c0
//                                                                                                       rozmiar IMG
//offset 0x40 - rozmiar naglowka
//0x02 - 
//0x08 - 0x1000
internal_file::internal_file(char xor_value,const char* file_name,const char* internal_file_name, internal_file_type file_type,int size) {
	char tmp_buffer[12];
	strncpy(tmp_buffer,internal_file_name,8);
	tmp_buffer[8] = 0;
	for(int a = 0; a < 8; a++ )
		if( tmp_buffer[a] == 0 )
			tmp_buffer[a] = 0x20;
	this->protected_buffer_use = false;
	this->internal_name = tmp_buffer;
	this->file_name = file_name;
	this->file_type = file_type;
	this->xor_value = xor_value;
	this->size = size;
	crypt_gps_id = 0;
	gps_file = false;
	self_exe = false;
#ifdef _WIN32
	fish = NULL;
#endif
	c_fish = false;

	product_id = 0;
	TRE_id = 0; //for imgTRE file only
	TRE_header_size = 0;

	//calculated positions of each subfile in the final IMG file
	fat_entry_offset_start = 0; //offsets for fat entries
	fat_entry_offset_end = 0;
	fat_blocks = 0; //number of occupied blocks by the file
	file_start = 0; //offsets for the file itself
	file_end = 0;

	password[0] = 0;

	if( file_type == imgIMG ) {
		file_start = 0; //offsets for the file itself
		file_end = 0x400;
	}

/*
	if( file_type == imgIMG ) {
		FAT = new char[0x400];
		memset(FAT,0x400,0);
	} else
		FAT = NULL;
*/
}

size_t internal_file::read(void* buffer, int len) {
	size_t ret = fread(buffer,1,len,input_file);
	dexor(buffer,len);
	return ret;
}

void	internal_file::set_gps_buffer(const char* buffer) {
	gps_file = true;
	memcpy(protected_buffer,buffer,0x200);
}

string	internal_file::get_internal_filename() {
	string t_name = internal_name;
	switch( file_type ) {
			case imgMPS: t_name+=".MPS";break;
			case imgTRE: t_name+=".TRE";break;
			case imgRGN: t_name+=".RGN";break;
			case imgLBL: t_name+=".LBL";break;
			case imgNET: t_name+=".NET";break;
			case imgNOD: t_name+=".NOD";break;
			case imgSRT: t_name+=".SRT";break;
			case imgTYP: t_name+=".TYP";break;
			case imgMDR: t_name+=".MDR";break;
			case imgDEM: t_name+=".DEM";break;
			case imgGMP: t_name+=".GMP";break;
	}
	return t_name;
}

void internal_file::dexor(void* tmp_buffer,int len) {
	if( xor_value == 0 )
		return;
	for( int i=0; i<len; i++)
		((unsigned char*)tmp_buffer)[i] = ((unsigned char*)tmp_buffer)[i]^xor_value;
}

internal_file::~internal_file() {
//	if( file_type == imgIMG )
//		delete FAT;
#ifdef _WIN32
	if( fish != NULL )
		delete fish;
#endif
}

void internal_file::set_tre(int TRE_id,int TRE_file_name,int TRE_header_size,string TRE_map_name) {
	this->TRE_file_name = TRE_file_name;
	this->TRE_id = TRE_id;
	this->TRE_header_size = TRE_header_size;
	this->TRE_map_name = TRE_map_name;
}

void internal_file::set_blocks(int block_size) {
	float f_blocks = float(this->size) / float(block_size);
	fat_blocks = int(f_blocks);
	if( (f_blocks - int(f_blocks)) > 0 )
		fat_blocks++;
}

void internal_file::set_fat_position(int last_fat_offset) {

	//int last_occupied = first_free_block + fat_blocks;
	//start & end in fat
	fat_entry_offset_start = last_fat_offset;
	fat_entry_offset_end = last_fat_offset;

	fat_entry_offset_end   += ((fat_blocks / 240) + ((fat_blocks % 240)>0?1:0)) * 0x200;

	//return last_occupied;
}

int internal_file::set_file_position(int first_free_block,int block_size) { //,int last_fat_offset) {
	//fat starts at 0x400 or at 0x1000
	int last_occupied = first_free_block + fat_blocks;
	fat_first = first_free_block;

	//initialise var

	//start & end in file
	file_start = fat_first * block_size;
	file_end = (fat_first + fat_blocks) * block_size;

	return last_occupied;
}

bool internal_file::get_fat(int file_pos,char* buffer) {
	if( file_pos < fat_entry_offset_start || file_pos >= fat_entry_offset_end )
		return false;

	int	ffff = 0xffff;
	int	curr_block = fat_first; //curr fat block
	int tmp_fat_entry = fat_entry_offset_start;
	int block_wrote = 0;
	char next_fat_number = 0;
	while( tmp_fat_entry != file_pos ) {
		tmp_fat_entry += 0x200;
		curr_block += 240;
		next_fat_number++;
	}

	memset(buffer, 0, 0x200);
	memcpy(buffer, "\x01\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20", 12);
	if( file_type == imgIMG )
		buffer[0x10] = 3;
	else {
		memset(&buffer[1], 0x20, 8);
		strncpy(&buffer[1], internal_name.c_str(), 8);
		switch( file_type ) {
			case imgMPS: memcpy(&buffer[9],"MPS",3);break;
			case imgTRE: memcpy(&buffer[9],"TRE",3);break;
			case imgRGN: memcpy(&buffer[9],"RGN",3);break;
			case imgLBL: memcpy(&buffer[9],"LBL",3);break;
			case imgNET: memcpy(&buffer[9],"NET",3);break;
			case imgNOD: memcpy(&buffer[9],"NOD",3);break;
			case imgSRT: memcpy(&buffer[9],"SRT",3);break;
			case imgTYP: memcpy(&buffer[9],"TYP",3);break;
			case imgMDR: memcpy(&buffer[9],"MDR",3);break;
			case imgDEM: memcpy(&buffer[9],"DEM",3);break;
			case imgGMP: memcpy(&buffer[9],"GMP",3);break;
		}
		buffer[0x11] = next_fat_number;
	}

	memcpy(&buffer[12], &size, 4);
	while( block_wrote < 240 && curr_block < (fat_blocks + fat_first)) {
		memcpy(&buffer[0x20 + block_wrote * 2], &curr_block, 2);
		block_wrote++;
		curr_block++;
	}
	while( block_wrote < 240 ) {
		memcpy(&buffer[0x20 + block_wrote * 2], &ffff , 2);
		block_wrote++;
	}
	return true;
}

int internal_file::get_file(int file_pos,char* buffer) {
	int	was_read;
	int	to_read_file;
	if( file_pos < file_start || file_pos >= file_end )
		return false;

	if( file_type == imgIMG )
		return false;
	if( file_type == imgMPS && self_exe == false ) {
		//generate file on the fly
		return false;
	}

	int to_read = 0x1000;

	if( file_end - file_pos < 0x1000 )
		to_read = file_end - file_pos;

	to_read_file = to_read;
	if( size - (file_pos - file_start) < to_read )
		to_read_file = size - (file_pos - file_start);

	memset(buffer,0,0x1000);
	input_file = fopen(file_name.c_str(),"rb");
	if( input_file == NULL )
		return false;

	fseek(input_file,internal_offset + (file_pos-file_start),SEEK_SET);
	was_read = (int)read(buffer,to_read_file);
	if( crypt_gps_id != 0 )
		IMG::xor_data(buffer,0x1000,crypt_gps_id);

#ifdef _WIN32
	if( file_pos-file_start == 0 )
		if( strlen(password) ) {
			if( fish == NULL )
				fish = new CBlowFish();
			fish->Initialize((unsigned char*)password,strlen(password));
			c_fish = true;
		}

	if( c_fish ) {
		fish->Decode((unsigned char*)buffer,(unsigned char*)buffer,0x1000);
	}
#endif

	if( gps_file && (file_pos-file_start) == 0 ) 
		memcpy(buffer,protected_buffer,0x200);

	if( file_type == imgTYP && product_id != 0 && (file_pos - file_start) == 0 && to_read >= 0x33 ) {
		//zmiana ID!
		int	family_id;
		int subfamily_id;

		family_id = (product_id & 0xffff0000)>>16;
		subfamily_id = (product_id & 0xffff);

		memcpy(&buffer[0x2f],&family_id,2);
		memcpy(&buffer[0x31],&subfamily_id,2);
	}

	fclose(input_file);
	return to_read;
}

/*****************************************************************************/
mps_internal_file::mps_internal_file()
: internal_file(0,"","MAPSOURC",imgMPS,0) {
	mps_buffer = NULL;
	initialised = false;
}

mps_internal_file::~mps_internal_file() {
	if( mps_buffer) 
		delete mps_buffer;
}

void mps_internal_file::set_mapset_name(string name) {
	mapset_name = name;
}

void mps_internal_file::set_product_id(int product_id) {
	this->product_id = product_id;
}

void mps_internal_file::clear() {
	tre_files.clear();
	region_list.clear();
	initialised = false;
}

void mps_internal_file::add_tre_file(internal_file tre_file) {
	tre_files.push_back(tre_file);
	initialised = false;
}

void mps_internal_file::add_keys(vector<string> key_list) {
	this->key_list = key_list;
}

int mps_internal_file::get_file_size() {
	char	buffer[0x1000];
	int		dest_address = 0;
	int		to_send = 0;
	short	packet_size = 0;
	map<int,string>::iterator	region_i;

	if( initialised == false ) {

		//zebranie danych dla rekordu 0x46 - zgrupowanie 
		for( tre_file_i = tre_files.begin(); tre_file_i != tre_files.end(); tre_file_i++ ) {
			if( region_list.find((*tre_file_i).product_id) == region_list.end() ) {
				region_list[(*tre_file_i).product_id] = (tre_file_i)->region_name;
			}
		}

		size = 0;
		for( tre_file_i = tre_files.begin(); tre_file_i != tre_files.end(); tre_file_i++ ) {
			// 0x4c
			size += 19;
			if( (tre_file_i)->region_name.size() ) {
				size += (int)(tre_file_i)->region_name.size() + 1;
				size += (int)(tre_file_i)->TRE_map_name.size() + 1;
				size += (int)(tre_file_i)->region_name.size() + 1 + 1; 
			} else {
				size += (int)mapset_name.size() + 1;
				size += (int)(tre_file_i)->TRE_map_name.size() + 1;
				size += (int)mapset_name.size() + 1 + 1; 
			}
		}

		// 0x46
		for( region_i = region_list.begin(); region_i != region_list.end(); region_i++ ) {
			size += 7;
			size += (*region_i).second.size() + 1;
		}

		//0x56
		size += 4;
		size += (int)mapset_name.size() + 1;

		//0x55
		if( key_list.size() ) {
			//size += 3;
			size += int(key_list.size()*29); //26 + 3 nag³owek
		}
		/*
		if(keysToSend) {
		// 0x55 lista kluczy
		mpsSize += 3;
		mpsSize += numberOfKeys * 26;
		}
		*/
		mps_buffer = new char[size];

		//generation
		for( tre_file_i = tre_files.begin(); tre_file_i != tre_files.end(); tre_file_i++ ) {
			//send 0x4c record
			memset(buffer,0,sizeof(buffer));
			buffer[0] = 0x4c;
			//memcpy(&buffer[3], &product_id, 4);
			memcpy(&buffer[3], &((*tre_file_i).product_id), 4);
			memcpy(&buffer[7], &(*tre_file_i).TRE_file_name, 4);
			
			if( (tre_file_i)->region_name.size() ) {
				to_send = (int)(tre_file_i)->region_name.size() + 11;
				memcpy(&buffer[11], (tre_file_i)->region_name.c_str(), (tre_file_i)->region_name.size());
			} else {
				to_send = (int)mapset_name.size() + 11;
				memcpy(&buffer[11], mapset_name.c_str(), mapset_name.size());
			}
			buffer[to_send] = 0;
			to_send++;

			memcpy(&buffer[to_send], (*tre_file_i).TRE_map_name.c_str(), (*tre_file_i).TRE_map_name.size());
			to_send += (int)(*tre_file_i).TRE_map_name.size();
			buffer[to_send] = 0;
			to_send++;

			if( (tre_file_i)->region_name.size() ) {
				memcpy(&buffer[to_send], (tre_file_i)->region_name.c_str(), (tre_file_i)->region_name.size());
				to_send += (int)(tre_file_i)->region_name.size();
			} else {
				memcpy(&buffer[to_send], mapset_name.c_str(), mapset_name.size());
				to_send += (int)mapset_name.size();
			}
			/*Secondary name HERE!!
			memcpy(&buffer[tmpSize], (*fileIMG)->Area.c_str(), (*fileIMG)->Area.size());
			tmpSize += (*fileIMG)->Area.size();*/
			buffer[to_send] = 0;
			to_send++;

			memcpy(&buffer[to_send], &((*tre_file_i).TRE_id), 4);
			to_send += 4;
			memset(&buffer[to_send], 0, 4);
			to_send += 4;

			packet_size = (short)to_send-3;
			memcpy(&buffer[1],&packet_size,2);

			memcpy(&mps_buffer[dest_address],buffer,to_send);
			dest_address += to_send;
		}
		{
			memset(buffer,0,sizeof(buffer));
			buffer[0] = 0x56;

			to_send = 3 + (int)mapset_name.size();
			memcpy(&buffer[3], mapset_name.c_str(), mapset_name.size());
			buffer[to_send] = 0;
			to_send++;
			buffer[to_send] = 1;
			to_send++;

			packet_size = (short)to_send-3;
			memcpy(&buffer[1],&packet_size,2);

			memcpy(&mps_buffer[dest_address],buffer,to_send);
			dest_address += to_send;
		}

		if( key_list.size() ) {
			for(vector<string>::iterator t_key = key_list.begin(); t_key< key_list.end(); t_key++){
				memset(buffer,0,sizeof(buffer));
				buffer[0] = 0x55;
				to_send = 3 + 26;
			
				memcpy(&buffer[3],(*t_key).c_str(),25);
				packet_size = (short)to_send-3;
				memcpy(&buffer[1],&packet_size,2);

				memcpy(&mps_buffer[dest_address],buffer,to_send);
				dest_address += to_send;
			}
		}

		for( region_i = region_list.begin(); region_i != region_list.end(); region_i++ ) {
			//send 0x46 record
			memset(buffer,0,sizeof(buffer));
			buffer[0] = 0x46;

			memcpy(&buffer[3], &((*region_i).first), 4);
			to_send = (int)((*region_i).second.size()) + 1 + 4 + 3;
			memcpy(&buffer[7], (*region_i).second.c_str(), (*region_i).second.size());
			buffer[7 + (*region_i).second.size()] = 0;

			packet_size = (short)to_send-3;
			memcpy(&buffer[1],&packet_size,2);

			memcpy(&mps_buffer[dest_address],buffer,to_send);
			dest_address += to_send;
		}

	}
	initialised = true;
	return this->size;
}

int mps_internal_file::get_file(int file_pos,char* buffer) {
	//buffer is 0x1000 large
	int	mps_position;
	if( file_pos < file_start || file_pos >= file_end )
		return false;

	if( file_type != imgMPS ) {
		return false;
	}

	mps_position = file_pos - file_start;
	memset(buffer,0,0x1000);

	int to_send = 0x1000;
	if( file_end - file_pos < 0x1000 )
		to_send = file_end - file_pos;

	if( mps_position < size ) {
		memcpy(buffer,&mps_buffer[mps_position], (to_send+mps_position > size ? (size - mps_position) : to_send) );
	}

	return to_send;
}

/*****************************************************************************/

IMG::IMG() {
	mps_enable = true;
//	typ_enable = false;
}

IMG& IMG::operator+=(const internal_file &i_file) {
	//make a local copy here
	files.push_back(i_file);

	return *this;
}

IMG& IMG::operator-=(const internal_file &i_file) {
	
	remove_IMG(i_file.get_filename(),i_file.get_internal_short_name());

	return *this;
}

void IMG::set_mapset(const char* mapset_name) {
	map_copyright = mapset_name;

	MPS.set_mapset_name(mapset_name);
	MPS.set_product_id(CONST_FAMILY_ID);
}

void IMG::dexor(void* tmp_buffer,int len) {
	if( xor_value == 0 )
		return;
	for( int i=0; i<len; i++)
		((unsigned char*)tmp_buffer)[i] = ((unsigned char*)tmp_buffer)[i]^xor_value;
}

size_t IMG::read(void* buffer, int len) {
	size_t ret = fread(buffer,1,len,input_file);
	dexor(buffer,len);
	return ret;
}

void IMG::remove_all() {
	MPS.clear();
	files.clear();
}

bool IMG::remove_exact_IMG(const char* internal_file_name) {
	bool	found = false;
	file_i = files.begin();

	while( file_i < files.end() ) {
		if(  (*file_i).get_internal_filename() == internal_file_name ) {
			found = true;
			file_i = files.erase(file_i);
		} else
			file_i++;
	}
	return found;
}

bool IMG::remove_IMG(const char* file_name,const char* internal_file_name) {
	bool	found = false;
	file_i = files.begin();

	while( file_i < files.end() ) {
		if( (*file_i).file_name == string(file_name) && !strcmp((*file_i).get_internal_short_name(),internal_file_name) ) {
			found = true;
			file_i = files.erase(file_i);
		} else
			file_i++;
	}
	return found;
}

int IMG::get_files_size() {
	int t_ret = 0;
	file_i = files.begin();

	while( file_i < files.end() ) {
		t_ret += (*file_i).get_file_size();
		file_i++;
	}
	return t_ret;
}

bool IMG::add_IMG_internal(const char* file_name,vector<internal_file> *TRE_file_list,const char* selected_file,const char* selected_file_rename) {
	return add_IMG(file_name,TRE_file_list,"",0,0,false,"","",0,selected_file,selected_file_rename);
}

bool IMG::add_IMG(const char* file_name,vector<internal_file> *TRE_file_list,
				  const char* password,unsigned int crypt_gps_id,int crypt_start,bool gps_file,const char* mps_file,
				  const char* region_name,const int current_product_id,
				  const char* selected_file,const char* selected_file_rename) 
{
	//analise the FAT
	internal_file_type	new_file_type;
	int					read_pos = 0x400;
	int					fat_size = 0;
	char				buffer[50];
	unsigned char		block[2];
	int					block_size;
	string				map_name;
	bool				in_internal_file = false;

#ifdef _WIN32
	fish = new CBlowFish();
#endif
	c_fish = false;

	input_file = fopen(file_name,"rb");
	if( input_file == NULL ) {
#ifdef _WIN32
		delete fish;
#endif
		return false;
	}

#ifdef _WIN32
	if( strlen(password) ) {
		fish->Initialize((unsigned char*)password,strlen(password));
		c_fish = true;
		strncpy(f_password,password,255);
	}
#endif

	fseek(input_file,0,SEEK_SET);
	xor_value = 0;
	read(read_buffer,15);
#ifdef _WIN32
	if( c_fish) {
		memset(f_buffer,0,sizeof f_buffer);
		memcpy(f_buffer,read_buffer,sizeof f_buffer);
		fish->Decode((unsigned char*)f_buffer,(unsigned char*)f_buffer,sizeof f_buffer);
		memcpy(read_buffer,f_buffer,15);
	}
#endif

	memcpy(buffer,&read_buffer[2],6);	
	if( !strncmp(buffer,"GARMIN",6) ) {
		in_internal_file = true;
	} else {
		read_pos += crypt_start;
		fseek(input_file,crypt_start,SEEK_SET);
		if( crypt_gps_id == 0 )
			fread(&xor_value,1,1,input_file);
		else
			xor_value = 0;

#ifdef _WIN32
		if( c_fish) {
			fseek(input_file,crypt_start,SEEK_SET);
			fread(f_buffer,15,1,input_file);			
			f_buffer[0] = (unsigned char)xor_value;
			fish->Decode((unsigned char*)f_buffer,(unsigned char*)f_buffer,sizeof f_buffer);
			xor_value = f_buffer[0];
		}
#endif

		fseek(input_file,crypt_start,SEEK_SET);
		read(read_buffer,0x200);
		if( crypt_gps_id != 0 )
			xor_data(read_buffer,0x200,crypt_gps_id);

#ifdef _WIN32
		if( c_fish) {
			memset(f_buffer,0,sizeof f_buffer);
			memcpy(f_buffer,read_buffer,0x200);
			fish->Decode((unsigned char*)f_buffer,(unsigned char*)f_buffer,0x200);
			memcpy(read_buffer,f_buffer,0x200);
		}
#endif
		//fseek(input_file,0x10,SEEK_SET);
		//read(buffer,16);
		memcpy(buffer,&read_buffer[0x10],16);	
		if( strncmp(buffer,"DSKIMG",6) ) {
			fclose(input_file);
#ifdef _WIN32
			delete fish;
#endif
			return false;
		}
	}

	if( in_internal_file ) {
		char	local_file_name[9];
		//char	file_type[4];
		char	t_file_type[255];			
		char	drive[5];
		char	dir[1024];
#ifdef _WIN32	
		memset(local_file_name,0,sizeof local_file_name);

		_splitpath(file_name,drive,dir,local_file_name,t_file_type);
#else
		const char	*t_ext;		
		t_ext = strrchr(file_name,'.');
		//*t_file_type = '\0';
		memset(t_file_type,0,sizeof(t_file_type));
		strncpy(t_file_type,t_ext,4);

		t_ext = strrchr(file_name,'/');
		if( t_ext != NULL ) {
			strncpy(local_file_name,t_ext+1,8);
		} else
			strncpy(local_file_name,file_name,8);
		local_file_name[8] = 0;
#endif		
		for(int i = 0; i<strlen(t_file_type); ++i)
			t_file_type[i] = toupper(t_file_type[i]);

		new_file_type = imgUNKNOWN;
		if( !memcmp(t_file_type,".TRE",3) )		new_file_type = imgTRE;
		else if( !memcmp(t_file_type,".RGN",3) )		new_file_type = imgRGN;
		else if( !memcmp(t_file_type,".LBL",3) )		new_file_type = imgLBL;
		else if( !memcmp(t_file_type,".NET",3) )		new_file_type = imgNET;
		else if( !memcmp(t_file_type,".NOD",3) )		new_file_type = imgNOD;
		else if( !memcmp(t_file_type,".DEM",3) )		new_file_type = imgDEM;
		else if( !memcmp(t_file_type,".MDR",3) )		new_file_type = imgMDR;
		else if( !memcmp(t_file_type,".   ",3) )		new_file_type = imgFAT;
		else if( !memcmp(t_file_type,".MPS",3) )		new_file_type = imgMPS;
		else if( !memcmp(t_file_type,".SRT",3) )		new_file_type = imgSRT;
		else if( !memcmp(t_file_type,".TYP",3) )		new_file_type = imgTYP;
		else if( !memcmp(t_file_type,".GMP",3) )		new_file_type = imgGMP;

		if( new_file_type == imgUNKNOWN ) {
			fclose(input_file);
#ifdef _WIN32
			delete fish;
#endif
			return false;
		}

		string t_img_file_name = "INT:";
		t_img_file_name+= string(file_name);

		fseek(input_file,0,SEEK_END);
		size_t file_len = ftell(input_file);

		internal_file new_file(0,file_name,local_file_name,new_file_type,file_len);
		strncpy(new_file.password,password,255);
		new_file.set_offset(0);
		new_file.set_crypt_gps_id(crypt_gps_id);
		new_file.product_id = current_product_id;
		if( crypt_start > 0 )
			new_file.set_self_exe();
		files.push_back(new_file);
		fclose(input_file);

		add_to_TRE_list(new_file_type,TRE_file_list,region_name);
		
#ifdef _WIN32
		delete fish;
#endif
		return true;
	}

	//OK - magic found
	//read map name - to be stored together with TRE file
	//fseek(input_file,0x49,SEEK_SET);
	//read(buffer,20);
	memcpy(buffer,&read_buffer[0x49],20);

	//fseek(input_file,0x65,SEEK_SET);
	//read(&(buffer[20]),20);
	memcpy(&(buffer[20]),&read_buffer[0x65],20);
	buffer[40] = 0;

	//fseek(input_file,0x61,SEEK_SET);
	//read(block,2);
	memcpy(block,&read_buffer[0x61],2);
	block_size = (1 << (block[0])) << block[1];
	for(int i = 39; i ; i-- )
		if( buffer[i] == ' ' )
			buffer[i] = 0;
		else
			break;
	map_name = buffer;

	//ok - now parse FAT to get all internal files
	for(;;) {
		new_file_type = add_file_from_FAT(file_name,map_name,block_size,read_pos,fat_size,crypt_gps_id,crypt_start,gps_file,mps_file,current_product_id,selected_file,selected_file_rename);
		if( new_file_type == imgUNKNOWN )
			break;
		add_to_TRE_list(new_file_type,TRE_file_list,region_name);
	}

	fclose(input_file);
#ifdef _WIN32
	delete fish;
#endif
	return true;
}

void IMG::add_to_TRE_list(internal_file_type new_file_type,vector<internal_file> *TRE_file_list,const char* region_name) {
	if( TRE_file_list == NULL )
		return;

	if( (new_file_type == imgTRE || new_file_type == imgMDR || new_file_type == imgSRT || new_file_type == imgTYP || new_file_type == imgGMP) && TRE_file_list != NULL) {
		files.back().region_name = "map";
		if( region_name != NULL )
			if( strlen(region_name) )
				files.back().region_name = region_name;
		internal_file t_TRE_file = (files.back());//copy
		if( new_file_type == imgMDR )
			t_TRE_file.TRE_map_name = "MDR index file";
		if( new_file_type == imgSRT )
			t_TRE_file.TRE_map_name = "SRT codepage";
		if( new_file_type == imgTYP )
			t_TRE_file.TRE_map_name = "TYP types file";
		if( new_file_type == imgGMP )
			t_TRE_file.TRE_map_name = "NT map";
		TRE_file_list->push_back(t_TRE_file);
	}
}

internal_file_type IMG::add_file_from_FAT(
	const char* img_file_name,
	string map_name,
	int block_size,
	int &read_pos,
	int &fat_size,
	unsigned int crypt_gps_id,int crypt_start,
	bool gps_file,
	const char* mps_file,
	const int current_product_id,
	const char* selected_file,const char* selected_file_rename) {

	char	file_name[9];
	char	file_type[4];
	char	t_file_type[4];
	int		file_len;
	unsigned char	znak;
	int		t_read_pos;
	unsigned short	t_secEndBlock;
	unsigned short	secStartBlock;
	unsigned short	secEndBlock;
	bool	t_end = false;
	internal_file_type	new_file_type = imgUNKNOWN;

	//last_pos = ftell(input_file);

	if( fat_size > 0 )
		if( (read_pos-crypt_start) >= fat_size )
			return new_file_type;

	fseek(input_file,read_pos,SEEK_SET);
	read(read_buffer,0x200);
	if( crypt_gps_id != 0 )
		xor_data(read_buffer,0x200,crypt_gps_id);

#ifdef _WIN32
	if( c_fish) {
		memset(f_buffer,0,sizeof f_buffer);
		memcpy(f_buffer,read_buffer,0x200);
		fish->Decode((unsigned char*)f_buffer,(unsigned char*)f_buffer,sizeof f_buffer);
		memcpy(read_buffer,f_buffer,0x200);
	}
#endif

	//fseek(input_file,read_pos+1,SEEK_SET);
	//read(file_name,8);
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
		&&	strncmp(file_type, "TYP", 3)
		&&	strncmp(file_type, "MDR", 3)
		&&	strncmp(file_type, "   ", 3)
		&&	strncmp(file_type, "MPS", 3)
		&&	strncmp(file_type, "DEM", 3)
		&&	strncmp(file_type, "GMP", 3)
		&&	strncmp(file_type, "NOD", 3)) {
			fseek(input_file,read_pos,SEEK_CUR);
			read_pos += 0x200;
			return new_file_type;
	}
	if( !strncmp(file_type, "   ", 3) )
		fat_size = file_len;//(file_len / block_size) + ((file_len % block_size)?1:0);
	//fseek(input_file,16,SEEK_CUR);

	//read(&secStartBlock, 2);
	memcpy(&secStartBlock,&read_buffer[32],2);
	//read(&t_secEndBlock, 2);
	memcpy(&t_secEndBlock,&read_buffer[34],2);
	secEndBlock = secStartBlock;

	t_read_pos = 36;
	while(!t_end) {
		while(secEndBlock == (t_secEndBlock - 1) && t_read_pos < 0x200) {
			if(secEndBlock == (t_secEndBlock - 1))
				secEndBlock = t_secEndBlock;
			//read(&t_secEndBlock, 2);
			memcpy(&t_secEndBlock,&read_buffer[t_read_pos],2);
			t_read_pos+=2;

			if(secEndBlock != (t_secEndBlock - 1))
				t_secEndBlock = 0xffff;
		}

		if((t_secEndBlock != 0xffff) && t_read_pos >= 0x200) {
			// more than one block
			//fseek(input_file,read_pos + 0x200 + 9, SEEK_SET);
			fseek(input_file,read_pos + 0x200, SEEK_SET);
			read(read_buffer,0x200);
			if( crypt_gps_id != 0 )
				xor_data(read_buffer,0x200,crypt_gps_id);

#ifdef _WIN32
			if( c_fish) {
				memset(f_buffer,0,sizeof f_buffer);
				memcpy(f_buffer,read_buffer,0x200);
				fish->Decode((unsigned char*)f_buffer,(unsigned char*)f_buffer,sizeof f_buffer);
				memcpy(read_buffer,f_buffer,0x200);
			}
#endif

			memcpy(t_file_type,&read_buffer[9],3);
			//read(t_file_type, 3);
			if( memcmp(file_type,t_file_type,3) == 0 ) {
				read_pos += 0x200;
				t_read_pos = 32;
				//fseek(input_file,20,SEEK_CUR);
			} else {
				t_end = true;
				read_pos += 0x200;
			}
		} else {
			t_end = true;
			read_pos += 0x200;
		}
	}

	if( !memcmp(file_type,"TRE",3) )		new_file_type = imgTRE;
	else if( !memcmp(file_type,"RGN",3) )		new_file_type = imgRGN;
	else if( !memcmp(file_type,"LBL",3) )		new_file_type = imgLBL;
	else if( !memcmp(file_type,"NET",3) )		new_file_type = imgNET;
	else if( !memcmp(file_type,"NOD",3) )		new_file_type = imgNOD;
	else if( !memcmp(file_type,"DEM",3) )		new_file_type = imgDEM;
	else if( !memcmp(file_type,"MDR",3) )		new_file_type = imgMDR;
	else if( !memcmp(file_type,"   ",3) )		new_file_type = imgFAT;
	else if( !memcmp(file_type,"MPS",3) )		new_file_type = imgMPS;
	else if( !memcmp(file_type,"SRT",3) )		new_file_type = imgSRT;
	else if( !memcmp(file_type,"TYP",3) )		new_file_type = imgTYP;
	else if( !memcmp(file_type,"GMP",3) )		new_file_type = imgGMP;

	if( gps_file == false && strlen(mps_file) && new_file_type == imgMPS ) {
		//create MPS file on disc
		FILE*	temp_mps_file;
		int		t_len = file_len;

		internal_file new_file(xor_value,img_file_name,file_name,new_file_type,file_len);
		new_file.set_offset(secStartBlock * block_size + crypt_start);
#ifdef _WIN32
		if( c_fish ) strncpy(new_file.password,f_password,255);
#endif

		temp_mps_file = fopen(mps_file,"w+b");		
		fseek(input_file,new_file.get_offset(), SEEK_SET);

		while( t_len > 0x200 ) {
			read(read_buffer,0x200);
			if( crypt_gps_id != 0 )
				xor_data(read_buffer,0x200,crypt_gps_id);
#ifdef _WIN32
			if( c_fish) {
				memset(f_buffer,0,sizeof f_buffer);
				memcpy(f_buffer,read_buffer,0x200);
				fish->Decode((unsigned char*)f_buffer,(unsigned char*)f_buffer,sizeof f_buffer);
				memcpy(read_buffer,f_buffer,0x200);
			}
#endif

			fwrite(read_buffer,1,0x200,temp_mps_file);
			t_len-=0x200;
		}
		read(read_buffer,t_len);
		if( crypt_gps_id != 0 )
			xor_data(read_buffer,t_len,crypt_gps_id);
#ifdef _WIN32
		if( c_fish) {
			memset(f_buffer,0,sizeof f_buffer);
			memcpy(f_buffer,read_buffer,t_len);
			fish->Decode((unsigned char*)f_buffer,(unsigned char*)f_buffer,sizeof f_buffer);
			memcpy(read_buffer,f_buffer,t_len);
		}
#endif
		fwrite(read_buffer,1,t_len,temp_mps_file);

		fclose(temp_mps_file);
	}

	if( (gps_file == false && (new_file_type != imgFAT && (new_file_type != imgMPS || crypt_start > 0 ) /*&& new_file_type != imgSRT*/ )) ||
		(gps_file == true && (new_file_type != imgFAT /*&& new_file_type != imgSRT*/ )) ) {
			bool process_file = false;

			//warunek - jesli do zaladowania tylko wybrany plik z IMG...
			if( selected_file == NULL )
				process_file = true;
			else if( strlen(selected_file) == 0 || (strlen(selected_file) > 0 && strcmp(selected_file,file_name) == 0) )
				process_file = true;

			if( process_file ) {
				string t_img_file_name = img_file_name;

				if( gps_file == true ) {
					t_img_file_name = "GPS:";
					t_img_file_name+= string(file_name);
				}

				if( selected_file != NULL && selected_file_rename != NULL ) {
					if( strlen(selected_file) )
						strncpy(file_name,selected_file_rename,8);
				}

				internal_file new_file(xor_value,t_img_file_name.c_str(),file_name,new_file_type,file_len);
				new_file.set_offset(secStartBlock * block_size + crypt_start);
				new_file.set_crypt_gps_id(crypt_gps_id);
				new_file.product_id = current_product_id;
#ifdef _WIN32
				if( c_fish ) strncpy(new_file.password,f_password,255);
#endif
				if( new_file_type == imgTRE ) {
					string file_id;
					int tre_id;
					int tre_file_id;
					int tre_header_size;
					char* end;
					if( file_name[0] == 'I' ) {
						file_id = file_name;
						file_id[0] = 'x';
						file_id = "0" + file_id;
						tre_id = strtol(file_id.c_str(), &end, 0);
					} else {
						tre_id = strtol(file_name, &end, 10);
					}
					tre_file_id = tre_id;

					fseek(input_file,new_file.get_offset(), SEEK_SET);
					read(&znak, 1);
					if( crypt_gps_id != 0 )
						xor_data(((char*)&znak),1,crypt_gps_id);
#ifdef _WIN32
					if( c_fish) {
						memset(f_buffer,0,sizeof f_buffer);
						f_buffer[0] = znak;
						fish->Decode((unsigned char*)f_buffer,(unsigned char*)f_buffer,sizeof f_buffer);
						znak = f_buffer[0];
					}
#endif

					tre_header_size = znak;
					if(znak >= 0x78) {
						fseek(input_file,new_file.get_offset(), SEEK_SET);
						read(read_buffer,0x80);

#ifdef _WIN32
						if( c_fish) {
							memset(f_buffer,0,sizeof f_buffer);
							memcpy(f_buffer,read_buffer,0x200);
							fish->Decode((unsigned char*)f_buffer,(unsigned char*)f_buffer,sizeof f_buffer);
							memcpy(read_buffer,f_buffer,0x200);
						}
#endif

						memcpy(&tre_id,&read_buffer[0x74],4);

						//fseek(input_file,new_file.get_offset() + 0x74, SEEK_SET);
						//read(&tre_id, 4);
						if( crypt_gps_id != 0 )
							xor_data(((char*)(&tre_id)),4,crypt_gps_id);
					}
					new_file.set_tre(tre_id,tre_file_id,tre_header_size,map_name);

					//update MPS
					//MPS.add_tre_file(new_file);
				}
				if( crypt_start > 0 )
					new_file.set_self_exe();
				files.push_back(new_file);
			}
	}
	return new_file_type;
}

void IMG::add_keys(vector<string> key_list) {
	MPS.add_keys(key_list);
}

void IMG::disable_mps() { 
	mps_enable = false; 
};

bool IMG::operator()(const internal_file e1,const internal_file e2) {
	return e2.file_start > e1.file_start;
}

void IMG::delete_GMP() {
	for( file_i = files.begin(); file_i != files.end(); file_i++ ) {
		if( (*file_i).file_type == imgGMP) {
#ifdef _WIN32
			DeleteFile((*file_i).get_filename());
#else
			remove((*file_i).get_filename());
#endif
		}
	}
}

void IMG::calculate_final_GMP() {
	vector<internal_file>::iterator file_i;
	
	//1 - kazdy zestaw tworzy osobny GMP - TRE+RGN+LBL+NET+NOD
	//2 - ze zbioru plikow GMP utworzyc jeden IMG
	// IMG = MDR + SRT + TYP + lista GMP

	//wyszukaæ wszystkie pod-pliki o tej samej nazwie

	bool found;
	for( file_i = files.begin(); file_i != files.end(); ) {
		found = false;
		if( (*file_i).file_type == imgTRE ) {
			nt_files[(*file_i).get_internal_short_name()].tre_file = (*file_i);
			file_i = files.erase(file_i);
			found = true;
		}
		if( (*file_i).file_type == imgLBL ) {
			nt_files[(*file_i).get_internal_short_name()].lbl_file = (*file_i);
			file_i = files.erase(file_i);
			found = true;
		}
		if( (*file_i).file_type == imgRGN ) {
			nt_files[(*file_i).get_internal_short_name()].rgn_file = (*file_i);
			file_i = files.erase(file_i);
			found = true;
		}
		if( (*file_i).file_type == imgNET ) {
			nt_files[(*file_i).get_internal_short_name()].net_file = (*file_i);
			file_i = files.erase(file_i);
			found = true;
		}
		if( (*file_i).file_type == imgNOD ) {
			nt_files[(*file_i).get_internal_short_name()].nod_file = (*file_i);
			file_i = files.erase(file_i);
			found = true;
		}
		if( !found )
			file_i++;
	}

	if( nt_files.size() ) {
		string	gmp_filename;
		for( map<string,internal_file_NT>::iterator nt_file = nt_files.begin(); nt_file != nt_files.end(); nt_file++ ) {
			gmp_filename = create_GMP(nt_file);
			add_IMG_internal(gmp_filename.c_str(),NULL);

			files.back().product_id = (*nt_file).second.tre_file.product_id;
			files.back().region_name = (*nt_file).second.tre_file.region_name;
			files.back().TRE_map_name = (*nt_file).second.tre_file.TRE_map_name;
			files.back().TRE_file_name = (*nt_file).second.tre_file.TRE_file_name;
			files.back().TRE_id = (*nt_file).second.tre_file.TRE_id;
			//cout<<(*nt_file).second.tre_file.TRE_map_name;
		}
	}
}

string IMG::create_GMP(map<string,internal_file_NT>::iterator file_set) {
	//utworzenie fizycznego pliku na dysku - pliku GMP
	//SIM_interface	GMP_file();
	string		return_filename;
	time_t		timer;
	struct tm	*tblock;
	FILE*		output_file;
	timer = time(NULL);
	tblock = localtime(&timer);
	unsigned char	gmpHeader[0x35];
	unsigned int	gmpHeaderLen = 0x35;

	memset(gmpHeader,0,sizeof gmpHeader);

	unsigned char	imgMonth = tblock->tm_mon + 1;
	unsigned char	imgYear = tblock->tm_year /* - 100 */ ;
	unsigned short	imgfYear = tblock->tm_year + 1900;
	unsigned char	imgfMonth = tblock->tm_mon + 1;
	unsigned char	imgfDay = tblock->tm_mday;
	unsigned char	imgfTime[3];

	gmpHeader[0] = gmpHeaderLen;
	gmpHeader[0x0c] = 0x01;

	memcpy(&gmpHeader[0xe],&imgfYear,2);
	gmpHeader[0x10] = imgfMonth;
	gmpHeader[0x11] = imgfDay;

	gmpHeader[0x12] = tblock->tm_hour;
	gmpHeader[0x13] = tblock->tm_min;
	gmpHeader[0x14] = tblock->tm_sec;

	memcpy(&gmpHeader[0x2], "GARMIN GMP", 10);

	//start nag³ówków:
	//0x15 - ? moze wpisy copyright?? 
	//0x19 - TRE
	//0x1d - RGN
	//0x21 - LBL
	//0x25 - NET
	//0x29 - NOD
	//0x2d - DEM
	//0x31 - ?

	//wpisy copyright - 2 stringi - oryginalne to:
	//Copyright Garmin Ltd. or its subsidiaries. All rights reserved
	//Copying is expressly prohibited and may result in criminal charges and/or civil action being brought against you

	/*
	Muszê zrobiæ czytanie i modyfikacjê nag³ówków.
	*/

	//send_data(gps,buffer,0x1000 - 0x200);

	rgn_manager* _rgn = new rgn_manager(
		(*file_set).second.rgn_file.file_name,
		(unsigned int)(*file_set).second.rgn_file.internal_offset,
		imgRGN,
		(unsigned char)(*file_set).second.rgn_file.get_xor(),
		(unsigned int)(*file_set).second.rgn_file.size);
	_rgn->read_header();

	tre_manager* _tre = new tre_manager(
		(*file_set).second.tre_file.file_name,
		(unsigned int)(*file_set).second.tre_file.internal_offset,
		imgTRE,
		(unsigned char)(*file_set).second.tre_file.get_xor(),
		(unsigned int)(*file_set).second.tre_file.size);
	_tre->read_header();

	lbl_manager* _lbl = new lbl_manager(
		(*file_set).second.lbl_file.file_name,
		(unsigned int)(*file_set).second.lbl_file.internal_offset,
		imgLBL,
		(unsigned char)(*file_set).second.lbl_file.get_xor(),
		(unsigned int)(*file_set).second.lbl_file.size);
	_lbl->read_header();


	net_manager* _net = NULL;
	if( (*file_set).second.net_file.file_name.size() ) {
		_net = new net_manager(
		(*file_set).second.net_file.file_name,
		(unsigned int)(*file_set).second.net_file.internal_offset,
		imgNET,
		(unsigned char)(*file_set).second.net_file.get_xor(),
		(unsigned int)(*file_set).second.net_file.size);
		_net->read_header();
	}

	nod_manager* _nod = NULL;
	if( (*file_set).second.nod_file.file_name.size() && (*file_set).second.net_file.file_name.size() ) {
		_nod = new nod_manager(
		(*file_set).second.nod_file.file_name,
		(unsigned int)(*file_set).second.nod_file.internal_offset,
		imgNOD,
		(unsigned char)(*file_set).second.nod_file.get_xor(),
		(unsigned int)(*file_set).second.nod_file.size);
		_nod->read_header();
	}

	return_filename = ((*file_set).second.tre_file.get_internal_short_name() + string(".GMP"));
	output_file = fopen( return_filename.c_str(),"wb");

	_tre->set_output_file(output_file);
	_lbl->set_output_file(output_file);
	_rgn->set_output_file(output_file);
	if( _net )
		_net->set_output_file(output_file);
	if( _nod )
		_nod->set_output_file(output_file);

	_tre->set_header_offset(0x3f);
	_rgn->set_header_offset(_tre->get_header_end());
	_lbl->set_header_offset(_rgn->get_header_end());
	if( _net )
		_net->set_header_offset(_lbl->get_header_end());
	if( _nod )
		_nod->set_header_offset(_net->get_header_end());
	
	unsigned int _header_end = _lbl->get_header_end();
	if( _net )
		_header_end = _net->get_header_end();
	if( _nod )
		_header_end = _nod->get_header_end();

	_tre->set_data_start_offset(_header_end);
	_rgn->set_data_start_offset(_tre->get_data_end_offset());
	_lbl->set_data_start_offset(_rgn->get_data_end_offset());
	if( _net )
		_net->set_data_start_offset(_lbl->get_data_end_offset());
	if( _nod )
		_nod->set_data_start_offset(_net->get_data_end_offset());

	//0x19 - TRE
	//0x1d - RGN
	//0x21 - LBL
	//0x25 - NET
	//0x29 - NOD
	//0x2d - DEM
	//0x31 - ?
	{
		unsigned int _header_offset;
		_header_offset = _tre->get_header_begin();
		memcpy(&gmpHeader[0x19],&_header_offset,4);
		_header_offset = _rgn->get_header_begin();
		memcpy(&gmpHeader[0x1d],&_header_offset,4);
		_header_offset = _lbl->get_header_begin();
		memcpy(&gmpHeader[0x21],&_header_offset,4);

		if( _net ) {
			_header_offset = _net->get_header_begin();
			memcpy(&gmpHeader[0x25],&_header_offset,4);
		}

		if( _nod ) {
			_header_offset = _nod->get_header_begin();
			memcpy(&gmpHeader[0x29],&_header_offset,4);
		}
	}

	fwrite(gmpHeader,1,gmpHeaderLen,output_file);

	_tre->write_header();
	_tre->write_data();

	_lbl->write_header();
	_lbl->write_data();

	_rgn->write_header();
	_rgn->write_data();

	if( _net ) {
		_net->write_header();
		_net->write_data();
	}

	if( _nod ) {
		_nod->write_header();
		_nod->write_data();
	}

	fclose(output_file);
	delete _lbl;
	delete _tre;
	delete _rgn;
	if( _net )
		delete _net;
	if( _nod )
		delete _nod;

	return return_filename;
}

void IMG::calculate_final_IMG() {
	int max_file_size = 0;
	int last_fat_offset;
	vector<internal_file>::iterator file_i;
//	vector<internal_file>::iterator file_typ;
	vector<internal_file>::iterator file_mps;
	block_size = 512;
	int fat_entries = 0; //every entry is 0x200 bytes big
	int fat_temp;
	int final_fat_size = 0;
	int	maximum_address = 0;
	int force_block_size = 0;


	//add? sorting of files here
	//sort(files.begin(),files.end(),IMG());

	//add all TRE files to MPS
	
//	typ_enable = false;
//	file_typ = files.end();
	for( file_i = files.begin(); file_i != files.end(); file_i++ ) {
		if( (*file_i).file_type == imgMPS ) {
			file_i = files.erase(file_i);
			if( file_i == files.end() )
				break;
		}
		if( (*file_i).file_type == imgTRE || (*file_i).file_type == imgGMP )
			MPS.add_tre_file((*file_i));

	}

	//jeszcze raz iterator do file TYP - w przypadku gdyby bylo ich wiecej erase zmienia...
	last_fat_entry = 0;

	while(true) {
		if( long_IMG_header == false )
			last_fat_offset = 0x400;
		else
			last_fat_offset = 0x1000;
		fat_entries = 0; //every entry is 0x200 bytes big

		//checking sizes
		max_file_size = final_fat_size;

		for( file_i = files.begin(); file_i != files.end(); file_i++ ) {
			if( (*file_i).get_file_size() > max_file_size )
				max_file_size = (*file_i).get_file_size();
		}
		if( mps_enable ) {
			if( MPS.get_file_size() > max_file_size )
				max_file_size = MPS.get_file_size();
		}

		//the best block size
		if( max_file_size > 0x67000 )
			block_size = 1024;
		if( max_file_size > 0x240000 )
			block_size = 2048;
		//if( max_file_size > 0x4E3800 )
		//	block_size = 4096;

		if( force_block_size > block_size )
			block_size = force_block_size;

		//set number of blocks each file has to occupy
		for( file_i = files.begin(); file_i != files.end(); file_i++ ) {
			(*file_i).set_blocks(block_size);

			fat_temp = (*file_i).get_blocks();
			while( fat_temp > 0 ) {
				fat_entries++;
				fat_temp -=240;
			}
		}
		//MPS
		if( mps_enable ) {
			MPS.set_blocks(block_size);
			fat_temp = MPS.get_blocks();
			while( fat_temp > 0 ) {
				fat_entries++;
				fat_temp -=240;
			}
		}

		//calculate how many blocks for the header
		fat_temp = (0x400 + fat_entries * 0x200);
		if( long_IMG_header )
			fat_temp = (0x1000 + fat_entries * 0x200);
			
		{
			float f_blocks = float(fat_temp) / float(block_size);
			int fat_blocks = int(f_blocks);
			if( (f_blocks - int(f_blocks)) > 0 )
				fat_blocks++;
			fat_temp = fat_blocks;
		}

		while( fat_temp > 0 ) {
			fat_entries++;
			fat_temp -=240;
		}

		//now can calculate how many blocks for the header
		internal_file IMG_header(0,"","        ",imgIMG,fat_entries * 0x200 + (long_IMG_header ? 0x1200 : 0x400));
		IMG_header.set_blocks(block_size);

		//add IMG header
		files.insert(files.begin(),IMG_header);

		if( mps_enable ) {
			files.push_back(MPS);
			file_mps = files.end()-1;
		}

		//Now all files are at their positions - can build the FAT table
		//build the FAT table here - what mean - let every file knows at what file position
		//it's FAT starts and ends as well as it knows what is the begin and end address
		fat_temp = 0;
		//typ_enable = false;

		//Order in FAT:
		//1 - imgIMG
		//2 - if exist - imgTYP
		//3 - the rest

		//Order in FS
		//1 - imgIMG
		//2 - data
		//3 - MPS
		//4 - TYP

		//Set order for FAT
		files_fat.clear();
		/*
		for( file_i = files.begin(); file_i != files.end(); file_i++ ) {
			if( (*file_i).file_type == imgTYP ) {
				typ_enable = true;
				file_typ = file_i;
			}
		}*/
		files_fat.push_back( &(*files.begin()) );
		//if( typ_enable )
		//	files_fat.push_back( &(*file_typ) );
		for( file_i = files.begin()+1; file_i != files.end(); file_i++ ) {
		//	if( (*file_i).file_type != imgTYP )
			files_fat.push_back( &(*file_i) );
		}
		for( file_f = files_fat.begin(); file_f != files_fat.end(); file_f++ ) {
			(*file_f)->set_fat_position(last_fat_offset);
			last_fat_offset = (*file_f)->fat_entry_offset_end;
		}

		//Set order for FILES in FS
		for( file_i = files.begin(); file_i != files.end(); file_i++ ) {
			if( /*(*file_i).file_type != imgTYP &&*/ (*file_i).file_type != imgMPS ) {
				fat_temp = (*file_i).set_file_position(fat_temp,block_size);
				maximum_address = (*file_i).file_end;
			}
		}
		if( mps_enable ) {
			fat_temp = (*file_mps).set_file_position(fat_temp,block_size);
			last_fat_offset = (*file_mps).fat_entry_offset_end;
			maximum_address = (*file_mps).file_end;
		}
		/*
		if( typ_enable ) {
			fat_temp = (*file_typ).set_file_position(fat_temp,block_size);
			last_fat_offset = (*file_typ).fat_entry_offset_end;
			maximum_address = (*file_typ).file_end;
		}*/

		last_fat_entry = fat_temp;

		//check for block size regarding size of FAT!
		//cout<<"FAT size :"<<int(last_fat_offset)<<endl;
		//cout<<"Block    :"<<int(block_size)<<endl;
		final_fat_size = last_fat_offset;
		//if( mps_enable )
		//	maximum_address = MPS.file_end;

		//check the maximum addressable size!
		if( block_size == 512 ) {
			if( maximum_address / 512 < 0xfffe ) //&& final_fat_size < 0x67000 )
				break;
		}
		if( block_size == 1024 ) {
			if( maximum_address / 1024 < 0xfffe ) //&& final_fat_size < 0x200000 )
				break;
		}
		if( block_size == 2048 ) {
			if( maximum_address / 2048 < 0xfffe ) //&& final_fat_size < 0x400000 )
				break;
		}
		if( block_size == 4096 ) {
			if( maximum_address / 4096 < 0xfffe ) //&& final_fat_size < 0x800000 )
				break;
		}
		if( block_size >= 8192 ) 
			break;

		if( maximum_address / 512 >= 0xfffe)
			force_block_size = 1024;
		if( maximum_address / 1024 >= 0xfffe)
			force_block_size = 2048;
		if( maximum_address / 2048 >= 0xfffe)
			force_block_size = 4096;
		if( maximum_address / 4096 >= 0xfffe)
			force_block_size = 8192;
		if( maximum_address / 8192 >= 0xfffe)
			force_block_size = 16384;
		if( maximum_address / 16384 >= 0xfffe)
			force_block_size = 32768;
		if( maximum_address / 32768 >= 0xfffe)
			force_block_size = 65536;
	
		//remove the IMG header & MPS file for recalculation!
		files.erase(files.begin());
		for( file_i = files.begin(); file_i != files.end(); file_i++ ) {
			if( (*file_i).file_type == imgMPS ) {
				file_i = files.erase(file_i);
				break;
			}
		}
	}

	//sort(files.begin(),files.end(),IMG());
	size_of_final_img = fat_temp * block_size;
	sent_bytes_img = 0;
}

void IMG::after_upload() {
	if( files.size() ) 
		if( files.front().file_type == imgIMG )
			files.erase(files.begin());
	MPS.clear();
}

bool IMG::send_data(transmission_gps* gps,char* buffer,int size) {
	bool	result = true;
	int		sent_pos = 0;
	int		to_sent = 0;
	try {
		if( gps->get_preffered_chunk_size() >= size ) {
			result = gps->send_map_chunk(buffer,size);
			sent_bytes_img += size;
		} else {
			while( sent_pos < size && result ) {
				to_sent = (size - sent_pos) > gps->get_preffered_chunk_size() ? gps->get_preffered_chunk_size() : (size - sent_pos);
				result = gps->send_map_chunk(&(buffer[sent_pos]),to_sent);
				sent_pos += to_sent;
			}
			sent_bytes_img += size;
		}
	} catch(...) {
		result = false;
	}
	return result;	
}

void IMG::create_header(char* imgHeader,const char* copyright) {
	time_t		timer;
	struct tm	*tblock;

	timer = time(NULL);
	tblock = localtime(&timer);
	int				total_size;

	unsigned char	imgMonth = tblock->tm_mon + 1;
	unsigned char	imgYear = tblock->tm_year /* - 100 */ ;
	unsigned short	imgfYear = tblock->tm_year + 1900;
	unsigned char	imgfMonth = tblock->tm_mon + 1;
	unsigned char	imgfDay = tblock->tm_mday;
	unsigned char	imgfTime[3];

	imgfTime[0] = tblock->tm_hour;
	imgfTime[1] = tblock->tm_min;
	imgfTime[2] = tblock->tm_sec;

	for(int a = 0; a < 0x200; a++)
		imgHeader[a] = 0;	// setmem(imgHeader,0x200,0);
	imgHeader[0x0e] = 1;

	memcpy(&imgHeader[0x10], "\x44\x53\x4B\x49\x4D\x47\x00\x02", 8);
	memcpy(&imgHeader[0x0a], &imgMonth, 1);
	memcpy(&imgHeader[0x0b], &imgYear, 1);

	memcpy(&imgHeader[0x39], &imgfYear, 2);
	memcpy(&imgHeader[0x3b], &imgfMonth, 1);
	memcpy(&imgHeader[0x3c], &imgfDay, 1);
	memcpy(&imgHeader[0x3d], imgfTime, 3);

	//rozmiar naglowka
	if( long_IMG_header )
		imgHeader[0x40] = 8;
	else
		imgHeader[0x40] = 2;
	memcpy(&imgHeader[0x41], "GARMIN", 6);

	if( long_IMG_header == false ) {

		if( last_fat_entry * block_size > 1024 * 1024 * 120 ) {
			memcpy(&imgHeader[0x18], c_imgDskimg2_1g, 8);
			memcpy(&imgHeader[0x5d], c_imgDskimg5_1g, 8);
			memcpy(&imgHeader[0x1c0], c_imgDskimg7_1g, 13);
		} else {
			memcpy(&imgHeader[0x18], c_imgDskimg2_128m, 8);
			memcpy(&imgHeader[0x5d], c_imgDskimg5_128m, 8);
			memcpy(&imgHeader[0x1c0], c_imgDskimg7_128m, 13);
		}
		if(block_size == 512 ) imgHeader[0x62] = 0;
		if(block_size == 1024 ) imgHeader[0x62] = 1;
		if(block_size == 2048 ) imgHeader[0x62] = 2;
		if(block_size == 4096 ) imgHeader[0x62] = 3;
		if(block_size == 8192 ) imgHeader[0x62] = 4;
		if(block_size == 16384) imgHeader[0x62] = 5;
		if(block_size == 32768) imgHeader[0x62] = 6;
	} else {
		if(block_size == 512) {
			memcpy(&imgHeader[0x18], c_imgDskimg2_long_512, 8);
			memcpy(&imgHeader[0x5d], c_imgDskimg5_long_512, 8);
			memcpy(&imgHeader[0x1c0], c_imgDskimg7_long_512, 13);
		} else if(block_size == 1024) {
			memcpy(&imgHeader[0x18], c_imgDskimg2_long_1024, 8);
			memcpy(&imgHeader[0x5d], c_imgDskimg5_long_1024, 8);
			memcpy(&imgHeader[0x1c0], c_imgDskimg7_long_1024, 13);
		} else if(block_size == 2048) {
			memcpy(&imgHeader[0x18], c_imgDskimg2_long_2048, 8);
			memcpy(&imgHeader[0x5d], c_imgDskimg5_long_2048, 8);
			memcpy(&imgHeader[0x1c0], c_imgDskimg7_long_2048, 13);
		} else if(block_size == 4096) {
			memcpy(&imgHeader[0x18], c_imgDskimg2_long_4096, 8);
			memcpy(&imgHeader[0x5d], c_imgDskimg5_long_4096, 8);
			memcpy(&imgHeader[0x1c0], c_imgDskimg7_long_4096, 13);
		} else if(block_size == 8192) {
			memcpy(&imgHeader[0x18], c_imgDskimg2_long_8192, 8);
			memcpy(&imgHeader[0x5d], c_imgDskimg5_long_8192, 8);
			memcpy(&imgHeader[0x1c0], c_imgDskimg7_long_8192, 13);
		}
	}

	if( long_IMG_header ) {
		memcpy(&imgHeader[0x63],&last_fat_entry,2);
		total_size = last_fat_entry * block_size;
		total_size /= 0x200;
		//memcpy(&imgHeader[0x1ca],&total_size,4);
	}

	// 123456789012345678901234567890
	char	part1[20];
	char	part2[30];

	memset(part1,0x20,20);
	memset(part2,0x20,30);

	strncpy(part1,copyright,20);
	if( strlen(copyright)>20 )
		strncpy(part2,&copyright[20],30);

	memcpy(&imgHeader[0x49], part1, 20);
	memcpy(&imgHeader[0x65], part2, 30);
	memcpy(&imgHeader[0x1fe], "\x55\xaa", 2);
}

void IMG::xor_data(char* buffer,int size,unsigned int gps_id) {
	if( gps_id == 0 )
		return;
	int t=0;
	char id[4];
	int p;

	memcpy(id,&gps_id,4);

	for( p = 0; p < size; ++p ) {
		buffer[p] = buffer[p] ^ id[t];
		t++;
		if( t > 3 ) {
			t=0;
		}
	}
}

bool IMG::send_raw_data(transmission_gps* gps,void (progress::*calculate)(int,int,bool,bool),bool crypted,unsigned int gps_id) {
	char		buffer[0x1001];
	size_t		t_read;
	progress	instance;

	//1. send only FIRST file
	input_file = fopen(raw_file.c_str(),"rb");
	if( input_file == NULL )
		return false;

	if( raw_start_data > 0 )
		fseek(input_file,raw_start_data,SEEK_SET);
	fread(&xor_value,1,1,input_file);
	if( gps_id != 0 ) xor_value = 0;

	fseek(input_file,raw_start_data + 0x10,SEEK_SET);
	read(buffer,16);
	
	if( gps_id != 0 )	IMG::xor_data(buffer,16,gps_id);
	//xor_data(buffer,16,gps_id);
	/*
	if( strncmp(buffer,"DSKIMG",6) ) {
		fclose(input_file);
		return false;
	}*/

	sent_bytes_img = 0;
	if( raw_start_data > 0 )
		size_of_final_img = raw_len_data;
	else {
		fseek(input_file,0,SEEK_END);
		size_of_final_img = ftell(input_file);
	}

	fseek(input_file,raw_start_data,SEEK_SET);

	(instance.*calculate)(sent_bytes_img,size_of_final_img,false,true);

	while( (t_read = fread(buffer,1,0x1000,input_file)) > 0 ) {
		if( gps_id != 0 )	IMG::xor_data(buffer,0x1000,gps_id);
		//xor_data(buffer,0x1000,gps_id);
		send_data(gps,buffer,(int)t_read);
		(instance.*calculate)(sent_bytes_img,size_of_final_img,false,true);
	}

	fclose(input_file);
	return true;
}

void IMG::reset_download_files() {
	file_i = files.begin();
}

internal_file* IMG::next_download_file() {
	internal_file* t_ret;
	if( file_i < files.end() ) {
		t_ret = &(*file_i);
		file_i++;
		return t_ret;
	}
	return NULL;
}

bool IMG::send_data(transmission_gps* gps,void (progress::*calculate)(int,int,bool,bool)) {
	//1. send imgIMG
	//2. send FAT tables
	//3. send Files
	//4. send MSP
	//first file must be imgIMG
	bool	result = true;
	int		no_files = files.size();
	char	buffer[0x1000];
	int		to_send;
	progress	instance;

	file_i = files.begin();

	//create master file here!
	//send_data(gps,(*file_i).FAT,0x400);
	memset(buffer,0,0x1000);
	while( map_copyright.size() < 50 )
		map_copyright += " ";
	create_header(buffer,map_copyright.c_str());
	send_data(gps,buffer,0x200);

	if( long_IMG_header == false ) {
		memset(buffer,0,0x1000);
		send_data(gps,buffer,0x200);
	} else {
		memset(buffer,0,0x1000);
		send_data(gps,buffer,0x1000 - 0x200);
	}
	
	(instance.*calculate)(sent_bytes_img,size_of_final_img,false,true);

	//file_typ = files.end();
	/*for( file_i = files.begin(); file_i != files.end(); file_i++ ) {
		if( (*file_i).file_type == imgTYP )
			file_typ = file_i;
	}*/

	//now FATs
	//if( file_typ != files.end() ) {
	//	while( (*file_typ).get_fat(sent_bytes_img,buffer) )
	//		send_data(gps,buffer,0x200);
	//}

	for( file_f = files_fat.begin(); file_f != files_fat.end(); file_f++ ) {
	//	if( file_i != file_typ ) {
		while( (*file_f)->get_fat(sent_bytes_img,buffer) )
			send_data(gps,buffer,0x200);
	//	}
	}
	//if( mps_enable ) {
	//	while( MPS.get_fat(sent_bytes_img,buffer) )
	//		send_data(gps,buffer,0x200);
	//}
	(instance.*calculate)(sent_bytes_img,size_of_final_img,false,true);
	
	//fill FAT to full block size!
	file_i = files.begin();
	memset(buffer,0xff,0x1000);

//	if( long_IMG_header == false ) {

	while( (*file_i).file_end != sent_bytes_img ) {
		result = send_data(gps,buffer,0x200);
		(instance.*calculate)(sent_bytes_img,size_of_final_img,false,true);
		if( !result )
			return result;
	}
	no_files--;
//	}
	memset(buffer,0,0x1000);

	//and now the files
	while (true ) {
		for( file_i = files.begin()+1; file_i != files.end(); file_i++ ) {

			if( sent_bytes_img == (*file_i).file_start )
				no_files--;

			if( (*file_i).file_type == imgMPS ) {
				MPS.file_start = (*file_i).file_start;
				MPS.file_end = (*file_i).file_end;
				while( (to_send = MPS.get_file(sent_bytes_img,buffer)) > 0 ) {
					result = send_data(gps,buffer,to_send);
					(instance.*calculate)(sent_bytes_img,size_of_final_img,false,true);
				}
			} else {
				while( (to_send = (*file_i).get_file(sent_bytes_img,buffer)) > 0 ) {
					result = send_data(gps,buffer,to_send);
					(instance.*calculate)(sent_bytes_img,size_of_final_img,false,true);
				}
			}
			if( !result )
				return result;
		}

		if( no_files <= 0 )
			break;
	}
	if( !result )
		return result;

	return result;
}
