/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/
#include "gps.h"
#include "single_map.h"
#include <string.h>

transmission_gps::transmission_gps(bool silent) {
	this->silent = silent;
	got_protocol = false;
	got_memory = false;
	got_memory_erased = false;
	flash_id = 0x0a;
	GPS_map_capable = false;
	GPS_routable = false;
	GPS_unlock_capable = false;


	gps_found = false;
	gps_communication_ok = false;
	got_speed_change = false;
	got_error_speed_change = false;

	GPS_id = 0;
	comm_medium = resCOM;
	error_code = errNo;
}

bool transmission_gps::request_file(char* file_name,char* tmp_file,internal_file* i_file,int &total_size,int &position,bool buffer_data,int subfile_start) {
	unsigned int	packets_no;
	unsigned char	current_packet;
	unsigned char	last_packet = 0;
	int				last_result = 0;
	FILE*			file;
	char			file_buffer[16];
	progress		instance;
	int				last_size = 0;

	if( i_file == NULL )
		strcpy(file_buffer,file_name);
	else
		strcpy(file_buffer,i_file->get_internal_filename().c_str());

	if( i_file ) {
		i_file->set_offset(0);
		file = fopen(tmp_file,"rb");
		if( file != NULL ) {
			fseek(file,0,SEEK_END);

			last_size = ftell(file);
			i_file->set_offset(ftell(file));
			i_file->file_name = string(tmp_file);
			fclose(file);
		}
	}
	file = fopen(tmp_file,"r+b");
	if( file == NULL )
		file = fopen(tmp_file,"w+b");

	if( subfile_start > 0 ) {
		//subfile_start -= last_size;
		fflush(file);
		fseek(file,subfile_start,SEEK_SET);
	} else if( i_file ) {
		if( i_file->get_offset() ) {
			fflush(file);
			fseek(file,i_file->get_offset(),SEEK_SET);
		}
	}
#ifdef _DEBUG
	cout<<"Packet data start"<<endl;
#endif

	if( request_file_packet(file_buffer) == true ) {
		while( last_result = read_data() ) {
			if( last_result == 0x5c || last_result == 0) {
				fclose(file);
#ifdef _DEBUG
				cout<<"No initiate packet wrong"<<endl;
#endif
				return false;
			}
			if( last_result == 0x5b )
				break;

		}
		if( last_result != 0x5b ) {
			fclose(file);
#ifdef _DEBUG
			cout<<"No packet"<<endl;
#endif
			return false;
		}
	
		packets_no = received_buffer_convert_to_int(get_data_index());

		while(packets_no) {
			read_data();
			current_packet = receive_buffer[get_data_index()];
			if( last_packet != current_packet || receive_buffer[get_command_index()] != 0x5a ) {
				//packet drop!
				fclose(file);
#ifdef _DEBUG
				cout<<"Wrong order"<<endl;
#endif
				return false;
			}
			last_packet++; //255 overflow is OK

			if( total_size > 0 ) {
				position += received_length-(get_data_index()+1);
				instance.calculate(position,total_size,true);
			}

			//cout<<".";
			//if( comm_medium == resCOM )
			//	fwrite(&receive_buffer[get_data_index()+1],1,receive_buffer[get_command_index()+1]-1,file);
			//else
			fwrite(&receive_buffer[get_data_index()+1],1,received_length-(get_data_index()+1),file);
			packets_no--;
#ifdef _WIN32
			Sleep(10);
#endif
		}
		fclose(file);

		if( buffer_data ) {
			int		t_read = 0x200;

			memset(receive_buffer,0,0x200);
			file = fopen(tmp_file,"r+b");
			fseek(file,i_file->get_offset(),SEEK_SET);
			if( i_file->get_file_size() < 0x200 )
				t_read = i_file->get_file_size();
			fread(receive_buffer,1,t_read,file);
			i_file->set_gps_buffer(receive_buffer);

			memset(receive_buffer,0,t_read);
			fseek(file,i_file->get_offset(),SEEK_SET);
			fwrite(receive_buffer,1,t_read,file);
			fclose(file);
		}
		return true;
	}
	return false;
}

void transmission_gps::parse_protocol() {
	int read_pos = get_data_index();
	int code;

	while( read_pos < received_length ) {
		if( buffer[read_pos] == 'A' ) {
			code = convert_to_short(read_pos+1);
			if( code == 900 ) GPS_map_capable = true;
			if( code == 902 ) GPS_unlock_capable = true;
			if( code == 904 ) GPS_routable = true;
		}
		read_pos += 3;
	}
}

int transmission_gps::parse_message() {
	unsigned char code = (unsigned char)buffer[get_command_index()];
	gps_found = true;

#if _DEBUG
/*	if( code != 0x5a )
		cout<<"Code: "<<hex<<int(code)<<dec<<endl;*/
#endif

	if( comm_medium == resSIM )
		return 0x6;

	if( comm_medium == resUSB && buffer[0] != 0x14 && buffer[0] != 0x00 )
		return 0;

	if( comm_medium == resUSB && buffer[0] == 0x00 ) {
		switch(code) {
				case 0x06:
					GPS_id = convert_to_int(get_data_index());
					return 0x06;
				default:
					return 0;
		}
	}
	switch(code) {
		case 0xff:	// Pid_Product_Data: /* Version/ID record
			//doID();
			if( comm_medium == resCOM )
				strncpy(GPS_version,&(buffer[7]),255);
			else if( comm_medium == resUSB )
				strncpy(GPS_version,&(buffer[16]),255);
			return 0xff;
		case 0x20:	// Software version
			//doSoftwareVersion();
			//od 7 bajtu napis
			return 0x20;
		case 0x5b:  // start download
			memcpy(receive_buffer,buffer,sizeof(buffer));
			return 0x5b;
		case 0x5a:  // download
			memcpy(receive_buffer,buffer,sizeof(buffer));
			return 0x5a;
		case 0x5f:	// Map flash description
			gps_communication_ok = true;
			flash_id = convert_to_short(get_data_index());
			GPS_max_maps = convert_to_short(get_data_index()+2);
			GPS_memory = convert_to_int(get_data_index()+4);
			got_memory = true;
			return 0x5f;
		case 0x4a:	// zmazana pamiec...
			got_memory_erased = true;
			return 0x4a;
		case 0xfc:	// doBaudRate()
			//doBaudRate(); - nie pytam o predkosc!!
			return 0xfc;
		case 0xfd:	// protocol array
			parse_protocol();
			return 0xfd;
		case 0x6d:	// unlock
			//doUnlock();
			return 0x6d;
		case 0x31:	// doCurrentBaudRate
			process_speed_change();
			return 0x31;
		case 0x26:	// Serial
			if( comm_medium == resCOM && buffer[get_length_index()] >= 4 ) {
				gps_communication_ok = true;
				GPS_id = convert_to_int(get_data_index());
			}
			if( comm_medium == resUSB ) {
				GPS_id = convert_to_int(get_data_index());
			}
			return 0x26;
		case 6:		// Pid_Ack_Byte
			return 0x06;
		case 15:	// Pid_Nak_Byte
			return 15;
		case 21:	// Pid_Nak_Byte:
			return 21;
		default:
			return code;
	}
	/*
	if(comm_mode == resDownload) {
	switch(code)
	{
	case 0x5a:	// receive data
	return 0x5a;
	default:	// cos zle - zadnych innych komunikatow nie powinno byc....
	comm_mode = resQA; 
	return 0;
	}
	}
	*/
	return 0;
}

unsigned int transmission_gps::convert_to_short(int index) {
	int_conv.value = 0;
	if( index > (0x1000-2) )
		return 0;
	int_conv.value_c[0] = buffer[index];
	int_conv.value_c[1] = buffer[index+1];

	return int_conv.value;
}

unsigned int transmission_gps::received_buffer_convert_to_int(int index) {
	int_conv.value = 0;
	if( index > (0x1000-4) )
		return 0;
	int_conv.value_c[0] = receive_buffer[index];
	int_conv.value_c[1] = receive_buffer[index+1];
	int_conv.value_c[2] = receive_buffer[index+2];
	int_conv.value_c[3] = receive_buffer[index+3];

	return int_conv.value;
}

unsigned int transmission_gps::convert_to_int(int index) {
	int_conv.value = 0;
	if( index > (0x1000-4) )
		return 0;
	int_conv.value_c[0] = buffer[index];
	int_conv.value_c[1] = buffer[index+1];
	int_conv.value_c[2] = buffer[index+2];
	int_conv.value_c[3] = buffer[index+3];

	return int_conv.value;
}

 void transmission_gps::delay(long ms)
 {
#ifndef LINUX
 /*
	 long	t1, t2;

	 t1 = t2 = GetTickCount();
	 while((t2 - t1) < ms) {
		 t2 = GetTickCount();
	 }
*/
	 Sleep(ms);
#else
	 usleep(ms*1000);
#endif
 }	// sleep()
