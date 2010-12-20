#include "mp_mainFrame.h"
#include "error.h"
#include <stdexcept>
#include <string>
#include <crtdbg.h>

MainFrame::MainFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	//_CrtSetBreakAlloc(7696635);
	this->SetSizeHints( wxSize( 500,390 ), wxDefaultSize );
	
	m_menubar1 = new wxMenuBar( 0 );
	file = new wxMenu();
	wxMenuItem* openMp;
	openMp = new wxMenuItem( file, ID_OPEN_MP, wxString( wxT("Open MP") ) , wxEmptyString, wxITEM_NORMAL );
	file->Append( openMp );
	
	wxMenuItem* openDb;
	openDb = new wxMenuItem( file, ID_OPEN_DB, wxString( wxT("Open DB") ) , wxEmptyString, wxITEM_NORMAL );
	file->Append( openDb );
	
	wxMenuItem* createEmptyDb;
	createEmptyDb = new wxMenuItem( file, ID_CREATE_EMPTY_DB, wxString( wxT("Create empty DB") ) , wxEmptyString, wxITEM_NORMAL );
	file->Append( createEmptyDb );
	
	file->AppendSeparator();
	
	import = new wxMenu();
	wxMenuItem* importEsri;
	importEsri = new wxMenuItem( import, ID_IMPORT_ESRI, wxString( wxT("Import ESRI") ) , wxEmptyString, wxITEM_NORMAL );
	import->Append( importEsri );
	
	wxMenuItem* importMp;
	importMp = new wxMenuItem( import, ID_IMPORT_MP, wxString( wxT("Import MP") ) , wxEmptyString, wxITEM_NORMAL );
	import->Append( importMp );
	
	file->Append( -1, wxT("Import"), import );
	
	file->AppendSeparator();
	
	wxMenuItem* exit;
	exit = new wxMenuItem( file, ID_EXIT, wxString( wxT("Exit") ) , wxEmptyString, wxITEM_NORMAL );
	file->Append( exit );
	
	m_menubar1->Append( file, wxT("File") );
	
	this->SetMenuBar( m_menubar1 );
	
	m_statusBar1 = this->CreateStatusBar( 1, wxST_SIZEGRIP, wxID_ANY );
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizer1->SetFlexibleDirection( wxHORIZONTAL );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_NONE );
	
	m_notebook2 = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_panel1 = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxGridSizer* gSizer4;
	gSizer4 = new wxGridSizer( 5, 2, 0, 0 );
	
	m_staticText1 = new wxStaticText( m_panel1, wxID_ANY, wxT("Map name"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	gSizer4->Add( m_staticText1, 0, wxALL, 5 );
	
	m_textCtrl21 = new wxTextCtrl( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	gSizer4->Add( m_textCtrl21, 0, wxALL, 5 );
	
	m_staticText2 = new wxStaticText( m_panel1, wxID_ANY, wxT("Map ID"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	gSizer4->Add( m_staticText2, 0, wxALL, 5 );
	
	m_textCtrl3 = new wxTextCtrl( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	gSizer4->Add( m_textCtrl3, 0, wxALL, 5 );
	
	m_staticText3 = new wxStaticText( m_panel1, wxID_ANY, wxT("Points"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	gSizer4->Add( m_staticText3, 0, wxALL, 5 );
	
	m_textCtrl4 = new wxTextCtrl( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	gSizer4->Add( m_textCtrl4, 0, wxALL, 5 );
	
	m_staticText4 = new wxStaticText( m_panel1, wxID_ANY, wxT("Lines"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	gSizer4->Add( m_staticText4, 0, wxALL, 5 );
	
	m_textCtrl5 = new wxTextCtrl( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	gSizer4->Add( m_textCtrl5, 0, wxALL, 5 );
	
	m_staticText5 = new wxStaticText( m_panel1, wxID_ANY, wxT("Polygons"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	gSizer4->Add( m_staticText5, 0, wxALL, 5 );
	
	m_textCtrl6 = new wxTextCtrl( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	gSizer4->Add( m_textCtrl6, 0, wxALL, 5 );
	
	m_panel1->SetSizer( gSizer4 );
	m_panel1->Layout();
	gSizer4->Fit( m_panel1 );
	m_notebook2->AddPage( m_panel1, wxT("Map information"), true );
	m_panel2 = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	wxGridSizer* gSizer5;
	gSizer5 = new wxGridSizer( 1, 2, 0, 120 );
	
	m_checkBox1 = new wxCheckBox( m_panel2, wxID_ANY, wxT("Optimize streets by merging. Maximum merging angle:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBox1->SetValue(true);
	
	gSizer5->Add( m_checkBox1, 0, wxALIGN_BOTTOM|wxALL, 5 );
	
	m_angle = new wxTextCtrl( m_panel2, wxID_ANY, wxT("120"), wxDefaultPosition, wxDefaultSize, 0 );
	m_angle->SetMaxLength( 3 ); 
	gSizer5->Add( m_angle, 0, wxALL, 5 );
	
	bSizer2->Add( gSizer5, 1, wxEXPAND, 5 );
	
	m_checkBox7 = new wxCheckBox( m_panel2, wxID_ANY, wxT("Check for potential errors and correct them. Does MODIFY coordinates!"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBox7->SetValue(true);
	
	bSizer2->Add( m_checkBox7, 0, wxALL, 5 );
	
	m_checkBox2 = new wxCheckBox( m_panel2, wxID_ANY, wxT("Generate routing information. If exist - use as base input routing."), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBox2->SetValue(true);
	
	bSizer2->Add( m_checkBox2, 0, wxALL, 5 );
	
	m_checkBox3 = new wxCheckBox( m_panel2, wxID_ANY, wxT("Detect entry/exit points using bounding rectange or map background if defined (0x4b)."), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBox3->SetValue(true);
	
	bSizer2->Add( m_checkBox3, 0, wxALL, 5 );
	
	m_checkBox4 = new wxCheckBox( m_panel2, wxID_ANY, wxT("Assign new ID for NOD points and streets."), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBox4->SetValue(true);
	
	bSizer2->Add( m_checkBox4, 0, wxALL, 5 );
	
	m_checkBox5 = new wxCheckBox( m_panel2, wxID_ANY, wxT("Assign default speed attribute using road type."), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBox5->SetValue(true);
	
	bSizer2->Add( m_checkBox5, 0, wxALL, 5 );
	
	m_checkBox6 = new wxCheckBox( m_panel2, wxID_ANY, wxT("Preserve routing class if defined. Use with caution, incorrect road class cause wrong routing!"), wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer2->Add( m_checkBox6, 0, wxALL, 5 );
	
	m_buttonGenerate = new wxButton( m_panel2, wxID_ANY, wxT("Generate routing with above parameters"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_buttonGenerate, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	m_panel2->SetSizer( bSizer2 );
	m_panel2->Layout();
	bSizer2->Fit( m_panel2 );
	m_notebook2->AddPage( m_panel2, wxT("Routing"), false );
	m_panel5 = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_grid1 = new wxGrid( m_panel5, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	
	// Grid
	m_grid1->CreateGrid( 22, 2 );
	m_grid1->EnableEditing( true );
	m_grid1->EnableGridLines( true );
	m_grid1->EnableDragGridSize( false );
	m_grid1->SetMargins( 0, 0 );
	
	// Columns
	m_grid1->SetColSize( 0, 309 );
	m_grid1->SetColSize( 1, 57 );
	m_grid1->EnableDragColMove( false );
	m_grid1->EnableDragColSize( true );
	m_grid1->SetColLabelSize( 30 );
	m_grid1->SetColLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Rows
	m_grid1->SetRowSize( 0, 17 );
	m_grid1->SetRowSize( 1, 17 );
	m_grid1->SetRowSize( 2, 17 );
	m_grid1->SetRowSize( 3, 17 );
	m_grid1->SetRowSize( 4, 17 );
	m_grid1->EnableDragRowSize( true );
	m_grid1->SetRowLabelSize( 80 );
	m_grid1->SetRowLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Label Appearance
	
	// Cell Defaults
	m_grid1->SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_TOP );
	m_grid1->SetMinSize( wxSize( -1,170 ) );
	
	bSizer3->Add( m_grid1, 0, wxEXPAND, 5 );
	
	m_panel5->SetSizer( bSizer3 );
	m_panel5->Layout();
	bSizer3->Fit( m_panel5 );
	m_notebook2->AddPage( m_panel5, wxT("Routing road classes"), false );
	m_panel3 = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxGridSizer* gSizer3;
	gSizer3 = new wxGridSizer( 2, 1, 0, 0 );
	
	m_button3 = new wxButton( m_panel3, wxID_ANY, wxT("Export DB to MP text"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer3->Add( m_button3, 0, wxALL, 5 );
	
	m_button9 = new wxButton( m_panel3, wxID_ANY, wxT("Visualize restrictions as MP"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer3->Add( m_button9, 0, wxALL, 5 );
	
	m_button4 = new wxButton( m_panel3, wxID_ANY, wxT("Export to SHP as Navteq"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer3->Add( m_button4, 0, wxALL, 5 );
	
	m_panel3->SetSizer( gSizer3 );
	m_panel3->Layout();
	gSizer3->Fit( m_panel3 );
	m_notebook2->AddPage( m_panel3, wxT("Export"), false );
	m_panel4 = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxGridSizer* gSizer2;
	gSizer2 = new wxGridSizer( 4, 1, 0, 0 );
	
	m_button1 = new wxButton( m_panel4, wxID_ANY, wxT("Split roads into single segments"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer2->Add( m_button1, 0, wxALL, 5 );
	
	m_button2 = new wxButton( m_panel4, wxID_ANY, wxT("aa"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer2->Add( m_button2, 0, wxALL, 5 );
	
	m_button5 = new wxButton( m_panel4, wxID_ANY, wxT("MyButton"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer2->Add( m_button5, 0, wxALL, 5 );
	
	m_button6 = new wxButton( m_panel4, wxID_ANY, wxT("MyButton"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer2->Add( m_button6, 0, wxALL, 5 );
	
	m_button7 = new wxButton( m_panel4, wxID_ANY, wxT("MyButton"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer2->Add( m_button7, 0, wxALL, 5 );
	
	m_panel4->SetSizer( gSizer2 );
	m_panel4->Layout();
	gSizer2->Fit( m_panel4 );
	m_notebook2->AddPage( m_panel4, wxT("Special"), false );
	m_panel7 = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_sql = new wxTextCtrl( m_panel7, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 461,80 ), wxTE_MULTILINE );
	fgSizer2->Add( m_sql, 0, wxALL, 5 );
	
	m_execSQL = new wxButton( m_panel7, wxID_ANY, wxT("Execute script"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_execSQL, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	m_panel7->SetSizer( fgSizer2 );
	m_panel7->Layout();
	fgSizer2->Fit( m_panel7 );
	m_notebook2->AddPage( m_panel7, wxT("Lua script"), false );
	
	fgSizer1->Add( m_notebook2, 1, wxEXPAND | wxALL, 5 );
	
	m_panel10 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_textCtrl2 = new wxTextCtrl( m_panel10, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,80 ), wxTE_MULTILINE|wxTE_READONLY );
	bSizer1->Add( m_textCtrl2, 0, wxEXPAND, 5 );
	
	m_panel10->SetSizer( bSizer1 );
	m_panel10->Layout();
	bSizer1->Fit( m_panel10 );
	fgSizer1->Add( m_panel10, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( fgSizer1 );
	this->Layout();
	/****************/
	//m_angle = new wxTextCtrl( m_panel2, wxID_ANY, wxT("120"), wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC)  );

	m_notebook2->ChangeSelection(0);

	// Connect Events
	m_button1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnSplitRoads ), NULL, this );
	m_button3->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnExportMP ), NULL, this );
	m_button4->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnExportNavteq ), NULL, this );
	m_button9->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnExportRestr ), NULL, this );
	m_execSQL->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnExecSQL ), NULL, this );
	m_notebook2->Connect( wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler( MainFrame::OnSQLPageSelect ), NULL, this );
	m_buttonGenerate->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnGenerateRouting ), NULL, this );
	this->Connect( openMp->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnOpenMP ) );
	this->Connect( openDb->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnOpenDB ) );
	this->Connect( exit->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnExit ) );
	this->Connect( createEmptyDb->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnCreateDB ) );
	this->Connect( importEsri->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnImportESRI ) );
	this->Connect( importMp->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnImportMP ) );


	/************************************/
	mpDB = NULL;
	disableMP();
	errorClass::redirectMessages(m_textCtrl2);

	m_grid1->SetCellValue( 0, 0, _T("01 Major HWY thick"));
	m_grid1->SetCellValue( 1, 0, _T("02 Principal HWY-thick")); 
	m_grid1->SetCellValue( 2, 0, _T("03 Principal HWY-medium"));
	m_grid1->SetCellValue( 3, 0, _T("04 Arterial Road -medium"));
	m_grid1->SetCellValue( 4, 0, _T("05 Arterial Road-thick"));
	m_grid1->SetCellValue( 5, 0, _T("06 Road-thin"));
	m_grid1->SetCellValue( 6, 0, _T("07 Alley-thick"));
	m_grid1->SetCellValue( 7, 0, _T("08 Ramp"));
	m_grid1->SetCellValue( 8, 0, _T("09 Ramp"));
	m_grid1->SetCellValue( 9, 0, _T("10 Unpaved Road-thin"));
	m_grid1->SetCellValue(10, 0, _T("11 Major HWY Connector-thick"));
	m_grid1->SetCellValue(11, 0, _T("12 Roundabout"));
	m_grid1->SetCellValue(12, 0, _T("13 Custom routable road 1"));
	m_grid1->SetCellValue(13, 0, _T("14 Custom routable road 2"));
	m_grid1->SetCellValue(14, 0, _T("15 Custom routable road 3"));
	m_grid1->SetCellValue(15, 0, _T("16 Custom routable road 4"));
	m_grid1->SetCellValue(16, 0, _T("17 Custom routable road 5"));
	m_grid1->SetCellValue(17, 0, _T("18 Custom routable road 6"));
	m_grid1->SetCellValue(18, 0, _T("19 Custom routable road 7"));
	m_grid1->SetCellValue(19, 0, _T("22 Trail"));
	m_grid1->SetCellValue(20, 0, _T("26 Ferry"));
	m_grid1->SetCellValue(21, 0, _T("27 Ferry"));


	sqlFrame = new SQLFrame(this, wxID_ANY, _T("SQL resutl"));
	progress = new ProgressFrame(this, wxID_ANY, _T("Processing"));
	importEsriDialog = new ImporESRI(this, wxID_ANY);
	

}

MainFrame::~MainFrame()
{
	// Disconnect Events
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnOpenMP ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnOpenDB ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnCreateDB ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnImportESRI ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnImportMP ) );

	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnExit ) );
	m_notebook2->Disconnect( wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler( MainFrame::OnSQLPageSelect ), NULL, this );
	m_button3->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnExportMP ), NULL, this );
	m_button4->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnExportNavteq ), NULL, this );
	m_button1->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnSplitRoads ), NULL, this );
	m_button9->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnExportRestr ), NULL, this );
	m_execSQL->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnExecSQL ), NULL, this );

	delete sqlFrame;
	delete progress;
	delete importEsriDialog;

	if( mpDB ) 
		sqlite3_close(mpDB);
}

void MainFrame::waitCursor(bool state) {
/*	if( state )
		m_notebook2->SetCursor(wxCursor(wxCURSOR_WAIT));
	else
		m_notebook2->SetCursor(wxCursor(wxCURSOR_ARROW));
*/
}

void MainFrame::disableMP() {
	m_notebook2->Enable(false);
	m_textCtrl21->SetLabel(_T(""));
	m_textCtrl3->SetLabel(_T(""));
	m_textCtrl4->SetLabel(_T(""));
	m_textCtrl5->SetLabel(_T(""));
	m_textCtrl6->SetLabel(_T(""));

	/*
	m_panel1->Enable(false);
	m_panel2->Enable(false);
	m_panel3->Enable(false);
*/
}

void MainFrame::enableMP() {
	MPInfo	mp_info;
	m_notebook2->Enable(true);

	mp_info.setDB(mpDB);
	m_textCtrl21->SetLabel(mp_info.getName());
	m_textCtrl3->SetLabel(mp_info.getID());
	m_textCtrl4->SetLabel(mp_info.getPoints());
	m_textCtrl5->SetLabel(mp_info.getLines());
	m_textCtrl6->SetLabel(mp_info.getPolygons());

	/*
	m_panel1->Enable(true);
	m_panel2->Enable(true);
	m_panel3->Enable(true);
	*/
}

void MainFrame::OnExit( wxCommandEvent& event ) {
	this->Close();
}

void MainFrame::OnCreateDB( wxCommandEvent& event ) {
	bool		conversionOK = false;
	wxString	dbName;

	wxFileDialog file(this,_T("Select a new DB file"),_T(""), _T(""), _T("*.*"),wxFD_SAVE);
	if( file.ShowModal() == wxID_OK ) {
		dbName = file.GetPath();
		dbName.append(_T(".db.sdb"));
		try {
			DBCreate dbCreate(dbName.char_str());
			dbCreate.createInputDB();
			conversionOK = true;
		} catch( std::exception &e ) {
			m_statusBar1->PushStatusText( wxString::FromUTF8( e.what() ));
		}
	}

	if( conversionOK ) {
		try {
			if( mpDB ) {
				m_textCtrl2->AppendText( _T("Closing current DB.\n") );
				sqlite3_close(mpDB);
			}
			if( sqlite3_open(dbName.char_str(),&mpDB) != SQLITE_OK )
				sqlite3_close(mpDB);
			else {
				enableMP();		
				MPInfo mp_info;
				mp_info.setDB(mpDB);
				m_textCtrl2->AppendText( mp_info.getDBinfo_IMG() );
			}
		} catch ( std::exception &e ) {
			m_statusBar1->PushStatusText( wxString::FromUTF8( e.what() ));
			mpDB = NULL;
			disableMP();
		}
	}

}

void MainFrame::OnImportESRI( wxCommandEvent& event ) {
	importEsriDialog->ShowModal();
}

void MainFrame::OnImportMP( wxCommandEvent& event ) {
}

void MainFrame::OnOpenDB( wxCommandEvent& event ) {
	bool		conversionOK = false;
	MPInfo		mp_info;
	int			rc;
	wxFileDialog file(this,_T("Select a DB file"));
	if( file.ShowModal() == wxID_OK ) {
		try {
			if( mpDB ) {
				m_textCtrl2->AppendText( _T("Closing current DB.\n") );
				sqlite3_close(mpDB);
				mpDB = NULL;
			}
			rc = sqlite3_open(file.GetPath().char_str(),&mpDB);
			//m_textCtrl2->AppendText( wxString::Format(_T("%i"),rc ));

			if( rc != SQLITE_OK ) {
				//m_textCtrl2->AppendText( wxString::FromUTF8( sqlite3_errmsg(mpDB) ) );
				sqlite3_close(mpDB);
			} else
				m_statusBar1->PushStatusText(_T("DB is ready."));
			enableMP();		
			mp_info.setDB(mpDB);
			m_textCtrl2->AppendText( mp_info.getDBinfo_IMG() );
		} catch ( std::exception &e ) {
			m_statusBar1->PushStatusText( wxString::FromUTF8( e.what() ));
			mpDB = NULL;
			disableMP();
		}
	}
}

void MainFrame::OnOpenMP( wxCommandEvent& event ) {
	bool		conversionOK = false;
	wxString	dbName;

	wxFileDialog file(this,_T("Select a map file"));
	if( file.ShowModal() == wxID_OK ) {
		dbName = file.GetPath();
		dbName.append(_T(".db.sdb"));

		DBCreate dbCreate(dbName.char_str());
		MPReader mpReader(file.GetPath().char_str());

		if( dbCreate.constructorError() ) {
			m_statusBar1->PushStatusText(_T("Error creating DB." ));
			return;
		}

		if( mpReader.constructorError() ) {
			m_statusBar1->PushStatusText(_T("Error opening MP." ));
			return;
		}
		waitCursor(true);
		
		m_statusBar1->PushStatusText(_T("Preparing database." ));
		try {
			dbCreate.createInputDB();
			m_statusBar1->PushStatusText(_T("Reading MP file: " + file.GetFilename()));
			mpReader.setDB(dbCreate.getDB());
			mpReader.readMP();
			m_statusBar1->PushStatusText(_T("MP file converted to DB."));
			m_panel1->Enable(true);
			conversionOK = true;
		} catch( std::exception &e ) {
			m_statusBar1->PushStatusText( wxString::FromUTF8( e.what() ));
		}

		waitCursor(false);
	}
	if( conversionOK ) {
		try {
			if( mpDB ) {
				m_textCtrl2->AppendText( _T("Closing current DB.\n") );
				sqlite3_close(mpDB);
			}
			if( sqlite3_open(dbName.char_str(),&mpDB) != SQLITE_OK )
				sqlite3_close(mpDB);
			else {
				enableMP();		
				MPInfo mp_info;
				mp_info.setDB(mpDB);
				m_textCtrl2->AppendText( mp_info.getDBinfo_IMG() );
			}
		} catch ( std::exception &e ) {
			m_statusBar1->PushStatusText( wxString::FromUTF8( e.what() ));
			mpDB = NULL;
			disableMP();
		}
	}
}

void MainFrame::OnGenerateRouting( wxCommandEvent& event ) {
	MPProcess		mpProcess;
	MPMergeParams	mergeParams;
	try {
		if( mpDB ) {
			wxWindowDisabler disableAll;
			mainApp->Yield();
			progress->Show(true);
			progress->CenterOnParent();

			m_statusBar1->PushStatusText(_T("Generating routing." ));
			waitCursor(true);
			mpProcess.setDB(mpDB);

			//mergeParams
			m_angle->GetValue().ToLong(&mergeParams.max_angle,10);

			mpProcess.mergeRoads(mainApp,progress->m_Progress,&mergeParams);
			
			mpProcess.checkIntersect(mainApp,progress->m_Progress);

			m_textCtrl2->AppendText( _T("Generation done.\n") );
			enableMP();
			progress->Show(false);

		}
	} catch( std::exception &e ) {
		m_statusBar1->PushStatusText( wxString::FromUTF8(e.what()) );
	}
	waitCursor(false);
}


void MainFrame::OnSplitRoads( wxCommandEvent& event ) {
	MPProcess	mpProcess;
	try {
		if( mpDB ) {
			wxWindowDisabler disableAll;
			mainApp->Yield();
			progress->Show(true);
			progress->CenterOnParent();

			m_statusBar1->PushStatusText(_T("Splitting all roads into single segment parts." ));
			waitCursor(true);
			mpProcess.splitRoadsAtSegments(mpDB,mainApp,progress->m_Progress);
			m_textCtrl2->AppendText( _T("All roads has been split into single segments.\n") );
			enableMP();
			progress->Show(false);
		}
	} catch( std::exception &e ) {
		m_statusBar1->PushStatusText( wxString::FromUTF8(e.what()) );
	}
	waitCursor(false);
}

void MainFrame::OnExportMP( wxCommandEvent& event ) {
	MPExport mpExport;
	if( mpDB ) {
		wxFileDialog file(this,_T("Select a map file"),_T(""), _T(""), _T("*.*"),wxFD_SAVE);
		if( file.ShowModal() == wxID_OK ) {
			m_statusBar1->PushStatusText(_T("Exporting to MP file."));
			waitCursor(true);
			mpExport.exportMP(mpDB,file.GetPath().char_str());
			waitCursor(false);
			m_textCtrl2->AppendText( _T("Export to MP done.\n") );
		}
	}
}

void MainFrame::OnExportNavteq( wxCommandEvent& event ) {
	MPExport mpExport;

	OnSplitRoads( event );
	//		 m_statusBar1->PushStatusText(_T("Exporting to SHP file."));

	if( mpDB ) {
		wxFileDialog file(this,_T("Select a map file"),_T(""), _T(""), _T("*.*"),wxFD_SAVE);
		if( file.ShowModal() == wxID_OK ) {
			m_statusBar1->PushStatusText(_T("Exporting to SHP file."));
			waitCursor(true);
			mpExport.exportSHP(mpDB,_polyline,file.GetPath().char_str());
			waitCursor(false);
			m_textCtrl2->AppendText( _T("Export to SHP done.\n") );
		}
	}
}

void MainFrame::OnExportRestr( wxCommandEvent& event ) {
	MPExport mpExport;
	if( mpDB ) {
		wxFileDialog file(this,_T("Select a map file"),_T(""), _T(""), _T("*.*"),wxFD_SAVE);
		if( file.ShowModal() == wxID_OK ) {
			m_statusBar1->PushStatusText(_T("Exporting restrictions to MP file."));
			waitCursor(true);
			mpExport.exportRestrictionsMP(mpDB,file.GetPath().char_str());
			waitCursor(false);
			m_textCtrl2->AppendText( _T("Export restrictions to MP done.\n") );
		}
	}

}

void MainFrame::OnSQLPageSelect( wxNotebookEvent& event ) {
	if( event.GetSelection() == 5 )
		sqlFrame->Show(true);
	else
		sqlFrame->Show(false);
	event.Skip();
}

void MainFrame::printLua(const char* text) {
	sqlFrame->m_htmlSQL->AppendToPage( wxString::FromUTF8(text));
}

void MainFrame::OnExecSQL( wxCommandEvent& event ) {

	MPProcess mp_process;
	mp_process.setDB(mpDB);
	mp_process.registerLua(sqlFrame->m_htmlSQL);

	sqlFrame->m_htmlSQL->SetPage(_T("<html><body>"));
	mp_process.doString( m_sql->GetLabel().ToUTF8() );
	sqlFrame->m_htmlSQL->AppendToPage( _T("</body></html>") );

	return;

	MPInfo	mp_info;
	if( mpDB ) {
		mp_info.setDB(mpDB);

		sqlFrame->m_htmlSQL->SetPage(_T("<html><body><table>"));

		sqlFrame->m_htmlSQL->AppendToPage( mp_info.execQuery(m_sql->GetLabel().ToUTF8()) );
		while( mp_info.hasNextLine() ) 
			sqlFrame->m_htmlSQL->AppendToPage( mp_info.getNextLine() );

		sqlFrame->m_htmlSQL->AppendToPage( _T("</table></body></html>") );
	}
}
