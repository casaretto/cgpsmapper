/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

#include "com_gps.h"
#include <stdexcept>
#ifdef LINUX
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#endif

char	r_poweroff[] =	{ "\x0a\x02\x08\x00" };
char	r_finish[] =	{ "\x0a\x02\x3a\x00" };
char	r_abort[] =		{ "\x0a\x02\x00\x00" };
char	r_ack1[] =		{ "\x06\x02\xff\x00" };
char	r_nak1[] =		{ "\x15\x02\xff\x00" };
char	r_m1[] =		{ "\xfe\x01\x20" };			// module info
char	r_m2[] =		{ "\xfe\x00" };

char	r_async[] =		{ "\x1c\x02\x00\x00" };		// Async OFF
char	r_setboud[] =	{ "\x30\x04\x00\x00\x00\x00" };

char	r_polling[] =	{ "\x0a\x02\x3a\x00" };
char	r_mapflash[] =	{ "\x0a\x02\x3f\x00" };		// MapFlash description...
char	r_serialid[] =	{ "\x0a\x02\x0e\x00" };
char	r_avalbaud[] =	{ "\x0a\x02\x39\x00" };		// Dostepne baud rates...
char	r_erasemap[] =	{ "\x4b\x02\x00\x00" };		// erase
char	r_burnmap[] =	{ "\x2d\x02\x00\x00" };		// burn

char	r_getfile[] =	{ "\x59\x13\x00\x00\x00\x00\xFF\xFF\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" };
char	r_reqdump[] =	{ "\x6c\x08\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF" };

#define __HISTORY 0

#if __HISTORY == 1
char	history[0x100][0x1000];
int		history_pos;
#endif

COM_interface::COM_interface(bool silent,int send_delay,int get_delay,int speed_delay) 
  : transmission_gps(silent) {
	current_speed = 9600;
	readBufferFill = 0;
	this->send_delay = send_delay;
	this->get_delay = get_delay;
	this->speed_delay = speed_delay;
#if __HISTORY == 1
	memset(history,0x100*0x1000,0);
	history_pos = 0;
#endif
}


COM_interface::~COM_interface() {
	close_com();
}

bool COM_interface::open_com(const char* com_port) {
#ifndef LINUX
	hCom = CreateFile(com_port,GENERIC_READ | GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL);
	if(hCom == INVALID_HANDLE_VALUE) {
		dwError = GetLastError();	
		return false;
	}
	delay(10);

	fSuccess = GetCommState(hCom, &dcb);
	if(!fSuccess) {
		close_com();
		return false;
	}

	dcb.BaudRate = 9600;
	dcb.ByteSize = 8;
	dcb.Parity = FALSE;
	dcb.StopBits = ONESTOPBIT;
	dcb.XonLim = 4086;
	dcb.XoffLim = 4086;

	fSuccess = SetCommState(hCom, &dcb);
	if(!fSuccess) {
		close_com();
		return false;
	}

	commTimeOuts.ReadIntervalTimeout = 0;
	commTimeOuts.ReadTotalTimeoutConstant = 0;
	commTimeOuts.ReadTotalTimeoutMultiplier = 0;
	commTimeOuts.WriteTotalTimeoutMultiplier = 0;
	commTimeOuts.WriteTotalTimeoutConstant = 0;

	SetCommTimeouts(hCom, &commTimeOuts);
	SetupComm(hCom, 4096, 4096);
	PurgeComm(hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
#else
	/* LINUX */
	hCom = open(com_port, O_RDWR | O_NOCTTY );
	if(hCom <= 0) {
		return false;
	}

	tcgetattr(hCom, &termParam);
	memset(&termSetParam, 0, sizeof(termSetParam));

	termSetParam.c_cflag |= ( CLOCAL | CREAD);
	cfsetispeed(&termSetParam, B9600);
	cfsetospeed(&termSetParam, B9600);

	termSetParam.c_cflag &= ~PARENB;
	termSetParam.c_cflag &= ~CSIZE;
	termSetParam.c_cflag |= CS8;

	termSetParam.c_cflag &= ~CSTOPB;

	tcsetattr(hCom, TCSADRAIN, &termSetParam);

	fcntl(hCom, F_SETFL, FNDELAY);
#endif
	gps_communication_ok = true;	
	return true;
}

void COM_interface::close_com() {
#ifndef LINUX
	try {
		if( hCom != INVALID_HANDLE_VALUE )
			CloseHandle(hCom);
		hCom = INVALID_HANDLE_VALUE;
	} catch (...) {
		//I don't care NOW
	}
#else
	close(hCom);
#endif
}

bool COM_interface::request_file_packet(char* file_name) {
	for(int a = 8; a < 8 + 12; a++) r_getfile[a] = 0;
	memcpy(&(r_getfile[6]), &flash_id, 2);
	memcpy(&r_getfile[8], file_name, strlen(file_name));
	send_message(r_getfile, 0x15, "");
	return true;
}

bool COM_interface::erase_memory() {
	bool result = false;
	int tries = 0;
	map_address = 0;

	memcpy(&(r_erasemap[2]), &flash_id, 2);
	send_message(r_erasemap, 4, "memory");
	while( tries < 50 && got_memory_erased == false ) {
		read_data_com(1);
		tries++;
		delay(10);
#ifndef _DLL_VERSION
		cout<<".";
#endif
	};
	return got_memory_erased;
}

bool COM_interface::send_map_chunk(char* data,int size) {
	int read_pos,write_pos;
	int last_correct_read_pos;
	int last_correct_write_pos;
	int tries;
	if( size > 0x1000 )
		return false;
	//konwersha 0x10 na 0x10 0x10 moze spowodowac zwiekszenie ilosci danych!
	//adres docelowy musi byc parzysty!
	//wysylana ilosc danych musi(?) byc parzysta!

	//wypelniam bajtami po kolej pilnujac warunkow
	read_pos = 0;
	while( read_pos < size ) {
		last_correct_write_pos = 0;
		write_pos = 1 + 1 + 4; //command + size + address
		while( write_pos < 254 && read_pos < size) {
			send_buffer[write_pos] = data[read_pos];
			read_pos++;
			write_pos++;
			if( (read_pos % 2) == 0 ) {
				//standard
				last_correct_read_pos = read_pos;
				last_correct_write_pos = write_pos;
			} else if ( write_pos == 1 ) {
				//if first byte
				last_correct_read_pos = read_pos;
				last_correct_write_pos = write_pos+1;
				send_buffer[write_pos] = 0;
			} else {
				last_correct_read_pos = read_pos-1;
				last_correct_write_pos = write_pos-1;
			}
		}

		send_buffer[0] = 0x24;
		send_buffer[1] = (last_correct_write_pos-2);
		int_conv.value = map_address;
		memcpy(&(send_buffer[2]),int_conv.value_c,4);

		tries = 0;
		do {
			send_message(send_buffer,last_correct_write_pos,"upload");
			get_message(2);
			tries++;
		} while( tries < 4 && last_result != 0x06 );
		if( last_result != 0x06 )
			return false;

		map_address += last_correct_write_pos-6;		
		read_pos = last_correct_read_pos;
	}

	return true;
}

bool COM_interface::finalize_upload() {
	int tries = 0;
	memcpy(&(r_burnmap[2]), &flash_id, 2);
	send_message(r_burnmap, 4, "burn");
	while( tries < 10 ) {
		read_data_com(1);
		if( last_result == 0x06 )
			return true;
		tries++;
		delay(10);
	};
	return false;
}
void COM_interface::turn_off() {
	send_message(r_poweroff, 4, "memory");
}
bool COM_interface::get_memory_info() {
	int tries = 0;
	send_message(r_mapflash, 4, "memory");
	while( tries < 5 && got_memory == false ) {
		read_data_com(1);
		tries++;
		delay(3);
	};
	return got_memory;
}

bool COM_interface::get_basic_info() {
	int tries = 0;

	send_message(r_abort, 4, "abort");	//initiate connection
	if( read_data_com(4) == false ) {
		return false;
	}

	send_message(r_m2, 2, "info");
	read_data_com(4);

	send_message(r_m1, 3, "info");
	read_data_com(4);

	//read if anything more in the queue
	//that should include protocol array
	read_data_com(1);

	send_message(r_serialid, 4, "get id");
	tries = 0;
	while( tries < 5 && GPS_id == 0 ) {
		read_data_com(1);
		tries++;
		delay(3);
	};
	return true;
}

void COM_interface::process_speed_change() {
	unsigned int	baud;
#ifdef LINUX
	speed_t		selectedBaud = B9600;
#endif
	baud = convert_to_int(3);

	//delay(5);
#ifndef LINUX
	fSuccess = GetCommState(hCom, &dcb);
	if(!fSuccess) {
#ifndef _DLL_VERSION
		cout << "Could not change speed" << endl;
#endif
		throw runtime_error("Could not change speed!");
	}
#endif
	if((baud > 110000) && (baud < 120000)) {
#ifndef LINUX
		dcb.BaudRate = 115200;
#else
		selectedBaud = B115200;
#endif
	}

	if((baud > 50000) && (baud < 60000)) {
#ifndef LINUX
		dcb.BaudRate = 57600;
#else
		selectedBaud = B57600;
#endif
	}

	if((baud > 30000) && (baud < 40000)) {
#ifndef LINUX
		dcb.BaudRate = 38400;
#else
		selectedBaud = B38400;
#endif
	}

	if((baud > 15000) && (baud < 25000)) {
#ifndef LINUX
		dcb.BaudRate = 19200;
#else
		selectedBaud = B19200;
#endif
	}

	if((baud > 8000) && (baud < 12000)) {
#ifndef LINUX
		dcb.BaudRate = 9600;
#else
		selectedBaud = B9600;
#endif
	}

#ifndef LINUX
	current_speed = dcb.BaudRate;
#else
	current_speed = selectedBaud;
#endif

	delay(speed_delay);
#ifndef LINUX
	fSuccess = SetCommState(hCom, &dcb);
#else
	fSuccess = false;
	cfsetospeed(&termSetParam, selectedBaud);
	cfsetispeed(&termSetParam, selectedBaud);

	if( tcsetattr(hCom, TCSAFLUSH, &termSetParam) != -1 )
		fSuccess = true;
#endif
	if(fSuccess == false) {
#ifndef _DLL_VERSION
		cout << "Error changing speed - trying to restore communication..." << endl;
#endif
		//throw runtime_error("Error changing speed!");
		got_error_speed_change = true;
		delay(3);
		return;
	}
	send_message(r_polling, 4, "Polling");
	if(get_message(20) == false) {
#ifndef _DLL_VERSION
		cout << "Can't change the speed - trying to restore communication..." << endl;
#endif
		//throw runtime_error("Can't change the speed!");
		got_error_speed_change = true;
		delay(3);
		return;
	} else
		got_speed_change = true;
	send_message(r_polling, 4, "Polling");
	get_message(2);	// ack
	send_message(r_polling, 4, "Polling");
	get_message(1);	// ack
	cout<<"Speed change OK"<<endl;
}

bool COM_interface::set_async() {
	int tries = 0;
	bool result = false;
	send_message(r_async, 4, "async");
	while( tries < 2  ) {
		if( read_data_com(1) ) {
			return true;
		}
		tries++;
	}
	return false;
}
bool COM_interface::set_speed(int speed) {
	int tries = 0;

	got_speed_change = false;
	if( speed <= 9600 && current_speed == 9600 )
		return true;

	memcpy(&r_setboud[2], &speed, 4);
	send_message(r_setboud,6,"speed");
	tries = 0;
	while( tries < 5 && got_speed_change == false && got_error_speed_change == false ) {
		read_data_com(1);
		tries++;
		delay(3);
	};
	return got_speed_change;
}

/* wait and read garmin message */
bool COM_interface::read_data_com(int timeout) {
	if( get_message(timeout) ) {
		return true;
	}
	return false;
}

int COM_interface::read_data() {
	int tries = 0;
	while( tries < 10  ) {
		if( read_data_com(1) ) {
			return last_result;
		}
		tries++;
	}
	return false;
}

bool COM_interface::get_message(int timeout) {
	short	fails, GPSchecksum;
	time_t	t;
	char	*p,c, last;
	short	length, ne;
	int		Garmin_NAKS_Sent = 0;
	bool	processed = false;

	p = buffer;
	length = len = received_length = ne = last = 0;
	memset(buffer, 0, 0x1000);
	t = time(NULL);
	fails = 0;


redoit:
	for(;;) {
		if((time(NULL) - t) > timeout) {
			if(++fails > 3 ) {
				return false;
			}
		}

		delay(get_delay);

		while(async_in_stat()) {

			fails = 0;

			c = async_in();
			t = time(NULL); /* reset time */

			if(c == 0x03 && last == 0x10)
				ne = 1;
			else
				ne = 0;

			if(c == 0x10 && last == 0x10)
				last = 0;
			else {
				last = *p++ = c;
				++length;
			}

			if(*(p - 1) == 0x03 && *(p - 2) == 0x10 && ne) {
				GPSchecksum = checksum(buffer, length);
#if __HISTORY == 1
				if( history_pos == 6 )
					history_pos = 6;
				memcpy(history[history_pos++],buffer,length);
				if( history_pos>= 0x100 )
					history_pos = 0;
#endif
				if(GPSchecksum == 0) {
					send_message(r_nak1, 4, "NAK");	/* send NAK */
					readBufferFill = 0;
					p = buffer;
					length = len = received_length = ne = last = 0;
					memset(buffer, 0, 0x1000);
					delay(10);
					if(++Garmin_NAKS_Sent < 7) 
						goto redoit;
					//cout << "Too Many NAKS in reading from GPS, aborting\n";
					throw runtime_error("Too Many NAKS in reading from GPS, aborting");
					error_code = errTooManyNAK;
				}

				r_ack1[2] = buffer[1]; // setup proper ack response
				if(buffer[1] != 0x06) {		// Don't ACK an ACK!
					//if more data waiting - do not send!
					if( !async_in_stat() )
						send_message(r_ack1, 4, "ACK");	/* send ACK */
				}
//				else if( verboseMode ) {
//					cout<<"got ACK for 0x"<<hex<<int(buffer[3])<<dec<<endl;
//				}
				len = received_length = length;
				last_result = parse_message();
				processed = true;
				if( last_result == 0x5a )
					received_length = (static_cast<unsigned char>(buffer[2])) + 3;

				length = 0;
				memset(buffer,0,0x1000);
				p = buffer;

				//for some cases BREAK reading to give time for analysis...
				if( last_result == 0x5b || last_result == 0x5a )
					return true;
			}
		}

		if( processed && (length == 0))
			return true;
	}
	return false;
}


/* send the garmin formatted message */
void COM_interface::send_message(char *message, int bytes, char *msgtype) {
	short	i;
	int		n;
	char	csum = 0;
	char	*p = buffer_out;

	*p++ = 0x10;				/* Start of message */
	for(i = 0; i < bytes; i++) {
		*p++ = message[i];
		csum -= message[i];
		if(message[i] == 0x10)
			*p++ = 0x10;
	}

	// Add in checksum
	*p++ = csum;
	if(csum == 0x10)
		*p++ = 0x10;

	// Add frame trailer
	*p++ = 0x10;
	*p++ = 0x03;

	n = (int) (p - buffer_out);

	if(((buffer_out[1] & 0x0f) == 0x0b) && ((buffer_out[1] & 0xf0) == 0x20)) {
#ifdef _DEBUG
	cout<<"Panic exit!"<<endl;
#endif
		throw runtime_error("Panic exit");
	}

	async_out(buffer_out, n);
	delay(send_delay);
	
#if __HISTORY == 1
	buffer_out[0] = 0xff;
	memcpy(history[history_pos++],buffer_out,n);
	if( history_pos>= 0x100 )
		history_pos = 0;
#endif
}


void COM_interface::async_out(char *message, int bytes) {
	unsigned long	written;
#ifndef LINUX
	WriteFile(hCom, message, bytes, &written, NULL);
	FlushFileBuffers(hCom);
#else
	written = write(hCom, message, bytes);
#endif
}

int COM_interface::async_in_stat(void) {
	unsigned long		bytes;
#ifndef LINUX
	COMSTAT			b;
#endif
	unsigned long	along;


	if(readBufferFill && currentByteFill < readBufferFill) {
		return readBufferFill;
	}

	currentByteFill = 0;
	readBufferFill = 0;
	memset(buffer_in,0,0x1000);

#ifndef LINUX
	ClearCommError(hCom, &along, &b);
	if(b.cbInQue > 0) {
		readBufferFill = b.cbInQue < 4086 ? b.cbInQue : 4086;
		ReadFile(hCom, &buffer_in, readBufferFill, &bytes, NULL);
		return bytes;//b.cbInQue;
	}

#else
	{
		readBufferFill = read(hCom, &buffer_in, 4086);
		if(readBufferFill > 0)
			return readBufferFill;
	}
#endif
	readBufferFill = 0;
	return 0;
}

int COM_interface::async_in(void) {
	unsigned char	c = 0;

	if(readBufferFill > 0 && currentByteFill < readBufferFill) {
		//cout<<currentByteFill;
		return buffer_in[currentByteFill++];
	}

	return c + 1000;
}

short COM_interface::checksum(char *m, int l) {
	short		i;
	unsigned char	cs;

	cs = 0;
	for(i = 1; i < l - 3; i++) {
		cs -= m[i];
	}
	if(cs == (unsigned char)m[l - 3])
		return 1;
	return 0;
}

