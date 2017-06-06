#pragma once

#define TWM_VIEWEXPAND (WM_APP + 0)
#define TWM_VIEWCOLLAPCE (WM_APP + 1)
#define	TWM_VIEWCLOSE (WM_APP + 2)
#define	TWM_VIEWSHOW (WM_APP + 3)

#define TWM_DRAG_ENTER	(WM_APP + 3)			// WPARAM: COleDataObject*, LPARAM: DragParameters* 
#define TWM_DRAG_LEAVE	(WM_APP + 4)			// WPARAM: unused,          LPARAM: unused 
#define TWM_DRAG_OVER	(WM_APP + 5)			// WPARAM: COleDataObject*, LPARAM: DragParameters* 
#define TWM_DROP_EX		(WM_APP + 6)			// WPARAM: COleDataObject*, LPARAM: DragParameters* 
#define TWM_DROP		(WM_APP + 7)			// WPARAM: COleDataObject*, LPARAM: DragParameters* 
#define TWM_DRAG_SCROLL	(WM_APP + 8)			// WPARAM: dwKeyState,      LPARAM: point 
#define TWM_DO_SCROLL	(WM_APP + 9)			// WPARAM: vertical steps,  LPARAM: horizonal steps 
