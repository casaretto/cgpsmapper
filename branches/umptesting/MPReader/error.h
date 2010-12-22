#ifndef __ERROR_H
#define __ERROR_H

#ifdef _WIN32
#include <windows.h>
#endif

#include "wx/wx.h"

const int _message_max = 90;
const int _message_len = 150;

class errorClass {
	static char		warn_messages[_message_max][_message_len];
	static wxTextCtrl* txtControl;

public:
	static bool		show_message_error;

	void static redirectMessages(wxTextCtrl* _txtControl);
	void static showError(int line,const char* file_name,const char* code);
};

#endif