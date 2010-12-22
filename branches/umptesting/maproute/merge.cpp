/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <algorithm>
#include <assert.h>
#include "merge.h"
#include "utils.h"
#include "Config.h"

using namespace std;

__int64 LineMerge::nextNodeId = 1;
std::map<__int64,__int64> LineMerge::nodIdMapping;

#define MAX_LBL_LENGTH 80

//8,9,0xb,0xc - taki sam typ jak droga o najwyzszym typie z ktora sie styka!

class	sort_coords {
public:
	bool operator () (const Line *x, const Line *y) const	{
	return (x->minX < y->minX );
	}
};

class	sort_order {
public:
	bool operator () (const Line *x, const Line *y) const	{
	if( x->order > y->order )
		return false;
	return true;
	}
};

LineMerge::LineMerge() {
	minBox.x = 180.0;
	minBox.y = 90.0;
	maxBox.x = -180.0;
	maxBox.y = -90.0;
}

void LineMerge::DefineData(
	string input_file_name,
	string routing_dbf_file,
	string restriction_dbf_file,
	string routing_connection_dbf_file,
	MergeParams mergeParams,
	ConfigReader* cf) 
{
	this->cf = cf;
	defaultType = mergeParams.adefaultType;
	highestRgnId = 1;
	mainidxRgnId = -1;
//	HighRoadClass = NULL;
//	LowRoadClass = NULL;
//	NodeIDUsage = NULL;
	input_routing_dbf= NULL;
	input_connection_dbf = NULL;

	if( cf->typeTranslation.size() ) {
		TR.Read(cf->typeTranslation.c_str());
	}

	string shp_file_name = input_file_name + ".shp";
	string dbf_file_name = input_file_name + ".dbf";
	input_shp = SHPOpen(shp_file_name.c_str(),"rb");
	input_dbf = DBFOpen(dbf_file_name.c_str(),"rb");

	if( input_shp == NULL ) {
		cout<<"Could not open shapefile: "<<shp_file_name<<endl;
		exit(1);
	}

	if( input_dbf == NULL ) {
		cout<<"Could not open DBF: "<<dbf_file_name<<endl;
		exit(1);
	}

	//moze byc nadpisane...
	idxLeftCity = DBFGetFieldIndex(input_dbf,mergeParams.leftCity.c_str());
	idxLeftRegion = DBFGetFieldIndex(input_dbf,mergeParams.leftRegion.c_str());
	idxLeftCountry = DBFGetFieldIndex(input_dbf,mergeParams.leftCountry.c_str());
	idxRightCity = DBFGetFieldIndex(input_dbf,mergeParams.rightCity.c_str());
	idxRightRegion = DBFGetFieldIndex(input_dbf,mergeParams.rightRegion.c_str());
	idxRightCountry = DBFGetFieldIndex(input_dbf,mergeParams.rightCountry.c_str());
	idxLeftStart = DBFGetFieldIndex(input_dbf,mergeParams.leftStart.c_str());
	idxLeftEnd = DBFGetFieldIndex(input_dbf,mergeParams.leftEnd.c_str());
	idxLeftType = DBFGetFieldIndex(input_dbf,mergeParams.leftType.c_str());
	idxRightStart = DBFGetFieldIndex(input_dbf,mergeParams.rightStart.c_str());
	idxRightEnd = DBFGetFieldIndex(input_dbf,mergeParams.rightEnd.c_str());
	idxRightType = DBFGetFieldIndex(input_dbf,mergeParams.rightType.c_str());
	idxLeftZip = DBFGetFieldIndex(input_dbf,mergeParams.leftZip.c_str());
	idxRightZip = DBFGetFieldIndex(input_dbf,mergeParams.rightZip.c_str());


	input_restriction_dbf = NULL;
	if( restriction_dbf_file.length() > 0 ) {
		restriction_dbf_file += ".dbf";
		input_restriction_dbf = DBFOpen(restriction_dbf_file.c_str(),"rb");

		if( input_restriction_dbf ) {
			idxNodeId1 = DBFGetFieldIndex(input_restriction_dbf,mergeParams.nodeId1.c_str());
			idxNodeId2 = DBFGetFieldIndex(input_restriction_dbf,mergeParams.nodeId2.c_str());
			idxNodeId3 = DBFGetFieldIndex(input_restriction_dbf,mergeParams.nodeId3.c_str());
			idxNodeId4 = DBFGetFieldIndex(input_restriction_dbf,mergeParams.nodeId4.c_str());
			idxRoadId1 = DBFGetFieldIndex(input_restriction_dbf,mergeParams.roadId1.c_str());
			idxRoadId2 = DBFGetFieldIndex(input_restriction_dbf,mergeParams.roadId2.c_str());
			idxRoadId3 = DBFGetFieldIndex(input_restriction_dbf,mergeParams.roadId3.c_str());
		} else if( restriction_dbf_file.size() ) {
			cout<<"Could not open file: "<<restriction_dbf_file<<endl;
		}
	}

	//assert(1!=1);

	if( routing_dbf_file.length() > 0 ) {
		routing_dbf_file += ".dbf";
		input_routing_dbf = DBFOpen(routing_dbf_file.c_str(),"rb");

		if( input_routing_dbf ) {

			idxNodeId = DBFGetFieldIndex(input_routing_dbf,mergeParams.nodeId.c_str());
			idxRgnId = DBFGetFieldIndex(input_routing_dbf,mergeParams.rgnId.c_str());
			if( idxRgnId==-1 )
				cout<<"Field RoadId ("<< mergeParams.rgnId <<") was not found in the input data - no routing will be imported."<<endl;
			idxNode = DBFGetFieldIndex(input_routing_dbf,mergeParams.node.c_str());
			idxBound = DBFGetFieldIndex(input_routing_dbf,mergeParams.bound.c_str());

			idxLeftCity = DBFGetFieldIndex(input_routing_dbf,mergeParams.leftCity.c_str());
			idxLeftRegion = DBFGetFieldIndex(input_routing_dbf,mergeParams.leftRegion.c_str());
			idxLeftCountry = DBFGetFieldIndex(input_routing_dbf,mergeParams.leftCountry.c_str());
			idxRightCity = DBFGetFieldIndex(input_routing_dbf,mergeParams.rightCity.c_str());
			idxRightRegion = DBFGetFieldIndex(input_routing_dbf,mergeParams.rightRegion.c_str());
			idxRightCountry = DBFGetFieldIndex(input_routing_dbf,mergeParams.rightCountry.c_str());
			idxLeftStart = DBFGetFieldIndex(input_routing_dbf,mergeParams.leftStart.c_str());
			idxLeftEnd = DBFGetFieldIndex(input_routing_dbf,mergeParams.leftEnd.c_str());
			idxLeftType = DBFGetFieldIndex(input_routing_dbf,mergeParams.leftType.c_str());
			idxRightStart = DBFGetFieldIndex(input_routing_dbf,mergeParams.rightStart.c_str());
			idxRightEnd = DBFGetFieldIndex(input_routing_dbf,mergeParams.rightEnd.c_str());
			idxRightType = DBFGetFieldIndex(input_routing_dbf,mergeParams.rightType.c_str());
			idxLeftZip = DBFGetFieldIndex(input_routing_dbf,mergeParams.leftZip.c_str());
			idxRightZip = DBFGetFieldIndex(input_routing_dbf,mergeParams.rightZip.c_str());
		}else if( routing_dbf_file.size()) {
			cout<<"Could not open file: "<<routing_dbf_file<<endl;
		}
	} 

	if( routing_connection_dbf_file.length() > 0 ) {
		routing_connection_dbf_file += ".dbf";
		input_connection_dbf = DBFOpen(routing_connection_dbf_file.c_str(),"rb");

		if( input_connection_dbf ) {
			idxconnRoadId = DBFGetFieldIndex(input_connection_dbf,mergeParams.connRoadId.c_str());
			idxconnNodeId = DBFGetFieldIndex(input_connection_dbf,mergeParams.connNodeId.c_str());
			idxZLevel =     DBFGetFieldIndex(input_connection_dbf,mergeParams.zLevel.c_str());

			idxLeftStart = DBFGetFieldIndex(input_dbf,mergeParams.leftStart.c_str());
			idxLeftEnd = DBFGetFieldIndex(input_dbf,mergeParams.leftEnd.c_str());
			idxLeftType = DBFGetFieldIndex(input_dbf,mergeParams.leftType.c_str());
			idxRightStart = DBFGetFieldIndex(input_dbf,mergeParams.rightStart.c_str());
			idxRightEnd = DBFGetFieldIndex(input_dbf,mergeParams.rightEnd.c_str());
			idxRightType = DBFGetFieldIndex(input_dbf,mergeParams.rightType.c_str());
			idxLeftZip = DBFGetFieldIndex(input_dbf,mergeParams.leftZip.c_str());
			idxRightZip = DBFGetFieldIndex(input_dbf,mergeParams.rightZip.c_str());

		} else if( routing_dbf_file.size()) {
			cout<<"Could not open file: "<<routing_connection_dbf_file<<endl;
		}
	} 


	idxLevel = DBFGetFieldIndex(input_dbf,mergeParams.level.c_str());
	idxEndLevel = DBFGetFieldIndex(input_dbf,mergeParams.endLevel.c_str());
	mainidxRgnId = DBFGetFieldIndex(input_dbf,mergeParams.dataRgnId.c_str());
	idxType = DBFGetFieldIndex(input_dbf,mergeParams.type.c_str());
	idxSpeed = DBFGetFieldIndex(input_dbf,mergeParams.speed.c_str());
	idxRoadClass = DBFGetFieldIndex(input_dbf,mergeParams.roadClass.c_str());
	idxLabel = DBFGetFieldIndex(input_dbf,mergeParams.label.c_str());
	idxLabel3 = DBFGetFieldIndex(input_dbf,mergeParams.label3.c_str());
	idxDescription = DBFGetFieldIndex(input_dbf,mergeParams.description.c_str());
	idxCity = DBFGetFieldIndex(input_dbf,mergeParams.city.c_str());
	idxRegion = DBFGetFieldIndex(input_dbf,mergeParams.region.c_str());
	idxCountry = DBFGetFieldIndex(input_dbf,mergeParams.country.c_str());
    idxZip = DBFGetFieldIndex(input_dbf,mergeParams.zip.c_str());
	idxOneWay = DBFGetFieldIndex(input_dbf,mergeParams.oneWay.c_str());
	idxToll = DBFGetFieldIndex(input_dbf,mergeParams.toll.c_str());
	//idxZLevel = DBFGetFieldIndex(input_dbf,mergeParams.zLevel.c_str());
	idxnonReferenceNode = DBFGetFieldIndex(input_dbf,mergeParams.nonReferenceNode.c_str());
	idxreferenceNode = DBFGetFieldIndex(input_dbf,mergeParams.referenceNode.c_str());
	idxfuncClass = DBFGetFieldIndex(input_dbf,mergeParams.funcClass.c_str());
	idxRestrictionMPC = DBFGetFieldIndex(input_dbf,mergeParams.restrictionMPC.c_str());

	idxStreetVE = DBFGetFieldIndex(input_dbf,mergeParams.StreetVE.c_str());
	idxStreetVD = DBFGetFieldIndex(input_dbf,mergeParams.StreetVD.c_str());
	idxStreetVC = DBFGetFieldIndex(input_dbf,mergeParams.StreetVC.c_str());
	idxStreetVB = DBFGetFieldIndex(input_dbf,mergeParams.StreetVB.c_str());
	idxStreetVT = DBFGetFieldIndex(input_dbf,mergeParams.StreetVT.c_str());
	idxStreetVP = DBFGetFieldIndex(input_dbf,mergeParams.StreetVP.c_str());
	idxStreetVI = DBFGetFieldIndex(input_dbf,mergeParams.StreetVI.c_str());
	idxStreetVR = DBFGetFieldIndex(input_dbf,mergeParams.StreetVR.c_str());
	idxRestrAttributes = DBFGetFieldIndex(input_dbf,mergeParams.RestrAttributes.c_str());

	idxZLevelStart = DBFGetFieldIndex(input_dbf,mergeParams.zLevelStart.c_str());
	idxZLevelEnd = DBFGetFieldIndex(input_dbf,mergeParams.zLevelEnd.c_str());

	nextNodeId = 1;
}

int LineMerge::ReadBool(DBFHandle dbf,int fieldIdx,int record,bool withNegative) {

	if( fieldIdx < 0 )
		return 0;

	string condition = "F";
	if( DBFGetNativeFieldType(dbf,fieldIdx ) == 'L' || DBFGetNativeFieldType(dbf,fieldIdx ) == 'C')
		condition = DBFReadLogicalAttribute(dbf,record,fieldIdx);
	else
		if( DBFReadIntegerAttribute(dbf,record,fieldIdx) > 0 )
			condition ="T";
		else if(withNegative) {
			if( DBFReadIntegerAttribute(dbf,record,fieldIdx) < 0 )
				return -1;
		}
	if( condition == "T" || condition == "t" || condition == "Y" || condition == "y")
		return 1;
	return 0;
}

int LineMerge::ReadAddressType(DBFHandle dbf,int record,int fieldIdx) {
	if( fieldIdx < 0 )
		return 0;

	string condition = "N";
	if( DBFGetNativeFieldType(dbf,fieldIdx ) == 'L' || DBFGetNativeFieldType(dbf,fieldIdx ) == 'C') {
		condition = DBFReadStringAttribute(dbf,record,fieldIdx);
		if( condition == "O" ) return 2;
		if( condition == "B" ) return 3;
		if( condition == "E" ) return 1;
		return 0;
	}

	return DBFReadIntegerAttribute(dbf,record,fieldIdx);
}

bool LineMerge::ReadInput(bool readRouting,bool navteq,bool mapsource) {
	int t_record = 0;
	int	t_vector;
	int iPart;
	int	tMaxPoint;
	string	collectedRestrictions;
	static set<__int64> duplicateCheck;
	static bool duplicateElements = false;

	string	elementType;
	//double distance, direction;
	Coords	min_coord,max_coord;

	string	condition;
	vector<Line*>	temporary_input_data;
	Coords			new_coord,last_coord;
	Line*			line;
	Segment			segment_start;
	Segment			segment_end;
	Segment*		new_segment;
	int				zLevelStart,zLevelEnd;

	bool			changeCoord;
	Coords			a1,a2,b1,b2;

	//numeracja - dla formatu navteq
	int				start_left,end_left,start_right,end_right;

	SHPObject* shp_object;

	
	if( input_restriction_dbf != NULL ) {
		cout<<"Generic routing generation has been set, but as restrictions are imported, original routing will be imported too."<<endl;
		readRouting = true;
	}

	this->baseRoadClass = 4;
	this->baseRoadType = 2;
	highestOrder = 0;

	while( t_record < input_shp->nRecords ) {
	/*
		t_vector = DBFReadIntegerAttribute(input_dbf,t_record,idxType);
		if( t_vector > 5 && t_vector !=8 && t_vector !=9 ) {
			t_record++;
			continue;
		}
	*/
		
		//if( DBFReadIntegerAttribute(input_dbf,t_record,mainidxRgnId) > 20  ) {
/*		if( DBFReadIntegerAttribute(input_dbf,t_record,mainidxRgnId) < 6600 || DBFReadIntegerAttribute(input_dbf,t_record,mainidxRgnId) > 6620 ) {
			t_record++;
			continue;
		}
*/
/*		if( DBFReadIntegerAttribute(input_dbf,t_record,0) != 3017239 && DBFReadIntegerAttribute(input_dbf,t_record,0) != 30545 ) {
			t_record++;
			continue;
		}
*/		

		ShowProgress(input_shp->nRecords, t_record);
		shp_object = SHPReadObject(input_shp,t_record);
		line = NULL;
		temporary_input_data.clear();

		for(t_vector = 0, iPart = 0; t_vector < shp_object->nVertices; t_vector++) {
			if(iPart < shp_object->nParts && shp_object->panPartStart[iPart] == t_vector) {
				if( line ) {
					temporary_input_data.push_back( line );
					line->RecalcMinMax();
				}
				line = new Line();
				last_coord = Coords(-200.0,-200.0);
				//tMaxPoint = shp_object->nVertices;//?

				if( mainidxRgnId>-1) {
					line->rgnId = DBFReadDoubleAttribute(input_dbf,t_record,mainidxRgnId);
					if( line->rgnId < 0 )
						line->rgnId = -1;
					else {
						if( duplicateCheck.find(line->rgnId) != duplicateCheck.end() ) {
							cout<<"Duplicate ROADID / LINK_ID in input data ->"<<line->rgnId<<endl;
							duplicateElements = true;
						} else
							duplicateCheck.insert(line->rgnId);
					}

					if( highestRgnId < line->rgnId )
						highestRgnId = line->rgnId;
					
					if( line->rgnId > -1 )
						rgnIdMapping.push_back( RgnMapping(line->rgnId,line->rgnId) );
					//rgnIdMap[line->rgnId] = line->rgnId;
				} else
					line->rgnId = -1;

				if( idxType>-1) {
					if( DBFGetNativeFieldType(input_dbf,idxType) == 'C' ) {
						elementType = DBFReadStringAttribute(input_dbf,t_record,idxType);
					
						line->type = strtol(elementType.c_str(), NULL, 0);
						if( line->type == 0 )
							line->type = TR.SearchRGNName40(elementType.c_str());
					} else
						line->type = DBFReadIntegerAttribute(input_dbf,t_record,idxType);
				} else line->type = defaultType;

				if( line->type <= 0 ) line->type = defaultType;

				//

				if( idxLabel>-1) line->label = DBFReadStringAttribute(input_dbf,t_record,idxLabel);
				if( idxLabel3>-1) line->label3 = DBFReadStringAttribute(input_dbf,t_record,idxLabel3);
				if( idxDescription>-1) line->description = DBFReadStringAttribute(input_dbf,t_record,idxDescription);
				if( idxCity>-1) line->city = DBFReadStringAttribute(input_dbf,t_record,idxCity);
				if( idxRegion>-1) line->region = DBFReadStringAttribute(input_dbf,t_record,idxRegion);
				if( idxCountry>-1) line->country = DBFReadStringAttribute(input_dbf,t_record,idxCountry);
				if( idxZip>-1) line->zip = DBFReadStringAttribute(input_dbf,t_record,idxZip);
				if( idxSpeed>-1) {
					line->speed = DBFReadIntegerAttribute(input_dbf,t_record,idxSpeed);
					if( mapsource ) {
/*				7 - 128 km/h
				6 - 108 km/h  *
				5 -  93 km/h  *
				4 -  72 km/h  *
				3 -  56 km/h  *
				2 -  40 km/h  *
				1 -  20 km/h
				0 -   8 km/h - ferry
*/
						if( line->speed >= 128 ) line->speed = 7;
						else if( line->speed >= 108 ) line->speed = 6;
						else if( line->speed >= 93 ) line->speed = 5;
						else if( line->speed >= 72 ) line->speed = 4;
						else if( line->speed >= 56 ) line->speed = 3;
						else if( line->speed >= 40 ) line->speed = 2;
						else if( line->speed >= 20 ) line->speed = 1;
						else if( line->speed >= 8 ) line->speed = 0;
					}
				} else line->speed = SetSpeedClass(line->type);

				if( ConfigReader::GENERATE_SPEED )
					line->speed = SetSpeedClass(line->type);

				//else line->speed = -1;

				//if( DBFReadIntegerAttribute(input_dbf,t_record,idxType) < 3 )
				//	line->label = "";

				if( idxLevel>-1 ) line->level = DBFReadIntegerAttribute(input_dbf,t_record,idxLevel);
				if( idxEndLevel>-1 ) line->endLevel = DBFReadIntegerAttribute(input_dbf,t_record,idxEndLevel);

				if( DBFGetNativeFieldType(input_dbf,idxOneWay) == 'C' || DBFGetNativeFieldType(input_dbf,idxOneWay) == 'L' ) {
					string cond = DBFReadLogicalAttribute(input_dbf,t_record,idxOneWay);

					if( atoi(cond.c_str()) )
						line->oneWay = atoi(cond.c_str());
					else {
						if( cond == "T" || cond == "Y" )
							line->oneWay = 1;
						else if( cond == "F" )
							line->oneWay = -1;
						else
							line->oneWay = 0;
					}
				} else		
					line->oneWay = DBFReadIntegerAttribute(input_dbf,t_record,idxOneWay);//Read(input_dbf,idxOneWay,t_record,true);
				line->toll = ReadBool(input_dbf,idxToll,t_record);
				//line->zLevel = DBFReadIntegerAttribute(input_dbf,t_record,idxZLevel);
				if( idxRestrictionMPC > -1 )
					line->restrictionMPC = DBFReadStringAttribute(input_dbf,t_record,idxRestrictionMPC);

				line->StreetVE = ReadBool(input_dbf,idxStreetVE,t_record);
				line->StreetVD = ReadBool(input_dbf,idxStreetVD,t_record);
				line->StreetVC = ReadBool(input_dbf,idxStreetVC,t_record);
				line->StreetVB = ReadBool(input_dbf,idxStreetVB,t_record);
				line->StreetVT = ReadBool(input_dbf,idxStreetVT,t_record);
				line->StreetVP = ReadBool(input_dbf,idxStreetVP,t_record);
				line->StreetVI = ReadBool(input_dbf,idxStreetVI,t_record);
				line->StreetVR = ReadBool(input_dbf,idxStreetVR,t_record);

				zLevelStart = 0;
				zLevelEnd = 0;
				if( idxZLevelStart > -1 && idxZLevelEnd > -1 ) {
					zLevelStart = DBFReadIntegerAttribute(input_dbf,t_record,idxZLevelStart);
					zLevelEnd = DBFReadIntegerAttribute(input_dbf,t_record,idxZLevelEnd);
				}

				if( idxRestrAttributes > -1 ) {
					collectedRestrictions =  DBFReadStringAttribute(input_dbf,t_record,idxRestrAttributes);
					if( collectedRestrictions.size() >= 10 ) {
						if( collectedRestrictions[0] == '1' ) line->StreetVC = true;
						if( collectedRestrictions[1] == '1' ) line->StreetVB = true;
						if( collectedRestrictions[2] == '1' ) line->StreetVT = true;
						//if( collectedRestrictions[3] == '1' ) line->StreetVC = true; //carpool
						if( collectedRestrictions[4] == '1' ) line->StreetVP = true;
						if( collectedRestrictions[5] == '1' ) line->StreetVI = true;
						if( collectedRestrictions[6] == '1' ) line->StreetVR = true;
						//if( collectedRestrictions[7] == '1' ) line->StreetVC = true; // through traffic
						if( collectedRestrictions[8] == '1' ) line->StreetVD = true;
						if( collectedRestrictions[9] == '1' ) line->StreetVE = true;
					}
				}

				//if( line->speed < 0  )
				if( idxRoadClass > -1 )  line->roadClass = DBFReadIntegerAttribute(input_dbf,t_record,idxRoadClass);
				else line->roadClass = SetRoadClass(line->type,!line->city.empty());

				if( mapsource ) {
					line->roadClass--;
				}
				if( line->roadClass > 4 )
					line->roadClass = 4;

				if( navteq ) {
					start_left = 0;
					end_left = 0;
					start_right = 0;
					end_right = 0;

					//reference
					if(idxLeftStart>-1) start_left = DBFReadIntegerAttribute(input_dbf,t_record,idxLeftStart);
					//non reference
					if(idxLeftEnd>-1) end_left = DBFReadIntegerAttribute(input_dbf,t_record,idxLeftEnd);

					if(idxRightStart>-1) start_right = DBFReadIntegerAttribute(input_dbf,t_record,idxRightStart);
					if(idxRightEnd>-1) end_right = DBFReadIntegerAttribute(input_dbf,t_record,idxRightEnd);

					if(idxfuncClass>-1) {
						line->roadClass = DBFReadIntegerAttribute(input_dbf,t_record,idxfuncClass);
						line->roadClass = 5-line->roadClass;
						if( line->roadClass < 0 )
							line->roadClass = 0;
					}

					//SPEED - dla Navteq - negatyw!
					if( idxSpeed>-1) {
						line->speed = DBFReadIntegerAttribute(input_dbf,t_record,idxSpeed);
						//mapowanie - 1 ->7
						//			  8-> 0
						line->speed = 8 - line->speed;
						if( line->speed > 7 ) line->speed = 7;
						if( line->speed < 0 ) line->speed = 0;

/*
(space) NOT APPLICABLE
1 > 130 KPH > 80 MPH
2 101-130 KPH 65-80 MPH
3 91-100 KPH 55-64 MPH
4 71-90 KPH 41-54 MPH
5 51-70 KPH 31-40 MPH
6 31-50 KPH 21-30 MPH
7 11-30 KPH 6-20 MPH
8 < 11 KPH < 6 MPH
*/
					}
					line->nonReferenceNode = DBFReadDoubleAttribute(input_dbf,t_record,idxnonReferenceNode);
					line->referenceNode = DBFReadDoubleAttribute(input_dbf,t_record,idxreferenceNode);
				}

				line->order = highestOrder++;
				iPart++;
			}
			new_coord = Coords(shp_object->padfX[t_vector],shp_object->padfY[t_vector]);

			if( readRouting == false ) {
				if( !(last_coord == new_coord) ) 
					line->points.push_back(new_coord);
				last_coord = new_coord;
			} else
				line->points.push_back(new_coord);
		}
		line->RecalcMinMax();

		min_coord.x = line->minX;
		min_coord.y = line->minY;
		max_coord.x = line->maxX;
		max_coord.y = line->maxY;

		/*ConfigReader::WGS84.calcDistance( min_coord,max_coord, distance, direction);
		if( distance < ConfigReader::CRITICAL_DISTANCE / 4.0 ) {
			cout<<"RoadID: "<<(int)line->rgnId<<" is too short!"<<endl;
			delete line;
		} else {*/
		temporary_input_data.push_back( line );
		if( temporary_input_data.size() > 1 ) {
			sort(temporary_input_data.begin(),temporary_input_data.end(),sort_coords());
			Merge(&temporary_input_data,true);
		}

		if( mapsource ) {
			segment_start.nodeStart = 0;
			segment_start.nodeId = -1;
			segment_start.zLevel = zLevelStart;

			if(idxLeftCity>-1) segment_start.leftCity = DBFReadStringAttribute(input_dbf,t_record,idxLeftCity);
			if(idxLeftRegion>-1) segment_start.leftRegion = DBFReadStringAttribute(input_dbf,t_record,idxLeftRegion);
			if(idxLeftCountry>-1) segment_start.leftCountry = DBFReadStringAttribute(input_dbf,t_record,idxLeftCountry);
			if(idxRightCity>-1) segment_start.rightCity = DBFReadStringAttribute(input_dbf,t_record,idxRightCity);
			if(idxRightRegion>-1) segment_start.rightRegion = DBFReadStringAttribute(input_dbf,t_record,idxRightRegion);
			if(idxRightCountry>-1) segment_start.rightCountry = DBFReadStringAttribute(input_dbf,t_record,idxRightCountry);

			if(idxLeftZip>-1) segment_start.leftZip = DBFReadStringAttribute(input_dbf,t_record,idxLeftZip);
			if(idxRightZip>-1) segment_start.rightZip = DBFReadStringAttribute(input_dbf,t_record,idxRightZip);

			if(idxLeftStart>-1) segment_start.leftStart = DBFReadIntegerAttribute(input_dbf,t_record,idxLeftStart);
			if(idxLeftEnd>-1) segment_start.leftEnd = DBFReadIntegerAttribute(input_dbf,t_record,idxLeftEnd);

			if(idxLeftType>-1) {
				segment_start.leftType = ReadAddressType(input_dbf,t_record,idxLeftType);
				if( segment_start.leftType == 1 )
					segment_start.leftType = 2;
				else if( segment_start.leftType == 2 )
					segment_start.leftType = 1;
			}

			if(idxRightStart>-1) segment_start.rightStart = DBFReadIntegerAttribute(input_dbf,t_record,idxRightStart);
			if(idxRightEnd>-1) segment_start.rightEnd = DBFReadIntegerAttribute(input_dbf,t_record,idxRightEnd);

			if(idxRightType>-1) {
				segment_start.rightType = ReadAddressType(input_dbf,t_record,idxRightType);
				if( segment_start.rightType == 1 )
					segment_start.rightType = 2;
				else if( segment_start.rightType == 2 )
					segment_start.rightType = 1;
			}

			segment_end.nodeStart = line->points.size()-1;
			segment_end.nodeId = -1;
			segment_end.zLevel = zLevelEnd;

			line->segments.push_back(segment_start);
			line->segments.push_back(segment_end);
		}

		if( navteq ) {
			tMaxPoint = line->points.size()-1;
			segment_start.nodeStart=0;
			segment_end.nodeStart=tMaxPoint;
			if( line->points[0].y < line->points[tMaxPoint].y ||
				( line->points[0].y == line->points[tMaxPoint].y &&
				line->points[0].x < line->points[tMaxPoint].x) ) {

					segment_start.nodeId = line->referenceNode;
					segment_end.nodeId = line->nonReferenceNode;

					segment_start.leftStart = start_left; //reference
					segment_start.leftEnd = end_left; //non reference
					segment_start.rightStart = start_right;
					segment_start.rightEnd = end_right;					
			} else {
				segment_end.nodeId = line->referenceNode;
				segment_start.nodeId = line->nonReferenceNode;

				segment_start.leftStart = end_left; //non reference
				segment_start.leftEnd = start_left; //reference
				segment_start.rightStart = end_right;
				segment_start.rightEnd = start_right;
			}

			if( segment_start.leftStart && segment_start.rightStart ) {
				if( segment_start.leftStart % 2 ) {
					segment_start.leftType = 2;//odd
					segment_start.rightType = 1;//even
				} else {
					segment_start.rightType = 2;//odd
					segment_start.leftType = 1;//even
				}
			} else {
				if( segment_start.leftStart ) 
						segment_start.leftType = 3;				
				if( segment_start.rightStart ) 
						segment_start.rightType = 3;				
			}

			line->segments.push_back(segment_start);
			line->segments.push_back(segment_end);
	
			if( nextNodeId < segment_start.nodeId )	nextNodeId = segment_start.nodeId;
			if( nextNodeId < segment_end.nodeId )	nextNodeId = segment_end.nodeId;
		}

		input_data.insert(input_data.end(),temporary_input_data.begin(),temporary_input_data.end());

		SHPDestroyObject( shp_object );
		t_record++;
	}

	maxBox.x = max(maxBox.x,input_shp->adBoundsMax[0]);
	maxBox.y = max(maxBox.y,input_shp->adBoundsMax[1]);

	minBox.x = min(minBox.x,input_shp->adBoundsMin[0]);
	minBox.y = min(minBox.y,input_shp->adBoundsMin[1]);

	minBox = Line::AlignToGrid(minBox);
	maxBox = Line::AlignToGrid(maxBox);

	minBox.x += 0.00007;
	minBox.y += 0.00007;
	maxBox.x -= 0.00007;
	maxBox.y -= 0.00007;

	//read DBF attributes here
	if( input_routing_dbf != NULL && idxRgnId <0 && !mapsource ) {
		cout<<"RoadID value was not read from input file - no routing data will be read!"<<endl;
	}

	if( navteq ) {
		readRouting = false;
		__int64	rgnId;
		__int64	nodeId;
		int				zLevel;
		Line*			li;

		struct t_node_line {
			vector<Line*> lines;
		};

		map<__int64,t_node_line> routable_nodesId;
		map<__int64,t_node_line>::iterator r_element;
		vector<Line*>::iterator r_line;
		std::vector<Segment>::iterator i_segment;
	
		for(vector < Line* >::iterator i = input_data.begin(); i != input_data.end(); i++) {
			if( (*i)->rgnId >= 0  ) {
				i_segment = (*i)->segments.begin();
				while( i_segment != (*i)->segments.end() ) {
					if( (*i_segment).nodeId > -1 ) {
						r_element = routable_nodesId.find((*i_segment).nodeId);
						if( r_element != routable_nodesId.end() ) {
							(*r_element).second.lines.push_back(*i);
						} else {
							t_node_line t;
							t.lines.push_back(*i);
							routable_nodesId[(*i_segment).nodeId] = t;
						}
					}
					i_segment++;
				}
			}
		}

		t_record = 0;
		while( t_record < input_connection_dbf->nRecords ) {
			rgnId = DBFReadDoubleAttribute(input_connection_dbf,t_record,idxconnRoadId);
			nodeId = DBFReadIntegerAttribute(input_connection_dbf,t_record,idxconnNodeId);
				//line->zLevel = DBFReadIntegerAttribute(input_dbf,t_record,idxZLevel);
			if( idxZLevel > -1 )
				zLevel = DBFReadIntegerAttribute(input_connection_dbf,t_record,idxZLevel);
			else
				zLevel = 0;

			r_element = routable_nodesId.find(nodeId); //czy jest znaleziony nodeId
			if( r_element != routable_nodesId.end() ) {
				for( r_line = (*r_element).second.lines.begin(); r_line != (*r_element).second.lines.end(); r_line++ ) {
					li = *r_line;
					i_segment = li->segments.begin();
					while( i_segment != li->segments.end() ) {
						if( (*i_segment).nodeId == nodeId ) {
							(*i_segment).valid_intersection = true;
							(*i_segment).zLevel = zLevel;
						}
						i_segment++;
					}
				}
				//czy jest segment start i end?

			}

			t_record++;
		}

		for(vector < Line* >::iterator i = input_data.begin(); i != input_data.end(); i++) {
			for( i_segment = (*i)->segments.begin(); i_segment != (*i)->segments.end(); i_segment++ ) {
				if( (*i_segment).valid_intersection == false )
					(*i_segment).nodeId = -1;
			}
		}
	}
	
	if( readRouting && input_routing_dbf != NULL && idxRgnId >=0 ) {
		map<unsigned long,Line*> routable_elements;
		map<unsigned long,Line*>::iterator r_element;
		Line* li;
		__int64 rgnId;
		for(vector < Line* >::iterator i = input_data.begin(); i != input_data.end(); i++) {
			if( (*i)->rgnId >= 0  ) {
				routable_elements[(*i)->rgnId] = (*i);
			}
		}

		t_record = 0;
		while( t_record < input_routing_dbf->nRecords ) {
			rgnId = DBFReadDoubleAttribute(input_routing_dbf,t_record,idxRgnId);

			r_element = routable_elements.find(rgnId);
			if( r_element != routable_elements.end() ) {
				li = (*r_element).second;
				//Segment segment;				

				if(idxNode >-1 ) {
					new_segment = li->GetSegment(DBFReadDoubleAttribute(input_routing_dbf,t_record,idxNode),changeCoord,a1,a2,b1,b2);
					//segment.nodeStart = DBFReadIntegerAttribute(input_routing_dbf,t_record,idxNode);
				} else 
					new_segment = li->GetSegment(0,changeCoord,a1,a2,b1,b2);
					//segment.nodeStart = 0;

				//new_segment->nodeId = -1;

				if( readRouting ) {
					if(idxNodeId >-1) {
						new_segment->nodeId = DBFReadIntegerAttribute(input_routing_dbf,t_record,idxNodeId);
						if( nextNodeId < new_segment->nodeId )
							nextNodeId = new_segment->nodeId;
					}
				}
				if(idxBound > -1)
					new_segment->bound = ReadBool(input_routing_dbf,idxBound,t_record);
				//ustawienie bezposrednio w linii atrybutu bound - podczas detekcji 'bound' bedzie mozna uzyc
				if( new_segment->bound ) {
					li->points[new_segment->nodeStart].bound = true;
				}

				if(idxLeftCity>-1) new_segment->leftCity = DBFReadStringAttribute(input_routing_dbf,t_record,idxLeftCity);
				else new_segment->leftCity = li->city;
				if(idxLeftRegion>-1) new_segment->leftRegion = DBFReadStringAttribute(input_routing_dbf,t_record,idxLeftRegion);
				else new_segment->leftRegion = li->region;
				if(idxLeftCountry>-1) new_segment->leftCountry = DBFReadStringAttribute(input_routing_dbf,t_record,idxLeftCountry);
				else new_segment->leftCountry = li->country;
				if(idxRightCity>-1) new_segment->rightCity = DBFReadStringAttribute(input_routing_dbf,t_record,idxRightCity);
				else new_segment->rightCity = li->city;
				if(idxRightRegion>-1) new_segment->rightRegion = DBFReadStringAttribute(input_routing_dbf,t_record,idxRightRegion);
				else new_segment->rightRegion = li->region;
				if(idxRightCountry>-1) new_segment->rightCountry = DBFReadStringAttribute(input_routing_dbf,t_record,idxRightCountry);
				else new_segment->rightCountry = li->country;

				if(idxLeftZip>-1) new_segment->leftZip = DBFReadStringAttribute(input_routing_dbf,t_record,idxLeftZip);
				else new_segment->leftZip = li->zip;
				if(idxRightZip>-1) new_segment->rightZip = DBFReadStringAttribute(input_routing_dbf,t_record,idxRightZip);
				else new_segment->rightZip = li->zip;

				if(idxLeftStart>-1) new_segment->leftStart = DBFReadIntegerAttribute(input_routing_dbf,t_record,idxLeftStart);
				if(idxLeftEnd>-1) new_segment->leftEnd = DBFReadIntegerAttribute(input_routing_dbf,t_record,idxLeftEnd);

				if(idxLeftType>-1) new_segment->leftType = ReadAddressType(input_routing_dbf,t_record,idxLeftType);

				if(idxRightStart>-1) new_segment->rightStart = DBFReadIntegerAttribute(input_routing_dbf,t_record,idxRightStart);
				if(idxRightEnd>-1) new_segment->rightEnd = DBFReadIntegerAttribute(input_routing_dbf,t_record,idxRightEnd);

				if(idxRightType>-1) new_segment->rightType = ReadAddressType(input_routing_dbf,t_record,idxRightType);

				if( new_segment->nodeStart >= (int)li->points.size() ) {
					cout<<"RoadID: "<<(long)li->rgnId<<" segments definition beyond available number of defined points"<<endl;
				} else {
					//li->segments.push_back( segment );
				}
			}
			t_record++;
		}
	}

	if( input_restriction_dbf != NULL ) {
		t_record = 0;
		while( t_record < input_restriction_dbf->nRecords ) {
			Restriction restriction;
			restriction.nodeId1 = DBFReadDoubleAttribute(input_restriction_dbf,t_record,idxNodeId1);
			restriction.nodeId2 = DBFReadDoubleAttribute(input_restriction_dbf,t_record,idxNodeId2);
			restriction.nodeId3 = DBFReadDoubleAttribute(input_restriction_dbf,t_record,idxNodeId3);
			if( idxNodeId4 > -1 )
				restriction.nodeId4 = DBFReadDoubleAttribute(input_restriction_dbf,t_record,idxNodeId4);
			else
				restriction.nodeId4 = 0;

			restriction.RoadId1 = DBFReadDoubleAttribute(input_restriction_dbf,t_record,idxRoadId1);
			restriction.RoadId2 = DBFReadDoubleAttribute(input_restriction_dbf,t_record,idxRoadId2);
			restriction.RoadId3 = DBFReadDoubleAttribute(input_restriction_dbf,t_record,idxRoadId3);

			rgnRestriction.push_back( restriction );
			t_record++;
		}
	}

	if( duplicateElements ) {
		cout<<"Duplicate ROADID / LINK_ID were detected during import process - ROUTING WILL BE NOT CORRECT - YOU HAVE TO CORRECT YOUR DATA"<<endl;
		cout<<"Process will NOT be stop - but output will be wrong for roads with ROADID / LINK_ID listed above."<<endl;
	}

	cout<<"Segment sorting"<<endl;
	for( vector<Line*>::iterator line_j = input_data.begin(); line_j < input_data.end(); line_j++) {
		if( !(*line_j)->segments.empty() ) 
			sort((*line_j)->segments.begin(),(*line_j)->segments.end(),Segment());

		//revert
		if( (*line_j)->oneWay == -1 ) {
			(*line_j)->Revert();
		}
	}

	return true;
}

LineMerge::~LineMerge() {

	/*if( HighRoadClass ) {
		delete []HighRoadClass;
		delete []LowRoadClass;
		delete []NodeIDUsage;
	}*/

	if( input_shp )
		SHPClose(input_shp);
	if( input_dbf )
		DBFClose(input_dbf);
	if( input_routing_dbf )
		DBFClose(input_routing_dbf);

	vector<Line*>::iterator line_i;

	for( line_i = input_data.begin(); line_i < input_data.end(); line_i++) {
		delete (*line_i);
	}
}

void LineMerge::CheckEndingElement(vector<Line*>::iterator line_i) {
	Segment*		segment;
	bool			changeCoord;
	Coords			a1,a2,b1,b2;

	//zmiana - dodanie 'dummy' na poczatku i koncu kazdej drogi
	segment = (*line_i)->GetSegment(0,changeCoord,a1,a2,b1,b2);
	if( segment->nodeId < 0 ) {
		segment->nodeId = nextNodeId;
		nextNodeId++;
	}
	segment = (*line_i)->GetSegment((*line_i)->points.size()-1,changeCoord,a1,a2,b1,b2);
	if( segment->nodeId < 0 ) {
		segment->nodeId = nextNodeId;
		nextNodeId++;
	}

}

void LineMerge::CheckIntersectionElement(vector<Line*>::iterator line_i,bool mapsource) {
	vector<Line*>::iterator line_j;
	unsigned int	i_coord;
	Coords			a1,a2,b1,b2;
	Segment*		segment;
	bool			changeCoord;

	//zmiana - dodanie 'dummy' na poczatku i koncu kazdej drogi
	/*
	//przeniesione do osobnej procedury

	segment = (*line_i)->GetSegment(0,changeCoord,a1,a2,b1,b2);
	if( segment->nodeId < 0 ) {
		segment->nodeId = nextNodeId;
		nextNodeId++;
	}
	segment = (*line_i)->GetSegment((*line_i)->points.size()-1,changeCoord,a1,a2,b1,b2);
	if( segment->nodeId < 0 ) {
		segment->nodeId = nextNodeId;
		nextNodeId++;
	}
	*/

	for( line_j = input_data.begin(); line_j < input_data.end(); line_j++) {

		if( (*line_j)->rgnId == -1 )
			continue;

		if( (*line_i)->rgnId == (*line_j)->rgnId )
			continue;

		if( (*line_j)->minX > (*line_i)->maxX )
			break;

		if(	    (*line_i)->minX <= (*line_j)->maxX
			&&	(*line_i)->maxX >= (*line_j)->minX
			&&	(*line_i)->minY <= (*line_j)->maxY
			&&	(*line_i)->maxY >= (*line_j)->minY) {
			//dla kazdego punktu line_i sprawdzac caly line_j
			for( i_coord = 0; i_coord < (*line_i)->points.size(); i_coord++ ) {
				//cout<<" "<<(*line_j)->rgnId<"  ";
				CheckIntersection(line_i, line_j, i_coord, mapsource);
			}
		}
	}
}

bool LineMerge::ChangeCoords(Coords oldCoords,Coords newCoords) {
	vector<Line*>::iterator line_i;
	vector<Coords>::iterator i_coord_i;

	if( oldCoords == newCoords )
		return false;

	for( line_i = input_data.begin(); line_i < input_data.end(); line_i++) {
		if( (*line_i)->rgnId == -1 )
			continue;

		for( i_coord_i = (*line_i)->points.begin(); i_coord_i < (*line_i)->points.end(); i_coord_i++ ) {
			if( (*i_coord_i) == oldCoords ) {
				(*i_coord_i) = newCoords;
			}
		}
	}

	return true;
}

void LineMerge::CheckIntersection(vector<Line*>::iterator line_i,vector<Line*>::iterator line_j,int i_coord,bool check_zlevels) {
	//sprawdzenie wspolrzednej i_coord z obiektu line_i
	Segment*	segment_i;
	Segment*	segment_j;

	bool	changeCoord;
	bool	equal;
	double	distance,direction;
	Coords	a1,a2,b1,b2;
	__int64	existingNodeId;
	int		existingNode;

	__int64 a = (*line_i)->rgnId;
	__int64 b = (*line_j)->rgnId;
	vector<Coords>::iterator j_coord_i = (*line_j)->points.begin();
	vector<Segment>::iterator i_segment;
	int j_coord = 0;

	for( ; j_coord_i < (*line_j)->points.end(); j_coord_i++, j_coord++ ) {
		equal = false;

		if( ConfigReader::TOLERANCE_METERS < 0.0 )
			equal = ( (*line_i)->points[i_coord] == (*j_coord_i) );
		else {
			ConfigReader::WGS84.calcDistance( (*line_i)->points[i_coord], (*j_coord_i), distance, direction);
			equal = ( distance <= ConfigReader::TOLERANCE_METERS );
			if( equal ) {
				(*line_i)->points[i_coord].x = ( (*line_i)->points[i_coord].x + (*j_coord_i).x ) / 2.0;
				(*line_i)->points[i_coord].y = ( (*line_i)->points[i_coord].y + (*j_coord_i).y ) / 2.0;

				(*j_coord_i) = (*line_i)->points[i_coord];
			}
		}

		if( equal ) {
		
			if( check_zlevels ) {
				segment_i = (*line_i)->GetSegment(i_coord,changeCoord,a1,a2,b1,b2);
				segment_j = (*line_j)->GetSegment(j_coord,changeCoord,a1,a2,b1,b2);

				if( segment_i->zLevel != segment_j->zLevel )
					continue;
			}

			//dodanie i / lub zmodyfikowanie segmentow
			segment_i = (*line_i)->GetSegment(i_coord,changeCoord,a1,a2,b1,b2);
			if( changeCoord ) {
				ChangeCoords(a1,a2);
				ChangeCoords(b1,b2);
			}

			segment_j = (*line_j)->GetSegment(j_coord,changeCoord,a1,a2,b1,b2);
			if( changeCoord ) {
				ChangeCoords(a1,a2);
				ChangeCoords(b1,b2);
			}

			if( segment_j->nodeId < 0 && segment_i->nodeId < 0 ) {
				segment_j->nodeId = segment_i->nodeId = nextNodeId;
				nextNodeId++;
			} else if( segment_j->nodeId < 0 && segment_i->nodeId > -1 ) 
				segment_j->nodeId = segment_i->nodeId;
			else if( segment_i->nodeId < 0 && segment_j->nodeId > -1 )
				segment_i->nodeId = segment_j->nodeId;
			else {
				if( segment_i->nodeId != segment_j->nodeId ) {
					//if( nodIdMapping.find(segment_i->nodeId) != nodIdMapping.end() ) {

					nodIdMapping[segment_i->nodeId] = nextNodeId;
					nodIdMapping[segment_j->nodeId] = nextNodeId;

					segment_j->nodeId = nextNodeId;
					segment_i->nodeId = nextNodeId;
					nextNodeId++;
				}
			}

			//sprawdzenie, czy nie ma powtarzajacych sie nodeId w segmentach...
			existingNodeId = -1;
			existingNode = -1;
			for( i_segment = (*line_i)->segments.begin(); i_segment < (*line_i)->segments.end(); i_segment++ ) {
				if( (*i_segment).nodeId > -1 && (*i_segment).nodeId == existingNodeId ) {
					//powtarza sie nodeId w ulicy!
					i_segment = (*line_i)->segments.erase(i_segment);
				}
				existingNodeId = (*i_segment).nodeId;
			}

			existingNodeId = -1;
			for( i_segment = (*line_j)->segments.begin(); i_segment < (*line_j)->segments.end(); i_segment++ ) {
				if( (*i_segment).nodeId > -1 && (*i_segment).nodeId == existingNodeId ) {
					//powtarza sie nodeId w ulicy!
					i_segment = (*line_j)->segments.erase(i_segment);
				}
				existingNodeId = (*i_segment).nodeId;
			}

		}
	}
}

/*
 Setting future road classes basing on type of the road
*/
void LineMerge::UpdateNodeOfRoad(vector<Line*>::iterator line_i,bool force) {
	vector<Segment>::iterator segment_i;
	if( ((*line_i)->roadClass > -1 && ConfigReader::PRESERVE_ROAD_CLASS == false) || 
		((*line_i)->roadClass < 0 && ConfigReader::PRESERVE_ROAD_CLASS == true) ) {
		for( segment_i = (*line_i)->segments.begin(); segment_i < (*line_i)->segments.end(); segment_i++) {
			if( segment_i->nodeId > -1 ) {
				assert(segment_i->nodeId >= 0);
				//assert(segment_i->nodeId < array_size);

				if( HighRoadClass.find( segment_i->nodeId ) == HighRoadClass.end() )
					HighRoadClass[ segment_i->nodeId ] = -1;

				if( LowRoadClass.find( segment_i->nodeId ) == LowRoadClass.end() )
					LowRoadClass[ segment_i->nodeId ] = -1;

				if( HighRoadClass[segment_i->nodeId] != -1 && LowRoadClass[segment_i->nodeId] != -1 &&
				HighRoadClass[segment_i->nodeId] != (*line_i)->roadClass &&
				LowRoadClass[segment_i->nodeId] != (*line_i)->roadClass && force == false ) {
					if( SetRoadClass((*line_i)->type,!(*line_i)->city.empty()) < 3 ) {
						(*line_i)->roadClass = LowRoadClass[segment_i->nodeId];
						UpdateNodeOfRoad(line_i,true);
					} else {
						(*line_i)->roadClass = HighRoadClass[segment_i->nodeId];
						UpdateNodeOfRoad(line_i,true);
					}
					RCchangedElements++;
				}

				if( HighRoadClass[segment_i->nodeId] == -1 ) {
					HighRoadClass[segment_i->nodeId] = (*line_i)->roadClass;
				} else if( HighRoadClass[segment_i->nodeId] > (*line_i)->roadClass ) {
					LowRoadClass[segment_i->nodeId] = (*line_i)->roadClass;
				} else if( HighRoadClass[segment_i->nodeId] < (*line_i)->roadClass ) {
					LowRoadClass[segment_i->nodeId] = HighRoadClass[segment_i->nodeId];
					HighRoadClass[segment_i->nodeId] = (*line_i)->roadClass;
				}
			}
		}
	}
}


int LineMerge::SetRoadClass(int type,bool in_city) {
	if( cf->r_class4[type] == 1 ) return 4;
	if( cf->r_class3[type] == 1 ) return 3;
	if( cf->r_class2[type] == 1 ) return 2;
	if( cf->r_class1[type] == 1 ) return 1;
	return 0;
}

/*
				7 - 128 km/h
				6 - 108 km/h  *
				5 -  93 km/h  *
				4 -  72 km/h  *
				3 -  56 km/h  *
				2 -  40 km/h  *
				1 -  20 km/h
				0 -   8 km/h - ferry
				* - can change in Ms menu


				Interstate hgw		7 - 1,0xb,8,9
				Major hgw		*	6 - 2
				Other hgw		*	5 - 3
				Collector rd	*	4 - 4
				Residential rd	*	3 - 5,6,0xc
								*	2 - 7
									1 - 0xa

				Ferry				0 - 1a, 1b
				*/
int LineMerge::SetSpeedClass(int type) {
	if( type == 1 || type == 9 || type == 0xb ) return 7;
	if( type == 2 || type == 8) return 6;
	if( type == 3 ) return 5;
	if( type == 4 ) return 4;
	if( type == 5 || type == 6 || type == 0xc ) return 3;
	if( type == 7 ) return 2;
	if( type == 0xa ) return 1;
	
	if( type >= 0xd && type <= 0x13) return 1;

	return 0;
}

__int64 LineMerge::SetSameNodeID(Line* line_1,bool start_1,Line* line_2,bool start_2) {
	Segment*		segment_1;
	Segment*		segment_2;
	bool			changeCoords;
	Coords			a1,a2,b1,b2;
	__int64			t_ret;

	if( line_1 && line_2 ) {
		if( start_1 )
			segment_1 = line_1->GetSegment(0,changeCoords,a1,a2,b1,b2,false,true);
		else
			segment_1 = line_1->GetSegment(line_1->points.size()-1,changeCoords,a1,a2,b1,b2,false,true);

		if( start_2 )
			segment_2 = line_2->GetSegment(0,changeCoords,a1,a2,b1,b2,false,true);
		else
			segment_2 = line_2->GetSegment(line_2->points.size()-1,changeCoords,a1,a2,b1,b2,false,true);

		//check zLevel
		if( segment_1->zLevel != segment_2->zLevel )
			return -1;

		if( segment_1->nodeId < 0 && segment_2->nodeId < 0)
			t_ret = segment_1->nodeId = segment_2->nodeId = nextNodeId++;
		else if( segment_1->nodeId < 0 )
			t_ret = segment_1->nodeId = segment_2->nodeId;
		else if( segment_2->nodeId < 0 )
			t_ret = segment_2->nodeId = segment_1->nodeId;						
		else {
			if( segment_2->nodeId != segment_1->nodeId ) {

				nodIdMapping[segment_1->nodeId] = nextNodeId;
				nodIdMapping[segment_2->nodeId] = nextNodeId;

				segment_1->nodeId = nextNodeId;
				segment_2->nodeId = nextNodeId;
				nextNodeId++;

				//cout<<"Inconsistency in nodID "<<endl;
				t_ret = segment_1->nodeId;
			} else
				t_ret = segment_2->nodeId;
		}
	} else if( line_1 ) {
		if( start_1 )
			segment_1 = line_1->GetSegment(0,changeCoords,a1,a2,b1,b2,false,true);
		else
			segment_1 = line_1->GetSegment(line_1->points.size()-1,changeCoords,a1,a2,b1,b2,false,true);

		if( segment_1->nodeId < 0 )
			t_ret = segment_1->nodeId = nextNodeId++;
		else
			t_ret = segment_1->nodeId;
	}
	return t_ret;
}

void LineMerge::DetectRestrictionMPC() {

	// f1;l2
	// f1,2;l3,4
	vector<Line*>::iterator line_i;
	Line*			line_2;
	Line*			line_3;
	char			vertex;
	char *			pEnd;
	__int64			roadID1,roadID2,roadID3;
	bool			changeCoords;
	Coords			a1,a2,b1,b2;
	Segment*		segment_2;
	Segment*		segment_3;
	Segment*		segment_4;
	Segment*		segment_5;

	map<unsigned long,Line*> routable_elements;
	for(vector < Line* >::iterator i = input_data.begin(); i != input_data.end(); i++) {
		if( (*i)->rgnId >= 0  ) {
			routable_elements[(*i)->rgnId] = (*i);
		}
	}

	for( line_i = input_data.begin(); line_i < input_data.end(); line_i++) {
		if( (*line_i)->restrictionMPC.size() ) {
			roadID1 = (*line_i)->rgnId;
			while (true ) {
				roadID2 = roadID3 = -1;
				cout<<"Restriction: "<<(*line_i)->restrictionMPC<<" "<<(*line_i)->restrictionMPC.size();
				if( !(*line_i)->restrictionMPC.size() )
					break;
				vertex = (*line_i)->restrictionMPC[0];
				(*line_i)->restrictionMPC.erase(0,1);

				roadID2 = strtoul((*line_i)->restrictionMPC.c_str(),&pEnd,0);
				line_2 = (*routable_elements.find(roadID2)).second;
				line_3 = NULL;

				(*line_i)->restrictionMPC.erase(0, pEnd - (*line_i)->restrictionMPC.c_str());
				if( (*line_i)->restrictionMPC.size() ) {
					if( (*line_i)->restrictionMPC[0] == ',' ) {
						(*line_i)->restrictionMPC.erase(0,1);
						roadID3 = strtoul((*line_i)->restrictionMPC.c_str(),&pEnd,0);
						(*line_i)->restrictionMPC.erase(0, pEnd - (*line_i)->restrictionMPC.c_str());
						line_3 = (*routable_elements.find(roadID3)).second;
					}

					if( (*line_i)->restrictionMPC[0] == ';' )
						(*line_i)->restrictionMPC.erase(0,1);
				}

				cout<<"RoadID: "<<roadID1<<","<<roadID2<<","<<roadID3<<endl;

				if( routable_elements.find(roadID2) != routable_elements.end() ) {
					//cout<<roadID1<<","<<roadID2<<endl;
					bool	first_point;
					size_t	last_point = (*line_i)->points.size()-1;

					Restriction restriction;

					restriction.RoadId1 = roadID1;
					restriction.RoadId2 = roadID2;
					restriction.RoadId3 = roadID3;

					if( vertex == 'f' || vertex == 'F' )
						first_point = true; //punkt styku
					else
						first_point = false;
					
					{
						//1 punkt
						restriction.nodeId1 = SetSameNodeID((*line_i),!first_point /*false*/);
						if( restriction.nodeId1 == -1 ) {
							cout<<"Different zLevels"<<endl;
							continue;
						}
	
						if( line_2->points[0] == (*line_i)->points[ first_point? 0 : last_point ] ) {
							//2 punkt
							restriction.nodeId2 = SetSameNodeID((*line_i),first_point,line_2,true);
							if( restriction.nodeId2 == -1 ) {
								cout<<"Different zLevels"<<endl;
								continue;
							}
		
							if( roadID3 > -1 ) {
								if( line_2->points[line_2->points.size()-1] == line_3->points[0] ) {
									//3
									restriction.nodeId3 = SetSameNodeID(line_2,false,line_3,true);									
									if( restriction.nodeId3 == -1 ) {
										cout<<"Different zLevels"<<endl;
										continue;
									}

									//4
									restriction.nodeId4 = SetSameNodeID(line_3,false);
								} else if( line_2->points[line_2->points.size()-1] == line_3->points[line_3->points.size()-1] ) {
									//3
									restriction.nodeId3 = SetSameNodeID(line_2,false,line_3,false);
									if( restriction.nodeId3 == -1 ) {
										cout<<"Different zLevels"<<endl;
										continue;
									}
									//4
									restriction.nodeId4 = SetSameNodeID(line_3,true);
								} else {
									cout<<"Inconsistency"<<endl;
									restriction.nodeId1 = -1;
								}
							} else {
								//3 punkt - koncowy							
								restriction.nodeId3 = SetSameNodeID(line_2,false);
							}
						} else if( line_2->points[line_2->points.size()-1] == (*line_i)->points[first_point? 0 : last_point ] ) {
							//2 punkt
							restriction.nodeId2 = SetSameNodeID((*line_i),first_point,line_2,false);
							if( restriction.nodeId2 == -1 ) {
								cout<<"Different zLevels"<<endl;
								continue;
							}

							if( roadID3 > -1 ) {
								if( line_2->points[0] == line_3->points[0] ) {
									//3
									restriction.nodeId3 = SetSameNodeID(line_2,true,line_3,true);									
									if( restriction.nodeId3 == -1 ) {
										cout<<"Different zLevels"<<endl;
										continue;
									}
									//4
									restriction.nodeId4 = SetSameNodeID(line_3,false);
								} else if( line_2->points[0] == line_3->points[line_3->points.size()-1] ) {
									//3
									restriction.nodeId3 = SetSameNodeID(line_2,true,line_3,false);
									if( restriction.nodeId3 == -1 ) {
										cout<<"Different zLevels"<<endl;
										continue;
									}
									//4
									restriction.nodeId4 = SetSameNodeID(line_3,true);
								} else {
									cout<<"Inconsistency"<<endl;
									restriction.nodeId1 = -1;
								}
							} else {
								//3 punkt - koncowy							
								restriction.nodeId3 = SetSameNodeID(line_2,true);
							}
							
						} else {
							continue;
							cout<<"No intersection at the restriction! Check your data. RoadIDs are: "<<roadID1<<","<<roadID2<<","<<roadID3<<endl;
						}
					}

					//cout<<restriction.nodeId1<<","<<restriction.nodeId2<<","<<restriction.nodeId3<<endl;

					if( restriction.nodeId1 > -1 && restriction.nodeId2 > -1 && restriction.nodeId3 > -1 )
						rgnRestriction.push_back( restriction );
				}
				if( !(*line_i)->restrictionMPC.size() )
					break;
			}
		}
	}
	//
}


void LineMerge::UpdateRestrictionsID() {
	map<__int64,__int64> _mapping;
	map<__int64,__int64>::iterator i_mapping;

	for( vector<RgnMapping>::iterator it = rgnIdMapping.begin(); it != rgnIdMapping.end(); it++) {
		_mapping[(*it).oryginalRgnId] = (*it).newRgnId;
	}

	for( std::vector<Restriction>::iterator i_restr = rgnRestriction.begin(); i_restr != rgnRestriction.end(); i_restr++ ) {
		i_mapping = _mapping.find((*i_restr).RoadId1);
		if( i_mapping != _mapping.end() ) {
			(*i_restr).RoadId1 = (*i_mapping).second;
		}
		i_mapping = _mapping.find((*i_restr).RoadId2);
		if( i_mapping != _mapping.end() ) {
			(*i_restr).RoadId2 = (*i_mapping).second;
		}
		if( (*i_restr).RoadId3 > -1 ) {
			i_mapping = _mapping.find((*i_restr).RoadId3);
			if( i_mapping != _mapping.end() ) {
				(*i_restr).RoadId3 = (*i_mapping).second;
			}
		}

		i_mapping = nodIdMapping.find((*i_restr).nodeId1);
		if( i_mapping != nodIdMapping.end() ) {
			__int64 n_nod = (*i_mapping).second;
			i_mapping = nodIdMapping.find(n_nod);
			while( i_mapping != nodIdMapping.end() ) {
				n_nod = (*i_mapping).second;
				i_mapping = nodIdMapping.find(n_nod);
			}			
			(*i_restr).nodeId1 = n_nod;
		}

		i_mapping = nodIdMapping.find((*i_restr).nodeId2);
		if( i_mapping != nodIdMapping.end() ) {
			__int64 n_nod = (*i_mapping).second;
			i_mapping = nodIdMapping.find(n_nod);
			while( i_mapping != nodIdMapping.end() ) {
				n_nod = (*i_mapping).second;
				i_mapping = nodIdMapping.find(n_nod);
			}			
			(*i_restr).nodeId2 = n_nod;
		}

		i_mapping = nodIdMapping.find((*i_restr).nodeId3);
		if( i_mapping != nodIdMapping.end() ) {
			__int64 n_nod = (*i_mapping).second;
			i_mapping = nodIdMapping.find(n_nod);
			while( i_mapping != nodIdMapping.end() ) {
				n_nod = (*i_mapping).second;
				i_mapping = nodIdMapping.find(n_nod);
			}			
			(*i_restr).nodeId3 = n_nod;
		}

		if( (*i_restr).nodeId4 > -1 ) {
			i_mapping = nodIdMapping.find((*i_restr).nodeId4);
			if( i_mapping != nodIdMapping.end() ) {
				__int64 n_nod = (*i_mapping).second;
				i_mapping = nodIdMapping.find(n_nod);
				while( i_mapping != nodIdMapping.end() ) {
					n_nod = (*i_mapping).second;
					i_mapping = nodIdMapping.find(n_nod);
				}			
				(*i_restr).nodeId4 = n_nod;
			}
		}
	}
//	dla kazdej restrykcji - aktualizacja rgnId z oryginalRgnId na newRgnId
}

/*
void LineMerge::TranslateMPCRestrictions() {
	map<unsigned long,Line*> routable_elements;
	map<unsigned long,Line*>::iterator r_element;
	std::vector<Segment>::iterator i_segment;
	int	nodeId1,nodeId2,nodeId3,nodeId4;
	
	Line* line1;
	Line* line2;
	Line* line3;

	for(vector < Line* >::iterator i = input_data.begin(); i != input_data.end(); i++) {
		if( (*i)->rgnId >= 0  ) {
			routable_elements[(*i)->rgnId] = (*i);
		}
	}

	for( rgnIdMappint_iter = rgnIdMapping.begin(); rgnIdMappint_iter != rgnIdMapping.end(); rgnIdMappint_iter++ ) {
		r_element = routable_elements.find((*rgnIdMappint_iter).newRgnId);
		if( (*rgnIdMappint_iter).newRgnId == (*rgnIdMappint_iter).oryginalRgnId )
			continue;
		if( r_element != routable_elements.end() )
			routable_elements[(*rgnIdMappint_iter).oryginalRgnId ] = (*r_element).second;
		else
			cout<<"Mapping error"<<endl;
	}

	for( std::vector<Restriction>::iterator i_restr = rgnRestriction.begin(); i_restr != rgnRestriction.end(); i_restr++ ) {
		if( (*i_restr).nodeId1 < 0 ) {
			r_element = routable_elements.find((*i_restr).RoadId1);
			if( r_element == routable_elements.end() ) {
				cout<<"Not found RgnID: "<<(*i_restr).RoadId1<<endl;
				continue;
			}
			line1 = (*r_element).second;

			r_element = routable_elements.find((*i_restr).RoadId2);
			if( r_element == routable_elements.end() ) {
				cout<<"Not found RgnID: "<<(*i_restr).RoadId2<<endl;
				continue;
			}
			line2 = (*r_element).second;

			if( (*i_restr).RoadId3 >= 0 ) {
				r_element = routable_elements.find((*i_restr).RoadId3);
				if( r_element == routable_elements.end() ) {
					cout<<"Not found RgnID: "<<(*i_restr).RoadId3<<endl;
					continue;
				}
				line3 = (*r_element).second;
			}

			for( i_segment = line1->segments.begin(); i_segment != line1->segments.end(); i_segment++ ) {
				if( (*i_segment).originalRgnID == (*i_restr).RoadId1 ) {
					if( !(*i_segment).reversed ) {
						if( (*i_restr).firstVertex ) {
							nodeId1 = 
						}
					} else {
					}
				}
			}
		}
	}
}
*/
void LineMerge::SetRoutingClass() {
	//jezeli droga sie styka, ten sam rodzaj i jedna z nich ma -1 to przepisuje klase
	vector<Line*>::iterator line_i;
	vector<Segment>::iterator j_segment_i;
	int		t_type_map[21];
	int		t_poz = 0;
	int		t_pass = 1;
	int		t_all = (int)input_data.size();
	int		i;
//	bool	proposedAccepted;
	for(i = 0; i<21; ++i)
		t_type_map[i] = 0;

/*
	if( LowRoadClass == NULL ) {
		int maxNodeId = 0;
		for( line_i = input_data.begin(); line_i < input_data.end(); line_i++) {
			for( j_segment_i = (*line_i)->segments.begin(); j_segment_i < (*line_i)->segments.end(); j_segment_i++ ) {
				if( maxNodeId < (*j_segment_i).nodeId )
					maxNodeId = (*j_segment_i).nodeId;
			}
		}

		maxNodeId++;
		array_size = maxNodeId+1;
		HighRoadClass = new char[array_size];
		LowRoadClass = new char[array_size];
		NodeIDUsage = new char[array_size];

		for( int i = 0; i < maxNodeId; i++ ) {
			HighRoadClass[i] = -1;
			LowRoadClass[i] = -1;
			NodeIDUsage[i] = 0;
		}
	}*/
	
	for( line_i = input_data.begin(); line_i < input_data.end(); line_i++) {
		if( (*line_i)->type < 21 )
			t_type_map[(*line_i)->type]++;
		for( j_segment_i = (*line_i)->segments.begin(); j_segment_i < (*line_i)->segments.end(); j_segment_i++ ) {
			if( (*j_segment_i).nodeId >= 0 ) {
				assert((*j_segment_i).nodeId >= 0);
				//assert((*j_segment_i).nodeId < array_size);

				if( (*j_segment_i).bound )
					NodeIDUsage[(*j_segment_i).nodeId] = 100;
				else if( (*j_segment_i).nodeStart == 0 || (*j_segment_i).nodeStart == (*line_i)->points.size()-1)
					NodeIDUsage[(*j_segment_i).nodeId] = 50;
				else
					NodeIDUsage[(*j_segment_i).nodeId]++;
			}
		}
	}

	for( vector<Restriction>::iterator ir = rgnRestriction.begin(); ir != rgnRestriction.end(); ir++) {

		if( (*ir).nodeId1 > 0 ) {
			assert((*ir).nodeId1 >= 0);
			//assert((*ir).nodeId1 < array_size);
			NodeIDUsage[(*ir).nodeId1] = 100;
		}
		if( (*ir).nodeId2 > 0 ) {
			assert((*ir).nodeId2 >= 0);
			//assert((*ir).nodeId2 < array_size);
			NodeIDUsage[(*ir).nodeId2] = 100;
		}
		if( (*ir).nodeId3 > 0 ) {
			assert((*ir).nodeId3 >= 0);
			//assert((*ir).nodeId3 < array_size);
			NodeIDUsage[(*ir).nodeId3] = 100;
		}
		if( (*ir).nodeId4 > 0 ) {
			assert((*ir).nodeId4 >= 0);
			//assert((*ir).nodeId4 < array_size);
			NodeIDUsage[(*ir).nodeId4] = 100;
		}
	}

	//show stat
	for( i =1; i < 20; ++i ) {
		if( t_type_map[i] ) {
			cout<<"Type "<<i<<": "<<t_type_map[i]<<" -> "<<(static_cast<float>(t_type_map[i])/static_cast<float>(t_all))*100.0<<endl;
		}
	}

	RCchangedElements = input_data.size();
	int prev_RCchangedElements;
	do {
		prev_RCchangedElements = RCchangedElements;
		RCchangedElements = 0;
		t_poz = 0;

		for( line_i = input_data.begin(); line_i < input_data.end(); line_i++) {
			UpdateNodeOfRoad(line_i);
			ShowProgress(t_all, t_poz++);
		}
		if( RCremainingElements ) {
			cout<<"Next pass..."<<endl;	
		}
	} while( RCchangedElements && RCchangedElements < prev_RCchangedElements );
}

void LineMerge::RemoveEmptySegments() {
	vector<Line*>::iterator line_i;
	vector<Segment>::iterator segment_i;
	vector<Segment>::iterator segment_prev;

	bool	changeCoord;
	bool	reduced = false;
	Coords	a1,a2,b1,b2;
	//int a;

	cout<<"Removing empty segment definitions"<<endl;

	for( line_i = input_data.begin(); line_i < input_data.end(); line_i++) {
		if( (*line_i)->rgnId > -1 ) {			
			for( segment_i = (*line_i)->segments.begin(); segment_i < (*line_i)->segments.end();  ) {
				if( (*segment_i).nodeStart > 0 && (*segment_i).nodeStart < ((*line_i)->points.size()-1) ) {
					if( NodeIDUsage[(*segment_i).nodeId] <= 1 ) {						
						segment_prev = segment_i;
						segment_prev--;
						if( segment_prev >= (*line_i)->segments.begin() ) {
							if( (*segment_prev).leftCity == (*segment_i).leftCity &&
								(*segment_prev).leftRegion == (*segment_i).leftRegion &&
								(*segment_prev).leftCountry == (*segment_i).leftCountry &&
								(*segment_prev).rightCity == (*segment_i).rightCity &&
								(*segment_prev).rightRegion == (*segment_i).rightRegion &&
								(*segment_prev).rightCountry == (*segment_i).rightCountry &&
								(*segment_prev).leftZip == (*segment_i).leftZip &&
								(*segment_prev).rightZip == (*segment_i).rightZip &&
								(*segment_prev).leftStart <=0 && (*segment_i).leftStart <= 0 &&
								(*segment_prev).leftEnd <=0 && (*segment_i).leftEnd <= 0 &&
								(*segment_prev).rightStart <=0 && (*segment_i).rightStart <= 0 &&
								(*segment_prev).rightEnd <=0 && (*segment_i).rightEnd <= 0 ) {
								//usuniecie segmentu
								segment_i = (*line_i)->segments.erase(segment_i);
							} else
								segment_i++;
						} else
							segment_i++;
					} else
						segment_i++;
				} else
						segment_i++;
			}
		}
	}
}

bool LineMerge::CheckNodID(Coords intersectionPoint,__int64 nodId,__int64 roadId) {
	vector<Line*>::iterator line_j;
	vector<Segment>::iterator j_segment_i;
	bool	t_ret = true;

	for( line_j = input_data.begin(); line_j < input_data.end(); line_j++) {

		if( (*line_j)->rgnId == -1 )
			continue;

		if( roadId == (*line_j)->rgnId )
			continue;

		for( j_segment_i = (*line_j)->segments.begin(); j_segment_i < (*line_j)->segments.end(); j_segment_i++ ) {
			if( nodId == (*j_segment_i).nodeId ) {
				if( !(intersectionPoint == (*line_j)->points[(*j_segment_i).nodeStart]) ) {
					cout<<"Error - intersection points have different coordinates"<<endl<<"RoadID1="<<roadId<<" RoadID2="<<(*line_j)->rgnId<<" NodID="<<nodId<<endl;
					cout<<intersectionPoint<<","<<(*line_j)->points[(*j_segment_i).nodeStart]<<endl;
					cout<<"Removing intersection"<<endl<<endl;
					(*j_segment_i).nodeId = -1;
					t_ret = false;
				}
			}
		}
	}
	return t_ret;
}

bool LineMerge::CheckNodID() {
	vector<Line*>::iterator line_i;
	vector<Segment>::iterator j_segment_i;
	bool	t_ret = true;
	for( line_i = input_data.begin(); line_i < input_data.end(); line_i++) {
		if( (*line_i)->rgnId > -1 ) {
			for( j_segment_i = (*line_i)->segments.begin(); j_segment_i < (*line_i)->segments.end(); j_segment_i++ ) {
				if( (*j_segment_i).nodeId >= 0 )
					if( CheckNodID((*line_i)->points[(*j_segment_i).nodeStart],(*j_segment_i).nodeId,(*line_i)->rgnId) == false )
						t_ret = false;
			}
		}
	}
	return t_ret;
}

bool LineMerge::CheckNodes() {
	vector<Line*>::iterator line_i;
	vector<Segment>::iterator j_segment_i;
	bool	changeCoord;
	bool	reduced = false;
	Coords	a1,a2,b1,b2;
	//int a;

	for( line_i = input_data.begin(); line_i < input_data.end(); line_i++) {
		if( (*line_i)->rgnId > -1 ) {
			for( j_segment_i = (*line_i)->segments.begin(); j_segment_i < (*line_i)->segments.end(); j_segment_i++ ) {
				assert((*j_segment_i).nodeStart < (*line_i)->points.size());
				(*line_i)->GetSegment((*j_segment_i).nodeStart,changeCoord,a1,a2,b1,b2);
				if( changeCoord ) {
					reduced = true;
					ChangeCoords(a1,a2);
					ChangeCoords(b1,b2);
					//cout<<a<<":"<<a1.x<<","<<a1.y<<" ";
				}
			}
		}
	}
	return reduced;
}

void LineMerge::InitialSorting() {
	cout<<endl<<"Sorting"<<endl;
	sort(input_data.begin(),input_data.end(),sort_coords());
}

void LineMerge::FinalSorting() {
	cout<<endl<<"Sorting"<<endl;
	sort(input_data.begin(),input_data.end(),sort_order());
}

void LineMerge::SetEndingNodes() {

	//dodanie nodId na koncach drog - jesli slepe

	vector<Line*>::iterator line_i;
	cout<<"Checking proper ending of roads"<<endl;

	int		t_poz = 0;
	int		t_all = (int)input_data.size();

	for( line_i = input_data.begin(); line_i < input_data.end(); line_i++) {
		if( (*line_i)->rgnId == -1 ) 
			continue;
			
		ShowProgress(t_all, t_poz++);
		//cout<<"\r"<<t_poz++<<"  ";
		CheckEndingElement(line_i);
	}

}

void LineMerge::NodDetection(bool mapsource) {

	//wyszukiwanie wszyskich skrzyzowani (dla danych typow oczywiscie)
	//nadawanie RgnId
	//zmiana klasy routingu (i predkosci?) w zaleznosci od typu drogi
	vector<Line*>::iterator line_i;
	//vector<Coords>::iterator coord_i;
	Coords l_coord;

	int		t_poz = 0;
	int		t_all = (int)input_data.size();

	cout<<"Generic routing generation"<<endl;
	nextNodeId++;

	for( line_i = input_data.begin(); line_i < input_data.end(); line_i++) {
		if( (*line_i)->rgnId == -1 ) 
			continue;
			
		ShowProgress(t_all, t_poz++);
		//cout<<"\r"<<t_poz++<<"  ";
		CheckIntersectionElement(line_i,mapsource);
	}

	//poniewa¿ nodeId mogly byc zamienianie - trzeba wykorzystac mapowanie z nodIdMapping zeby zast¹piæ zmapowane nodeId na finalne
	vector<Segment>::iterator	segment_i;
	map<__int64,__int64>::iterator	i_mapping;
	__int64							n_nod;

	for( line_i = input_data.begin(); line_i < input_data.end(); line_i++) {
		if( (*line_i)->rgnId > -1 ) {
			for( segment_i = (*line_i)->segments.begin(); segment_i < (*line_i)->segments.end(); segment_i++ ) {
				i_mapping = nodIdMapping.find((*segment_i).nodeId);
				if( i_mapping != nodIdMapping.end() ) {
					n_nod = (*i_mapping).second;
					i_mapping = nodIdMapping.find(n_nod);
					while( i_mapping != nodIdMapping.end() ) {
						n_nod = (*i_mapping).second;
						i_mapping = nodIdMapping.find(n_nod);
					}
					(*segment_i).nodeId = n_nod;
				}
			}
		}
	}
}

void LineMerge::CreateTables() {
	vector<Line*>::iterator line_i;
	/*vector<Segment>::iterator j_segment_i;

	int maxNodeId = 0;
	for( line_i = input_data.begin(); line_i < input_data.end(); line_i++) {
		for( j_segment_i = (*line_i)->segments.begin(); j_segment_i < (*line_i)->segments.end(); j_segment_i++ ) {
			if( maxNodeId < (*j_segment_i).nodeId )
				maxNodeId = (*j_segment_i).nodeId;
		}
	}

	maxNodeId++;
	array_size = maxNodeId+1;*/

	NodesNumber = nextNodeId+1;
	array_size = NodesNumber;
	//HighRoadClass = new char[array_size];
	//LowRoadClass = new char[array_size];
	//NodeIDUsage = new char[array_size];
/*
	for( int i = 0; i < NodesNumber; i++ ) {
		HighRoadClass[i] = -1;
		LowRoadClass[i] = -1;
		NodeIDUsage[i] = 0;
	}
*/
	for( line_i = input_data.begin(); line_i < input_data.end(); line_i++)
		UpdateNodeOfRoad(line_i);
}

void LineMerge::BoundsDetection() {
	int		t_max;
	__int64	a;
	bool	changeCoord;
	Coords	a1,a2,b1,b2;
	Coords	coordNew;
	Segment* segment;
	cout<<"Generic bounds detection"<<endl;

	nextNodeId++;

	for( vector<Line*>::iterator line_i = input_data.begin(); line_i < input_data.end(); line_i++) {
		if( (*line_i)->rgnId < 0 )
			continue;
		a = (*line_i)->rgnId;

		t_max = (int)(*line_i)->points.size()-1;
		if( 
			(*line_i)->points[0].bound ||
			(*line_i)->points[0].x <= minBox.x ||
			(*line_i)->points[0].y <= minBox.y ||
			(*line_i)->points[0].x >= maxBox.x ||
			(*line_i)->points[0].y >= maxBox.y) 
		{

			/*
			(*line_i)->points[0].x == input_shp->adBoundsMax[0] ||
			(*line_i)->points[0].y == input_shp->adBoundsMax[1] ||
			(*line_i)->points[0].x == input_shp->adBoundsMin[0] ||
			(*line_i)->points[0].y == input_shp->adBoundsMin[1]) {*/

			segment = (*line_i)->GetSegment(0,changeCoord,a1,a2,b1,b2,true);
			segment->bound = 1;
			if( segment->nodeId < 0 )
				segment->nodeId = ++nextNodeId;
			if( changeCoord ) {
				ChangeCoords(a1,a2);
				ChangeCoords(b1,b2);
			}
		}

		if( 
			(*line_i)->points[t_max].bound ||
			(*line_i)->points[t_max].x <= minBox.x ||
			(*line_i)->points[t_max].y <= minBox.y ||
			(*line_i)->points[t_max].x >= maxBox.x ||
			(*line_i)->points[t_max].y >= maxBox.y) 
		{

			segment = (*line_i)->GetSegment(t_max,changeCoord,a1,a2,b1,b2,true);
			segment->bound = 1;
			if( segment->nodeId < 0 )
				segment->nodeId = ++nextNodeId;
			if( changeCoord ) {
				ChangeCoords(a1,a2);
				ChangeCoords(b1,b2);
			}
		}
	}
}

void LineMerge::Split(vector<Line*>::iterator line_i,int operation) {
	int split_point;
	int point_a;
	int point_b;
	vector<Coords>::iterator j_coord_i;
	vector<Coords>::iterator i_coord_i;
	vector<Segment>::iterator segment_i;
	bool	changeCoord;
	Coords	a1,a2,b1,b2;
	__int64	new_nod_id;

	//int a = (*line_i)->rgnId;
	if( (*line_i)->type != 0x16 && ((*line_i)->type > 0x0c && (*line_i)->type < 0x1a) || (*line_i)->type > 0x1b )
		return;

// limit segments to 60

	//a = (*line_i)->segments.size() ;
	if( (*line_i)->segments.size() > 60 ) {
		split_point = (*line_i)->segments[50].nodeStart;

		Line* line = new Line( *(*line_i),split_point );
		line->order = highestOrder++;
		new_nod_id = nextNodeId++;

		segment_i = line->GetSegment(split_point,changeCoord,a1,a2,b1,b2);
		if( segment_i->nodeId >= 0 )
			new_nod_id = segment_i->nodeId;

		segment_i->nodeId = new_nod_id;

		input_data.push_back( line );
		if( (*line_i)->rgnId > -1 ) {
			highestRgnId++;
			line->rgnId = highestRgnId;
			
			if( (*line_i)->rgnId > -1 )
				rgnIdMapping.push_back( RgnMapping((*line_i)->rgnId,line->rgnId) );
		}

		if( split_point >= (int)(*line_i)->points.size() ) {
			input_data.erase(line_i);
			return;
		}

		(*line_i)->points.erase((*line_i)->points.begin(),(*line_i)->points.begin()+split_point);
		(*line_i)->RecalcMinMax();
		//segmenty!
		segment_i = (*line_i)->segments.begin();
		while( segment_i < (*line_i)->segments.end() ) {
			if( (*segment_i).nodeStart < split_point ) 
				segment_i = (*line_i)->segments.erase(segment_i);
			else {
				(*segment_i).nodeStart -= split_point;
				segment_i++;
			}
		}
		segment_i = (*line_i)->GetSegment(0,changeCoord,a1,a2,b1,b2);
		if( segment_i->nodeId < 0 )
			segment_i->nodeId = new_nod_id;

		Split( line_i, operation );
		return;
	}

	Coords last_cord(90,90);

	for( i_coord_i = (*line_i)->points.begin(), point_a = 0; i_coord_i < (*line_i)->points.end(); i_coord_i++, point_a++ ) {
		if( (*i_coord_i) == last_cord ) {
			for( segment_i = (*line_i)->segments.begin(); segment_i < (*line_i)->segments.end(); segment_i++ ) {
				if( (*segment_i).nodeStart >= point_a )
					(*segment_i).nodeStart--;
			}
			i_coord_i = (*line_i)->points.erase( i_coord_i );
		} else
			last_cord = (*i_coord_i);
	}

	for( i_coord_i = (*line_i)->points.begin(), point_a = 0; i_coord_i < (*line_i)->points.end(); i_coord_i++, point_a++ ) {
		for(  point_b = point_a, j_coord_i = i_coord_i; j_coord_i < (*line_i)->points.end(); j_coord_i++, point_b++ ) {
			if( i_coord_i != j_coord_i && (*i_coord_i) == (*j_coord_i)) {

				split_point = point_a +(point_b - point_a)/2;

				Line* line = new Line( *(*line_i),split_point );
				line->order = highestOrder++;
				{
					nextNodeId++;

					Segment new_segment;
					new_segment.nodeStart = split_point;
					new_segment.nodeId = nextNodeId;
					line->segments.push_back( new_segment );
		
				}

				input_data.push_back( line );
				if( (*line_i)->rgnId > -1 ) {
					highestRgnId++;
					line->rgnId = highestRgnId;
				
					if( (*line_i)->rgnId > -1 )
						rgnIdMapping.push_back( RgnMapping((*line_i)->rgnId,line->rgnId) );
				}
				
				if( split_point >= (int)(*line_i)->points.size() ) {
					input_data.erase(line_i);
					return;
				}

				(*line_i)->points.erase((*line_i)->points.begin(),(*line_i)->points.begin()+split_point);
				(*line_i)->RecalcMinMax();
				//segmenty!
				segment_i = (*line_i)->segments.begin();
				while( segment_i < (*line_i)->segments.end() ) {
					if( (*segment_i).nodeStart < split_point ) 
						segment_i = (*line_i)->segments.erase(segment_i);
					else {
						(*segment_i).nodeStart -= split_point;
						segment_i++;
					}
				}
				{
					Segment new_segment;
					new_segment.nodeStart = 0;
					new_segment.nodeId = nextNodeId;
					(*line_i)->segments.insert((*line_i)->segments.begin(),new_segment );
				}

				Split( line_i, operation );

				break;
			}
		}
	}
}

void LineMerge::Merge(vector<Line*>	*local_input_data,vector<Line*>::iterator line_i,bool read_merge) {
	vector<vector<Line*>::iterator> preselected_lines;
	vector<vector<Line*>::iterator> merge_candidates;
	vector<int> merge_candidates_angle;
	vector<vector<Line*>::iterator>::iterator line_selector_i;
	vector<int>::iterator line_selector_a;
	vector<Line*>::iterator line_i2;
	
	vector<Segment>::iterator segment_1;
	vector<Segment>::iterator segment_2;
	Segment*		segment_nod_1;
	Segment*		segment_nod_2;
	bool			changeCoords;
	Coords			a1,a2,b1,b2;
	
	int upper,lower;	//ktore konce maja kandydatow 

	upper = 0;
	lower = 0;
	if( !read_merge )
		(*line_i)->merged = true;

	for( line_i2 = (*local_input_data).begin(); line_i2 < (*local_input_data).end(); line_i2++) {

		if( (*line_i2) == (*line_i) )
			continue;

		if( (*line_i2)->minX > (*line_i)->maxX )
			break;

		if( (*line_i2)->merged == false && 
			(*line_i2)->minX <= (*line_i)->maxX &&
			(*line_i2)->maxX >= (*line_i)->minX &&
			(*line_i2)->minY <= (*line_i)->maxY &&
			(*line_i2)->maxY >= (*line_i)->minY &&
			(*line_i)->segments.size() +  (*line_i2)->segments.size() < 60)
		{

				if( (*line_i)->points[0] == (*line_i2)->points[(*line_i2)->points.size()-1] 
					||
					(*line_i)->points[0] == (*line_i2)->points[0] ) {
						if(
						(*line_i2)->StreetVE == (*line_i)->StreetVE &&
						(*line_i2)->StreetVD == (*line_i)->StreetVD &&
						(*line_i2)->StreetVC == (*line_i)->StreetVC &&
						(*line_i2)->StreetVB == (*line_i)->StreetVB &&
						(*line_i2)->StreetVT == (*line_i)->StreetVT &&
						(*line_i2)->StreetVP == (*line_i)->StreetVP &&
						(*line_i2)->StreetVI == (*line_i)->StreetVI &&
						(*line_i2)->StreetVR == (*line_i)->StreetVE &&
						(*line_i2)->type == (*line_i)->type &&
						(*line_i2)->label == (*line_i)->label &&
						(*line_i2)->label3 == (*line_i)->label3 &&
						(*line_i2)->country == (*line_i)->country &&
						(*line_i2)->region == (*line_i)->region &&
						(*line_i2)->city == (*line_i)->city &&
						(*line_i2)->description == (*line_i)->description &&
						//(*line_i2)->oneWay == (*line_i)->oneWay &&
						(*line_i2)->toll == (*line_i)->toll &&
						//(*line_i2)->zLevel == (*line_i)->zLevel &&
/*
						(
							( (*line_i2)->zLevelStart == (*line_i)->zLevelStart && (*line_i)->points[0] == (*line_i2)->points[0] ) ||
							( (*line_i2)->zLevelEnd == (*line_i)->zLevelStart && (*line_i)->points[0] == (*line_i2)->points[(*line_i2)->points.size()-1])

						) &&*/
						(*line_i2)->speed == (*line_i)->speed &&
						(*line_i2)->roadClass == (*line_i)->roadClass ) {
							preselected_lines.push_back( line_i2 );
							lower++;
						}
					}

				if( (*line_i2)->points[0] == (*line_i)->points[(*line_i)->points.size()-1] 
					||					
					(*line_i)->points[(*line_i)->points.size()-1] == (*line_i2)->points[(*line_i2)->points.size()-1] ) {
						if(
						(*line_i2)->StreetVE == (*line_i)->StreetVE &&
						(*line_i2)->StreetVD == (*line_i)->StreetVD &&
						(*line_i2)->StreetVC == (*line_i)->StreetVC &&
						(*line_i2)->StreetVB == (*line_i)->StreetVB &&
						(*line_i2)->StreetVT == (*line_i)->StreetVT &&
						(*line_i2)->StreetVP == (*line_i)->StreetVP &&
						(*line_i2)->StreetVI == (*line_i)->StreetVI &&
						(*line_i2)->StreetVR == (*line_i)->StreetVE &&
						(*line_i2)->type == (*line_i)->type &&
						(*line_i2)->label == (*line_i)->label &&
						(*line_i2)->label3 == (*line_i)->label3 &&
						(*line_i2)->country == (*line_i)->country &&
						(*line_i2)->region == (*line_i)->region &&
						(*line_i2)->city == (*line_i)->city &&
						(*line_i2)->description == (*line_i)->description &&
						//(*line_i2)->oneWay == (*line_i)->oneWay &&
						(*line_i2)->toll == (*line_i)->toll &&
						//(*line_i2)->zLevel == (*line_i)->zLevel &&
/*
						(
							( (*line_i2)->zLevelStart == (*line_i)->zLevelEnd && (*line_i2)->points[0] == (*line_i)->points[(*line_i)->points.size()-1]) ||
							( (*line_i2)->zLevelEnd == (*line_i)->zLevelEnd && (*line_i)->points[(*line_i)->points.size()-1] == (*line_i2)->points[(*line_i2)->points.size()-1] )

						) &&
*/
						(*line_i2)->speed == (*line_i)->speed &&
						(*line_i2)->roadClass == (*line_i)->roadClass ) {
							preselected_lines.push_back( line_i2 );
							upper++;
						}
					}
		}
	}


	//czy jest ju¿ routing w wybranych drogach...
	if( !preselected_lines.empty() && (*line_i)->segments.size() ) {
		for( line_selector_i = preselected_lines.begin(); line_selector_i < preselected_lines.end(); /*line_selector_i++*/ )	{
			line_i2 = *line_selector_i;
		
			if( (*line_i2)->segments.size() ) {
				if( (*line_i)->points[0] == (*line_i2)->points[0] ) {
					segment_nod_1 = (*line_i)->GetSegment(0,changeCoords,a1,a2,b1,b2,false,true);
					segment_nod_2 = (*line_i2)->GetSegment(0,changeCoords,a1,a2,b1,b2,false,true);

					if( segment_nod_1->zLevel != segment_nod_2->zLevel || segment_nod_1->nodeId != segment_nod_2->nodeId ) {
						//nie mo¿na robiæ merge!
						line_selector_i = preselected_lines.erase(line_selector_i);
						continue;
					} else
						line_selector_i++;
				}

				if( (*line_i)->points[0] == (*line_i2)->points[(*line_i2)->points.size()-1] ) {
					segment_nod_1 = (*line_i)->GetSegment(0,changeCoords,a1,a2,b1,b2,false,true);
					segment_nod_2 = (*line_i2)->GetSegment((*line_i2)->points.size()-1,changeCoords,a1,a2,b1,b2,false,true);

					if( segment_nod_1->zLevel != segment_nod_2->zLevel || segment_nod_1->nodeId != segment_nod_2->nodeId ) {
						//nie mo¿na robiæ merge!
						line_selector_i = preselected_lines.erase(line_selector_i);
						continue;
					} else
						line_selector_i++;
				}

				if( (*line_i)->points[(*line_i)->points.size()-1] == (*line_i2)->points[(*line_i2)->points.size()-1] ) {
					segment_nod_1 = (*line_i)->GetSegment((*line_i)->points.size()-1,changeCoords,a1,a2,b1,b2,false,true);
					segment_nod_2 = (*line_i2)->GetSegment((*line_i2)->points.size()-1,changeCoords,a1,a2,b1,b2,false,true);

					if( segment_nod_1->zLevel != segment_nod_2->zLevel || segment_nod_1->nodeId != segment_nod_2->nodeId ) {
						//nie mo¿na robiæ merge!
						line_selector_i = preselected_lines.erase(line_selector_i);
						continue;
					} else
						line_selector_i++;
				}

				if( (*line_i)->points[(*line_i)->points.size()-1] == (*line_i2)->points[0] ) {
					segment_nod_1 = (*line_i)->GetSegment((*line_i)->points.size()-1,changeCoords,a1,a2,b1,b2,false,true);
					segment_nod_2 = (*line_i2)->GetSegment(0,changeCoords,a1,a2,b1,b2,false,true);

					if( segment_nod_1->zLevel != segment_nod_2->zLevel || segment_nod_1->nodeId != segment_nod_2->nodeId ) {
						//nie mo¿na robiæ merge!
						line_selector_i = preselected_lines.erase(line_selector_i);
						continue;
					} else
						line_selector_i++;
				}
			} else
				line_selector_i++;
		}
	}


	if( !preselected_lines.empty() ) {
		for( line_selector_i = preselected_lines.begin(); line_selector_i < preselected_lines.end(); line_selector_i++ )	{
			line_i2 = *line_selector_i;

			//nazwa, typ drogi, predkosc, routeClass, oneWay
			if( (*line_i)->points[0] == (*line_i2)->points[(*line_i2)->points.size()-1] ||
				(*line_i2)->points[0] == (*line_i)->points[(*line_i)->points.size()-1] ) {
					if( (*line_i2)->oneWay == (*line_i)->oneWay ) {

							//sprawdzenie kierunku!
							if( (*line_i)->points[0] == (*line_i2)->points[(*line_i2)->points.size()-1] ) {
								if( lower == 1 ) {
									merge_candidates_angle.push_back(Line::GetDirectionDifferenceAngle( (*line_i)->points[0],(*line_i)->points[1],(*line_i2)->points[(*line_i2)->points.size()-2],(*line_i2)->points[(*line_i2)->points.size()-1]));
									merge_candidates.push_back( line_i2 );
								} else if( Line::GetDirectionDifference( (*line_i)->points[0],(*line_i)->points[1],(*line_i2)->points[(*line_i2)->points.size()-2],(*line_i2)->points[(*line_i2)->points.size()-1]) ) {
									//atrybuty zgodne - 
									merge_candidates_angle.push_back(Line::GetDirectionDifferenceAngle( (*line_i)->points[0],(*line_i)->points[1],(*line_i2)->points[(*line_i2)->points.size()-2],(*line_i2)->points[(*line_i2)->points.size()-1]));
									merge_candidates.push_back( line_i2 );
								}
							} else {
								if( upper == 1 ) {
									merge_candidates_angle.push_back(Line::GetDirectionDifferenceAngle( (*line_i2)->points[0],(*line_i2)->points[1],(*line_i)->points[(*line_i)->points.size()-2],(*line_i)->points[(*line_i)->points.size()-1]));
									merge_candidates.push_back( line_i2 );
								} else if( Line::GetDirectionDifference( (*line_i2)->points[0],(*line_i2)->points[1],(*line_i)->points[(*line_i)->points.size()-2],(*line_i)->points[(*line_i)->points.size()-1]) ) {
									//atrybuty zgodne - 
									merge_candidates_angle.push_back(Line::GetDirectionDifferenceAngle( (*line_i2)->points[0],(*line_i2)->points[1],(*line_i)->points[(*line_i)->points.size()-2],(*line_i)->points[(*line_i)->points.size()-1]));
									merge_candidates.push_back( line_i2 );
								}
							}
					}
			} else if( (*line_i)->points[0] == (*line_i2)->points[0] && 
				(*line_i)->oneWay == 0 && (*line_i2)->oneWay == 0) {
				/******************************/				
				/*revert*/
					{
						//sprawdzenie kierunku!
						if( lower == 1 ) {
								//tylko jeden
							merge_candidates_angle.push_back(Line::GetDirectionDifferenceAngle( (*line_i)->points[1],(*line_i)->points[0],(*line_i2)->points[0],(*line_i2)->points[1]));
							(*line_i2)->Revert();
							merge_candidates.push_back( line_i2 );
						} else if( Line::GetDirectionDifference( (*line_i)->points[1],(*line_i)->points[0],(*line_i2)->points[0],(*line_i2)->points[1]) ) {
							//odwracane jest line_i2
							merge_candidates_angle.push_back(Line::GetDirectionDifferenceAngle( (*line_i)->points[1],(*line_i)->points[0],(*line_i2)->points[0],(*line_i2)->points[1]));
							(*line_i2)->Revert();
							merge_candidates.push_back( line_i2 );
						}
					}
			} else if( (*line_i)->points[(*line_i)->points.size()-1] == (*line_i2)->points[(*line_i2)->points.size()-1] && 
					(*line_i)->oneWay == 0 && (*line_i2)->oneWay == 0) {
						{
							if( upper == 1 ) {
								merge_candidates_angle.push_back(Line::GetDirectionDifferenceAngle( (*line_i)->points[(*line_i)->points.size()-2],(*line_i)->points[(*line_i)->points.size()-1],(*line_i2)->points[(*line_i2)->points.size()-1],(*line_i2)->points[(*line_i2)->points.size()-2]));
								(*line_i2)->Revert();
								merge_candidates.push_back( line_i2 );
							} else if( Line::GetDirectionDifference( (*line_i)->points[(*line_i)->points.size()-2],(*line_i)->points[(*line_i)->points.size()-1],(*line_i2)->points[(*line_i2)->points.size()-1],(*line_i2)->points[(*line_i2)->points.size()-2]) ) {
								//odwracane jest line_i2
								merge_candidates_angle.push_back(Line::GetDirectionDifferenceAngle( (*line_i)->points[(*line_i)->points.size()-2],(*line_i)->points[(*line_i)->points.size()-1],(*line_i2)->points[(*line_i2)->points.size()-1],(*line_i2)->points[(*line_i2)->points.size()-2]));
								(*line_i2)->Revert();
								merge_candidates.push_back( line_i2 );
							}
						}
			}
					/******************************/
		}
	}


	if( !merge_candidates.empty() ) {
		int	min_angle = 99999;
		line_i2 = NULL;
		
		for( line_selector_a = merge_candidates_angle.begin(), line_selector_i = merge_candidates.begin(); line_selector_i < merge_candidates.end(); line_selector_i++,line_selector_a++ ) {
			if( min_angle > *line_selector_a ) {
				min_angle = *line_selector_a;
				line_i2 = *line_selector_i;
			}
		}
//		if( (*line_i)->length >= max_length ) {
			if( (*line_i)->Merge( *line_i2 ) ) {
				AddMapping((*line_i2)->rgnId,(*line_i)->rgnId);

				//dodac info o oryginalnym rgnId do segmentu...?

				if( (*line_i)->segments.size() >= 60 ) {
					cout<<"Too many segments: "<<(*line_i2)->rgnId<<","<<(*line_i)->rgnId<<endl;
				}

				delete( (*line_i2) );
				(*local_input_data).erase( line_i2 );
				repeatMerge = true;
				Merge( local_input_data, line_i, read_merge );
			}
//		} else {
//			(*line_i)->merged = false;
//			Merge( local_input_data, line_i2, read_merge );
//		}
	}
}

__int64 LineMerge::GetNewRoadID(__int64 originalId) {
	for( rgnIdMappint_iter = rgnIdMapping.begin(); rgnIdMappint_iter != rgnIdMapping.end(); rgnIdMappint_iter++ ) {
		if( (*rgnIdMappint_iter).oryginalRgnId == originalId )
			return (*rgnIdMappint_iter).newRgnId;
	}
	return 0;
}

void LineMerge::AddMapping(__int64 originalId,__int64 newId) {
	//sprawdzam czy mapowanie 'lancuchowe'
	//do zmiany - zbyt wolne....
	//stara
	bool exist = false;
	for( rgnIdMappint_iter = rgnIdMapping.begin(); rgnIdMappint_iter != rgnIdMapping.end(); rgnIdMappint_iter++ ) {
		if( (*rgnIdMappint_iter).newRgnId == originalId ) {
			(*rgnIdMappint_iter).newRgnId = newId;
		}
		if( (*rgnIdMappint_iter).oryginalRgnId == originalId && (*rgnIdMappint_iter).newRgnId == newId )
			exist = true;
	}
	if( exist )
		return;
	if( originalId > -1 )
		rgnIdMapping.push_back( RgnMapping(originalId,newId) );
}

void LineMerge::Split(int operation) {

	input_data.reserve( input_data.size() * 2 );

	vector<Line*>::iterator line_i = input_data.begin();
	cout<<"Initial number of lines: "<<(int)input_data.size()<<endl;
	cout<<"Checking self-intersection"<<endl;
	while( line_i != input_data.end() ) {
		Split(line_i,operation);
		cout<<"\r"<<(int)input_data.size()<<"  ";
		line_i++;
	}
	//cout<<endl;
}

bool LineMerge::Merge(vector<Line*>	*local_input_data,bool read_merge) {
	vector<Line*>::iterator line_i;
	int t_all;
	int t_poz;

	if( read_merge == false )
		cout<<"Merging lines"<<endl;

	do { 
		repeatMerge = false;
		t_all = (int)(*local_input_data).size();
		t_poz = 0;
		line_i = (*local_input_data).begin();
		while( line_i < (*local_input_data).end() ) {
			if( (*line_i)->merged == false /*&&
				(*line_i)->type != 8 &&
				(*line_i)->type != 9*/ ) {

				if( (*line_i)->segments.size() < 60 )
					Merge(local_input_data, line_i, read_merge);
				//if( read_merge == false )
				//cout<<"\r"<<(int)(*local_input_data).size()<<"  ";
			}
			ShowProgress(t_all, t_poz++);
			t_all = (int)(*local_input_data).size();
			line_i++;
		}
		if( repeatMerge ) { 
			InitialSorting();
			for( line_i = (*local_input_data).begin(); line_i < (*local_input_data).end(); line_i++ )
				(*line_i)->merged = false;
			cout<<"Merging..."<<endl;
		}
	} while( repeatMerge );
	//cout<<"Number of lines after reduction: "<<(int)local_input_data.size()<<endl;
	cout<<"\rNumber of elements after merge: "<<(int)(*local_input_data).size()<<endl;
	return true;
}


void LineMerge::SmoothInput() {
	vector<Line*>::iterator line_i;
	cout<<"Generalising lines"<<endl;
	for( line_i = input_data.begin(); line_i < input_data.end(); line_i++) {
		(*line_i)->Smooth();
	}
}

void LineMerge::RemoveZeroLength() {
	cout<<"Removing 0-length lines"<<endl;
	vector<Line*>::iterator line_i;
	for( line_i = input_data.begin(); line_i < input_data.end(); line_i++) {
		(*line_i)->RecalcMinMax();
		if( (*line_i)->length == 0 ) {
			delete( (*line_i) );
			input_data.erase( line_i );
		}
	}
}

void LineMerge::Enumerate() {
	vector<Line*>::iterator line_i = input_data.begin();
	__int64 rgnId = highestRgnId;
	//int	order = 0;
	cout<<"Initial enumeration"<<endl;
	for( line_i = input_data.begin(); line_i < input_data.end(); line_i++) {
		//(*line_i)->order = order++;
		if( (*line_i)->type <= 0x10 || (*line_i)->type == 0x16 || (*line_i)->type == 0x1a || (*line_i)->type == 0x1b ) {
			rgnId++;
			if( (*line_i)->rgnId <= 0 ) {
				(*line_i)->rgnId = rgnId;
				highestRgnId = rgnId;
			}
		} else
			(*line_i)->rgnId = -1;
	}

	//cout<<"Routable roads: "<<rgnId-1<<endl<<"Sorting"<<endl;
}

__int64 LineMerge::GetRoadID(__int64 realNodID) {
	static map<__int64,__int64> rgn_mapping;
	static int new_id = 1;
	
	if( ConfigReader::ENUMERATE == false )
		return realNodID;

	if( realNodID < 0 )
		return realNodID;

	if( rgn_mapping.find( realNodID ) != rgn_mapping.end() )
		return rgn_mapping[realNodID];
	
	rgn_mapping[realNodID] = new_id;
	new_id++;

	return rgn_mapping[realNodID];
}

__int64 LineMerge::GetNodID(__int64 realNodID) {
	static map<__int64,__int64> mapping;
	static int new_id = 1;
	
	if( ConfigReader::ENUMERATE == false )
		return realNodID;

	if( realNodID < 0 )
		return realNodID;

	if( mapping.find( realNodID ) != mapping.end() )
		return mapping[realNodID];
	
	mapping[realNodID] = new_id;
	new_id++;

	return mapping[realNodID];
}


void LineMerge::ExportSegments() {
	int			element = 0;
	int			nodIdRemoved = 0;
	DBFHandle	dbf_handle;
	SHPHandle	shp_handle;
	SHPObject*	shp_object;
	long		tmp_node_id;
	double		x[2];
	double		y[2];

	dbf_handle = DBFCreate("processed_segments");
	shp_handle = SHPCreate("processed_segments", SHPT_POINT);
	
	DBFAddField(dbf_handle, "roadid", FTInteger, 9, 0);
	DBFAddField(dbf_handle, "node", FTInteger, 6, 0);
	DBFAddField(dbf_handle, "nodeid", FTInteger, 9, 0);
	DBFAddField(dbf_handle, "bound", FTInteger, 1, 0);

	DBFAddField(dbf_handle, "ltype", FTInteger, 2, 0);
	DBFAddField(dbf_handle, "lstart", FTInteger, 6, 0);
	DBFAddField(dbf_handle, "lend", FTInteger, 6, 0);

	DBFAddField(dbf_handle, "rtype", FTInteger, 2, 0);
	DBFAddField(dbf_handle, "rstart", FTInteger, 6, 0);
	DBFAddField(dbf_handle, "rend", FTInteger, 6, 0);

	DBFAddField(dbf_handle, "lcity", FTString, 30, 0);
	DBFAddField(dbf_handle, "rcity", FTString, 30, 0);
	DBFAddField(dbf_handle, "lregion", FTString, 30, 0);
	DBFAddField(dbf_handle, "rregion", FTString, 30, 0);
	DBFAddField(dbf_handle, "lcountry", FTString, 30, 0);
	DBFAddField(dbf_handle, "rcountry", FTString, 30, 0);

	DBFAddField(dbf_handle, "lzip", FTString, 15, 0);
	DBFAddField(dbf_handle, "rzip", FTString, 15, 0);

	for( vector<Line*>::iterator it = input_data.begin(); it != input_data.end(); it++) {
		for( vector<Segment>::iterator segment_i = (*it)->segments.begin(); segment_i != (*it)->segments.end(); segment_i++ ) {
			if( (*segment_i).leftStart == 0 && (*segment_i).leftEnd == 0 )
				(*segment_i).leftType = 0;
			if( (*segment_i).rightStart == 0 && (*segment_i).rightEnd == 0 )
				(*segment_i).rightType = 0;
			DBFWriteIntegerAttribute (dbf_handle,element,0,(int)GetRoadID((*it)->rgnId));
			DBFWriteIntegerAttribute (dbf_handle,element,1,(int)(*segment_i).nodeStart);

			if( (*segment_i).nodeId >= 0 ) {
				assert((*segment_i).nodeId >= 0);
				//assert((*segment_i).nodeId < array_size);
/*				tmp_node_id = (*segment_i).nodeId;
				if( ConfigReader::ENUMERATE ) {
					tmp_node_id = NodeIDUsage.begin() - NodeIDUsage.find((*segment_i).nodeId) + 1;
				}*/
				if( NodeIDUsage[(*segment_i).nodeId] > 1 ) {
					//DBFWriteIntegerAttribute (dbf_handle,element,2,tmp_node_id);
					DBFWriteIntegerAttribute (dbf_handle,element,2,(int)GetNodID((*segment_i).nodeId));
				} else {
					nodIdRemoved++;
					//cout<<"Removing dummy routing node :"<<(int)(*segment_i).nodeId<<endl;
					DBFWriteIntegerAttribute (dbf_handle,element,2,-1);
				}
			} else
				DBFWriteIntegerAttribute (dbf_handle,element,2,-1);
			DBFWriteIntegerAttribute (dbf_handle,element,3,(*segment_i).bound);
			DBFWriteIntegerAttribute (dbf_handle,element,4,(*segment_i).leftType);
			DBFWriteIntegerAttribute (dbf_handle,element,5,(*segment_i).leftStart);
			DBFWriteIntegerAttribute (dbf_handle,element,6,(*segment_i).leftEnd);
			DBFWriteIntegerAttribute (dbf_handle,element,7,(*segment_i).rightType);
			DBFWriteIntegerAttribute (dbf_handle,element,8,(*segment_i).rightStart);
			DBFWriteIntegerAttribute (dbf_handle,element,9,(*segment_i).rightEnd);

			DBFWriteStringAttribute (dbf_handle,element,10,(*segment_i).leftCity.c_str());
			DBFWriteStringAttribute (dbf_handle,element,11,(*segment_i).rightCity.c_str());
			DBFWriteStringAttribute (dbf_handle,element,12,(*segment_i).leftRegion.c_str());
			DBFWriteStringAttribute (dbf_handle,element,13,(*segment_i).rightRegion.c_str());
			DBFWriteStringAttribute (dbf_handle,element,14,(*segment_i).leftCountry.c_str());
			DBFWriteStringAttribute  (dbf_handle,element,15,(*segment_i).rightCountry.c_str());

			DBFWriteStringAttribute (dbf_handle,element,16,(*segment_i).leftZip.c_str());
			DBFWriteStringAttribute  (dbf_handle,element,17,(*segment_i).rightZip.c_str());

			if( (*segment_i).nodeId >= 0 ) {
				if( node_data.find((*segment_i).nodeId) == node_data.end() ) {
					node_data[(*segment_i).nodeId] = RoutingNode((*it)->points[(*segment_i).nodeStart],(*segment_i).nodeId);
					if( (*segment_i).bound )
						node_data[(*segment_i).nodeId].bound = 1;
				}
			}

			x[0] = (*it)->points[(*segment_i).nodeStart].x;
			y[0] = (*it)->points[(*segment_i).nodeStart].y;

			shp_object = SHPCreateSimpleObject( SHPT_POINT, 1, x, y, NULL );
			SHPComputeExtents( shp_object );
			SHPWriteObject( shp_handle, -1, shp_object );
			SHPDestroyObject( shp_object );

			element++;			
		}
	}


	SHPClose(shp_handle);
	DBFClose(dbf_handle);
	
	cout<<"Dummy routing nodes removed: "<<nodIdRemoved<<endl;

	shp_handle = SHPCreate("nod_points", SHPT_POINT);
	dbf_handle = DBFCreate("nod_points");

	DBFAddField(dbf_handle, "nodid", FTInteger, 5, 0);
	DBFAddField(dbf_handle, "bound", FTInteger, 2, 0);
	
	element = 0;
	for( std::map<__int64,RoutingNode>::iterator rn = node_data.begin(); rn != node_data.end(); rn++ ) {
		x[0] = (*rn).second.coord.x;
		y[0] = (*rn).second.coord.y;
		shp_object = SHPCreateSimpleObject( SHPT_POINT, 1, x, y, NULL );
		SHPComputeExtents( shp_object );
		SHPWriteObject( shp_handle, -1, shp_object );
		SHPDestroyObject( shp_object );

		DBFWriteIntegerAttribute (dbf_handle,element,0,(int)GetNodID((*rn).second.nodeId));
		DBFWriteIntegerAttribute (dbf_handle,element,1,(int)GetNodID((*rn).second.bound));
		element++;
	}
	SHPClose(shp_handle);
	DBFClose(dbf_handle);

}

void LineMerge::ExportMapping() {
	int			element = 0;
	long		tmp_node_id;
	DBFHandle	dbf_handle;

	dbf_handle = DBFCreate("processed_mapping");
	
	DBFAddField(dbf_handle, "org_roadid", FTInteger, 9, 0);
	DBFAddField(dbf_handle, "new_roadid", FTInteger, 9, 0);

	for( vector<RgnMapping>::iterator it = rgnIdMapping.begin(); it != rgnIdMapping.end(); it++) {
		DBFWriteIntegerAttribute (dbf_handle,element,0,(int)(*it).oryginalRgnId);
		DBFWriteIntegerAttribute (dbf_handle,element,1,(int)GetRoadID((*it).newRgnId));

		element++;			
	}

	DBFClose(dbf_handle);

	if( !rgnRestriction.empty() ) {
		dbf_handle = DBFCreate("processed_restriction");
		DBFAddField(dbf_handle, "nodeid1", FTInteger, 9, 0);
		DBFAddField(dbf_handle, "nodeid2", FTInteger, 9, 0);
		DBFAddField(dbf_handle, "nodeid3", FTInteger, 9, 0);
		DBFAddField(dbf_handle, "nodeid4", FTInteger, 9, 0);
		DBFAddField(dbf_handle, "roadid1", FTInteger, 9, 0);
		DBFAddField(dbf_handle, "roadid2", FTInteger, 9, 0);
		DBFAddField(dbf_handle, "roadid3", FTInteger, 9, 0);

		element = 0;
		for( vector<Restriction>::iterator ir = rgnRestriction.begin(); ir != rgnRestriction.end(); ir++) {
			DBFWriteIntegerAttribute (dbf_handle,element,0,(int)GetNodID((*ir).nodeId1));
			DBFWriteIntegerAttribute (dbf_handle,element,1,(int)GetNodID((*ir).nodeId2));
			DBFWriteIntegerAttribute (dbf_handle,element,2,(int)GetNodID((*ir).nodeId3));
			DBFWriteIntegerAttribute (dbf_handle,element,3,(int)GetNodID((*ir).nodeId4));

			DBFWriteIntegerAttribute (dbf_handle,element,4,(int)GetRoadID(GetNewRoadID((*ir).RoadId1)));
			DBFWriteIntegerAttribute (dbf_handle,element,5,(int)GetRoadID(GetNewRoadID((*ir).RoadId2)));
			DBFWriteIntegerAttribute (dbf_handle,element,6,(int)GetRoadID(GetNewRoadID((*ir).RoadId3)));
		
			element++;
		}
		DBFClose(dbf_handle);
	}
}

void LineMerge::ExportLines() {
	SHPHandle	shp_handle;
	DBFHandle	dbf_handle;
	SHPObject*	shp_object;
	int			element = 0;
	double		x[50000];
	double		y[50000];
	int			point = 0;
	string		city,region,country;
	string		zip,exit;
	int			r_class[5];
	float		r_all = 0;
	r_class[0] = r_class[1] = r_class[2] = r_class[3] = r_class[4] = 0;

	shp_handle = SHPCreate("processed_line", SHPT_ARC);
	dbf_handle = DBFCreate("processed_line");

	DBFAddField(dbf_handle, "type", FTInteger, 5, 0);
	DBFAddField(dbf_handle, "label", FTString, MAX_LBL_LENGTH, 0);
	DBFAddField(dbf_handle, "descr", FTString, MAX_LBL_LENGTH, 0);
	DBFAddField(dbf_handle, "label3", FTString, MAX_LBL_LENGTH, 0);

	DBFAddField(dbf_handle, "city", FTString, MAX_LBL_LENGTH, 0);
	DBFAddField(dbf_handle, "region", FTString, MAX_LBL_LENGTH, 0);
	DBFAddField(dbf_handle, "country", FTString, MAX_LBL_LENGTH, 0);
	DBFAddField(dbf_handle, "zip", FTString, MAX_LBL_LENGTH, 0);
	
	DBFAddField(dbf_handle, "oneway", FTInteger, 5, 0);
	DBFAddField(dbf_handle, "toll", FTInteger, 5, 0);
	DBFAddField(dbf_handle, "speed", FTInteger, 5, 0);
	DBFAddField(dbf_handle, "roadclass", FTInteger, 5, 0);
	
	DBFAddField(dbf_handle, "roadid", FTInteger, 9, 0);

	DBFAddField(dbf_handle, "level", FTInteger, 5, 0);
	DBFAddField(dbf_handle, "endlevel", FTInteger, 5, 0);

	DBFAddField(dbf_handle, "StreetVE", FTInteger, 1, 0);
	DBFAddField(dbf_handle, "StreetVD", FTInteger, 1, 0);
	DBFAddField(dbf_handle, "StreetVC", FTInteger, 1, 0);
	DBFAddField(dbf_handle, "StreetVB", FTInteger, 1, 0);
	DBFAddField(dbf_handle, "StreetVT", FTInteger, 1, 0);
	DBFAddField(dbf_handle, "StreetVP", FTInteger, 1, 0);
	DBFAddField(dbf_handle, "StreetVI", FTInteger, 1, 0);
	DBFAddField(dbf_handle, "StreetVR", FTInteger, 1, 0);

	for( vector<Line*>::iterator it = input_data.begin(); it != input_data.end(); it++) {
		//SHP
		//if( (*it)->rgnId == -1 )
		//	continue;

		//if( (*it)->rgnId == 2 ) {
		//	point = 0;
		//}

		point = 0;
		if( (*it)->points.size() < 2 )
			continue;

		r_class[(*it)->roadClass]++;
		r_all++;

		for( vector<Coords>::iterator cord = (*it)->points.begin(); cord != (*it)->points.end(); cord++ ) {
			x[point] = (*cord).x;
			y[point] = (*cord).y;
			point++;
		}

		shp_object = SHPCreateSimpleObject( SHPT_ARC, point, x, y, NULL );
		SHPComputeExtents( shp_object );
		if( SHPWriteObject( shp_handle, -1, shp_object ) < 0 )
			cout<<"ERROR writing element:"<<element<<endl;
		SHPDestroyObject( shp_object );

		if( !DBFWriteIntegerAttribute (dbf_handle,element,0,(*it)->type) )
			cout<<"ERROR attr 1 element:"<<element<<endl;

		if( !DBFWriteStringAttribute (dbf_handle,element,1,(*it)->label.c_str()) )
			cout<<"ERROR attr 2 element:"<<element<<endl;
		DBFWriteStringAttribute (dbf_handle,element,2,(*it)->description.c_str());
		DBFWriteStringAttribute (dbf_handle,element,3,(*it)->label3.c_str());

		if( (*it)->segments.size() && (!(*it)->city.size() || !(*it)->region.size() || !(*it)->country.size() || !(*it)->zip.size()) ) {
			if( (*it)->segments[0].rightZip.size() ) 
				(*it)->zip = (*it)->segments[0].rightZip; 
			else if( (*it)->segments[0].leftZip.size() ) 
				(*it)->zip = (*it)->segments[0].leftZip; 

			if( (*it)->segments[0].rightCity.size() ) 
				(*it)->city = (*it)->segments[0].rightCity; 
			else if( (*it)->segments[0].leftCity.size() ) 
				(*it)->city = (*it)->segments[0].leftCity; 

			if( (*it)->segments[0].rightRegion.size() ) 
				(*it)->region = (*it)->segments[0].rightRegion; 
			else if( (*it)->segments[0].leftRegion.size() ) 
				(*it)->region = (*it)->segments[0].leftRegion; 

			if( (*it)->segments[0].rightCountry.size() ) 
				(*it)->country = (*it)->segments[0].rightCountry; 
			else if( (*it)->segments[0].leftCountry.size() ) 
				(*it)->country = (*it)->segments[0].leftCountry; 
		}
		DBFWriteStringAttribute (dbf_handle,element,4,(*it)->city.c_str());//city
		DBFWriteStringAttribute (dbf_handle,element,5,(*it)->region.c_str());//region
		DBFWriteStringAttribute (dbf_handle,element,6,(*it)->country.c_str());//country
		DBFWriteStringAttribute (dbf_handle,element,7,(*it)->zip.c_str());
		DBFWriteIntegerAttribute (dbf_handle,element,8,(*it)->oneWay);
		DBFWriteIntegerAttribute (dbf_handle,element,9,(*it)->toll);

		DBFWriteIntegerAttribute (dbf_handle,element,10,(*it)->speed);
		DBFWriteIntegerAttribute (dbf_handle,element,11,(*it)->roadClass);

		if( !DBFWriteIntegerAttribute (dbf_handle,element,12,(int)GetRoadID((*it)->rgnId)) )
			cout<<"ERROR attr 13 element:"<<element<<endl;

		DBFWriteIntegerAttribute (dbf_handle,element,13,(*it)->level>-1?(*it)->level:0 );//start_level);
		DBFWriteIntegerAttribute (dbf_handle,element,14,(*it)->endLevel>-1?(*it)->endLevel:0);//end_level);

		DBFWriteIntegerAttribute (dbf_handle,element,15,(*it)->StreetVE);
		DBFWriteIntegerAttribute (dbf_handle,element,16,(*it)->StreetVD);
		DBFWriteIntegerAttribute (dbf_handle,element,17,(*it)->StreetVC);
		DBFWriteIntegerAttribute (dbf_handle,element,18,(*it)->StreetVB);
		DBFWriteIntegerAttribute (dbf_handle,element,19,(*it)->StreetVT);
		DBFWriteIntegerAttribute (dbf_handle,element,20,(*it)->StreetVP);
		DBFWriteIntegerAttribute (dbf_handle,element,21,(*it)->StreetVI);
		DBFWriteIntegerAttribute (dbf_handle,element,22,(*it)->StreetVR);

		element++;
	}

	SHPClose(shp_handle);
	DBFClose(dbf_handle);

	cout<<"Routing class stat:"<<endl;
	cout<<"Class 4: "<<r_class[4]<<" -> "<<(static_cast<float>(r_class[4])/r_all)*100.0<<endl;
	cout<<"Class 3: "<<r_class[3]<<" -> "<<(static_cast<float>(r_class[3])/r_all)*100.0<<endl;
	cout<<"Class 2: "<<r_class[2]<<" -> "<<(static_cast<float>(r_class[2])/r_all)*100.0<<endl;
	cout<<"Class 1: "<<r_class[1]<<" -> "<<(static_cast<float>(r_class[1])/r_all)*100.0<<endl;
	cout<<"Class 0: "<<r_class[0]<<" -> "<<(static_cast<float>(r_class[0])/r_all)*100.0<<endl;
}

void LineMerge::ExportMP() {
	int		point;
	int		nodIdRemoved = 0;

	vector<Segment>::iterator segment_i;

	ofstream mp("processed_line.mp",ios::out);

	mp<<";*******************************************************************************"<<endl;
	mp<<";cGPSmapper a Generic routing generator"<<endl;
	mp<<";Copyright(C) Stanislaw Kozicki, 2000-2004"<<endl;
	mp<<";*******************************************************************************"<<endl;
	mp<<";REMEMBER TO ADD A VALID [IMG ID] SECTION"<<endl;
	mp<<endl<<endl;
	mp<<"[IMG ID]"<<endl<<"ID=90000000"<<endl<<"Name=MapRoute generated map"<<endl<<"Routing=Y"<<endl;
	mp<<"Levels=5"<<endl<<"Level0=24"<<endl<<"Level1=22"<<endl<<"Level2=20"<<endl;
	mp<<"Level3=19"<<endl<<"Level4=18"<<endl<<"[END]"<<endl;
	mp<<endl<<";Followind DICTIONARY section can/should be modified to meet your requirements"<<endl<<endl;

mp<<"[DICTIONARY]"<<endl;
mp<<";           0              1               2               3               4               5               6              "<<endl;
mp<<";           123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcde"<<endl;
mp<<"Level1Rgn10=10000100001111000001011111110101111111110111111100000000000000011111111111111111111110101000110000011100000000"<<endl;
mp<<"Level2Rgn10=00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
mp<<"Level3Rgn10=00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
mp<<"Level4Rgn10=00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
mp<<"Level5Rgn10=00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
mp<<"Level6Rgn10=00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
mp<<"Level7Rgn10=00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
mp<<"Level8Rgn10=00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
mp<<"Level9Rgn10=00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
mp<<"Level1Rgn20=11111111111111111111000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
mp<<"Level2Rgn20=11111111111111111000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
mp<<"Level3Rgn20=11111111111100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
mp<<"Level4Rgn20=11111111111100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
mp<<"Level5Rgn20=11111111111100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
mp<<"Level6Rgn20=11111111111100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
mp<<"Level7Rgn20=11111111111100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
mp<<"Level8Rgn20=11111111111100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
mp<<"Level9Rgn20=11111111111100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
mp<<"Level1Rgn40=11111110000000000001000101111110110000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
mp<<"Level2Rgn40=11111000000000000000000000011110010000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
mp<<"Level3Rgn40=11100000000000000000000000010100000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
mp<<"Level4Rgn40=11100000000000000000000000010100000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
mp<<"Level5Rgn40=11100000000000000000000000010100000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
mp<<"Level6Rgn40=11100000000000000000000000010100000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
mp<<"Level7Rgn40=11100000000000000000000000010100000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
mp<<"Level8Rgn40=11100000000000000000000000010100000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
mp<<"Level9Rgn40=11100000000000000000000000010100000000000000000000000000000000000000000000000000000000000000000000000000000000"<<endl;
mp<<"Level1Rgn80=11100000000000000001111001000110000000010000000001000000000111111111011110010001000000000000000000000000000000"<<endl;
mp<<"Level2Rgn80=11100000000000000001110000000000000000010000000001000000000111100000010000000001000000000000000000000000000000"<<endl;
mp<<"Level3Rgn80=11100000000000000001110000000000000000010000000001000000000111100000010000000001000000000000000000000000000000"<<endl;
mp<<"Level4Rgn80=11100000000000000001110000000000000000010000000001000000000111100000010000000001000000000000000000000000000000"<<endl;
mp<<"Level5Rgn80=11100000000000000001110000000000000000010000000001000000000111100000010000000001000000000000000000000000000000"<<endl;
mp<<"Level6Rgn80=11100000000000000001110000000000000000010000000001000000000111100000010000000001000000000000000000000000000000"<<endl;
mp<<"Level7Rgn80=11100000000000000001110000000000000000010000000001000000000111100000010000000001000000000000000000000000000000"<<endl;
mp<<"Level8Rgn80=11100000000000000001110000000000000000010000000001000000000111100000010000000001000000000000000000000000000000"<<endl;
mp<<"Level9Rgn80=11100000000000000001110000000000000000010000000001000000000111100000010000000001000000000000000000000000000000"<<endl;
mp<<"[END-DICTIONARY]"<<endl<<endl;

	for( vector<Restriction>::iterator ir = rgnRestriction.begin(); ir != rgnRestriction.end(); ir++) {
		mp<<"[Restrict]"<<endl;
		mp<<"Nod="<<GetNodID((*ir).nodeId2)<<endl;
		mp<<"TraffPoints="<<GetNodID((*ir).nodeId1)<<","<<GetNodID((*ir).nodeId2);
		if( (*ir).nodeId3 >-1 )	mp<<","<<GetNodID((*ir).nodeId3);
		if( (*ir).nodeId4 >-1)	mp<<","<<GetNodID((*ir).nodeId4);
		mp<<endl;
		mp<<"TraffRoads="<<GetRoadID((*ir).RoadId1);
		if( (*ir).RoadId2 >-1)	mp<<","<<GetRoadID((*ir).RoadId2);
		if( (*ir).RoadId3 >-1)	mp<<","<<GetRoadID((*ir).RoadId3);
		mp<<endl<<"[END-Restrict]"<<endl<<endl;	
	}

	for( vector<Line*>::iterator it = input_data.begin(); it != input_data.end(); it++) {
		if( (*it)->points.size() < 2 )
			continue;

		mp<<"[RGN40]"<<endl;
		mp<<"Type=0x"<<hex<<(int)(*it)->type<<dec<<endl;
		mp<<"RoadID="<<GetRoadID((*it)->rgnId)<<endl;
		mp<<"Label="<<(*it)->label<<endl;
		if( (*it)->level > -1 )
			mp<<"Level="<<(*it)->level<<endl;
		if( (*it)->endLevel > -1 ) 
			mp<<"EndLevel="<<(*it)->endLevel<<endl;
		else 
			mp<<"EndLevel=9"<<endl;
		mp<<"Data0=";
		point = 0;
		for( vector<Coords>::iterator cord = (*it)->points.begin(); cord != (*it)->points.end(); cord++,point++ ) {
			if( point )
				mp<<",";
			mp<<"("<< (format_double("%.9f",(*cord).y)).c_str() <<",";
			mp<<format_double("%.9f",(*cord).x).c_str()<<")";
		}
		mp<<endl;
		mp<<"Label2="<<(*it)->description<<endl;
		mp<<"Label3="<<(*it)->label3<<endl;
		mp<<"DirIndicator="<<(*it)->oneWay<<endl;//(*it)->direction<<endl;
		mp<<"CityName="<<(*it)->city<<endl<<"RegionName="<<(*it)->region<<endl<<"CountryName="<<(*it)->country<<endl;
		mp<<"Zip="<<(*it)->zip<<endl;
		
		mp<<"Routeparam="<<(*it)->speed<<","<<(*it)->roadClass<<","<<(*it)->oneWay<<","<<(*it)->toll<<","<<(*it)->StreetVE<<","<<(*it)->StreetVD<<","<<(*it)->StreetVC<<","<<(*it)->StreetVB<<","<<(*it)->StreetVT<<","<<(*it)->StreetVP<<","<<(*it)->StreetVI<<","<<(*it)->StreetVR<<endl;

		for( segment_i = (*it)->segments.begin(), point = 1; segment_i != (*it)->segments.end(); segment_i++,point++ ) {
			if( (*segment_i).leftStart == 0 && (*segment_i).leftEnd == 0 )
				(*segment_i).leftType = 0;
			if( (*segment_i).rightStart == 0 && (*segment_i).rightEnd == 0 )
				(*segment_i).rightType = 0;

			mp<<"Numbers"<<point<<"="<<(*segment_i).nodeStart<<",";
			mp<< ((*segment_i).leftType == 0 ? "N" : ((*segment_i).leftType == 1 ? "E" : ((*segment_i).leftType == 2 ? "O" : "B")))<<",";
			mp<<(*segment_i).leftStart<<","<<(*segment_i).leftEnd<<",";
			mp<< ((*segment_i).rightType == 0 ? "N" : ((*segment_i).rightType == 1 ? "E" : ((*segment_i).rightType == 2 ? "O" : "B")))<<",";
			mp<<(*segment_i).rightStart<<","<<(*segment_i).rightEnd<<",";

			if( (*segment_i).leftZip.length() ) mp<<(*segment_i).leftZip<<",";
			else mp<<"-1,";
			if( (*segment_i).rightZip.length() ) mp<<(*segment_i).rightZip<<",";
			else mp<<"-1,";

			if( (*segment_i).leftCity.length() ){
				mp<<(*segment_i).leftCity<<","<<(*segment_i).leftRegion<<","<<(*segment_i).leftCountry<<",";
			} else
				mp<<"-1,";
			if( (*segment_i).rightCity.length() ){
				mp<<(*segment_i).rightCity<<","<<(*segment_i).rightRegion<<","<<(*segment_i).rightCountry<<endl;
			} else
				mp<<"-1"<<endl;
		}
		for( segment_i = (*it)->segments.begin(), point = 1; segment_i != (*it)->segments.end(); segment_i++ ) {
			if( (*segment_i).nodeId >= 0 ) {
				assert((*segment_i).nodeId >= 0);
				//assert((*segment_i).nodeId < array_size);

				if( NodeIDUsage[(*segment_i).nodeId] > 1 ) {
					mp<<"Nod"<<point<<"="<<(*segment_i).nodeStart<<","<<GetNodID((*segment_i).nodeId)<<","<<(*segment_i).bound<<endl;
					point++;
				} else
					nodIdRemoved++;

			}
		}
		mp<<"[END]"<<endl<<endl;
	}
	cout<<"Dummy routing nodes removed: "<<nodIdRemoved<<endl;
}

void LineMerge::ShowProgress(int all, int current) {
	if( current == 0 ) {
		progress = 0;
		cout<<"\r0%";
	}
	double p,c,a;
	c = (double)current;
	a = (double)all;
	p = c / a * 100.0;
	int	p1 = (int)p;
	if( p1 != progress ) {
		cout<<"\r"<<p1<<"%";
		progress = p1;
	}
}

/*
Our restrictions are exactly in MPC format i.e:

 

"The TURN_RSTRS attribute, together with the LINK_ID attribute, specifies any turn restrictions
that the current road segment may be apart of. Turn restrictions provide a way to specify that 
certain maneuvers are not allowed, such as a turn from one road to another road, or u-turns at 
a particular intersection. In order to identify road segments that are involved in a restriction 
the LINK_ID attribute provides a numeric identifier that should be unique between all of the input
Shapefiles.  Additionally turn restrictions can be specified to apply to specific vehicle types 
in a similar format as the ACC_MASK attribute.

TURN_RSTRS attributes are only populated for the road segment that starts a restricted maneuver.
All information for chaining to all the other road segments in a restricted maneuver is contained 
in the starting road segment TURN_RSTRS attribute.

The format of the TURN_RSTRS attribute is a string with a character to indicate which end point 
vertex of the starting road segment chains onto the next road followed by a comma delimited list 
of numeric LINK_IDs which indicate the other road segments involved in the restricted maneuver.  
The other road segments are listed in order of traversal along the restricted maneuver from the 
starting segment to the last road segment involved.  As circular road segments are not allowed in 
the MPC software input this provides a unique sequence of road segments, identified by LINK_ID that
makes up a restricted maneuver. Additionally, the first chaining character is required to make no 
u-turn restrictions unambiguous.  The LINK_IDs list may also be followed by an optional vehicle 
mask that provides which vehicle types that the restriction does not apply to.

A leading character of 'F' indicates that the first vertex of the current road segment chains onto 
the first road segment who's unique LINK_ID is given at the head of the comma delimited list. A 
leading character of 'L' indicates that the last vertex of the current road segment chains to the 
first road segment in the comma delimited list. Additional turn restrictions can be specified in the 
TURN_RSTRS attribute by separating restricted maneuvers by a delimiting semicolon.

For example, if there were two restrictions starting at the first vertex of the current road segment 
with LINK_ID=4, one being a no left turn onto to a road segment with LINK_ID=5 and one being a no 
u-turn at the first vertex of the current road, the TURN_RSTRS value of the LINK_ID=4 segment should 
look like 'F5;F4'. If the no u-turn restriction applies instead to the end of the LINK_ID=4 road 
segment the TURN_RSTRS value would be 'F5;L4';

As another example, suppose that the maneuver going from the end of road segment with LINK_ID=4 to 
a segment with LINK_ID=5 followed by the segment with LINK_ID=6 is restricted. Then the TURN_RSTRS 
value for the segment with LINK_ID=4 would be 'L5,6'. "

 

Should I create 2 tables with this structures:

1-      Nodes -> RoadID, NodeIndex, NodeID  -> with only nodes with restrictions : NodeID sould be unique is it?

2-      Restrictions.dbf -> fromRodID, To RoadId, NodeID1, NodeID2, NodeID3

Is it all what cgpsmapper requires?
All the best
*/
