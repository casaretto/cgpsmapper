/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

//#include <vld.h>
#include <iostream>
#include <boost/progress.hpp>
#include "preview.h"
#ifdef _WIN32
#include <windows.h> //GetModuleFileName
#endif
using namespace std;


/*
MDR_TRIM_FEAT.CXX-851-6.13.4.0 - nieprawid³owa tablica MDR1 - offsety MDR11 by³y z³e
MDR_TRIM_FEAT.CXX-399-6...	   - b³êdne rekordy w MDR9/10 (by³ z³y mdr11 ref size)
MDR_TRIM_FEAT.CXX-320-...      (prawdopodobnie to samo co 301)
MDR_TRIM_FEAT.CXX-301-         - b³êdna kolejnoœæ MDR14

MDR_TRIM_ADDR.CXX-346-..       - zmiana w sortowaniu danych w MDR13 - bylo: ORDER BY MDR1.id, T53_REGIONY.t_text
                                                                 zmiana na: ORDER BY MDR1.id, T53_REGIONY.t_text, T4E_KRAJE.t_text


*/

preview* Preview;

int main (int argc,char* argv[]) {
	//_CrtSetBreakAlloc(954);
	//_CrtSetBreakAlloc(988684);
	bool	sign_tdb = false;
	bool	by_region = false;
	bool	multibody = false;
	bool	idx_ignore = false;
	bool	pvx_mode = false;
	string	base_name = "";

	cout<<"*******************************************************************************\n";
	cout<<"              cGPSMapper home page: http://cgpsmapper.com \n";
	cout<<"*******************************************************************************\n";
	cout<<"              Preview & index builder for cGPSmapper - 6.9\n\n";
	//cout<<"Preview builder is creating TDB preview file and MP preview file.\n";
	//cout<<"You must compile MP file with cGPSmapper program separately to get preview IMG!\n\n";

	if( argc < 2 ) {
		cout<<"Use:\n\tcpreview pv.txt [-r] [-m]\n";
		cout<<"\t-r  Main search done by region (default - by country)"<<endl;
		cout<<"\t-m  Indexing each word separately for street names"<<endl;

		//cout<<" -nMDR	- no MDR file generated"<<endl;
		return 2001;
	}

	if( argc > 2 ) {
		int t_arg = 2;
		while( t_arg < argc ) {
			if( argv[t_arg][0] == '-' ) {
				if(argv[t_arg][1] == 't') sign_tdb = true;
				if(argv[t_arg][1] == 'r') by_region = true;
				if(argv[t_arg][1] == 'm') multibody = true;
				if(argv[t_arg][1] == 'b') base_name = "mdr_base";
				if(argv[t_arg][1] == 'i') idx_ignore = true;
				if(argv[t_arg][1] == 'x') pvx_mode = true;
			}
			t_arg++;
		}
	}

	if( sign_tdb ) {
		TDB tdb(0,"","",0,false,false);
		tdb.SignTDB(argv[1]);
		return 0;
	}

	Preview = new preview(base_name,idx_ignore);
	if( Preview->openConfig(argv[1])) {
		boost::progress_timer t;
		char	program_exe[2000];
		char	temp[256];
		string	s_temp;
#ifdef _WIN32
		GetModuleFileName(NULL,program_exe,2000);
#else
		strncpy(program_exe,argv[0],1999);
#endif
/*
#ifndef _DEBUG
		GetEnvironmentVariable("VERSION", temp, 255);
		s_temp = temp;
		if( s_temp == "ROUTABLE" || s_temp == "PRO" || s_temp == "PERSONAL" )
#endif
			mdr_create = true;
*/
		Preview->setHomePath(program_exe);
		//mdr_create ustawione przed TDB maker - jesli s¹ sekcje NET - bêdzie indeksowanie
		Preview->run(by_region,multibody,pvx_mode);

		Preview->processFiles();
		cout<<endl;
	}
	delete Preview;
	return 0;
};
