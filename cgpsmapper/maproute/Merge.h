/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

#pragma once

#include <vector>
#include <map>
#include <string>
#include "typeTranslation.h"
#include "line.h"
#include "Config.h"

class RgnMapping {
public:
	__int64	oryginalRgnId;
	__int64	newRgnId;

	RgnMapping(__int64 oryginalRgnId,__int64 newRgnId) {
		this->newRgnId = newRgnId;
		this->oryginalRgnId = oryginalRgnId;
	};

};

class Restriction {
public:
	__int64	nodeId1;
	__int64	nodeId2;
	__int64	nodeId3;
	__int64	nodeId4;
	__int64	RoadId1;
	__int64	RoadId2;
	__int64	RoadId3;

	//tylko dla MPC - pomocnicze
	bool	firstVertex;

	Restriction() {
		this->nodeId1 = -1;
		this->nodeId2 = -1;
		this->nodeId3 = -1;
		this->nodeId4 = -1;
		this->RoadId1 = -1;
		this->RoadId2 = -1;
		this->RoadId3 = -1;
	};

	Restriction(__int64 nodeId1,__int64 nodeId2,__int64 nodeId3,__int64 nodeId4,__int64 RoadId1,__int64 RoadId2,__int64 RoadId3) {
		this->nodeId1 = nodeId1;
		this->nodeId2 = nodeId2;
		this->nodeId3 = nodeId3;
		this->nodeId4 = nodeId4;
		this->RoadId1 = RoadId1;
		this->RoadId2 = RoadId2;
		this->RoadId3 = RoadId3;
	};
};

class RoutingNode {
public:
	Coords	coord;
	__int64		nodeId;
	int		bound;
	std::vector<__int64>	roadIds;

	RoutingNode() {
	}

	RoutingNode(Coords coord,__int64 nodeId) {
		this->coord = coord;
		this->nodeId = nodeId;
		bound = 0;
	}
};

class LineMerge {

private:
	ConfigReader* cf;
	typeReader	TR;

	int		defaultType;
	int		idxRgnId;
	int		idxNode;

	int		idxLevel;
	int		idxEndLevel;
	int		idxLeftCity;
	int		idxLeftRegion;
	int		idxLeftCountry;
	int		idxRightCity;
	int		idxRightRegion;
	int		idxRightCountry;
	int		idxLeftZip;
	int		idxRightZip;
	int		idxLeftStart;
	int		idxLeftEnd;
	int		idxLeftType;
	int		idxRightStart;
	int		idxRightEnd;
	int		idxRightType;
	int		idxStreetVE;
	int		idxStreetVD;
	int		idxStreetVC;
	int		idxStreetVB;
	int		idxStreetVT;
	int		idxStreetVP;
	int		idxStreetVI;
	int		idxStreetVR;
	int		idxRestrAttributes;

	int		mainidxRgnId;
	int		idxNodeId;
	int		idxBound;
	int		idxType;
	int		idxSpeed;
	int		idxLabel;
	int		idxLabel3;
	int		idxDescription;
	int		idxCity;
	int		idxRegion;
	int		idxCountry;
    int		idxZip;
	int		idxOneWay;
	int		idxToll;
	int		idxRoadClass;
	int		idxRestrictionMPC;

	int		idxNodeId1;
	int		idxNodeId2;
	int		idxNodeId3;
	int		idxNodeId4;
	int		idxRoadId1;
	int		idxRoadId2;
	int		idxRoadId3;

	int		idxZLevel;
	int		idxZLevelStart;
	int		idxZLevelEnd;
	int		idxnonReferenceNode;
	int		idxreferenceNode;
	int		idxfuncClass;
	//connection data
	//std::string dataConnFile;
	int		idxconnRoadId; //link_id
	int		idxconnNodeId; 


	int		baseRoadClass;
	int		baseRoadType;

	//void	ChangeNode(int originalNodeId,int toChangeNodeId);
	bool	ChangeCoords(Coords oldCords,Coords newCords);//current erased ->true
	void	Merge(std::vector<Line*> *local_input_data,std::vector<Line*>::iterator line_i,bool read_merge = false);
	void	Split(std::vector<Line*>::iterator line_i,int operation);

	void	CheckIntersection(std::vector<Line*>::iterator line_i,std::vector<Line*>::iterator line_j,int i_coord,bool check_zlevels);
	void	CheckIntersectionElement(std::vector<Line*>::iterator line_i,bool mapsource); //mapsource - porowuje zlevelstart / end
	void	CheckEndingElement(std::vector<Line*>::iterator line_i);
	
	bool	CheckNodID(Coords intersectionPoint,__int64 nodId,__int64 roadId);
	
	void	UpdateNodeOfRoad(std::vector<Line*>::iterator line_i,bool force = false);

	__int64		GetNodID(__int64 realNodID); // jesli ENUMERATE to zmienia
	__int64		GetRoadID(__int64 realNodID); // jesli ENUMERATE to zmienia

	int		RCchangedElements;
	int		RCremainingElements;

	__int64	NodesNumber;
	std::map<__int64,int>	HighRoadClass;
	std::map<__int64,int>	LowRoadClass;
	std::map<__int64,int>	NodeIDUsage;//ilosc uzyc przez RgnId
	__int64	array_size;

	void	ShowProgress(int all,int current);
	int		ReadBool(DBFHandle dbf,int fieldIdx,int record,bool withNegative = false);
	int		ReadAddressType(DBFHandle dbf,int record,int fieldIdx);

	int		SetRoadClass(int type,bool in_city);
	int		SetSpeedClass(int type);
	int		progress;
	__int64	highestRgnId;
	__int64	highestOrder;

	//mapa - nowy rgnId, oryginalny rgnId
	bool	repeatMerge;

	void	AddMapping(__int64 originalId,__int64 newId);
	__int64	GetNewRoadID(__int64 originalId);
	std::vector<RgnMapping> rgnIdMapping;
	std::vector<RgnMapping>::iterator	rgnIdMappint_iter;
	std::vector<Restriction> rgnRestriction;

	//map<int,int> rgnIdMap;
	//map<int,int>::iterator rgnIdMap_iter;
public:
	static __int64	nextNodeId;
	static	std::map<__int64,__int64> nodIdMapping;

	//config variables 
	std::vector<Line*>	input_data;
	std::map<__int64,RoutingNode>	node_data;

	Coords		minBox,maxBox;

	SHPHandle	input_shp;
	DBFHandle	input_dbf;
	DBFHandle	input_routing_dbf;
	DBFHandle	input_connection_dbf; //navteq
	DBFHandle	input_restriction_dbf;

	LineMerge();

	void	DefineData(std::string input_file_name,std::string routing_dbf_file,std::string restriction_dbf_file,std::string connection_dbf_file,
		MergeParams mergeParams,ConfigReader* cf);
	~LineMerge();

	bool	ReadInput(bool readRouting,bool navteq,bool mapsource);
	bool	Merge(std::vector<Line*> *local_input_data,bool read_merge = false);

	void	InitialSorting();
	void	FinalSorting();

	void	Enumerate();
	void	SmoothInput();
	void	Split(int operation);
	void	CreateTables();

	void	NodDetection(bool mapsource);
	void	SetEndingNodes();
	bool	CheckNodes(); //sprawdzenie (i redukcja) wezlow tylko
	bool	CheckNodID(); //sprawdzenie czy skrzyzowania sie 'krzyzuja'

	void	DetectRestrictionMPC();
	__int64		SetSameNodeID(Line* line_1,bool start_1,Line* line_2 = NULL,bool start_2 = false);

	void	UpdateRestrictionsID();
	//void	TranslateMPCRestrictions();
	void	BoundsDetection();
	void	RemoveZeroLength();
	void	RemoveEmptySegments();

	void	SetRoutingClass();

	void	ExportLines();
	void	ExportSegments();
	void	ExportMapping();

	void	ExportMP();
};