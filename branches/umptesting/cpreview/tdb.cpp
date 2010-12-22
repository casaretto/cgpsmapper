/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

#include "tdb.h"
#include "crccheck.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

//---------------------------------------------------------------------------

TDB_header::TDB_header() {
	productCode = 1;
	m_family = 0;
	sub_family = 0;
	version = 403;
	mapVersion = 100;
	codepage = 0;
	productName = "";
	productType = "";
	unk8 = 16; //nod bit size?
	unk10_bg_color = 0; // bylo 0 !!

	unlock_protect = 0;
	transparent = 0;
	fill_type = 0;
	routing_protect = 0;
	heigh_profile = 0;
	maxN = 0;
	maxE = 0;
	maxS = 0;
	maxW = 0;
}

void TDB_header::Write(xor_fstream* file,int m_Type)
{
	unsigned char rec_type = 0x50;
	unsigned char unk2  = 0x11;
	unsigned char unk3 = 0x01;
	unsigned char unk4 = 0x01;
	unsigned char unk5 = 0x01;
	int  unk7 = 0x0;
	unsigned char unk10 = 0x10;
	unsigned char unk11 = 0x27;
	int  unk12 = 0x10000;
	//unk10_bg_color = 0x04;

	file->Write(&rec_type,1);

	version = 300;
	recordLength = 4 + 2 + static_cast<int>(productName.length()) + 1 + 2 + static_cast<int>(productType.length()) + 1;
	if( m_Type & 2 ) {
		version = 400;
		recordLength += 6;
		recordLength += 4;
		if( m_Type & 4 ) {
			version = 402;
			recordLength += 1;
			if( m_Type & 8 ) {
				version = 403;
				recordLength += 3;
				recordLength += 16;
				if( m_Type & 16 ) 
					recordLength += 4; //???
				if( m_Type & 32 ) {
					version = 411;
					recordLength += 7;
					recordLength += 21;
				}
			}
		}
	}

	file->Write(&recordLength,2);
	productCode = m_family<<16 | sub_family;
	file->Write(&productCode,4);
	file->Write(&version,2);
	file->WriteString(productName);
	file->Write(&mapVersion,2);
	file->WriteString(productType);

	if( m_Type & 2 ) {
		file->Write(&unlock_protect,1);
		file->Write(&unk2,1);//0x11
		file->Write(&unk3,1);//0x01
		file->Write(&unk4,1);//0x01
		file->Write(&unk5,1);//0x01
		file->Write(&transparent,1);
		file->Write(&unk7,4);
		if( m_Type & 4 ) {
			file->Write(&unk8,1); //0x10
			if( m_Type & 4 ) {
				file->Write(&fill_type,1);
				file->Write(&unk10_bg_color,1);
				file->Write(&routing_protect,1);

				//store 0?!
				maxN = 0;
				maxE = 0;
				maxS = 0;
				maxW = 0;

				file->Write(&maxN,4);
				file->Write(&maxE,4);
				file->Write(&maxS,4);
				file->Write(&maxW,4);
				if( m_Type & 16 ) {
					file->Write(&codepage,4);
					if( m_Type & 32 ) {
						file->Write(&unk10,1);
						file->Write(&unk11,1);
						file->Write(&unk12,4);
						file->Write(&heigh_profile,1);
						file->FillWrite(0,21);
					}
				}
			}
		}
	}
}


void TDB_copyright::Write(xor_fstream* file)
{
	int t_pos = 0;
	unsigned char rec_type = 0x44;

	file->Write(&rec_type,1);

	recordLength = 0;

	while( t_pos < (int)list_TDB_info_string.size() )
	{
		recordLength += 4;
		recordLength += static_cast<int>(list_TDB_info_string[t_pos].name.length());
		recordLength += 1;
		t_pos++;
	}
	file->Write(&recordLength,2);

	t_pos = 0;
	while( t_pos < (int)list_TDB_info_string.size() )
	{
		file->Write(&list_TDB_info_string[t_pos].code,4);
		file->WriteString(list_TDB_info_string[t_pos].name);
		t_pos++;
	}
}
/////////////////////////////////////////////////////////////////////////////

void TDB_map::Write(xor_fstream* file)
{
	unsigned char rec_type = 0x42;
	unsigned char null_byte = 0x00;
	map_0 = 0;

	file->Write(&rec_type,1);

	recordLength = 4 + 4 + 16 + static_cast<int>(name.length()) + 1;

	file->Write(&recordLength,2);
	file->Write(&areaCode,4);

	file->Write(&map_0,4);   //0x00 00 00 00

	maxN>>=8;
	maxE>>=8;
	maxS>>=8;
	maxW>>=8;

	file->Write(&null_byte,1);
	file->Write(&maxN,3);
	file->Write(&null_byte,1);
	file->Write(&maxE,3);
	file->Write(&null_byte,1);
	file->Write(&maxS,3);
	file->Write(&null_byte,1);
	file->Write(&maxW,3);

	file->WriteString(name);
}

void TDB_img::Write(xor_fstream* file,int m_Type)
{
	unsigned char rec_type = 0x4c;//ok
	unsigned char null_byte = 0x00;
	int	value;

	file->Write(&rec_type,1);

	recordLength = 4 + 4 + 16 + static_cast<int>(img_name.length()) + 1 + 4;
	recordLength += (img_sections_real * 4); //rgn lbl tre
	//NET_length = 0;
	//if( NET_length )
	//	recordLength += 4;
	//if( NOD_length )
	//	recordLength += 4;

	recordLength += 1;

	if( m_Type & 2 ) {
		recordLength += 2; //dca

		recordLength += 4; //mdr  - len??

		recordLength += 2; //?

		//nazwy 13 * ilosc
		recordLength += img_sections_real * 13;
	}

	file->Write(&recordLength,2);
	file->Write(&IMGID,4);

	file->Write(&areaCode,4);   //0x00 00 00 00

	maxN>>=8;
	maxE>>=8;
	maxS>>=8;
	maxW>>=8;

	file->Write(&null_byte,1);
	file->Write(&maxN,3);
	file->Write(&null_byte,1);
	file->Write(&maxE,3);
	file->Write(&null_byte,1);
	file->Write(&maxS,3);
	file->Write(&null_byte,1);
	file->Write(&maxW,3);

	file->WriteString(img_name);

	//img_sections = 4;
	//img_sections_real = 3;
	file->Write(&img_sections,2);
	file->Write(&img_sections_real,2);

	if( GMP_length ) {
		file->Write(&GMP_length,4);
	} else {
		file->Write(&TRE_length,4);
		file->Write(&RGN_length,4);
		file->Write(&LBL_length,4);
		if( NET_length )
			file->Write(&NET_length,4);
		if( NOD_length )
			file->Write(&NOD_length,4);
	}
	null_byte = 1;
	file->Write(&null_byte,1);
	if( m_Type & 2 ) {
		file->Write(&regionID,2);

		value = 1<<8; //was 255!
		file->Write(&value,4);//mdr len

		if( GMP_length ) {
			file->WriteString(gmp_name);
		} else {
			file->WriteString(tre_name);
			file->WriteString(rgn_name);
			file->WriteString(lbl_name);
			if( NET_length )
				file->WriteString(net_name);
			if( NOD_length )
				file->WriteString(nod_name);
		}

		value = 0; 
		file->Write(&value,2);
	}

}


void TDB_dca::Write(xor_fstream* file) {
	unsigned char rec_type = 0x52;
	unsigned char null_byte = 0x00;
	vector<TDB_dca_region>::iterator r_i;

	recordLength = 0;
	file->Write(&rec_type,1);

	for(r_i = regions.begin(); r_i < regions.end(); r_i++ ) {
		recordLength += 1 + static_cast<int>((*r_i).name.length()) + 1;
	}
	file->Write(&recordLength,2);

	for(r_i = regions.begin(); r_i < regions.end(); r_i++ ) {
		file->Write(&(*r_i).m_sub_family,1);
		file->WriteString((*r_i).name);
	}
}

void TDB_region::Write(xor_fstream* file) {
	unsigned char rec_type = 0x53;
	unsigned char null_byte = 0x00;

	file->Write(&rec_type,1);

	recordLength = 3;
	file->Write(&recordLength,2);

	file->Write(&this->m_sub_family,1);
	file->Write(&this->productCode,2);
}


/////////////////////////////////////////////////////////////////////////////

TDB::TDB(unsigned int productCode,string productName,string productType,int mapVersion,bool locked,bool marine){
	m_family = 0;
	m_sub_family = 0;

	m_TDB_copyright.recordLength = 0;
	m_TDB_header.productCode = productCode;
	m_TDB_header.productName = productName;
	m_TDB_header.productType = productType;
	m_TDB_header.mapVersion = mapVersion;
	m_TDB_header.m_family = 0;
	m_TDB_header.sub_family = 0;
	m_Type = 1;//was 1!! - 
	//wyglada na to ze wersja 407 jest konieczna zeby mapa dzialala w nRoute

	m_TDB_header.unlock_protect = (locked?1:0);
	m_TDB_header.transparent = 0;
	m_TDB_header.fill_type = 0;
	m_TDB_header.heigh_profile = 0;

	if( marine )
		m_TDB_header.unk10_bg_color = 0x08;
	else
		m_TDB_header.unk10_bg_color = 0;


	//	if( getSetting("VERSION") == "ROUTABLE" || getSetting("VERSION") == "PERSONAL")
	m_TDB_header.routing_protect = 0; //dozwolone wyznaczanie
	//	else
	//		m_TDB_header.routing_protect = 1;
}

void TDB::setHeighProvile() {
	m_Type |= 32 + 16 + 8 + 4 + 2;
	m_TDB_header.heigh_profile = 1;

}

void TDB::setCodePage(int codepage) {
	//#if __MDR == 1
	m_Type |= /*16 + */ 8 + 4 + 2;
	m_TDB_header.codepage = codepage;
	//#endif;
}

void TDB::setMDR(bool status) {
	if( status )
		m_Type |= /*32 + 16 + */ 8 + 4 + 2;
}

void TDB::setFamily(int family) {
	this->m_family = family;
	m_TDB_header.m_family = family;
	//this->m_TDB_header.productCode = (family<<16);
	m_Type |= 32 + 16 + 8 + 4 + 2; //was 8+4+2

}

void TDB::addRegion(int regionID,string name) {
	TDB_dca_region region;
	region.name = name;
	region.m_sub_family = regionID;
	m_TDB_dca.regions.push_back(region);
}

void TDB::setSubFamily(int sub_family) {
	this->m_TDB_header.productCode |= sub_family;
	m_TDB_header.sub_family = sub_family;
	this->m_sub_family = sub_family;

	//moze byc wiecej regionow w mapie... teraz ZAWSZE 1
	/*
	this->m_TDB_dca.m_sub_family =  sub_family;
	this->m_TDB_dca.name = name;
	*/
	/*	{
	TDB_dca_region a;
	TDB_dca_region b;
	a.m_sub_family = 1;
	a.name = "region 1";

	b.m_sub_family = 2;
	b.name = "region 2";

	//		m_TDB_dca.regions.push_back(a);
	m_TDB_dca.regions.push_back(b);
	}
	*/
	//m_Type |= 8 + 4 + 2;

	this->m_TDB_region.m_sub_family = sub_family;
	this->m_TDB_region.productCode = this->m_TDB_header.productCode;

}



bool TDB::WriteTDBfile(const char* tdb_file_name)
{
	int t_pos = 0;
	file = new xor_fstream(tdb_file_name,"w+b");
	file->SetXorMask(0);

	//0x50
	m_TDB_header.maxE = 0;
	m_TDB_header.maxW = 0;
	m_TDB_header.maxS = 0;
	m_TDB_header.maxN = 0;
	m_TDB_header.maxE = m_TDB_map.maxE;
	m_TDB_header.maxW = m_TDB_map.maxW;
	m_TDB_header.maxS = m_TDB_map.maxS;
	m_TDB_header.maxN = m_TDB_map.maxN;

	m_TDB_header.maxE = m_TDB_header.maxE << 8;
	m_TDB_header.maxW = m_TDB_header.maxW << 8;
	m_TDB_header.maxS = m_TDB_header.maxS << 8;
	m_TDB_header.maxN = m_TDB_header.maxN << 8;

	m_TDB_header.Write(file,m_Type);

	//0x44
	if( m_TDB_copyright.list_TDB_info_string.size() )
		m_TDB_copyright.Write(file);

	set<int> tmp;
	while( t_pos < (int)m_TDB_img.size() ) {
		if( tmp.find(m_TDB_img[t_pos].regionID) == tmp.end() ) {
			tmp.insert(tmp.begin(),m_TDB_img[t_pos].regionID);
			addRegion(m_TDB_img[t_pos].regionID,m_TDB_img[t_pos].regionName);
		}
		t_pos++;
	}
	t_pos = 0;

	if( m_Type & 2 ) {
		//0x52
		m_TDB_dca.Write(file);

		//0x53 - niepotrzebne?
		//m_TDB_region.Write(file);
	}

	//0x42
	m_TDB_map.Write(file);

	//0x4c
	while( t_pos < (int)m_TDB_img.size() )
	{
		//zawsze JEDEN region....
		m_TDB_img[t_pos].Write(file,this->m_Type);//this->m_sub_family);
		t_pos++;
	}
	delete file;
	return true;
}

void TDB::SignTDB(const char* tdb_file_name) {	
	int				file_size = 0;
	unsigned long	crc = 0xffffffff;
	unsigned long	value;
	char			value_t;
	char			sign[26];
	time_t			t_tt;
	file = new xor_fstream(tdb_file_name,"r+b");
	file->SetXorMask(0);

	t_tt = time(NULL);
	srand(static_cast<unsigned int>(t_tt));

	file_size = file->file_size;
	while(file_size) {
		file->Read(&value_t,1);
		value = value_t;
		crc =  (unsigned long)(UPDC32(value,crc));
		file_size--;
	}
	delete file;
	crc = crc^0xffffffff;

	file = new xor_fstream(tdb_file_name,"a+b");
	file->SetXorMask(0);

	memset(sign,0,sizeof(sign));
	for( value = 3; value < 24; value++ ) {
		sign[value] = char(rand());
	}

	sign[0] = 'T';
	sign[1] = 0x14;	
	sign[0x11 + 3] = char(crc & 0xff);
	sign[0xc  + 3] = char((crc & 0xff00)>>8);
	sign[0x9  + 3] = char((crc & 0xff0000)>>16);
	sign[0x02 + 3] = char((crc & 0xff000000)>>24);

	//CRC32
	//1 (lowest) -> na pozycje 0x11
	//2          -> 0x0c
	//3          -> 0x09
	//4          -> 0x02
	file->Write(sign,23);

	delete file;
}
