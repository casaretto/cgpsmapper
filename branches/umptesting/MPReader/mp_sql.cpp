#include "mp_sql.h"

SQLFrame::SQLFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxGridSizer* gSizer4;
	gSizer4 = new wxGridSizer( 1, 1, 0, 0 );
	
	m_htmlSQL = new wxHtmlWindow( this, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), wxHW_SCROLLBAR_AUTO );
	gSizer4->Add( m_htmlSQL, 0, wxEXPAND, 5 );
	
	this->SetSizer( gSizer4 );
	this->Layout();
}

SQLFrame::~SQLFrame()
{
}

ProgressFrame::ProgressFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	m_Progress = new wxGauge( this, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL );
	bSizer4->Add( m_Progress, 0, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( bSizer4 );
	this->Layout();
	
	this->Centre( wxBOTH );
}

ProgressFrame::~ProgressFrame()
{
}