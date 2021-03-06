/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDTempSurfRunoff.c

wil.wollheim@unh.edu

This module calculates the temperature of surface runoff and infiltration to groundwater
Irrigation inputs are not accounted here.

*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>
#include <math.h>

// Input
static int _MDInAirTempID  = MFUnset;
static int _MDInPrecipID   = MFUnset;
static int _MDInSnowMeltID = MFUnset;
static int _MDInSnowPackID = MFUnset;
// Output
static int _MDOutWTempSurfROID   = MFUnset;

static void _MDWTempSurfRunoff (int itemID) {
	float airT;
	float SnowT = 0;
	float SurfWatT;
	float SnowPack;

	float SnowIn;
	float PrecipIn;

	 
    airT               = MFVarGetFloat (_MDInAirTempID,         itemID, 0.0);
    SnowPack           = MFVarGetFloat (_MDInSnowPackID,        itemID, 0.0);
    SnowIn             = MFVarGetFloat (_MDInSnowMeltID,        itemID, 0.0);
    PrecipIn           = MFVarGetFloat (_MDInPrecipID,          itemID, 0.0);
   
    if (!isnan(SnowIn) && !isnan(PrecipIn) && !isnan(SnowT) && !isnan(airT) && (SnowIn + PrecipIn) > 0.0){
        if (SnowPack > 0) {SurfWatT = 0;}
        else{
    	SurfWatT = MDMaximum((SnowIn * SnowT + PrecipIn * airT) / (SnowIn + PrecipIn), 0.0);
        }
    	MFVarSetFloat (_MDOutWTempSurfROID,itemID,SurfWatT); 
    }
    else{
  	    MFVarSetMissingVal(_MDOutWTempSurfROID,itemID);
    }
    //if (airT > 25){
   // 	printf("SurfWaterT = SurfWatT %f \n", SurfWatT);
   // }
    	
    
}

int MDWTempSurfRunoffDef () {

	if (_MDOutWTempSurfROID != MFUnset) return (_MDOutWTempSurfROID);

	MFDefEntering ("Surface runoff temperature");

	if (((_MDInPrecipID       = MDPrecipitationDef ()) == CMfailed) ||
	    ((_MDInSnowMeltID     = MDSPackMeltDef     ()) == CMfailed) ||
	    ((_MDInSnowPackID     = MFVarGetID (MDVarSnowPack,        "mm",   MFInput,  MFState, MFBoundary))  == CMfailed) ||
	    ((_MDInAirTempID      = MFVarGetID (MDVarAirTemperature,  "degC", MFInput,  MFState, MFBoundary))  == CMfailed) ||
	    ((_MDOutWTempSurfROID = MFVarGetID (MDVarWTempSurfRunoff, "degC", MFOutput, MFState, MFBoundary)) == CMfailed) ||	
        (MFModelAddFunction (_MDWTempSurfRunoff) == CMfailed)) return (CMfailed);

	MFDefLeaving ("Surface runoff temperature");
	return (_MDOutWTempSurfROID);
}
