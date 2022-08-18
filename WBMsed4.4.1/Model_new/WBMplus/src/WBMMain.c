/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

WBMMain.c

balazs.fekete@unh.edu

*******************************************************************************/
#include "wbm.h"

enum { MDpet, MDsurplus, MDinfiltration, MDrunoff, MDdischarge,  MDwatertemp, MDthermal, MDbalance, MDgeometry, MDbgc, MDbgc_DIN, MDbgc_DINPLUSBIOMASS, MDbgc_DOC, MDfecal, MDsedimentflux, MDsedimentflux_noBF, MDbedloadflux,MDBQARTinputs, MDBQARTpreprocess,MDbankfullQcalc,MDRiverbedShapeExponent, MDparticulatenutrients,MDparticulatenutrients_noBF, MDwaterdensity};

int main (int argc,char *argv []) {
	int argNum;
	int  optID = MDbalance;
	const char *optStr, *optName = MDOptModel;
	const char *options [] = { "pet", "surplus", "infiltration", "runoff", "discharge",  "watertemp", "thermal", "balance", "geometry", "bgc", "bgc_DIN","bgc_DINPLUSBIOMASS", "bgc_DOC", "fecal", "sedimentflux","sedimentflux_nobf","bedloadflux", "BQARTinputs" , "BQARTpreprocess", "bankfullQcalc", "riverbedshape", "particulatenutrients","particulatenutrients_nobf", "waterdensity", (char *) NULL };

	argNum = MFOptionParse (argc,argv);

	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);

	switch (optID) {
		case MDpet:          return (MFModelRun (argc,argv,argNum,MDRainPotETDef));
		case MDsurplus:      return (MFModelRun (argc,argv,argNum,MDRainWaterSurplusDef));
		case MDinfiltration: return (MFModelRun (argc,argv,argNum,MDRainInfiltrationDef));
		case MDrunoff:       return (MFModelRun (argc,argv,argNum,MDRunoffDef));
		case MDdischarge:    return (MFModelRun (argc,argv,argNum,MDDischargeDef));
		case MDbalance:      return (MFModelRun (argc,argv,argNum,MDWaterBalanceDef));
		case MDwatertemp:    return (MFModelRun (argc,argv,argNum,MDWTempRiverRouteDef));
		case MDthermal:		return (MFModelRun (argc,argv,argNum,MDThermalInputsDef));		// RJS 013112
		case MDgeometry:     return (MFModelRun (argc,argv,argNum,MDRiverWidthDef));
		case MDbgc:          return (MFModelRun (argc,argv,argNum,MDBgcRoutingDef));
		case MDbgc_DOC:      return (MFModelRun (argc,argv,argNum,MDBgcDOCRoutingDef));
		case MDbgc_DIN:      return (MFModelRun (argc,argv,argNum,MDBgcDINRoutingDef));
		case MDbgc_DINPLUSBIOMASS:    return (MFModelRun (argc,argv,argNum,MDBgcDINPlusBiomassRoutingDef));
		case MDsedimentflux:    return (MFModelRun (argc,argv,argNum,MDSedimentFluxDef));
		//case MDsedimentflux_noBF:    return (MFModelRun (argc,argv,argNum,MDSedimentFlux_noBFDef));
		case MDbedloadflux:    return (MFModelRun (argc,argv,argNum,MDBedloadFluxDef));	
		//case MDBQARTpreprocess:    return (MFModelRun (argc,argv,argNum,MDBQARTpreprocessDef));	
		//case MDbankfullQcalc:    return (MFModelRun (argc,argv,argNum,MDBankfullQcalcDef));	
		case MDRiverbedShapeExponent:    return (MFModelRun (argc,argv,argNum,MDRiverbedShapeExponentDef));
		case MDparticulatenutrients:    return (MFModelRun (argc,argv,argNum,MDParticulateNutrientsDef));
		//case MDparticulatenutrients_noBF:    return (MFModelRun (argc,argv,argNum,MDParticulateNutrients_noBFDef));
		case MDwaterdensity:    return (MFModelRun (argc,argv,argNum,MDWaterDensityDef));
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	return (CMfailed);
}
