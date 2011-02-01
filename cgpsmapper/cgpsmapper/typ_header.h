/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/

#ifndef TYP_headerH
#define TYP_headerH

#include <vector>
#include <string>
#include "filexor.h"

class TYPBase {
public:
	TYPBase() {rgnMarine = false; rgnType = 0; rgnSubType = 0;lang1_code = 0;lang2_code = 0; lang3_code = 0; lang4_code = 0; extended_font=0; day_font=-1; night_font=-1;};

	int			rgnType;
	int			rgnSubType;
	bool		rgnMarine;

	int			extended_font;
	int			day_font;
	int			night_font;

	char		lang1_code;
	char		lang2_code;
	char		lang3_code;
	char		lang4_code;

	std::string		lang1;
	std::string		lang2;
	std::string		lang3;
	std::string		lang4;

	int			size;
	int			data_address;
	int			typ_address;

	int			calculateSize(int&) {return 0;};
	int			calculateTypAddress(int& address,int typ_address_size);
};


class TYPPoint : public TYPBase {
public:
	static int		DATA_START;
	static int		TYP_START;

	unsigned short	flag;

	int				colors;
	int				color[255];
	int				array[32][32];
	int				columns,lines;

	int				night_colors;
	int				night_color[255];
	int				night_array[32][32];

	TYPPoint()		{night_colors = 0;colors = 0;columns=0;lines=0;};

	int				calculateSize(int& address);
	void			write(xor_fstream* file);
	void			writeTypAddress(xor_fstream* file,int typ_address_size);

	bool operator()(const TYPPoint& e1,const TYPPoint& e2);
};

class TYPLine : public TYPBase {
public:
	static int		DATA_START;
	static int		TYP_START;

	unsigned short	flag;

	bool			wide;
	bool			pattern;
	bool			antyalias;
	unsigned char	line_width;
	unsigned char	border_width;

	int				day_foreground;
	int				night_foreground;

	int				day_background;
	int				night_background;
	int				day_color;
	int				night_color;


	int				array_lines;
	unsigned int	array[32];

	TYPLine()		{array_lines = 0;wide = true; antyalias = false; pattern = false;day_foreground=-1;night_foreground=-1;day_background=-1;night_background=-1;};
	int				calculateSize(int& address);
	void			write(xor_fstream* file);
	void			writeTypAddress(xor_fstream* file,int typ_address_size);

	bool operator()(const TYPLine& e1,const TYPLine& e2);
};

class TYPPolygon : public TYPBase {
public:
	static int		DATA_START;
	static int		TYP_START;

	unsigned short	flag;

	int	day_foreground;
	int	night_foreground;
	int	day_background;
	int	night_background;
	
	int				day_color;
	int				night_color;

	unsigned int	array[32];

	TYPPolygon()	{day_foreground=-1;night_foreground=-1;day_background=-1;night_background=-1;};

	int				calculateSize(int& address);
	void			write(xor_fstream* file);
	void			writeTypAddress(xor_fstream* file,int typ_address_size);

	bool operator()(const TYPPolygon& e1,const TYPPolygon& e2);
};

class TYPPolygonTypes {
public:
	int				rgnType;
	int				rgnSubType;
	unsigned char	value;
	int				order;

	TYPPolygonTypes() {value=0; this->rgnType = 0; this->rgnSubType = 0; this->order = 0;};
	TYPPolygonTypes(int rgnType,int order) {value=0; this->rgnSubType = 0; this->rgnType = rgnType; this->order = order;};
	bool operator()(const TYPPolygonTypes& e1,const TYPPolygonTypes& e2);
};

class TYP_Header 
{
public:
	int		typHeaderLength; //0
	unsigned char	typName[10];     //2
	unsigned char	typTyp0[2];      //12
	unsigned short	typYear;         //14
	unsigned char	typMonth,typDay; //16
	unsigned char	typTime[3];      //18
	unsigned char	typUnknown[2];	 // E4 04 ?

	int		codepage;
	//POI
	int		typTYP1start; //4 bajty
	int		typTYP1length;

	//lines
	int		typTYP2start;
	int		typTYP2length;

	//areas
	int		typTYP6start;
	int		typTYP6length;

	int		product_id;

	//POI table
	int		typTYP3start;
	unsigned short	typTYP3rec_len;
	int		typTYP3length;

	//lines table
	int		typTYP4start;
	unsigned short	typTYP4rec_len;
	int		typTYP4length;

	//areas table
	int		typTYP7start;
	unsigned short	typTYP7rec_len;
	int		typTYP7length;
	
	//used polygons - priorytety?!
	int		typTYP5start;
	unsigned short	typTYP5rec_len;
	int		typTYP5length;
//data

	std::vector<TYPPoint>	points;
	std::vector<TYPLine>		lines;
	std::vector<TYPPolygon>	polygons;
	std::vector<TYPPolygonTypes>			polygons_table;//?

//reading data from txt file
	void	process(const char* file_name,const char* file_out);

	TYP_Header();
private:
	static int	readColor(std::string value);

	int		colorXPM(std::string code);
	bool	readXPM(xor_fstream* file,int array[32][32],int color[255],std::string value,int &colors,int &oryginal_colors,int &lines,int &columns);

	bool	readTYPdefinition(const char* file_name);
	void	readPoint(xor_fstream* file);
	void	readLine(xor_fstream* file);
	void	readPolygon(xor_fstream* file);
	void	readDrawOrder(xor_fstream* file);
	void	readId(xor_fstream* file);

	void	calculate();

	bool	createTYP(const char* out_file_name);
};
#endif

