/******************************************************************************
Calculating the Bedload Flux using the modified Begnold(1966) equation

MDBedloadFlux.c

Updated 3/28/2019 to include critical velocity parameter

Eqations:
Modified Bagnold (1966): [Ps/Ps-P][(Pg(Q^b)S)/(g tan(f)] ~~ constant slope (on/off)
Lammers & Bledsoe (2018): 0.004((PgQS/w)-0.1]^1.5)(Ds^-0.5)(Q/w)-0.5)*w ~~constant slope (on/off)
Ashley: 45.7(Q^0.69)(S^0.88)(Qs^0.31); Empirical was: 0.02(Q^0.7)(Qs^0.25)(V^-1.2)
Syvitski et al., (2019): (Ps/Ps-Pf)pf (Q^b)S (0.01 V/u)
*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>
#include <math.h>
#include <stdlib.h>

// Input
static int _MDInDischargeID   		= MFUnset;
static int _MDInDischMeanID	  	   	= MFUnset;
static int _MDInRiverSlopeID	    = MFUnset;
//static int _MDInUpStreamQbID	    = MFUnset;
//static int _MDInMDVarVelocityTHID	    = MFUnset;
static int _MDInRiverbedVelocityMeanID  = MFUnset;
static int _MDInSedimentFluxID = MFUnset;
//static int _MDInRiverbedWidthMeanID = MFUnset;
static int _MDInMDVarBedloadEquationID = MFUnset;
static int _MDInWTemp_QxT 			= MFUnset;
//static int _MDInRiverWidthID 	= MFUnset;
//static int _MDInWaterDensityID	= MFUnset;
static int _MDInQsConcID			= MFUnset;
static int _MDInQs_barID 			= MFUnset;
// Output
static int _MDOutBedloadFluxID 		= MFUnset;
static int _MDOutSettlingVelocityID = MFUnset;
static int _MDOutKinematicViscosityID = MFUnset;
//static int _MDOutDeltaBedloadID 	= MFUnset;
static int _MDOutWaterDensityID		= MFUnset;
static int _MDOutStreamPowerID		= MFUnset;
static int _MDOutCritStreamPowerID	= MFUnset;
static int _MDOutParticleSizeID		= MFUnset;
static int _MDOutQb_bar_AshleyID	= MFUnset;

static void _MDBedloadFlux (int itemID) {
	float trnfac, rslope,Qday, Qsbar, DischMean, alphabed, Qb_bar_ashley;//, Qsday, pixel_length; 
	float rhofluid, rhosand, trneff,anglerep,degTOr;
	float Qb_kg_sec;//, Qb_kg_day,velocityTH;
	//float upstream_Qb, deltaQb;
	float  vMean, settV, kinematicV, nomi,rho;
	float Qc,Ds,constDs, d, constSlope, streamPower, critStreamPower, rwidth, Tw, constTw;
	float g = 9.8;
	int bedloadEq;
	// Set parameters
	
	rhosand	 = 2670;			// Sand density
	trneff	 = 0.1;  			// Bedload efficency - changed from 0.1 on April 2019
	anglerep = 32.21;			// Limiting angle
	degTOr 	 = 1.745329252e-2;	// 2.0*PI/360.0 convert degrees to radians 
	alphabed = 1.0; 			// Q coefficient 
	constSlope = 0.001;			// river slope
	constDs = 0.001; 			// particle size (m)
	constTw = 20; 				// water temperature
	critStreamPower = 0.1; 		//critical stream power k/m/s, W/m2
	//velocityTH = MFVarGetFloat (_MDInMDVarVelocityTHID, itemID, 0.0);
	//vMean     = MFVarGetFloat (_MDInRiverbedVelocityMeanID,  itemID, 0.0);
	
	DischMean = MFVarGetFloat (_MDInDischMeanID, 	itemID, 0.0);	
	//rwidth = 15.0 * pow(DischMean,0.5); //from Cohen et al. (2014)
	rwidth = 2.15 * pow(DischMean,0.67);
	//MFVarSetFloat (_MDOutSettlingVelocityID, itemID, rwidth);
	Qday = MFVarGetFloat (_MDInDischargeID ,itemID, 0.0);	// in m3/s	
	//Qsday = MFVarGetFloat (_MDInSedimentFluxID ,itemID, 0.0);
	Qsbar = MFVarGetFloat (_MDInQs_barID ,itemID, 0.0);
	rslope = MFVarGetFloat (_MDInRiverSlopeID,itemID, 0.0);// in %	
	//Tw = constTw;
	Tw =  MFVarGetFloat (_MDInWTemp_QxT ,itemID, 0.0);	// simulated water temperature in degC
	Qc = MFVarGetFloat(_MDInQsConcID ,itemID, 0.0);			// Sediment Concentration
	rhofluid = 1000; 			// Constant Fluid density
	if (Qc > 0.0 ){
		rho = 1000*(1 - (Tw+288.9414)/(508929.2*(Tw+68.12963))* pow((Tw-3.9863),2));
		//rhofluid = rho + Qc;
		rhofluid = rho;
	}
	MFVarSetFloat (_MDOutWaterDensityID, itemID, rhofluid);
	
	//Qb_bar_ashley = 2818 * pow(DischMean, 1.01) * pow(rslope, 1.49);
	//MFVarSetFloat (_MDOutQb_bar_AshleyID, itemID, Qb_bar_ashley);
	//Ds = (29911 * pow(rslope, 1.11867)) / 1000.0; // calculated sediment particle size (m)	
	//Ds = (2.45 * pow(DischMean, 0.76) * pow(rslope, 1.05) * pow(Qsbar,-0.5));// by Tom Ashley
	//Ds = 0.66 * pow(DischMean, 0.76) * pow(rslope, 1.12) * pow((Qsbar + Qb_bar_ashley),-0.39);// in [m]; by Tom Ashley (Apr 2020)
	//MFVarSetFloat (_MDOutQb_bar_AshleyID, itemID, Qsbar);
	//MFVarSetFloat (_MDOutStreamPowerID, itemID, DischMean);
	//MFVarSetFloat (_MDOutCritStreamPowerID, itemID, rslope);
//	Ds = 0.66 * pow(DischMean, 0.76) * pow(rslope, 1.12) * pow(Qsbar,-0.39);// in [m]; by Tom Ashley (Oct 2021)
	Ds = 3.77 * pow(DischMean/rwidth, 1.42) * pow(rslope, 1.26) * pow(Qsbar/rwidth,-0.5);// in [m]; by Tom Ashley (Oct 2021)

	MFVarSetFloat (_MDOutParticleSizeID, itemID, Ds);
	
	streamPower = (rhofluid * g * Qday * rslope) / rwidth; 
	critStreamPower = 0.1*(rhofluid * pow(((2.65-1)*g*Ds),1.5));
	//MFVarSetFloat (_MDOutStreamPowerID, itemID, streamPower);
	//MFVarSetFloat (_MDOutCritStreamPowerID, itemID, critStreamPower);
	
	Qb_kg_sec = 0.0;
	
	if (streamPower > critStreamPower){
	//if (vMean > velocityTH){
		bedloadEq = MFVarGetInt (_MDInMDVarBedloadEquationID, itemID, 0.0);
		//rslope = constSlope;
		//Ds = constDs;
		switch(bedloadEq){
			case 1:
	//Modified Bagnold~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			//	printf("in Bagnold");
				trnfac = (rhofluid * rhosand * trneff) / ((rhosand - rhofluid) * tan(anglerep*degTOr));
				MFVarSetFloat (_MDOutKinematicViscosityID, itemID, trnfac);
				Qb_kg_sec = trnfac * rslope * pow(Qday, alphabed); // in kg/s 
				break;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			case 2:
	//Lammers & Bledsoe (2018): 0.000004((PgQS/w)-0.1]^1.5)(Ds^-0.5)(Q/w)-0.5)*w ~~~~~~~~~~~~~~~~~~~~
			//	printf("in Lammers & Bledsoe (2018)");
		//		Qb_kg_sec = (0.000143 * pow((streamPower - critStreamPower), 1.5) * pow(Ds,-0.5)* pow((Qday/rwidth),-0.5)) * rwidth;
				Qb_kg_sec = (0.00044 * pow((streamPower - critStreamPower), 1.5) * pow(Ds,-0.5)* pow((Qday/rwidth),-0.5)) * rwidth;

				break;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~	
			case 3:	
	//Ashley: 45.7(Q^0.69)(S^0.88)(Qs^0.31); OLD empirical0.02(Q^0.7)(Qs^0.25)(V^-1.2)~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
				Qb_kg_sec  = 2818 * pow(Qday, 1.01) * pow(rslope, 1.49);
				//Qb_kg_sec = 45.7 * pow(Qday,0.69) * pow(Qsday,0.31) * pow(rslope,0.88);

				break;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~	
			case 4:	
	//Syvitski et al. (2019): (Ps/Ps-Pf)pf (Q^b)S (0.01 V/u) ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
				//printf("in Syvitski et al. (2019)");
	//			kinematicV = (0.001791/(1+ (0.03368*Tw) + (0.00021 * pow(Tw,2))))/rhofluid;
	//			nomi = ((rhosand/rhofluid) - 1.0) * g;
	//			d = Ds * pow((nomi / (pow(kinematicV,2.0))), (1.0/3.0)); 
	//			settV = ((8.0 * kinematicV)/Ds) * (pow(1.0 + (0.139 * pow(d, 3.0)),0.5) - 1.0);
	//			Qb_kg_sec = ((rhosand/(rhosand-rhofluid))*rhofluid) * pow(Qday, alphabed) * rslope * (0.01*(vMean/settV));
	//			MFVarSetFloat (_MDOutSettlingVelocityID, itemID, settV);
	//			MFVarSetFloat (_MDOutKinematicViscosityID, itemID, kinematicV);
				break;	
				
		}
	}
	if(Qb_kg_sec < 0.0) Qb_kg_sec = 0.0 ; // in kg/s  
//	if (Qb_kg_sec != Qb_kg_sec && Qday > 10000) printf("2 emID = %d, Qb_kg_sec = %f, Qday = %f, Qsday = %f, rslope = %f\n", itemID, Qb_kg_sec, Qday, Qsday, rslope);

	MFVarSetFloat (_MDOutBedloadFluxID, itemID, Qb_kg_sec);
	//printf("_MDOutWaterDensityID = %f, _MDOutKinematicViscosityID = %f, _MDOutSettlingVelocityID = %f, _MDOutKinematicViscosityID = %f, _MDOutBedloadFluxID = %f, _MDOutStreamPowerID = %f \n", _MDOutWaterDensityID, _MDOutKinematicViscosityID, _MDOutSettlingVelocityID, _MDOutKinematicViscosityID, _MDOutBedloadFluxID, _MDOutStreamPowerID);

// Calculate Qb budget
	//upstream_Qb =  MFVarGetFloat (_MDInUpStreamQbID,itemID, 0.0);
	//deltaQb = upstream_Qb - Qb_kg_sec; //local bedload budget
	//MFVarSetFloat (_MDOutDeltaBedloadID, itemID, deltaQb); 
	//MFVarSetFloat (_MDInUpStreamQbID , itemID, (upstream_Qb*-1));
	//MFVarSetFloat (_MDInUpStreamQbID , itemID, Qb_kg_sec);  
}

enum { MDinput, MDcalculate, MDcorrected };

int MDBedloadFluxDef() {
	MFDefEntering ("BedloadFlux");
	if (//((_MDInDischargeID  = MDDischargeBFDef    ())			 == CMfailed) || 
	//if (((_MDInDischargeID  = MDDischargeDef    ())			 == CMfailed) || 
	//((_MDInWaterDensityID 	= MDWaterDensityDef	()) == CMfailed) ||
	//((_MDInSedimentFluxID = MDSedimentFluxDef  ()) 				== CMfailed) ||
	((_MDInWTemp_QxT  				= MDWTempRiverRouteDef ()) == CMfailed) ||
	((_MDInDischargeID	 			= MFVarGetID (MDVarDischarge, 	"m3/s",	MFInput,  MFState, MFBoundary)) == CMfailed) ||
	((_MDInDischMeanID	 			= MFVarGetID (MDVarDischMean, 	"m3/s",	MFInput,  MFState, MFBoundary)) == CMfailed) ||
	//((_MDInSedimentFluxID  		= MFVarGetID (MDVarSedimentFlux,"kg/s",	MFInput,  MFState, MFBoundary)) == CMfailed) ||
	((_MDInQs_barID  				= MFVarGetID (MDVarQs_bar,		"kg/s",	MFInput,  MFState, MFBoundary)) == CMfailed) ||
	((_MDInRiverSlopeID  			= MFVarGetID (MDVarRiverSlope,  " ",	MFInput, MFState, MFBoundary)) == CMfailed) ||
	//((_MDInUpStreamQbID  	= MFVarGetID (MDVarUpStreamQb,  " ",	MFRoute, MFState, MFBoundary)) == CMfailed) ||
	//((_MDInMDVarVelocityTHID		= MFVarGetID (MDVarVelocityTH,  MFNoUnit, 	MFInput, MFState, MFBoundary)) == CMfailed) ||
	((_MDInRiverbedVelocityMeanID   = MFVarGetID (MDVarRiverbedVelocityMean, "m/s",    MFInput,  MFState, MFBoundary)) == CMfailed) ||
	//((_MDInRiverbedWidthMeanID  	= MFVarGetID (MDVarRiverbedWidthMean,     "m",      MFInput, MFState, MFBoundary)) == CMfailed) ||
	//((_MDInRiverWidthID  			= MFVarGetID (MDVarRiverWidth,     "m",      MFInput, MFState, MFBoundary)) == CMfailed) ||
	//((_MDInWTemp_QxT     			= MFVarGetID (MDVarWTemp_QxT,     "degC",      MFInput, MFState, MFBoundary)) == CMfailed) ||
	((_MDInMDVarBedloadEquationID	= MFVarGetID (MDVarBedloadEquation,     MFNoUnit, 	MFInput, MFState, MFBoundary)) == CMfailed) ||
	((_MDInQsConcID					= MFVarGetID (MDVarQsConc  			,"g/L",	MFOutput, MFState, MFBoundary)) == CMfailed) ||
	//Output
 	((_MDOutBedloadFluxID			= MFVarGetID (MDVarBedloadFlux, "kg/s",MFOutput, MFState, MFBoundary)) == CMfailed) ||
 	((_MDOutSettlingVelocityID		= MFVarGetID (MDVarSettlingVelocity, "m/s",MFOutput, MFState, MFBoundary)) == CMfailed) ||
 	((_MDOutKinematicViscosityID	= MFVarGetID (MDVarKinematicViscosity, "m2/s",MFOutput, MFState, MFBoundary)) == CMfailed) ||
 	//((_MDOutDeltaBedloadID	= MFVarGetID (MDDeltaBedload  , "kg/s",MFOutput, MFState, MFBoundary)) == CMfailed) ||
	((_MDOutWaterDensityID        	= MFVarGetID (MDVarWaterDensity,   "kg/m3", MFOutput, MFState, MFBoundary)) == CMfailed) ||
	((_MDOutStreamPowerID			= MFVarGetID (MDVarStreamPower,   "W/m2", MFOutput, MFState, MFBoundary)) == CMfailed) ||
	((_MDOutCritStreamPowerID		= MFVarGetID (MDVarCritStreamPower,   "W/m2", MFOutput, MFState, MFBoundary)) == CMfailed) ||
	((_MDOutParticleSizeID			= MFVarGetID (MDVarParticleSize,   "m", MFOutput, MFState, MFBoundary)) == CMfailed) ||
	((_MDOutQb_bar_AshleyID			= MFVarGetID (MDVarQb_bar_Ashley, "kg/s",MFOutput, MFState, MFBoundary)) == CMfailed) ||
	
	(MFModelAddFunction (_MDBedloadFlux) == CMfailed)) return (CMfailed);

	MFDefLeaving  ("BedloadFlux");
	return (_MDOutBedloadFluxID);
}
