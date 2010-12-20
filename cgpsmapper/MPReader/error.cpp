#include <string>
#include <iostream>
#include "error.h"

using namespace std;

#ifdef _WIN32
bool errorClass::show_message_error = true;
#else
bool errorClass::show_message_error = false;
#endif

wxTextCtrl* errorClass::txtControl = NULL;

char errorClass::warn_messages[_message_max][_message_len] = 
{
	"W001: Could not open include file.",
	"W002: No zip codes file defined.",
	"W003: No highways file defined.",
	"W004: Cannot determine type of element, type cannot be defined before RGNTYPE.",
	"W005: Error reading data.",
	"W006: Null exit faclilty name for RGN10 element.",
	"W007: Wrong coordinates.",
	"W008: Element spans more than 10 degrees!",
	"W009: Invalid [WPT] section - RGNTYPE is not defined.",
	"W010: ELEVATION parameter is depreciated",
	"W011: Invalid [PLT] section - RGNTYPE is not defined.",
	"W012: Element cannot be indexed if label start with a special character ~[0x..].",
	"W013: TRESIZE smaller than 100.",
	"W014: RGNLIMIT should not be lower than 500.",
	"W015: TRESIZE larger than 7000 - TRESIZE is fixed (i.e. the TRESIZE is automatically changed to 7000).",
	"W016: ",
	"W017: ",
	"W018: ID of map should be larger than 0x10000 (65536) or may not work in MapSource.",
	"W019: More than ONE background objects defined - switching to full manual background creation mode.",
	"W020: Object has more than 255 nodes - it is allowed only for the PREVIEW map - did you forget to add 'Preview=Y' in [IMG ID]?.",
	"W021: Missing [_DRAWORDER] section - polygons may not be visible.",
	"W022: DRAWPRIORITY must be between 0 and 31.",
	"W023: Wrong index information. Search by city / region / country will NOT work.",
	"W024: Type of element should not be 0.",
	"W025: Not a valid value.",

	"E001: Could not open file with country name definitions.",
	"E002: Invalid name for CountryField in [DEFINITIONS].",
	"E003: Could not open file with region name definitions.",
	"E004: Invalid name for RegionField in [DEFINITIONS].",
	"E005: Invalid name for RegionCountryIdx in [DEFINITIONS].",
	"E006: Could not open file with city name definitions.",
	"E007: Invalid name for CityField in [DEFINITIONS].",
	"E008: Invalid name for CityRegionIdx in [DEFINITIONS].",
	"E009: Invalid name for ZipCodeField in [DEFINITIONS].",
	"E010: Invalid name for HighwayRegionIdx in [DEFINITIONS].",
	"E011: Invalid name for HighwayField in [DEFINITIONS].",
	"E012: Invalid sequence in [COUNTRIES].",
	"E013: Invalid sequence in [REGIONS].",
	"E014: Invalid sequence in [CITIES].",
	"E015: Invalid sequence in [ZIPCODES].",
	"E016: Invalid sequence in [HIGHWAYS].",
	"E017: Cannot parse coordinates.",
	"E018: Type of element for RGN40/RGN80 cannot be higher than 127.",
	"E019: Street cannot intersect with itself! Split this element!",
	"E020: No more than 8 active layer allowed.",
	"E021: Grid definition for layers must be descending (check LevelX keys in [IMG ID]).",
	"E022: STREETNUMBERSSTART and STREETNUMBERSEND keys are no longer supported - use ROADID instead.",
	"E023: Layer detail level too high to cover non-splittable objects from lower layer - decrease detail level (use higher Level#).",
	"E024: Top layer detail level too high to cover entire map - decrease detail level of the less detail layer (use higher Level#).",
	"E025: Zoom definition for layers must be ascending (check ZoomX keys in [IMG ID]).",
	"E026: More than 65535 Tre regions were created in a single layer - use bigger TRESIZE and RGNLIMIT or split your map.",
	"E027: Timeout limit - compilation interrupted because of the timeout set by administrator.",
	"E028: Region and Country information defined by HIGHWAY is not consistent with definition of CITY.",
	"E029: ID of map is not an integer value.",
	"E030: Name of the file for preview must be composed always from 8 digits.",
	"E031: For the preview creation name '00000008.img' is not permitted.",
	"E032: Layer 0 of the map cannot be empty.",
	"E033: Less than 2 layers not allowed.",
	"E034: Not enough columns for XPM bitmap definition.",
	"E035: Wrong XPM bitmap definition.",
	"E036: ID of map cannot be higher than 268435455 (0x0FFFFFFF).",
	"E037: City index beyond the number of defined cities.",
	"E038:",
	"E039: Too many labels in the final map - split map into smaller parts.",
	"E040: R*Tree structure bigger than 65535. Try higher TreSize value or split the map.",

	"R001: Cannot find segment for routing.",
	"R002: Routing between same points.",
	"R003: Routable object cannot be filtered - check your [DICTIONARY] section.",
	"R004: Removing element which can be routable.",
	"R005: Maximum allowed NODID value is 1048575.",
	"R006: Creating connections error.",
	"R007: Node reduction.",
	"R008: Too short road to be routable - coordinates were aligned to same place.",
	"R009: Limit of 60 segments for a single road exceed.",
	"R010: No data for routing - remove 'ROUTING=Y' from [IMG ID] for non routable maps!",
	"R011: NODID points cannot be closer than 5.4 meter!",
	"R012: NODID point defined for non existing point of the road!",
	"R013: Restriction defined for non existing NODID!",
	"R014: Several DataX for routable roads not permitted!"
};

void errorClass::redirectMessages(wxTextCtrl* _txtControl) {
	txtControl = _txtControl;
}

void errorClass::showError(int line,const char* file_name,const char* code)
{
	char	buffer[255];
	int		error_number;
	int		human_error_number = -1; // error code coded in string
	bool	fatal = false;
	bool	unknown = true;
	string	text;

	//locate code and select severity
	if( code == NULL )
		return;
	//form:
	//E001 or PE01
	if( code[0] == 'E' || code[1] == 'E' ) {
		fatal = true;
		text = "Error ";
	} else
		text = "Warning ";

	//text += code;

	for( error_number =0; error_number < _message_max; error_number++ ) {
		if( strlen(warn_messages[error_number]) >= strlen(code) )
			if( !memcmp(code,warn_messages[error_number],strlen(code)) ) {
				unknown = false;
				break;
			}
	}

	if( file_name ) {
		if( line >= 0 ) {
			text = file_name;
			text += "(";
			_itoa_s(line,buffer,250,10);
			text += buffer;
			text += ") : ";
			if( !unknown ) text += warn_messages[error_number];
		} else {
			if( !unknown ) text += warn_messages[error_number];
			text += " ";
			text += file_name;
		}
	} else
		if( !unknown ) text += warn_messages[error_number];

	if( txtControl ) {
		txtControl->AppendText( _T("\n") );
		txtControl->AppendText( wxString::FromUTF8( text.c_str() ) );
	} else {
		cout<<text<<endl;
	}

	if( fatal )
	{
		if( !unknown && strlen(code) > 2 ) {
			if( code[1] <'0' || code[1] >'9' )
				human_error_number = atoi(&code[2]);
			else
				human_error_number = atoi(&code[1]);
		}

		if( show_message_error )
#ifdef _WIN32
			MessageBoxA(NULL,text.c_str(),"Input data ERROR",MB_ICONASTERISK | MB_OK);		
#else
			;
#endif
#ifdef _DEBUG
		cout<<human_error_number<<endl;
#endif
		exit(human_error_number);
	}
}