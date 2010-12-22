#ifndef MP_MAIN_FRAME
#define MP_MAIN_FRAME

#include "wx\wx.h"
#include "wx\grid.h"
#include "wx\notebook.h"
#include "wx\html\htmlwin.h"
#include "mp_read.h"
#include "db_create.h"
#include "mp_export.h"
#include "mp_process.h"
#include "mp_info.h"
#include "mp_sql.h"
#include "mp_esri.h"
#include <string>

#define ID_OPEN_MP 1000
#define ID_OPEN_DB 1001
#define ID_CREATE_EMPTY_DB 1002
#define ID_IMPORT_ESRI 1003
#define ID_IMPORT_MP 1004
#define ID_EXIT 1005

class MainFrame : public wxFrame 
{
	private:
		SQLFrame*		sqlFrame;
		ProgressFrame*	progress;
		ImporESRI*		importEsriDialog;
		wxApp*			mainApp;

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
		virtual void OnSplitRoads( wxCommandEvent& event );
		virtual void OnExportMP( wxCommandEvent& event );
		virtual void OnExportNavteq( wxCommandEvent& event );
		virtual void OnOpenMP( wxCommandEvent& event );
		virtual void OnOpenDB( wxCommandEvent& event );

		virtual void OnCreateDB( wxCommandEvent& event );
		virtual void OnImportESRI( wxCommandEvent& event );
		virtual void OnImportMP( wxCommandEvent& event );

		virtual void OnExit( wxCommandEvent& event );
		virtual void OnExportRestr( wxCommandEvent& event );
		virtual void OnExecSQL( wxCommandEvent& event );
		virtual void OnSQLPageSelect( wxNotebookEvent& event );
		virtual void OnGenerateRouting( wxCommandEvent& event );
	
		void	disableMP();
		void	enableMP();
		void	waitCursor(bool state);
		static void	processCallback(std::string &message);
		sqlite3*	mpDB;
	public:
		MainFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("MP Processor"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,390 ), long style = wxCAPTION|wxCLOSE_BOX|wxICONIZE|wxMINIMIZE|wxMINIMIZE_BOX|wxSYSTEM_MENU|wxTAB_TRAVERSAL );
		~MainFrame();
		
		void	setApp(wxApp* _mainApp) {mainApp = _mainApp;};
		void	printLua(const char* text);
};


#endif