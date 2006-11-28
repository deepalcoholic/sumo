#ifndef GNEViewTraffic_h
#define GNEViewTraffic_h
//---------------------------------------------------------------------------//
//                        GNEViewTraffic.h -
//  A view on the simulation; this view is a microscopic one
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Tue, 15 Dec 2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.7  2006/11/28 12:10:41  dkrajzew
// got rid of FXEX-Mutex (now using the one supplied in FOX)
//
// Revision 1.6  2006/08/01 07:31:35  dkrajzew
// API adaptations
//
// Revision 1.5  2005/10/07 11:38:33  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.4  2005/09/15 12:03:02  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2005/01/31 09:27:35  dkrajzew
// added the possibility to save nodes and edges or the build network to netedit
//
// Revision 1.2  2005/01/05 23:07:04  miguelliebe
// debugging
//
// Revision 1.1  2004/12/15 09:20:19  dkrajzew
// made guisim independent of giant/netedit
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>
#include <utils/geom/Boundary.h>
#include <utils/geom/Position2D.h>
#include <utils/gfx/RGBColor.h>
#include <utils/geom/Position2DVector.h>
#include <utils/shapes/Polygon2D.h>
#include <gui/GUISUMOViewParent.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/drawer/GUIColoringSchemesMap.h>
#include <utils/gui/drawer/GUIBaseLaneDrawer.h>
#include <gui/GUIViewTraffic.h>

#ifdef _WIN32
#include <windows.h>
#include <GL/gl.h>
#endif


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSVehicle;
class GUINet;
class GUISUMOViewParent;
class GUIVehicle;
class GUILaneWrapper;
class GUIEdge;
class GUIPerspectiveChanger;
class GUIBaseVehicleDrawer;
class GUIBaseDetectorDrawer;
class GUIBaseJunctionDrawer;
class GUIBaseROWDrawer;



/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class GNEViewTraffic
 * Microsocopic view at the simulation
 */
class GNEViewTraffic : public GUIViewTraffic {
  FXDECLARE(GNEViewTraffic)
public:
    /// constructor
    GNEViewTraffic(FXComposite *p, GUIMainWindow &app,
        GUISUMOViewParent *parent, GUINet &net, FXGLVisual *glVis);

    /// constructor
    GNEViewTraffic(FXComposite *p, GUIMainWindow &app,
        GUISUMOViewParent *parent, GUINet &net, FXGLVisual *glVis,
        FXGLCanvas *share);
    /// destructor
    virtual ~GNEViewTraffic();

    /// builds the view toolbars
    void buildViewToolBars(GUIGlChildWindow &);

    long onLeftBtnRelease(FXObject*sender,FXSelector selector,void*data);

	long onCmdEditGraph(FXObject*,FXSelector,void*);

protected:

	GUISUMOViewParent *par;


protected:
    GNEViewTraffic() { }
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/


#endif

// Local Variables:
// mode:C++
// End:

