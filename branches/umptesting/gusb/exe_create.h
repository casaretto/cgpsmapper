/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

# ifndef __exe_create_H_
# define __exe_create_H_
#ifdef _WIN32
#include <string>
#include <windows.h>

#ifndef _DLL_VERSION

extern long crc_calc(char* buffer,int size,unsigned long gps_id);
extern void xor_data(char* buffer,int size,unsigned int gps_id);

int create_exe_package(unsigned int gps_id,int exp_day,int exp_month,int exp_year,char sendmap_exe[2024],
						std::string copyright_string,std::string licence_file,std::string expired_file,std::string sendmap_exe_img,char* temp_file=NULL);
#endif
#endif
#endif
