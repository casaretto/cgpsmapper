/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

#include "filexor.h"
#include "utils.h"
#include <string.h>
/*
 =======================================================================================================================
 =======================================================================================================================
 */

xor_fstream::xor_fstream(const char *file_name, const char *mode) {
	this->file_name = file_name;
	stream = fopen(file_name, mode);
	headerOffset = 0;

	if(!stream) {
		error = true;
		return;
	}

	error = false;

	m_mask = 0;
	line_no = 0;
	seekg(0, SEEK_END);
	file_size = tellg();
	seekg(0, SEEK_SET);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
xor_fstream::~xor_fstream(void) {
	if(stream > 0)
		fclose(stream);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xor_fstream::seekg(long offset, int whence) {
	if(whence == SEEK_SET)
		offset += headerOffset;
	fseek(stream, offset, whence);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
long xor_fstream::tellg(void) {
	long	t_ret = ftell(stream);
	t_ret -= headerOffset;
	return t_ret;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int xor_fstream::Read(void *Buffer, int Count) {
	int size, t_licznik = 0;

	memset(Buffer,0,Count);

	try
	{
		size = static_cast<int>(fread(((char *) Buffer), 1, Count, stream));

		if(m_mask && size) {
			while(t_licznik < size) {
				((unsigned char *) Buffer)[t_licznik] = ((unsigned char *) Buffer)[t_licznik] ^ m_mask;
				t_licznik++;
			}
		}
	}

	catch (...) {
		throw runtime_error("Error reading file.");
	}

	return size;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int xor_fstream::Write(const void *Buffer, int Count) {
	unsigned char	znak;
	int				t_licznik = 0;

	while(t_licznik < Count) {
		znak = ((unsigned char *) Buffer)[t_licznik] ^ m_mask;
		fwrite(&znak, 1, 1, stream);	// write(&znak,1);
		t_licznik++;
	}

	return Count;
}

int xor_fstream::WriteInt(int value, int Count) {
	return Write(&value,Count);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xor_fstream::FillWrite(unsigned char value, int Count) {
	unsigned char	bufor[1];
	bufor[0] = value ^ m_mask;

	while(Count) {
		fwrite(bufor, 1, 1, stream);	// write(bufor,1);
		Count--;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int xor_fstream::SearchKey(string master_key, string &value, string terminator) {
	string	key;
	int		t_file_pos = tellg();
	int		t_line_pos = this->line_no;

	int		t_read = ReadInput(key, value);
	while(t_read != 0 && key.find(terminator) > 0) {
		if(master_key == key) {
			seekg(t_file_pos, SEEK_SET);
			this->line_no = t_line_pos;
			return 2;
		}

		t_read = ReadInput(key, value);
	}
	this->line_no = t_line_pos;
	seekg(t_file_pos, SEEK_SET);
	return 0;
}


bool xor_fstream::SeekLine(int line) {
	
	int		t_line = 0;
	char	znak;
	seekg(0,SEEK_SET);

	while( t_line < line ) {
		if( !Read(&znak,1) ) return false;
		if( znak == '\n' ) t_line++;
	}
	return true;
}

//
// =======================================================================================================================
//    czytanie z pliku TXT wejsciowego 0 - blad czytania 1 - [key] 2 - key = value 3 - nieznany string - komentarz 4 -
//    key = NULL 5 - nieznany string - bez ';'
// =======================================================================================================================
//
int xor_fstream::ReadInput(string &key, string &value) {
	key = /* upper_case( */ ReadLine(); // );
	if(tellg() >= file_size)
		return 0;
	if( key.size() == 0 )
		return 3;
	if(key[0] == '[') {
		if(key.find(']'))
			key = key.substr(0, key.find(']') + 1);
		key = upper_case(key);
		return 1;
	}

	if(key[0] == ';')
		return 3;
	if(key.find("=") < key.length()) {
		value = key.substr(key.find("=") + 1, key.length() - 1 - key.find("="));
		key = key.substr(0, key.find("="));
		key = upper_case(key);
		if(value.length() == 0)
			return 4;
		return 2;
	}

	return 5;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
string xor_fstream::ReadLine(char terminator) {
	char	znak;

	string	t_ret_line;
	t_ret_line.reserve(10000);

	line_no++;
	if(!Read(&znak, 1))
		return t_ret_line;
	while(znak != terminator) {
		if(znak != '\r' && znak != '\n')
			t_ret_line = t_ret_line + znak;
		if(!Read(&znak, 1))
			return t_ret_line;
	}

	return t_ret_line;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
string xor_fstream::ReadLine(void) {
	string	t_ret;

	line_no++;

	if( fgets(m_buffer,BUFFER_MAX-1,stream) ) {
		while( m_buffer[strlen(m_buffer)-1] != '\n' && m_buffer[strlen(m_buffer)-1] != '\r' ) {
			t_ret += m_buffer;
			if( fgets(m_buffer,BUFFER_MAX-1,stream) == NULL)
				break;
		}
		if( strlen(m_buffer) > 0 && (m_buffer[strlen(m_buffer)-1] == '\n' || m_buffer[strlen(m_buffer)-1] == '\r') )
			m_buffer[strlen(m_buffer)-1] = 0;
		if( strlen(m_buffer) > 0 && (m_buffer[strlen(m_buffer)-1] == '\n' || m_buffer[strlen(m_buffer)-1] == '\r') )
			m_buffer[strlen(m_buffer)-1] = 0;
		t_ret += m_buffer;
	}


	/*

	if(!Read(&znak, 1))
		return t_ret;
	while(znak != '\n') {
		if(znak != '\r' && znak != '\n')
			t_ret = t_ret + znak;
		if(!Read(&znak, 1))
			return t_ret;
	}
	*/
	return t_ret;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
string xor_fstream::ReadString(void) {
	char	znak;
	string	t_ret;

	if(!Read(&znak, 1))
		return t_ret;
	while(znak) {
		t_ret = t_ret + znak;
		if(!Read(&znak, 1))
			return t_ret;
	}

	return t_ret;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xor_fstream::WriteString(string text) {
	char	znak;
	int		t_pos = 0;

	while(t_pos < (int) text.length()) {
		znak = text[t_pos];
		Write(&znak, 1);
		t_pos++;
	}

	FillWrite(0, 1);
}
