/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

#include <iostream>
#include "progress.h"

using namespace std;

int progress::percent = 0;

void progress::calculate(int position,int total) {
	float p = (float)position;
	float t = (float)total;
	if( int((p / t) * 100.0) != percent ) {
		percent = int((p / t)*100.0);
		cout<<"\r"<<percent<<"%"<<flush;
	}
}