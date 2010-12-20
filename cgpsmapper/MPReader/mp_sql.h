#ifndef _MP_SQL_FRAME
#define _MP_SQL_FRAME

#include "wx/wx.h"
#include "wx/html/htmlwin.h"
#include "mp_read.h"
#include "mp_export.h"
#include "mp_process.h"
#include "mp_info.h"

class SQLFrame : public wxFrame 
{
	private:
	
	public:
		wxHtmlWindow* m_htmlSQL;
		SQLFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		~SQLFrame();
	
};

class ProgressFrame : public wxFrame 
{
	private:
	
	protected:
	
	public:
		wxGauge* m_Progress;

		ProgressFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 495,48 ), long style = wxFRAME_FLOAT_ON_PARENT|wxFRAME_TOOL_WINDOW|wxTAB_TRAVERSAL );
		~ProgressFrame();
	
};

#endif