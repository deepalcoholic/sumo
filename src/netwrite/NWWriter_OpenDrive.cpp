/****************************************************************************/
/// @file    NWWriter_OpenDrive.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 04.05.2011
/// @version $Id$
///
// Exporter writing networks using the openDRIVE format
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif
#include "NWWriter_OpenDrive.h"
#include <utils/common/MsgHandler.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBNetBuilder.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/common/StdDefs.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS



// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static methods
// ---------------------------------------------------------------------------
void
NWWriter_OpenDrive::writeNetwork(const OptionsCont &oc, NBNetBuilder &nb) {
    // check whether a matsim-file shall be generated
    if (!oc.isSet("opendrive-output")) {
        return;
    }
    // some internal mapping containers
    int edgeID = 0;
    int nodeID = 0;
    StringBijection<int> edgeMap;
    StringBijection<int> nodeMap;
    //
    OutputDevice& device = OutputDevice::getDevice(oc.getString("opendrive-output"));
    device << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    device << "<OpenDRIVE>\n";
    device << "    <header revMajor=\"1\" revMinor=\"3\" name=\"\" version=\"1.00\" date=\"!!!\" north=\"0.0000000000000000e+00\" south=\"0.0000000000000000e+00\" east=\"0.0000000000000000e+00\" west=\"0.0000000000000000e+00\" maxRoad=\"517\" maxJunc=\"2\" maxPrg=\"0\"/>\n";
    // write normal edges (road)
    const NBEdgeCont &ec = nb.getEdgeCont();
    for (std::map<std::string, NBEdge*>::const_iterator i=ec.begin(); i!=ec.end(); ++i) {
        const NBEdge *e = (*i).second;
        device << "    <road name=\"" << e->getStreetName() << "\" length=\"" << e->getLength() << "\" id=\"" << getID(e->getID(), edgeMap, edgeID) << "\" junction=\"-1\">\n";
        device << "        <link>\n";
        device << "            <predecessor elementType=\"junction\" elementId=\"" << getID(e->getFromNode()->getID(), nodeMap, nodeID) << "\"/>\n";
        device << "            <successor elementType=\"junction\" elementId=\"" << getID(e->getToNode()->getID(), nodeMap, nodeID) << "\"/>\n";
        device << "        </link>\n";
        device << "        <type s=\"0\" type=\"town\"/>\n";
        writePlanView(e->getGeometry(), device);
        device << "        <elevationProfile><elevation s=\"0\" a=\"0\" b=\"0\" c=\"0\" d=\"0\"/></elevationProfile>\n";
        device << "        <lateralProfile></lateralProfile>\n";
        device << "        <lanes>\n";
        device << "            <laneSection s=\"0\">\n";
        writeEmptyCenterLane(device);
        device << "                <right>\n";
        const std::vector<NBEdge::Lane> &lanes = e->getLanes();
        for(int j=e->getNumLanes(); --j>=0;) {
            device << "                    <lane id=\"-" << e->getNumLanes()-j <<"\" type=\"driving\" level=\"0\">\n";
            device << "                        <link>\n";
            device << "                            <predecessor id=\"-1\"/>\n"; // internal roads have this
            device << "                            <successor id=\"-1\"/>\n"; // internal roads have this
            device << "                        </link>\n";
            device << "                        <width sOffset=\"0\" a=\"" << lanes[j].width << "\" b=\"0\" c=\"0\" d=\"0\"/>\n";
            device << "                        <roadMark sOffset=\"0\" type=\"broken\" weight=\"standard\" color=\"standard\" width=\"0.13\"/>\n";
            device << "                    </lane>\n";
        }
        device << "                 </right>\n";
        device << "            </laneSection>\n";
        device << "        </lanes>\n";
        device << "        <objects></objects>\n";
        device << "        <signals></signals>\n";
        device << "    </road>\n";
    }
    device << "\n";
    // write junction-internal edges (road)
    const NBNodeCont &nc = nb.getNodeCont();
    for (std::map<std::string, NBNode*>::const_iterator i=nc.begin(); i!=nc.end(); ++i) {
        NBNode *n = (*i).second;
        unsigned int index = 0;
        const std::vector<NBEdge*> &incoming = (*i).second->getIncomingEdges();
        for(std::vector<NBEdge*>::const_iterator j=incoming.begin(); j!=incoming.end(); ++j) {
            const std::vector<NBEdge::Connection> &elv = (*j)->getConnections();
            for(std::vector<NBEdge::Connection>::const_iterator k=elv.begin(); k!=elv.end(); ++k) {
                if((*k).toEdge==0) {
                    continue;
                }
                const NBEdge::Connection &c = *k;
                PositionVector shape = c.shape;
                if(c.haveVia) {
                    shape.appendWithCrossingPoint(c.viaShape);
                }
                device << "    <road name=\"" << c.id << "\" length=\"" << shape.length() << "\" id=\"" << getID(c.id, edgeMap, edgeID) << "\" junction=\"" << getID(n->getID(), nodeMap, nodeID) << "\">\n";
                device << "        <link>\n";
                device << "            <predecessor elementType=\"road\" elementId=\"" << getID((*j)->getID(), edgeMap, edgeID) << "\"/>\n";
                device << "            <successor elementType=\"road\" elementId=\"" << getID((*k).toEdge->getID(), edgeMap, edgeID) << "\"/>\n";
                device << "        </link>\n";
                device << "        <type s=\"0\" type=\"town\"/>\n";
                writePlanView(shape, device);
                device << "        <elevationProfile><elevation s=\"0\" a=\"0\" b=\"0\" c=\"0\" d=\"0\"/></elevationProfile>\n";
                device << "        <lateralProfile></lateralProfile>\n";
                device << "        <lanes>\n";
                device << "            <laneSection s=\"0\">\n";
                writeEmptyCenterLane(device);
                device << "                <right>\n";
                device << "                    <lane id=\"-1\" type=\"driving\" level=\"0\">\n";
                device << "                        <link>\n";
                //device << "                            <predecessor id=\"1\"/>\n";// !!!
                //device << "                            <successor id=\"-1\"/>\n";// !!!
                device << "                        </link>\n";
                device << "                        <width sOffset=\"0\" a=\"" << SUMO_const_laneWidth << "\" b=\"0\" c=\"0\" d=\"0\"/>\n";
                device << "                        <roadMark sOffset=\"0\" type=\"broken\" weight=\"standard\" color=\"standard\" width=\"0.13\"/>\n";
                device << "                    </lane>\n";
                device << "                 </right>\n";
                device << "            </laneSection>\n";
                device << "        </lanes>\n";
                device << "        <objects></objects>\n";
                device << "        <signals></signals>\n";
                device << "    </road>\n";
            }
        }
    }

    // write junctions (junction)
    for (std::map<std::string, NBNode*>::const_iterator i=nc.begin(); i!=nc.end(); ++i) {
        NBNode *n = (*i).second;
        device << "    <junction name=\"" << n->getID() << "\" id=\"" << getID(n->getID(), nodeMap, nodeID) << "\">\n";
        unsigned int index = 0;
        const std::vector<NBEdge*> &incoming = n->getIncomingEdges();
        for(std::vector<NBEdge*>::const_iterator j=incoming.begin(); j!=incoming.end(); ++j) {
            const std::vector<NBEdge::Connection> &elv = (*j)->getConnections();
            for(std::vector<NBEdge::Connection>::const_iterator k=elv.begin(); k!=elv.end(); ++k) {
                if((*k).toEdge==0) {
                    continue;
                }
                device << "    <connection id=\"" << index << "\" incomingRoad=\"" << getID((*j)->getID(), edgeMap, edgeID)  
                    << "\" connectingRoad=\"" << getID((*k).id, edgeMap, edgeID) << "\" contactPoint=\"start\"/>\n";
                ++index;
            }
        }
        device << "    </junction>\n";
    }

    device << "</OpenDRIVE>\n";
    device.close();
}


void 
NWWriter_OpenDrive::writePlanView(const PositionVector &shape, OutputDevice& device) {
    device << "        <planView>\n";
    SUMOReal offset = 0;
    for(unsigned int j=0; j<shape.size()-1; ++j) {
        const Position &p = shape[j];
        Line l = shape.lineAt(j);
        device << "            <geometry s=\"" << offset << "\" x=\"" << p.x() << "\" y=\"" << p.y() << "\" hdg=\"" << l.atan2Angle() << "\" length=\"" << l.length() << "\"><line/></geometry>\n";
        offset += l.length();
    }
    device << "        </planView>\n";
}


void
NWWriter_OpenDrive::writeEmptyCenterLane(OutputDevice& device) {
    device << "                <center>\n";
    device << "                    <lane id=\"0\" type=\"driving\" level= \"0\">\n";
    device << "                        <link></link>\n";
    device << "                        <roadMark sOffset=\"0\" type=\"solid\" weight=\"standard\" color=\"standard\" width=\"0.13\"/>\n";
    device << "                    </lane>\n";
    device << "                </center>\n";
}


int 
NWWriter_OpenDrive::getID(const std::string &origID, StringBijection<int> &map, int &lastID) {
    if(map.hasString(origID)) {
        return map.get(origID);
    }
    map.insert(origID, lastID++);
    return lastID - 1;
}


/****************************************************************************/

