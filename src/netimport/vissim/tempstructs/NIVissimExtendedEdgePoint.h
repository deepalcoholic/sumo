#ifndef NIVissimExtendedEdgePoint_h
#define NIVissimExtendedEdgePoint_h
//---------------------------------------------------------------------------//
//                        NIVissimExtendedEdgePoint.h -  ccc
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
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
// Revision 1.3  2005/04/27 12:24:37  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.2  2003/06/05 11:46:57  dkrajzew
// class templates applied; documentation added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H


#include <utils/common/IntVector.h>
#include <utils/geom/Position2D.h>

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class NIVissimExtendedEdgePoint {
public:
    NIVissimExtendedEdgePoint(int edgeid, IntVector &lanes,
        double position, IntVector &assignedVehicles);
    ~NIVissimExtendedEdgePoint();
    int getEdgeID() const;
    double getPosition() const;
    Position2D getGeomPosition() const;
    const IntVector &getLanes() const;
private:
    int myEdgeID;
    IntVector myLanes;
    double myPosition;
    IntVector myAssignedVehicles;
};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

