/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

MDDischMean.c

balazs.fekete@unh.edu

*******************************************************************************/

#include <stdio.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>

static int _MDInAirTempIDID = MFUnset;
static int _MDInAvgNStepsID = MFUnset;

static int _MDOutAirTempMeanID     = MFUnset;

static void _MDAirTempMean (int itemID) {
	int   nSteps;
	float accumDisch;
	float tempMean;

	accumDisch = MFVarGetFloat (_MDInAccumDischargeID,  itemID, 0.0);
	nSteps     = MFVarGetInt   (_MDInAvgNStepsID,       itemID,   0);
	tempMean  = MFVarGetFloat (_MDOutAirTempMeanID,      itemID, 0.0);
	tempMean  = (float) (((double) tempMean * (double) nSteps + accumDisch) / ((double) (nSteps + 1)));
	MFVarSetFloat (_MDOutAirTempMeanID, itemID, tempMean);
}

enum { MDinput, MDcalculate };

int MDAirTempMeanDef () {
	int  optID = MFUnset;
	const char *optStr, *optName = MDVarDischMean;
	const char *options [] = { MDInputStr, MDCalculateStr, (char *) NULL };

	if (_MDOutDischMeanID != MFUnset) return (_MDOutDischMeanID);
	MFDefEntering ("Temperature Mean");

	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);

	switch (optID) {
		case MDinput: _MDOutAirTempMeanID  = MFVarGetID (MDVarDischMean, "deg", MFInput,  MFState, MFBoundary); break;
		case MDcalculate:
			if (((_MDInAvgNStepsID       = MDAvgNStepsDef ())   == CMfailed) ||
			    ((_MDInAccumDischargeID  = MDAccumRunoffDef ()) == CMfailed) ||
			    ((_MDOutAirTempID      = MFVarGetID (MDVarDischMean, "deg", MFOutput, MFState, MFInitial))  == CMfailed) ||
			    (MFModelAddFunction (_MDDischMean) == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving ("Temperature Mean");
	return (_MDOutDischMeanID);
}
