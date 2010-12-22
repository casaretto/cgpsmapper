/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

#include <iostream>
#include <locale.h>
#include <map>
#include <vector>
#include <set>
#include <boost/tokenizer.hpp>
#include <string.h>
#include <algorithm>
#include "../sqlite-source/sqlite3.h"
#include "./../mapRead/mapReadDLL.h"
#include "utils.h"
#include "filexor.h"
#include "error.h"
#include "compare.h"

using namespace std;
using namespace boost;
using namespace g_compare;

#define _CGPSMAPPER "0100 "

#define _ERROR_FIND 0



#ifdef _DEBUG
#define def_mdrbase "mdrdata"
#else
#define def_mdrbase ":memory:"
#endif

#define _CITY_VALID_TYPE_MAX 14

namespace FTR {
	string				mdrbase;

	sqlite3				*MDRbase;
	bool				database;
	bool				multibody;

	long				map1 = -1;
	int					sub_maps;

	long				current_map;
	int					current_sub_map;
	xor_fstream*		file_out;
	set<unsigned long>	net_array;
	bool				first_pass;
	short				current_level;
	int					t_map_id;
	unsigned long		t_net_address;
	unsigned long		t_poi_address;
	unsigned char		t_type;
	unsigned char		t_subType;
	string				sql;
	char				t_buf[20];
	int					label_coding;
	int					code_page;
	int					map_id_from_filename;
	int					current_road_id;

	class _key2 {
	public:
		string		text1;
		string		text2;
	};
	bool operator<(const _key2& k1,const _key2& k2) {
		if( k1.text1 == k2.text1 )
			return k1.text2 < k2.text2;
		return k1.text1 < k2.text1;
	};

	class _key3 {
	public:
		string		text1;
		string		text2;
		string		text3;
	};
	bool operator<(const _key3& k1,const _key3& k2) {
		if( k1.text3 == k2.text3 ) {
			if( k1.text2 == k2.text2 )
				return k1.text1 < k2.text1;
			return k1.text2 < k2.text2;
		}
		return k1.text3 < k2.text3;
	};

	class _46 {
	public:
		unsigned char	family;
		unsigned char	subType;

		unsigned char	RGN;
		unsigned short	TRE;

		unsigned char	unkn1; //0
		unsigned int	idx_43;
		unsigned int	unkn2;//jesli idx_43 to 0x8000 ?
		unsigned long	lbl1;
		string			text;
		unsigned long	poi_address;
	};

	class _41 {
	public:
		unsigned long	net;
		unsigned long	lbl1;
		string			text;
	};

	class _45 {
	public:
		unsigned short	idx_4e;//kraj
		unsigned long	lbl1;
		string			text;
		unsigned long	net;
	};

	class _44 {
	public:
		unsigned short	idx_53;//region
		unsigned long	lbl1;
		string			text;
		unsigned long	net;
	};

	class _42 {
	public:
		unsigned int	idx_43;//miasta
		unsigned long	lbl1;
		string			text;
		unsigned long	net;
	};

	class _road {
	public:
		_41						net_object;
	};

	class _road_index {
	public:
		map<unsigned short,_44>	region_list;
		map<unsigned short,_45>	country_list;
		map<unsigned short,_42>	city_list;
	};

	//statystyka - typy uzywane w IMG
	class _4c {
	public:
		unsigned char	g_type; //0 Road, 1 Poly, 2 POI
		unsigned char	type;
		unsigned char	sub_type;
		unsigned char	highest_level;
	};

	//miasta
	class _43 {
	public:
		unsigned short	index;
		unsigned short	id_53;
		unsigned short	id_4e;
		unsigned long	lbl1;
		string			text;
		bool			valid;
	};

	//region
	class _53 {
	public:
		unsigned short	index;
		unsigned short	id_4e;
		unsigned long	lbl1;
		string			text;
		bool			valid;
	};

	//4e - kraj
	class _4e {
	public:
		unsigned short	index;
		unsigned long	lbl1;
		string			text;
		bool			valid;
	};

	//zip
	class _50 {
	public:
		unsigned int	index;
		unsigned long	lbl1;
		string			text;
	};

	vector<_50>					_zip;
	map<string,_road>			_ulice;
	map<string,_road_index>		_ulice_adres;
	vector<_43>					_43_miasta;
	vector<_53>					_53_regiony;
	vector<_4e>					_4e_kraje;
	vector<_46>					_poi;
	map<unsigned long,_4c>		_4c_types;

	void deleteAllData() {
		_zip.clear();
		_ulice.clear();
		_ulice_adres.clear();
		_43_miasta.clear();
		_53_regiony.clear();
		_4e_kraje.clear();
		_poi.clear();
		_4c_types.clear();
	}


	string normalizeTo8bit(string input) {
		for(size_t i = 0; i<input.size(); ++i) {
			if( label_coding == 6 ) {
				if( ((unsigned char)input[i]) >= 0x2a && ((unsigned char)input[i]) <= 0x2f ){
					input.erase(i,1);				
				}
			}
			if( i < input.size() ) {
				if( i > 0 && ((unsigned char)input[i]) <= 5 ) {
					input.erase(i,1);
				}
			}
			if( i < input.size() ) {
				if( label_coding == 9 && input[i] > 0 &&  input[i] < 0x1e  )
					input.erase(i,1);
			}
		}
		return input;
	}

	string normalizeString(string input) {
		for(size_t i = 0; i<input.size(); ++i) {
			if( ((unsigned char)input[i]) < 32 )
				input.erase(i,1);
		}
		return input;
	}

	string stripAfterBracket(string input) {
		size_t bracket;
		bracket = input.find('{');
		if( bracket != string::npos ) {
			input = input.substr(0,bracket);
		}
		return input;
	}

	void CALL_API addPoi(unsigned char t_type,unsigned char t_subType,string name,unsigned long l1,unsigned short city,unsigned char RGN,unsigned short TRE) {
		_46 t_46;
		name = stripAfterBracket(name);
		if( name.size() == 0 ) return;
		
#if _ERROR_FIND == 1
		return;
#endif

		if( t_type > 0x11 && t_type <= 0x20 ) return;
		if( t_type == 0x63 ) return;

		if( t_type < 0x20 ) t_46.family = 1;
		else if( t_type == 0x2a) t_46.family = 2;
		else if( t_type == 0x2b) t_46.family = 3;
		else if( t_type == 0x2c) t_46.family = 4;
		else if( t_type == 0x2d) t_46.family = 5;
		else if( t_type == 0x2e) t_46.family = 6;
		else if( t_type == 0x2f) t_46.family = 7;
		else if( t_type == 0x30) t_46.family = 8;
		else if( t_type == 0x28) t_46.family = 9;
		else if( t_type == 0x63) t_46.family = 0xa;
		else if( t_type == 0x64) t_46.family = 0xb;
		else if( t_type == 0x65) t_46.family = 0xc;
		else if( t_type == 0x66) t_46.family = 0xd;
		else if( t_type == 0x67) t_46.family = 0xe;
		else return;

		//if( _poi.size() > 2 ||city  )
		//	return;

		t_46.unkn2 = 0;
		t_46.idx_43 = city;
		if( city ) {
			if( city-1 > _43_miasta.size() )
				errorClass::showError(-1,NULL,errorClass::PE08);
				//cout<<"Wrong city or IMG read error! ->"<<city<<endl;
			_43_miasta[city-1].valid = true;
			if( _43_miasta[city-1].id_53 ) {
				_53_regiony[_43_miasta[city-1].id_53-1].valid = true;
				if( _53_regiony[_43_miasta[city-1].id_53-1].id_4e )
					_4e_kraje[_53_regiony[_43_miasta[city-1].id_53-1].id_4e-1].valid = true;
			}
			if( _43_miasta[city-1].id_4e )
				_4e_kraje[_43_miasta[city-1].id_4e-1].valid = true;
			t_46.unkn2 = 0x8000;
		} else
			t_46.unkn2 = 0;
		t_46.unkn1 = 0;
		t_46.lbl1 = l1;
		t_46.text = name;
		if( t_46.family != 1 ) 
			t_46.subType = t_subType;
		else
			t_46.subType = t_type;
		t_46.RGN = RGN;
		t_46.TRE = TRE;
		t_46.poi_address = t_poi_address;
		_poi.push_back(t_46);

	}

	int CALL_API readIndex(int country_idx,const char* country,unsigned long l1,
		int region_idx,int region_country_idx,const char* region,unsigned long l2,
		int city_idx,int city_region_idx,int city_country_idx,const char* city,unsigned long l3,
		int zip_idx,const char* zip,unsigned long l4) {

			if( zip_idx ) {
				_50 t_50;
				t_50.index = zip_idx;
				t_50.lbl1 = l4;
				t_50.text = zip;

				_zip.push_back(t_50);
			}

			if( country_idx ) {
				_4e t_4e;
				t_4e.index = country_idx;
				t_4e.lbl1 = l1;
				t_4e.text = country;
				t_4e.valid = false;
				_4e_kraje.push_back(t_4e);
			}

			if( region_idx ) {
				_53 t_53;
				t_53.index = region_idx;
				//t_53._4e_text
				t_53.id_4e = region_country_idx;
				t_53.lbl1 = l2;
				t_53.text = region;
				t_53.valid = false;
				_53_regiony.push_back(t_53);
			}

			if( city_idx ) {
				_43 t_43;
				t_43.id_53 = city_region_idx;
				t_43.id_4e = city_country_idx;
				t_43.index = city_idx;
				t_43.lbl1 = l3;
				t_43.text = city;
				t_43.valid = false;
				_43_miasta.push_back(t_43);
			}
			return 1;
		}

		int CALL_API readHeader(int map_index,int map_sub_index,const char* map_id,const char* map_name,const char* copyright1,const char* copyright2,int levels,float &x0,float &x1,float &y0,float &y1,int locked,unsigned char level_def[10]) {
			t_map_id = atol(map_id);
			current_sub_map = map_sub_index;			

			if( database == true ) {
				char *s = sqlite3_mprintf("INSERT INTO MDR1(map_id) VALUES (%i);",t_map_id);
				int b_result = sqlite3_exec(FTR::MDRbase,s,NULL,NULL,NULL);
				sqlite3_free(s);
			}
			return 1;
		}
					          
		int CALL_API readDataI(int layer,int object_type,int type,int sub_type, unsigned int poi_address, unsigned int net_address,const char* label,mapRead_coord_integer* coords,int coord_size,const char flag,unsigned int reserved) {
			return 1;
		}

		int CALL_API readData(int layer,int object_type,int type,int sub_type, unsigned int poi_address, unsigned int net_address,unsigned int object_id,const char* label,mapRead_coord* coords,int coord_size,const char flag,unsigned int l1) {
			t_type = (type & 0xff00)>>8;
			t_subType = sub_type;
			current_road_id = object_id;

#if _ERROR_FIND == 1
			if( object_type == 0x80 ) return 1;
			if( object_type == 0x40 ) return 1;
			if( object_type == 0x20 ) return 1;
			static int count = 0;
			count++;
			if( count = 2 ) return 1;
#endif

			if( type & 0x010000 )
				return 1;

			if(first_pass) {
				unsigned long key;
				unsigned char t_type;
		
				if( object_type == 0x10 || object_type == 0x20 ) t_type = 2;
				if( object_type == 0x80 ) t_type = 1;
				if( object_type == 0x40 ) t_type = 0;

				key = (t_type<<16) | type;
				if( _4c_types.find(key) == _4c_types.end() && strlen(label) > 0 ) {

					_4c t_4c;
					if( object_type == 0x10 || object_type == 0x20 ) t_4c.g_type = 2;
					if( object_type == 0x40 ) t_4c.g_type = 0;
					if( object_type == 0x80 ) t_4c.g_type = 1;

					t_4c.type = (type & 0xff00)>>8;
					t_4c.sub_type = (type & 0xff);
					t_4c.highest_level = (unsigned char)current_level;

					_4c_types[key] = t_4c;
				}

				//zebranie miasto/region/kraj
				if( layer == 0 && (object_type == 0x10 || object_type == 0x20) ) {
					if( flag & 0x01 ) {
						t_poi_address = poi_address;
						mapRead_readPOI(current_map,type,sub_type,poi_address,net_address);
					} else {
						unsigned char RGN;
						unsigned short TRE;
						mapRead_readTRE(RGN,TRE);
						addPoi( ((type & 0xff00)>>8),(type & 0xff),label,l1,0,RGN,TRE);
					}
				}
				if( flag & 0x02 ) {
					t_net_address = net_address;
					mapRead_readNET(current_map,net_address);
				} else if( object_type == 0x40 && (type <= 0x10 || type == 0x16) ) {
					//reczne dodanie do indeksowania
					//int records = 0;
					//netData_i(label,l1,"",0,"",0,"",0,records,NULL);
				}
			} 
			return 1;
		}

		bool add_index_from_file(string file_name) {
			int	t_next_id = 0;
			int	t_id;
			int	t_type;
			int t_rgn,t_tre,t_idx43;
			int	t_rgn_type;
			char t_buff[6];
			string t_name;
			char *s;
			int b_result;

			//cout<<"idx:"<<file_name<<" ";
			xor_fstream* t_file = new xor_fstream(file_name.c_str(),"rb");
			if( t_file->error ) {
				delete t_file;
				return false;
			}
			t_file->Read(t_buff,5);
			
			for( int t_a = 0; t_a < 5; t_a++ ) {
				if( t_buff[t_a] != _CGPSMAPPER[t_a] )
					errorClass::showError(-1,file_name.c_str(),errorClass::PE09);
			}
			t_file->Read(&t_id,4);
			if( t_id != t_map_id )
				errorClass::showError(-1,file_name.c_str(),errorClass::PE06);
			
			//cout<<t_id<<endl;
			sqlite3_exec(FTR::MDRbase,"BEGIN TRANSACTION",NULL,NULL,NULL);
			for(;;) {
				t_rgn = 0;
				t_tre = 0;
				t_idx43 = 0;
				t_type = 0;
				t_rgn_type = 0;

				if( t_file->Read(&t_type,1) == 0 )
					break;
				//t_file->Read(&t_rgn_type,1);					
				t_file->Read(&t_rgn,1);					
				t_file->Read(&t_tre,2);
				t_file->Read(&t_idx43,4);
				t_name = t_file->ReadString();

				if( t_idx43-1 > _43_miasta.size() )
					errorClass::showError(-1,NULL,errorClass::PE08);
					//cout<<"Wrong city or index read error! ->"<<t_idx43<<endl;

#if _ERROR_FIND == 1
				static int count = 0;
				count++;
				if( count >= 9305 && count <= 9306) {
#endif

				_43_miasta[t_idx43-1].valid = true;
				if( _43_miasta[t_idx43-1].id_53 ) {
					_53_regiony[_43_miasta[t_idx43-1].id_53-1].valid = true;
					if( _53_regiony[_43_miasta[t_idx43-1].id_53-1].id_4e )
						_4e_kraje[_53_regiony[_43_miasta[t_idx43-1].id_53-1].id_4e-1].valid = true;
				}
				if( _43_miasta[t_idx43-1].id_4e )
					_4e_kraje[_43_miasta[t_idx43-1].id_4e-1].valid = true;

				s = sqlite3_mprintf("INSERT INTO POI_IDX(id, rgn , tre ,idx_43 ,t_text,map_id ) VALUES (%i, %i,%i,%i,'%q',%i);",t_next_id, t_rgn,t_tre,t_idx43,t_name.c_str(),t_map_id);
				b_result = sqlite3_exec(FTR::MDRbase,s,NULL,NULL,NULL);
				sqlite3_free(s);

#if _ERROR_FIND == 1
				}
#endif
				t_next_id++;

			}
			sqlite3_exec(FTR::MDRbase,"END TRANSACTION",NULL,NULL,NULL);
			delete t_file;
			return true;
		}

		bool add_road(string name,unsigned long lname) {
			name = stripAfterBracket(name);
			if( !name.size() )
				return false;
			if( _ulice.find(normalizeString(name)) == _ulice.end() ) {
				_road road;
				road.net_object.lbl1 = lname;
				road.net_object.net = t_net_address;
				road.net_object.text = normalizeTo8bit(name);

				_ulice[normalizeString(name)] = road;
			}
			return true;
		}

		int CALL_API netData_i(const char* name1,unsigned long lname1,const char* name2,unsigned long lname2,const char* name3,unsigned long lname3,const char* name4,unsigned long lname4,int one_way,int road_class,int road_speed,int &records,mapRead_address_i address[60]) {
			static int t=0;
#if _ERROR_FIND == 1
			return 1;
#endif
			if( first_pass ) {

				string	s_name;
				string	t_name;
				int		t_name_idx;
				unsigned short	t_city_index;
				unsigned long tl_name;
				map<unsigned long,_road>::iterator	ulica;
				map<string,_road_index>::iterator	ulica_adres;
				unsigned short		region_i,country_i;

				//lista NET
				if( strlen(name1) ) {
					s_name = name1;
					if( !add_road(name1,lname1) )
						return 1;
					/*tokenizer<> tok(s_name);
					for(tokenizer<>::iterator beg=tok.begin(); beg!=tok.end();++beg){
						add_road(*beg,lname1);
					}*/
				} else
					return 1;

				if( strlen(name2) ) {
					s_name = name2;
					add_road(name2,lname2);
					/*tokenizer<> tok(s_name);
					for(tokenizer<>::iterator beg=tok.begin(); beg!=tok.end();++beg){
						add_road(*beg,lname2);
					}*/
				}

				if( strlen(name3) ) {
					s_name = name3;
					add_road(name3,lname3);
					/*tokenizer<> tok(s_name);
					for(tokenizer<>::iterator beg=tok.begin(); beg!=tok.end();++beg){
						add_road(*beg,lname3);
					}*/
				}

				t_name_idx = 1;
				for(;;) {
					if( t_name_idx == 1 ) {t_name = name1;tl_name = lname1;}
					if( t_name_idx == 2 ) {t_name = name2;tl_name = lname2;}
					if( t_name_idx == 3 ) {t_name = name3;tl_name = lname3;}

					t_name = stripAfterBracket(t_name);


					if( !t_name.size() ) break;
					t_name_idx++;
					if( t_name_idx > 3 ) break;
					if( _ulice_adres.find(normalizeString(t_name)) == _ulice_adres.end() ) {
						_road_index t_road_index;
						_ulice_adres[normalizeString(t_name)] = t_road_index;			
					}
					ulica_adres = _ulice_adres.find(normalizeString(t_name));

					for(int i=0; i<records; ++i) {
						if( current_road_id == address[i].road_idx ) {
							for(int j=0; j<6; ++j) {
								if( j==0 )
									t_city_index = address[i].city_left;
								else if( j == 1 )
									t_city_index = address[i].city_right;
								else if( j == 2 )
									t_city_index = address[i].city_left_2;
								else if( j == 3 )
									t_city_index = address[i].city_right_2;
								else if( j == 4 )
									t_city_index = address[i].city_left_3;
								else if( j == 5 )
									t_city_index = address[i].city_right_3;							

								if( t_city_index ) {
									if( (*ulica_adres).second.city_list.find(t_city_index) == (*ulica_adres).second.city_list.end() ) {					
										_42 t_42;
										t_42.idx_43 = t_city_index;
										t_42.lbl1 = tl_name;
										t_42.text = normalizeTo8bit(t_name);
										t_42.net = t_net_address;
										(*ulica_adres).second.city_list[t_city_index] = t_42;
									}
									_43_miasta[t_city_index-1].valid = true;
									region_i = _43_miasta[t_city_index-1].id_53;
									if( region_i ) {
										if( (*ulica_adres).second.region_list.find(region_i) == (*ulica_adres).second.region_list.end() ) {
											_44 t_44;
											t_44.idx_53 = region_i;
											t_44.lbl1 = tl_name;
											t_44.text = normalizeTo8bit(t_name);
											t_44.net = t_net_address;
											(*ulica_adres).second.region_list[region_i] = t_44;
										}
										_53_regiony[region_i-1].valid = true;
										country_i = _53_regiony[region_i-1].id_4e;
										if( country_i ) {
											if( (*ulica_adres).second.country_list.find(country_i) == (*ulica_adres).second.country_list.end() ) {
												_45 t_45;
												t_45.idx_4e = country_i;
												t_45.lbl1 = tl_name;
												t_45.text = normalizeTo8bit(t_name);
												t_45.net = t_net_address;
												(*ulica_adres).second.country_list[country_i] = t_45;
											}
											_4e_kraje[country_i-1].valid = true;
										}
									}
								}

							}
							//}
						}
					}
				}		
			} else {


				//44
				//45
				//42
			}
			return 1;
		}

		int CALL_API poiData_i(const char* name, unsigned long l1,const char* house_number,const char* street_name,unsigned short city,unsigned short zip,const char* phone,unsigned char RGN,unsigned short TRE) {

			addPoi(t_type,t_subType,name,l1,city,RGN,TRE);
			return 1;
		}

		void storeTypes() {
			int b_result ;
			//sort(_4c_types.begin(),_4c_types.end());
			unsigned char	rec_type = 0x4c;
			unsigned short	rec_size = 4;
		
			for( map<unsigned long,_4c>::iterator i = _4c_types.begin(); i != _4c_types.end(); ++i ) {
				if( database == true ) {

					char *s = sqlite3_mprintf("INSERT INTO T4C_TYPES VALUES (%i,%i,%i,%i,%i);",t_map_id,(*i).second.g_type,(*i).second.type,(*i).second.sub_type,(*i).second.highest_level);					
					b_result = sqlite3_exec(FTR::MDRbase,s,NULL,NULL,NULL);
					sqlite3_free(s);
				} else {
					file_out->Write(&rec_type,1);
					file_out->Write(&rec_size,2);
					file_out->Write(&(*i).second.g_type,1);
					file_out->Write(&(*i).second.type,1);
					file_out->Write(&(*i).second.sub_type,1);
					file_out->Write(&(*i).second.highest_level,1);
				}
			}
		}

		void storeZip() {
			int b_result ;
			unsigned char	rec_type = 0x50;
			unsigned short	rec_size = 4;

			for( vector<_50>::iterator _i_zip = _zip.begin(); _i_zip != _zip.end(); ++_i_zip ) {
				if( database == true ) {
					char *s = sqlite3_mprintf("INSERT INTO ZIP VALUES (%i,%i,%i,'%q');",t_map_id,(*_i_zip).index,(*_i_zip).lbl1,(*_i_zip).text.c_str());
					b_result = sqlite3_exec(FTR::MDRbase,s,NULL,NULL,NULL);
					sqlite3_free(s);
				} else {
					rec_size = 9 + (unsigned short)(*_i_zip).text.size();

					file_out->Write(&rec_type,1);
					file_out->Write(&rec_size,2);
					file_out->Write(&(*_i_zip).index,4);
					file_out->Write(&(*_i_zip).lbl1,4);
					file_out->WriteString((*_i_zip).text);
				}
			}
		}

		void storeCity() {
			int b_result;
			unsigned char	rec_type = 0x43;
			unsigned short	rec_size = 4;
			//unsigned short	index;

			for( vector<_43>::iterator i = _43_miasta.begin(); i != _43_miasta.end(); ++i ) {
				if( (*i).valid ) {
					//T43_MIASTA (map_id int,t_index int, id_53 int, id_4e int, lbl1 int, t_text text)
					if( database == true ) {

						char *s = sqlite3_mprintf("INSERT INTO T43_MIASTA VALUES (-1,%i,%i,%i,%i,%i,'%q');",
							t_map_id,(*i).index,(*i).id_53,(*i).id_4e,(*i).lbl1,(*i).text.c_str());
						b_result = sqlite3_exec(FTR::MDRbase,s,NULL,NULL,NULL);
						sqlite3_free(s);

					} else {
						rec_size = 11 + (unsigned short)(*i).text.size();
						file_out->Write(&rec_type,1);
						file_out->Write(&rec_size,2);
						file_out->Write(&(*i).index,2);

						file_out->Write(&(*i).id_4e,2);
						file_out->Write(&(*i).id_53,2);

						//file_out->Write(&(*i).second.id_53 ,2);
						file_out->Write(&(*i).lbl1,4);
						file_out->WriteString((*i).text);
					}
				}
			}
		}

		void storeCountryRegion() {
			int b_result;
			unsigned char	rec_type = 0x4e;
			unsigned short	rec_size = 4;
			unsigned short	tmp = 0;
			//unsigned short	index;

			for( vector<_4e>::iterator i = _4e_kraje.begin(); i != _4e_kraje.end(); ++i ) {
				if( (*i).valid ) {
					//CREATE TABLE T4E_KRAJE (map_id int,t_index int, lbl1 int, t_text text);",NULL,NULL,&errmsg);
					//CREATE TABLE T53_REGIONY (map_id int,t_index int, id_4e int, lbl1 int, t_text text);",NULL,NULL,&errmsg);

					if( database == true ) {
						char *s = sqlite3_mprintf("INSERT INTO T4E_KRAJE VALUES (-1,%i,%i,%i,'%q');",
							t_map_id,(*i).index,(*i).lbl1,(*i).text.c_str());
						b_result = sqlite3_exec(FTR::MDRbase,s,NULL,NULL,NULL);
						sqlite3_free(s);
					} else {
						rec_size = 7 + (unsigned short)(*i).text.size();
						file_out->Write(&rec_type,1);
						file_out->Write(&rec_size,2);
						file_out->Write(&(*i).index,2);
						file_out->Write(&(*i).lbl1,4);
						file_out->WriteString((*i).text);
					}
				}
			}

			rec_type = 0x53;
			for( vector<_53>::iterator j = _53_regiony.begin(); j != _53_regiony.end(); ++j ) {
				if( (*j).valid ) {
					if( database == true ) {
						char *s = sqlite3_mprintf("INSERT INTO T53_REGIONY VALUES (-1,%i,%i,%i,%i,'%q');",
							t_map_id,(*j).index,(*j).id_4e,(*j).lbl1,(*j).text.c_str());
						b_result = sqlite3_exec(FTR::MDRbase,s,NULL,NULL,NULL);
						sqlite3_free(s);
					} else {
						rec_size = 9 + (unsigned short)(*j).text.size();
						file_out->Write(&rec_type,1);
						file_out->Write(&rec_size,2);
						file_out->Write(&(*j).index,2);

						file_out->Write(&(*j).id_4e,2);

						//file_out->Write(&(*j).second.id_4e,2);
						file_out->Write(&(*j).lbl1,4);
						file_out->WriteString((*j).text);
					}
				}
			}
		}

		void storeStreets() {
			int				rec_id = 1;
			int				b_result;
			static int		unique_id = 0;
			int				t_unique_id;
			unsigned char	rec_type;
			unsigned short	rec_size = 4;
			unsigned short	tmp = 0;
			string			street_token;
			string			xflag_text; //text po wycieciu sortowanego slowa i usunieciu spacji
			int				string_pos = 0;
			sqlite3_stmt	*ppStmt3;
			int				rc;
			//unsigned int	index;
			map<string,_road>::iterator	l;
			map<string,_road_index>::iterator	ulica_adres;

			map<unsigned short,_44>::iterator	region_i;
			map<unsigned short,_45>::iterator	country_i;
			map<unsigned short,_42>::iterator	city_i;
			//ulice
			//CREATE TABLE ULICE_NET (map_id int,net int,lbl1 int,t_text text);",NULL,NULL,&errmsg);
			//CREATE TABLE ULICE (map_id int,t_text text,id_43 int,id_53 int,id_4e int);",NULL,NULL,&errmsg);

			/*
select sort_id,sort_start_letter,t_text,t_full_text,
replace(rtrim(t_full_text,quote(t_text)),' ','') as xflag_seq
from ulice_net
where t_text like 'sobieskiego'
order by sort_id, xflag_seq
			*/
			//klucz - NET+MAP_ID
			rec_type = 0x41;
			for( l = _ulice.begin(); l != _ulice.end(); ++l) {
				//cout<<(*l).second.net_object.text.c_str()
				if( database == true ) {
					unique_id++;
					t_unique_id = unique_id;
					{
						char *t_s = sqlite3_mprintf("select sort_id from ulice_net where t_text = '%q';",(*l).second.net_object.text.c_str());						
						rc = sqlite3_prepare_v2(FTR::MDRbase,t_s,-1,&ppStmt3,NULL);
						rc = sqlite3_step( ppStmt3 );

						if ( rc == SQLITE_ROW ) 
							t_unique_id = sqlite3_column_int(ppStmt3,0);

						sqlite3_free(t_s);
						sqlite3_finalize( ppStmt3 );
					}

					

					char *s = sqlite3_mprintf("INSERT INTO ULICE_NET(id, net_map ,map_id ,net ,lbl1 ,t_text,t_full_text, sort_start_letter, xflag, xflag_text ,sort_id ) VALUES (%i,'%i-%i',%i,%i,%i,'%q','%q',%i,%i, '%q', %i);",
						rec_id++,
						t_map_id,(*l).second.net_object.net,
						t_map_id,(*l).second.net_object.net,(*l).
						second.net_object.lbl1,
						(*l).second.net_object.text.c_str(),
						(*l).second.net_object.text.c_str(),//full text
						0, //sort start
						0, //xflag - jeszcze nieznany
						"", //xflag_text - pusty
						t_unique_id);

					if( (*l).second.net_object.text.size() )
						if( (*l).second.net_object.text[0] != '{' )
							b_result = sqlite3_exec(FTR::MDRbase,s,NULL,NULL,NULL);

					sqlite3_free(s);
									
					if( multibody && (*l).second.net_object.text.find(" ") != (*l).second.net_object.text.npos ) {
						string_pos = 0;
						street_token = (*l).second.net_object.text;

						while( street_token.find(" ") != street_token.npos ) {
							do { // wyciêcie wiêkszej iloœci spacji				
								string_pos = (*l).second.net_object.text.find(" ",string_pos)+1;				
								street_token = (*l).second.net_object.text.substr(string_pos);
							} while (street_token[0] == ' ');

							//ignorowac jezeli wyraz jedno literowy
							if( street_token.find(" ") != street_token.npos ) {
								if( street_token.find(" ") == 1 )
									continue;
							} else if( street_token.size() < 2 )
								continue;

							if( street_token[0] == '{' )
								break;

							unique_id++;
							t_unique_id = unique_id;
							{
								char *t_s = sqlite3_mprintf("select sort_id from ulice_net where t_text = '%q';",street_token.c_str());						
								rc = sqlite3_prepare_v2(FTR::MDRbase,t_s,-1,&ppStmt3,NULL);
								rc = sqlite3_step( ppStmt3 );

								if ( rc == SQLITE_ROW ) 
									t_unique_id = sqlite3_column_int(ppStmt3,0);

								sqlite3_free(t_s);
								sqlite3_finalize( ppStmt3 );
							}

							xflag_text = trim((*l).second.net_object.text.substr(0,string_pos));

							char *s = sqlite3_mprintf("INSERT INTO ULICE_NET(id, net_map ,map_id ,net ,lbl1 ,t_text,t_full_text, sort_start_letter, xflag, xflag_text, sort_id ) VALUES (%i,'%i-%i',%i,%i,%i,'%q','%q',%i,%i,'%q',%i);",
								rec_id++,
								t_map_id,(*l).second.net_object.net,
								t_map_id,(*l).second.net_object.net,
								(*l).second.net_object.lbl1,
								street_token.c_str(),(*l).second.net_object.text.c_str(),
								string_pos,//sort_start_letter
								0,//xflag
								xflag_text.c_str(),
								t_unique_id);
							b_result = sqlite3_exec(FTR::MDRbase,s,NULL,NULL,NULL);
							sqlite3_free(s);
						}
					}
				} else {
					rec_size = 9 + (unsigned short)(*l).second.net_object.text.size();
					file_out->Write(&rec_type,1);
					file_out->Write(&rec_size,2);
					file_out->Write(&(*l).second.net_object.net,4);
					file_out->Write(&(*l).second.net_object.lbl1,4);
					file_out->WriteString((*l).second.net_object.text);
				}
			}

			if( multibody ) {
				int	t_last_id = -1;
				int	t_id;
				int	t_sort;
				string	last_string;
				char *s;

				last_string = "";
				char *t_s = sqlite3_mprintf("select id,sort_id,xflag,t_full_text from ulice_net order by sort_id, xflag_text;");						
				rc = sqlite3_prepare_v2(FTR::MDRbase,t_s,-1,&ppStmt3,NULL);
				rc = sqlite3_step( ppStmt3 );
				do {
					if ( rc == SQLITE_ROW ) {
						t_id = sqlite3_column_int(ppStmt3,0);
						t_unique_id = sqlite3_column_int(ppStmt3,1);

						if( t_last_id != t_unique_id ) {
							last_string = string((char*)sqlite3_column_text(ppStmt3,3));
							t_sort = 0;	

							/*
							s = sqlite3_mprintf("UPDATE ulice_net SET xflag = %i WHERE id = %i",t_sort<<1 | 1,t_id);
							sqlite3_exec(FTR::MDRbase,s,NULL,NULL,NULL);			
							sqlite3_free(s);
							*/
	
							//rc = sqlite3_bind_int(ppStmt3, 1, t_sort<<1 | 1);
							t_last_id = t_unique_id;
						} else {
							if( last_string != string((char*)sqlite3_column_text(ppStmt3,3)) )
								t_sort++;
							s = sqlite3_mprintf("UPDATE ulice_net SET xflag = %i WHERE id = %i",t_sort<<1,t_id);
							sqlite3_exec(FTR::MDRbase,s,NULL,NULL,NULL);			
							sqlite3_free(s);

							last_string = string((char*)sqlite3_column_text(ppStmt3,3));
							//rc = sqlite3_bind_int(ppStmt3, 1, t_sort<<1);
						}						
					}
					rc = sqlite3_step( ppStmt3 );
				} while(rc == SQLITE_ROW);

				sqlite3_free(t_s);
				sqlite3_finalize( ppStmt3 );			

				//Ustawienie flagi dla pierwszego
				t_s = sqlite3_mprintf("select id,sort_id,xflag,t_full_text from ulice_net order by t_text;");						
				rc = sqlite3_prepare_v2(FTR::MDRbase,t_s,-1,&ppStmt3,NULL);
				rc = sqlite3_step( ppStmt3 );
				do {
					if ( rc == SQLITE_ROW ) {
						t_id = sqlite3_column_int(ppStmt3,0);
						t_unique_id = sqlite3_column_int(ppStmt3,1);
						t_sort = sqlite3_column_int(ppStmt3,2);//xflag

						if( t_last_id != t_unique_id ) {
							s = sqlite3_mprintf("UPDATE ulice_net SET xflag = %i WHERE id = %i",t_sort | 1,t_id);
							sqlite3_exec(FTR::MDRbase,s,NULL,NULL,NULL);			
							sqlite3_free(s);							

							t_last_id = t_unique_id;
						} 						
					}
					rc = sqlite3_step( ppStmt3 );
				} while(rc == SQLITE_ROW);

				sqlite3_free(t_s);
				sqlite3_finalize( ppStmt3 );
			}

			//ulice - info
			for( ulica_adres = _ulice_adres.begin(); ulica_adres != _ulice_adres.end(); ++ulica_adres) {
				rec_type = 0x44;//region
				for( region_i = (*ulica_adres).second.region_list.begin();  region_i != (*ulica_adres).second.region_list.end(); ++ region_i ) {
					if( database == true ) {
						//net_map text,map_id int,t_text text,lbl1 int,id_43 int,id_53 int,id_4e int,net int
						char *s = sqlite3_mprintf("INSERT INTO ULICE(net_map ,map_id ,t_text ,lbl1 ,id_43 ,id_53 ,id_4e ,net) VALUES ('%i-%i',%i,'%q',%i,%i,%i,%i,%i);",
							t_map_id, (*region_i).second.net,
							t_map_id,
							(*region_i).second.text.c_str(), 
							(*region_i).second.lbl1,
							0,
							(*region_i).second.idx_53,
							0,
							(*region_i).second.net);
						b_result = sqlite3_exec(FTR::MDRbase,s,NULL,NULL,NULL);
						sqlite3_free(s);
					} else {
						rec_size = 7 + (unsigned short)(*region_i).second.text.size();
						file_out->Write(&rec_type,1);
						file_out->Write(&rec_size,2);

						//index = (*_53_regiony.find((*region_i).second.idx_53)).second.index;
						//file_out->Write(&index,2);

						file_out->Write(&(*region_i).second.idx_53,2);
						file_out->Write(&(*region_i).second.lbl1,4);
						file_out->WriteString((*region_i).second.text);
					}
				}
				rec_type = 0x45;//country
				for( country_i = (*ulica_adres).second.country_list.begin();  country_i != (*ulica_adres).second.country_list.end(); ++ country_i ) {
					if( database == true ) {
						char *s = sqlite3_mprintf("INSERT INTO ULICE(net_map ,map_id ,t_text ,lbl1 ,id_43 ,id_53 ,id_4e ,net) VALUES ('%i-%i',%i,'%q',%i,%i,%i,%i,%i);",
							t_map_id,(*country_i).second.net,
							t_map_id,(*country_i).second.text.c_str(),(*country_i).second.lbl1,0,0,(*country_i).second.idx_4e,(*country_i).second.net);
						b_result = sqlite3_exec(FTR::MDRbase,s,NULL,NULL,NULL);
						sqlite3_free(s);
					} else {
						//if( (*_4e_kraje.find((*country_i).second.idx_4e)).second.valid ) {
						rec_size = 7 + (unsigned short)(*country_i).second.text.size();
						file_out->Write(&rec_type,1);
						file_out->Write(&rec_size,2);

						//index = (*_4e_kraje.find((*country_i).second.idx_4e)).second.index;
						//file_out->Write(&index,2);
						file_out->Write(&(*country_i).second.idx_4e,2);

						file_out->Write(&(*country_i).second.lbl1,4);
						file_out->WriteString((*country_i).second.text);
						//}
					}
				}

				rec_type = 0x42;//city
				for( city_i = (*ulica_adres).second.city_list.begin();  city_i != (*ulica_adres).second.city_list.end(); ++ city_i ) {
					//if( (*_43_miasta.find((*city_i).second.idx_43)).second.valid ) {
					if( database == true ) {
						char *s = sqlite3_mprintf("INSERT INTO ULICE(net_map ,map_id ,t_text ,lbl1 ,id_43 ,id_53 ,id_4e ,net) VALUES ('%i-%i',%i,'%q',%i,%i,%i,%i,%i);",
							t_map_id,(*city_i).second.net,
							t_map_id,(*city_i).second.text.c_str(),(*city_i).second.lbl1,(*city_i).second.idx_43,0,0,(*city_i).second.net);
						b_result = sqlite3_exec(FTR::MDRbase,s,NULL,NULL,NULL);
						sqlite3_free(s);
					} else {
						rec_size = 9 + (unsigned short)(*city_i).second.text.size();
						file_out->Write(&rec_type,1);
						file_out->Write(&rec_size,2);

						//index = (*_43_miasta.find((*city_i).second.idx_43)).second.index;
						//file_out->Write(&index,4);

						file_out->Write(&(*city_i).second.idx_43,4);
						file_out->Write(&(*city_i).second.lbl1,4);
						file_out->WriteString((*city_i).second.text);
						//}
					}
				}
			}

		}

		void storePOI() {
			int	b_result;
			unsigned char	rec_type = 0x46;
			unsigned short	rec_size = 4;
			unsigned short	tmp = 0;
			int	order = 1;
			//unsigned short	index;

			//CREATE TABLE POI (map_id int,family int, sub_type int, rgn int, tre int, unkn1 int,idx_43 int,unkn2 int, lbl1 int,t_text text);",NULL,NULL,&errmsg);
			for( vector<_46>::iterator i = _poi.begin(); i != _poi.end(); ++i ) {
				if( database == true ) {
					char *s = sqlite3_mprintf("INSERT INTO POI VALUES (%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,'%q',%i);",
						order++,t_map_id,-1,(*i).family,(*i).subType,(*i).RGN,(*i).TRE,(*i).unkn1,(*i).idx_43,(*i).unkn2,(*i).lbl1,(*i).text.c_str(),(*i).poi_address);
					b_result = sqlite3_exec(FTR::MDRbase,s,NULL,NULL,NULL);
					sqlite3_free(s);
				} else {
					rec_size = 15 + (unsigned short)(*i).text.size();
					file_out->Write(&rec_type,1);
					file_out->Write(&rec_size,2);
					file_out->Write(&(*i).family,1);
					file_out->Write(&(*i).subType,1);
					file_out->Write(&(*i).RGN,1);
					file_out->Write(&(*i).TRE,2);

					file_out->Write(&tmp,1);

					file_out->Write(&(*i).idx_43,2);
					file_out->Write(&(*i).unkn2,2);

					file_out->Write(&(*i).lbl1,4);
					file_out->WriteString((*i).text);
				}
			}
			
			//aktualizuj POI z danymi z POI_IDX!
			{
				sqlite3_stmt *ppStmt;
				char* s;
				int t_tre,t_rgn,t_idx_43,t_map_id;
				int	rc = sqlite3_prepare_v2(FTR::MDRbase,"SELECT rgn,tre,idx_43,map_id FROM POI_IDX;",-1,&ppStmt,NULL);
				rc = sqlite3_step( ppStmt );
				do {
					if ( rc == SQLITE_ROW ) {
						t_rgn = sqlite3_column_int(ppStmt,0);
						t_tre = sqlite3_column_int(ppStmt,1);
						t_idx_43 = sqlite3_column_int(ppStmt,2);
						t_map_id = sqlite3_column_int(ppStmt,3);

						s = sqlite3_mprintf("UPDATE POI SET idx_43 = %i WHERE tre = %i and rgn = %i and map_id = %i",				
							t_idx_43, t_tre, t_rgn, t_map_id);
						sqlite3_exec(FTR::MDRbase,s,NULL,NULL,NULL);			
						sqlite3_free(s);
					}
					rc = sqlite3_step( ppStmt );
				} while(rc == SQLITE_ROW);
				sqlite3_finalize( ppStmt );
			}
		}
}

//int databaseMDRcallback(void*,int,char**, char**);

bool initMDR(string base_name) {
	if( base_name.size() )
		FTR::mdrbase = base_name;
	else
		FTR::mdrbase = def_mdrbase;
	remove(FTR::mdrbase.c_str());

	cout<<"Indexer compatible with cgpsmapper"<<_CGPSMAPPER<<"version only."<<endl;

	if( sqlite3_open(FTR::mdrbase.c_str(),&FTR::MDRbase) != SQLITE_OK ) {
		sqlite3_close(FTR::MDRbase);
		return false;
	}
	sqlite3_exec(FTR::MDRbase,"PRAGMA journal_mode = OFF;",NULL,NULL,NULL);
#ifdef _DEBUG
	sqlite3_extended_result_codes(FTR::MDRbase, 1);
#endif
	return true;
}

bool createTables() {
	//create tables here
	int b_result;
#define _SORT

#ifndef _DEBUG
#define _SORT
#endif
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE POI_IDX (id int,rgn int, tre int,idx_43 int,t_text text,map_id int);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
#ifndef _SORT
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR1 (address int,id int,map_id int,offset int,structSize int,mdr11_data_offset int,mdr11_records_count int,mdr10_data_offset,mdr7_data_offset int,mdr7_records_count int,mdr5_data_offset int,mdr5_records_count int,mdr6_data_offset int,mdr6_records_count int,MDR16_data_offset int,MDR16_records_count int,MDR17_data_offset int,MDR17_record_count int,MDR18_data_offset int,MDR18_record_count int,mdr20_data_offset int,mdr20_records_count int,mdr21_data_offset int,mdr21_records_count int,mdr22_data_offset int,mdr22_records_count int,mdrX_data_offset int,mdrX_records_count int);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR15 (address int,lbl_index int,t_text text ,leng int);",NULL,NULL,NULL);	if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR14 (address int,mdr1_id int,id int,rec_id int,mdr15_id int,lbl1 int,t_text text,id_4e int );",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR13 (address int,mdr1_id int,id int,rec_id int,mdr15_id int,mdr14_id int,lbl1 int,t_text text,t_index int );",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR12 (address int,t_text text ,mdr11_id int);",NULL,NULL,NULL);	if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR11 (address int,mdr1_id int,id int,rgn int,tre int,lbl1 int,mdr13_id int,mdr14_id int,mdr5_id int,mdr15_id int, t_text text );",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR10 (address int,id int,sub_type int,mdr11_id int,uniq int);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR9  (address int,id int,family int,mdr10_id int);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR4  (address int,id int,type int,sub_type int,level int);",NULL,NULL,NULL);	if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR6 (address int,mdr1_id int,id int,lbl8 int,t_text text );",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR5 (address int,mdr1_id int,id int,mdr15_id int,lbl4 int,lbl1 int,mdr13_id int,mdr14_id int,mdr20_id int,t_text text,id_43 int,uniq int,region text );",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR7 (address int,mdr1_id int,id int,mdr15_id int,lbl1 int,uniq int,nflag int,xflag int,mdr5_id int,t_text text,sort_text text,city text,region text,country text,id_43 int,id_53 int,id_4e int,net_map text,map_id int);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR7_tmp (address int,mdr1_id int,id int,mdr15_id int,lbl1 int,uniq int,nflag int,xflag int,mdr5_id int,t_text text,sort_text text,city text,region text,country text,id_43 int,id_53 int,id_4e int,net_map text);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR8 (address int,id int,t_text text ,mdr7_id int);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 

	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR20 (address int,id int,mdr1_id int,mdr7_id int,id_43 int,uniq int);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR21 (address int,id int,mdr1_id int,mdr7_id int,id_53 int,uniq int);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR22 (address int,id int,mdr1_id int,mdr7_id int,id_4e int,uniq int);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 

	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR23 (address int,id int,mdr1_id int,mdr13_id int, mdr14_id int, lbl1 int, t_text text,uniq int);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR24 (address int,id int,mdr1_id int,mdr14_id int, lbl1 int, t_text text,uniq int);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR25 (address int,id int,mdr1_id int,mdr5_id int,city text,region text,country text);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR26 (address int,id int,mdr1_id int,mdr28_id,mdr14_id int);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR27 (address int,id int,mdr1_id int,mdr5_id,city text,region text);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR28 (address int,id int,mdr1_id int,mdr23_id int, t_text text,mdr21_id int,mdr27_id int,mdr14_id int);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR29 (address int,id int,mdr1_id int,mdr24_id int, t_text text,mdr22_id int,mdr25_id int,mdr26_id int);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 

	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE T4C_TYPES (map_id int,g_type int, type int, sub_type int, highest_level int);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE ZIP (map_id int,t_index int, lbl1 int, t_text text );",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE T43_MIASTA (mdr5_id int,map_id int,t_index int, id_53 int, id_4e int, lbl1 int, t_text text );",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE T4E_KRAJE (mdr14_id int,map_id int,t_index int, lbl1 int, t_text text );",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE T53_REGIONY (mdr13_id int,map_id int,t_index int, id_4e int, lbl1 int, t_text text );",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE POI (id int,map_id int,mdr11_id int,family int, sub_type int, rgn int, tre int, unkn1 int,idx_43 int,unkn2 int, lbl1 int,t_text text,img_address int);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE ULICE_NET (id int,net_map text,map_id int,net int,lbl1 int,t_text text,t_full_text text,mdr7_id_temp int,sort_start_letter int, xflag int, xflag_text text, sort_id int);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE ULICE (net_map text,map_id int,t_text text,lbl1 int,id_43 int,id_53 int,id_4e int,net int);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
#else
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR1 (address int,id int,map_id int,offset int,structSize int,mdr11_data_offset int,mdr11_records_count int,mdr10_data_offset,mdr7_data_offset int,mdr7_records_count int,mdr5_data_offset int,mdr5_records_count int,mdr6_data_offset int,mdr6_records_count int,MDR16_data_offset int,MDR16_records_count int,MDR17_data_offset int,MDR17_record_count int,MDR18_data_offset int,MDR18_record_count int,mdr20_data_offset int,mdr20_records_count int,mdr21_data_offset int,mdr21_records_count int,mdr22_data_offset int,mdr22_records_count int,mdrX_data_offset int,mdrX_records_count int);",NULL,NULL,NULL);	if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR15 (address int,lbl_index int,t_text text collate gsort,leng int);",NULL,NULL,NULL);	if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR14 (address int,mdr1_id int,id int,rec_id int,mdr15_id int,lbl1 int,t_text text collate gsort,id_4e int );",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR13 (address int,mdr1_id int,id int,rec_id int,mdr15_id int,mdr14_id int,lbl1 int,t_text text collate gsort,t_index int);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR12 (address int,t_text text collate gsort,mdr11_id int);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR11 (address int,mdr1_id int,id int,rgn int,tre int,lbl1 int,mdr13_id int,mdr14_id int,mdr5_id int,mdr15_id int, t_text text collate gsort);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR10 (address int,id int,sub_type int,mdr11_id int,uniq int);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR9  (address int,id int,family int,mdr10_id int);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR4  (address int,id int,type int,sub_type int,level int);",NULL,NULL,NULL);	if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR6 (address int,mdr1_id int,id int,lbl8 int,t_text text collate gsort);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR5 (address int,mdr1_id int,id int,mdr15_id int,lbl4 int,lbl1 int,mdr13_id int,mdr14_id int,mdr20_id int,t_text text collate gsort,id_43 int,uniq int,region text collate gsort);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR7 (address int,mdr1_id int,id int,mdr15_id int,lbl1 int,uniq int,nflag int,xflag int,mdr5_id int,t_text text collate gsort,sort_text text collate gsort,city text collate gsort,region text collate gsort,country text collate gsort,id_43 int,id_53 int,id_4e int,net_map text,map_id int);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR7_tmp (address int,mdr1_id int,id int,mdr15_id int,lbl1 int,uniq int,nflag int,xflag int,mdr5_id int,t_text text,sort_text text collate gsort,city text collate gsort,region text collate gsort,country text collate gsort,id_43 int,id_53 int,id_4e int,net_map text);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR8 (address int,id int,t_text text collate gsort,mdr7_id int);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 

	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR20 (address int,id int,mdr1_id int,mdr7_id int,id_43 int,uniq int);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR21 (address int,id int,mdr1_id int,mdr7_id int,id_53 int,uniq int);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR22 (address int,id int,mdr1_id int,mdr7_id int,id_4e int,uniq int);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 

	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR23 (address int,id int,mdr1_id int,mdr13_id int, mdr14_id int, lbl1 int, t_text text collate gsort,uniq int);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR24 (address int,id int,mdr1_id int,mdr14_id int, lbl1 int, t_text text  collate gsort,uniq int);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR25 (address int,id int,mdr1_id int,mdr5_id int,city text,region text,country text);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR26 (address int,id int,mdr1_id int,mdr28_id,mdr14_id int);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR27 (address int,id int,mdr1_id int,mdr5_id,city text,region text);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR28 (address int,id int,mdr1_id int,mdr23_id int, t_text text collate gsort,mdr21_id int,mdr27_id int,mdr14_id int);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE MDR29 (address int,id int,mdr1_id int,mdr24_id int, t_text text collate gsort,mdr22_id int,mdr25_id int,mdr26_id int);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 

	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE T4C_TYPES (map_id int,g_type int, type int, sub_type int, highest_level int);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE ZIP (map_id int,t_index int, lbl1 int, t_text text collate gsort);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE T43_MIASTA (mdr5_id int,map_id int,t_index int, id_53 int, id_4e int, lbl1 int, t_text text collate gsort);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE T4E_KRAJE (mdr14_id int,map_id int,t_index int, lbl1 int, t_text text collate gsort);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE T53_REGIONY (mdr13_id int,map_id int,t_index int, id_4e int, lbl1 int, t_text text collate gsort);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE POI (id int,map_id int,mdr11_id int,family int, sub_type int, rgn int, tre int, unkn1 int,idx_43 int,unkn2 int, lbl1 int,t_text text collate gsort,img_address int);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE ULICE_NET (id int,net_map text,map_id int,net int,lbl1 int,t_text text collate gsort,t_full_text text collate gsort,mdr7_id_temp int,sort_start_letter int, xflag int, xflag_text text collate gsort, sort_id int);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE TABLE ULICE (net_map text,map_id int,t_text text collate gsort,lbl1 int,id_43 int,id_53 int,id_4e int,net int);",NULL,NULL,NULL);if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
#endif
	//uzywany id jest wiekszy od 0 - nieuzywane równe 0
	//indexy
	//MDR20 - tworzenie indeksów
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX ulice_mdr20 ON ulice (map_id,lbl1,id_43);",NULL,NULL,NULL);

	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX POI_IDX_I ON POI_IDX (id);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX POI_IDX_I2 ON POI_IDX (rgn,tre);",NULL,NULL,NULL);

	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX MDR1_I ON MDR1 (id,map_id);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX MDR1_I2 ON MDR1 (map_id);",NULL,NULL,NULL);

	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX MDR15_I ON MDR15 (t_text);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX MDR15_L ON MDR15 (lbl_index,leng);",NULL,NULL,NULL);

	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX MDR5_TE ON MDR5 (t_text,region);",NULL,NULL,NULL);	
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX MDR5_TE_2 ON MDR5 (id_43,mdr1_id,t_text);",NULL,NULL,NULL);
	
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX MDR21_LM ON MDR21 (id);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX MDR22_LM ON MDR22 (id);",NULL,NULL,NULL);

	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX MDR21_LM_1 ON MDR21 (mdr1_id, id_53, id);",NULL,NULL,NULL);

	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX MDR7_LM_T ON MDR7_tmp (mdr1_id,mdr15_id);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX MDR7_LM_T_1 ON MDR7_tmp (country,sort_text);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX MDR7_LM_T_2 ON MDR7_tmp (city ,sort_text);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX MDR7_LM_T_3 ON MDR7_tmp (region ,sort_text);",NULL,NULL,NULL);

	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX MDR7_MAP_ID ON MDR7 (mdr1_id);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX MDR7_LM ON MDR7 (mdr1_id,mdr15_id);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX MDR7_LBL ON MDR7 (mdr1_id,lbl1);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX MDR7_C ON MDR7 (city,sort_text);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX MDR7_TEXT_ID ON MDR7 (t_text,id);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX MDR7_TEXT ON MDR7 (t_text);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX MDR7_N ON MDR7_tmp (net_map);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX MDR7_CITY ON MDR7 (map_id,lbl1);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX MDR7_CITY_ID ON MDR7 (map_id,lbl1,id);",NULL,NULL,NULL);

	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX T4C_TYPES_I ON T4C_TYPES (map_id);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX T4C_TYPES_IA ON T4C_TYPES (type);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX ZIP_I ON ZIP (map_id);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX ZIP_IA ON ZIP (t_index);",NULL,NULL,NULL);
	
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX t43_mdr20 ON t43_miasta (id_53,map_id,t_index,t_text);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX T43_MIASTA_ID53 ON T43_MIASTA (id_53);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX T43_MIASTA_I ON T43_MIASTA (map_id);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX T43_MIASTA_IA ON T43_MIASTA (t_index);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX T43_MIASTA_M ON T43_MIASTA (map_id,t_index);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX T43_MIASTA_M_2 ON T43_MIASTA (map_id,t_index,id_53);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX T43_MIASTA_T ON T43_MIASTA (t_text);",NULL,NULL,NULL);

	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX t4e_mdr20 ON t4e_kraje (t_index,map_id,t_text);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX T4E_KRAJE_I ON T4E_KRAJE (map_id);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX T4E_KRAJE_IA ON T4E_KRAJE (t_index);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX T4E_KRAJE_M ON T4E_KRAJE (map_id,t_index);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX T4E_KRAJE_TE ON T4E_KRAJE  (t_text);",NULL,NULL,NULL);
	
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX t53_mdr20 ON t53_regiony (t_index,map_id,id4e,t_text);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX t53_mdr20_2 ON t53_regiony (t_text, map_id,t_index);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX T53_REGIONY_ID_4e ON T53_REGIONY (id_4e);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX T53_REGIONY_I ON T53_REGIONY (map_id);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX T53_REGIONY_IA ON T53_REGIONY (t_index);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX T53_REGIONY_M ON T53_REGIONY (map_id,t_index);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX T53_REGIONY_M_2 ON T53_REGIONY (map_id,t_index,t_text);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX T53_REGIONY_TE ON T53_REGIONY (t_text);",NULL,NULL,NULL);
	
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX POI_I ON POI (map_id);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX POI_IA ON POI (family);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX POI_M ON POI (map_id,idx_43);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX POI_TEXT ON POI (t_text , family, sub_type);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX POI_RGN ON POI (map_id, rgn, tre);",NULL,NULL,NULL);

	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX ULICE_ID_43 ON ULICE (id_43);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX ULICE_IA ON ULICE (map_id,lbl1,id_43);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX ULICE_I ON ULICE (net_map);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX ULICE_L ON ULICE (map_id,lbl1);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX ULICE_ID ON ULICE_NET (id);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX ULICE_NET_IN ON ULICE_NET (net_map);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX ULICE_NET_IA ON ULICE_NET (t_text);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX ULICE_NET_M ON ULICE_NET (map_id);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX ULICE_NET_M ON ULICE_SORT_X (sort_id, xflag_text);",NULL,NULL,NULL);

	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX ULICE_IN ON ULICE (map_id);",NULL,NULL,NULL);
	b_result = sqlite3_exec(FTR::MDRbase,"CREATE INDEX ULICE_IAN ON ULICE (t_text);",NULL,NULL,NULL);

	b_result = sqlite3_exec(FTR::MDRbase,"INSERT INTO MDR15 (lbl_index,t_text,leng) VALUES(0,'',0);",NULL,NULL,NULL);
	if( b_result ) std::runtime_error(sqlite3_errmsg(FTR::MDRbase)); 
	return true;
}

bool doneMDR(bool erase) {
	sqlite3_close(FTR::MDRbase);
	if(erase)
		remove(FTR::mdrbase.c_str());

	return true;
}

using namespace FTR;

sqlite3	*getMDRbase() {
	return MDRbase;
}

int getCodePage() {
	return code_page;
}

void setMultibody(bool multibody) {
	FTR::multibody = multibody;
}


bool processIMG(std::string fileName,bool idx_ignore) {
	FTR::first_pass = true;

	database = true;

	if( database == true ) {		
		sqlite3_create_collation(FTR::MDRbase,"gsort",SQLITE_UTF8,NULL,compare);
		createTables();
	}
	
	//cout<<"1";
	map_id_from_filename = atoi(return_file_id(fileName).c_str());


	//cout<<"2";
	map1 = mapRead_openIMG(fileName.c_str(),sub_maps,FTR::readHeader,FTR::readData,NULL,NULL,NULL);
	if( map1 < 0 )
		errorClass::showError(-1,fileName.c_str(),errorClass::PE04);
	//
	//cout<<"3";
	mapRead_labelStyle(0);
	mapRead_readCities(map1,FTR::readIndex);
	mapRead_setInternal(map1,FTR::poiData_i,FTR::netData_i);

	if( map1 < 0 ) {
		//cout<<"Not a valid IMG file"<<endl;
		return false;
	}

	float	x0 = -180.0;
	float	x1 = +180.0;
	float	y0 = -90.0;
	float	y1 = +90.0;
	float	min1=0;
	float	min2=0;

	//cout<<"Reading objects for indexing."<<endl;
	current_map = map1;
	label_coding = mapRead_getLabelCoding(current_map,1);
	code_page = mapRead_getCodePage(current_map,1);
	
	if( label_coding == 6 ) {
		cout<<endl<<"Indexing IMG files which are using 6bit label coding may result in NON working index!"<<endl;
	}

	
	//cout<<"Collecting statistic data"<<endl;
	for( current_level = 9; current_level>=0; current_level-- ) {
		try {
			mapRead_readMap(current_map,0,x0,y0,x1,y1,min1,min2,current_level);
		} catch (exception &e) {
			cout<<flush<<e.what()<<endl;
			cout<<"Fatal error reading IMG file - please report to cgpsmapper@gmail.com"<<endl;
			exit(1);
		}
	}

#if _TEMP_REMOVE == 0
	if( idx_ignore == false ) {
		if( add_index_from_file(string(fileName+".idx")) == false ) {
			errorClass::showError(-1,string(fileName+".idx").c_str(),errorClass::PE05);
		}
	}
#endif
	unsigned short	language_id;

	if( code_page == 936 ) { setlocale(LC_ALL,".936"); language_id = 5; }
	else if( code_page == 950 ) { setlocale(LC_ALL,".950");language_id = 6;}
	else if( code_page == 1252 ) { setlocale(LC_ALL,".1252");language_id = 7;}
	else if( code_page == 1251 ) { setlocale(LC_ALL,".1251");language_id = 8;}
	else if( code_page == 932 ) { setlocale(LC_ALL,".932");language_id = 9;}
	else if( code_page == 1256 ) { setlocale(LC_ALL,".1256");language_id = 0x10;}
	else if( code_page == 1257 ) { setlocale(LC_ALL,".1257");language_id = 0x11;}
	else if( code_page == 949 ) { setlocale(LC_ALL,".949");language_id = 0xa;}
	else if( code_page == 874 ) { setlocale(LC_ALL,".874");language_id = 0xb;}
	else if( code_page == 1250 ) { setlocale(LC_ALL,".1250");language_id = 0xc;}
	else if( code_page == 1253 ) { setlocale(LC_ALL,".1253");language_id = 0xd;}
	else if( code_page == 1254 ) { setlocale(LC_ALL,".1254");language_id = 0xe;}
	else if( code_page == 1255 ) { setlocale(LC_ALL,".1255");language_id = 0xf;}
	else {
		//cout<<"Code page not recognized - assuming CP 1252. Index might not work!"<<endl;
		setlocale(LC_ALL,".1252");
		language_id = 7;
	}

	//cout<<"Creating FTR file"<<endl;
	first_pass = false;

	if( database == true )
		sqlite3_exec(FTR::MDRbase,"BEGIN TRANSACTION",NULL,NULL,NULL);
	

	storeTypes();
	storeCity();
	storeZip();
	storeStreets();
	storePOI();
	storeCountryRegion();
	if( database == true ) sqlite3_exec(FTR::MDRbase,"END TRANSACTION",NULL,NULL,NULL);

	mapRead_closeIMG(map1);
	mapRead_closeAll();

	deleteAllData();
	return true;
}
