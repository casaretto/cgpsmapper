/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

#include <fstream>
//#include <omp.h>
//#include <omp_lib.h>
#include "preview.h"
#include "filexor.h"
#include "error.h"
#include "utils.h"
#include "ftrcreate.h"
#include "compare.h"
#include "./../gusb/sendmapDLL.h"
#include "vcgl_gpc.h"
#include <boost/progress.hpp>

#ifndef _WIN32
#include <libgen.h>
#endif

using namespace std;
using namespace map_read;

namespace exportIMG {

	IMG	*img;
	int		export_level;
	float	m_x0,m_x1,m_y0,m_y1;
	
	string	img_filename;
	string	img_name;
	string	img_id;
	ofstream out_file;
	int		max_detail_level;

	bool					background;
	vcgl_gpc::gpc_polygon*	background_poly;
	vcgl_gpc				gpc;

	//             (int map_index,int map_sub_index,const char* map_id,const char* map_name,const char* copyright1,const char* copyright2,int levels,float &x0,float &x1,float &y0,float &y1,int locked,unsigned char level_def[10])
	int readHeader_(int map_index,int map_sub_index,const char* map_id,const char* map_name,const char* copyright1,const char* copyright2,int levels,float &x0,float &x1,float &y0,float &y1,int locked,unsigned char level_def[10]) {
		img_name = map_name;
		img_id = map_id;

		export_level = levels-1;
		out_file<<";Map:   "<<map_name<<endl;
		out_file<<";MapID: "<<map_id<<endl;
		m_x0 = x0;
		m_x1 = x1;
		m_y0 = y0;
		m_y1 = y1;

		max_detail_level = level_def[levels];

		return 1;
	}

	//            (int layer,int object_type,int type,int sub_type, unsigned int poi_address, unsigned int net_address,const char* label,mapRead_coord_integer* coords,int coord_size,const char flag,unsigned int reserved)
	int readDataI_(int layer,int object_type,int type,int sub_type, unsigned int poi_address, unsigned int net_address,const char* label,mapRead_coord_integer* coords,int coord_size,const char flag,unsigned int reserved) {
		return 1;
	}
	
	//           (int layer,int object_type,int type,int sub_type, unsigned int poi_address,unsigned int net_address,unsigned int object_id,const char* label,mapRead_coord* coords,int coord_size,const char flag,unsigned int reserved)
	int readData_(int layer,int object_type,int type,int sub_type, unsigned int poi_address,unsigned int net_address,unsigned int object_id,const char* label,mapRead_coord* coords,int coord_size,const char flag,unsigned int reserved) {
		out_file.precision(10);
	
		//if( object_type == 0x80 && type == 0x4b && background == true)
		//	return 1;

		if( object_type == 0x80 && type == 0x4b00 ) {
			background = true;

			vcgl_gpc::gpc_vertex_list* vertex_list = (vcgl_gpc::gpc_vertex_list*)malloc(sizeof(vcgl_gpc::gpc_vertex_list));
			vertex_list->vertex = (vcgl_gpc::gpc_vertex*)malloc(coord_size * sizeof(vcgl_gpc::gpc_vertex));
			vertex_list->num_vertices = coord_size;

			for( int i =0; i < coord_size; ++i ) {
				vertex_list->vertex[i].x = coords[i].we;
				vertex_list->vertex[i].y = coords[i].ns;
			}
			gpc.gpc_add_contour(background_poly,vertex_list);

			return 1;
		}

		out_file<<"[RGN"<<hex<<object_type<<dec<<"]"<<endl;
#ifdef _DEBUG
		out_file<<"Type=0x"<<hex<<(type>>8)<<dec<<endl;
		out_file<<"SubType="<<sub_type<<endl;
		if( object_type & 0x010000 )
			out_file<<"Marine=Y"<<endl;
#else
		out_file<<"Type=0x"<<hex<<type<<dec<<endl;
#endif
		//out_file<<"SubType="<<sub_type<<endl;
		if( label != NULL )
			out_file<<"Label="<<label<<endl;

		out_file<<"Data0=";//<<layer<<"=";
		for( int i =0; i < coord_size; ++i ) {
			out_file<<"("<<fixed<<coords[i].ns<<","<<fixed<<coords[i].we<<")";
			if( (i+1) < coord_size ) out_file<<",";
		}
		out_file<<endl;
		if( flag & 0x01 ) {
			img->readPOI(type,sub_type,poi_address,net_address);
		}

		out_file<<"[END]"<<endl<<endl;
		return 1;
	}

	//          (const char* house_number,const char* street_name,const char* city, const char* region, const char* country,const char* zip,const char* phone)
	int poiData_(const char* house_number,const char* street_name,const char* city, const char* region, const char* country,const char* zip,const char* phone) {
		if( strlen(house_number) ) out_file<<"HouseNumber="<<house_number<<endl;
		if( strlen(street_name) ) out_file<<"StreetDesc="<<street_name<<endl;
		if( strlen(zip) ) out_file<<"Zip="<<zip<<endl;
		if( strlen(phone) ) out_file<<"Phone="<<phone<<endl;

		if( strlen(city) ) out_file<<"City="<<city<<endl;
		if( strlen(region) ) out_file<<"Region="<<region<<endl;
		if( strlen(country) ) out_file<<"Country="<<country<<endl;

		return 1;
	}
}
//MDX
MDX_element::MDX_element() {
	this->imgID = 0;
	this->familySubID = 0;
	this->familyID = 0;
	this->imgFileName = 0;
}

MDX_element::MDX_element(unsigned long imgID,unsigned short familySubID,unsigned short familyID,unsigned long imgFileName) {
	this->imgID = imgID;
	this->familySubID = familySubID;
	this->familyID = familyID;
	this->imgFileName = imgFileName;
}

bool MDX_element::operator()(const MDX_element &e1,const MDX_element &e2) {
	return e2.imgID > e1.imgID;
}

void MDX_manager::AddIMG(MDX_element element) {
	this->mdx.push_back(element);
}

void MDX_manager::Write(const char* file_name) {
	char buffer[4];
	unsigned short version = 0x64;
	unsigned long rec_size = 0x0c;
	unsigned long rec_cnt = static_cast<unsigned long>(mdx.size());
	xor_fstream *file = new xor_fstream(file_name,"wb+");

	sort(mdx.begin(),mdx.end(),MDX_element());

	memcpy(buffer,"Midx",4);
	//header
	file->Write(buffer,4);
	//version
	file->Write(&version,2);
	//record size
	file->Write(&rec_size,4);
	//record count
	file->Write(&rec_cnt,4);

	for(vector<MDX_element>::iterator mdx_iter = mdx.begin(); mdx_iter < mdx.end(); mdx_iter++ ) {
		file->Write(& (*mdx_iter).imgID,4);
		file->Write(& (*mdx_iter).familySubID,2);
		file->Write(& (*mdx_iter).familyID,2);
		file->Write(& (*mdx_iter).imgFileName,4);
	}

	delete file;
}

//MDR
previewMDR::~previewMDR() {
	doneMDR(erase_temporary);
	list_img.clear();
}

previewMDR::previewMDR(string base_name,bool idx_ignore) {
#ifdef _DEBUG
	erase_temporary = false;
#else
	erase_temporary = true;
#endif
	if( base_name.size() )
		erase_temporary = false;

	this->idx_ignore = idx_ignore;
	if( initMDR(base_name) == false )
		errorClass::showError(-1,NULL,errorClass::PE03);
}

void previewMDR::execute(bool by_region,bool multibody) {
	std::vector<img_item>::iterator img_file;

	setMultibody(multibody);

	//#pragma omp parallel for 
	//for( int i = 0; i < list_img.size(); i++) {
	//	img_file = list_img.begin()+i;
	for(img_file = list_img.begin(); img_file < list_img.end(); img_file++) {
		cout<<"MDRProcessing: "<<img_file->name<<" ";
		processIMG(img_file->name.c_str(),idx_ignore);
		if( getCodePage() != codepage ) {
			cout<<"WARNING! codepage in file is : "<<getCodePage()<<" while global codepage is set to : "<<codepage<<" - index will probably NOT work!";
		}
		cout<<endl;
	}

	//create MDR
	//MDR4
	if( by_region )
		mdrCreator.set_by_region();

	sqlite3_exec(getMDRbase(),"BEGIN TRANSACTION",NULL,NULL,NULL);

	mdrCreator.set_multibody(multibody);
	cout<<"Creating indexes - be patient"<<endl;
	mdrCreator.index_mdr1();
	//kraje i regiony
	//cout<<".";
	mdrCreator.index_mdr13_mdr14();
	//zip
	//cout<<".";
	mdrCreator.index_mdr6();
	//miasta
	//cout<<".";
	{
		mdrCreator.index_mdr5();
		//cout<<"mdr5 :";
	}
	//ulice
	//cout<<endl;
	{
		mdrCreator.index_mdr7();
		//cout<<"mdr7 :";
	}
	//poi
	//cout<<endl;
	{
		mdrCreator.index_mdr11();
		//cout<<"mdr11 :";
	}
	//poi index
	//cout<<endl;
	{
		//cout<<"mdr9-10 :";
		mdrCreator.index_mdr9_mdr10();
	}
	////cout<<endl;
	mdrCreator.index_mdr4();//typy
	mdrCreator.index_mdr8();//indexy do MDR7
	mdrCreator.index_mdr12();//indexy do MDR11

	//cout<<".";
	{
		//cout<<"mdr20-21-22 :";
		//!!
		mdrCreator.index_mdr20_mdr21_mdr22();
	}
	cout<<"Transaction end"<<endl;
	sqlite3_exec(getMDRbase(),"END TRANSACTION",NULL,NULL,NULL);

	cout<<endl<<"Writing MDR file";
	mdrCreator.write_mdr(upper_case(fileName) + ".MDR");

	//create IMG file
	string final_img = upper_case(fileName);
	string mdr_file = upper_case(fileName) + ".MDR";
	final_img += "_MDR.IMG";
	
	string sort_filename = "CP1252  ";
	if( codepage == 874) sort_filename = "CP874TI ";
	else if( codepage == 932) sort_filename = "CP932JP ";
	else if( codepage == 936) sort_filename = "CP936   ";
	else if( codepage == 949) sort_filename = "CP949KR ";
	else if( codepage == 950) sort_filename = "CP950   ";
	else if( codepage == 1250) sort_filename = "CP1250PL";
	else if( codepage == 1251) sort_filename = "CP1251  ";
	else if( codepage == 1253) sort_filename = "CP1253GR";
	else if( codepage == 1254) sort_filename = "CP1254TK";
	else if( codepage == 1255) sort_filename = "CP1255HB";
	else if( codepage == 1256) sort_filename = "CP1256AB";
	else if( codepage == 1257) sort_filename = "CP1257BC";

	string sort_file = sortfile_location + "sort.img";

	if( se_connect(2,final_img.c_str(),"INDEX") == _SE_OK ) {
		se_disable_mps();
		se_disable_output();
		se_set_mapset("INDEX");

		se_add_file(mdr_file.c_str(),"");
		if( se_add_file_internal(sort_file.c_str(),sort_filename.c_str(),upper_case(fileName).c_str()) != _SE_OK )
			errorClass::showError(-1,NULL,errorClass::PE07);

		if( se_upload_no_callback(9600) == _SE_OK )
			cout<<" "<<final_img<<" is created."<<endl;

		se_disconnect();
	}
	if( erase_temporary )
		remove(string(upper_case(fileName) + ".MDR").c_str());
}

//TDB
previewTDB::previewTDB() {
	//MP parametry
	levels = 0;
	zoom_def[0] = 0;
	level[0] = 0;
	TRE_ID = 1; //ID for preview map

	fileName = "file_name";

	FID = 0;
	colour = 0;
	height = false;
	locked = false;
	codepage = 0;
	productCode = 0;
	mapVersion = 100;
}

void previewTDB::execute(bool &mdr, bool &pvx) {
	int				t_sub;
	int				t_idx;
	int				nod_bit_size = 0;
	mapRead_rect	bounds;
	mapRead_coord	minimum_size;
	std::vector<img_item>::iterator img_file;
	std::string		internal_name;

	mdr = false;

	internal_name = fileName + ".mp";
	exportIMG::out_file.open(internal_name.c_str(),ios_base::out);

	//Preparing TDB file / class
	TDB_file = new TDB(productCode,mapsourceName,CDSetName,mapVersion,locked,marine);
	TDB_file->m_TDB_map.areaCode = 1;
	TDB_file->m_TDB_map.name = mapSetName;
	TDB_file->m_TDB_map.maxE = -180;
	TDB_file->m_TDB_map.maxN = -180;
	TDB_file->m_TDB_map.maxS = -180;
	TDB_file->m_TDB_map.maxW = -180;

	TDB_file->m_TDB_header.unk10_bg_color = colour;
	TDB_file->setSubFamily(productCode);
	if( FID ) {
		TDB_file->setCodePage(codepage);
		TDB_file->setFamily(FID);	
	}
	if( height ) {
		TDB_file->setCodePage(codepage);
		TDB_file->setHeighProvile();
	}

	//adding copyrights
	if( copy1.size() ) {
		TDB_info_string info1_string;
		info1_string.code = 0x306;
		info1_string.name = copy1;
		TDB_file->m_TDB_copyright.list_TDB_info_string.push_back(info1_string);
	}
	
	TDB_info_string infoS_string;
	infoS_string.code = 0x0f000207;
	infoS_string.name = "NOB.BMP"; //nazwa bitmapy - copyright!
	TDB_file->m_TDB_copyright.list_TDB_info_string.push_back(infoS_string);
	

	TDB_info_string info2_string;
	info2_string.code = 0x206;
	info2_string.name = "gps_mapper";
	TDB_file->m_TDB_copyright.list_TDB_info_string.push_back(info2_string);
	
	if( copy2.size() ) {
		TDB_info_string info3_string;
		info3_string.code = 0x306;
		info3_string.name = copy2;
		TDB_file->m_TDB_copyright.list_TDB_info_string.push_back(info3_string);
	}
	//
	if( copy3.size() ) {
		TDB_info_string info4_string;
		info4_string.code = 0x106;
		info4_string.name = copy3;
		TDB_file->m_TDB_copyright.list_TDB_info_string.push_back(info4_string);
	}
	//end of copyrights

	exportIMG::out_file<<"[IMG ID]"<<endl;
	exportIMG::out_file<<"ID="<<TRE_ID<<endl;
	exportIMG::out_file<<"Preview=Y"<<endl;
	exportIMG::out_file<<"Name="<<mapSetName<<endl;
	exportIMG::out_file<<"Codepage="<<codepage<<endl;

	exportIMG::out_file<<"Levels="<<levels+1<<endl;
	for( int i = 0; i <= levels; ++i ) {
		exportIMG::out_file<<"Level"<<i<<"="<<(int)level[i]<<endl;
		exportIMG::out_file<<"Zoom"<<i<<"="<<(int)zoom_def[i]<<endl;
	}
	exportIMG::out_file<<"[END]"<<endl<<endl;

	for(img_file = list_img.begin(); img_file < list_img.end(); img_file++) {

		//processing files
//		#pragma omp critical
		{
			cout<<"TDBProcessing: "<<img_file->name<<endl;
		}

		{
			xor_fstream* t_file = new xor_fstream(string(img_file->name+".idx").c_str(),"rb");
			if( !t_file->error )
				mdr = true;
			delete t_file;
		}

		// - now index and preview are separate - just to have more clear code...

		exportIMG::img = new IMG(exportIMG::readHeader_,exportIMG::readData_,exportIMG::readDataI_,exportIMG::poiData_,NULL);
		exportIMG::img->setLableStyle(1);
		exportIMG::background = false;
		exportIMG::background_poly = (vcgl_gpc::gpc_polygon*)malloc(sizeof (vcgl_gpc::gpc_polygon));
		exportIMG::background_poly->num_contours = 0;
		exportIMG::background_poly->contour = NULL;

		t_idx = exportIMG::img->openMap(img_file->name.c_str(),t_sub);
		if( t_idx < 0 )
			errorClass::showError(-1,img_file->name.c_str(),errorClass::PE04);
		exportIMG::img_filename = img_file->id_name;

		bounds.x0 = -180.0;
		bounds.x1 = 180.0;
		bounds.y0 = -90.0;
		bounds.y1 = 90.0;

		if( exportIMG::max_detail_level <= this->level[0] ) {
			cout<<endl<<"In IMG file "<< img_file->name <<" less detailed layer is "<<exportIMG::max_detail_level<<" bits while you are trying to create preview with most detailed layer at "<<(int)this->level[0]<<endl;
			cout<<"Most detailed layer in the preview file must be smaller."<<endl;
			errorClass::showError(0,NULL,errorClass::PE11);
			//cout<<this->level[0];
		}

		minimum_size.we = 0.0;
		minimum_size.ns = 0.0;
		//generate MP file
		if( !pvx ) {
			try {
				exportIMG::img->readMap(0,bounds,minimum_size,exportIMG::export_level);
			} catch (exception &e) {
				cout<<flush<<e.what()<<endl;
				cout<<"Fatal error reading IMG file - please report to cgpsmapper@gmail.com"<<endl;
				exit(1);
			}
		}
		//in case of transparency - generate 'generic'
		if( exportIMG::background == false || pvx ) {
			//this->t transparent = 1;

			exportIMG::out_file<<"[RGN80]"<<endl;
			exportIMG::out_file<<"Type=0x4a00"<<endl;
			exportIMG::out_file<<"Label="<<trim(exportIMG::img_name) + "~[0x1d]" + exportIMG::img_filename<<endl;

			exportIMG::out_file<<"Data0=";//<<layer<<"=";
			exportIMG::out_file<<"("<<exportIMG::m_y0<<","<<exportIMG::m_x0<<"),";
			exportIMG::out_file<<"("<<exportIMG::m_y1<<","<<exportIMG::m_x0<<"),";
			exportIMG::out_file<<"("<<exportIMG::m_y1<<","<<exportIMG::m_x1<<"),";
			exportIMG::out_file<<"("<<exportIMG::m_y0<<","<<exportIMG::m_x1<<")";
			exportIMG::out_file<<endl;
			exportIMG::out_file<<"[END]"<<endl<<endl;
		} else {
			vcgl_gpc::gpc_polygon gb;
			vcgl_gpc::gpc_polygon gret;
//			vcgl_gpc::gpc_polygon gret2;

			gb.num_contours = 0;
			gb.contour = NULL;

			vcgl_gpc::gpc_vertex_list* vertex_list = (vcgl_gpc::gpc_vertex_list*)malloc(sizeof(vcgl_gpc::gpc_vertex_list));
			vertex_list->vertex = (vcgl_gpc::gpc_vertex*)malloc(4 * sizeof(vcgl_gpc::gpc_vertex));
			vertex_list->num_vertices = 4;

			vertex_list->vertex[0].x = exportIMG::m_x0;	vertex_list->vertex[0].y = exportIMG::m_y0;
			vertex_list->vertex[1].x = exportIMG::m_x0;	vertex_list->vertex[1].y = exportIMG::m_y1;
			vertex_list->vertex[2].x = exportIMG::m_x1;	vertex_list->vertex[2].y = exportIMG::m_y1;
			vertex_list->vertex[3].x = exportIMG::m_x1;	vertex_list->vertex[3].y = exportIMG::m_y0;
			exportIMG::gpc.gpc_add_contour(&gb,vertex_list);

			exportIMG::gpc.gpc_polygon_clip(vcgl_gpc::GPC_INT,&gb,exportIMG::background_poly,&gret);

			//exportIMG::gpc.gpc_polygon_clip(vcgl_gpc::GPC_XOR,&gret2,&gb,&gret);

			exportIMG::out_file<<"[RGN80]"<<endl;
			//exportIMG::out_file<<"Type=0x4"<<endl;
			exportIMG::out_file<<"Type=0x4a00"<<endl;
			exportIMG::out_file<<"Label="<<trim(exportIMG::img_name) + "~[0x1d]" + exportIMG::img_filename<<endl;

			//exportIMG::out_file<<"Data0=";//<<layer<<"=";
			for( int j =0; j < gret.num_contours; ++j ) {
				exportIMG::out_file<<endl<<"Data0=";//<<layer<<"=";
				for( int i =0; i < gret.contour[j].num_vertices; ++i ) {					
					exportIMG::out_file<<"("<<gret.contour[j].vertex[i].y<<","<<
					gret.contour[j].vertex[i].x<<"),";					
				}
			}

			exportIMG::out_file<<endl;
			exportIMG::out_file<<"[END]"<<endl<<endl;

			exportIMG::gpc.gpc_free_polygon(&gb);
			exportIMG::gpc.gpc_free_polygon(&gret);
//			exportIMG::gpc.gpc_free_polygon(&gret2);
		}
		exportIMG::gpc.gpc_free_polygon(exportIMG::background_poly);

		//add TDB item
		TDB_img t_img;
		t_img.areaCode = 1;//only one...
		t_img.regionID = img_file->region_id;
		t_img.regionName = img_file->region_name;

		//tutaj musi byc ID z nazwy pliku - sprawdz
		t_img.IMGID = atoi(return_file_id(img_file->name).c_str());//atoi(exportIMG::img_id.c_str());
		t_img.img_name = trimString(exportIMG::img_name);
		t_img.maxN = static_cast<int>(exportIMG::m_y1 * ((double(1 << 30) / double(90.0))));
		t_img.maxS = static_cast<int>(exportIMG::m_y0 * ((double(1 << 30) / double(90.0))));
		t_img.maxE = static_cast<int>(exportIMG::m_x1 * ((double(1 << 30) / double(90.0))));
		t_img.maxW = static_cast<int>(exportIMG::m_x0 * ((double(1 << 30) / double(90.0))));

		TDB_file->m_TDB_map.maxE = max(TDB_file->m_TDB_map.maxE,t_img.maxE);
		TDB_file->m_TDB_map.maxW = max(TDB_file->m_TDB_map.maxW,t_img.maxW);
		TDB_file->m_TDB_map.maxS = max(TDB_file->m_TDB_map.maxS,t_img.maxS);
		TDB_file->m_TDB_map.maxN = max(TDB_file->m_TDB_map.maxN,t_img.maxN);

		t_img.GMP_length = exportIMG::img->getFileSize(t_idx,0,imgGMP,internal_name);
		if( t_img.GMP_length ) {
			t_img.img_sections = 2;
			t_img.img_sections_real = 1;
			t_img.gmp_name = internal_name + ".GMP";
		} else {
			t_img.img_sections = 4;
			t_img.img_sections_real = 3;
		
			t_img.TRE_length = exportIMG::img->getFileSize(t_idx,0,imgTRE,internal_name);
			t_img.tre_name = internal_name + ".TRE";

			t_img.RGN_length = exportIMG::img->getFileSize(t_idx,0,imgRGN,internal_name);
			t_img.rgn_name = internal_name + ".RGN";

			t_img.LBL_length = exportIMG::img->getFileSize(t_idx,0,imgLBL,internal_name);
			t_img.lbl_name = internal_name + ".LBL";

			t_img.NET_length = exportIMG::img->getFileSize(t_idx,0,imgNET,internal_name);
			t_img.net_name = internal_name + ".NET";

			t_img.NOD_length = exportIMG::img->getFileSize(t_idx,0,imgNOD,internal_name);
			t_img.nod_name = internal_name + ".NOD";
			//mdr = true;
			if( t_img.NET_length ) {
				mdr = true;
				t_img.img_sections = 5;
				t_img.img_sections_real = 4;
			}
			if( t_img.NOD_length ) {
				t_img.img_sections = 6;
				t_img.img_sections_real = 5;
			}
		}

		{ 
			nod_bit_size = 0;
			int nod_section_size = exportIMG::img->getNOD1Size(t_idx,0);
			while( nod_section_size>0 ) {
				nod_bit_size++;
				nod_section_size>>=1;
			}
		}

		if( TDB_file->m_TDB_header.unk8 < nod_bit_size )
			TDB_file->m_TDB_header.unk8 = nod_bit_size;

		TDB_file->m_TDB_img.push_back(t_img);

		//MDX
		MDX_file.AddIMG(MDX_element(atoi(exportIMG::img_id.c_str()),img_file->product_id,FID,t_img.IMGID));

		exportIMG::img->closeMap(t_idx);
		delete exportIMG::img;
		exportIMG::img = NULL;
	}
	exportIMG::out_file.close();
	if( !pvx ) 
		MDX_file.Write(string(fileName + ".MDX").c_str());
	
	internal_name = fileName + ".TDB";
	TDB_file->WriteTDBfile(internal_name.c_str());
	if( FID ) TDB_file->SignTDB(internal_name.c_str());
	delete TDB_file;


	//create REG file:
	std::ofstream reg_file;
	reg_file.open(string(fileName + ".reg").c_str(),ios_base::out);
	reg_file<<"REGEDIT4"<<endl<<endl;
	if( FID ) {
		reg_file<<"[HKEY_LOCAL_MACHINE\\SOFTWARE\\Garmin\\MapSource\\Families\\"<<fileName<<"]"<<endl;
		reg_file<<"\"ID\"=hex:"<<hex<<((FID&0x00ff)<10?"0":"")<<(FID&0x00ff)<<","<<(((FID&0xff00)>>8)<10?"0":"")<<((FID&0xff00)>>8)<<endl<<endl;
		if( mdr && !pvx ) {
			reg_file<<"\"IDX\"=\"C:\\\\myMaps\\\\"<<fileName.c_str()<<".mdx"<<"\""<<endl;
			reg_file<<"\"MDR\"=\"C:\\\\myMaps\\\\"<<fileName.c_str()<<"_mdr.img"<<"\""<<endl;
		}

		reg_file<<"[HKEY_LOCAL_MACHINE\\SOFTWARE\\Garmin\\MapSource\\Families\\"<<fileName<<"\\"<<dec<<productCode<<hex<<"]"<<endl;
	} else
		reg_file<<"[HKEY_LOCAL_MACHINE\\SOFTWARE\\Garmin\\MapSource\\Products\\"<<fileName<<"]"<<endl;

	reg_file<<"\"LOC\"=\"C:\\\\myMaps\\\\imgs\\\\\""<<endl;
	reg_file<<"\"BMAP\"=\"C:\\\\myMaps\\\\"<<fileName.c_str()<<".img\""<<endl;
	reg_file<<"\"TDB\"=\"C:\\\\myMaps\\\\"<<fileName.c_str()<<".tdb\""<<endl;

	reg_file.close();

	if( pvx )
		mdr = false;
}

//global preview
preview::preview(string base_name,bool idx_ignore)  {
	mdr = new previewMDR(base_name,idx_ignore);
	tdb = new previewTDB();
}

void preview::setHomePath(const char* progPath) {
#ifndef _WIN32
#define MAX_PATH 2040
#endif
	char	local_file_name[MAX_PATH];
	//char	file_type[4];
	char	t_file_type[MAX_PATH];
	char	drive[5];
	char	dir[MAX_PATH];
#ifdef _WIN32
	_splitpath(progPath,drive,dir,local_file_name,t_file_type);

	mdr->sortfile_location = drive;
	mdr->sortfile_location += dir;
#else
	strncpy(t_file_type,progPath,MAX_PATH-1);
	char* t_ext = strrchr(t_file_type,'/');
	memset(local_file_name,0,sizeof(local_file_name));
	strncpy(local_file_name,t_file_type,t_ext - t_file_type+1);
	//mdr->sortfile_location = basename(local_file_name);
	mdr->sortfile_location = local_file_name;
#endif
}

preview::~preview() {
	exportIMG::img_filename = "";
	exportIMG::img_name = "";
	exportIMG::img_id = "";

	delete mdr;
	delete tdb;
}

void preview::run(bool by_region,bool multibody,bool pvx_mode) {
	bool do_mdr = false;
	tdb->execute(do_mdr,pvx_mode); // jesli s¹ NET - to do_mdr ustawione bedzie na TRUE
	if( do_mdr )
		mdr->execute(by_region,multibody);
}

void preview::processFiles() {
	cout<<endl<<"REG file has been created - but you **MUST** edit it before use!"<<endl;
	cout<<"Fix key TDB to point to the file in the right directory"<<endl;
	cout<<"Fix key BMAP to point to the file in the right directory"<<endl;
	cout<<"Fix key LOC to point to the directory where detail IMG are stored"<<endl;
}

//Reading of config files
bool preview::openConfig(const char* fileName) {
	string	key,value;
	int		t_read;

	file = new xor_fstream(fileName,"rb");	
	if( file->error ) { 
		errorClass::showError(-1,NULL,errorClass::PE01);
		return false;
	}
	file->SetXorMask(0);

	t_read = file->ReadInput(key,value);
	while( t_read == 1 || t_read == 3 || t_read == 5 )
	{
		if( key == string("[FILES]") ) 
			readFiles();
		if( key == string("[MAP]") ) {
			readMap();
		}
		if( key == string("[DICTIONARY]") ) 
			readDictionary();
		t_read = file->ReadInput(key,value);
	}

	cout<<"Codepage set to: "<<tdb->codepage<<endl;
	g_compare::init(tdb->codepage);

	delete file;
	return true;
}

void preview::readDictionary() {
	string    key,value;
	int       t_read;

	tdb->dictionary.push_back("[DICTIONARY]");
	t_read = file->ReadInput(key,value);
	while( t_read != 0 && key != "[END-DICTIONARY]"  )
	{
		tdb->dictionary.push_back(key+"="+value);
		t_read = file->ReadInput(key,value);
	}
	tdb->dictionary.push_back("[END-DICTIONARY]");
}

void preview::readFiles() {
	string  key,value;
	int     t_read;
	string	t_name;
	bool	bad_name;
	string	current_region;
	int		current_regionID = 1;
	int		current_productID = tdb->productCode;

	current_region = "Map";

	t_read = file->ReadInput(key,value);
	while( t_read != 0 && key.find("[END") > 0) {
		if( key == "REGION") {
			current_regionID = atoi(value.substr(0,value.find(",")).c_str());
			current_region = value.substr(value.find(",")+1);
		}
		if( key == "PRODUCT" || key == "PRODUCTCODE" ) {
			current_productID = atoi(value.c_str());
		}
		if( key == "IMG" )  {
			while( value[value.size()-1] == ' ' )
				value.erase(value.size()-1,1);
			//cout<<"'"<<value<<"'"<<endl;

			t_name = filename_id(value);
			string t_tmp = return_file_id(value);

			bad_name = false;
			for( size_t t_poz = 0; t_poz<t_tmp.size(); ++t_poz ) {
				if( t_tmp[t_poz] < '0' || t_tmp[t_poz] > '9' )
					bad_name = true;
			}
			if( t_tmp.size() != 8 ) bad_name = true;
			if( bad_name ) {
				errorClass::showError(file->line_no,file->file_name.c_str(),errorClass::PE02);
			}
			if( upper_case(mdr->fileName) == upper_case(t_name) ) {
				errorClass::showError(file->line_no,file->file_name.c_str(),errorClass::PE10);
			}

			img_item file_item(t_name,value,current_regionID,current_region,current_productID);
			tdb->list_img.push_back(file_item);
			mdr->list_img.push_back(file_item);
		}
		t_read = file->ReadInput(key,value);
	}
}

void preview::readMap() {
	string    key,value;
	int       t_read;

	tdb->FID = 0;
	//ID = "09999999";
	tdb->TRE_ID = 9999999;
	tdb->codepage = 0;//1252;
	tdb->colour = 0;
	tdb->height = false;
	t_read = file->ReadInput(key,value);
	while( t_read != 0 && key != "[END-MAP]")
	{
		if( t_read == 2 ) {
			if( key == "PRODUCTCODE" ) tdb->productCode = atoi(value.c_str());
			if( key == "MAPVERSION" ) tdb->mapVersion = atoi(value.c_str());
			if( key == "LOCK" ) tdb->locked = (upper_case(value)=="Y" || value=="1");
			if( key == "MARINE" ) tdb->marine = (upper_case(value)=="Y" || value=="1" || upper_case(value)=="B" || upper_case(value)=="G");
			if( key == "FID" ) tdb->FID = atoi(value.c_str());
			if( key == "COLOR" ) tdb->colour = atoi(value.c_str());
			if( key == "ID" ) {
				string ID;
				ID = value;
				if( ID[0] == 'I' ) {
					char	*end;
					string	t_id = "0" + ID;
					t_id[1] = 'x';
					tdb->TRE_ID = strtol(t_id.c_str(), &end, 0);
				} else {
					tdb->TRE_ID = atoi(ID.c_str());
				}
			}
			if( key == "FILENAME" ) {
					tdb->fileName = value;
					mdr->fileName = value;
			}
			if( key == "HEIGHTPROFILE" ) tdb->height = (upper_case(value)=="Y" || value=="1");
			if( key == "CODEPAGE" ) tdb->codepage = atoi(value.c_str());
			if( key == "MAPSOURCENAME" ) tdb->mapsourceName = value;
			if( key == "MAPSETNAME" ) tdb->mapSetName = value;
			if( key == "CDSETNAME" ) tdb->CDSetName = value;
			if( key == "COPY1" ) tdb->copy1 = value;
			if( key == "COPY2" ) tdb->copy2 = value;
			if( key == "COPY3" ) tdb->copy3 = value;

			if( key == "LEVELS" ) tdb->levels = atoi(value.c_str()) - 1;
			if( key == "LEVEL0" ) tdb->level[0] = atoi(value.c_str());
			if( key == "LEVEL1" ) tdb->level[1] = atoi(value.c_str());
			if( key == "LEVEL2" ) tdb->level[2] = atoi(value.c_str());
			if( key == "LEVEL3" ) tdb->level[3] = atoi(value.c_str());
			if( key == "LEVEL4" ) tdb->level[4] = atoi(value.c_str());
			if( key == "LEVEL5" ) tdb->level[5] = atoi(value.c_str());
			if( key == "LEVEL6" ) tdb->level[6] = atoi(value.c_str());
			if( key == "LEVEL7" ) tdb->level[7] = atoi(value.c_str());
			if( key == "LEVEL8" ) tdb->level[8] = atoi(value.c_str());
			if( key == "LEVEL9" ) tdb->level[9] = atoi(value.c_str());

			if( key == "ZOOM0" ) tdb->zoom_def[0] = atoi(value.c_str());
			if( key == "ZOOM1" ) tdb->zoom_def[1] = atoi(value.c_str());
			if( key == "ZOOM2" ) tdb->zoom_def[2] = atoi(value.c_str());
			if( key == "ZOOM3" ) tdb->zoom_def[3] = atoi(value.c_str());
			if( key == "ZOOM4" ) tdb->zoom_def[4] = atoi(value.c_str());
			if( key == "ZOOM5" ) tdb->zoom_def[5] = atoi(value.c_str());
			if( key == "ZOOM6" ) tdb->zoom_def[6] = atoi(value.c_str());
			if( key == "ZOOM7" ) tdb->zoom_def[7] = atoi(value.c_str());
			if( key == "ZOOM8" ) tdb->zoom_def[8] = atoi(value.c_str());
			if( key == "ZOOM9" ) tdb->zoom_def[9] = atoi(value.c_str());
		}
		t_read = file->ReadInput(key,value);
	}
	if( tdb->codepage == 0 ) {
		cout<<"Warning - no CODEPAGE key in [MAP] Assuming default - CP1252."<<endl;
		tdb->codepage = 1252;
	}
	mdr->codepage = tdb->codepage;

/*	if( locked ) {
		if( getSetting("LOCK") == "YES" ) {
			if( atoi(getSetting("EXTRAINFO").c_str()) != FID ) {
				FID = atoi(getSetting("EXTRAINFO").c_str());
				cout<<"WARNING - FID value has been overwritten by value from registration."<<endl;
				cout<<"New FID value is: "<<FID<<endl;
			}
		} else
			locked = false;
		cout<<"Lock status: "<<locked<<endl;
	}*/
}
