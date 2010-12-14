/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

#pragma once

#include <iostream>
#include <string>
#include <map>
#include "filexor.h"
#include "geograph.h"

class MergeParams {
public:
	std::string	typeTranslation;
	std::string type;
	std::string speed;
	std::string roadClass;
	int	   adefaultType;
	std::string level;
	std::string endLevel;
	std::string label;
	std::string label3;
	std::string description;
	std::string city;
	std::string region;
	std::string country;
	std::string zip;
	std::string oneWay;
	std::string toll;
	std::string restrictionMPC;

	std::string dataRgnId;
	std::string StreetVE;
	std::string StreetVD;
	std::string StreetVC;
	std::string StreetVB;
	std::string StreetVT;
	std::string StreetVP;
	std::string StreetVI;
	std::string StreetVR;
	std::string RestrAttributes;

	std::string rgnId;
	std::string node;
	std::string nodeId;
	std::string bound;
	std::string leftCity;
	std::string leftRegion;
	std::string leftCountry;
	std::string rightCity;
	std::string rightRegion;
	std::string rightCountry;
	std::string leftZip;
	std::string rightZip;
	std::string leftStart;
	std::string leftEnd;
	std::string leftType;
	std::string rightStart;
	std::string rightEnd;
	std::string rightType;
	
	std::string nodeId1;
	std::string nodeId2;
	std::string nodeId3;
	std::string nodeId4;
	std::string roadId1;
	std::string roadId2;
	std::string roadId3;

	std::string zLevel;
	std::string zLevelStart;
	std::string zLevelEnd;

	std::string nonReferenceNode;
	std::string referenceNode;
	std::string funcClass;
	//link_id ->roadid

	//connection data
	std::string dataConnFile;
	std::string connRoadId; //link_id
	std::string	connNodeId; //reference node
	bool		navteq;
	bool		mapsource;
};

class ConfigReader {
	
public:
	static	double	CRITICAL_DISTANCE;
	static	double	DIFF_DISTANCE;
	static	int		ANGLE_DIFF;
	
	static	int		HIGHWAY1_MAX_ANGLE_DIFF;
	static	int		HIGHWAY2_MAX_ANGLE_DIFF;
	static	int		HIGHWAY3_MAX_ANGLE_DIFF;
	static	double	TOLERANCE_METERS;
	static	bool	ENUMERATE;
	static	bool	GENERATE_SPEED;
	static	bool	PRESERVE_ROAD_CLASS;
	static	Datum	WGS84;
	char	r_class1[25];
	char	r_class2[25];
	char	r_class3[25];
	char	r_class4[25];

	std::map<std::string,std::string> data;

	std::string	typeTranslation;
	std::string data_file;
	std::string dataType;
	int		dataDefaultType;
	std::string dataLabel;
	std::string dataLabel3;
	std::string dataSpeed;
	std::string dataClass;
	std::string dataDescr;
	std::string dataDirection;
	std::string dataCity;
	std::string dataRegion;
	std::string dataCountry;
	std::string dataZip;
	std::string dataOneway;
	std::string dataToll;
	std::string dataRoadID;
	std::string dataLevel;
	std::string dataEndLevel;
	std::string restrictionMPC;

	//Navteq routing data
	std::string zLevel;
	std::string nonReferenceNode;
	std::string referenceNode;
	std::string funcClass;
	//link_id ->roadid

	//MPC routing data
	std::string	zLevelStart;
	std::string	zLevelEnd;

	//connection data
	std::string dataConnFile;
	std::string connRoadId; //link_id
	std::string	connNodeId; //reference node
	bool		navteq;
	bool		mapsource;


	std::string StreetVE;//emergency
	std::string StreetVD;//delivery
	std::string StreetVC;//car
	std::string StreetVB;//bus
	std::string StreetVT;//taxi
	std::string StreetVP;//pedestrian
	std::string StreetVI;//bicycle
	std::string StreetVR;//truck

	std::string routing_file;
	std::string routingNode;
	std::string routingNodeID;
	std::string routingRoadID;
	std::string routingBound;

	std::string routingLeftStart;
	std::string routingLeftEnd;
	std::string routingLeftType;
	std::string routingRightStart;
	std::string routingRightEnd;
	std::string routingRightType;

	std::string routingLeftCity;
	std::string routingLeftRegion;
	std::string routingLeftCountry;
	std::string routingRightCity;
	std::string routingRightRegion;
	std::string routingRightCountry;

	std::string routingLeftZip;
	std::string routingRightZip;

	std::string restrAttributes;

	std::string restr_file;
	std::string restrNODID1;
	std::string restrNODID2;
	std::string restrNODID3;
	std::string restrNODID4;
	std::string restrRoadID1;
	std::string restrRoadID2;
	std::string restrRoadID3;

	ConfigReader(const char* file_name);

	void	ReadData(xor_fstream *ini_file);
	void	ReadRouting(xor_fstream *ini_file);
	void	ReadRestrictions(xor_fstream *ini_file);
	void	ReadRClass(xor_fstream *ini_file);
	void	ReadParameters(xor_fstream *ini_file);

	MergeParams	getMergeParams();
};