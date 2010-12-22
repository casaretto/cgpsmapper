#ifndef _MP_ESRI_FRAME
#define _MP_ESRI_FRAME

#include "wx/wx.h"
#include "wx/notebook.h"

class ImporESRI : public wxDialog 
{
	private:
	
	protected:
		wxButton* m_button11;
		wxButton* m_button17;
		wxButton* m_button18;
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

		virtual void OnOpenESRI( wxCommandEvent& event );
		virtual void OnOpenSegment( wxCommandEvent& event );
		virtual void OnOpenRestrictions( wxCommandEvent& event );
		virtual void OnOpenZLevel( wxCommandEvent& event );

		virtual void OnStartImport( wxCommandEvent& event );

		void		setChoice( wxArrayString &choices );
		void		setChoiceSegment( wxArrayString &choices );
		void		setChoiceRestrictions( wxArrayString &choices );
		void		setChoiceZLevel( wxArrayString &choices );
		
		wxString	main_dbf;
		wxString	segment_dbf;
		wxString	restrictions_dbf;
		wxString	zlevel_dbf;
	public:
		ImporESRI( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Import ESRI"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 557,687 ), long style = wxCAPTION|wxDEFAULT_DIALOG_STYLE );
		~ImporESRI();
	
};


#endif