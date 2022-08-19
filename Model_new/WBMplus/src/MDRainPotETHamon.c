/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

MDPotETHamon.c

balazs.fekete@unh.edu

*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>

static int _MDInDayLengthID = MFUnset;
static int _MDInAtMeanID    = MFUnset;
static int _MDInSnowDepthID = MFUnset;
static int _MDOutPetID      = MFUnset;

static void _MDRainPotETHamon (int itemID) {
// Hamon (1963) PE in mm for day
// Input
	
//	printf ("Anf Hamon \n");
	float dayLen=0;  // daylength in fraction of day
	float airT;		// air temperatur [degree C]
	float Snow;    // Snow depth [mm]
// Local
	float rhoSat;	// saturated vapor density [kg/m3]
	//float lat;  	//latitude
// Output
	float pet;

	dayLen = MFVarGetFloat (_MDInDayLengthID, itemID, 0.1);
	airT   = MFVarGetFloat (_MDInAtMeanID,    itemID,  0.0);
	
	if (Snow < 100){
   		rhoSat  = 2.167 * MDPETlibVPressSat (airT) / (airT + 273.15);
   		pet     = 165.1 * 2.0 * dayLen * rhoSat; // 2 * DAYLEN = daylength as fraction of 12 hours
     		//printf("pet:%f\n",pet);
   		
   	} else{
   		pet = 0;
   		//lat = MFModelGetLatitude (itemID);
   		//printf("lat:%f\n",lat);	
   		//lat = MFModelGetLatitude (itemID) / 180.0 * M_PI; // convert from deg to rad
   		//printf("lat:%f\n",lat);	
   		}
//   rhoSat  = 2.167 * MDPETlibVPressSat (airT) / (airT + 273.15);
//   pet     = 165.1 * 2.0 * dayLen * rhoSat; // 2 * DAYLEN = daylength as fraction of 12 hours 	

   MFVarSetFloat (_MDOutPetID,itemID,pet);
   }

int MDRainPotETHamonDef () {
	if (_MDOutPetID != MFUnset) return (_MDOutPetID);

	MFDefEntering ("Rainfed Potential Evapotranspiration (Hamon)");
	if (((_MDInDayLengthID = MDSRadDayLengthDef ()) == CMfailed) ||
	    ((_MDInAtMeanID    = MFVarGetID (MDVarAirTemperature,    "degC", MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInSnowDepthID  = MFVarGetID (MDVarSnowDepth, "mm", MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutPetID      = MFVarGetID (MDVarRainPotEvapotrans, "mm",   MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
	    (MFModelAddFunction (_MDRainPotETHamon) == CMfailed)) return (CMfailed);
	MFDefLeaving  ("Rainfed Potential Evapotranspiration (Hamon)");
	return (_MDOutPetID);
}
