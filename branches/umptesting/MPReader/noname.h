///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __noname__
#define __noname__

#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/statusbr.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/grid.h>
#include <wx/notebook.h>
#include <wx/frame.h>
#include <wx/radiobox.h>
#include <wx/choice.h>
#include <wx/dialog.h>
#include <wx/html/htmlwin.h>
#include <wx/gauge.h>

///////////////////////////////////////////////////////////////////////////

#define ID_OPEN_MP 1000
#define ID_OPEN_DB 1001
#define ID_CREATE_EMPTY_DB 1002
#define ID_IMPORT_ESRI 1003
#define ID_IMPORT_MP 1004
#define ID_EXIT 1005

///////////////////////////////////////////////////////////////////////////////
/// Class MainFrame
///////////////////////////////////////////////////////////////////////////////
class MainFrame : public wxFrame 
{
	private:
	
	protected:
		wxMenuBar* m_menubar1;
		wxMenu* file;
		wxMenu* import;
		wxStatusBar* m_statusBar1;
		wxNotebook* m_notebook2;
		wxPanel* m_panel1;
		wxStaticText* m_staticText1;
		wxTextCtrl* m_textCtrl21;
		wxStaticText* m_staticText2;
		wxTextCtrl* m_textCtrl3;
		wxStaticText* m_staticText3;
		wxTextCtrl* m_textCtrl4;
		wxStaticText* m_staticText4;
		wxTextCtrl* m_textCtrl5;
		wxStaticText* m_staticText5;
		wxTextCtrl* m_textCtrl6;
		wxPanel* m_panel2;
		wxCheckBox* m_checkBox1;
		wxTextCtrl* m_angle;
		wxCheckBox* m_checkBox7;
		wxCheckBox* m_checkBox2;
		wxCheckBox* m_checkBox3;
		wxCheckBox* m_checkBox4;
		wxCheckBox* m_checkBox5;
		wxCheckBox* m_checkBox6;
		wxButton* m_buttonGenerate;
		wxPanel* m_panel5;
		wxGrid* m_grid1;
		wxPanel* m_panel3;
		wxButton* m_button3;
		wxButton* m_button9;
		wxButton* m_button4;
		wxPanel* m_panel4;
		wxButton* m_button1;
		wxButton* m_button2;
		wxButton* m_button5;
		wxButton* m_button6;
		wxButton* m_button7;
		wxPanel* m_panel7;
		wxTextCtrl* m_sql;
		wxButton* m_execSQL;
		wxPanel* m_panel10;
		wxTextCtrl* m_textCtrl2;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnOpenMP( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnOpenDB( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnCreateDB( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnImportESRI( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnImportMP( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnExit( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnSQLPageSelect( wxNotebookEvent& event ){ event.Skip(); }
		virtual void OnGenerateRouting( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnExportMP( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnExportRestr( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnExportNavteq( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnSplitRoads( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnExecSQL( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		MainFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("MP Processor"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,390 ), long style = wxCAPTION|wxCLOSE_BOX|wxICONIZE|wxMINIMIZE|wxMINIMIZE_BOX|wxSYSTEM_MENU|wxTAB_TRAVERSAL );
		~MainFrame();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class ImporESRI
///////////////////////////////////////////////////////////////////////////////
class ImporESRI : public wxDialog 
{
	private:
	
	protected:
		wxButton* m_button11;
		wxRadioBox* m_radioBox1;
		wxNotebook* m_notebook2;
		wxPanel* m_panel8;
		wxStaticText* m_staticText7;
		wxChoice* m_choice1;
		wxStaticText* m_staticText8;
		wxChoice* m_choice2;
		wxStaticText* m_staticText9;
		wxChoice* m_choice3;
		wxStaticText* m_staticText10;
		wxChoice* m_choice4;
		wxStaticText* m_staticText11;
		wxChoice* m_choice5;
		wxStaticText* m_staticText12;
		wxChoice* m_choice6;
		wxStaticText* m_staticText13;
		wxChoice* m_choice7;
		wxPanel* m_panel11;
		wxStaticText* m_staticText14;
		wxChoice* m_choice8;
		wxStaticText* m_staticText15;
		wxChoice* m_choice9;
		wxStaticText* m_staticText16;
		wxChoice* m_choice10;
		wxPanel* m_panel12;
		wxStaticText* m_staticText17;
		wxChoice* m_choice11;
		wxStaticText* m_staticText18;
		wxChoice* m_choice12;
		wxStaticText* m_staticText19;
		wxChoice* m_choice13;
		wxStaticText* m_staticText20;
		wxChoice* m_choice14;
		wxButton* m_button13;
		
		wxStaticText* m_staticText22;
		wxChoice* m_choice16;
		wxStaticText* m_staticText23;
		wxChoice* m_choice17;
		wxStaticText* m_staticText24;
		wxChoice* m_choice18;
		wxStaticText* m_staticText25;
		wxChoice* m_choice19;
		wxStaticText* m_staticText26;
		wxChoice* m_choice20;
		wxStaticText* m_staticText27;
		wxChoice* m_choice21;
		wxStaticText* m_staticText28;
		wxChoice* m_choice22;
		wxStaticText* m_staticText29;
		wxChoice* m_choice23;
		wxStaticText* m_staticText30;
		wxChoice* m_choice24;
		wxStaticText* m_staticText31;
		wxChoice* m_choice25;
		wxStaticText* m_staticText32;
		wxChoice* m_choice26;
		wxStaticText* m_staticText33;
		wxChoice* m_choice27;
		wxStaticText* m_staticText34;
		wxChoice* m_choice28;
		wxStaticText* m_staticText35;
		wxChoice* m_choice29;
		wxStaticText* m_staticText36;
		wxChoice* m_choice30;
		wxStaticText* m_staticText37;
		wxChoice* m_choice31;
		wxPanel* m_panel13;
		wxStaticText* m_staticText38;
		wxChoice* m_choice32;
		wxStaticText* m_staticText39;
		wxChoice* m_choice33;
		wxStaticText* m_staticText40;
		wxChoice* m_choice34;
		wxStaticText* m_staticText41;
		wxChoice* m_choice35;
		wxStaticText* m_staticText42;
		wxChoice* m_choice36;
		wxStaticText* m_staticText43;
		wxChoice* m_choice37;
		wxStaticText* m_staticText44;
		wxChoice* m_choice38;
		wxStaticText* m_staticText45;
		wxChoice* m_choice39;
		wxStaticText* m_staticText46;
		wxChoice* m_choice40;
		wxStaticText* m_staticText54;
		wxChoice* m_choice48;
		wxButton* m_button14;
		
		wxStaticText* m_staticText47;
		wxChoice* m_choice41;
		wxStaticText* m_staticText48;
		wxChoice* m_choice42;
		wxStaticText* m_staticText49;
		wxChoice* m_choice43;
		wxStaticText* m_staticText50;
		wxChoice* m_choice44;
		wxStaticText* m_staticText51;
		wxChoice* m_choice45;
		wxStaticText* m_staticText52;
		wxChoice* m_choice46;
		wxStaticText* m_staticText53;
		wxChoice* m_choice47;
		wxPanel* m_panel14;
		wxStaticText* m_staticText55;
		wxChoice* m_choice49;
		wxStaticText* m_staticText56;
		wxChoice* m_choice50;
		wxStaticText* m_staticText57;
		wxChoice* m_choice51;
		wxStaticText* m_staticText58;
		wxChoice* m_choice52;
		wxStaticText* m_staticText59;
		wxChoice* m_choice53;
		wxStaticText* m_staticText60;
		wxChoice* m_choice54;
		wxStaticText* m_staticText61;
		wxChoice* m_choice55;
		wxStaticText* m_staticText62;
		wxChoice* m_choice56;
		wxStaticText* m_staticText63;
		wxChoice* m_choice57;
		wxStaticText* m_staticText64;
		wxChoice* m_choice58;
		wxStaticText* m_staticText65;
		wxChoice* m_choice59;
		wxStaticText* m_staticText66;
		wxChoice* m_choice60;
		wxButton* m_button15;
		
		wxStaticText* m_staticText67;
		wxChoice* m_choice61;
		wxStaticText* m_staticText68;
		wxChoice* m_choice62;
		wxButton* m_button12;
	
	public:
		ImporESRI( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Import ESRI"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 557,687 ), long style = wxCAPTION|wxDEFAULT_DIALOG_STYLE );
		~ImporESRI();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class MyDialog2
///////////////////////////////////////////////////////////////////////////////
class MyDialog2 : public wxDialog 
{
	private:
	
	protected:
	
	public:
		MyDialog2( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Import data parameters"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 550,163 ), long style = wxCAPTION|wxDEFAULT_DIALOG_STYLE );
		~MyDialog2();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class SQLFrame
///////////////////////////////////////////////////////////////////////////////
class SQLFrame : public wxFrame 
{
	private:
	
	protected:
		wxHtmlWindow* m_htmlSQL;
	
	public:
		SQLFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		~SQLFrame();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class ProgressFrame
///////////////////////////////////////////////////////////////////////////////
class ProgressFrame : public wxFrame 
{
	private:
	
	protected:
		wxGauge* m_Progress;
	
	public:
		ProgressFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 495,48 ), long style = wxFRAME_FLOAT_ON_PARENT|wxFRAME_TOOL_WINDOW|wxTAB_TRAVERSAL );
		~ProgressFrame();
	
};

#endif //__noname__
