/*

 Created by: cgpsmapper
  
 This is open source software. Source code is under the GNU General Public License version 3.0 (GPLv3) license.

 Permission to modify the code and to distribute modified code is granted,
 provided the above notices are retained, and a notice that the code was
 modified is included with the above copyright notice.

*/
#ifndef __Position_H_
#define __Position_H_

#include "BasicType.h"

class Position {
private:

	static rect_t map_bounds;
public:
	static int	c_iMargin;
	// Size of the window in pixels.
	static long g_lWidth, g_lHeight;
	// Size of the map in pixels.
	static long g_lTotalWidth, g_lTotalHeight;
	// Shift of the window in degrees.
	static float g_X, g_Y;
	// Shift of the window in pixels.
	static long g_lX, g_lY;

	static void SetBounds(const rect_t& map_bounds,const long lWidth,const long lHeight);
	static rect_t* GetBounds() {return &map_bounds;};

	static long XToScreen (const float& _x);
	static long YToScreen (const float& _y);

	static void ScreenToPt (const int& _x, const int& _y, point_t & _point);

	static void GetVisibleRect (rect_t & _rctVisible);

	static void SetX (long _lX);
	static void SetY (long _lY);

	static void SetToCenter (const point_t & _point);
};


#endif // __Position_H_