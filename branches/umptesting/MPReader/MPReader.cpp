// MPReader.cpp : Defines the entry point for the console application.
//
#include <iostream>

#include "mp_mainFrame.h"
#include "wx\wx.h"

class MyApp: public wxApp
{
	MainFrame *frame;
	
	virtual bool OnInit();
	virtual int OnExit();
};

int MyApp::OnExit() {
	//delete frame;
	return 0;
}

bool MyApp::OnInit()
{
	frame = new MainFrame( NULL, wxID_ANY, _T("MP Processor"));//_T("Hello World"), wxPoint(50,50), wxSize(450,340) );
	frame->setApp(this);
	frame->Show(TRUE);
	SetTopWindow(frame);
	return TRUE;
}

IMPLEMENT_APP(MyApp)
/*
using namespace std;

void print_hello(int number) {
  cout << "hello world " << number << endl;
}

int main(int argc, char* argv[])
{

	  lua_State *myLuaState = lua_open();

  // Connect LuaBind to this lua state
  luabind::open(myLuaState);

  // Add our function to the state's global scope
  luabind::module(myLuaState) [
    luabind::def("print_hello", print_hello)
  ];

  // Now call our function in a lua script
  luaL_dostring(
    myLuaState,
    "print_hello(123)\n"
  );

  lua_close(myLuaState);
  
	{
		remove("db");
		MPReader r(argv[1],"db");

		MPProcess process;

		if( r.isOK() ) {
			try {
				r.prepareDB();
				cout<<"Start reading input file"<<endl;
				r.readMP();
				cout<<"Split roads into segments"<<endl;
				process.splitRoadsAtSegments(r.getDB());

				cout<<"Export to out.mp"<<endl;
				MPExport ex;
				ex.exportMP(r.getDB(),"out.mp");
				cout<<"Export to out.shp"<<endl;
				ex.exportSHP(r.getDB(),_polyline,"out");

			} catch( exception &e ) {
				cout<<e.what();
			}
		}
	}

}
*/