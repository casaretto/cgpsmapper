/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

#ifndef __COMGPSH
#define __COMGPSH

#ifndef LINUX
#include <time.h>
//#include <conio.h>
#include <windows.h>
#endif

#ifdef LINUX
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#define byte unsigned char
#endif

#include <iostream>
#include "gps.h"

using namespace std;

class COM_interface : public transmission_gps {
private:
#ifndef LINUX
	DCB		dcb;
	HANDLE		hCom;
	DWORD		dwError;
	BOOL		fSuccess;
	COMMTIMEOUTS	commTimeOuts;
#else
	int		hCom;
	struct		termios	termParam;
	struct		termios	termSetParam;
	bool		fSuccess;
#endif

	int		readBufferFill;
	int		currentByteFill;
	char	buffer_in[0x1000]; //translacja
	char	buffer_out[0x1000];
	
	int		len;

protected:	 
	bool	read_data_com(int timeout);
	bool	get_message(int timeout);
	void	send_message(char *message, int bytes, char *msgtype);
	void	async_out(char *message, int bytes);
	int		async_in_stat();
	int		async_in();
	short	checksum(char *m, int l);

	int		read_data();
	int		current_speed;

protected:
	int		get_command_index() {return 1;};
	int		get_data_index() {return 3;};
	int		get_length_index() {return 2;};

	void	process_speed_change();
	int		last_result;

	int		send_delay;
	int		get_delay;
	int		speed_delay;
public:
	COM_interface(bool silent = false,int send_delay = 3,int get_delay = 3,int speed_delay = 100);
	virtual ~COM_interface();
	int  get_preffered_chunk_size() {return 0xf0;};

	void close_com();
	bool open_com(const char* com_port);

	bool get_basic_info();
	bool get_memory_info();

	void turn_off();
	bool erase_memory();
	bool set_async();
	bool set_speed(int speed);

	bool send_map_chunk(char* data,int size);
	bool finalize_upload();

	bool request_file_packet(char* file_name);
};

#endif
