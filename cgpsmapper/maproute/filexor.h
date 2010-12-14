/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

#ifndef FileXORH
#define FileXORH

#include <stdio.h>
#include <string>
#include <stdexcept>

const int		MAX_LINE_SIZE = 2048;
const int		BUFFER_MAX = 64000;

class			xor_fstream {
public:
	xor_fstream(const char *file_name, const char *mode);
	virtual		~xor_fstream(void);

	void		SetXorMask(unsigned char mask)	{ m_mask = mask; };
	virtual int Read(void *Buffer, int Count);

	virtual int Write(const void *Buffer, int Count);

	void		FillWrite(unsigned char value, int Count);					// wype³nienie wartoœciami

	std::string		ReadString(void);
	void		WriteString(std::string text);

	int			ReadInput(std::string &key, std::string &value);
	int			SearchKey(std::string key, std::string &value, std::string terminator);	// szuka klucza od pozycji biezacej do
																			///terminatora - przywraca pozycje!

	std::string		ReadLine(char terminator);
	std::string		ReadLine(void);

	long int	file_size;
	long		line_no;
	std::string		file_name;
	void		seekg(long offset, int whence);
	long		tellg(void);

	bool		error;
	int			headerOffset;

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
private:
	FILE			*stream;
	unsigned char	m_mask;
	char			m_buffer[BUFFER_MAX];
};
#endif
