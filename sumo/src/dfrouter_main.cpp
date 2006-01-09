/***************************************************************************
                          main.cpp
              The main procedure for the build of person/vehicle routes
                             -------------------
    project              : SUMO
    subproject           : router
    begin                : Thu, 06 Jun 2002
    copyright            : (C) 2002 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
namespace
{
    const char rcsid[] =
        "$Id$";
}
// $Log$
// Revision 1.3  2006/01/09 13:33:30  dkrajzew
// debugging error handling
//
// Revision 1.2  2005/12/21 12:48:38  ericnicolay
// *** empty log message ***
//
// Revision 1.1  2005/12/08 12:51:15  ericnicolay
// add new workingmap for the dfrouter
//
// Revision 0.1  2005/12/06 14:51:08  dkrajzew
// first start
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
#include <sax/SAXException.hpp>
#include <sax/SAXParseException.hpp>
#include <utils/common/TplConvert.h>
#include <iostream>
#include <string>
#include <fstream>
#include <limits.h>
#include <ctime>
#include <router/ROLoader.h>
#include <router/RONet.h>
#include <router/ROVehicleType_Krauss.h>
#include <routing_df/RODijkstraRouter.h>
#include <routing_df/RODFEdgeBuilder.h>
#include <router/ROFrame.h>
#include <utils/common/MsgHandler.h>
#include <utils/options/Option.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsIO.h>
#include <utils/options/OptionsSubSys.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/SystemFrame.h>
#include <utils/common/HelpPrinter.h>
#include <utils/common/ToString.h>
#include <utils/xml/XMLSubSys.h>
#include <routing_df/RODFFrame.h>
#include <routing_df/DFRONet.h>
#include "dfrouter_help.h"
#include "dfrouter_build.h"
#include "sumo_version.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * functions
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * data processing methods
 * ----------------------------------------------------------------------- */
/**
 * loads the net
 * The net is in this meaning made up by the net itself and the dynamic
 * weights which may be supplied in a separate file
 */
DFRONet *
loadNet(ROLoader &loader, OptionsCont &oc)
{
    // load the net
    RODFEdgeBuilder builder;
    DFRONet *net = new DFRONet( loader.loadNet( builder ) );
    if(net==0) {
        return 0;
    }

    return net;
}


/**
 * Computes the routes saving them
 */
void
startComputation(RONet &net, ROLoader &loader, OptionsCont &oc)
{
    // prepare the output
    net.openOutput(
        oc.getString("output"), true);
    // build the router
    RODijkstraRouter router(net);
    // initialise the loader
    loader.openRoutes(net, oc.getFloat("gBeta"), oc.getFloat("gA"));
    // the routes are sorted - process stepwise
    if(!oc.getBool("unsorted")) {
        loader.processRoutesStepWise(
            oc.getInt("b"), oc.getInt("e"), net, router);
    }
    // the routes are not sorted: load all and process
    else {
        loader.processAllRoutes(
            oc.getInt("b"), oc.getInt("e"), net, router);
    }
    // end the processing
    loader.closeReading();
    net.closeOutput();
}


/* -------------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */
int
main(int argc, char **argv)
{
    int ret = 0;
    DFRONet *net = 0;
#ifndef _DEBUG
    try {
#endif
        // initialise the application system (messaging, xml, options)
        int init_ret = SystemFrame::init(false, argc, argv,
			RODFFrame::fillOptions_fullImport);
        if(init_ret<0) {
            cout << "SUMO dfrouter" << endl;
            cout << " (c) DLR/ZAIK 2000-2006; http://sumo.sourceforge.net" << endl;
            switch(init_ret) {
            case -1:
                cout << " Version " << version << endl;
                cout << " Build #" << NEXT_BUILD_NUMBER << endl;
                break;
            case -2:
                HelpPrinter::print(help);
                break;
            default:
                cout << " Use --help to get the list of options." << endl;
            }
            SystemFrame::close();
            return 0;
        } else if(init_ret!=0||!RODFFrame::checkOptions(OptionsSubSys::getOptions())) {
            throw ProcessError();
        }
        // retrieve the options
        OptionsCont &oc = OptionsSubSys::getOptions();
		ROFrame::setDefaults(oc);
        // load data
        ROVehicleBuilder vb;
        ROLoader loader(oc, vb, false);
        net = loadNet(loader, oc);
        if(net!=0) {
            // build routes
			try {net->getApproach() ;//startComputation(*net, loader, oc);
            } catch (SAXParseException &e) {
                MsgHandler::getErrorInstance()->inform( toString<int>(e.getLineNumber()));
                ret = 1;
            } catch (SAXException &e) {MsgHandler::getErrorInstance()->inform(TplConvert<XMLCh>::_2str(e.getMessage()));
                ret = 1;
            }
        } else {
            ret = 1;
        }
#ifndef _DEBUG
    } catch (...) {
        MsgHandler::getErrorInstance()->inform("Quitting (on error).", false);
        ret = 1;
    }
#endif
    delete net;
    SystemFrame::close();
    if(ret==0) {
        cout << "Success." << endl;
    }
    return ret;
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

