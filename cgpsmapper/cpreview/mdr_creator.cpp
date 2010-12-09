/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

#include <time.h>
#include <boost/tokenizer.hpp>
#include <boost/progress.hpp>
#include <string.h>
#include "mdr_creator.h"
#include "compare.h"

#define multipart 0

#define MDR1_REC_SIZE 0x46
#define MRD8_INDEX_CONST 100
#define MRD12_INDEX_CONST 100

//MDR5
//	LBL_string* mdr15; //trzeba 'zatrudnic' lbl managera
//  unsigned int lbl4, lbl1; //indexy z docelowego pliku IMG
//select * from t43_miasta order by t_text;
// lbl4->t_index
// lbl1->lbl1
//AddMDR5(MDR1_element* mdr1,int rgnType, int rgnType2,unsigned int RGNidx, unsigned int TREidx,LBL_4* lbl4, LBL_3* lbl3, LBL_2* lbl2) //miasta
/*
create table mdr15 (t_text text,offset int);
create table mdr14 (map_id int,lbl4 int,mdr15_rowid int);
create table mdr13 (map_id int,lbl3 int,mdr14_id int,mdr15_rowid int);

*/

typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

using namespace MDR_CREATOR;

MDR_Header::MDR_Header() {
	time_t timer;
	struct tm *tblock;
	timer = time(NULL);
	tblock = localtime(&timer);

	mdrHeaderLength = 0x238;
	//mdrHeaderLength = 0x2bc;
	memcpy(&mdrName,"GARMIN MDR",10);
	mdrMdr0[0] = 1;
	mdrMdr0[1] = 0;
	mdrYear = tblock->tm_year - 100;
	mdrMonth = tblock->tm_mon;
	mdrDay = tblock->tm_mday;
	mdrTime[0] = tblock->tm_hour;
	mdrTime[1] = tblock->tm_min;
	mdrTime[2] = tblock->tm_sec;

	memset(&mdr_header,0,sizeof(mdr_header));

	//nieznane....
	mdr_header.mdr5_flag = 0;
#if multipart == 1
	mdr_header.mdr7_flag = 0x863; //??
#else
	mdr_header.mdr7_flag = 0x43; //??
#endif
	mdr_header.mdr8_flag = 0;//0x040a; //1024 w indeksie, 4 znaki
	mdr_header.mdr12_flag = 0;//0x040a; //1024 w indeksie

	mdr_header.mdr6_flag = 4;
	mdr_header.mdr2_rec_size = 2;
	mdr_header.mdr3_rec_size = 2;
	mdr_header.mdr4_rec_size = 3;
	mdr_header.mdr9_rec_size = 4;

	mdr_header.mdr1_flag = 1;
	mdr_header.mdr1_rec_size = 8;
	mdr_header.mdr1_offset = mdrHeaderLength;


	mdr_header.mdr20_flag = 0x00008800;
	mdr_header.mdr21_flag = 0x00011800;
	mdr_header.mdr22_flag = 0x00011000;
	mdr_header.mdr23_flag = 0;
	mdr_header.mdr24_flag = 0;
	mdr_header.mdr25_flag = 0;
	mdr_header.mdr26_flag = 0;
	mdr_header.mdr27_flag = 0;
	mdr_header.mdr28_flag = 7;
	mdr_header.mdr29_flag = 0xf;

}

void MDR_Header::WriteIMGHeader(xor_fstream* file,unsigned long size,const char* internal_file_name) {
	unsigned char	imgHeader[0x200];
	unsigned short	mdr_start;

	unsigned char	c_imgDskimg2_512[] = { 0x04, 0x00, 0x10, 0x00, 0x10, 0x00, 0x00, 0x00 };
	unsigned char	c_imgDskimg5_512[] = { 0x10, 0x00, 0x04, 0x00, 0x09, 0x00, 0x00, 0x04 };
	unsigned char	c_imgDskimg7_512[] = { 0x01, 0x00, 0x00, 0x0f, 0x04, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04 };

	unsigned char	c_imgDskimg2_1024[] = { 0x10, 0x00, 0x10, 0x00, 0x1c, 0x00, 0x00, 0x00 };
	unsigned char	c_imgDskimg5_1024[] = { 0x10, 0x00, 0x10, 0x00, 0x09, 0x01, 0x00, 0x0e };
	unsigned char	c_imgDskimg7_1024[] = { 0x01, 0x00, 0x00, 0x0f, 0x10, 0x1b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c };

	unsigned char	c_imgDskimg2_2048[] = { 0x10, 0x00, 0x10, 0x00,0x40, 0x00, 0x00, 0x00 };     // 0x18 - 0x1f
	unsigned char	c_imgDskimg5_2048[] = { 0x10, 0x00, 0x10, 0x00,0x09, 0x02, 0x00, 0x10 };     // 0x5d - 0x64
	unsigned char	c_imgDskimg7_2048[] = { 0x01, 0x00, 0x00, 0x0f,0x10, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40 };     // 0x1c0

	int blockSize = 512;
	if( size > 0x67000 )
		blockSize = 1024;
	if( size > 0x200000 )
		blockSize = 2048; //???? przetestowac kiedy 2kb blok!!

	time_t		timer;
	struct tm	*tblock;

	timer = time(NULL);
	tblock = localtime(&timer);

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned char	imgMonth = tblock->tm_mon + 1;
	unsigned char	imgYear = tblock->tm_year /* - 100 */ ;
	unsigned short	imgfYear = tblock->tm_year + 1900;
	unsigned char	imgfMonth = tblock->tm_mon + 1;
	unsigned char	imgfDay = tblock->tm_mday;
	unsigned char	imgfTime[3];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	imgfTime[0] = tblock->tm_hour;
	imgfTime[1] = tblock->tm_min;
	imgfTime[2] = tblock->tm_sec;

	for(int a = 0; a < 0x200; a++)
		imgHeader[a] = 0;
	imgHeader[0x0e] = 1;

	memcpy(&imgHeader[0x10], "\x44\x53\x4B\x49\x4D\x47\x00\x02", 8);
	memcpy(&imgHeader[0x0a], &imgMonth, 1);
	memcpy(&imgHeader[0x0b], &imgYear, 1);

	memcpy(&imgHeader[0x39], &imgfYear, 2);
	memcpy(&imgHeader[0x3b], &imgfMonth, 1);
	memcpy(&imgHeader[0x3c], &imgfDay, 1);
	memcpy(&imgHeader[0x3d], imgfTime, 3);
	imgHeader[0x40] = 2;
	memcpy(&imgHeader[0x41], "GARMIN", 6);

	if(blockSize == 512) {
		memcpy(&imgHeader[0x18], c_imgDskimg2_512, 8);
		memcpy(&imgHeader[0x5d], c_imgDskimg5_512, 8);
		memcpy(&imgHeader[0x1c0], c_imgDskimg7_512, 12);
	}
	else if(blockSize == 1024) {
		memcpy(&imgHeader[0x18], c_imgDskimg2_1024, 8);
		memcpy(&imgHeader[0x5d], c_imgDskimg5_1024, 8);
		memcpy(&imgHeader[0x1c0], c_imgDskimg7_1024, 12);
	}else if(blockSize == 2048) {
		memcpy(&imgHeader[0x18], c_imgDskimg2_2048, 8);
		memcpy(&imgHeader[0x5d], c_imgDskimg5_2048, 8);
		memcpy(&imgHeader[0x1c0], c_imgDskimg7_2048, 12);
	}

	// 123456789012345678901234567890
	memcpy(&imgHeader[0x49], "Index file          ", 20);
	memcpy(&imgHeader[0x65], "                              ", 30);
	memcpy(&imgHeader[0x1fe], "\x55\xaa", 2);

	file->Write(imgHeader,0x200);

	//Tutaj FAT
	file->seekg(0x400,SEEK_SET);
	unsigned long master_fat_size = 2;
	unsigned short current_block = 0x0;
	unsigned short next_fat_number = 0;

	//wyznaczam ilosc ma master FAT - zalezny od ilosci FAT dla sekcji
	float t_size = float(size) / float(blockSize);

	master_fat_size++;
	while( t_size > 240 ) {
		master_fat_size++;
		t_size-=240;
	}

	master_fat_size = master_fat_size * 0x200 / blockSize;

	bool next_fat = WriteFAT(file, master_fat_size, "   ", "GPSMAPPE", current_block, next_fat_number, (master_fat_size + 1) * blockSize);
	while(next_fat) {
		master_fat_size -= 240;
		next_fat_number++;
		current_block--;
		next_fat = WriteFAT(file, master_fat_size, "   ", "GPSMAPPE", current_block, next_fat_number, 0);
	}

	//MDR FAT
	master_fat_size = static_cast<unsigned long>(float(size) / float(blockSize) + float(0.5));
	mdr_start = current_block;
	next_fat = WriteFAT(file, master_fat_size, "MDR", internal_file_name, current_block, next_fat_number, (master_fat_size + 1) * blockSize);
	while(next_fat) {
		master_fat_size -= 240;
		next_fat_number++;
		current_block--;
		next_fat = WriteFAT(file, master_fat_size, "MDR", internal_file_name, current_block, next_fat_number, 0);
	}

	file->seekg(current_block * blockSize-1,SEEK_SET);
	file->FillWrite(0,1);

	MDRfile_start = mdr_start * blockSize;
	file->seekg(MDRfile_start,SEEK_SET);
}

bool MDR_Header::WriteFAT(xor_fstream* file,
		unsigned long& size,
		const char *type,
		const char *imgid,
		unsigned short &start_block,
		unsigned short &next_fat_number,
		unsigned int sect_size) {

	unsigned char fat[0x204];
	int curr_block = 0;

	memset(fat, 0, 0x20);
	memset(&fat[0x20], 0xff, 0x200-0x20);
	memcpy(fat, "\x01\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20", 12);
	if(!strncmp(type, "   ", 3)) {
		fat[0x10] = 3;
	}
	else {
		memcpy(&fat[1], imgid, 8);
		memcpy(&fat[9], type, 3);
		fat[0x11] = static_cast<unsigned char>(next_fat_number);
	}

	memcpy(&fat[12], &sect_size, 4);

	while(curr_block <= static_cast<int>(size)) {
		memcpy(&fat[0x20 + curr_block * 2], &start_block, 2);
		curr_block++;
		start_block++;

		if(curr_block > 240) {
			file->Write(fat,0x200);
			return true;
		}
	}
	file->Write(fat,0x200);
	return false;
}

void MDR_Header::write1Value(xor_fstream* file,std::string sql,int rec_size) {
	sqlite3_stmt *ppStmt2;
	int rc = sqlite3_prepare_v2(getMDRbase(),sql.c_str(),-1,&ppStmt2,NULL);

	rc = sqlite3_step( ppStmt2 );
	if ( rc == SQLITE_ROW ) {
		do {
			file->WriteInt(sqlite3_column_int(ppStmt2,0),rec_size);
			rc = sqlite3_step( ppStmt2 );
		} while(rc == SQLITE_ROW);
		sqlite3_finalize( ppStmt2 );
	}

}

void MDR_Header::writeMDR1(xor_fstream* file) {
    sqlite3_stmt *ppStmt;
    sqlite3_stmt *ppStmt2;
	char	*s;

	//                                               0       1   2        3      4                 5                  6                 7                  8                9                   10                 11                  12                 13                  14                 15                  16                 17
	int rc = sqlite3_prepare_v2(getMDRbase(),"SELECT map_id, id, address, offset,MDR5_data_offset, MDR5_records_count,MDR6_data_offset, MDR6_records_count,MDR7_data_offset, MDR7_records_count,MDR11_data_offset, MDR11_records_count,MDR10_data_offset, MDR20_records_count,MDR20_data_offset, MDR21_records_count,MDR21_data_offset, MDR22_records_count,MDR22_data_offset, MDRx_records_count,MDRx_data_offset FROM MDR1 ORDER BY id;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {
			//zapisanie ID i rozmiaru MDR1 - zalezne od ilosci wszystkich referencji - ³atwe i szybkie do wyliczenia

			file->seekg( mdr_header.mdr1_offset + (sqlite3_column_int(ppStmt,1)-1)*8 , SEEK_SET );

			file->WriteInt(sqlite3_column_int(ppStmt,0),4); //IMG ID
			file->WriteInt(sqlite3_column_int(ppStmt,3),4); //offset
			
			file->seekg( sqlite3_column_int(ppStmt,3), SEEK_SET );
			mdr1_data.structSize = MDR1_REC_SIZE;

			mdr1_data.MDR5_data_offset = sqlite3_column_int(ppStmt,4);
			mdr1_data.MDR5_records_count = sqlite3_column_int(ppStmt,5);
			
			mdr1_data.MDR6_data_offset = sqlite3_column_int(ppStmt,6);
			mdr1_data.MDR6_records_count = sqlite3_column_int(ppStmt,7);

			mdr1_data.MDR7_data_offset = sqlite3_column_int(ppStmt,8);
			mdr1_data.MDR7_records_count = sqlite3_column_int(ppStmt,9);

			mdr1_data.MDR11_data_offset = sqlite3_column_int(ppStmt,10);
			mdr1_data.MDR11_records_count = sqlite3_column_int(ppStmt,11);
			mdr1_data.MDR10_data_offset = sqlite3_column_int(ppStmt,12);

			mdr1_data.MDR20_data_offset = sqlite3_column_int(ppStmt,14);
			mdr1_data.MDR20_records_count = sqlite3_column_int(ppStmt,13);

			mdr1_data.MDR21_data_offset = sqlite3_column_int(ppStmt,16);
			mdr1_data.MDR21_records_count = sqlite3_column_int(ppStmt,15);

			mdr1_data.MDR22_data_offset = sqlite3_column_int(ppStmt,18);
			mdr1_data.MDR22_records_count = sqlite3_column_int(ppStmt,17);

			mdr1_data.MDRx_data_offset = sqlite3_column_int(ppStmt,20);
			mdr1_data.MDRx_records_count = sqlite3_column_int(ppStmt,19);

			file->Write(&mdr1_data,mdr1_data.structSize);

			file->seekg( sqlite3_column_int(ppStmt,3) + mdr1_data.MDR11_data_offset, SEEK_SET );
			s = sqlite3_mprintf("SELECT id FROM MDR11 WHERE mdr1_id=%i ORDER BY id ;",sqlite3_column_int(ppStmt,1));
			write1Value(file,s,mdr11_ref_size);
			sqlite3_free(s);

			s = sqlite3_mprintf("select id from mdr10 where mdr11_id in (select id from mdr11 where mdr1_id=%i) order by id;",sqlite3_column_int(ppStmt,1));
			write1Value(file,s,mdr11_ref_size);
			sqlite3_free(s);

			s = sqlite3_mprintf("SELECT id FROM MDR7 WHERE mdr1_id=%i ORDER BY id ;",sqlite3_column_int(ppStmt,1));
			write1Value(file,s,mdr7_ref_size);
			sqlite3_free(s);

			s = sqlite3_mprintf("SELECT id FROM MDR5 WHERE mdr1_id=%i ORDER BY id ;",sqlite3_column_int(ppStmt,1));
			write1Value(file,s,mdr5_ref_size);
			sqlite3_free(s);

			s = sqlite3_mprintf("SELECT id FROM MDR6 WHERE mdr1_id=%i ORDER BY id;",sqlite3_column_int(ppStmt,1));
			write1Value(file,s,mdr6_ref_size);
			sqlite3_free(s);

			s = sqlite3_mprintf("SELECT id FROM MDR20 WHERE mdr1_id=%i ORDER BY id;",sqlite3_column_int(ppStmt,1));
			write1Value(file,s,mdr20_ref_size);
			sqlite3_free(s);

			s = sqlite3_mprintf("SELECT id FROM MDR21 WHERE mdr1_id=%i ORDER BY id;",sqlite3_column_int(ppStmt,1));
			write1Value(file,s,mdr21_ref_size);
			sqlite3_free(s);

			s = sqlite3_mprintf("SELECT id FROM MDR22 WHERE mdr1_id=%i ORDER BY id;",sqlite3_column_int(ppStmt,1));
			write1Value(file,s,mdr22_ref_size);
			sqlite3_free(s);
		}
		rc = sqlite3_step( ppStmt );
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );
}

void MDR_Header::writeMDR2(xor_fstream* file) {
}
void MDR_Header::writeMDR3(xor_fstream* file) {
}
void MDR_Header::writeMDR4(xor_fstream* file) {
	int		t_size = 0;
	sqlite3_stmt *ppStmt;

	file->seekg( mdr_header.mdr4_offset , SEEK_SET );

	int	rc = sqlite3_prepare_v2(getMDRbase(),"SELECT type,sub_type,level FROM MDR4 ORDER BY id;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {
			file->WriteInt(sqlite3_column_int(ppStmt,0),1);
			t_size += 1;

			file->WriteInt(sqlite3_column_int(ppStmt,2),1);
			t_size += 1;

			file->WriteInt(sqlite3_column_int(ppStmt,1),1);
			t_size += 1;
		}
		rc = sqlite3_step( ppStmt );
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );
	assert(t_size == mdr_header.mdr4_size);
}

void MDR_Header::writeMDR5(xor_fstream* file) {
	int		lbl15_index;
	int		mdr13_id,mdr14_id;
	int		t_size = 0;
	int		last_mdr20_id=-1;
	sqlite3_stmt *ppStmt;
	sqlite3_stmt *ppStmt2;
	unsigned long t_lbl1;

	file->seekg( mdr_header.mdr5_City_offset, SEEK_SET );

	int	rc = sqlite3_prepare_v2(getMDRbase(),"SELECT mdr1_id,id ,mdr15_id ,lbl4 ,lbl1 ,mdr13_id,mdr14_id,mdr20_id,uniq FROM MDR5 ORDER BY id;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {
			mdr13_id = -1;
			mdr14_id = -1;
			if( sqlite3_column_int(ppStmt,5) > -1 ) 
				mdr13_id = getSelectCount("SELECT t_index FROM MDR13 WHERE id = %i;",sqlite3_column_int(ppStmt,5));
			if( sqlite3_column_int(ppStmt,6) > -1 ) 
				mdr14_id = getSelectCount("SELECT id_4e FROM MDR14 WHERE id = %i;",sqlite3_column_int(ppStmt,6));

			lbl15_index = sqlite3_column_int(ppStmt,2);

			t_lbl1 = sqlite3_column_int(ppStmt,4);
			if( !sqlite3_column_int(ppStmt,8) )
				t_lbl1 |= 0x800000;
			
			file->WriteInt(sqlite3_column_int(ppStmt,0),mdr1_ref_size); //mdr1_id
			t_size += mdr1_ref_size;
			file->WriteInt(sqlite3_column_int(ppStmt,3),(mdr_header.mdr5_flag & 3)+1);
			t_size += ((mdr_header.mdr5_flag & 3)+1);
			file->WriteInt(t_lbl1,3); //
			t_size += 3;
			if( mdr_header.mdr5_flag & 0x04 ) {
				if( mdr13_id > -1 ) {
					file->WriteInt(mdr13_id,2);
					t_size += 2;
				} else if( mdr14_id > -1 ) {
					file->WriteInt(mdr14_id | 0x4000,2);
					t_size += 2;
				} else {
					file->WriteInt(0,2);
					t_size += 2;
				}
			}	
			if( mdr_header.mdr5_flag & 0x08 ) {
				file->WriteInt(lbl15_index,mdr15_ref_size);
				t_size += mdr15_ref_size;
			}

			if( sqlite3_column_int(ppStmt,7) > -1 )
				if( last_mdr20_id > sqlite3_column_int(ppStmt,7) ) {
					cout<<"Error indexing - MDR20 : "<<last_mdr20_id<<","<<sqlite3_column_int(ppStmt,7)<<endl;
				}

			if( mdr_header.mdr5_flag & 0x100 ) {
				if( sqlite3_column_int(ppStmt,7) > -1 ) {
					last_mdr20_id = sqlite3_column_int(ppStmt,7);
					file->WriteInt(sqlite3_column_int(ppStmt,7),mdr20_ref_size);
				} else
					file->WriteInt(0,mdr20_ref_size);
				t_size += mdr20_ref_size;
			}
		}
		rc = sqlite3_step( ppStmt );
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );

	assert(t_size == mdr_header.mdr5_size);
}
void MDR_Header::writeMDR6(xor_fstream* file) {
	int		lbl15_index;
	int		t_size = 0;
	sqlite3_stmt *ppStmt;

	file->seekg( mdr_header.mdr6_Zip_offset , SEEK_SET );

	int	rc = sqlite3_prepare_v2(getMDRbase(),"SELECT id, mdr1_id, lbl8, t_text FROM MDR6 ORDER BY id;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {
			lbl15_index = MdrCreator::add_lbl15(std::string((char*)sqlite3_column_text(ppStmt,3)));
			
			file->WriteInt(sqlite3_column_int(ppStmt,1),mdr1_ref_size); //mdr1_id
			t_size += mdr1_ref_size;
			file->WriteInt(sqlite3_column_int(ppStmt,2),(mdr_header.mdr6_flag & 3)+1);
			t_size += ((mdr_header.mdr6_flag & 3)+1);

			if( mdr_header.mdr6_flag & 0x04 ) {
				file->WriteInt(lbl15_index,mdr15_ref_size);
				t_size += mdr15_ref_size;
			}
		}
		rc = sqlite3_step( ppStmt );
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );

	assert(t_size == mdr_header.mdr6_size);
}

void MDR_Header::writeMDR9(xor_fstream* file) {
	int		lbl15_index;
	int		t_size = 0;
	sqlite3_stmt *ppStmt;

	file->seekg( mdr_header.mdr9_offset , SEEK_SET );

	int	rc = sqlite3_prepare_v2(getMDRbase(),"SELECT id, family,mdr10_id FROM MDR9 ORDER BY id;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {
			file->WriteInt(sqlite3_column_int(ppStmt,1),1);
			t_size += 1;

			file->WriteInt(sqlite3_column_int(ppStmt,2),mdr10_ref_size);
			t_size += mdr10_ref_size;
		}
		rc = sqlite3_step( ppStmt );
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );
	assert(t_size == mdr_header.mdr9_size);
}

void MDR_Header::writeMDR10(xor_fstream* file) {
	int		t_idx;
	int		t_size = 0;
	sqlite3_stmt *ppStmt;

	file->seekg( mdr_header.mdr10_offset , SEEK_SET );

	int	rc = sqlite3_prepare_v2(getMDRbase(),"SELECT id, sub_type,mdr11_id,uniq FROM MDR10 ORDER BY id;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {
			file->WriteInt(sqlite3_column_int(ppStmt,1),1);
			t_size += 1;

			t_idx = sqlite3_column_int(ppStmt,2);
			if( sqlite3_column_int(ppStmt,3) ) {
				if( mdr11_ref_size_bit == 1 ) t_idx |= 0x80;
				if( mdr11_ref_size_bit == 2 ) t_idx |= 0x8000;
				if( mdr11_ref_size_bit == 3 ) t_idx |= 0x800000;
				if( mdr11_ref_size_bit == 4 ) t_idx |= 0x80000000;
			}
			file->WriteInt(t_idx,mdr11_ref_size_bit);
			t_size += mdr11_ref_size_bit;
		}
		rc = sqlite3_step( ppStmt );
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );
	assert(t_size == mdr_header.mdr10_size);
}

void MDR_Header::writeMDR11(xor_fstream* file) {
	int		t_size = 0;
	int		mdr13_id,mdr14_id;
	sqlite3_stmt *ppStmt;
	unsigned long t_idx;

	file->seekg( mdr_header.mdr11_Name_offset , SEEK_SET );

	int	rc = sqlite3_prepare_v2(getMDRbase(),"SELECT id, mdr1_id, mdr15_id,rgn, tre, mdr5_id,mdr14_id, mdr13_id,lbl1 FROM MDR11 ORDER BY id;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {

			if( sqlite3_column_int(ppStmt,7) > -1 ) 
				mdr13_id = getSelectCount("SELECT t_index FROM MDR13 WHERE id = %i;",sqlite3_column_int(ppStmt,7));
			else
				mdr13_id = -1;
			if( sqlite3_column_int(ppStmt,6) > -1 ) 
				mdr14_id = getSelectCount("SELECT id_4e FROM MDR14 WHERE id = %i;",sqlite3_column_int(ppStmt,6));
			else
				mdr14_id = -1;

			file->WriteInt(sqlite3_column_int(ppStmt,1),mdr1_ref_size); //mdr1_id
			t_size += mdr1_ref_size;

			file->WriteInt(sqlite3_column_int(ppStmt,3),1);
			t_size += 1;

			file->WriteInt(sqlite3_column_int(ppStmt,4),2);
			t_size += 2;

			file->WriteInt(sqlite3_column_int(ppStmt,8),3);
			t_size += 3;

			if( mdr_header.mdr11_flag & 0x01 ) {
				t_size += ((mdr_header.mdr11_flag>>2)&3)+2;
				if( sqlite3_column_int(ppStmt,5) > -1 ) { //tu jeszcze mo¿liwoœæ lbl4 - ale zlewam to...?
					if(((mdr_header.mdr11_flag>>2)&3)+2 == 2) t_idx = 0x8000;
					if(((mdr_header.mdr11_flag>>2)&3)+2 == 3) t_idx = 0x800000;
					if(((mdr_header.mdr11_flag>>2)&3)+2 == 4) t_idx = 0x80000000;
					t_idx += sqlite3_column_int(ppStmt,5);

					file->WriteInt(t_idx,((mdr_header.mdr11_flag>>2)&3)+2);
				} else {
					t_idx = 0;
					if( mdr13_id > -1 )	t_idx = mdr13_id;
					else if( mdr14_id > -1 ) t_idx = mdr14_id | 0x4000;

					file->WriteInt(t_idx,((mdr_header.mdr11_flag>>2)&3)+2);
				}
			}

			if( mdr_header.mdr11_flag & 0x02 ) {
				file->WriteInt(sqlite3_column_int(ppStmt,2),mdr15_ref_size);
				t_size += mdr15_ref_size;
			}
		}
		rc = sqlite3_step( ppStmt );
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );

	assert(t_size == mdr_header.mdr11_size);
}

void MDR_Header::writeMDR13(xor_fstream* file) {
	int		lbl15_index;
	int		t_size = 0;
	int		mdr14_id;
	sqlite3_stmt *ppStmt;

	file->seekg(  mdr_header.mdr13_State_offset , SEEK_SET );

	int	rc = sqlite3_prepare_v2(getMDRbase(),"SELECT id, mdr1_id,mdr15_id,t_index,mdr14_id FROM MDR13 ORDER BY id;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {
			mdr14_id = 0;
			if( sqlite3_column_int(ppStmt,4) > -1 ) 
				mdr14_id = getSelectCount("SELECT id_4e FROM MDR14 WHERE id = %i;",sqlite3_column_int(ppStmt,4));

			file->WriteInt(sqlite3_column_int(ppStmt,1),mdr1_ref_size);
			t_size += mdr1_ref_size;

			file->WriteInt(sqlite3_column_int(ppStmt,3),2);
			t_size += 2;

			file->WriteInt(mdr14_id,2);
			t_size += 2;

			file->WriteInt(sqlite3_column_int(ppStmt,2),mdr15_ref_size);
			t_size += mdr15_ref_size;
		}
		rc = sqlite3_step( ppStmt );
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );
	assert(t_size == mdr_header.mdr13_size);
}
void MDR_Header::writeMDR14(xor_fstream* file) {
	int		lbl15_index;
	int		t_size = 0;
	sqlite3_stmt *ppStmt;

	file->seekg( mdr_header.mdr14_Country_offset , SEEK_SET );

	int	rc = sqlite3_prepare_v2(getMDRbase(),"SELECT id, mdr1_id,mdr15_id,id_4e FROM MDR14 ORDER BY id;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {
			file->WriteInt(sqlite3_column_int(ppStmt,1),mdr1_ref_size);
			t_size += mdr1_ref_size;

			file->WriteInt(sqlite3_column_int(ppStmt,3),2);
			t_size += 2;

			file->WriteInt(sqlite3_column_int(ppStmt,2),mdr15_ref_size);
			t_size += mdr15_ref_size;
		}
		rc = sqlite3_step( ppStmt );
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );
	assert(t_size == mdr_header.mdr14_size);
}

void MDR_Header::writeMDR7(xor_fstream* file) {
	int		lbl15_index;
	int		t_size = 0;
	sqlite3_stmt *ppStmt;

/*
	mdr1_id int,id int,mdr15_id int,lbl1 int,uniq int,nflag int,xflag int,
	mdr5_id int,t_text text,sort_text text,city text,region text,country text,id_43 int,id_53 int,id_4e int


	unsigned long t_lbl1 = this->lbl1| (unique?0x800000:0);
	file->seekg(mdr_header->mdr_header->mdr_header.mdr7_Address_offset + address , SEEK_SET );

	file->Write(&mdr1->work_index,mdr_header->mdr1_ref_size);
	file->Write(&t_lbl1,3);
	if( mdr_header->mdr_header.mdr7_flag & 0x01 ) {
		file->Write(this->mdr15->LBLaddress,mdr_header->mdr15_ref_size);
	}	
	if( mdr_header->mdr_header.mdr7_flag & 0x20 ) {
		file->Write(&this->nflag,1);
	}	
*/
	unsigned long t_lbl1;
	file->seekg( mdr_header.mdr7_Address_offset , SEEK_SET );

	int	rc = sqlite3_prepare_v2(getMDRbase(),"SELECT mdr1_id,id ,mdr15_id ,lbl1 ,uniq ,nflag ,xflag ,mdr5_id ,t_text,sort_text FROM MDR7 ORDER BY id;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {
			lbl15_index = sqlite3_column_int(ppStmt,2);

			t_lbl1 = sqlite3_column_int(ppStmt,3);
			if( !sqlite3_column_int(ppStmt,4) )
				t_lbl1 |= 0x800000;
			
			file->WriteInt(sqlite3_column_int(ppStmt,0),mdr1_ref_size); //mdr1_id
			t_size += mdr1_ref_size;
			file->WriteInt(t_lbl1,3); //
			t_size += 3;
			if( mdr_header.mdr7_flag & 0x01 ) {
				file->WriteInt(lbl15_index,mdr15_ref_size);
				t_size += mdr15_ref_size;
			}	
			if( mdr_header.mdr7_flag & 0x20 ) {
				file->WriteInt(sqlite3_column_int(ppStmt,5),1);
				t_size++;
			}
			if( mdr_header.mdr7_flag & 0x40 ) {
				file->WriteInt(sqlite3_column_int(ppStmt,6),1);
				t_size++;
			}
			if( mdr_header.mdr7_flag & 0x80 ) {
				file->WriteInt(sqlite3_column_int(ppStmt,6),2);
				t_size+=2;
			}
		}
		rc = sqlite3_step( ppStmt );
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );

	assert(t_size == mdr_header.mdr7_size);
}

void MDR_Header::writeMDR8(xor_fstream* file) {
}

void MDR_Header::writeMDR20(xor_fstream* file) {
	int		t_size = 0;
	unsigned long t_idx;
	sqlite3_stmt *ppStmt;
/*
		val = IMG_Helper::Read_N(off, hdr->mdr_20_ref_size,file);
		printf("rec %06x: mdr7.idx: %02x street:%02x ->", rec+1, (val>>1), val & 1);
*/

	file->seekg( mdr_header.mdr20_offset, SEEK_SET );

	int	rc = sqlite3_prepare_v2(getMDRbase(),"SELECT mdr7_id, uniq FROM MDR20 ORDER BY id;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {
			t_idx = sqlite3_column_int(ppStmt,0);
			t_idx <<= 1;
			if( !sqlite3_column_int(ppStmt,1) )
				t_idx |= 1;

			file->WriteInt(t_idx,mdr_header.mdr20_rec_size);
			t_size += mdr_header.mdr20_rec_size;
		}
		rc = sqlite3_step( ppStmt );
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );

	assert(t_size == mdr_header.mdr20_size);
}

void MDR_Header::writeMDR21(xor_fstream* file) {
	int		t_size = 0;
	unsigned long t_idx;
	sqlite3_stmt *ppStmt;
	file->seekg( mdr_header.mdr21_offset, SEEK_SET );

	int	rc = sqlite3_prepare_v2(getMDRbase(),"SELECT mdr7_id, uniq FROM MDR21 ORDER BY id;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {
			t_idx = sqlite3_column_int(ppStmt,0);
			t_idx <<= 1;
			if( !sqlite3_column_int(ppStmt,1) )
				t_idx |= 1;

			file->WriteInt(t_idx,mdr_header.mdr21_rec_size);
			t_size += mdr_header.mdr21_rec_size;
		}
		rc = sqlite3_step( ppStmt );
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );

	assert(t_size == mdr_header.mdr21_size);
}

void MDR_Header::writeMDR22(xor_fstream* file) {
	int		t_size = 0;
	unsigned long t_idx;
	sqlite3_stmt *ppStmt;

	file->seekg( mdr_header.mdr22_offset, SEEK_SET );

	int	rc = sqlite3_prepare_v2(getMDRbase(),"SELECT mdr7_id, uniq FROM MDR22 ORDER BY id;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {
			t_idx = sqlite3_column_int(ppStmt,0);
			t_idx <<= 1;
			if( !sqlite3_column_int(ppStmt,1) )
				t_idx |= 1;

			file->WriteInt(t_idx,mdr_header.mdr22_rec_size);
			t_size += mdr_header.mdr22_rec_size;
		}
		rc = sqlite3_step( ppStmt );
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );

	assert(t_size == mdr_header.mdr22_size);
}

void MDR_Header::writeMDR23(xor_fstream* file) {
	int		t_size = 0;
	int		mdr13_id,mdr14_id;
	unsigned long t_idx;
	sqlite3_stmt *ppStmt;

	file->seekg( mdr_header.mdr23_offset, SEEK_SET );
	int	rc = sqlite3_prepare_v2(getMDRbase(),"SELECT mdr1_id, mdr13_id, mdr14_id, lbl1, uniq FROM MDR23 ORDER BY id;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {
			t_idx = sqlite3_column_int(ppStmt,3);
			if( !sqlite3_column_int(ppStmt,4) )
				t_idx |= 0x800000;

			if( sqlite3_column_int(ppStmt,1) > -1 )
				mdr13_id = getSelectCount("SELECT t_index FROM MDR13 WHERE id = %i;",sqlite3_column_int(ppStmt,1));
			else
				mdr13_id = 0;
			
			if( sqlite3_column_int(ppStmt,2) > -1 )
				mdr14_id = getSelectCount("SELECT id_4e FROM MDR14 WHERE id = %i;",sqlite3_column_int(ppStmt,2));
			else
				mdr14_id = 0;

			file->WriteInt(sqlite3_column_int(ppStmt,0),mdr1_ref_size);
			t_size += mdr1_ref_size;

			file->WriteInt(mdr13_id,2);
			t_size += 2;

			file->WriteInt(mdr14_id,2);
			t_size += 2;

			file->WriteInt(t_idx,3);
			t_size += 3;
		}
		rc = sqlite3_step( ppStmt );
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );

	assert(t_size == mdr_header.mdr23_size);
}

void MDR_Header::writeMDR24(xor_fstream* file) {
	int		t_size = 0;
	int		mdr14_id;
	unsigned long t_idx;
	sqlite3_stmt *ppStmt;

	file->seekg( mdr_header.mdr24_offset, SEEK_SET );
	int	rc = sqlite3_prepare_v2(getMDRbase(),"SELECT mdr1_id, mdr14_id, lbl1, uniq FROM MDR24 ORDER BY id;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {
			t_idx = sqlite3_column_int(ppStmt,2);
			if( !sqlite3_column_int(ppStmt,3) )
				t_idx |= 0x800000;

			if( sqlite3_column_int(ppStmt,1) > -1 )
				mdr14_id = getSelectCount("SELECT id_4e FROM MDR14 WHERE id = %i;",sqlite3_column_int(ppStmt,1));
			else
				mdr14_id = 0;

			file->WriteInt(sqlite3_column_int(ppStmt,0),mdr1_ref_size);
			t_size += mdr1_ref_size;

			file->WriteInt(mdr14_id,2);
			t_size += 2;

			file->WriteInt(t_idx,3);
			t_size += 3;
		}
		rc = sqlite3_step( ppStmt );
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );

	assert(t_size == mdr_header.mdr24_size);
}

void MDR_Header::writeMDR25(xor_fstream* file) {
	int		t_size = 0;
	sqlite3_stmt *ppStmt;

	file->seekg( mdr_header.mdr25_offset, SEEK_SET );
	int	rc = sqlite3_prepare_v2(getMDRbase(),"SELECT mdr1_id, mdr5_id FROM MDR25 ORDER BY id;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {
			file->WriteInt(sqlite3_column_int(ppStmt,1),mdr5_ref_size);
			t_size += mdr5_ref_size;
		}
		rc = sqlite3_step( ppStmt );
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );

	assert(t_size == mdr_header.mdr25_size);
}

void MDR_Header::writeMDR26(xor_fstream* file) {
	int		t_size = 0;
	sqlite3_stmt *ppStmt;

	file->seekg( mdr_header.mdr26_offset, SEEK_SET );
	int	rc = sqlite3_prepare_v2(getMDRbase(),"SELECT mdr28_id FROM MDR26 ORDER BY id;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {
			file->WriteInt(sqlite3_column_int(ppStmt,0),mdr28_ref_size);
			t_size += mdr28_ref_size;
		}
		rc = sqlite3_step( ppStmt );
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );

	assert(t_size == mdr_header.mdr26_size);
}

void MDR_Header::writeMDR27(xor_fstream* file) {
	int		t_size = 0;
	sqlite3_stmt *ppStmt;

	file->seekg( mdr_header.mdr27_offset, SEEK_SET );
	int	rc = sqlite3_prepare_v2(getMDRbase(),"SELECT mdr5_id FROM MDR27 ORDER BY id;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {
			file->WriteInt(sqlite3_column_int(ppStmt,0),mdr5_ref_size);
			t_size += mdr5_ref_size;
		}
		rc = sqlite3_step( ppStmt );
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );

	assert(t_size == mdr_header.mdr27_size);
}

void MDR_Header::writeMDR28(xor_fstream* file) {
	int		t_size = 0;
	unsigned int mdr15_id;
	sqlite3_stmt *ppStmt;
/*
	mdr23
	mdr15
	mdr21
	mdr27
*/
	file->seekg( mdr_header.mdr28_offset, SEEK_SET );
	int	rc = sqlite3_prepare_v2(getMDRbase(),"SELECT mdr23_id,t_text,mdr21_id,mdr27_id FROM MDR28 ORDER BY id;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {
			file->WriteInt(sqlite3_column_int(ppStmt,0),mdr23_ref_size);
			t_size += mdr23_ref_size;

			mdr15_id = MdrCreator::add_lbl15(std::string((char*)sqlite3_column_text(ppStmt,1)));
			file->WriteInt(mdr15_id ,mdr15_ref_size );
			t_size += mdr15_ref_size;

			file->WriteInt(sqlite3_column_int(ppStmt,2),mdr21_ref_size);
			t_size += mdr21_ref_size;

			file->WriteInt(sqlite3_column_int(ppStmt,3),mdr27_ref_size);
			t_size += mdr27_ref_size;
		}
		rc = sqlite3_step( ppStmt );
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );

	assert(t_size == mdr_header.mdr28_size);
}

void MDR_Header::writeMDR29(xor_fstream* file) {
	int		t_size = 0;
	unsigned int mdr15_id;
	sqlite3_stmt *ppStmt;
/*
	mdr24
	mdr15
	mdr22
	mdr25
	mdr26
*/
	file->seekg( mdr_header.mdr29_offset, SEEK_SET );
	int	rc = sqlite3_prepare_v2(getMDRbase(),"SELECT mdr24_id,t_text,mdr22_id,mdr25_id,mdr26_id FROM MDR29 ORDER BY id;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {
			file->WriteInt(sqlite3_column_int(ppStmt,0),mdr24_ref_size);
			t_size += mdr24_ref_size;

			mdr15_id = MdrCreator::add_lbl15(std::string((char*)sqlite3_column_text(ppStmt,1)));
			file->WriteInt(mdr15_id ,mdr15_ref_size );
			t_size += mdr15_ref_size;

			file->WriteInt(sqlite3_column_int(ppStmt,2),mdr22_ref_size);
			t_size += mdr22_ref_size;

			file->WriteInt(sqlite3_column_int(ppStmt,3),mdr25_ref_size);
			t_size += mdr25_ref_size;

			file->WriteInt(sqlite3_column_int(ppStmt,4),mdr26_ref_size);
			t_size += mdr26_ref_size;
		}
		rc = sqlite3_step( ppStmt );
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );

	assert(t_size == mdr_header.mdr29_size);
}

void MDR_Header::writeMDR15(xor_fstream* file) {
	int		lbl15_index;
	int		t_size = 0;
	sqlite3_stmt *ppStmt;

	file->seekg( mdr_header.mdr15_pktStr_offset, SEEK_SET );

	//zerowy rekord
	file->WriteInt(0,1);
	t_size++;

	int	rc = sqlite3_prepare_v2(getMDRbase(),"SELECT lbl_index, t_text, leng FROM MDR15 where lbl_index > 0 ORDER BY lbl_index;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {
			file->seekg( mdr_header.mdr15_pktStr_offset + sqlite3_column_int(ppStmt,0), SEEK_SET );
			file->WriteString((char*)sqlite3_column_text(ppStmt,1));
			t_size += sqlite3_column_int(ppStmt,2);
			t_size++; //null 
		}
		rc = sqlite3_step( ppStmt );
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );

	assert(t_size == mdr_header.mdr15_size);
}

void MDR_Header::Write(const char* file_name) {
	unsigned long size = Calculate();

	//dodac generowanie naglowka DISKIMG

	//zapisanie naglowka
	xor_fstream *file = new xor_fstream(file_name,"wb+");
	MDRfile_start = 0;

	string internal_name = file_name;
	internal_name = internal_name.substr(0,internal_name.length()-8);
	if( internal_name.length() > 8 )
		internal_name = internal_name.substr(0,8);
	else
		while( internal_name.length() < 8 )
			internal_name += "_";
	//WriteIMGHeader(file,size,internal_name.c_str());
	//MDRfile_start = 0;

	file->Write(&mdrHeaderLength, 2);
	file->Write(mdrName, 10);
	file->Write(mdrMdr0, 2);
	file->Write(&mdrYear, 2);
	file->Write(&mdrMonth, 1);
	file->Write(&mdrDay, 1);
	file->Write(mdrTime, 3);
	
	file->Write(&mdr_header,mdrHeaderLength);
	//MDR1
	writeMDR1(file);

	//MDR6
	writeMDR6(file);

	//MDR7
	writeMDR7(file);

	//MDR8
	writeMDR8(file);

	//MDR5
	writeMDR5(file);

	//MDR11
	writeMDR11(file);

	//MDR12
	//zlewka...

	//MDR9
	writeMDR9(file);

	//MDR10
	writeMDR10(file);

	//MDR2
	writeMDR2(file);

	//MDR3
	writeMDR3(file);

	//MDR4
	writeMDR4(file);

	//MDR17

	//MDR16

	//MDR15

	//MDR13
	writeMDR13(file);

	//MDR14
	writeMDR14(file);

	//MDR15
	writeMDR15(file);

	writeMDR20(file);
	writeMDR21(file);
	writeMDR22(file);

	writeMDR23(file);
	writeMDR24(file);
	writeMDR25(file);
	writeMDR26(file);
	writeMDR27(file);
	writeMDR28(file);
	writeMDR29(file);

	delete file;
}

int	MDR_Header::getSelectCount(std::string sql) {
    sqlite3_stmt *ppStmt;
	int ret_val = -1;

	int	rc = sqlite3_prepare_v2(getMDRbase(),sql.c_str(),-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	if( rc == SQLITE_ROW ) 
		ret_val = sqlite3_column_int(ppStmt,0);
	sqlite3_finalize( ppStmt );
	return ret_val;
}

int	MDR_Header::getSelectCount(std::string sql,int i) {
    sqlite3_stmt *ppStmt;
	int ret_val = -1;

	char *s = sqlite3_mprintf(sql.c_str(),i);				
	int	rc = sqlite3_prepare_v2(getMDRbase(),s,-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	if( rc == SQLITE_ROW ) 
		ret_val = sqlite3_column_int(ppStmt,0);
	sqlite3_finalize( ppStmt );
	return ret_val;
}

std::string	MDR_Header::getSelectString(std::string sql,int i,int j) {
    sqlite3_stmt *ppStmt;
	std::string ret_val = "";

	char *s = sqlite3_mprintf(sql.c_str(),i,j);				
	int	rc = sqlite3_prepare_v2(getMDRbase(),s,-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	if( rc == SQLITE_ROW ) 
		ret_val = string((char*)sqlite3_column_text(ppStmt,0));
	sqlite3_finalize( ppStmt );
	return ret_val;
}

int	MDR_Header::getSelectCount(std::string sql,int i,int j) {
    sqlite3_stmt *ppStmt;
	int ret_val = -1;

	char *s = sqlite3_mprintf(sql.c_str(),i,j);				
	int	rc = sqlite3_prepare_v2(getMDRbase(),s,-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	if( rc == SQLITE_ROW ) 
		ret_val = sqlite3_column_int(ppStmt,0);
	sqlite3_finalize( ppStmt );
	return ret_val;
}

int MDR_Header::getNoBytes(unsigned long value) {
	int cnt =0;
	while(value){
		value >>=8;
		cnt ++;
	}
	return cnt;
}

unsigned long MDR_Header::Calculate() {
	int i,j;
	unsigned short last_type = 0xffff;
	unsigned short temp_type = 0;
	int mdr8_count,mdr12_count;
	int last_work_index;
	sqlite3_stmt *ppStmt;	
	char	*s;
	int	rc;

	//1. kalkulacja
	//MDR1
	unsigned long address = 0;

	//DO ZMIANY - TO MUSZA BYC PARAMETRY!!!!!
	mdr_header.m_CodePage = g_compare::get_codepage();
	mdr_header.unk1 = g_compare::get_lang_id();
	if( mdr_header.m_CodePage != 1252 )
		mdr_header.unk2 = 1;
	else
		mdr_header.unk2 = 2;
	mdr_header.unk3 = 0x17;

	mdr_header.mdr34_rec_size = 3;
	mdr_header.mdr35_rec_size = 3;
	mdr_header.mdr36_rec_size = 4;
	mdr_header.mdr39_rec_size = 1;
	mdr_header.mdr40_rec_size = 1;


	//::address mdr15_address;
	//memset(mdr15_address,0,3);

	//ustawic naglowek!

	//przeliczenie podstawowych struktur
	mdr_header.mdr15_size = 1;//z lbl_index = 0 len = 1
	mdr_header.mdr15_size += getSelectCount("SELECT max(lbl_index) FROM MDR15;");
	mdr_header.mdr15_size += getSelectCount("SELECT leng,max(lbl_index) FROM MDR15;");

	mdr15_ref_size = mdr_header.mdr15_size > 0xFFFFFF ? 4 : 3;

	mdr1_ref_size = getNoBytes(getSelectCount("SELECT max(id) FROM MDR1;"));
	mdr7_ref_size = getNoBytes(getSelectCount("SELECT max(id) FROM MDR7;"));
	mdr10_ref_size = getNoBytes(getSelectCount("SELECT max(id) FROM MDR10;"));
	mdr11_ref_size = getNoBytes(getSelectCount("SELECT max(id) FROM MDR11;")); // ostatni bit - flaga!
	mdr11_ref_size_bit = getNoBytes(getSelectCount("SELECT max(id) FROM MDR11;")<<1); // ostatni bit - flaga!
	//mdr10_ref_size = mdr11_ref_size;
	mdr_header.mdr9_rec_size = 1 + mdr10_ref_size;

	//naglowek
	mdr20_ref_size = getNoBytes(getSelectCount("SELECT max(id) FROM MDR20;"));
	mdr21_ref_size = getNoBytes(getSelectCount("SELECT max(id) FROM MDR21;"));
	mdr22_ref_size = getNoBytes(getSelectCount("SELECT max(id) FROM MDR22;"));

	mdr23_ref_size = getNoBytes(getSelectCount("SELECT max(id) FROM MDR23;"));
	mdr24_ref_size = getNoBytes(getSelectCount("SELECT max(id) FROM MDR24;"));
	mdr25_ref_size = getNoBytes(getSelectCount("SELECT max(id) FROM MDR25;"));
	mdr26_ref_size = getNoBytes(getSelectCount("SELECT max(id) FROM MDR26;"));
	mdr27_ref_size = getNoBytes(getSelectCount("SELECT max(id) FROM MDR27;"));
	mdr28_ref_size = getNoBytes(getSelectCount("SELECT max(id) FROM MDR28;"));

	mdr_header.mdr5_flag = (getNoBytes(getSelectCount("select max(t_index) from t43_miasta;"))-1);
	mdr_header.mdr5_flag |= 4;//mdr13/14
	mdr_header.mdr5_flag |= 8;//mdr15
	if( mdr20_ref_size )
		mdr_header.mdr5_flag |= 0x100;//mdr20
	mdr_header.mdr5_flag |= 0x10;//?
	mdr_header.mdr5_rec_size = static_cast<unsigned short>(mdr1_ref_size + ((mdr_header.mdr5_flag & 3) + 1) + 3)/*lbl1*/;
	if( mdr_header.mdr5_flag & 4 )	mdr_header.mdr5_rec_size += 2;
	if( mdr_header.mdr5_flag & 8 )	mdr_header.mdr5_rec_size += mdr15_ref_size;
	if( mdr_header.mdr5_flag & 0x100 )	mdr_header.mdr5_rec_size += mdr20_ref_size;

	mdr5_ref_size = getNoBytes(getSelectCount("SELECT max(id) FROM MDR5;"));
	mdr6_ref_size = getNoBytes(getSelectCount("SELECT max(id) FROM MDR6;"));
	if( mdr6_ref_size > 1 ) mdr_header.mdr6_flag = 1+4;
	if( mdr6_ref_size > 2 ) mdr_header.mdr6_flag = 2+4;
	if( mdr6_ref_size > 3 ) mdr_header.mdr6_flag = 3+4;
	mdr_header.mdr6_rec_size = static_cast<unsigned short>(mdr1_ref_size + /*mdr6_ref_size +*/ ((mdr_header.mdr6_flag & 3) + 1));
	if( mdr_header.mdr6_flag & 0x04 ) mdr_header.mdr6_rec_size += mdr15_ref_size;

	mdr_header.mdr7_rec_size = mdr1_ref_size + 3;
	if( mdr_header.mdr7_flag & 1 ) mdr_header.mdr7_rec_size += mdr15_ref_size;
	if( mdr_header.mdr7_flag & 0x20 ) mdr_header.mdr7_rec_size += 1;
	if( mdr_header.mdr7_flag & 0x40 ) mdr_header.mdr7_rec_size += 1;
	if( mdr_header.mdr7_flag & 0x80 ) mdr_header.mdr7_rec_size += 2;

	mdr_header.mdr8_rec_size = static_cast<unsigned short>(((mdr_header.mdr8_flag >> 8) & 0xff) + mdr7_ref_size);
	mdr_header.mdr12_rec_size = static_cast<unsigned short>(((mdr_header.mdr12_flag >> 8) & 0xff) + mdr11_ref_size_bit);
	// nie u¿ywam MDR12
	mdr_header.mdr12_rec_size = 0;

	mdr_header.mdr13_rec_size = mdr1_ref_size + 2 + 2 + mdr15_ref_size;
	mdr_header.mdr14_rec_size = mdr1_ref_size + 2 + mdr15_ref_size;

	mdr_header.mdr11_flag = 0x13; //standardowo zawsze przewidziane mdr5 i mdr13!
	if( getSelectCount("SELECT count(*) FROM MDR11 where mdr5_id > -1;") > 0 )	mdr_header.mdr11_flag |= 0x11;
	if( getSelectCount("SELECT count(*) FROM MDR11 where mdr13_id > -1 or mdr14_id > -1;") > 0 )	mdr_header.mdr11_flag |= 0x1;
	
	if( mdr_header.mdr11_flag & 0x1 ) {
		int t_ref = 0;
		int t_size = static_cast<int>(getSelectCount("SELECT max(id) FROM MDR5;"));
		t_size = t_size << 1; //????//flaga zapisywana dla MDR5 - 1 bit dodatkowo
		if( mdr_header.mdr11_flag & 0x10 )
			t_ref = getNoBytes(t_size);
		t_ref = max(t_ref,getNoBytes(static_cast<unsigned long>(getSelectCount("SELECT max(id) FROM MDR13;"))));
		t_ref = max(t_ref,getNoBytes(static_cast<unsigned long>(getSelectCount("SELECT max(id) FROM MDR14;"))));
		t_ref = max(t_ref,2);

		if( t_ref == 3 )	  mdr_header.mdr11_flag |= 0x1<<2;
		else if( t_ref == 4 ) mdr_header.mdr11_flag |= 0x2<<2;
		else if( t_ref == 5 ) mdr_header.mdr11_flag |= 0x3<<2;
	}
	
	mdr_header.mdr11_rec_size = mdr1_ref_size + 1 + 2 + 3;
	if( mdr_header.mdr11_flag & 1 ) mdr_header.mdr11_rec_size += static_cast<unsigned short>(((mdr_header.mdr11_flag >> 2) & 3) + 2);
	if( mdr_header.mdr11_flag & 2 ) mdr_header.mdr11_rec_size += mdr15_ref_size; 

	/*
	char* s = sqlite3_mprintf("UPDATE MDR1 SET MDR5_ref_size = %i, MDR6_ref_size = %i, MDR7_ref_size = %i, MDR11_ref_size = %i;",
		getNoBytes(getSelectCount("SELECT max(id) FROM MDR5;")),
		getNoBytes(getSelectCount("SELECT max(id) FROM MDR6;")),
		getNoBytes(getSelectCount("SELECT max(id) FROM MDR7;")),
		getNoBytes(getSelectCount("SELECT max(id) FROM MDR11;")));
	sqlite3_exec(getMDRbase(),s,NULL,NULL,NULL);			
	sqlite3_free(s);
	*/

	//przeliczanie
	//MDR1
	//MDR1 - rozmiar rekordów
	address = mdrHeaderLength;//getSelectCount("SELECT count(*) FROM MDR1;") * MDR1_REC_SIZE;
	mdr_header.mdr1_size = getSelectCount("SELECT count(*) FROM MDR1;") * mdr_header.mdr1_rec_size;
	
	/*

	MDR header

	-> tablice offsetów do kolejnych tablic MDR - 46 bajtowe 
	-> tablice MDR
	-> tablice offsetów do kolejnych tablic MDR - 46 bajtowe 
	-> tablice MDR
	..
	-> MDR1 (8 bajtowe) -> pocz¹tek -> MDR1_OFFSET
	  -ID
	  -adres do tablicy offsetow (OFFSET) bezwzglêdny 
	...
	-> MDR2 etc..

	*/
	int relative_address = 0;
	//MDR1 - rozmiar danych
	rc = sqlite3_prepare_v2(getMDRbase(),"SELECT id FROM MDR1 ORDER BY id;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {
			int mdr1_rec_size = getSelectCount("SELECT count(*) FROM MDR5 where MDR1_ID=%i;",sqlite3_column_int(ppStmt,0)) * mdr5_ref_size;
			mdr1_rec_size += getSelectCount("SELECT count(*) FROM MDR11 where MDR1_ID=%i;",sqlite3_column_int(ppStmt,0)) * mdr11_ref_size;
			mdr1_rec_size += getSelectCount("select count(*) from mdr10 where mdr11_id in (select id from mdr11 where mdr1_id=%i);",sqlite3_column_int(ppStmt,0)) * mdr11_ref_size;
			mdr1_rec_size += getSelectCount("SELECT count(*) FROM MDR7 where MDR1_ID=%i;",sqlite3_column_int(ppStmt,0)) * mdr7_ref_size;
			mdr1_rec_size += getSelectCount("SELECT count(*) FROM MDR6 where MDR1_ID=%i;",sqlite3_column_int(ppStmt,0)) * mdr6_ref_size;
			mdr1_rec_size += getSelectCount("SELECT count(*) FROM MDR20 where MDR1_ID=%i;",sqlite3_column_int(ppStmt,0)) * mdr20_ref_size;
			mdr1_rec_size += getSelectCount("SELECT count(*) FROM MDR21 where MDR1_ID=%i;",sqlite3_column_int(ppStmt,0)) * mdr21_ref_size;
			mdr1_rec_size += getSelectCount("SELECT count(*) FROM MDR22 where MDR1_ID=%i;",sqlite3_column_int(ppStmt,0)) * mdr22_ref_size;

			s = sqlite3_mprintf("UPDATE MDR1 SET offset = %i WHERE id = %i",				
				//tmp_address, //adres dla 2 rekordów MDR1 - relatywny do mdr1_offset
				address,
				//end_mdr1_main_records, //to musi byc adres bezwzglêdny w MDR! dla tablicy
				sqlite3_column_int(ppStmt,0));
			sqlite3_exec(getMDRbase(),s,NULL,NULL,NULL);			
			sqlite3_free(s);
			
			address += (MDR1_REC_SIZE + mdr1_rec_size);
			relative_address = MDR1_REC_SIZE;

			//adresy do tablic z danymi
			s = sqlite3_mprintf("UPDATE MDR1 SET MDR11_records_count = %i,MDR11_data_offset = %i WHERE id = %i",
				getSelectCount("SELECT count(*) FROM MDR11 where MDR1_ID=%i;",sqlite3_column_int(ppStmt,0)),
				relative_address,
				sqlite3_column_int(ppStmt,0));
			sqlite3_exec(getMDRbase(),s,NULL,NULL,NULL);			
			sqlite3_free(s);
			relative_address += getSelectCount("SELECT count(*) FROM MDR11 where MDR1_ID=%i;",sqlite3_column_int(ppStmt,0)) * mdr11_ref_size;

			s = sqlite3_mprintf("UPDATE MDR1 SET MDR10_data_offset = %i WHERE id = %i",
				relative_address,
				sqlite3_column_int(ppStmt,0));
			sqlite3_exec(getMDRbase(),s,NULL,NULL,NULL);			
			sqlite3_free(s);
			relative_address += getSelectCount("SELECT count(*) FROM MDR11 where MDR1_ID=%i;",sqlite3_column_int(ppStmt,0)) * mdr11_ref_size;

			s = sqlite3_mprintf("UPDATE MDR1 SET MDR7_records_count = %i,MDR7_data_offset=%i WHERE id = %i",
				getSelectCount("SELECT count(*) FROM MDR7 where MDR1_ID=%i;",sqlite3_column_int(ppStmt,0)),
				relative_address,
				sqlite3_column_int(ppStmt,0));
			sqlite3_exec(getMDRbase(),s,NULL,NULL,NULL);			
			sqlite3_free(s);
			relative_address += getSelectCount("SELECT count(*) FROM MDR7 where MDR1_ID=%i;",sqlite3_column_int(ppStmt,0)) * mdr7_ref_size;

			s = sqlite3_mprintf("UPDATE MDR1 SET MDR5_records_count = %i, MDR5_data_offset = %i WHERE id = %i",
				getSelectCount("SELECT count(*) FROM MDR5 where MDR1_ID=%i;",sqlite3_column_int(ppStmt,0)),
				relative_address,
				sqlite3_column_int(ppStmt,0));
			sqlite3_exec(getMDRbase(),s,NULL,NULL,NULL);			
			sqlite3_free(s);
			relative_address += getSelectCount("SELECT count(*) FROM MDR5 where MDR1_ID=%i;",sqlite3_column_int(ppStmt,0)) * getNoBytes(getSelectCount("SELECT max(id) FROM MDR5;"));

			s = sqlite3_mprintf("UPDATE MDR1 SET MDR6_records_count = %i, MDR6_data_offset = %i WHERE id = %i",
				getSelectCount("SELECT count(*) FROM MDR6 where MDR1_ID=%i;",sqlite3_column_int(ppStmt,0)),
				relative_address,
				sqlite3_column_int(ppStmt,0));
			sqlite3_exec(getMDRbase(),s,NULL,NULL,NULL);			
			sqlite3_free(s);
			relative_address += getSelectCount("SELECT count(*) FROM MDR6 where MDR1_ID=%i;",sqlite3_column_int(ppStmt,0)) * mdr6_ref_size;

			s = sqlite3_mprintf("UPDATE MDR1 SET MDR20_records_count = %i, MDR20_data_offset = %i WHERE id = %i",
				getSelectCount("SELECT count(*) FROM MDR20 where MDR1_ID=%i;",sqlite3_column_int(ppStmt,0)),
				relative_address,
				sqlite3_column_int(ppStmt,0));
			sqlite3_exec(getMDRbase(),s,NULL,NULL,NULL);			
			sqlite3_free(s);
			relative_address += getSelectCount("SELECT count(*) FROM MDR20 where MDR1_ID=%i;",sqlite3_column_int(ppStmt,0)) * mdr20_ref_size;

			s = sqlite3_mprintf("UPDATE MDR1 SET MDR21_records_count = %i, MDR21_data_offset = %i WHERE id = %i",
				getSelectCount("SELECT count(*) FROM MDR21 where MDR1_ID=%i;",sqlite3_column_int(ppStmt,0)),
				relative_address,
				sqlite3_column_int(ppStmt,0));
			sqlite3_exec(getMDRbase(),s,NULL,NULL,NULL);			
			sqlite3_free(s);
			relative_address += getSelectCount("SELECT count(*) FROM MDR21 where MDR1_ID=%i;",sqlite3_column_int(ppStmt,0)) * mdr21_ref_size;

			s = sqlite3_mprintf("UPDATE MDR1 SET MDR22_records_count = %i, MDR22_data_offset = %i WHERE id = %i",
				getSelectCount("SELECT count(*) FROM MDR22 where MDR1_ID=%i;",sqlite3_column_int(ppStmt,0)),
				relative_address,
				sqlite3_column_int(ppStmt,0));
			sqlite3_exec(getMDRbase(),s,NULL,NULL,NULL);			
			sqlite3_free(s);
			relative_address += getSelectCount("SELECT count(*) FROM MDR22 where MDR1_ID=%i;",sqlite3_column_int(ppStmt,0)) * mdr22_ref_size;

			s = sqlite3_mprintf("UPDATE MDR1 SET MDRX_records_count = 0, MDRX_data_offset = %i WHERE id = %i",
				relative_address,
				sqlite3_column_int(ppStmt,0));
			sqlite3_exec(getMDRbase(),s,NULL,NULL,NULL);			
			sqlite3_free(s);
			relative_address += 0;

			rc = sqlite3_step( ppStmt );
		}
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );

	mdr_header.mdr1_offset = address;
	address += mdr_header.mdr1_size;

	//MDR16 - pierwsza sekcja..	
	//MDR2
	mdr_header.mdr2_offset = address;

	//MDR3
	mdr_header.mdr3_offset = mdr_header.mdr2_size + mdr_header.mdr2_offset;

	//MDR4
	mdr_header.mdr4_offset = mdr_header.mdr3_size + mdr_header.mdr3_offset;
	mdr_header.mdr4_size = getSelectCount("SELECT count(*) FROM MDR4;") * 3;

	//mdr34
	mdr_header.mdr34_offset = mdr_header.mdr4_size + mdr_header.mdr4_offset;

	//mdr35
	mdr_header.mdr35_offset = mdr_header.mdr34_offset;

	//MDR5
	mdr_header.mdr5_City_offset = mdr_header.mdr35_offset;
	mdr_header.mdr5_size = getSelectCount("SELECT count(*) FROM MDR5;") * mdr_header.mdr5_rec_size;//calculateMDRsection(mdr_header.mdr5_rec_size,"MDR5");

	//MDR6 - zip
	mdr_header.mdr6_Zip_offset = mdr_header.mdr5_City_offset + mdr_header.mdr5_size;
	mdr_header.mdr6_size = getSelectCount("SELECT count(*) FROM MDR6;") * mdr_header.mdr6_rec_size;//calculateMDRsection(mdr_header.mdr6_rec_size,"MDR6");

	//MDR7
	mdr_header.mdr7_Address_offset = mdr_header.mdr6_size + mdr_header.mdr6_Zip_offset;
	mdr_header.mdr7_size = getSelectCount("SELECT count(*) FROM MDR7;") * mdr_header.mdr7_rec_size;//calculateMDRsection(mdr_header.mdr7_rec_size,"MDR7");

	//MDR8
	mdr_header.mdr8_offset = mdr_header.mdr7_size + mdr_header.mdr7_Address_offset;
	mdr_header.mdr8_size = getSelectCount("SELECT count(*) FROM MDR8;") * mdr_header.mdr8_rec_size;//calculateMDRsection(mdr_header.mdr8_rec_size,"MDR8");
	if( !mdr_header.mdr8_size ) 
		mdr_header.mdr8_rec_size = 0;

	//MDR20
	mdr_header.mdr20_offset = mdr_header.mdr8_offset + mdr_header.mdr8_size;
	long mdr20_rec_count = getSelectCount("SELECT count(id) FROM MDR20;");
	mdr_header.mdr20_rec_size = getNoBytes(mdr20_rec_count<<2);
	mdr_header.mdr20_size = mdr20_rec_count * mdr_header.mdr20_rec_size;

	//MDR21
	mdr_header.mdr21_offset = mdr_header.mdr20_offset + mdr_header.mdr20_size;
	long mdr21_rec_count = getSelectCount("SELECT count(id) FROM MDR21;");
	mdr_header.mdr21_rec_size = getNoBytes(mdr21_rec_count<<2);
	mdr_header.mdr21_size = mdr21_rec_count * mdr_header.mdr21_rec_size;

	//MDR22
	mdr_header.mdr22_offset = mdr_header.mdr21_offset + mdr_header.mdr21_size;
	long mdr22_rec_count = getSelectCount("SELECT count(id) FROM MDR22;");
	mdr_header.mdr22_rec_size = getNoBytes(mdr22_rec_count<<2);
	mdr_header.mdr22_size = mdr22_rec_count * mdr_header.mdr22_rec_size;

	//MDR9
	mdr_header.mdr9_offset = mdr_header.mdr22_offset + mdr_header.mdr22_size;
	mdr_header.mdr9_size = getSelectCount("SELECT count(*) FROM MDR9;") * mdr_header.mdr9_rec_size;//calculateMDRsection(mdr_header.mdr9_rec_size,"MDR9");

	//MDR10
	mdr_header.mdr10_offset = mdr_header.mdr9_size + mdr_header.mdr9_offset;
	mdr_header.mdr10_size = getSelectCount("SELECT count(*) FROM MDR10;") * (mdr11_ref_size_bit+1);//calculateMDRsection(mdr_header.mdr11_rec_size+1,"MDR10");

	//MDR11
	mdr_header.mdr11_Name_offset = mdr_header.mdr10_size + mdr_header.mdr10_offset;
	mdr_header.mdr11_size = getSelectCount("SELECT count(*) FROM MDR11;") * mdr_header.mdr11_rec_size;//calculateMDRsection(mdr_header.mdr11_rec_size,"MDR11");

	//mdr36
	mdr_header.mdr36_offset = mdr_header.mdr11_Name_offset + mdr_header.mdr11_size;

	//MDR12
	mdr_header.mdr12_offset = mdr_header.mdr36_offset;
	mdr_header.mdr12_size = getSelectCount("SELECT count(*) FROM MDR12;") * mdr_header.mdr12_rec_size;//calculateMDRsection(mdr_header.mdr12_rec_size,"MDR12");

	//MDR13
	mdr_header.mdr13_State_offset = mdr_header.mdr12_size + mdr_header.mdr12_offset;
	mdr_header.mdr13_size = getSelectCount("SELECT count(*) FROM MDR13;") * mdr_header.mdr13_rec_size;//calculateMDRsection(mdr_header.mdr13_rec_size,"MDR13");

	//MDR14
	mdr_header.mdr14_Country_offset = mdr_header.mdr13_size + mdr_header.mdr13_State_offset;
	mdr_header.mdr14_size = getSelectCount("SELECT count(*) FROM MDR14;") * mdr_header.mdr14_rec_size;//calculateMDRsection(mdr_header.mdr14_rec_size,"MDR14");

	//MDR23
	mdr_header.mdr23_offset = mdr_header.mdr14_Country_offset + mdr_header.mdr14_size;
	long mdr23_rec_count = getSelectCount("SELECT count(id) FROM MDR23;");
	mdr_header.mdr23_rec_size = mdr1_ref_size + 2 + 2 + 3;
	mdr_header.mdr23_size = mdr23_rec_count * mdr_header.mdr23_rec_size;

	//MDR24
	mdr_header.mdr24_offset = mdr_header.mdr23_offset + mdr_header.mdr23_size;
	long mdr24_rec_count = getSelectCount("SELECT count(id) FROM MDR24;");
	mdr_header.mdr24_rec_size = mdr1_ref_size + 2 + 3;
	mdr_header.mdr24_size = mdr24_rec_count * mdr_header.mdr24_rec_size;

	//MDR28
	mdr_header.mdr28_offset = mdr_header.mdr24_offset + mdr_header.mdr24_size;
	long mdr28_rec_count = getSelectCount("SELECT count(id) FROM MDR28;");
	mdr_header.mdr28_rec_size = mdr23_ref_size + mdr15_ref_size + mdr21_ref_size + mdr27_ref_size;
	mdr_header.mdr28_size = mdr28_rec_count * mdr_header.mdr28_rec_size;

	//MDR29
	mdr_header.mdr29_offset = mdr_header.mdr28_offset + mdr_header.mdr28_size;
	long mdr29_rec_count = getSelectCount("SELECT count(id) FROM MDR29;");
	mdr_header.mdr29_rec_size = mdr24_ref_size + mdr15_ref_size + mdr22_ref_size + mdr25_ref_size + mdr26_ref_size;
	mdr_header.mdr29_size = mdr29_rec_count * mdr_header.mdr29_rec_size;

	//MDR25
	mdr_header.mdr25_offset = mdr_header.mdr29_offset + mdr_header.mdr29_size;
	long mdr25_rec_count = getSelectCount("SELECT count(id) FROM MDR25;");
	mdr_header.mdr25_rec_size = mdr5_ref_size;
	mdr_header.mdr25_size = mdr25_rec_count * mdr_header.mdr25_rec_size;

	//MDR26
	mdr_header.mdr26_offset = mdr_header.mdr25_offset + mdr_header.mdr25_size;
	long mdr26_rec_count = getSelectCount("SELECT count(id) FROM MDR26;");
	mdr_header.mdr26_rec_size = mdr28_ref_size;
	mdr_header.mdr26_size = mdr26_rec_count * mdr_header.mdr26_rec_size;

	//MDR27
	mdr_header.mdr27_offset = mdr_header.mdr26_offset + mdr_header.mdr26_size;
	long mdr27_rec_count = getSelectCount("SELECT count(id) FROM MDR27;");
	mdr_header.mdr27_rec_size = mdr5_ref_size;
	mdr_header.mdr27_size = mdr27_rec_count * mdr_header.mdr27_rec_size;

	//mdr30
	mdr_header.mdr30_offset = mdr_header.mdr27_size + mdr_header.mdr27_offset;

	//mdr31
	mdr_header.mdr31_offset = mdr_header.mdr30_offset;

	//mdr32
	mdr_header.mdr32_offset = mdr_header.mdr31_offset;

	//mdr33
	mdr_header.mdr33_offset = mdr_header.mdr32_offset;

	//mdr37
	mdr_header.mdr37_offset = mdr_header.mdr33_offset;

	//mdr38
	mdr_header.mdr38_offset = mdr_header.mdr37_offset;

	//mdr39
	mdr_header.mdr39_offset = mdr_header.mdr38_offset;

	//mdr40
	mdr_header.mdr40_offset = mdr_header.mdr39_offset;

	//MDR15
	mdr_header.mdr15_pktStr_offset = mdr_header.mdr40_offset;
	//mdr_header.mdr15_size

	//MDR17
	mdr_header.mdr17_offset = 0;

	//MDR16
	mdr_header.mdr16_Huffman_offset = 0;

	return mdr_header.mdr15_pktStr_offset + mdr_header.mdr15_size;
}

int MDR_Header::calculateMDRsection(int rec_size,std::string table_name) {
	address = 0;
	sqlite3_stmt *ppStmt;	

	char *s = sqlite3_mprintf("SELECT id FROM %q ORDER BY id;",table_name.c_str());
	int rc = sqlite3_prepare_v2(getMDRbase(),s,-1,&ppStmt,NULL);
	sqlite3_free(s);

	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {
			s = sqlite3_mprintf("UPDATE %q SET address = %i WHERE id = %i",
				table_name.c_str(),
				address,
				sqlite3_column_int(ppStmt,0));
			sqlite3_exec(getMDRbase(),s,NULL,NULL,NULL);			
			sqlite3_free(s);
			address += rec_size;
		}
		rc = sqlite3_step( ppStmt );
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );
	return address;
}

/****************************************************************************/

void MdrCreator::write_mdr(std::string text) {
	mdrHeader.Write(text.c_str());
}


static int callback(void *NotUsed, int argc, char **argv, char **azColName){
  int i;
  for(i=0; i<argc; i++){
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  printf("\n");
  return 0;
}

int	MdrCreator::add_lbl15(std::string text) {
	sqlite3_stmt *ppStmt;	
	int		lbl15_index = 0;

	if( text.size() == 0 )
		return 0;

	char	*s = sqlite3_mprintf("SELECT lbl_index FROM MDR15 WHERE t_text = '%q';",text.c_str());
	int		rc = sqlite3_prepare_v2(getMDRbase(),s,-1,&ppStmt,NULL);
	//do {
    rc = sqlite3_step( ppStmt );    
	if ( rc == SQLITE_ROW ) {
		lbl15_index = sqlite3_column_int(ppStmt,0);
	}
	//} while(rc == SQLITE_ROW );
	sqlite3_finalize( ppStmt );
	sqlite3_free(s);

	if( lbl15_index == 0 ) {
		rc = sqlite3_prepare_v2(getMDRbase(),"SELECT leng,lbl_index FROM MDR15 order by lbl_index desc LIMIT 1;",-1,&ppStmt,NULL);
		rc = sqlite3_step( ppStmt );    
		if ( rc == SQLITE_ROW ) {
			lbl15_index = sqlite3_column_int(ppStmt,0);
			lbl15_index += sqlite3_column_int(ppStmt,1);
			lbl15_index++; //NULL terminator
		}
		sqlite3_finalize( ppStmt );

		s = sqlite3_mprintf("INSERT INTO MDR15(lbl_index,t_text,leng) VALUES(%i,'%q',%i);",lbl15_index,text.c_str(),text.size());
		rc = sqlite3_exec(getMDRbase(),s,NULL,NULL,NULL);
		sqlite3_free(s);
	}
	return lbl15_index;
}

void MdrCreator::index_mdr13_mdr14() {
	int		lbl15_index;
	int		rec_id = 1;
	int		mdr_id = -1;
	sqlite3_stmt *ppStmt;
	int		mdr1_map_index = 1;

//	sqlite3_exec(getMDRbase(),"BEGIN TRANSACTION",NULL,NULL,NULL);

	//Przeniesienie wszystkich krajów - utworzenie MDR14
	int	rc = sqlite3_prepare_v2(getMDRbase(),"SELECT DISTINCT MDR1.id, T4E_KRAJE.t_index, T4E_KRAJE.lbl1, T4E_KRAJE.t_text, T4E_KRAJE.map_id FROM T4E_KRAJE INNER JOIN MDR1 ON (T4E_KRAJE.map_id=MDR1.map_id) ORDER BY MDR1.id, T4E_KRAJE.t_index;",-1,&ppStmt,NULL);
	//int	rc = sqlite3_prepare_v2(getMDRbase(),"SELECT DISTINCT MDR1.id, T4E_KRAJE.t_index, T4E_KRAJE.lbl1, T4E_KRAJE.t_text, T4E_KRAJE.map_id FROM T4E_KRAJE INNER JOIN MDR1 ON (T4E_KRAJE.map_id=MDR1.map_id) ORDER BY MDR1.id, T4E_KRAJE.t_text;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {
			if( mdr_id != sqlite3_column_int(ppStmt,0) )
				rec_id = 1;
			else
				rec_id++;
			mdr_id = sqlite3_column_int(ppStmt,0);

			lbl15_index = add_lbl15(std::string((char*)sqlite3_column_text(ppStmt,3)));
			char *s = sqlite3_mprintf("INSERT INTO MDR14(rec_id,mdr1_id ,id ,mdr15_id ,lbl1 ,t_text, id_4e ) VALUES(%i,%i,%i,%i,%i,'%q',%i);",
				rec_id,sqlite3_column_int(ppStmt,0),mdr1_map_index,lbl15_index,sqlite3_column_int(ppStmt,2),sqlite3_column_text(ppStmt,3),sqlite3_column_int(ppStmt,1));

			sqlite3_exec(getMDRbase(),s,NULL,NULL,NULL);
			sqlite3_free(s);

			s = sqlite3_mprintf("UPDATE T4E_KRAJE SET mdr14_id = %i WHERE map_id = %i AND t_index = %i;",mdr1_map_index,sqlite3_column_int(ppStmt,4),sqlite3_column_int(ppStmt,1));
			sqlite3_exec(getMDRbase(),s,NULL,NULL,NULL);			
			sqlite3_free(s);

			mdr1_map_index++;
		}
		rc = sqlite3_step( ppStmt );
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );

	//Utworzenie MDR13 - rekordy zwi¹zane z MDR14
/*
	(mdr1_id int,id int,mdr15_id int,mdr14_id int,lbl1 int,t_text text)
*/
	mdr1_map_index = 1;
	//rc = sqlite3_prepare_v2(getMDRbase(),"SELECT DISTINCT MDR1.id, T4E_KRAJE.mdr14_id, T53_REGIONY.lbl1, T53_REGIONY.t_text, T53_REGIONY.map_id,T53_REGIONY.t_index FROM T4E_KRAJE, T53_REGIONY,MDR1 WHERE T4E_KRAJE.map_id = T53_REGIONY.map_id AND T4E_KRAJE.t_index = T53_REGIONY.id_4e AND T53_REGIONY.map_id = MDR1.map_ID ORDER BY MDR1.id, T53_REGIONY.t_text;",-1,&ppStmt,NULL);
	//rc = sqlite3_prepare_v2(getMDRbase(),"SELECT DISTINCT MDR1.id, T4E_KRAJE.mdr14_id, T53_REGIONY.lbl1, T53_REGIONY.t_text, T53_REGIONY.map_id,T53_REGIONY.t_index FROM T4E_KRAJE, T53_REGIONY,MDR1 WHERE T4E_KRAJE.map_id = T53_REGIONY.map_id AND T4E_KRAJE.t_index = T53_REGIONY.id_4e AND T53_REGIONY.map_id = MDR1.map_ID ORDER BY MDR1.id,  T4E_KRAJE.t_text, T53_REGIONY.t_text;",-1,&ppStmt,NULL);
	rc = sqlite3_prepare_v2(getMDRbase(),"SELECT DISTINCT MDR1.id, T4E_KRAJE.mdr14_id, T53_REGIONY.lbl1, T53_REGIONY.t_text, T53_REGIONY.map_id,T53_REGIONY.t_index FROM T4E_KRAJE, T53_REGIONY,MDR1 WHERE T4E_KRAJE.map_id = T53_REGIONY.map_id AND T4E_KRAJE.t_index = T53_REGIONY.id_4e AND T53_REGIONY.map_id = MDR1.map_ID ORDER BY MDR1.id, T53_REGIONY.t_index ;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {
			if( mdr_id != sqlite3_column_int(ppStmt,0) )
				rec_id = 1;
			else
				rec_id++;
			mdr_id = sqlite3_column_int(ppStmt,0);
			lbl15_index = add_lbl15(std::string((char*)sqlite3_column_text(ppStmt,3)));
			char *s = sqlite3_mprintf("INSERT INTO MDR13(rec_id, mdr1_id ,id ,mdr15_id ,mdr14_id, lbl1 ,t_text, t_index ) VALUES(%i, %i,%i,%i,%i,%i,'%q',%i);",
				rec_id, 
				sqlite3_column_int(ppStmt,0),
				mdr1_map_index,
				lbl15_index,
				sqlite3_column_int(ppStmt,1),
				sqlite3_column_int(ppStmt,2),
				sqlite3_column_text(ppStmt,3),
				sqlite3_column_int(ppStmt,5));

			sqlite3_exec(getMDRbase(),s,NULL,NULL,NULL);
			sqlite3_free(s);

			s = sqlite3_mprintf("UPDATE T53_REGIONY SET mdr13_id = %i WHERE map_id = %i AND t_index = %i;",mdr1_map_index,sqlite3_column_int(ppStmt,4),sqlite3_column_int(ppStmt,5));
			sqlite3_exec(getMDRbase(),s,NULL,NULL,NULL);			
			sqlite3_free(s);

			mdr1_map_index++;
		}
		rc = sqlite3_step( ppStmt );
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );

	//dodanie MDR13 niezwi¹zanych z MDR14

//	sqlite3_exec(getMDRbase(),"END TRANSACTION",NULL,NULL,NULL);
}

void MdrCreator::index_mdr4() {
	//select distinct type,sub_type,highest_level from t4c_types order by type,sub_type;
	//MDR4  (type int,sub_type int,level int
	sqlite3_stmt *ppStmt;
	int			unique;
	int			id = 1;
//	sqlite3_exec(getMDRbase(),"BEGIN TRANSACTION",NULL,NULL,NULL);

	//int	rc = sqlite3_prepare_v2(getMDRbase(),"select distinct type,sub_type,max(highest_level) from t4c_types where g_type = 2  group by type,sub_type;",-1,&ppStmt,NULL);
	int	rc = sqlite3_prepare_v2(getMDRbase(),"select distinct type,0 as sub_type,max(highest_level) from t4c_types where g_type = 2 and type < 16 and sub_type = 0 group by type " \
	" union " \
	" select distinct type,sub_type,max(highest_level) from t4c_types where g_type = 2 and type >= 42 group by type,sub_type;",-1,&ppStmt,NULL);

	//select distinct type,sub_type,highest_level,g_type from t4c_types where g_type = 2 order by g_type desc, type, sub_type;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {
			if( sqlite3_column_int(ppStmt,0) != 0x63 ) {
				if( sqlite3_column_int(ppStmt,0) <= 0x11 ||
					sqlite3_column_int(ppStmt,0) == 0x20 ||
					(sqlite3_column_int(ppStmt,0) >= 0x2a && sqlite3_column_int(ppStmt,0) <= 0x30) ||
					sqlite3_column_int(ppStmt,0) == 0x28 ||
					(sqlite3_column_int(ppStmt,0) >= 0x64 && sqlite3_column_int(ppStmt,0) <= 0x67) ) {

						char *s = sqlite3_mprintf("INSERT INTO MDR4(id,type,sub_type,level) VALUES(%i,%i,%i,%i);",				
							id,
							sqlite3_column_int(ppStmt,0), //
							sqlite3_column_int(ppStmt,1), //
							sqlite3_column_int(ppStmt,2)); //				
						sqlite3_exec(getMDRbase(),s,NULL,NULL,NULL);
						sqlite3_free(s);
						id++;
				}
			}
		}
		rc = sqlite3_step( ppStmt );
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );

//	sqlite3_exec(getMDRbase(),"END TRANSACTION",NULL,NULL,NULL);
}

void MdrCreator::index_mdr12() {
	//indexy do MDR11
	//MDR12 (t_text text,mdr11_id int)
	int			p_record = 1;
	char		*s;
	sqlite3_stmt *ppStmt;
	return;

	int	rc = sqlite3_prepare_v2(getMDRbase(),"select id, substr(t_text,0,4) as s_text from MDR11 group by s_text order by s_text;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {
			if( sqlite3_column_int(ppStmt,0) >= p_record ) {
				p_record = sqlite3_column_int(ppStmt,0) + MRD12_INDEX_CONST;


				s = sqlite3_mprintf("INSERT INTO MDR12(t_text,mdr11_id) VALUES('%q',%i);",
					sqlite3_column_text(ppStmt,1), //
					sqlite3_column_int(ppStmt,0));
				sqlite3_exec(getMDRbase(),s,NULL,NULL,NULL);
				sqlite3_free(s);
			}
		}
		rc = sqlite3_step( ppStmt );
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );

//	sqlite3_exec(getMDRbase(),"END TRANSACTION",NULL,NULL,NULL);
}

void MdrCreator::index_mdr8() {
	//indexy do MDR7
	//co 10000
	//MDR8 (t_text text,mdr7_id int)
	int			p_record = 1;
	int			id = 1;
	char		*s;
	sqlite3_stmt *ppStmt;

	return;

	//collate???
	int	rc = sqlite3_prepare_v2(getMDRbase(),"select id, substr(t_text,0,4) as s_text from MDR7 group by s_text order by s_text;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {
			if( sqlite3_column_int(ppStmt,0) >= p_record ) {
				p_record = sqlite3_column_int(ppStmt,0) + MRD8_INDEX_CONST;

				s = sqlite3_mprintf("INSERT INTO MDR8(id,t_text,mdr7_id) VALUES(%i,'%q',%i);",
					id,
					sqlite3_column_text(ppStmt,1), //
					sqlite3_column_int(ppStmt,0));
				sqlite3_exec(getMDRbase(),s,NULL,NULL,NULL);
				sqlite3_free(s);
				id++;
			}
		}
		rc = sqlite3_step( ppStmt );
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );
}

void MdrCreator::index_mdr5() {
//MDR5 (mdr1_id int,id int,mdr15_id int,lbl4 int,lbl1 int,mdr13_id int,mdr14_id int,mdr20_id int,t_text text)

	int		lbl15_index;
	int		last_lbl15_index;

	int		lbl15_index2;
	int		last_lbl15_index2;

	int		mdr13_id = -1;
	int		mdr14_id = -1;
	int		unique;
	char	*s;
	sqlite3_stmt *ppStmt;
	sqlite3_stmt *ppStmt2;
	int		mdr1_map_index = 1;
//	sqlite3_exec(getMDRbase(),"BEGIN TRANSACTION",NULL,NULL,NULL);

	last_lbl15_index = -1;
	last_lbl15_index2 = -1;
	//int	rc = sqlite3_prepare_v2(getMDRbase(),"SELECT DISTINCT MDR1.id, T43_MIASTA.id_53, T43_MIASTA.id_4e, T43_MIASTA.lbl1, T43_MIASTA.t_text, T43_MIASTA.map_id,T43_MIASTA.t_index, t53_regiony.t_text FROM T43_MIASTA, MDR1, t53_regiony WHERE t53_regiony.t_index = t43_miasta.id_53 and t53_regiony.map_id = mdr1.map_id and T43_MIASTA.map_id = MDR1.map_id ORDER BY T43_MIASTA.t_text, t53_regiony.t_text;",-1,&ppStmt,NULL);
	int	rc = sqlite3_prepare_v2(getMDRbase(),"SELECT DISTINCT MDR1.id, T43_MIASTA.id_53, T43_MIASTA.id_4e, T43_MIASTA.lbl1, T43_MIASTA.t_text, T43_MIASTA.map_id,T43_MIASTA.t_index, t53_regiony.t_text, t4e_kraje.t_text FROM t4e_kraje, T43_MIASTA, MDR1, t53_regiony WHERE t4e_kraje.t_index = t53_regiony.id_4e and t4e_kraje.map_id = mdr1.map_id and t53_regiony.t_index = t43_miasta.id_53 and t53_regiony.map_id = mdr1.map_id and T43_MIASTA.map_id = MDR1.map_id ORDER BY T43_MIASTA.t_text, t53_regiony.t_text, t4e_kraje.t_text;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {
			//sqlite3_column_text(ppStmt,7) // - region
			mdr13_id = -1;
			mdr14_id = -1;

			lbl15_index = add_lbl15(std::string((char*)sqlite3_column_text(ppStmt,4)));
			lbl15_index2 = add_lbl15(std::string((char*)sqlite3_column_text(ppStmt,7))); //region
			unique = 0;
			if( last_lbl15_index == lbl15_index && last_lbl15_index2 == lbl15_index2 ) {
				unique = 1;
			}
			last_lbl15_index = lbl15_index;
			last_lbl15_index2 = lbl15_index2;

			//jesli region
			if( sqlite3_column_int(ppStmt,1) > -1 ) {

				s = sqlite3_mprintf("SELECT DISTINCT T53_REGIONY.mdr13_id FROM T53_REGIONY WHERE T53_REGIONY.t_index = %i AND T53_REGIONY.map_id = %i;",
					sqlite3_column_int(ppStmt,1),sqlite3_column_int(ppStmt,5));
				rc = sqlite3_prepare_v2(getMDRbase(),s,-1,&ppStmt2,NULL);
				sqlite3_free(s);
				rc = sqlite3_step( ppStmt2 );
				if ( rc == SQLITE_ROW ) {
					mdr13_id = sqlite3_column_int(ppStmt2,0);
				}
				sqlite3_finalize( ppStmt2 );
			} else if( sqlite3_column_int(ppStmt,2) > -1 ) {
				s = sqlite3_mprintf("SELECT DISTINCT T4E_KRAJE.mdr14_id FROM T4E_KRAJE WHERE T4E_KRAJE.t_index = %i AND T4E_KRAJE.map_id = %i;",
					sqlite3_column_int(ppStmt,2),sqlite3_column_int(ppStmt,5));
				rc = sqlite3_prepare_v2(getMDRbase(),s,-1,&ppStmt2,NULL);
				sqlite3_free(s);
				rc = sqlite3_step( ppStmt2 );
				if ( rc == SQLITE_ROW ) {
					mdr14_id = sqlite3_column_int(ppStmt2,0);
				}
				sqlite3_finalize( ppStmt2 );
			}

			s = sqlite3_mprintf("INSERT INTO MDR5(mdr1_id,id ,mdr15_id ,lbl4 ,lbl1 ,mdr13_id ,mdr14_id ,mdr20_id ,t_text, id_43,uniq,region ) VALUES(%i,%i,%i,%i,%i,%i,%i,%i,'%q',%i,%i,'%q');",
				sqlite3_column_int(ppStmt,0), //mdr1 id
				mdr1_map_index, //id
				lbl15_index,
				sqlite3_column_int(ppStmt,6), //lbl4 - index
				sqlite3_column_int(ppStmt,3), //lbl1
				mdr13_id,
				mdr14_id,
				-1, //mdr20
				sqlite3_column_text(ppStmt,4),//miasto
				sqlite3_column_int(ppStmt,6),
				unique,
				sqlite3_column_text(ppStmt,7));
			sqlite3_exec(getMDRbase(),s,NULL,NULL,NULL);
			sqlite3_free(s);

			s = sqlite3_mprintf("UPDATE T43_MIASTA SET mdr5_id = %i WHERE map_id = %i AND t_index = %i;",
				mdr1_map_index,
				sqlite3_column_int(ppStmt,5),
				sqlite3_column_int(ppStmt,6));
			sqlite3_exec(getMDRbase(),s,NULL,NULL,NULL);			
			sqlite3_free(s);

			mdr1_map_index++;
		}
		rc = sqlite3_step( ppStmt );
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );
//	sqlite3_exec(getMDRbase(),"END TRANSACTION",NULL,NULL,NULL);
}

void MdrCreator::index_mdr6() {
//MDR6 (mdr1_id int,id int,lbl1 int,lbl8 int,t_text text)
	int		lbl15_index;
	sqlite3_stmt *ppStmt;
	int		mdr1_map_index = 1;
//	sqlite3_exec(getMDRbase(),"BEGIN TRANSACTION",NULL,NULL,NULL);

	int	rc = sqlite3_prepare_v2(getMDRbase(),"SELECT DISTINCT MDR1.id, zip.t_index, zip.t_text FROM zip,mdr1 WHERE zip.map_id = MDR1.map_id ORDER BY zip.t_text;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {
			lbl15_index = add_lbl15(std::string((char*)sqlite3_column_text(ppStmt,2)));
			char *s = sqlite3_mprintf("INSERT INTO MDR6(mdr1_id ,id ,lbl8, t_text ) VALUES(%i,%i,%i,'%q');",
				sqlite3_column_int(ppStmt,0),mdr1_map_index, sqlite3_column_int(ppStmt,1),sqlite3_column_text(ppStmt,2));

			sqlite3_exec(getMDRbase(),s,NULL,NULL,NULL);
			sqlite3_free(s);

			mdr1_map_index++;
		}
		rc = sqlite3_step( ppStmt );
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );

//	sqlite3_exec(getMDRbase(),"END TRANSACTION",NULL,NULL,NULL);
}

void MdrCreator::index_mdr20_mdr21_mdr22() {
	//20 - miasta
	//21 - regiony
	//22 - kraje

	//MDR20
	
	int		last_lbl15_city = -1;
	int		last_lbl15_region = -1;
	int		last_lbl15_country = -1;
	int		last_map_id = -1;
	int		last_lbl15_index;
	int		id_53;
	int		unique;

	int		last_lbl15_index2;
	int		unique2;

	char	*s;
	sqlite3_stmt *ppStmt;
	sqlite3_stmt *ppStmt2;
	int		mdr1_map_index = 1;
	int		mdr2_map_index = 1;
	int		mdr7_id;
	bool	ignore_record;
	string	country;
	string	region;
	string	city;
	//dwie ulice z tego samego pliku IMG
	//uniq jesli net
	string	street_name,prev_street_name,prev_full_street_name;


	last_lbl15_index = -1;
	//int	rc = sqlite3_prepare_v2(getMDRbase(),"select distinct mdr7_tmp.t_text, mdr7_tmp.city , mdr7.id, mdr7_tmp.mdr1_id, mdr7_tmp.id_43 from mdr7_tmp,mdr7 where mdr7_tmp.id_43 > -1 and mdr7.mdr1_id = mdr7_tmp.mdr1_id AND mdr7.mdr15_id = mdr7_tmp.mdr15_id order by mdr7_tmp.city ,mdr7_tmp.sort_text,mdr7.id;",-1,&ppStmt,NULL);
	//int	rc = sqlite3_prepare_v2(getMDRbase(),"select mdr7.id, mdr7.mdr15_id, ulice.id_43, mdr7.mdr1_id,t43_miasta.t_text from ulice, mdr7,t43_miasta where ulice.map_id = mdr7.map_id and ulice.lbl1 = mdr7.lbl1 and ulice.id_43 > 0 and t43_miasta.map_id = ulice.map_id and t43_miasta.t_index = ulice.id_43 order by t43_miasta.t_text , mdr7.t_text , mdr7.id;",-1,&ppStmt,NULL);

	cout<<"mdr20";
	//                                                  0        1             2            3            4                5                   6               7
	int	rc = sqlite3_prepare_v2(getMDRbase()," select mdr7.id, mdr7.mdr15_id, ulice.id_43, mdr7.mdr1_id,t43_miasta.t_text,t53_regiony.t_text,t4e_kraje.t_text,mdr7.t_text " \
	" from ulice, mdr7,t43_miasta,t53_regiony,t4e_kraje " \
	" where " \
	" t43_miasta.id_53 = t53_regiony.t_index and t53_regiony.map_id = t43_miasta.map_id and " \
	" t4e_kraje.t_index = t53_regiony.id_4e and t53_regiony.map_id = t4e_kraje.map_id and " \
	" ulice.map_id = mdr7.map_id and " \
	" ulice.lbl1 = mdr7.lbl1 and ulice.id_43 > 0 and " \
	" t43_miasta.map_id = ulice.map_id and t43_miasta.t_index = ulice.id_43 " \
	" order by t43_miasta.t_text , " \
	" t53_regiony.t_text  , " \
	" t4e_kraje.t_text , " \
	//" mdr7.t_text , mdr7.id;",-1,&ppStmt,NULL);
	" mdr7.id;",-1,&ppStmt,NULL);

	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {
			if( last_lbl15_index != sqlite3_column_int(ppStmt,1) ) {
				unique = 0;
				mdr7_id = sqlite3_column_int(ppStmt,0);
			} else {
				unique = 1;
				//??
				//if( last_map_id != sqlite3_column_int(ppStmt,3)  ) {
					mdr7_id = sqlite3_column_int(ppStmt,0);
				//}
			}
			last_map_id = sqlite3_column_int(ppStmt,3);
			last_lbl15_index = sqlite3_column_int(ppStmt,1);

			s = sqlite3_mprintf("INSERT INTO MDR20(id, mdr7_id,id_43,uniq,mdr1_id) VALUES(%i,%i,%i,%i,%i);",
				mdr1_map_index,
				mdr7_id,
				sqlite3_column_int(ppStmt,2),
				unique,
				sqlite3_column_int(ppStmt,3));
			sqlite3_exec(getMDRbase(),s,NULL,NULL,NULL);
			sqlite3_free(s);

			//mdr7_tmp.mdr1_id -3
			//id_43 - 4
			//-> wybrac id_53
			id_53 = MDR_Header::getSelectCount("SELECT id_53 FROM t43_miasta,mdr1 WHERE mdr1.map_id = t43_miasta.map_id AND mdr1.id = %i AND t43_miasta.t_index = %i;",
				sqlite3_column_int(ppStmt,3), //mdr1_id
				sqlite3_column_int(ppStmt,2));
/*
			region = MDR_Header::getSelectString("SELECT t_text FROM t53_regiony, mdr1 WHERE mdr1.map_id = t53_regiony.map_id AND mdr1.id = %i AND t_index = %i;",
				sqlite3_column_int(ppStmt,3),
				id_53);
*/
			if( last_lbl15_city != add_lbl15((char*)sqlite3_column_text(ppStmt,4)) || 
				last_lbl15_region != add_lbl15((char*)sqlite3_column_text(ppStmt,5))
				/*last_lbl15_country != add_lbl15((char*)sqlite3_column_text(ppStmt,6))*/) {	
				
					last_lbl15_city = add_lbl15((char*)sqlite3_column_text(ppStmt,4));
					last_lbl15_region = add_lbl15((char*)sqlite3_column_text(ppStmt,5));
					//last_lbl15_country = add_lbl15((char*)sqlite3_column_text(ppStmt,6));

				s = sqlite3_mprintf("UPDATE MDR5 set MDR20_id=%i WHERE t_text = '%q' and region = '%q';",
					mdr1_map_index,
					sqlite3_column_text(ppStmt,4),
					sqlite3_column_text(ppStmt,5));
			
				sqlite3_exec(getMDRbase(),s,NULL,NULL,NULL);			
				sqlite3_free(s);
			}
			mdr1_map_index++;
		}
		rc = sqlite3_step( ppStmt );
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );
	cout<<".end";

	//MDR21
	cout<<"mdr21";

	mdr1_map_index = 1;
	last_lbl15_index = -1;
	prev_street_name = "";
	//rc = sqlite3_prepare_v2(getMDRbase(),"select DISTINCT mdr7_tmp.t_text , mdr7_tmp.region , mdr7.id, mdr7_tmp.mdr1_id, mdr7_tmp.id_53 from mdr7_tmp,mdr7 where mdr7.mdr1_id = mdr7_tmp.mdr1_id AND mdr7.mdr15_id = mdr7_tmp.mdr15_id AND mdr7_tmp.id_53 > -1 order by mdr7_tmp.region ,mdr7_tmp.sort_text,mdr7.id",-1,&ppStmt,NULL);
	rc = sqlite3_prepare_v2(getMDRbase(),"select mdr7.id, mdr7.mdr15_id, ulice.id_53, mdr7.mdr1_id, mdr7.sort_text, mdr7.t_text from ulice, mdr7,t53_regiony where ulice.map_id = mdr7.map_id and ulice.lbl1 = mdr7.lbl1 and ulice.id_53 > 0 and t53_regiony.map_id = ulice.map_id and t53_regiony.t_index = ulice.id_53 order by t53_regiony.t_text, mdr7.id;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {
			ignore_record = false;

			street_name = (char*)sqlite3_column_text(ppStmt,4);
			if( street_name != prev_street_name ) {
			//if( last_lbl15_index != sqlite3_column_int(ppStmt,1) ) {
				unique = 0;
				//mdr7_id = sqlite3_column_int(ppStmt,2);
			} else {
				unique = 1;
				if( prev_full_street_name == (char*)sqlite3_column_text(ppStmt,5) && last_map_id == sqlite3_column_int(ppStmt,3) ) {
					ignore_record = true;
				}
			}
			last_map_id = sqlite3_column_int(ppStmt,3);
			//last_lbl15_index = sqlite3_column_int(ppStmt,1);
			prev_street_name = street_name;
			prev_full_street_name = (char*)sqlite3_column_text(ppStmt,5);

			if( ignore_record == false ) {
				s = sqlite3_mprintf("INSERT INTO MDR21(id, mdr7_id, id_53, uniq,mdr1_id) VALUES(%i,%i, %i,%i,%i);",
					mdr1_map_index,
					sqlite3_column_int(ppStmt,0),
					sqlite3_column_int(ppStmt,2),
					unique,
					sqlite3_column_int(ppStmt,3));
				sqlite3_exec(getMDRbase(),s,NULL,NULL,NULL);
				sqlite3_free(s);

				mdr1_map_index++;
			} else {
				//ostatni ustawic na unique 1
				s = sqlite3_mprintf("UPDATE MDR21 set mdr7_id=%i WHERE id = %i;",
					sqlite3_column_int(ppStmt,0),
					mdr1_map_index-1);
				sqlite3_exec(getMDRbase(),s,NULL,NULL,NULL);
				sqlite3_free(s);
			}
		}
		rc = sqlite3_step( ppStmt );
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );
	cout<<".end mdr22";

	//sqlite3_exec(getMDRbase(),"INSERT INTO MDR21(mdr7_id,flag) select distinct mdr7.id,1 from mdr7 where length(region)>0 order by region,sort_text;",NULL,NULL,NULL);
	//MDR22
	//sqlite3_exec(getMDRbase(),"INSERT INTO MDR22(mdr7_id,flag) select distinct mdr7.id,1 from mdr7 where length(country)>0 order by country,sort_text;",NULL,NULL,NULL);
	mdr1_map_index = 1;
	last_lbl15_index = -1;
	prev_street_name = "";
	//rc = sqlite3_prepare_v2(getMDRbase(),"select mdr7_tmp.t_text, mdr7_tmp.country, mdr7.id, mdr7_tmp.mdr1_id, mdr7_tmp.id_4e from mdr7_tmp,mdr7 where mdr7_tmp.id_4e > -1 AND mdr7.mdr1_id = mdr7_tmp.mdr1_id AND mdr7.mdr15_id = mdr7_tmp.mdr15_id order by mdr7_tmp.country,mdr7_tmp.sort_text,mdr7.id;",-1,&ppStmt,NULL);
	rc = sqlite3_prepare_v2(getMDRbase(),"select mdr7.id, mdr7.mdr15_id, ulice.id_4e, mdr7.mdr1_id , mdr7.sort_text, mdr7.t_text from ulice, mdr7,t4e_kraje where ulice.map_id = mdr7.map_id and ulice.lbl1 = mdr7.lbl1 and ulice.id_4e > 0 and t4e_kraje.map_id = ulice.map_id and t4e_kraje.t_index = ulice.id_4e order by t4e_kraje.t_text , mdr7.id;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {
			ignore_record = false;

			/*
			if( last_lbl15_index != sqlite3_column_int(ppStmt,1) ) {
				unique = 0;
			} else {
				unique = 1;
				if( last_map_id == sqlite3_column_int(ppStmt,3) ) {
					ignore_record = true;
				}
			}
			*/
			street_name = (char*)sqlite3_column_text(ppStmt,4);
			if( street_name != prev_street_name ) {
				unique = 0;
			} else {
				unique = 1;
				if( prev_full_street_name == (char*)sqlite3_column_text(ppStmt,5) && last_map_id == sqlite3_column_int(ppStmt,3) ) {
					ignore_record = true;
				}
			}

			last_map_id = sqlite3_column_int(ppStmt,3);
			//last_lbl15_index = sqlite3_column_int(ppStmt,1);
			prev_full_street_name = (char*)sqlite3_column_text(ppStmt,5);
			prev_street_name = street_name;

			if( ignore_record == false ) {
				s = sqlite3_mprintf("INSERT INTO MDR22(id, mdr7_id,id_4e,uniq,mdr1_id) VALUES(%i,%i,%i,%i,%i);",
					mdr1_map_index,
					sqlite3_column_int(ppStmt,0),
					sqlite3_column_int(ppStmt,2),
					unique,
					sqlite3_column_int(ppStmt,3));
				sqlite3_exec(getMDRbase(),s,NULL,NULL,NULL);
				sqlite3_free(s);

				mdr1_map_index++;
			} else {
				//ostatni ustawic na unique 1
				s = sqlite3_mprintf("UPDATE MDR22 set mdr7_id=%i WHERE id = %i;",
					sqlite3_column_int(ppStmt,0),
					mdr1_map_index-1);
				sqlite3_exec(getMDRbase(),s,NULL,NULL,NULL);
				sqlite3_free(s);
			}
		}
		rc = sqlite3_step( ppStmt );
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );
	cout<<".end mdr23";

	//MDR23
	//MDR23 (address int,id int,mdr1_id int,mdr13_id int, mdr14_id int, lbl1 int, t_text text,unique int)
	mdr1_map_index = 1;
	last_lbl15_index = -1;
	rc = sqlite3_prepare_v2(getMDRbase(),"select mdr13.mdr1_id, mdr13.id as mdr13_id, mdr14.id as mdr14_id, mdr13.lbl1, mdr13.t_text, mdr14.t_text from mdr13, mdr14 where mdr13.mdr14_id = mdr14.id order by mdr13.t_text, mdr14.t_text;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {			
			if( last_lbl15_index != add_lbl15((char*)sqlite3_column_text(ppStmt,4)) )
				unique = 0;
			else
				unique = 1;
			last_lbl15_index = add_lbl15((char*)sqlite3_column_text(ppStmt,4));

			s = sqlite3_mprintf("INSERT INTO MDR23(id,mdr1_id,mdr13_id,mdr14_id,lbl1,t_text,uniq) VALUES(%i,%i,%i,%i,%i,'%q',%i);",
				mdr1_map_index,
				sqlite3_column_int(ppStmt,0),
				sqlite3_column_int(ppStmt,1),
				sqlite3_column_int(ppStmt,2),
				sqlite3_column_int(ppStmt,3),
				sqlite3_column_text(ppStmt,4),
				unique);
			sqlite3_exec(getMDRbase(),s,NULL,NULL,NULL);
			sqlite3_free(s);

			mdr1_map_index++;
		}
		rc = sqlite3_step( ppStmt );
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );
	cout<<".end mdr24";

	//MDR24
	mdr1_map_index = 1;
	rc = sqlite3_prepare_v2(getMDRbase(),"select mdr1_id, id, lbl1, t_text from mdr14 order by t_text;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	last_lbl15_index = -1;
	do {
		if ( rc == SQLITE_ROW ) {			
			if( last_lbl15_index != add_lbl15((char*)sqlite3_column_text(ppStmt,3)) )
				unique = 0;
			else
				unique = 1;
			last_lbl15_index = add_lbl15((char*)sqlite3_column_text(ppStmt,3));

			s = sqlite3_mprintf("INSERT INTO MDR24(id,mdr1_id,mdr14_id,lbl1,t_text,uniq) VALUES(%i,%i,%i,%i,'%q',%i);",
				mdr1_map_index,
				sqlite3_column_int(ppStmt,0),
				sqlite3_column_int(ppStmt,1),
				sqlite3_column_int(ppStmt,2),			
				sqlite3_column_text(ppStmt,3),
				unique);
			sqlite3_exec(getMDRbase(),s,NULL,NULL,NULL);
			sqlite3_free(s);

			mdr1_map_index++;
		}
		rc = sqlite3_step( ppStmt );
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );
	cout<<".end mdr25";

	//MDR25 (address int,id int,mdr5_id int)
	{
		int last_lbl15_city,last_lbl15_region,last_lbl15_country;
		last_lbl15_city = -1;
		last_lbl15_region = -1;
		last_lbl15_country = -1;
		last_map_id = -1;
		mdr1_map_index = 1;
		rc = sqlite3_prepare_v2(getMDRbase(),"select mdr5.id as mdr5_id, mdr5.mdr20_id, mdr5.t_text as city, mdr13.t_text as region, mdr14.t_text as country, mdr5.mdr1_id from mdr5, mdr13, mdr14 where mdr5.mdr13_id = mdr13.id and mdr13.mdr14_id = mdr14.id order by country, region, city;",-1,&ppStmt,NULL);
		rc = sqlite3_step( ppStmt );
		last_lbl15_index = -1;
		do {
			if ( rc == SQLITE_ROW ) {
				if( last_lbl15_city != add_lbl15((char*)sqlite3_column_text(ppStmt,2)) ||
					last_lbl15_region != add_lbl15((char*)sqlite3_column_text(ppStmt,3)) || 
					last_lbl15_country != add_lbl15((char*)sqlite3_column_text(ppStmt,4)) ) {
					//last_map_id != sqlite3_column_int(ppStmt,5) ) {

						last_map_id = sqlite3_column_int(ppStmt,5);
						last_lbl15_city = add_lbl15((char*)sqlite3_column_text(ppStmt,2));
						last_lbl15_region = add_lbl15((char*)sqlite3_column_text(ppStmt,3));
						last_lbl15_country = add_lbl15((char*)sqlite3_column_text(ppStmt,4));

						s = sqlite3_mprintf("INSERT INTO MDR25(id,mdr5_id,city,region,country) VALUES(%i,%i,'%q','%q','%q');",
							mdr1_map_index,
							sqlite3_column_int(ppStmt,0),
							sqlite3_column_text(ppStmt,2),
							sqlite3_column_text(ppStmt,3),
							sqlite3_column_text(ppStmt,4));
						sqlite3_exec(getMDRbase(),s,NULL,NULL,NULL);
						sqlite3_free(s);

						mdr1_map_index++;
				}
			}
			rc = sqlite3_step( ppStmt );
		} while(rc == SQLITE_ROW);
		sqlite3_finalize( ppStmt );
	}
	cout<<".end mdr27";

//mdr27 - order by region,city
	//-> mdr5 
	{
		mdr1_map_index = 1;
		int last_lbl15_city = -1;
		int last_lbl15_region = -1;
		int last_lbl15_country = -1;

		rc = sqlite3_prepare_v2(getMDRbase(),"select mdr5.id, mdr5.mdr1_id, mdr5.t_text as city,t53_regiony.t_text as region from mdr5, t43_miasta, t53_regiony, mdr1 where mdr5.id_43 = t43_miasta.t_index and mdr5.mdr1_id = mdr1.id and mdr1.map_id = t43_miasta.map_id and t43_miasta.id_53 = t53_regiony.t_index and t53_regiony.map_id = mdr1.map_id order by t53_regiony.t_text, mdr5.t_text;",-1,&ppStmt,NULL);
		rc = sqlite3_step( ppStmt );
		last_lbl15_index = -1;
		last_map_id = -1;
		do {
			if ( rc == SQLITE_ROW ) {			
				if( last_lbl15_city != add_lbl15((char*)sqlite3_column_text(ppStmt,2)) ||
					last_lbl15_region != add_lbl15((char*)sqlite3_column_text(ppStmt,3)) ) {
					//last_map_id != sqlite3_column_int(ppStmt,1)) {
					//last_lbl15_country != add_lbl15((char*)sqlite3_column_text(ppStmt,4)) ) {

						last_map_id = sqlite3_column_int(ppStmt,1);
						last_lbl15_city = add_lbl15((char*)sqlite3_column_text(ppStmt,2));
						last_lbl15_region = add_lbl15((char*)sqlite3_column_text(ppStmt,3));
						//last_lbl15_country = add_lbl15((char*)sqlite3_column_text(ppStmt,4));
						
						s = sqlite3_mprintf("INSERT INTO MDR27(id,mdr5_id,city,region) VALUES(%i,%i,'%q','%q');",
							mdr1_map_index,
							sqlite3_column_int(ppStmt,0),
							sqlite3_column_text(ppStmt,2),
							sqlite3_column_text(ppStmt,3));
						sqlite3_exec(getMDRbase(),s,NULL,NULL,NULL);
						sqlite3_free(s);

						mdr1_map_index++;
				}
			}
			rc = sqlite3_step( ppStmt );
		} while(rc == SQLITE_ROW);
		sqlite3_finalize( ppStmt );
	}
//mdr28 - posortowane regiony (unique)
	//-> mdr23(posortowane regiony), mdr21(ulice wg regionow), mdr27 (miasta wg regionow)
	//dla kazdego mdr23 - dodaje tylko unique do mdr28 i zbieram wszystkie przynalezne do grupy (mdr1_id + mdr13_id)
	//znajduje poczatek w mdr21 dla danego regionu
	//znajduje poczatek w mdr27 dla danego regionu

	cout<<".end mdr28"<<endl;

	{
		mdr1_map_index = 1;
		sqlite3_stmt *ppStmt2;

		int progress_limit;
		int progress;
		int	p = -1;

		int mdr1_id;
		int mdr13_id;
		int mdr14_id;
		int mdr21_id;
		int mdr23_id;
		int mdr27_id;
		int id_53;
		string region;

		rc = sqlite3_prepare_v2(getMDRbase(),"select count(*) from mdr23 order by id;",-1,&ppStmt,NULL);
		rc = sqlite3_step( ppStmt );
		progress_limit = sqlite3_column_int(ppStmt,0);
		sqlite3_finalize( ppStmt );
	
		cout<<"To process:"<<progress_limit<<endl;
		progress = 0;

		rc = sqlite3_prepare_v2(getMDRbase(),"select id, mdr1_id, mdr13_id,uniq,t_text,mdr14_id from mdr23 order by id;",-1,&ppStmt,NULL);
		rc = sqlite3_step( ppStmt );
		last_lbl15_index = -1;
		do {
			if ( rc == SQLITE_ROW ) {			
				mdr23_id = sqlite3_column_int(ppStmt,0);

				if( !sqlite3_column_int(ppStmt,3) == 1 ) {//unique
					region = (char*)sqlite3_column_text(ppStmt,4);
					mdr1_id = sqlite3_column_int(ppStmt,1);
					mdr13_id = sqlite3_column_int(ppStmt,2);
					mdr14_id = sqlite3_column_int(ppStmt,5);
					//nowy MDR28
					// -> mdr1_id, mdr13_id -> wibierzmy id_53
					//    select t53_regiony.* from t53_regiony, mdr1 where mdr1.map_id = t53_regiony.map_id and 'mdr1.id' = map1_id and mdr13_id = 'mdr13_id'
					/*
					s = sqlite3_mprintf("select t53_regiony.t_index from t53_regiony, mdr1 where mdr1.map_id = t53_regiony.map_id and mdr1.id = %i and mdr13_id = %i;",
						mdr1_id,mdr13_id);						
					rc = sqlite3_prepare_v2(getMDRbase(),s,-1,&ppStmt2,NULL);
					sqlite3_free(s);
					rc = sqlite3_step( ppStmt2 );
					if( rc  == SQLITE_ROW ) {
						id_53 = sqlite3_column_int(ppStmt2,0);
					} else
						id_53 = 0;
					sqlite3_finalize( ppStmt2 );
					*/
					//
					// -> wybieram mdr21_id - pierwszy rekord
					
					s = sqlite3_mprintf("select mdr21.id from  mdr21, mdr1, t53_regiony where t53_regiony.t_text = '%q' and  mdr21.mdr1_id = mdr1.id and t53_regiony.map_id = mdr1.map_id and mdr21.id_53 = t53_regiony.t_index order by mdr21.id  limit 1;",
						region.c_str());						
					rc = sqlite3_prepare_v2(getMDRbase(),s,-1,&ppStmt2,NULL);
					sqlite3_free(s);
					rc = sqlite3_step( ppStmt2 );
					if( rc  == SQLITE_ROW ) {
						mdr21_id = sqlite3_column_int(ppStmt2,0);
					} else
						mdr21_id = 0;
					sqlite3_finalize( ppStmt2 );

					//
					// -> wybieram mdr27_id					 
					s = sqlite3_mprintf("select mdr27.id from mdr27 where region = '%q' order by mdr27.id limit 1;",
						region.c_str());
					rc = sqlite3_prepare_v2(getMDRbase(),s,-1,&ppStmt2,NULL);
					sqlite3_free(s);
					rc = sqlite3_step( ppStmt2 );
					if( rc  == SQLITE_ROW ) {
						mdr27_id = sqlite3_column_int(ppStmt2,0);
					} else
						mdr27_id = 0;
					sqlite3_finalize( ppStmt2 );


					s = sqlite3_mprintf("INSERT INTO MDR28(id ,mdr1_id ,mdr23_id, t_text ,mdr21_id ,mdr27_id,mdr14_id ) VALUES(%i,%i,%i,'%q',%i,%i,%i);",
						mdr1_map_index,
						mdr1_id,mdr23_id,
						sqlite3_column_text(ppStmt,4),
						mdr21_id,mdr27_id,mdr14_id);
					sqlite3_exec(getMDRbase(),s,NULL,NULL,NULL);
					sqlite3_free(s);

					mdr1_map_index++;
				}
				
				progress++;
				if( p != int(((double)progress / (double)progress_limit )*100.0) ) {
					p = int(((double)progress / (double)progress_limit )*100.0);
					cout << "\r" << p;
					if(p < 10)
						cout << " ";
					if(p < 100)
						cout << " ";
					cout << "%";
				}
			}
			rc = sqlite3_step( ppStmt );
		} while(rc == SQLITE_ROW);
		sqlite3_finalize( ppStmt );
	}
//mdr26
	//-> mdr28	
	cout<<".end mdr26";

	mdr1_map_index = 1;
	rc = sqlite3_prepare_v2(getMDRbase(),"select distinct mdr28.id, mdr28.mdr1_id, mdr28.t_text ,mdr14.t_text from mdr28, mdr14, mdr23 where mdr14.id = mdr28.mdr14_id and mdr28.mdr23_id = mdr23.id order by mdr14.t_text, mdr23.t_text;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	last_lbl15_index = -1;
	do {
		if ( rc == SQLITE_ROW ) {			
			s = sqlite3_mprintf("INSERT INTO MDR26(id,mdr1_id,mdr28_id) VALUES(%i,%i,%i);",
				mdr1_map_index,
				sqlite3_column_int(ppStmt,1),
				sqlite3_column_int(ppStmt,0));
			sqlite3_exec(getMDRbase(),s,NULL,NULL,NULL);
			sqlite3_free(s);

			mdr1_map_index++;
		}
		rc = sqlite3_step( ppStmt );
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );

//mdr29
	cout<<".end mdr29";

	//-> mdr24, mdr22, mdr25, mdr26
	//select * from mdr23
	{
		mdr1_map_index = 1;
		sqlite3_stmt *ppStmt2;
		int mdr1_id;
		int mdr14_id;
		int mdr22_id;
		int mdr24_id;
		int mdr25_id;
		int mdr26_id;
		int id_4e;
		string	country;
		rc = sqlite3_prepare_v2(getMDRbase(),"select id, mdr1_id, mdr14_id, uniq,t_text from mdr24 order by id;",-1,&ppStmt,NULL);
		rc = sqlite3_step( ppStmt );
		last_lbl15_index = -1;
		do {
			if ( rc == SQLITE_ROW ) {			
				mdr24_id = sqlite3_column_int(ppStmt,0);

				if( !sqlite3_column_int(ppStmt,3) == 1 ) {//unique
					country = (char*)sqlite3_column_text(ppStmt,4);
					mdr1_id = sqlite3_column_int(ppStmt,1);
					//mdr14_id = sqlite3_column_int(ppStmt,2);
					
					//nowy MDR29
					// -> mdr1_id, mdr14_id -> wibierzmy id_4e
					//    select t53_regiony.* from t53_regiony, mdr1 where mdr1.map_id = t53_regiony.map_id and 'mdr1.id' = map1_id and mdr13_id = 'mdr13_id'
					/*
					s = sqlite3_mprintf("select t4e_kraje.t_index from t4e_kraje, mdr1 where mdr1.map_id = t4e_kraje.map_id and mdr1.id = %i and mdr14_id = %i;",
						mdr1_id,mdr14_id);						
					rc = sqlite3_prepare_v2(getMDRbase(),s,-1,&ppStmt2,NULL);
					sqlite3_free(s);
					rc = sqlite3_step( ppStmt2 );
					if( rc  == SQLITE_ROW ) {
						id_4e = sqlite3_column_int(ppStmt2,0);
					} else
						id_4e = 0;
					sqlite3_finalize( ppStmt2 );
					*/
					//
					// -> wybieram mdr22_id - pierwszy rekord
					
					s = sqlite3_mprintf("select mdr22.id from  mdr22, mdr1, t4e_kraje where t4e_kraje.t_text = '%q' and mdr22.mdr1_id = mdr1.id and t4e_kraje.map_id = mdr1.map_id and mdr22.id_4e = t4e_kraje.t_index order by mdr22.id  limit 1;",
						country.c_str());						
					rc = sqlite3_prepare_v2(getMDRbase(),s,-1,&ppStmt2,NULL);
					sqlite3_free(s);
					rc = sqlite3_step( ppStmt2 );
					if( rc  == SQLITE_ROW ) {
						mdr22_id = sqlite3_column_int(ppStmt2,0);
					} else
						mdr22_id = 0;
					sqlite3_finalize( ppStmt2 );

					//
					// -> wybieram mdr26_id
					s = sqlite3_mprintf("select mdr26.id, mdr28.t_text, mdr14.t_text from mdr26, mdr28, mdr14 where mdr26.mdr28_id = mdr28.id and mdr28.mdr14_id = mdr14.id and mdr14.t_text = '%q' order by mdr26.id  limit 1;",
						country.c_str());
					rc = sqlite3_prepare_v2(getMDRbase(),s,-1,&ppStmt2,NULL);
					sqlite3_free(s);
					rc = sqlite3_step( ppStmt2 );
					if( rc  == SQLITE_ROW ) {
						mdr26_id = sqlite3_column_int(ppStmt2,0);
					} else
						mdr26_id = 0;
					sqlite3_finalize( ppStmt2 );

					//
					// -> wybieram mdr25_id
					s = sqlite3_mprintf("select mdr25.id,country from mdr25 where country = '%q' order by mdr25.id limit 1;",
						country.c_str());
					rc = sqlite3_prepare_v2(getMDRbase(),s,-1,&ppStmt2,NULL);
					sqlite3_free(s);
					rc = sqlite3_step( ppStmt2 );
					if( rc  == SQLITE_ROW ) {
						mdr25_id = sqlite3_column_int(ppStmt2,0);
					} else
						mdr25_id = 0;
					sqlite3_finalize( ppStmt2 );

					s = sqlite3_mprintf("INSERT INTO MDR29(id ,mdr1_id ,mdr24_id , t_text ,mdr22_id ,mdr25_id ,mdr26_id ) VALUES(%i,%i,%i,'%q',%i,%i,%i);",
						mdr1_map_index,
						mdr1_id,
						mdr24_id,
						sqlite3_column_text(ppStmt,4),
						mdr22_id,mdr25_id,mdr26_id);
					sqlite3_exec(getMDRbase(),s,NULL,NULL,NULL);
					sqlite3_free(s);

					mdr1_map_index++;
				}
			}
			rc = sqlite3_step( ppStmt );
		} while(rc == SQLITE_ROW);
		sqlite3_finalize( ppStmt );
	}
	cout<<".end";

//	sqlite3_exec(getMDRbase(),"END TRANSACTION",NULL,NULL,NULL);
}

void MdrCreator::index_mdr7() {
//MDR7 (mdr1_id int,id int,mdr15_id int,lbl1 int,nflag int,mdr5_id int,t_text text

	/*
	nflag - od której litery sortowane
	xflag - pomocnicza kolejnoœæ
	 3 - dla nastepnego - gdy jest inne
	 
	 1 - domyœlnie
	 2 - dla nastepnego - gdy takie same
	 4,6,8...

	 0 - dla multibody jesli wyraz jest na poczatku, 3,4,6,8 (brak 1 i 2)
	 
	*/
	int			lbl15_index;
	int			last_lbl15_index = -1;
	int			unique;
	int			progress,progress_limit;
	int			p = -1;
    sqlite3_stmt *ppStmt;
    sqlite3_stmt *ppStmt2;
	char		*s;
	int			mdr1_map_index = 1;

	string		street_name,street_token,net_map,prev_net_map;
	bool		first_multibody;
	bool		second_multibody;
	int			string_pos;
	int			nflag;
	int			xflag;

	string		city_name,region_name,country_name;
	int			city_id,region_id,country_id;
	int			rc;
	rc = sqlite3_prepare_v2(getMDRbase(),"SELECT DISTINCT count(*) FROM ULICE,mdr1 WHERE ULICE.map_id = MDR1.map_id;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	progress_limit = sqlite3_column_int(ppStmt,0);
	sqlite3_finalize( ppStmt );
	
	progress = 0;

	mdr1_map_index = 1;
	last_lbl15_index = -1;
	/*
	nflag - od której litery sortowane
	xflag - pomocnicza kolejnoœæ

		1-bit - ustawiony oznacza pocz¹tek grupy ulic dla tej samej nazwy
		kolejne bity (shift>>1) kolejnosc jaka by byla gdyby wyciac spacje z czêœci poprzedzaj¹cej sam zaindeksowany wyraz
	*/

	if( multibody )	mdrHeader.mdr_header.mdr7_flag |= 0x40;

	//ToDo , xflag !!
	rc = sqlite3_prepare_v2(getMDRbase(),"select mdr1.id, t_text, lbl1, ulice_net.map_id, t_full_text, sort_start_letter, xflag from ulice_net, mdr1 where ulice_net.map_id = mdr1.map_id order by t_text;",-1,&ppStmt,NULL);
	//rc = sqlite3_prepare_v2(getMDRbase(),"select mdr1.id, t_text, lbl1, ulice_net.map_id, t_full_text, sort_start_letter, xflag from ulice_net, mdr1 where ulice_net.map_id = mdr1.map_id order by t_text, xflag;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {

			//unique
			lbl15_index = add_lbl15(std::string((char*)sqlite3_column_text(ppStmt,4)));//full text
			if( lbl15_index != last_lbl15_index ) {
				unique = 0;
				xflag = 1;
			} else {
				unique = 1;
				xflag = 0;
			}
			last_lbl15_index = lbl15_index;

			//lbl15_index = add_lbl15(std::string((char*)sqlite3_column_text(ppStmt,4)));
			if( multibody ) {
				xflag = sqlite3_column_int(ppStmt,6);
				if( xflag > 254 ) {
					mdrHeader.mdr_header.mdr7_flag |= 0x80;
					if( mdrHeader.mdr_header.mdr7_flag & 0x40 )
						mdrHeader.mdr_header.mdr7_flag ^= 0x40;
				}
			}

			char *s = sqlite3_mprintf("INSERT INTO MDR7(mdr1_id ,id ,mdr15_id ,lbl1 ,uniq ,nflag ,xflag ,mdr5_id ,t_text ,sort_text, map_id ) VALUES(%i,%i,%i,%i,%i,%i,%i,%i,'%q','%q',%i);",
				sqlite3_column_int(ppStmt,0), //mdr1.id
				mdr1_map_index, //index
				lbl15_index, //mdr15_id
				sqlite3_column_int(ppStmt,2), //lbl1
				unique,
				sqlite3_column_int(ppStmt,5), //nflag - dla multisort bêdzie wa¿ne (indeks startowy wyznaczaj¹cy wg którego wyrazu sortowanie)
				xflag,
				0, //mdr5_id jest nieustawione tutah
				sqlite3_column_text(ppStmt,4),
				sqlite3_column_text(ppStmt,1),
				sqlite3_column_int(ppStmt,3));
			sqlite3_exec(getMDRbase(),s,NULL,NULL,NULL);
			sqlite3_free(s);

			mdr1_map_index++;
		}
		rc = sqlite3_step( ppStmt );
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );
}

void MdrCreator::index_mdr9_mdr10() {
	//mdr10
//select family,sub_type,mdr11_id from poi order by family,t_text;

/*	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR10 (id int,sub_type int,mdr11_id,unique int);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR9  (id int,family int,mdr10_id);",NULL,NULL,NULL);
*/
	sqlite3_stmt *ppStmt;
	int			mdr1_map_index = 1;
	int			unique;
	int			last_type = -1;
	int			last_sub_type = -1;
	std::string	last_text;
	int			mdr9_id = 1;
//	sqlite3_exec(getMDRbase(),"BEGIN TRANSACTION",NULL,NULL,NULL);

	int	rc = sqlite3_prepare_v2(getMDRbase(),"select family,sub_type,mdr11_id,t_text from poi order by family,t_text,sub_type,mdr11_id;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {

			if( last_type == sqlite3_column_int(ppStmt,0) && last_sub_type == sqlite3_column_int(ppStmt,1) && last_text == string((char*)sqlite3_column_text(ppStmt,3))) {
				unique = 0;
			} else
				unique = 1;

			if( last_type != sqlite3_column_int(ppStmt,0) ) {
				//MDR9  (id int,family int,mdr10_id int
				char *s = sqlite3_mprintf("INSERT INTO MDR9(id ,family,mdr10_id) VALUES(%i,%i,%i);",
					mdr9_id, //id
					sqlite3_column_int(ppStmt,0), //sub_type
					mdr1_map_index);
				sqlite3_exec(getMDRbase(),s,NULL,NULL,NULL);
				sqlite3_free(s);

				mdr9_id++;
			}

			last_type = sqlite3_column_int(ppStmt,0);
			last_sub_type = sqlite3_column_int(ppStmt,1);
			last_text = string((char*)sqlite3_column_text(ppStmt,3));

			char *s = sqlite3_mprintf("INSERT INTO MDR10(id ,sub_type ,mdr11_id,uniq) VALUES(%i,%i,%i,%i);",
				mdr1_map_index, //id
				sqlite3_column_int(ppStmt,1), //sub_type
				sqlite3_column_int(ppStmt,2), //mdr11
				unique);
			sqlite3_exec(getMDRbase(),s,NULL,NULL,NULL);
			sqlite3_free(s);


			mdr1_map_index++;
		}
		rc = sqlite3_step( ppStmt );
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );

//	sqlite3_exec(getMDRbase(),"END TRANSACTION",NULL,NULL,NULL);

}

void MdrCreator::index_mdr11() {
//MDR11 (mdr1_id int,id int,rgn int,tre int,lbl1 int,mdr13_id int,mdr14_id int,mdr5_id int,mdr15_id int t_text text)
	int			lbl15_index;
	int			mdr5_id = -1;
	int			mdr14_id = -1;
	int			mdr13_id = -1;
	int			city_lbl1;
	int			progress,progress_limit;
	int			p = -1;
	char		*s;
	sqlite3_stmt *ppStmt;
	sqlite3_stmt *ppStmt2;
	int			mdr1_map_index = 1;
//	sqlite3_exec(getMDRbase(),"BEGIN TRANSACTION",NULL,NULL,NULL);

	int	rc = sqlite3_prepare_v2(getMDRbase(),"select count(*)  from poi, mdr1 where mdr1.map_id = poi.map_id;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	progress_limit = sqlite3_column_int(ppStmt,0);
	sqlite3_finalize( ppStmt );
	progress = 0;

	//rc = sqlite3_prepare_v2(getMDRbase(),"select mdr1.id, poi.rgn, poi.tre, poi.lbl1, poi.idx_43, poi.t_text, poi.map_id from poi, mdr1 where mdr1.map_id = poi.map_id ORDER BY poi.t_text, poi.family,poi.sub_type,idx_43,mdr1.id,poi.tre;",-1,&ppStmt,NULL);

	rc = sqlite3_prepare_v2(getMDRbase(),"select mdr1.id as id, poi.rgn, poi.tre as tre, poi.lbl1, poi.idx_43, poi.t_text as poi_name, poi.map_id, " \
		"poi.family as family, poi.sub_type as sub_type, '' as country, '' as region, '' as city, poi.id as img_address " \
		"from poi, mdr1 " \
		"where mdr1.map_id = poi.map_id and poi.idx_43 = 0 " \
		"UNION "\
		" select mdr1.id as id, poi.rgn, poi.tre as tre, poi.lbl1, poi.idx_43, poi.t_text as poi_name, poi.map_id, " \
		" poi.family as family, poi.sub_type as sub_type,t4e_kraje.t_text as country, t53_regiony.t_text as region, t43_miasta.t_text as city, poi.id as img_address " \
		" from poi,mdr1, t43_miasta, t53_regiony, t4e_kraje " \
		" where mdr1.map_id = poi.map_id and poi.map_id = t43_miasta.map_id and poi.idx_43 = t43_miasta.t_index " \
		" and t53_regiony.map_id = t43_miasta.map_id and t53_regiony.t_index = t43_miasta.id_53 " \
		" and t4e_kraje.map_id = t53_regiony.map_id and t4e_kraje.t_index = t53_regiony.id_4e " \
		" ORDER BY poi_name , family, sub_type, " \
		" country , region , city ,id, img_address,tre ;",-1,&ppStmt,NULL);

	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {
			lbl15_index = add_lbl15(std::string((char*)sqlite3_column_text(ppStmt,5)));
			mdr5_id = -1;
			mdr13_id = -1;
			mdr14_id = -1;
			//mdr5 or mdr13 or mdr14...
			if( sqlite3_column_int(ppStmt,4) > 0 ) {
				//idx_43
				s = sqlite3_mprintf("select mdr5_id, id_53, id_4e, lbl1 from T43_MIASTA where map_id = %i AND t_index = %i;",
					sqlite3_column_int(ppStmt,6), //mdr_id
					sqlite3_column_int(ppStmt,4) //
					);
				rc = sqlite3_prepare_v2(getMDRbase(),s,-1,&ppStmt2,NULL);
				rc = sqlite3_step( ppStmt2 );

				city_lbl1 = sqlite3_column_int(ppStmt2,3);
				mdr5_id = sqlite3_column_int(ppStmt2,0);
				int id_53 = sqlite3_column_int(ppStmt2,1);
				int id_4e = sqlite3_column_int(ppStmt2,2);

				sqlite3_free(s);
				sqlite3_finalize( ppStmt2 );

				//Obsluga dodatkowo dodanych miast z pliku .idx
				//te ktore dubluja sie miedzy MDR5 i MDR11 - w MDR11 id5 ustawiæ na -1 a zamiast mdr13 !
				if( mdr5_id > -1 && (id_53 > 0 || id_4e > 0) && city_lbl1 == sqlite3_column_int(ppStmt,3) ) {
					mdr5_id = -1;
				}

				if( mdr5_id <= 0 && (id_53 > 0 || id_4e > 0) ) {
					//mdr13 or mdr14..
					s = sqlite3_mprintf("select mdr13_id from t53_regiony where map_id = %i AND t_index = %i;",
						sqlite3_column_int(ppStmt,6), //mdr_id
						id_53 //
						);
					rc = sqlite3_prepare_v2(getMDRbase(),s,-1,&ppStmt2,NULL);
					rc = sqlite3_step( ppStmt2 );

					mdr13_id = sqlite3_column_int(ppStmt2,0);

					sqlite3_free(s);
					sqlite3_finalize( ppStmt2 );

					if( mdr13_id <= 0 ) {
						//mdr14 .. ?
						s = sqlite3_mprintf("select mdr14_id from t4e_kraje where map_id = %i AND t_index = %i;",
							sqlite3_column_int(ppStmt,6), //mdr_id
							id_4e //
							);
						rc = sqlite3_prepare_v2(getMDRbase(),s,-1,&ppStmt2,NULL);
						rc = sqlite3_step( ppStmt2 );

						mdr14_id = sqlite3_column_int(ppStmt2,0);

						sqlite3_free(s);
						sqlite3_finalize( ppStmt2 );
					}
				}
			}

			s = sqlite3_mprintf("INSERT INTO MDR11(mdr1_id ,id ,rgn ,tre ,lbl1 ,mdr13_id ,mdr14_id ,mdr5_id ,mdr15_id, t_text) VALUES(%i,%i,%i,%i,%i,%i,%i,%i,%i,'%q');",
				sqlite3_column_int(ppStmt,0), //mdr_id
				mdr1_map_index, //id
				sqlite3_column_int(ppStmt,1), //rgn
				sqlite3_column_int(ppStmt,2), //tre
				sqlite3_column_int(ppStmt,3), //lbl1
				mdr13_id,
				mdr14_id,
				mdr5_id,
				lbl15_index,
				sqlite3_column_text(ppStmt,5)); //text
			sqlite3_exec(getMDRbase(),s,NULL,NULL,NULL);
			sqlite3_free(s);

			s = sqlite3_mprintf("UPDATE POI SET mdr11_id = %i WHERE map_id = %i AND rgn = %i AND tre = %i;",
				mdr1_map_index,
				sqlite3_column_int(ppStmt,6),
				sqlite3_column_int(ppStmt,1),
				sqlite3_column_int(ppStmt,2));
			sqlite3_exec(getMDRbase(),s,NULL,NULL,NULL);			
			sqlite3_free(s);

			mdr1_map_index++;
			progress++;
/*
			if( p != int(((double)progress / (double)progress_limit )*100.0) ) {
				p = int(((double)progress / (double)progress_limit )*100.0);
				cout << "\r" << p;
				if(p < 10)
					cout << " ";
				if(p < 100)
					cout << " ";
				cout << "%";
			}*/
		}
		rc = sqlite3_step( ppStmt );
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );


//	sqlite3_exec(getMDRbase(),"END TRANSACTION",NULL,NULL,NULL);

}

void MdrCreator::index_mdr1() {
	sqlite3_stmt *ppStmt;
	int		mdr1_map_index = 1;
//	sqlite3_exec(getMDRbase(),"BEGIN TRANSACTION",NULL,NULL,NULL);

	int	rc = sqlite3_prepare_v2(getMDRbase(),"SELECT map_id FROM MDR1 ORDER BY map_id;",-1,&ppStmt,NULL);
	rc = sqlite3_step( ppStmt );
	do {
		if ( rc == SQLITE_ROW ) {
			char *s = sqlite3_mprintf("UPDATE MDR1 set id = %i WHERE map_id = %i;",mdr1_map_index,sqlite3_column_int(ppStmt,0));
			mdr1_map_index++;
			sqlite3_exec(getMDRbase(),s,NULL,NULL,NULL);
			sqlite3_free(s);
		}
		rc = sqlite3_step( ppStmt );
	} while(rc == SQLITE_ROW);
	sqlite3_finalize( ppStmt );

//	sqlite3_exec(getMDRbase(),"END TRANSACTION",NULL,NULL,NULL);
}

void MdrCreator::select_mdr4() {
	int b_result = sqlite3_exec(getMDRbase(),
		"SELECT type, sub_type, max(highest_level) FROM T4C_TYPES GROUP BY type, sub_type;",callback,NULL,NULL);
}

void MdrCreator::select_mdr5() {
	int b_result = sqlite3_exec(getMDRbase(),
		"SELECT DISTINCT T43_MIASTA.map_id,T43_MIASTA.t_text AS lbl15,T43_MIASTA.lbl1,T4E_KRAJE.t_text,T53_REGIONY.t_text,T43_MIASTA.t_index AS lbl4,MDR1.map_index FROM T43_MIASTA LEFT OUTER JOIN T4E_KRAJE ON (T43_MIASTA.id_53=T4E_KRAJE.t_index) LEFT OUTER JOIN T53_REGIONY ON (T43_MIASTA.id_4e=T53_REGIONY.t_index) INNER JOIN MDR1 ON (T43_MIASTA.map_id=MDR1.map_id);",callback,NULL,NULL);
}

void MdrCreator::set_by_region() {
	mdrHeader.mdr_header.mdr21_flag = 0x00011000;
	mdrHeader.mdr_header.mdr22_flag = 0x00011800;
}

void MdrCreator::set_multibody(bool t_multibody) {
	multibody = t_multibody;

	if( multibody ) 
		mdrHeader.mdr_header.mdr7_flag = 0x863; //??
	else
		mdrHeader.mdr_header.mdr7_flag = 0x43; //??
}
