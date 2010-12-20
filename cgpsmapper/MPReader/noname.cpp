///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "noname.h"

///////////////////////////////////////////////////////////////////////////

MainFrame::MainFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
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
	
	// Connect Events
	this->Connect( openMp->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnOpenMP ) );
	this->Connect( openDb->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnOpenDB ) );
	this->Connect( createEmptyDb->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnCreateDB ) );
	this->Connect( importEsri->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnImportESRI ) );
	this->Connect( importMp->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnImportMP ) );
	this->Connect( exit->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::OnExit ) );
	m_notebook2->Connect( wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler( MainFrame::OnSQLPageSelect ), NULL, this );
	m_buttonGenerate->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnGenerateRouting ), NULL, this );
	m_button3->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnExportMP ), NULL, this );
	m_button9->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnExportRestr ), NULL, this );
	m_button4->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnExportNavteq ), NULL, this );
	m_button1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnSplitRoads ), NULL, this );
	m_execSQL->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnExecSQL ), NULL, this );
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
	m_buttonGenerate->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnGenerateRouting ), NULL, this );
	m_button3->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnExportMP ), NULL, this );
	m_button9->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnExportRestr ), NULL, this );
	m_button4->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnExportNavteq ), NULL, this );
	m_button1->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnSplitRoads ), NULL, this );
	m_execSQL->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrame::OnExecSQL ), NULL, this );
}

ImporESRI::ImporESRI( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	m_button11 = new wxButton( this, wxID_ANY, wxT("Open ESRI file"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_button11, 0, wxALL, 5 );
	
	wxString m_radioBox1Choices[] = { wxT("No routing info / partial info"), wxT("cGPSmapper like routing input (segment dbf with routing data)"), wxT("Navteq like routing definition (z-level file, reference, non-reference nodes)") };
	int m_radioBox1NChoices = sizeof( m_radioBox1Choices ) / sizeof( wxString );
	m_radioBox1 = new wxRadioBox( this, wxID_ANY, wxT("Routing input type"), wxDefaultPosition, wxDefaultSize, m_radioBox1NChoices, m_radioBox1Choices, 1, wxRA_SPECIFY_COLS );
	m_radioBox1->SetSelection( 0 );
	bSizer5->Add( m_radioBox1, 0, wxALL, 5 );
	
	m_notebook2 = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_panel8 = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText7 = new wxStaticText( m_panel8, wxID_ANY, wxT("Type of road (integer)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	fgSizer3->Add( m_staticText7, 0, wxALL, 5 );
	
	wxArrayString m_choice1Choices;
	m_choice1 = new wxChoice( m_panel8, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice1Choices, 0 );
	m_choice1->SetSelection( 0 );
	fgSizer3->Add( m_choice1, 0, wxALL, 5 );
	
	m_staticText8 = new wxStaticText( m_panel8, wxID_ANY, wxT("Default type"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText8->Wrap( -1 );
	fgSizer3->Add( m_staticText8, 0, wxALL, 5 );
	
	wxArrayString m_choice2Choices;
	m_choice2 = new wxChoice( m_panel8, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice2Choices, 0 );
	m_choice2->SetSelection( 0 );
	fgSizer3->Add( m_choice2, 0, wxALL, 5 );
	
	m_staticText9 = new wxStaticText( m_panel8, wxID_ANY, wxT("Label"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText9->Wrap( -1 );
	fgSizer3->Add( m_staticText9, 0, wxALL, 5 );
	
	wxArrayString m_choice3Choices;
	m_choice3 = new wxChoice( m_panel8, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice3Choices, 0 );
	m_choice3->SetSelection( 0 );
	fgSizer3->Add( m_choice3, 0, wxALL, 5 );
	
	m_staticText10 = new wxStaticText( m_panel8, wxID_ANY, wxT("2nd label"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText10->Wrap( -1 );
	fgSizer3->Add( m_staticText10, 0, wxALL, 5 );
	
	wxArrayString m_choice4Choices;
	m_choice4 = new wxChoice( m_panel8, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice4Choices, 0 );
	m_choice4->SetSelection( 0 );
	fgSizer3->Add( m_choice4, 0, wxALL, 5 );
	
	m_staticText11 = new wxStaticText( m_panel8, wxID_ANY, wxT("3rd label"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11->Wrap( -1 );
	fgSizer3->Add( m_staticText11, 0, wxALL, 5 );
	
	wxArrayString m_choice5Choices;
	m_choice5 = new wxChoice( m_panel8, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice5Choices, 0 );
	m_choice5->SetSelection( 0 );
	fgSizer3->Add( m_choice5, 0, wxALL, 5 );
	
	m_staticText12 = new wxStaticText( m_panel8, wxID_ANY, wxT("Level"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText12->Wrap( -1 );
	fgSizer3->Add( m_staticText12, 0, wxALL, 5 );
	
	wxArrayString m_choice6Choices;
	m_choice6 = new wxChoice( m_panel8, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice6Choices, 0 );
	m_choice6->SetSelection( 0 );
	fgSizer3->Add( m_choice6, 0, wxALL, 5 );
	
	m_staticText13 = new wxStaticText( m_panel8, wxID_ANY, wxT("End level"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText13->Wrap( -1 );
	fgSizer3->Add( m_staticText13, 0, wxALL, 5 );
	
	wxArrayString m_choice7Choices;
	m_choice7 = new wxChoice( m_panel8, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice7Choices, 0 );
	m_choice7->SetSelection( 0 );
	fgSizer3->Add( m_choice7, 0, wxALL, 5 );
	
	m_panel8->SetSizer( fgSizer3 );
	m_panel8->Layout();
	fgSizer3->Fit( m_panel8 );
	m_notebook2->AddPage( m_panel8, wxT("Basic attributes"), true );
	m_panel11 = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxFlexGridSizer* fgSizer5;
	fgSizer5 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer5->SetFlexibleDirection( wxBOTH );
	fgSizer5->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText14 = new wxStaticText( m_panel11, wxID_ANY, wxT("City (left and right)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText14->Wrap( -1 );
	fgSizer5->Add( m_staticText14, 0, wxALL, 5 );
	
	wxArrayString m_choice8Choices;
	m_choice8 = new wxChoice( m_panel11, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice8Choices, 0 );
	m_choice8->SetSelection( 0 );
	fgSizer5->Add( m_choice8, 0, wxALL, 5 );
	
	m_staticText15 = new wxStaticText( m_panel11, wxID_ANY, wxT("Region (left and right)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText15->Wrap( -1 );
	fgSizer5->Add( m_staticText15, 0, wxALL, 5 );
	
	wxArrayString m_choice9Choices;
	m_choice9 = new wxChoice( m_panel11, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice9Choices, 0 );
	m_choice9->SetSelection( 0 );
	fgSizer5->Add( m_choice9, 0, wxALL, 5 );
	
	m_staticText16 = new wxStaticText( m_panel11, wxID_ANY, wxT("Country (left and right)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText16->Wrap( -1 );
	fgSizer5->Add( m_staticText16, 0, wxALL, 5 );
	
	wxArrayString m_choice10Choices;
	m_choice10 = new wxChoice( m_panel11, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice10Choices, 0 );
	m_choice10->SetSelection( 0 );
	fgSizer5->Add( m_choice10, 0, wxALL, 5 );
	
	m_panel11->SetSizer( fgSizer5 );
	m_panel11->Layout();
	fgSizer5->Fit( m_panel11 );
	m_notebook2->AddPage( m_panel11, wxT("Address"), false );
	m_panel12 = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer7;
	fgSizer7 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer7->SetFlexibleDirection( wxBOTH );
	fgSizer7->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_ALL );
	
	m_staticText17 = new wxStaticText( m_panel12, wxID_ANY, wxT("RoadID (Link ID)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText17->Wrap( -1 );
	fgSizer7->Add( m_staticText17, 0, wxALL, 5 );
	
	wxArrayString m_choice11Choices;
	m_choice11 = new wxChoice( m_panel12, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice11Choices, 0 );
	m_choice11->SetSelection( 0 );
	fgSizer7->Add( m_choice11, 0, wxALL, 5 );
	
	m_staticText18 = new wxStaticText( m_panel12, wxID_ANY, wxT("One way"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText18->Wrap( -1 );
	fgSizer7->Add( m_staticText18, 0, wxALL, 5 );
	
	wxArrayString m_choice12Choices;
	m_choice12 = new wxChoice( m_panel12, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice12Choices, 0 );
	m_choice12->SetSelection( 0 );
	fgSizer7->Add( m_choice12, 0, wxALL, 5 );
	
	m_staticText19 = new wxStaticText( m_panel12, wxID_ANY, wxT("Speed"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText19->Wrap( -1 );
	fgSizer7->Add( m_staticText19, 0, wxALL, 5 );
	
	wxArrayString m_choice13Choices;
	m_choice13 = new wxChoice( m_panel12, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice13Choices, 0 );
	m_choice13->SetSelection( 0 );
	fgSizer7->Add( m_choice13, 0, wxALL, 5 );
	
	m_staticText20 = new wxStaticText( m_panel12, wxID_ANY, wxT("Route class"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText20->Wrap( -1 );
	fgSizer7->Add( m_staticText20, 0, wxALL, 5 );
	
	wxArrayString m_choice14Choices;
	m_choice14 = new wxChoice( m_panel12, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice14Choices, 0 );
	m_choice14->SetSelection( 0 );
	fgSizer7->Add( m_choice14, 0, wxALL, 5 );
	
	bSizer9->Add( fgSizer7, 1, wxALIGN_TOP, 5 );
	
	wxGridSizer* gSizer10;
	gSizer10 = new wxGridSizer( 2, 2, 0, 0 );
	
	wxFlexGridSizer* fgSizer9;
	fgSizer9 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer9->SetFlexibleDirection( wxBOTH );
	fgSizer9->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_NONE );
	
	m_button13 = new wxButton( m_panel12, wxID_ANY, wxT("Open segment file"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer9->Add( m_button13, 0, wxALL, 5 );
	
	
	fgSizer9->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticText22 = new wxStaticText( m_panel12, wxID_ANY, wxT("RoadID"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText22->Wrap( -1 );
	fgSizer9->Add( m_staticText22, 0, wxALL, 5 );
	
	wxArrayString m_choice16Choices;
	m_choice16 = new wxChoice( m_panel12, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice16Choices, 0 );
	m_choice16->SetSelection( 0 );
	fgSizer9->Add( m_choice16, 0, wxALL, 5 );
	
	m_staticText23 = new wxStaticText( m_panel12, wxID_ANY, wxT("NodeID"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText23->Wrap( -1 );
	fgSizer9->Add( m_staticText23, 0, wxALL, 5 );
	
	wxArrayString m_choice17Choices;
	m_choice17 = new wxChoice( m_panel12, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice17Choices, 0 );
	m_choice17->SetSelection( 0 );
	fgSizer9->Add( m_choice17, 0, wxALL, 5 );
	
	m_staticText24 = new wxStaticText( m_panel12, wxID_ANY, wxT("Bound"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText24->Wrap( -1 );
	fgSizer9->Add( m_staticText24, 0, wxALL, 5 );
	
	wxArrayString m_choice18Choices;
	m_choice18 = new wxChoice( m_panel12, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice18Choices, 0 );
	m_choice18->SetSelection( 0 );
	fgSizer9->Add( m_choice18, 0, wxALL, 5 );
	
	m_staticText25 = new wxStaticText( m_panel12, wxID_ANY, wxT("Vertex (node)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText25->Wrap( -1 );
	fgSizer9->Add( m_staticText25, 0, wxALL, 5 );
	
	wxArrayString m_choice19Choices;
	m_choice19 = new wxChoice( m_panel12, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice19Choices, 0 );
	m_choice19->SetSelection( 0 );
	fgSizer9->Add( m_choice19, 0, wxALL, 5 );
	
	m_staticText26 = new wxStaticText( m_panel12, wxID_ANY, wxT("Left-side zip"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText26->Wrap( -1 );
	fgSizer9->Add( m_staticText26, 0, wxALL, 5 );
	
	wxArrayString m_choice20Choices;
	m_choice20 = new wxChoice( m_panel12, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice20Choices, 0 );
	m_choice20->SetSelection( 0 );
	fgSizer9->Add( m_choice20, 0, wxALL, 5 );
	
	m_staticText27 = new wxStaticText( m_panel12, wxID_ANY, wxT("Right-side zip"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText27->Wrap( -1 );
	fgSizer9->Add( m_staticText27, 0, wxALL, 5 );
	
	wxArrayString m_choice21Choices;
	m_choice21 = new wxChoice( m_panel12, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice21Choices, 0 );
	m_choice21->SetSelection( 0 );
	fgSizer9->Add( m_choice21, 0, wxALL, 5 );
	
	gSizer10->Add( fgSizer9, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer11;
	fgSizer11 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer11->SetFlexibleDirection( wxBOTH );
	fgSizer11->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText28 = new wxStaticText( m_panel12, wxID_ANY, wxT("City left"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText28->Wrap( -1 );
	fgSizer11->Add( m_staticText28, 0, wxALL, 5 );
	
	wxArrayString m_choice22Choices;
	m_choice22 = new wxChoice( m_panel12, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice22Choices, 0 );
	m_choice22->SetSelection( 0 );
	fgSizer11->Add( m_choice22, 0, wxALL, 5 );
	
	m_staticText29 = new wxStaticText( m_panel12, wxID_ANY, wxT("Region left"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText29->Wrap( -1 );
	fgSizer11->Add( m_staticText29, 0, wxALL, 5 );
	
	wxArrayString m_choice23Choices;
	m_choice23 = new wxChoice( m_panel12, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice23Choices, 0 );
	m_choice23->SetSelection( 0 );
	fgSizer11->Add( m_choice23, 0, wxALL, 5 );
	
	m_staticText30 = new wxStaticText( m_panel12, wxID_ANY, wxT("Country left"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText30->Wrap( -1 );
	fgSizer11->Add( m_staticText30, 0, wxALL, 5 );
	
	wxArrayString m_choice24Choices;
	m_choice24 = new wxChoice( m_panel12, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice24Choices, 0 );
	m_choice24->SetSelection( 0 );
	fgSizer11->Add( m_choice24, 0, wxALL, 5 );
	
	m_staticText31 = new wxStaticText( m_panel12, wxID_ANY, wxT("City right"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText31->Wrap( -1 );
	fgSizer11->Add( m_staticText31, 0, wxALL, 5 );
	
	wxArrayString m_choice25Choices;
	m_choice25 = new wxChoice( m_panel12, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice25Choices, 0 );
	m_choice25->SetSelection( 0 );
	fgSizer11->Add( m_choice25, 0, wxALL, 5 );
	
	m_staticText32 = new wxStaticText( m_panel12, wxID_ANY, wxT("Region right"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText32->Wrap( -1 );
	fgSizer11->Add( m_staticText32, 0, wxALL, 5 );
	
	wxArrayString m_choice26Choices;
	m_choice26 = new wxChoice( m_panel12, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice26Choices, 0 );
	m_choice26->SetSelection( 0 );
	fgSizer11->Add( m_choice26, 0, wxALL, 5 );
	
	m_staticText33 = new wxStaticText( m_panel12, wxID_ANY, wxT("Country right"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText33->Wrap( -1 );
	fgSizer11->Add( m_staticText33, 0, wxALL, 5 );
	
	wxArrayString m_choice27Choices;
	m_choice27 = new wxChoice( m_panel12, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice27Choices, 0 );
	m_choice27->SetSelection( 0 );
	fgSizer11->Add( m_choice27, 0, wxALL, 5 );
	
	m_staticText34 = new wxStaticText( m_panel12, wxID_ANY, wxT("Left side start"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText34->Wrap( -1 );
	fgSizer11->Add( m_staticText34, 0, wxALL, 5 );
	
	wxArrayString m_choice28Choices;
	m_choice28 = new wxChoice( m_panel12, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice28Choices, 0 );
	m_choice28->SetSelection( 0 );
	fgSizer11->Add( m_choice28, 0, wxALL, 5 );
	
	m_staticText35 = new wxStaticText( m_panel12, wxID_ANY, wxT("Left side end"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText35->Wrap( -1 );
	fgSizer11->Add( m_staticText35, 0, wxALL, 5 );
	
	wxArrayString m_choice29Choices;
	m_choice29 = new wxChoice( m_panel12, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice29Choices, 0 );
	m_choice29->SetSelection( 0 );
	fgSizer11->Add( m_choice29, 0, wxALL, 5 );
	
	m_staticText36 = new wxStaticText( m_panel12, wxID_ANY, wxT("Right side start"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText36->Wrap( -1 );
	fgSizer11->Add( m_staticText36, 0, wxALL, 5 );
	
	wxArrayString m_choice30Choices;
	m_choice30 = new wxChoice( m_panel12, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice30Choices, 0 );
	m_choice30->SetSelection( 0 );
	fgSizer11->Add( m_choice30, 0, wxALL, 5 );
	
	m_staticText37 = new wxStaticText( m_panel12, wxID_ANY, wxT("Right side end"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText37->Wrap( -1 );
	fgSizer11->Add( m_staticText37, 0, wxALL, 5 );
	
	wxArrayString m_choice31Choices;
	m_choice31 = new wxChoice( m_panel12, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice31Choices, 0 );
	m_choice31->SetSelection( 0 );
	fgSizer11->Add( m_choice31, 0, wxALL, 5 );
	
	gSizer10->Add( fgSizer11, 1, wxEXPAND, 5 );
	
	bSizer9->Add( gSizer10, 2, wxEXPAND, 5 );
	
	m_panel12->SetSizer( bSizer9 );
	m_panel12->Layout();
	bSizer9->Fit( m_panel12 );
	m_notebook2->AddPage( m_panel12, wxT("cGPSmapper routing"), false );
	m_panel13 = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer12;
	fgSizer12 = new wxFlexGridSizer( 2, 4, 0, 0 );
	fgSizer12->SetFlexibleDirection( wxBOTH );
	fgSizer12->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText38 = new wxStaticText( m_panel13, wxID_ANY, wxT("Toll"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText38->Wrap( -1 );
	fgSizer12->Add( m_staticText38, 0, wxALL, 5 );
	
	wxArrayString m_choice32Choices;
	m_choice32 = new wxChoice( m_panel13, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice32Choices, 0 );
	m_choice32->SetSelection( 0 );
	fgSizer12->Add( m_choice32, 0, wxALL, 5 );
	
	m_staticText39 = new wxStaticText( m_panel13, wxID_ANY, wxT("Emergency denied"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText39->Wrap( -1 );
	fgSizer12->Add( m_staticText39, 0, wxALL, 5 );
	
	wxArrayString m_choice33Choices;
	m_choice33 = new wxChoice( m_panel13, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice33Choices, 0 );
	m_choice33->SetSelection( 0 );
	fgSizer12->Add( m_choice33, 0, wxALL, 5 );
	
	m_staticText40 = new wxStaticText( m_panel13, wxID_ANY, wxT("Delivery denied"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText40->Wrap( -1 );
	fgSizer12->Add( m_staticText40, 0, wxALL, 5 );
	
	wxArrayString m_choice34Choices;
	m_choice34 = new wxChoice( m_panel13, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice34Choices, 0 );
	m_choice34->SetSelection( 0 );
	fgSizer12->Add( m_choice34, 0, wxALL, 5 );
	
	m_staticText41 = new wxStaticText( m_panel13, wxID_ANY, wxT("Car denied"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText41->Wrap( -1 );
	fgSizer12->Add( m_staticText41, 0, wxALL, 5 );
	
	wxArrayString m_choice35Choices;
	m_choice35 = new wxChoice( m_panel13, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice35Choices, 0 );
	m_choice35->SetSelection( 0 );
	fgSizer12->Add( m_choice35, 0, wxALL, 5 );
	
	m_staticText42 = new wxStaticText( m_panel13, wxID_ANY, wxT("Bus denied"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText42->Wrap( -1 );
	fgSizer12->Add( m_staticText42, 0, wxALL, 5 );
	
	wxArrayString m_choice36Choices;
	m_choice36 = new wxChoice( m_panel13, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice36Choices, 0 );
	m_choice36->SetSelection( 0 );
	fgSizer12->Add( m_choice36, 0, wxALL, 5 );
	
	m_staticText43 = new wxStaticText( m_panel13, wxID_ANY, wxT("Taxi denied"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText43->Wrap( -1 );
	fgSizer12->Add( m_staticText43, 0, wxALL, 5 );
	
	wxArrayString m_choice37Choices;
	m_choice37 = new wxChoice( m_panel13, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice37Choices, 0 );
	m_choice37->SetSelection( 0 );
	fgSizer12->Add( m_choice37, 0, wxALL, 5 );
	
	m_staticText44 = new wxStaticText( m_panel13, wxID_ANY, wxT("Pedestrian denied"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText44->Wrap( -1 );
	fgSizer12->Add( m_staticText44, 0, wxALL, 5 );
	
	wxArrayString m_choice38Choices;
	m_choice38 = new wxChoice( m_panel13, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice38Choices, 0 );
	m_choice38->SetSelection( 0 );
	fgSizer12->Add( m_choice38, 0, wxALL, 5 );
	
	m_staticText45 = new wxStaticText( m_panel13, wxID_ANY, wxT("Bicycle denied"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText45->Wrap( -1 );
	fgSizer12->Add( m_staticText45, 0, wxALL, 5 );
	
	wxArrayString m_choice39Choices;
	m_choice39 = new wxChoice( m_panel13, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice39Choices, 0 );
	m_choice39->SetSelection( 0 );
	fgSizer12->Add( m_choice39, 0, wxALL, 5 );
	
	m_staticText46 = new wxStaticText( m_panel13, wxID_ANY, wxT("Truck denied"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText46->Wrap( -1 );
	fgSizer12->Add( m_staticText46, 0, wxALL, 5 );
	
	wxArrayString m_choice40Choices;
	m_choice40 = new wxChoice( m_panel13, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice40Choices, 0 );
	m_choice40->SetSelection( 0 );
	fgSizer12->Add( m_choice40, 0, wxALL, 5 );
	
	m_staticText54 = new wxStaticText( m_panel13, wxID_ANY, wxT("Turn restrictions short format"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText54->Wrap( -1 );
	fgSizer12->Add( m_staticText54, 0, wxALL, 5 );
	
	wxArrayString m_choice48Choices;
	m_choice48 = new wxChoice( m_panel13, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice48Choices, 0 );
	m_choice48->SetSelection( 0 );
	fgSizer12->Add( m_choice48, 0, wxALL, 5 );
	
	bSizer11->Add( fgSizer12, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer13;
	fgSizer13 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer13->SetFlexibleDirection( wxBOTH );
	fgSizer13->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_button14 = new wxButton( m_panel13, wxID_ANY, wxT("Open restriction file"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer13->Add( m_button14, 0, wxALL, 5 );
	
	
	fgSizer13->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticText47 = new wxStaticText( m_panel13, wxID_ANY, wxT("NodeID1"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText47->Wrap( -1 );
	fgSizer13->Add( m_staticText47, 0, wxALL, 5 );
	
	wxArrayString m_choice41Choices;
	m_choice41 = new wxChoice( m_panel13, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice41Choices, 0 );
	m_choice41->SetSelection( 0 );
	fgSizer13->Add( m_choice41, 0, wxALL, 5 );
	
	m_staticText48 = new wxStaticText( m_panel13, wxID_ANY, wxT("NodeID2"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText48->Wrap( -1 );
	fgSizer13->Add( m_staticText48, 0, wxALL, 5 );
	
	wxArrayString m_choice42Choices;
	m_choice42 = new wxChoice( m_panel13, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice42Choices, 0 );
	m_choice42->SetSelection( 0 );
	fgSizer13->Add( m_choice42, 0, wxALL, 5 );
	
	m_staticText49 = new wxStaticText( m_panel13, wxID_ANY, wxT("NodeID3"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText49->Wrap( -1 );
	fgSizer13->Add( m_staticText49, 0, wxALL, 5 );
	
	wxArrayString m_choice43Choices;
	m_choice43 = new wxChoice( m_panel13, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice43Choices, 0 );
	m_choice43->SetSelection( 0 );
	fgSizer13->Add( m_choice43, 0, wxALL, 5 );
	
	m_staticText50 = new wxStaticText( m_panel13, wxID_ANY, wxT("NodeID4"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText50->Wrap( -1 );
	fgSizer13->Add( m_staticText50, 0, wxALL, 5 );
	
	wxArrayString m_choice44Choices;
	m_choice44 = new wxChoice( m_panel13, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice44Choices, 0 );
	m_choice44->SetSelection( 0 );
	fgSizer13->Add( m_choice44, 0, wxALL, 5 );
	
	m_staticText51 = new wxStaticText( m_panel13, wxID_ANY, wxT("RoadID1"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText51->Wrap( -1 );
	fgSizer13->Add( m_staticText51, 0, wxALL, 5 );
	
	wxArrayString m_choice45Choices;
	m_choice45 = new wxChoice( m_panel13, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice45Choices, 0 );
	m_choice45->SetSelection( 0 );
	fgSizer13->Add( m_choice45, 0, wxALL, 5 );
	
	m_staticText52 = new wxStaticText( m_panel13, wxID_ANY, wxT("RoadID2"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText52->Wrap( -1 );
	fgSizer13->Add( m_staticText52, 0, wxALL, 5 );
	
	wxArrayString m_choice46Choices;
	m_choice46 = new wxChoice( m_panel13, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice46Choices, 0 );
	m_choice46->SetSelection( 0 );
	fgSizer13->Add( m_choice46, 0, wxALL, 5 );
	
	m_staticText53 = new wxStaticText( m_panel13, wxID_ANY, wxT("RoadID3"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText53->Wrap( -1 );
	fgSizer13->Add( m_staticText53, 0, wxALL, 5 );
	
	wxArrayString m_choice47Choices;
	m_choice47 = new wxChoice( m_panel13, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice47Choices, 0 );
	m_choice47->SetSelection( 0 );
	fgSizer13->Add( m_choice47, 0, wxALL, 5 );
	
	bSizer11->Add( fgSizer13, 2, wxEXPAND, 5 );
	
	m_panel13->SetSizer( bSizer11 );
	m_panel13->Layout();
	bSizer11->Fit( m_panel13 );
	m_notebook2->AddPage( m_panel13, wxT("Restrictions"), false );
	m_panel14 = new wxPanel( m_notebook2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer14;
	fgSizer14 = new wxFlexGridSizer( 2, 4, 0, 0 );
	fgSizer14->SetFlexibleDirection( wxBOTH );
	fgSizer14->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText55 = new wxStaticText( m_panel14, wxID_ANY, wxT("RoadID (link ID)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText55->Wrap( -1 );
	fgSizer14->Add( m_staticText55, 0, wxALL, 5 );
	
	wxArrayString m_choice49Choices;
	m_choice49 = new wxChoice( m_panel14, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice49Choices, 0 );
	m_choice49->SetSelection( 0 );
	fgSizer14->Add( m_choice49, 0, wxALL, 5 );
	
	m_staticText56 = new wxStaticText( m_panel14, wxID_ANY, wxT("Reference node"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText56->Wrap( -1 );
	fgSizer14->Add( m_staticText56, 0, wxALL, 5 );
	
	wxArrayString m_choice50Choices;
	m_choice50 = new wxChoice( m_panel14, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice50Choices, 0 );
	m_choice50->SetSelection( 0 );
	fgSizer14->Add( m_choice50, 0, wxALL, 5 );
	
	m_staticText57 = new wxStaticText( m_panel14, wxID_ANY, wxT("Non-reference node"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText57->Wrap( -1 );
	fgSizer14->Add( m_staticText57, 0, wxALL, 5 );
	
	wxArrayString m_choice51Choices;
	m_choice51 = new wxChoice( m_panel14, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice51Choices, 0 );
	m_choice51->SetSelection( 0 );
	fgSizer14->Add( m_choice51, 0, wxALL, 5 );
	
	m_staticText58 = new wxStaticText( m_panel14, wxID_ANY, wxT("Speed"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText58->Wrap( -1 );
	fgSizer14->Add( m_staticText58, 0, wxALL, 5 );
	
	wxArrayString m_choice52Choices;
	m_choice52 = new wxChoice( m_panel14, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice52Choices, 0 );
	m_choice52->SetSelection( 0 );
	fgSizer14->Add( m_choice52, 0, wxALL, 5 );
	
	m_staticText59 = new wxStaticText( m_panel14, wxID_ANY, wxT("Route class"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText59->Wrap( -1 );
	fgSizer14->Add( m_staticText59, 0, wxALL, 5 );
	
	wxArrayString m_choice53Choices;
	m_choice53 = new wxChoice( m_panel14, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice53Choices, 0 );
	m_choice53->SetSelection( 0 );
	fgSizer14->Add( m_choice53, 0, wxALL, 5 );
	
	m_staticText60 = new wxStaticText( m_panel14, wxID_ANY, wxT("One way"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText60->Wrap( -1 );
	fgSizer14->Add( m_staticText60, 0, wxALL, 5 );
	
	wxArrayString m_choice54Choices;
	m_choice54 = new wxChoice( m_panel14, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice54Choices, 0 );
	m_choice54->SetSelection( 0 );
	fgSizer14->Add( m_choice54, 0, wxALL, 5 );
	
	m_staticText61 = new wxStaticText( m_panel14, wxID_ANY, wxT("Left side start"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText61->Wrap( -1 );
	fgSizer14->Add( m_staticText61, 0, wxALL, 5 );
	
	wxArrayString m_choice55Choices;
	m_choice55 = new wxChoice( m_panel14, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice55Choices, 0 );
	m_choice55->SetSelection( 0 );
	fgSizer14->Add( m_choice55, 0, wxALL, 5 );
	
	m_staticText62 = new wxStaticText( m_panel14, wxID_ANY, wxT("Left side end"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText62->Wrap( -1 );
	fgSizer14->Add( m_staticText62, 0, wxALL, 5 );
	
	wxArrayString m_choice56Choices;
	m_choice56 = new wxChoice( m_panel14, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice56Choices, 0 );
	m_choice56->SetSelection( 0 );
	fgSizer14->Add( m_choice56, 0, wxALL, 5 );
	
	m_staticText63 = new wxStaticText( m_panel14, wxID_ANY, wxT("Right side start"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText63->Wrap( -1 );
	fgSizer14->Add( m_staticText63, 0, wxALL, 5 );
	
	wxArrayString m_choice57Choices;
	m_choice57 = new wxChoice( m_panel14, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice57Choices, 0 );
	m_choice57->SetSelection( 0 );
	fgSizer14->Add( m_choice57, 0, wxALL, 5 );
	
	m_staticText64 = new wxStaticText( m_panel14, wxID_ANY, wxT("Right side end"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText64->Wrap( -1 );
	fgSizer14->Add( m_staticText64, 0, wxALL, 5 );
	
	wxArrayString m_choice58Choices;
	m_choice58 = new wxChoice( m_panel14, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice58Choices, 0 );
	m_choice58->SetSelection( 0 );
	fgSizer14->Add( m_choice58, 0, wxALL, 5 );
	
	m_staticText65 = new wxStaticText( m_panel14, wxID_ANY, wxT("Left side zip"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText65->Wrap( -1 );
	fgSizer14->Add( m_staticText65, 0, wxALL, 5 );
	
	wxArrayString m_choice59Choices;
	m_choice59 = new wxChoice( m_panel14, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice59Choices, 0 );
	m_choice59->SetSelection( 0 );
	fgSizer14->Add( m_choice59, 0, wxALL, 5 );
	
	m_staticText66 = new wxStaticText( m_panel14, wxID_ANY, wxT("Right side zip"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText66->Wrap( -1 );
	fgSizer14->Add( m_staticText66, 0, wxALL, 5 );
	
	wxArrayString m_choice60Choices;
	m_choice60 = new wxChoice( m_panel14, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice60Choices, 0 );
	m_choice60->SetSelection( 0 );
	fgSizer14->Add( m_choice60, 0, wxALL, 5 );
	
	bSizer12->Add( fgSizer14, 1, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer15;
	fgSizer15 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer15->SetFlexibleDirection( wxBOTH );
	fgSizer15->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_button15 = new wxButton( m_panel14, wxID_ANY, wxT("Open z-level file"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer15->Add( m_button15, 0, wxALL, 5 );
	
	
	fgSizer15->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticText67 = new wxStaticText( m_panel14, wxID_ANY, wxT("RoadID (link ID)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText67->Wrap( -1 );
	fgSizer15->Add( m_staticText67, 0, wxALL, 5 );
	
	wxArrayString m_choice61Choices;
	m_choice61 = new wxChoice( m_panel14, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice61Choices, 0 );
	m_choice61->SetSelection( 0 );
	fgSizer15->Add( m_choice61, 0, wxALL, 5 );
	
	m_staticText68 = new wxStaticText( m_panel14, wxID_ANY, wxT("Reference node"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText68->Wrap( -1 );
	fgSizer15->Add( m_staticText68, 0, wxALL, 5 );
	
	wxArrayString m_choice62Choices;
	m_choice62 = new wxChoice( m_panel14, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice62Choices, 0 );
	m_choice62->SetSelection( 0 );
	fgSizer15->Add( m_choice62, 0, wxALL, 5 );
	
	bSizer12->Add( fgSizer15, 1, wxEXPAND, 5 );
	
	m_panel14->SetSizer( bSizer12 );
	m_panel14->Layout();
	bSizer12->Fit( m_panel14 );
	m_notebook2->AddPage( m_panel14, wxT("Navteq input"), false );
	
	bSizer5->Add( m_notebook2, 1, wxEXPAND | wxALL, 5 );
	
	m_button12 = new wxButton( this, wxID_ANY, wxT("Import data"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_button12, 0, wxALL|wxALIGN_RIGHT, 5 );
	
	this->SetSizer( bSizer5 );
	this->Layout();
}

ImporESRI::~ImporESRI()
{
}

MyDialog2::MyDialog2( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
}

MyDialog2::~MyDialog2()
{
}

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
