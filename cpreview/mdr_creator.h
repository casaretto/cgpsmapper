/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

#ifndef _MDR_CREATOR_H
#define _MDR_CREATOR_H

#include <string>
#include "ftrcreate.h"
#include "filexor.h"
#include "../sqlite-source/sqlite3.h"

namespace MDR_CREATOR {

#pragma pack(1)
struct MDR_Header_struct
{
	unsigned short m_CodePage;
	unsigned short unk1;
	unsigned short unk2;
	unsigned short unk3;

    unsigned long mdr1_offset;		// this is a maps IDs list
	unsigned long mdr1_size;			
	unsigned short mdr1_rec_size;
	unsigned long mdr1_flag;

    unsigned long mdr2_offset;
	unsigned long mdr2_size;
	unsigned short mdr2_rec_size;
	unsigned long mdr2_flag;

    unsigned long mdr3_offset;
	unsigned long mdr3_size;
	unsigned short mdr3_rec_size;
	unsigned long mdr3_flag;

    unsigned long mdr4_offset;
	unsigned long mdr4_size;
	unsigned short mdr4_rec_size;
	unsigned long mdr4_flag;
	
    unsigned long mdr5_City_offset;			//cityTable
	unsigned long mdr5_size;
	unsigned short mdr5_rec_size;
	unsigned long mdr5_flag;

    unsigned long mdr6_Zip_offset;			//zipTable
	unsigned long mdr6_size;
	unsigned short mdr6_rec_size;
	unsigned long mdr6_flag;

    unsigned long mdr7_Address_offset;			//addressTable
	unsigned long mdr7_size;
	unsigned short mdr7_rec_size;
	unsigned long mdr7_flag;			// flag|0x2 = trimmed ?

    unsigned long mdr8_offset;
	unsigned long mdr8_size;
	unsigned short mdr8_rec_size;
	unsigned long mdr8_flag;
	/*
    òê ðàçìåð íîâîé mdr8 âûñ÷èòûâàåòñÿ ïî ñëåäóþùåé ôîðìóëå
    new_mdr8_size = (new_mdr8_rec_cnt + 1) * new_mdr8_record_size
    new_mdr8_rec_cnt = new_mdr7_record_cnt / ((old_mdr8_flag&0xFF)<< 10)
    new_mdr8_record_size = new_mdr7_ref_size + (old_mdr8_flag>>8)&0xFF
    íàâåðíîå mdr8 íåêèé èíäåêñ
	*/

    unsigned long mdr9_offset;
	unsigned long mdr9_size;
	unsigned short mdr9_rec_size;
	unsigned long mdr9_flag;

    unsigned long mdr10_offset;
	unsigned long mdr10_size;
	unsigned long mdr10_flag;
	
    unsigned long mdr11_Name_offset;			// nameTable
	unsigned long mdr11_size;
	unsigned short mdr11_rec_size;
	unsigned long mdr11_flag;

    unsigned long mdr12_offset;
	unsigned long mdr12_size;
	unsigned short mdr12_rec_size;
	unsigned long mdr12_flag;

    unsigned long mdr13_State_offset;			//state/district table
	unsigned long mdr13_size;
	unsigned short mdr13_rec_size;
	unsigned long mdr13_flag;

    unsigned long mdr14_Country_offset;			//country table
	unsigned long mdr14_size;
	unsigned short mdr14_rec_size;
	unsigned long mdr14_flag;

    unsigned long mdr15_pktStr_offset;		// packed strings
	unsigned long mdr15_size;
	unsigned char mdr15_packedFlag;
	
    unsigned long mdr16_Huffman_offset;		// huffman table
	unsigned long mdr16_size;
	unsigned short mdr16_rec_size;
	unsigned long mdr16_flag;

	unsigned long mdr17_offset;
	unsigned long mdr17_size;
	unsigned long mdr17_flag;
	
    unsigned long mdr18_offset;
	unsigned long mdr18_size;
	unsigned short mdr18_rec_size;
	unsigned long mdr18_flag;

    unsigned long mdr19_offset;
	unsigned long mdr19_size;
	unsigned short mdr19_rec_size;
	unsigned long mdr19_flag;

	//nowe
    unsigned long mdr20_offset;
	unsigned long mdr20_size;
	unsigned short mdr20_rec_size;
	unsigned long mdr20_flag;

    unsigned long mdr21_offset;
	unsigned long mdr21_size;
	unsigned short mdr21_rec_size;
	unsigned long mdr21_flag;

    unsigned long mdr22_offset;
	unsigned long mdr22_size;
	unsigned short mdr22_rec_size;
	unsigned long mdr22_flag;

    unsigned long mdr23_offset;
	unsigned long mdr23_size;
	unsigned short mdr23_rec_size;
	unsigned long mdr23_flag;

    unsigned long mdr24_offset;
	unsigned long mdr24_size;
	unsigned short mdr24_rec_size;
	unsigned long mdr24_flag;

    unsigned long mdr25_offset;
	unsigned long mdr25_size;
	unsigned short mdr25_rec_size;
	unsigned long mdr25_flag;

    unsigned long mdr26_offset;
	unsigned long mdr26_size;
	unsigned short mdr26_rec_size;
	unsigned long mdr26_flag;

    unsigned long mdr27_offset;
	unsigned long mdr27_size;
	unsigned short mdr27_rec_size;
	unsigned long mdr27_flag;

    unsigned long mdr28_offset;
	unsigned long mdr28_size;
	unsigned short mdr28_rec_size;
	unsigned long mdr28_flag;

    unsigned long mdr29_offset;
	unsigned long mdr29_size;
	unsigned short mdr29_rec_size;
	unsigned long mdr29_flag;

    unsigned long mdr30_offset;
	unsigned long mdr30_size;
	unsigned short mdr30_rec_size;
	unsigned long mdr30_flag;

    unsigned long mdr31_offset;
	unsigned long mdr31_size;

    unsigned long mdr32_offset;
	unsigned long mdr32_size;
	unsigned short mdr32_rec_size;
	unsigned long mdr32_flag;

    unsigned long mdr33_offset;
	unsigned long mdr33_size;

    unsigned long mdr34_offset;
	unsigned long mdr34_size;
	unsigned short mdr34_rec_size;
	unsigned long mdr34_flag;

    unsigned long mdr35_offset;
	unsigned long mdr35_size;
	unsigned short mdr35_rec_size;
	unsigned long mdr35_flag;

    unsigned long mdr36_offset;
	unsigned long mdr36_size;
	unsigned short mdr36_rec_size;
	unsigned long mdr36_flag;

    unsigned long mdr37_offset;
	unsigned long mdr37_size;
	unsigned short mdr37_rec_size;
	unsigned long mdr37_flag;

    unsigned long mdr38_offset;
	unsigned long mdr38_size;
	unsigned short mdr38_rec_size;
	unsigned long mdr38_flag;

    unsigned long mdr39_offset;
	unsigned long mdr39_size;
	unsigned short mdr39_rec_size;
	unsigned long mdr39_flag;

	unsigned long mdr40_offset;
	unsigned long mdr40_size;
	unsigned short mdr40_rec_size;
	unsigned long mdr40_flag;
/*
	unsigned long mdr41_offset;
	unsigned long mdr41_size;
	unsigned short mdr41_rec_size;
	unsigned long mdr41_flag;

	unsigned long mdr42_offset;
	unsigned long mdr42_size;
	unsigned short mdr42_rec_size;
	unsigned long mdr42_flag;

	unsigned long mdr43_offset;
	unsigned long mdr43_size;
	unsigned short mdr43_rec_size;
	unsigned long mdr43_flag;

	unsigned long mdr44_offset;
	unsigned long mdr44_size;
	unsigned short mdr44_rec_size;
	unsigned long mdr44_flag;

	unsigned long mdr45_offset;
	unsigned long mdr45_size;
	unsigned short mdr45_rec_size;
	unsigned long mdr45_flag;

	unsigned long mdr46_offset;
	unsigned long mdr46_size;
	unsigned short mdr46_rec_size;
	unsigned long mdr46_flag;*/

};
#pragma pack()

class MDR_Header 
{
public:
	int				mdrHeaderLength; //0
	unsigned char	mdrName[10];     //2
	unsigned char	mdrMdr0[2];      //12
	int				mdrYear;         //14
	unsigned char	mdrMonth,mdrDay; //16
	unsigned char	mdrTime[3];      //18

	xor_fstream*		file;
	unsigned long		MDRfile_start;
	unsigned long		address;
	MDR_Header_struct	mdr_header;

	//pomocnicze
	unsigned char mdr1_ref_size;
	unsigned char mdr5_ref_size;
	unsigned char mdr6_ref_size;
	unsigned char mdr7_ref_size;
	unsigned char mdr10_ref_size;
	unsigned char mdr11_ref_size;
	unsigned char mdr11_ref_size_bit;
	unsigned char mdr15_ref_size;
	unsigned char mdr20_ref_size;
	unsigned char mdr21_ref_size;
	unsigned char mdr22_ref_size;

	unsigned char mdr23_ref_size;
	unsigned char mdr24_ref_size;
	unsigned char mdr25_ref_size;
	unsigned char mdr26_ref_size;
	unsigned char mdr27_ref_size;
	unsigned char mdr28_ref_size;

#pragma pack(1)
	struct {
		unsigned short structSize;
		unsigned long MDR11_data_offset;
		unsigned long MDR11_records_count;
		unsigned long MDR10_data_offset; //èíäåêñû â MDR10. êîëè÷åñòâî = MDR11_records_count
		unsigned long MDR7_data_offset;
		unsigned long MDR7_records_count;
		unsigned long MDR5_data_offset;
		unsigned long MDR5_records_count;
		unsigned long MDR6_data_offset;
		unsigned long MDR6_records_count;

		unsigned long MDR20_data_offset;
		unsigned long MDR20_records_count;
		unsigned long MDR21_data_offset;
		unsigned long MDR21_records_count;
		unsigned long MDR22_data_offset;
		unsigned long MDR22_records_count;

		unsigned long MDRx_data_offset;
		unsigned long MDRx_records_count;

	} mdr1_data;
#pragma pack()

	MDR_Header();
	//~MDR_Header();

	void Write(const char* name);
	void WriteIMGHeader(xor_fstream* file,unsigned long size,const char* internal_file_name);
	unsigned long Calculate();
	int getNoBytes(unsigned long value);

	//jezeli TRUE - trzeba pisac nastepny fat dla tego pliku
	bool WriteFAT(xor_fstream* file,
		unsigned long& size,
		const char *type,
		const char *imgid,
		unsigned short &start_block,
		unsigned short &next_fat_no,
		unsigned int block_size);

	int calculateMDRsection(int rec_size,std::string table_name);
	static int	getSelectCount(std::string sql); // zwrócenie wartoœci count dla zapytania
	static int	getSelectCount(std::string sql,int i); // zwrócenie wartoœci count dla zapytania
	static int	getSelectCount(std::string sql,int i,int j); // zwrócenie wartoœci count dla zapytania
	static std::string	getSelectString(std::string sql,int i,int j); // zwrócenie wartoœci count dla zapytania

	void write1Value(xor_fstream* file,std::string sql,int rec_size);
	void writeMDR1(xor_fstream* file);
	void writeMDR2(xor_fstream* file);
	void writeMDR3(xor_fstream* file);
	void writeMDR4(xor_fstream* file);
	void writeMDR5(xor_fstream* file);
	void writeMDR6(xor_fstream* file);
	void writeMDR7(xor_fstream* file);
	void writeMDR8(xor_fstream* file);
	void writeMDR9(xor_fstream* file);
	void writeMDR10(xor_fstream* file);
	void writeMDR11(xor_fstream* file);
	void writeMDR13(xor_fstream* file);
	void writeMDR14(xor_fstream* file);
	void writeMDR15(xor_fstream* file);
	void writeMDR20(xor_fstream* file);
	void writeMDR21(xor_fstream* file);
	void writeMDR22(xor_fstream* file);

	void writeMDR23(xor_fstream* file);
	void writeMDR24(xor_fstream* file);
	void writeMDR25(xor_fstream* file);
	void writeMDR26(xor_fstream* file);
	void writeMDR27(xor_fstream* file);
	void writeMDR28(xor_fstream* file);
	void writeMDR29(xor_fstream* file);
};


class MdrCreator {
private:
	
	MDR_Header	mdrHeader;
	bool		multibody;

public:
	//int	mdr1_map_index;

	static int	add_lbl15(std::string text);

	void index_mdr1();
	void index_mdr4();
	void index_mdr5();
	void index_mdr6();
	void index_mdr7();
	void index_mdr8();
	void index_mdr11();
	void index_mdr12();
	void index_mdr13_mdr14();
	void index_mdr9_mdr10();
	void index_mdr20_mdr21_mdr22();


	void select_mdr4();
	void select_mdr5();
	void set_by_region();
	void set_multibody(bool t_multibody);

	void write_mdr(std::string text);
};

}

#endif
