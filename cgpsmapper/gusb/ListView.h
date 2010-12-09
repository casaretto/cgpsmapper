/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/
# ifndef __ListView_H_
# define __ListView_H_

#include <windows.h>
#include "commctrl.h"

#ifndef _DLL_VERSION

void AddColumn       (HWND _hList, int _i, const char * _strText);
void AutoSizeColumns (HWND _hList);
void __cdecl InsertItem      (HWND _hList, int _cItem, LPARAM _lParam, const char * _strFormat, ...);
void __cdecl SetItemText     (HWND _hList, int _cItem, const char * _strFormat, ...);
void         SetItemIcon     (HWND _hList, int _cItem, int _cIconIndex);
void __cdecl SetSubItemText  (HWND _hList, int _cItem, int _cSubItem, const char * _strFormat, ...);
LPARAM GetLParam     (HWND _hList, int _cItem);
void SetSelection    (HWND _hList, int _cItem);
void EnsureSelectionVisible (HWND _hList);
void SelectColumn    (HWND _hList, int _cColumn);

#endif
# endif // __ListView_H_