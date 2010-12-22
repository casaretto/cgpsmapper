/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

//#include <vld.h>
#include <windows.h>
#include "merge.h"
#include "config.h"

#define __FREE 0

using namespace std;

//TODO:
//Regarding maproute - I'll try to implement freezing of bound point to.
//przy upraszczaniu nie przesuwac bound points!!

int main(int argc, char* argv[])
{
	int checkLimit;
	/*

	Tablica przechowuje informacje dla kazdego elementu czy juz zostal przetworzony

	1. Laczenie elementow

	2. Detekcja skrzyzowan

	Ad 1.

	- Otwarcie pliku wejsciowego
	
	a.
	- Pobranie kolejnego elementu
	- Przeszukanie wszystkich nieprzetworzonych, czy stanowia kontynuacje danego elementu
	 - jezeli stanowi kontynuacje, modyfikacja obkektu tymczasowego i wywolanie a.

    - zapisz tymczasowy jako nowy obiekt
    - jezeli zostaly jakies do przetworzenia - idz do a.

	Ad 2.
	Detekcja skrzyzowan - podobnie do tego co juz mam


	*/
	string tcopy;
	tcopy +="*******************************************************************************\n";
	tcopy +="                         HTTP://CGPSMAPPER.COM\n";
	tcopy +="*******************************************************************************\n";
	tcopy +="cGPSmapper a Generic routing generator\n";
	tcopy +="Copyright(C) Stanislaw Kozicki, 2000-2010\n";
	tcopy +="cgpsmapper@gmail.com\n";
	tcopy +="*******************************************************************************\n";
	tcopy +="Version 7.5\n";
	tcopy +="*******************************************************************************\n";


	cout<<tcopy;

	if( argc < 2 ) {
		tcopy +="Parameters:\n";
		tcopy +="   config_file_name \n";
		tcopy +="   operation (coded) ( 7 will do all ):\n";
		tcopy +="     1 - Merge and correct wrong elements\n";
		tcopy +="     2 - Generate generic routing table\n";
		tcopy +="     4 - Bounds detection\n";
		tcopy +="     8 - Polish Format output (default is ESRI)\n";
		tcopy +="     16 - Routing class check only\n";
		tcopy +="     32 - No generalisation\n";
		tcopy +="     64 - No routing check (use ONLY if routing is correct at input!)\n";

		tcopy +="Example:\n";
		tcopy +="MapRoute configuration.ini 3";

		MessageBox(NULL,tcopy.c_str(),"cGPSmapper",MB_ICONASTERISK | MB_OK);
		return 1;
	}

	string lInput = "MapRoute.ini";
	string lOperation = "7";

	int operation;

	if( argc > 1 ) lInput = argv[1];
	if( argc > 2 ) lOperation = argv[2];

	operation = atoi( lOperation.c_str() );

	ConfigReader cf(lInput.c_str());

	LineMerge lmerge;
	if( cf.data_file.find(",") < cf.data_file.size() ) {
		string next_file = cf.data_file.substr(cf.data_file.find(",")+1);
		cf.data_file = cf.data_file.substr(0,cf.data_file.find(","));
		cout<<"Multiply input file defined."<<endl;
	
		while( cf.data_file.size() ) {
			lmerge.DefineData(cf.data_file.c_str(),
				cf.routing_file.c_str(),
				cf.restr_file.c_str(),
				cf.dataConnFile.c_str(),
				cf.getMergeParams(),&cf);
			lmerge.ReadInput( true /*!(operation & 2)*/, cf.navteq, cf.mapsource );

			cf.data_file = next_file.substr(0,next_file.find(","));
			if( next_file.find(",") < next_file.size() )
				next_file = next_file.substr(next_file.find(",")+1);
			else
				next_file = "";
		}
	} else {
		lmerge.DefineData(cf.data_file.c_str(),cf.routing_file.c_str(),
			cf.restr_file.c_str(),cf.dataConnFile.c_str(),cf.getMergeParams(),&cf);
		lmerge.ReadInput( true /*!(operation & 2)*/, cf.navteq, cf.mapsource );
	}

	//wczytanie do 'restrykcji' danych w formacie MPC - 
	//- automatycznie nada punkty nodId - jeszcze przed dedekcj¹ 
	//- nie ma to znaczenia - bo dedekcja zachowa je niezmienione
	lmerge.DetectRestrictionMPC();

	if( operation & 16 ) {
		lmerge.Enumerate();
		lmerge.InitialSorting();
		lmerge.SetRoutingClass(); //tablica
	} else {
		lmerge.Split(operation);
		if( operation & 1 ) {
			lmerge.InitialSorting();
			if( (operation & 2) && !cf.navteq )	
				lmerge.Enumerate();
			else {
				//sprawdzenie routingu
				if( !(operation & 64) ) {
					cout<<"Routing data has beein imported - now checking all the connection. This may take a while."<<endl;
					if( lmerge.CheckNodID() == false )
						cout<<"Some errors found in the routing data - correct them before creating final map!"<<endl;
				}
			}
			lmerge.Merge(&lmerge.input_data);
		}

		lmerge.Enumerate();
		lmerge.InitialSorting();
		lmerge.SetEndingNodes();

		if( (operation & 2) && !cf.navteq )	{
			lmerge.NodDetection(cf.mapsource);

			if( operation & 4 )
				lmerge.BoundsDetection();

			//czy nie przekraczaja 60 ?
			lmerge.Split(operation);
			lmerge.CreateTables(); //tablica
		}
		lmerge.UpdateRestrictionsID();
		//lmerge.TranslateMPCRestrictions();

		if( !(operation & 32) )	{
			lmerge.CheckNodes();
			lmerge.SmoothInput();
		}

		checkLimit = 0;
		do {
			cout<<"Checking nodes..."<<endl;
			checkLimit++;
		} while( lmerge.CheckNodes() && checkLimit < 50);

		if( operation & 4 && !(operation & 2) ) {
			lmerge.BoundsDetection();
			checkLimit = 0;
			do {
				cout<<"Checking nodes..."<<endl;
				checkLimit++;
			} while( lmerge.CheckNodes() && checkLimit < 50);
		}

		lmerge.SetRoutingClass(); //tablica
	}
	lmerge.FinalSorting();

	lmerge.RemoveEmptySegments();

	lmerge.RemoveZeroLength();

	if( operation & 8 ) {
		lmerge.ExportMP();
	} else {
		lmerge.ExportLines();
		if( !(operation & 16) )
			lmerge.ExportSegments();
	}

	lmerge.ExportMapping();
	
	return 0;
}

