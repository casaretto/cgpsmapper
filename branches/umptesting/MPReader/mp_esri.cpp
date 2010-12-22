#include "mp_esri.h"
#include "shapelib\shapefil.h"

ImporESRI::ImporESRI( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	wxGridSizer* gSizer7;
	gSizer7 = new wxGridSizer( 1, 3, 0, 0 );
	
	gSizer7->SetMinSize( wxSize( -1,50 ) ); 
	m_button11 = new wxButton( this, wxID_ANY, wxT("Open ESRI file"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer7->Add( m_button11, 0, wxALL, 5 );
	
	m_button17 = new wxButton( this, wxID_ANY, wxT("Import from MapRoute.ini"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer7->Add( m_button17, 0, wxALL, 5 );
	
	m_button18 = new wxButton( this, wxID_ANY, wxT("Store as MapRoute.ini"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer7->Add( m_button18, 0, wxALL, 5 );
	
	bSizer5->Add( gSizer7, 1, wxEXPAND, 5 );
	
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
	
	bSizer5->Add( m_notebook2, 16, wxEXPAND | wxALL, 5 );
	
	m_button12 = new wxButton( this, wxID_ANY, wxT("Import data"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_button12, 0, wxALL|wxALIGN_RIGHT, 5 );
	
	this->SetSizer( bSizer5 );
	this->Layout();
	m_button11->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ImporESRI::OnOpenESRI ), NULL, this );
	m_button13->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ImporESRI::OnOpenSegment ), NULL, this );
	m_button14->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ImporESRI::OnOpenRestrictions ), NULL, this );
	m_button15->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ImporESRI::OnOpenZLevel ), NULL, this );
	
	m_button12->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ImporESRI::OnStartImport ), NULL, this );

}

ImporESRI::~ImporESRI()
{
	m_button11->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ImporESRI::OnOpenESRI ), NULL, this );
	m_button13->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ImporESRI::OnOpenSegment ), NULL, this );
	m_button14->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ImporESRI::OnOpenRestrictions ), NULL, this );
	m_button15->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ImporESRI::OnOpenZLevel ), NULL, this );

	m_button12->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ImporESRI::OnStartImport ), NULL, this );
}

void ImporESRI::setChoice( wxArrayString &choices ) {
	m_choice1->Clear();
	m_choice1->Append(choices);

	m_choice2->Clear();
	m_choice2->Append(choices);

	m_choice3->Clear();
	m_choice3->Append(choices);

	m_choice4->Clear();
	m_choice4->Append(choices);

	m_choice5->Clear();
	m_choice5->Append(choices);

	m_choice6->Clear();
	m_choice6->Append(choices);

	m_choice7->Clear();
	m_choice7->Append(choices);

	m_choice8->Clear();
	m_choice8->Append(choices);

	m_choice9->Clear();
	m_choice9->Append(choices);

	m_choice10->Clear();
	m_choice10->Append(choices);
	//routing

	m_choice11->Clear();
	m_choice11->Append(choices);

	m_choice12->Clear();
	m_choice12->Append(choices);

	m_choice13->Clear();
	m_choice13->Append(choices);

	m_choice14->Clear();
	m_choice14->Append(choices);

	//navteq
	m_choice49->Clear();
	m_choice49->Append(choices);

	m_choice50->Clear();
	m_choice50->Append(choices);

	m_choice51->Clear();
	m_choice51->Append(choices);

	m_choice52->Clear();
	m_choice52->Append(choices);

	m_choice53->Clear();
	m_choice53->Append(choices);

	m_choice54->Clear();
	m_choice54->Append(choices);

	m_choice55->Clear();
	m_choice55->Append(choices);

	m_choice56->Clear();
	m_choice56->Append(choices);

	m_choice57->Clear();
	m_choice57->Append(choices);

	m_choice58->Clear();
	m_choice58->Append(choices);

	m_choice59->Clear();
	m_choice59->Append(choices);

	m_choice60->Clear();
	m_choice60->Append(choices);
}

void ImporESRI::setChoiceZLevel( wxArrayString &choices ) {
	m_choice61->Clear();
	m_choice61->Append(choices);

	m_choice62->Clear();
	m_choice62->Append(choices);
}

void ImporESRI::setChoiceRestrictions( wxArrayString &choices ) {

	m_choice41->Clear();//nodeid1
	m_choice41->Append(choices);

	m_choice42->Clear();//id2
	m_choice42->Append(choices);

	m_choice43->Clear();//id3
	m_choice43->Append(choices);

	m_choice44->Clear();
	m_choice44->Append(choices);

	m_choice45->Clear();
	m_choice45->Append(choices);

	m_choice46->Clear();
	m_choice46->Append(choices);

	m_choice47->Clear();
	m_choice47->Append(choices);
}

void ImporESRI::setChoiceSegment( wxArrayString &choices ) {
//routing - segment
	m_choice16->Clear();
	m_choice16->Append(choices);

	m_choice17->Clear();
	m_choice17->Append(choices);

	m_choice18->Clear();
	m_choice18->Append(choices);

	m_choice19->Clear();
	m_choice19->Append(choices);

	m_choice20->Clear();
	m_choice20->Append(choices);

	m_choice21->Clear();
	m_choice21->Append(choices);

	m_choice22->Clear();
	m_choice22->Append(choices);

	m_choice23->Clear();
	m_choice23->Append(choices);

	m_choice24->Clear();
	m_choice24->Append(choices);

	m_choice25->Clear();
	m_choice25->Append(choices);

	m_choice26->Clear();
	m_choice26->Append(choices);

	m_choice27->Clear();
	m_choice27->Append(choices);

	m_choice28->Clear();
	m_choice28->Append(choices);

	m_choice29->Clear();
	m_choice29->Append(choices);

	m_choice30->Clear();
	m_choice30->Append(choices);

	m_choice31->Clear();
	m_choice31->Append(choices);
//restrykcje

	m_choice32->Clear();//toll
	m_choice32->Append(choices);

	m_choice33->Clear();//emergency
	m_choice33->Append(choices);

	m_choice34->Clear();//delivery
	m_choice34->Append(choices);

	m_choice35->Clear();//car
	m_choice35->Append(choices);

	m_choice36->Clear();//bus
	m_choice36->Append(choices);

	m_choice37->Clear();//taxi
	m_choice37->Append(choices);

	m_choice38->Clear();//pedest
	m_choice38->Append(choices);

	m_choice39->Clear();//bicycyle
	m_choice39->Append(choices);

	m_choice40->Clear();//truck
	m_choice40->Append(choices);

	m_choice48->Clear();//special restr. format
	m_choice48->Append(choices);
}

void ImporESRI::OnOpenRestrictions( wxCommandEvent& event ) {
	DBFHandle		dbf_file;	
	wxArrayString	dbf_fields;
	char			pszFieldName[20];
	int				pnWidth;
	int				pnDecimals;

	restrictions_dbf.clear();
	dbf_fields.Add(_T(" - EMPTY - "));

	wxFileDialog file(this,_T("Select a cGPSmapper Restrictions dbf file"),wxEmptyString,wxEmptyString,_T("*.dbf"));
	if( file.ShowModal() == wxID_OK ) {
		restrictions_dbf = file.GetPath();

		dbf_file = DBFOpen(file.GetPath().char_str(),"r");
		if( dbf_file != NULL ) {
			for( int i=0; i < DBFGetFieldCount(dbf_file); i++ ) {

				DBFGetFieldInfo( dbf_file, i,pszFieldName, &pnWidth, &pnDecimals );

				dbf_fields.Add( wxString::FromUTF8( pszFieldName ) );
			}
			setChoiceRestrictions(dbf_fields);
			DBFClose(dbf_file);
		}
	}
}

void ImporESRI::OnOpenZLevel( wxCommandEvent& event ) {
	DBFHandle		dbf_file;	
	wxArrayString	dbf_fields;
	char			pszFieldName[20];
	int				pnWidth;
	int				pnDecimals;

	zlevel_dbf.clear();
	dbf_fields.Add(_T(" - EMPTY - "));

	wxFileDialog file(this,_T("Select Navteq z-Level dbf file"),wxEmptyString,wxEmptyString,_T("*.dbf"));
	if( file.ShowModal() == wxID_OK ) {
		zlevel_dbf = file.GetPath();

		dbf_file = DBFOpen(file.GetPath().char_str(),"r");
		if( dbf_file != NULL ) {
			for( int i=0; i < DBFGetFieldCount(dbf_file); i++ ) {

				DBFGetFieldInfo( dbf_file, i,pszFieldName, &pnWidth, &pnDecimals );

				dbf_fields.Add( wxString::FromUTF8( pszFieldName ) );
			}
			setChoiceZLevel(dbf_fields);
			DBFClose(dbf_file);
		}
	}
}

void ImporESRI::OnOpenSegment( wxCommandEvent& event ) {
	DBFHandle		dbf_file;	
	wxArrayString	dbf_fields;
	char			pszFieldName[20];
	int				pnWidth;
	int				pnDecimals;

	segment_dbf.clear();
	dbf_fields.Add(_T(" - EMPTY - "));

	wxFileDialog file(this,_T("Select a cGPSmapper Segment / routing dbf file"),wxEmptyString,wxEmptyString,_T("*.dbf"));
	if( file.ShowModal() == wxID_OK ) {
		segment_dbf = file.GetPath();

		dbf_file = DBFOpen(file.GetPath().char_str(),"r");
		if( dbf_file != NULL ) {
			for( int i=0; i < DBFGetFieldCount(dbf_file); i++ ) {

				DBFGetFieldInfo( dbf_file, i,pszFieldName, &pnWidth, &pnDecimals );

				dbf_fields.Add( wxString::FromUTF8( pszFieldName ) );
			}
			setChoiceSegment(dbf_fields);
			DBFClose(dbf_file);
		}
	}
}

void ImporESRI::OnOpenESRI( wxCommandEvent& event ) {
	DBFHandle		dbf_file;	
	wxArrayString	dbf_fields;
	char			pszFieldName[20];
	int				pnWidth;
	int				pnDecimals;

	main_dbf.clear();
	dbf_fields.Add(_T(" - EMPTY - "));

	wxFileDialog file(this,_T("Select an ESRI/dbf file"),wxEmptyString,wxEmptyString,_T("*.dbf"));
	if( file.ShowModal() == wxID_OK ) {
		main_dbf = file.GetPath();

		dbf_file = DBFOpen(file.GetPath().char_str(),"r");
		if( dbf_file != NULL ) {
			for( int i=0; i < DBFGetFieldCount(dbf_file); i++ ) {

				DBFGetFieldInfo( dbf_file, i,pszFieldName, &pnWidth, &pnDecimals );

				dbf_fields.Add( wxString::FromUTF8( pszFieldName ) );
			}
			setChoice(dbf_fields);
			DBFClose(dbf_file);
		}
	}

}

void ImporESRI::OnStartImport( wxCommandEvent& event ) {
	//check data - basics:
	if( !main_dbf.Len() ) {
		wxMessageDialog md(this,_T("Main import file has not been defined."),_T("Error"),wxOK);
		md.ShowModal();
		return;
	}

	if( m_radioBox1->GetSelection() == 1 && !segment_dbf.Len() ) {
		wxMessageDialog md(this,_T("cGPSmapper routing format selected but no segment file has not been defined."),_T("Error"),wxOK);
		md.ShowModal();
		return;
	}

	if( m_radioBox1->GetSelection() == 2 && !zlevel_dbf.Len() ) {
		wxMessageDialog md(this,_T("Navteq routing format selected but no Z-level file has not been defined."),_T("Error"),wxOK);
		md.ShowModal();
		return;
	}
/*		wxString	segment_dbf;
		wxString	restrictions_dbf;
		wxString	zlevel_dbf;
*/
}
