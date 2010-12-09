/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/
#include <stdio.h>
#include <stdarg.h>
#include "ListView.h"

#ifndef _DLL_VERSION

void AddColumn (HWND _hList, int _i, const char * _strText) {
	LVCOLUMN clm;
	clm.mask = LVCF_FMT | LVCF_TEXT;
	clm.fmt = LVCFMT_LEFT;
	clm.pszText = const_cast<char *> (_strText);
	clm.cchTextMax = static_cast<int>(::strlen (_strText));
	ListView_InsertColumn (_hList, _i, & clm);
}

void AutoSizeColumns (HWND _hList) {
	const int cColumns = Header_GetItemCount (ListView_GetHeader (_hList));
	for (int c = 0; c < cColumns; ++ c)
		ListView_SetColumnWidth (_hList, c, LVSCW_AUTOSIZE_USEHEADER);
}

LPARAM GetLParam (HWND _hList, int _cItem) {
	LVITEM lvi;
	lvi.mask = LVIF_PARAM;
	lvi.iItem = _cItem;
	lvi.iSubItem = 0;
	ListView_GetItem (_hList, & lvi);
	return lvi.lParam;
}

void SetSelection (HWND _hList, int _cItem) {
	ListView_SetItemState (_hList, _cItem, LVIS_SELECTED, LVIS_SELECTED);
	ListView_SetSelectionMark (_hList, _cItem);
}

static
int _GetSelection (HWND _hList) {
	// NOTE: ListView_GetSelectionMark() is not good because item sorting does not update sel mark.
	const int cItems = ListView_GetItemCount (_hList);
	for (int c = 0; c < cItems; ++ c)
		if (ListView_GetItemState (_hList, c, LVIS_SELECTED) & LVIS_SELECTED)
			return c;

	return -1;
}

void EnsureSelectionVisible (HWND _hList) {
	const int iSelItem = _GetSelection/*ListView_GetSelectionMark*/ (_hList);
	if (iSelItem != -1)
		ListView_EnsureVisible (_hList, iSelItem, FALSE);
}

void __cdecl InsertItem (HWND _hList, int _cItem, LPARAM _lParam, const char * _strFormat, ...) {
	va_list vl;
	va_start (vl, _strFormat);

	char strBuf [256];
	vsprintf (strBuf, _strFormat, vl);

	LVITEM lvi;
	lvi.mask = LVIF_TEXT | LVIF_PARAM;
	lvi.iItem = _cItem;
	lvi.iSubItem = 0;
	lvi.pszText = strBuf;
	lvi.cchTextMax = sizeof (strBuf)/sizeof (strBuf [0]);
	lvi.lParam = _lParam;
	ListView_InsertItem (_hList, & lvi);

	va_end (vl);
}

void __cdecl SetItemText (HWND _hList, int _cItem, const char * _strFormat, ...) {
	va_list vl;
	va_start (vl, _strFormat);

	char strBuf [256];
	::vsprintf (strBuf, _strFormat, vl);

	LVITEM lvi;
	lvi.mask = LVIF_TEXT;
	lvi.iItem = _cItem;
	lvi.iSubItem = 0;
	lvi.pszText = strBuf;
	lvi.cchTextMax = sizeof (strBuf)/sizeof (strBuf [0]);
	ListView_SetItem (_hList, & lvi);

	va_end (vl);
}

void SetItemIcon (HWND _hList, int _cItem, int _cIconIndex) {
	LVITEM lvi;
	lvi.mask = LVIF_IMAGE;
	lvi.iItem = _cItem;
	lvi.iSubItem = 0;
	lvi.iImage = _cIconIndex;
	ListView_SetItem (_hList, & lvi);
}

void __cdecl SetSubItemText (HWND _hList, int _cItem, int _cSubItem, const char * _strFormat, ...) {
	va_list vl;
	va_start (vl, _strFormat);

	char strBuf [256];
	::vsprintf (strBuf, _strFormat, vl);

	ListView_SetItemText (_hList, _cItem, _cSubItem, strBuf);

	va_end (vl);
}

void SelectColumn (HWND _hList, int _cColumn) {
	const int cRecords = ListView_GetItemCount (_hList);

	LVITEM lvi;
	lvi.mask = LVIF_STATE;
	lvi.stateMask = LVIS_SELECTED | LVIS_FOCUSED;

	for (int cColumn = 0; true; ++ cColumn) {
		LVCOLUMN lvc = {LVCF_SUBITEM};
		if (! ListView_GetColumn (_hList, cColumn, & lvc))
			break;

		lvi.iSubItem = cColumn;
		lvi.state = cColumn == _cColumn ? LVIS_SELECTED | LVIS_FOCUSED : 0;
		for (int cRecord = 0; cRecord < cRecords; ++ cRecord) {
			lvi.iItem = cRecord;
			ListView_SetItem (_hList, & lvi);
		}
	}

	// Ensure the column is visible.
	if (_cColumn != -1) {
		RECT rect;
		::GetClientRect (_hList, & rect);

		RECT rectSub;
		ListView_GetSubItemRect (_hList, 0, _cColumn, LVIR_BOUNDS, & rectSub);

		if (rectSub.left < 0)
			ListView_Scroll (_hList, rectSub.left, 0);
		else if (rectSub.right > rect.right) {
			ListView_Scroll (_hList, rectSub.right - rect.right, 0);

			ListView_GetSubItemRect (_hList, 0, _cColumn, LVIR_BOUNDS, & rectSub);
			if (rectSub.left < 0)
				ListView_Scroll (_hList, rectSub.left, 0);
		}
	}
}

#endif