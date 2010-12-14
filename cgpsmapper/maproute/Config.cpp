/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

#include "config.h"

using namespace std;

double ConfigReader::CRITICAL_DISTANCE = 3.0;
double ConfigReader::DIFF_DISTANCE = 0.3;
int ConfigReader::ANGLE_DIFF = 30;
int ConfigReader::HIGHWAY1_MAX_ANGLE_DIFF = 60;
int	ConfigReader::HIGHWAY2_MAX_ANGLE_DIFF = 70;
int	ConfigReader::HIGHWAY3_MAX_ANGLE_DIFF = 85;
double ConfigReader::TOLERANCE_METERS = -1.0;
bool ConfigReader::ENUMERATE = false;
bool ConfigReader::GENERATE_SPEED = false;
bool ConfigReader::PRESERVE_ROAD_CLASS = false;
Datum	ConfigReader::WGS84 = Datum("WGS84");

ConfigReader::ConfigReader(const char* file_name) {

	xor_fstream *ini_file = new xor_fstream(file_name, "rb");
	if(ini_file->file_size == 0 || ini_file->error == true) {
		cout << file_name << " can't be open!" << endl;
		delete ini_file;
		return;
	}

	for(int i = 0; i<25; ++i) {
		r_class1[i] = 0;
		r_class2[i] = 0;
		r_class3[i] = 0;
		r_class4[i] = 0;
	}

	r_class4[1] =1;

	r_class3[2] =1;r_class3[8] =1;r_class3[9] =1;r_class3[0xb] =1;
	
	r_class2[3] =1;r_class3[0xc] =1;

	r_class1[4] =1;

	cout << file_name << " found!" << endl;

	string	key, value;
	int		t_read;

	t_read = ini_file->ReadInput(key, value);
	while(t_read == 1 || t_read == 3 || t_read == 5 || t_read == 2) {
		if(key == string("[ROUTING]"))
			ReadRouting(ini_file);

		if(key == string("[RESTRICTION]"))
			ReadRestrictions(ini_file);
		
		if(key == string("[DATA]"))
			ReadData(ini_file);

		if(key == string("[ROUTINGCLASS]"))
			ReadRClass(ini_file);

		if(key == string("[PARAMETERS]"))
			ReadParameters(ini_file);

		t_read = ini_file->ReadInput(key, value);
	}

	delete ini_file;
}

void ConfigReader::ReadRClass(xor_fstream *file) {
	string	key, value;
	int		t_read;

	t_read = file->ReadInput(key, value);
	while(t_read != 0 && key.find("[END") > 0) {
		if(key == "4" && t_read == 2) r_class4[atoi(value.c_str())] = 1;
		if(key == "3" && t_read == 2) r_class3[atoi(value.c_str())] = 1;
		if(key == "2" && t_read == 2) r_class2[atoi(value.c_str())] = 1;
		if(key == "1" && t_read == 2) r_class1[atoi(value.c_str())] = 1;

		t_read = file->ReadInput(key, value);
	}
}

void ConfigReader::ReadData(xor_fstream *file) {
	if( data_file.length() ) {
		cout<<"Only ONE data file can be processed at a time"<<endl;
	}
	string	key, value;
	int		t_read;
	dataDefaultType = 0;

	navteq = false;
	mapsource = false;
	t_read = file->ReadInput(key, value);
	while(t_read != 0 && key.find("[END") > 0) {
		if(key == "FILE" && t_read == 2) data_file = value;
		if(key == "TYPE" && t_read == 2) dataType = value;
		if(key == "DEFAULTTYPE" && t_read == 2) dataDefaultType = atoi(value.c_str());
		if(key == "SPEED" && t_read == 2) dataSpeed = value;
		if(key == "ROADCLASS" && t_read == 2) dataClass = value;
		if(key == "LABEL" && t_read == 2) dataLabel = value;
		if(key == "LABEL2" && t_read == 2) dataDescr = value;
		if(key == "LABEL3" && t_read == 2) dataLabel3 = value;
		if(key == "LEVEL" && t_read == 2) dataLevel = value;
		if(key == "ENDLEVEL" && t_read == 2) dataEndLevel = value;
		if(key == "DESCRIPTION" && t_read == 2) dataDescr = value;
		if(key == "DIRECTION" && t_read == 2) dataDirection = value;
		if(key == "CITY" && t_read == 2) dataCity = value;
		if(key == "REGION" && t_read == 2) dataRegion = value;
		if(key == "COUNTRY" && t_read == 2) dataCountry = value;
		if(key == "ZIP" && t_read == 2) dataZip = value;
		if(key == "ONEWAY" && t_read == 2) dataOneway = value;
		if(key == "TOLL" && t_read == 2) dataToll = value;
		if(key == "ROADID" && t_read == 2) dataRoadID = value;
		if(key == "RESTRICTION" && t_read == 2) restrictionMPC = value;

		if(key == "VEHICLEE" && t_read == 2) StreetVE = value;//emergency
		if(key == "VEHICLED" && t_read == 2) StreetVD = value;//delivery
		if(key == "VEHICLEC" && t_read == 2) StreetVC = value;//car
		if(key == "VEHICLEB" && t_read == 2) StreetVB = value;//bus
		if(key == "VEHICLET" && t_read == 2) StreetVT = value;//taxi
		if(key == "VEHICLEP" && t_read == 2) StreetVP = value;//pedestrian
		if(key == "VEHICLEI" && t_read == 2) StreetVI = value;//bicycle
		if(key == "VEHICLER" && t_read == 2) StreetVR = value;//truck

		if(key == "COLLECTEDRESTRICTIONS" && t_read == 2) restrAttributes = value;//truck

		//navteq or mapsource
		if(key == "ZLEVEL" && t_read == 2) zLevel= value;
		if(key == "NONREFERENCENODE" && t_read == 2) nonReferenceNode= value;
		if(key == "REFERENCENODE" && t_read == 2) referenceNode= value;
		if(key == "LINKID" && t_read == 2) dataRoadID = value;
		if(key == "FUNCCLASS" && t_read == 2) funcClass = value;

		if(key == "CONNECTIONFILE" && t_read == 2) dataConnFile = value;
		if(key == "CONNECTIONLINKID" && t_read == 2) connRoadId = value;
		if(key == "CONNECTIONNODEID" && t_read == 2) connNodeId = value;

		//MPC-
		if(key == "ZLEVELSTART" && t_read == 2) zLevelStart = value;
		if(key == "ZLEVELEND" && t_read == 2) zLevelEnd = value;

		if(key == "LEFTSTART" && t_read == 2) routingLeftStart = value;
		if(key == "LEFTEND" && t_read == 2) routingLeftEnd = value;
		if(key == "LEFTTYPE" && t_read == 2) routingLeftType = value;
		if(key == "RIGHTSTART" && t_read == 2) routingRightStart = value;
		if(key == "RIGHTEND" && t_read == 2) routingRightEnd = value;
		if(key == "RIGHTTYPE" && t_read == 2) routingRightType = value;

		if(key == "LEFTZIP" && t_read == 2) routingLeftZip = value;
		if(key == "RIGHTZIP" && t_read == 2) routingRightZip = value;

		if(key == "LEFTCITY" && t_read == 2) routingLeftCity = value;
		if(key == "LEFTREGION" && t_read == 2) routingLeftRegion = value;
		if(key == "LEFTCOUNTRY" && t_read == 2) routingLeftCountry = value;
		if(key == "RIGHTCITY" && t_read == 2) routingRightCity = value;
		if(key == "RIGHTREGION" && t_read == 2) routingRightRegion = value;
		if(key == "RIGHTCOUNTRY" && t_read == 2) routingRightCountry = value;

		t_read = file->ReadInput(key, value);
	}

	cout<<"Input file(s): "<<data_file<<endl;
	cout<<"** Fields **"<<endl;
	cout<<"TYPE - "<<dataType<<endl;
	cout<<"DEFAULTTYPE - "<<dataDefaultType<<endl;
	cout<<"SPEED - "<<dataSpeed<<endl;
	cout<<"ROADCLASS - "<<dataClass<<endl;
	cout<<"LABEL - "<<dataLabel<<endl;
	cout<<"LABEL2 - "<<dataDescr<<endl;
	cout<<"LABEL3 - "<<dataLabel3<<endl;
	cout<<"LEVEL - "<<dataLevel<<endl;
	cout<<"ENDLEVEL - "<<dataEndLevel<<endl;
	cout<<"DESCRIPTION - "<<dataDescr<<endl;
	cout<<"DIRECTION - "<<dataDirection<<endl;
	cout<<"CITY - "<<dataCity<<endl;
	cout<<"REGION - "<<dataRegion<<endl;
	cout<<"COUNTRY - "<<dataCountry<<endl;
	cout<<"ZIP - "<<dataZip<<endl;
	cout<<"ONEWAY - "<<dataOneway<<endl;
	cout<<"TOLL - "<<dataToll<<endl;
	cout<<"ROADID - "<<dataRoadID <<endl;
	cout<<"RESTRICTION - "<<restrictionMPC<<endl;
	//cout<<"ZLEVEL - "<<zLevel<<endl;

	cout<<"VEHICLEE - "<<StreetVE<<endl;//emergency
	cout<<"VEHICLED - "<<StreetVD<<endl;//delivery
	cout<<"VEHICLEC - "<<StreetVC<<endl;//car
	cout<<"VEHICLEB - "<<StreetVB<<endl;//bus
	cout<<"VEHICLET - "<<StreetVT<<endl;//taxi
	cout<<"VEHICLEP - "<<StreetVP<<endl;//pedestrian
	cout<<"VEHICLEI - "<<StreetVI<<endl;//bicycle
	cout<<"VEHICLER - "<<StreetVR<<endl;//truck
	cout<<"COLLECTEDRESTRICTIONS - "<<restrAttributes<<endl;

	if( zLevelStart.size() && zLevelEnd.size() ) {
		cout<<"MPC input data defined, connection data will be created using zLevelStart and zLevelEnd flags."<<endl;
		cout<<"ZLEVELSTART - "<<zLevelStart<<endl;
		cout<<"ZLEVELEND - "<<zLevelEnd<<endl;
		cout<<"LINK ID (replace ROADID if defined) - "<<connRoadId<<endl;

		cout<<"LEFTSTART - "<<routingLeftStart<<endl;
		cout<<"LEFTEND - "<<routingLeftEnd<<endl;
		cout<<"LEFTTYPE - "<<routingLeftType<<endl;
		cout<<"RIGHTSTART - "<<routingRightStart<<endl;
		cout<<"RIGHTEND - "<<routingRightEnd<<endl;
		cout<<"RIGHTTYPE - "<<routingRightType<<endl;

		cout<<"LEFTCITY - "<<routingLeftCity<<endl;
		cout<<"RIGHTCITY - "<<routingRightCity<<endl;
		cout<<"LEFTREGION - "<<routingLeftRegion<<endl;
		cout<<"RIGHTREGION - "<<routingRightRegion<<endl;
		cout<<"LEFTCOUNTRY - "<<routingLeftCountry<<endl;
		cout<<"RIGHTCOUNTRY - "<<routingRightCountry<<endl;

		cout<<"LEFTZIP - "<<routingLeftZip<<endl;
		cout<<"RIGHTZIP - "<<routingRightZip<<endl;

		mapsource = true;
	}

	if( /*zLevel.size() &&*/ nonReferenceNode.size() && referenceNode.size() && dataConnFile.size() && connRoadId.size() && connNodeId.size() ) {
		cout<<"NAVTEQ input data defined, connection data will be read from file: "<<dataConnFile<<".shp and dbf."<<endl;
		cout<<"ZLEVEL - "<<zLevel<<endl;
		cout<<"REFERENCENODE - "<<referenceNode<<endl;
		cout<<"NONREFERENCENODE - "<<nonReferenceNode<<endl;
		cout<<"FUNCCLASS - "<<funcClass<<endl;
		cout<<"* Connection data *"<<endl;
		cout<<"Connection input file: "<<dataConnFile<<".shp"<<endl;
		cout<<"LINK ID (replace ROADID if defined) - "<<connRoadId<<endl;
		cout<<"NODE ID - "<<connNodeId<<endl;

		cout<<"LEFTSTART - "<<routingLeftStart<<endl;
		cout<<"LEFTEND - "<<routingLeftEnd<<endl;
		cout<<"LEFTTYPE - "<<routingLeftType<<endl;
		cout<<"RIGHTSTART - "<<routingRightStart<<endl;
		cout<<"RIGHTEND - "<<routingRightEnd<<endl;
		cout<<"RIGHTTYPE - "<<routingRightType<<endl;

		cout<<"LEFTCITY - "<<routingLeftCity<<endl;
		cout<<"RIGHTCITY - "<<routingRightCity<<endl;
		cout<<"LEFTREGION - "<<routingLeftRegion<<endl;
		cout<<"RIGHTREGION - "<<routingRightRegion<<endl;
		cout<<"LEFTCOUNTRY - "<<routingLeftCountry<<endl;
		cout<<"RIGHTCOUNTRY - "<<routingRightCountry<<endl;

		cout<<"LEFTZIP - "<<routingLeftZip<<endl;
		cout<<"RIGHTZIP - "<<routingRightZip<<endl;

		navteq = true;
	}
	cout<<"** End **"<<endl;
}

void ConfigReader::ReadRouting(xor_fstream *file) {
	if( routing_file.length() ) {
		cout<<"Only ONE data file can be processed at a time"<<endl;
	}
	string	key, value;
	int		t_read;

	if( navteq ) {
		cout<<"Navteq format data reading, skipping routing read of cGPSmapper format."<<endl;
		return;
	}

	t_read = file->ReadInput(key, value);
	while(t_read != 0 && key.find("[END") > 0) {
		if(key == "FILE" && t_read == 2) routing_file = value;
		if(key == "NODE" && t_read == 2) routingNode = value;
		if(key == "NODEID" && t_read == 2) routingNodeID = value;
		if(key == "ROADID" && t_read == 2) routingRoadID = value;
		if(key == "BOUND" && t_read == 2) routingBound = value;

		if(key == "LEFTSTART" && t_read == 2) routingLeftStart = value;
		if(key == "LEFTEND" && t_read == 2) routingLeftEnd = value;
		if(key == "LEFTTYPE" && t_read == 2) routingLeftType = value;
		if(key == "RIGHTSTART" && t_read == 2) routingRightStart = value;
		if(key == "RIGHTEND" && t_read == 2) routingRightEnd = value;
		if(key == "RIGHTTYPE" && t_read == 2) routingRightType = value;

		if(key == "LEFTZIP" && t_read == 2) routingLeftZip = value;
		if(key == "RIGHTZIP" && t_read == 2) routingRightZip = value;

		if(key == "LEFTCITY" && t_read == 2) routingLeftCity = value;
		if(key == "LEFTREGION" && t_read == 2) routingLeftRegion = value;
		if(key == "LEFTCOUNTRY" && t_read == 2) routingLeftCountry = value;
		if(key == "RIGHTCITY" && t_read == 2) routingRightCity = value;
		if(key == "RIGHTREGION" && t_read == 2) routingRightRegion = value;
		if(key == "RIGHTCOUNTRY" && t_read == 2) routingRightCountry = value;

		t_read = file->ReadInput(key, value);
	}

	cout<<"Routing and street attributes input file: "<<routing_file<<".dbf"<<endl;
	cout<<"** Fields **"<<endl;
	cout<<"NODE - "<<routingNode<<endl;
	cout<<"NODEID - "<<routingNodeID<<endl;
	cout<<"ROADID - "<<routingRoadID<<endl;
	cout<<"BOUND - "<<routingBound<<endl;

	cout<<"LEFTSTART - "<<routingLeftStart<<endl;
	cout<<"LEFTEND - "<<routingLeftEnd<<endl;
	cout<<"LEFTTYPE - "<<routingLeftType<<endl;
	cout<<"RIGHTSTART - "<<routingRightStart<<endl;
	cout<<"RIGHTEND - "<<routingRightEnd<<endl;
	cout<<"RIGHTTYPE - "<<routingRightType<<endl;

	cout<<"LEFTZIP - "<<routingLeftZip<<endl;
	cout<<"RIGHTZIP - "<<routingRightZip<<endl;

	cout<<"LEFTCITY - "<<routingLeftCity<<endl;
	cout<<"LEFTREGION - "<<routingLeftRegion<<endl;
	cout<<"LEFTCOUNTRY - "<<routingLeftCountry<<endl;
	cout<<"RIGHTCITY - "<<routingRightCity<<endl;
	cout<<"RIGHTREGION - "<<routingRightRegion<<endl;
	cout<<"RIGHTCOUNTRY - "<<routingRightCountry<<endl;
	cout<<"** End **"<<endl;
}

void ConfigReader::ReadRestrictions(xor_fstream *ini_file) {
	string	key, value;
	int		t_read;

	t_read = ini_file->ReadInput(key, value);
	while(t_read != 0 && key.find("[END") > 0) {
		if(key == "FILE" && t_read == 2) restr_file = value;
		if(key == "NODEID1" && t_read == 2) restrNODID1 = value;
		if(key == "NODEID2" && t_read == 2) restrNODID2 = value;
		if(key == "NODEID3" && t_read == 2) restrNODID3 = value;
		if(key == "NODEID4" && t_read == 2) restrNODID4 = value;
		if(key == "ROADID1" && t_read == 2) restrRoadID1 = value;
		if(key == "ROADID2" && t_read == 2) restrRoadID2 = value;
		if(key == "ROADID3" && t_read == 2) restrRoadID3 = value;
		t_read = ini_file->ReadInput(key, value);
	}

	cout<<"Restrictionc input file: "<<routing_file<<".dbf"<<endl;
	cout<<"** Fields **"<<endl;
	cout<<"NODEID1 - "<<restrNODID1<<endl;
	cout<<"NODEID2 - "<<restrNODID2<<endl;
	cout<<"NODEID3 - "<<restrNODID3<<endl;
	cout<<"NODEID4 - "<<restrNODID4<<endl;
	cout<<"ROADID1 - "<<restrRoadID1<<endl;
	cout<<"ROADID2 - "<<restrRoadID2<<endl;
	cout<<"ROADID3 - "<<restrRoadID3<<endl;
	cout<<"** End **"<<endl;
}

void ConfigReader::ReadParameters(xor_fstream *ini_file) {
	string	key, value;
	int		t_read;

	t_read = ini_file->ReadInput(key, value);
	while(t_read != 0 && key.find("[END") > 0) {
		
		if(key == "TYPETRANSLATION" && t_read == 2) typeTranslation = value;
		if(key == "ENUMERATE" && t_read == 2) ENUMERATE = ( value == "Y" || value == "y" || value == "1");
		if(key == "TOLERANCE" && t_read == 2) TOLERANCE_METERS = atof(value.c_str());
		if(key == "GENERATESPEED" && t_read == 2) GENERATE_SPEED = ( value == "Y" || value == "y" || value == "1");
		if(key == "KEEPROADCLASS" && t_read == 2) PRESERVE_ROAD_CLASS = ( value == "Y" || value == "y" || value == "1");

		t_read = ini_file->ReadInput(key, value);
	}

	if( TOLERANCE_METERS < 0.01 )
		TOLERANCE_METERS = -1.0;
}

MergeParams	ConfigReader::getMergeParams() {
	MergeParams mergeParams;

	
	mergeParams.typeTranslation = typeTranslation;
	mergeParams.type = dataType.c_str();
	mergeParams.speed = dataSpeed.c_str();
	mergeParams.roadClass = dataClass.c_str();
	mergeParams.adefaultType = dataDefaultType;
	mergeParams.level = dataLevel.c_str();
	mergeParams.endLevel = dataEndLevel.c_str();

	mergeParams.label = dataLabel.c_str();
	mergeParams.label3 = dataLabel3.c_str();
	mergeParams.description = dataDescr.c_str();
	mergeParams.city = dataCity.c_str();
	mergeParams.region = dataRegion.c_str();
	mergeParams.country = dataCountry.c_str();
	mergeParams.zip = dataZip.c_str();
	mergeParams.oneWay = dataOneway.c_str();
	mergeParams.restrictionMPC = restrictionMPC.c_str();
	mergeParams.toll = dataToll.c_str(); //speed,roadclass,
	
	mergeParams.zLevel = zLevel.c_str();
	mergeParams.zLevelStart = zLevelStart;
	mergeParams.zLevelEnd = zLevelEnd;

	mergeParams.nonReferenceNode = nonReferenceNode.c_str();
	mergeParams.referenceNode = referenceNode.c_str();
	mergeParams.funcClass = funcClass.c_str();
	mergeParams.dataConnFile = dataConnFile.c_str();
	mergeParams.connRoadId = connRoadId.c_str();
	mergeParams.connNodeId = connNodeId.c_str();
	//mergeParams.navteq = navteq.c_str();

	mergeParams.dataRgnId = dataRoadID.c_str();
	mergeParams.StreetVE = StreetVE.c_str();
	mergeParams.StreetVD = StreetVD.c_str();
	mergeParams.StreetVC = StreetVC.c_str();
	mergeParams.StreetVB = StreetVB.c_str();
	mergeParams.StreetVT = StreetVT.c_str();
	mergeParams.StreetVP = StreetVP.c_str();
	mergeParams.StreetVI = StreetVI.c_str();
	mergeParams.StreetVR = StreetVR.c_str();
	mergeParams.RestrAttributes = restrAttributes.c_str();

	mergeParams.rgnId = routingRoadID.c_str();
	mergeParams.node = routingNode.c_str();
	mergeParams.nodeId = routingNodeID.c_str();
	mergeParams.bound = routingBound.c_str();
	mergeParams.leftCity = routingLeftCity.c_str();
	mergeParams.leftRegion = routingLeftRegion.c_str();
	mergeParams.leftCountry = routingLeftCountry.c_str();
	mergeParams.rightCity = routingRightCity.c_str();
	mergeParams.rightRegion = routingRightRegion.c_str();
	mergeParams.rightCountry = routingRightCountry.c_str();
	mergeParams.leftZip = routingLeftZip.c_str();
	mergeParams.rightZip = routingRightZip.c_str();
	mergeParams.leftStart = routingLeftStart.c_str();
	mergeParams.leftEnd = routingLeftEnd.c_str();
	mergeParams.leftType = routingLeftType.c_str();
	mergeParams.rightStart = routingRightStart.c_str();
	mergeParams.rightEnd = routingRightEnd.c_str();
	mergeParams.rightType = routingRightType.c_str();
	mergeParams.nodeId1 = restrNODID1.c_str();
	mergeParams.nodeId2 = restrNODID2.c_str();
	mergeParams.nodeId3 = restrNODID3.c_str();
	mergeParams.nodeId4 = restrNODID4.c_str();
	mergeParams.roadId1 = restrRoadID1.c_str();
	mergeParams.roadId2 = restrRoadID2.c_str();
	mergeParams.roadId3 = restrRoadID3.c_str();	
	return mergeParams;
}