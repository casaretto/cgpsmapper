/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

#include <iostream>
#include "img.h"

using namespace std;

IMG *img;

int CALL_API readHeader(int map_index,const char* map_id,const char* map_name,const char* copyright1,const char* copyright2,int levels,float &x0,float &x1,float &y0,float &y1,int locked,unsigned char level_def[10]) {
	cout<<"[IMG ID]"<<endl;
	cout<<"Name="<<map_name<<endl;
	cout<<"Copyright="<<copyright1<<endl;
	cout<<"Levels="<<levels<<endl;
	for( int i = 0; i < levels; ++i ) {
		cout<<"Level"<<i<<"="<<(int)level_def[i]<<endl;
	}
	cout<<"[END]"<<endl<<endl;

	return 1;
}

int CALL_API readDataI(int layer,int object_type,int type,int sub_type, unsigned int poi_address, unsigned int net_address,const char* label,mapRead_coord_integer* coords,int coord_size) {
	return 1;
}

int CALL_API readData(int layer,int object_type,int type,int sub_type, unsigned int poi_address, unsigned int net_address,const char* label,mapRead_coord* coords,int coord_size) {
	std::cout.precision(10);

	cout<<"[RGN"<<hex<<object_type<<dec<<"]"<<endl;
	cout<<"Type="<<type<<endl;
	cout<<"SubType="<<sub_type<<endl;
	if( label != NULL )
		cout<<"Label="<<label<<endl;
	cout<<"Data"<<layer<<"=";
	for( int i =0; i <= coord_size; ++i ) {
		cout<<"("<<coords[i].ns<<","<<coords[i].we<<")";
		if( i < coord_size ) cout<<",";
	}
	cout<<endl;
	if( poi_address ) {
		img->readPOI(type,sub_type,poi_address);
	}

	cout<<"[END]"<<endl<<endl;
	return 1;
}

int CALL_API poiData(const char* house_number,const char* street_name,const char* city, const char* region, const char* country,const char* zip,const char* phone) {
	if( strlen(house_number) ) cout<<"HouseNumber="<<house_number<<endl;
	if( strlen(street_name) ) cout<<"StreetDesc="<<street_name<<endl;
	if( strlen(zip) ) cout<<"Zip="<<zip<<endl;
	if( strlen(phone) ) cout<<"Phone="<<phone<<endl;

	if( strlen(city) ) cout<<"City="<<city<<endl;
	if( strlen(region) ) cout<<"Region="<<region<<endl;
	if( strlen(country) ) cout<<"Country="<<country<<endl;

	return 1;
}

int main(int argc, char* argv[])
{
	mapRead_rect bounds;
	mapRead_coord minimum_size;

	img = new IMG(readHeader,readData,readDataI,poiData);

	img->openMap(argv[1]);
	bounds.x0 = 10.18;
	bounds.x1 = 30.21;
	bounds.y0 = 40.186;
	bounds.y1 = 52.16;

	minimum_size.we = 0.0;
	minimum_size.ns = 0.0;
	img->readMap(bounds,minimum_size,0);

	delete img;
	return 0;
}