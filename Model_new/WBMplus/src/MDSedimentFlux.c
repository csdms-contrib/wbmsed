/******************************************************************************
Calculating the Sediment Flux with the BQART model: Qs=w*B*Q^n1*A^n2*R*T
where Q is discharge [km3/yr] (calculated by WBM), A is the contributing area [km2],
R is the maximum Relief [km], T is average temperature [c] and B is for geological 
and human factors.

MDSedimentFlux.c
WBMsedNEWS1.2 - reservoir trapping Te was changed to exclude the small res calculation 
module (as a new larger reservoir dataset is now in use) and reduce the minimum threshold
from >0.1 to >0.0
*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>
#include <math.h>
#include <stdlib.h>

// Input
static int _MDInDischargeID   	   = MFUnset;
//static int _MDInWaterBalanceID 	   = MFUnset;
//static int _MDInDischargeAccID	   = MFUnset;
//static int _MDInResStorageID	   = MFUnset;
static int _MDInDischMeanID	  	   = MFUnset;
static int _MDInAirTempID   	   = MFUnset;
static int _MDInContributingAreaID = MFUnset;
//static int _MDInContributingAreaAccID = MFUnset;
static int _MDInReliefID 	   	   = MFUnset;
//static int _MDInAirTempAcc_timeID  = MFUnset;
//static int _MDInTimeStepsID 	   = MFUnset;
static int _MDInIceCoverID 	   	   = MFUnset;
static int _MDInSmallResCapacityID = MFUnset;
static int _MDInBQART_LithologyID  = MFUnset;
static int _MDInBQART_GNPID	   	   = MFUnset;
static int _MDInPopulationID	   = MFUnset;
static int _MDInResCapacityAccID   = MFUnset;
static int _MDInResCapacityID 	   = MFUnset;
static int _MDInTeAaccID		   = MFUnset;
static int _MDOutBQART_TeID 	   = MFUnset; 
static int _MDOutPopulationAccID   = MFUnset;
static int _MDOutMeanGNPID		   = MFUnset;
static int _MDOutBQART_EhID		   = MFUnset;
//static int _MDInNewDischargeAccID  = MFUnset;
static int _MDInNewAirTempAcc_timeID = MFUnset;
//static int _MDInNewTimeStepsID 	   = MFUnset;
//static int _MDInNewSedimentAccID   = MFUnset;
static int _MDInAirTempAcc_spaceID = MFUnset;
static int _MDInUpStreamQsID 	   = MFUnset;
static int _MDInBedloadFluxID 	   = MFUnset;
static int _MDInSuspendedBedFluxID =MFUnset;
static int _MDInMDVarCalculateTotalSedimentID =MFUnset;
static int _MDInMDVarSedPristineID = MFUnset;		
static int _MDInSedimentTrappingID = MFUnset;
static int _MDInAvgNStepsID      = MFUnset;
// Output
static int _MDOutSedimentFluxID 	= MFUnset;
static int _MDOutTotalSedimentFluxID= MFUnset;
static int _MDOutWashloadFluxID		= MFUnset;
static int _MDOutBQART_BID			= MFUnset;
static int _MDOutBQART_Qbar_km3yID 	= MFUnset;
static int _MDOutBQART_Qbar_m3sID 	= MFUnset;
static int _MDOutBQART_AID 	= MFUnset;
static int _MDOutBQART_RID 	= MFUnset;
static int _MDOutBQART_TID 			= MFUnset;
static int _MDOutPopulationDensityID = MFUnset;
static int _MDOutGNPAreaAccID 		= MFUnset;
static int _MDOutQs_barID 			= MFUnset;
//static int _MDInrnseedID 			= MFUnset; 
static int _MDOutLithologyAreaAccID  = MFUnset;
static int _MDOutLithologyMeanID = MFUnset;
static int _MDOutDeltaQsID = MFUnset;
static int _MDOutQsConcID = MFUnset;
static int _MDOutQsYieldID	= MFUnset;

static void _MDSedimentFlux (int itemID) {
	float Qs, Qsbar, TotalSed,Bedload,SuspendedBed,washload;
	float w  = 0.02; //in kg/s/km2/C
	float n1 = 0.31;
	float n2 = 0.5;
	int   TimeStep,SedPristine,TotalOption;
	float Qday, DischMean, Qbar_m3s,Qbar_km3y;//, Qacc;
	float Tday,Tbar,Tacc,T_time;
	float B, I, L, Te, Eh,upLithologyArea;// ,Lbar
	float A, R;
	float Ag;
	float tmp,TupSlop,PixSize_km2;
	float ResCapacity, ResCapacityAcc, TeQacc ,deltaTau,upStreamResCapacity,LargeResCapacity,SmallRecCapacity;
//	float SmallRecCapacity;
	float PopulationAcc,PopuDesity, MeanGNP,GNPAreaAcc;
	static float dailyRand, yearlyRand;
//	static int tmpTimeStep,tempTimeStep;
//	static int dayCount;
	float s,sigma,cbar, Psi, C;
	float QsConc,QsYield; //upstream_Qs,deltaQs,
	float percentDiff=0;
//Geting the values of these parameters
	PixSize_km2 =(MFModelGetArea(itemID)/pow(1000,2));
	Qday = MFVarGetFloat (_MDInDischargeID   , 	itemID, 0.0);	// in m3/s	
	DischMean = MFVarGetFloat (_MDInDischMeanID, itemID, 0.0);	// in m3/s
	Tday = MFVarGetFloat (_MDInAirTempID, 		itemID, 0.0);	// in C	
	R    = MFVarGetFloat (_MDInReliefID, 		itemID, 0.0);	// in m 
//Calculating contributing area for each pixel
	//A = MFVarGetFloat (_MDInContributingAreaAccID, itemID, 0.0) + PixSize_km2;// convert from m2 to km2  //calculating the contributing area
	//MFVarSetFloat (_MDInContributingAreaAccID, itemID, A);
	A    = MFVarGetFloat (_MDInContributingAreaID, 	itemID, 0.0);	//in km2
	//if (A <= 0) A = PixSize_km2;
	R = R/1000; // convert to km	
	if (R <= 0) R = 0.00005;
//Calculating maximum Relief for each pixel

	
// Geting the phase 1 (BQARTpreprocess) values for the first run 
//	tmp= MFVarGetInt (_MDInNewTimeStepsID, itemID, 0.0); 
//	if (tmp == 0) {
//		T_time = MFVarGetFloat (_MDInAirTempAcc_timeID, itemID, 0.0);
//		MFVarSetFloat (_MDInNewAirTempAcc_timeID, itemID, T_time);
		//Qacc = MFVarGetFloat (_MDInDischargeAccID, itemID, 0.0);
		//MFVarSetFloat (_MDInNewDischargeAccID, itemID, Qacc);
//		TimeStep = MFVarGetInt (_MDInTimeStepsID, itemID, 0.0);
//		MFVarSetInt (_MDInNewTimeStepsID, itemID, TimeStep);
//	}
//Accumulate temperature
	T_time = (MFVarGetFloat (_MDInNewAirTempAcc_timeID, itemID, 0.0) + Tday); 
	MFVarSetFloat (_MDInNewAirTempAcc_timeID, itemID, T_time);	
	TupSlop = MFVarGetFloat (_MDInAirTempAcc_spaceID, itemID, 0.0); 
	Tacc = TupSlop + (T_time * PixSize_km2);
	MFVarSetFloat (_MDInAirTempAcc_spaceID, itemID, Tacc);

//Accumulate discharge
	//Qacc = (MFVarGetFloat (_MDInNewDischargeAccID, itemID, 0.0)+ Qday);// in m3/s
	//MFVarSetFloat (_MDInNewDischargeAccID, itemID, Qacc);			

// Accumulate time steps
	//TimeStep = MFVarGetInt (_MDInTimeStepsID, itemID, 0.0);
	//tempTimeStep = (MFVarGetInt (_MDInNewTimeStepsID, itemID, 0.0)+1);//		!!! Chnaged for constant 7/10/10
//	TimeStep = (MFVarGetInt (_MDInNewTimeStepsID, itemID, 0.0) +1 );
//	MFVarSetInt (_MDInNewTimeStepsID, itemID, TimeStep);		//	!!! Chnaged for constant 7/10/10
	TimeStep     = MFVarGetInt   (_MDInAvgNStepsID, itemID, 0);
	MFVarSetFloat (_MDOutBQART_AID, itemID, A);
	MFVarSetFloat (_MDOutBQART_RID, itemID, R);
//Calculate moving avarege temperature (Tbar) and discharge
	Tbar = Tacc/TimeStep/A;
	MFVarSetFloat (_MDOutBQART_TID, itemID, Tbar);
	Qbar_m3s = DischMean; //in m3/s
	//Qbar_m3s = Qacc/TimeStep; //in m3/s
	MFVarSetFloat (_MDOutBQART_Qbar_m3sID, itemID, Qbar_m3s); 
	if (Qbar_m3s  > 0){ //convert to km3/y	
		Qbar_km3y = (Qbar_m3s/(pow(1000,3)))*(365*24*60*60);
 	}else	Qbar_km3y = 0;
	MFVarSetFloat (_MDOutBQART_Qbar_km3yID, itemID, Qbar_km3y); // in km3/yr
// Calculating B
	Ag = MFVarGetFloat (_MDInIceCoverID, itemID, 0.0);	//in %	
	I  = 1 + 0.09 * Ag; 
	
	//Calculating catchmant average lithology
	L  = MFVarGetFloat (_MDInBQART_LithologyID, itemID, 0.0);	//no units
	if (L <= 0) L = 1.0;
	upLithologyArea =  MFVarGetFloat(_MDOutLithologyAreaAccID, itemID, 0.0) + L * (MFModelGetArea (itemID)/pow(1000,2));
	MFVarSetFloat (_MDOutLithologyAreaAccID, itemID, upLithologyArea);
	//Lbar = upLithologyArea/A;
	MFVarSetFloat (_MDOutLithologyMeanID, itemID, (upLithologyArea/A));
	L  = MFVarGetFloat(_MDOutLithologyMeanID, itemID, 0.0);	//no units
	if (L <= 0) L = 1.0;
	SedPristine = MFVarGetInt (_MDInMDVarSedPristineID, itemID, 0.0);
	if (SedPristine == 0 ){ //NOT pristine -- disturbed
		// Calculating reservoir trapping (Te)
		ResCapacity = 0.0;
		ResCapacityAcc = 0.0;
		//TeQacc = 0.0;
		SmallRecCapacity = MFVarGetFloat (_MDInSmallResCapacityID, itemID, 0.0)/(pow(1000,3)); //convert from m3 to km3
		if (SmallRecCapacity < 0) SmallRecCapacity = 0.00;
		LargeResCapacity = MFVarGetFloat (_MDInResCapacityID,	   itemID, 0.0);
		if (LargeResCapacity < 0.0001) LargeResCapacity = 0.0000000;
		ResCapacity = SmallRecCapacity + LargeResCapacity;
		//ResCapacity = LargeResCapacity;
		if (ResCapacity < 0) ResCapacity = 0.00;
	
		//Calculating basin trapping efficiency Using Vorosmarty et al. 1997 method
		upStreamResCapacity = MFVarGetFloat (_MDInResCapacityAccID, itemID, 0.0);
		ResCapacityAcc = upStreamResCapacity + ResCapacity;
		MFVarSetFloat (_MDInResCapacityAccID, itemID, ResCapacityAcc);
		//TeAacc = MFVarGetFloat (_MDInTeAaccID, itemID, 0.0); ///new
		TeQacc = MFVarGetFloat (_MDInTeAaccID, itemID, 0.0); ///new
		if (ResCapacity > 0.0001){  //reservoir pixel
			deltaTau = ResCapacityAcc/Qbar_km3y; ///may want to change to daily Q!!!!!!
			Te = 1 - (0.05/pow(deltaTau, 0.5));
			TeQacc = Te * A;
			//TeQacc = Te * Qbar_km3y;
			//TeQacc = TeQacc + Te * Qbar_km3y;
			MFVarSetFloat (_MDInTeAaccID, itemID, TeQacc);
		} else{ // non reservoir pixel basin "outlet"
				//Te = TeQacc / Qbar_km3y;
				Te = TeQacc / A;
			}
		if (Qbar_km3y == 0.0) Te = 0.0;
		if (Te > 1) Te = 1.0;
		if (Te < 0) Te = 0.0;
//Old Te calculation
/*		if (ResCapacity > 0.00001){	
			if (ResCapacity > 0.5){
				deltaTau = ResCapacityAcc/Qbar_km3y; 
				Te = 1 - (0.05/pow(deltaTau, 0.5));
			}else{
				Te = 1.0 - (1.0 / (1 + 0.00021 * (ResCapacity/A)));
				Te = Te + (TeAacc/A); //new!!!!
				}
		    TeAacc = (TeAacc/A) + (Te*A);
			MFVarSetFloat (_MDInTeAaccID, itemID, TeAacc);
		}else{
			if (Qbar_km3y == 0.0){
			Te = 0.0;
			}else Te = TeAacc/A ;			
		}	
*/		
		//Calculating Eh
		// Calculating mean population density
		PopulationAcc = MFVarGetFloat(_MDOutPopulationAccID, itemID, 0.0) + MFVarGetFloat(_MDInPopulationID, itemID, 0.0); 
		MFVarSetFloat (_MDOutPopulationAccID, itemID, PopulationAcc);
		PopuDesity = PopulationAcc/A;
		MFVarSetFloat (_MDOutPopulationDensityID, itemID, PopuDesity);
		// Calculating mean GNP
		GNPAreaAcc = MFVarGetFloat(_MDOutGNPAreaAccID, itemID, 0.0) + (MFVarGetFloat(_MDInBQART_GNPID, itemID, 0.0) * PixSize_km2);
		MFVarSetFloat (_MDOutGNPAreaAccID, itemID, GNPAreaAcc);
		if (GNPAreaAcc == 0)
			MeanGNP = 0;
		else
			MeanGNP = GNPAreaAcc/A;
		MFVarSetFloat (_MDOutMeanGNPID, itemID, MeanGNP);
	
		Eh = 1.0;
		if (MeanGNP > 20000){
			if (PopuDesity > 30)Eh = 0.3;
		}
		if (MeanGNP < 2500){
			if (PopuDesity > 140)Eh = 2.0;
		}	
		
		//if (PopuDesity > 50){
		//	if (MeanGNP > 20000) Eh = 0.3;
		//	if (MeanGNP < 2500) Eh = 2.0;
		//}
		//if (PopuDesity > 200){
		//	if (MeanGNP > 15000) Eh = 0.3;
		//	if (MeanGNP < 1000) Eh = 2.0;
		//}
	}
	if (SedPristine == 1) { // Pristine sediment !!!!!
		Te = 0.0;
		Eh = 1.0;
	}	
	
	if (SedPristine == 2) { // semi-Pristine sediment (no reservoirs !!!!!
		//Calculating Eh
		// Calculating mean population density
		PopulationAcc = MFVarGetFloat(_MDOutPopulationAccID, itemID, 0.0) + MFVarGetFloat(_MDInPopulationID, itemID, 0.0); //devided by 25 for the 06min simulation in order to account for the smaler pixel size.
		MFVarSetFloat (_MDOutPopulationAccID, itemID, PopulationAcc);
		PopuDesity = PopulationAcc/A;
		MFVarSetFloat (_MDOutPopulationDensityID, itemID, PopuDesity);
		// Calculating mean GNP
		GNPAreaAcc = MFVarGetFloat(_MDOutGNPAreaAccID, itemID, 0.0) + (MFVarGetFloat(_MDInBQART_GNPID, itemID, 0.0) * PixSize_km2);
		MFVarSetFloat (_MDOutGNPAreaAccID, itemID, GNPAreaAcc);
		if (GNPAreaAcc == 0)
			MeanGNP = 0;
		else
			MeanGNP = GNPAreaAcc/A;
		MFVarSetFloat (_MDOutMeanGNPID, itemID, MeanGNP);
	
		Eh = 1.0;

		if (MeanGNP > 20000){
			if (PopuDesity > 30)Eh = 0.3;
		}
		if (MeanGNP < 2500){
			if (PopuDesity > 140)Eh = 2.0;
		}	
	
		Te = 0.0;
	}	//end SedPristine == 2
	
	if (SedPristine == 3) { // Input sediment Trapping
		Te = MFVarGetFloat(_MDInSedimentTrappingID, itemID, 0.0)/100;
		if (Qbar_km3y == 0.0) Te = 0.0;
		if (Te > 1) Te = 1.0;
		if (Te < 0) Te = 0.0;
			//Calculating Eh
		// Calculating mean population density
		PopulationAcc = MFVarGetFloat(_MDOutPopulationAccID, itemID, 0.0) + MFVarGetFloat(_MDInPopulationID, itemID, 0.0); 
		MFVarSetFloat (_MDOutPopulationAccID, itemID, PopulationAcc);
		PopuDesity = PopulationAcc/A;
		MFVarSetFloat (_MDOutPopulationDensityID, itemID, PopuDesity);
		// Calculating mean GNP
		GNPAreaAcc = MFVarGetFloat(_MDOutGNPAreaAccID, itemID, 0.0) + (MFVarGetFloat(_MDInBQART_GNPID, itemID, 0.0) * PixSize_km2);
		MFVarSetFloat (_MDOutGNPAreaAccID, itemID, GNPAreaAcc);
		if (GNPAreaAcc == 0)
			MeanGNP = 0;
		else
			MeanGNP = GNPAreaAcc/A;
		MFVarSetFloat (_MDOutMeanGNPID, itemID, MeanGNP);
	
		Eh = 1.0;
		if (MeanGNP > 20000){
			if (PopuDesity > 30)Eh = 0.3;
		}
		if (MeanGNP < 2500){
			if (PopuDesity > 140)Eh = 2.0;
		}	
	}//end SedPristine == 3
	MFVarSetFloat (_MDOutBQART_TeID, itemID, Te);	
	MFVarSetFloat (_MDOutBQART_EhID, itemID, Eh);
	
	B = I * L * (1 - Te) * Eh;
	if (B < 0){
		B = 0;
	}

	MFVarSetFloat (_MDOutBQART_BID, itemID, B);

// Calculating sediment flux (Qsbar)	
	if (Tbar>=2)	
		Qsbar = w * B * pow(Qbar_km3y,n1) * pow(A,n2) * R * Tbar; //in kg/s
	else
		Qsbar = 2*w * B * pow(Qbar_km3y,n1) * pow(A,n2) * R; //in kg/s
	
	MFVarSetFloat (_MDOutQs_barID, itemID, Qsbar);

// The Psi model. Calculating daily sediment flux (Qs).
	s     = 0.17 + (0.0000183 * Qbar_m3s); //Qbar in m3/s
	//sigma = 0.763 * pow(0.99995, Qbar_m3s);//Qbar in m3/s
	//cbar  = 1.4 - (0.025 * Tbar) + (0.00013 * (R*1000)) + (0.145 *log10(Qsbar));//R in m; Qsbar in kg/s
	//cbar  = 0.1 - (0.025 * Tbar) + (0.00013 * (R*1000)) + (0.145 *log(Qsbar));//R in m; Qsbar in kg/s  !!! Changed 6/20/2013
	cbar  = 1.4 - (0.025 * Tbar) + (0.00013 * R) + (0.145 *log10(Qsbar));//R in km; Qsbar in kg/s !!!---- Fixed (missing'()') on 2/5/2019!!!
	//cbar  = 0.1 - (0.025 * Tbar) + (0.00013 * (R)) + (0.145 *log10(Qsbar));//R in km; Qsbar in kg/s  -- not good Cbar too low!!
	if (Qsbar == 0) cbar = 0;
	

dailyRand = 0.00001; // Eliminate daily randomness !!!
yearlyRand = 0.00001;// Eliminate Yearly randomness!!!

	//Psi= exp((sigma * dailyRand)); 
	
	C = s * yearlyRand + cbar;
	//C = 1.0;
	Psi = 1.2458 - (0.236 * C);
	
	MFVarSetFloat (_MDOutDeltaQsID, itemID, C);
	MFVarSetFloat (_MDOutQsYieldID, itemID, Psi); 
	if (Qday > 0 && Qbar_m3s > 0)
		Qs =  (Psi * Qsbar * pow(Qday/Qbar_m3s, C)); //Qs in kg/s?
	else Qs = 0;

	QsYield = Qs / A; // calculating basin sediment yield kg/s/km2
	if (Qday > 0)
		QsConc = Qs/Qday; // Sediment Concentration g/L
	else QsConc = 0;
		
	MFVarSetFloat (_MDOutSedimentFluxID, itemID, Qs);
	// Calculate Qs budget
	//upstream_Qs =  MFVarGetFloat (_MDInUpStreamQsID,itemID, 0.0);
	//deltaQs = upstream_Qs - Qs; //local bedload budget
//	MFVarSetFloat (_MDOutDeltaQsID, itemID, deltaQs); 
//	MFVarSetFloat (_MDInUpStreamQsID , itemID, (upstream_Qs*-1));
//	MFVarSetFloat (_MDInUpStreamQsID , itemID, Qs); 
	MFVarSetFloat (_MDOutQsConcID, itemID, QsConc);
//	MFVarSetFloat (_MDOutQsYieldID, itemID, QsYield); 
	
	//	TotalOption = 1;
	SuspendedBed = MFVarGetFloat (_MDInSuspendedBedFluxID,itemID, 0.0);
	washload = Qs - SuspendedBed;
	if (washload < 0) washload = 0.0; 
	MFVarSetFloat (_MDOutWashloadFluxID, itemID, washload);
	Bedload =  MFVarGetFloat (_MDInBedloadFluxID,itemID, 0.0);
	TotalOption = MFVarGetInt (_MDInMDVarCalculateTotalSedimentID, itemID, 0.0);
	if (TotalOption == 1){
		//TotalSed = Qs + Bedload + SuspendedBed;
		TotalSed = Qs + Bedload;
		MFVarSetFloat (_MDOutTotalSedimentFluxID, itemID, TotalSed);
	}

}

enum { MDinput, MDcalculate, MDcorrected };

int MDSedimentFluxDef() {
	
	MFDefEntering ("SedimentFlux");
	
	if (//(_MDInWaterBalanceID   = MDWaterBalanceDef   ()) == CMfailed) || 
		//((_MDInDischargeID 		  = MDDischargeDef   ()) == CMfailed) || 
		((_MDInDischargeID 		  = MDDischargeBFDef   ()) 		== CMfailed) ||
		((_MDInSmallResCapacityID = MDSmallReservoirCapacityDef  ()) == CMfailed) ||
		//((_MDInSmallResCapacityID = MDWTempRiverRouteDef  ()) == CMfailed) ||
	    ((_MDInDischMeanID 		  =	MDDischMeanDef     ()) 		== CMfailed) ||
		((_MDInAvgNStepsID        = MDAvgNStepsDef ())   		== CMfailed) ||
    ((_MDInAirTempID 	  		= MFVarGetID (MDVarAirTemperature,		   "degC",  MFInput, MFState, MFBoundary)) == CMfailed) ||
	//((_MDInAirTempAcc_timeID   	= MFVarGetID (MDVarAirTemperatureAcc_time, "degC",	MFInput, MFState, MFBoundary)) == CMfailed) ||
	//((_MDInDischargeAccID  		= MFVarGetID (MDVarDischargeAcc,     	   "m3/s",	MFInput, MFState, MFBoundary)) == CMfailed) ||
	//((_MDInTimeStepsID     		= MFVarGetID (MDVarTimeSteps,      		 MFNoUnit,	MFInput, MFState, MFBoundary)) == CMfailed) ||
	//((_MDInElevationID 	= MFVarGetID (MDVarontributingArea,		"km2", 	MFInput, MFState, MFBoundary)) == CMfailed) ||
	((_MDInReliefID   			= MFVarGetID (MDVarRelief,					  "m", 	MFInput, MFState, MFBoundary)) == CMfailed) ||
	((_MDInContributingAreaID   = MFVarGetID (MDVarContributingArea,		  "km2", MFInput, MFState, MFBoundary)) == CMfailed) ||
	((_MDInIceCoverID   		= MFVarGetID (MDVarIceCover,	     	MFNoUnit,	MFInput, MFState, MFBoundary)) == CMfailed) ||
	((_MDInBQART_LithologyID 	= MFVarGetID (MDVarBQART_Lithology, 	MFNoUnit,	MFInput, MFState, MFBoundary)) == CMfailed) ||
	((_MDInBQART_GNPID  	 	= MFVarGetID (MDVarBQART_GNP,	       	MFNoUnit,	MFInput, MFState, MFBoundary)) == CMfailed) ||
	((_MDInPopulationID	   		= MFVarGetID (MDVarPopulation,     		MFNoUnit,	MFInput, MFState, MFBoundary)) == CMfailed) ||
    ((_MDInNewAirTempAcc_timeID = MFVarGetID (MDVarNewAirTemperatureAcc_time,"degC",MFOutput,MFState, MFInitial)) == CMfailed) ||
	((_MDInAirTempAcc_spaceID 	= MFVarGetID (MDVarAirTemperatureAcc_space,"degC"  ,MFRoute, MFState, MFBoundary)) == CMfailed) ||
	//((_MDInNewDischargeAccID  	= MFVarGetID (MDVarNewDischargeAcc,     "m3/s"	,MFOutput, MFState, MFInitial)) == CMfailed) ||
	//((_MDInNewSedimentAccID   	= MFVarGetID (MDVarNewSedimentAcc,      "m3/s"	,MFOutput, MFState, MFInitial)) == CMfailed) ||
	//((_MDInNewTimeStepsID     	= MFVarGetID (MDVarNewTimeSteps,      MFNoUnit	,MFOutput, MFState, MFInitial)) == CMfailed) ||
	((_MDInResCapacityID   	  	= MFVarGetID (MDVarReservoirCapacity,    "km3"	,MFInput,  MFState, MFBoundary)) == CMfailed) ||
	((_MDInTeAaccID  	  		= MFVarGetID (MDVarTeAacc,     		 	 ""		,MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	//((_MDInrnseedID  			= MFVarGetID (MDVarrnseed,     		 	 ""		,MFOutput, MFState, MFInitial)) == CMfailed) ||	
	//((_MDInContributingAreaAccID= MFVarGetID (MDVarContributingAreaAcc,	"km2", 	 MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	((_MDInUpStreamQsID  		= MFVarGetID (MDVarUpStreamQs,  		"kg/s",	 MFRoute, MFState, MFBoundary)) == CMfailed) ||
	//((_MDInMDVarPCQdifferenceID	= MFVarGetID (MDVarPCQdifference,     MFNoUnit, 	MFInput, MFState, MFBoundary)) == CMfailed) ||
	((_MDInMDVarCalculateTotalSedimentID= MFVarGetID (MDVarCalculateTotalSediment, MFNoUnit, 	MFInput, MFState, MFInitial)) == CMfailed) ||
	((_MDInMDVarSedPristineID	= MFVarGetID (MDVarSedPristine,     MFNoUnit, 	MFInput, MFState, MFBoundary)) == MFInitial) ||
//	((_MDInAreaID  = MFVarGetID (MDVarArea, "km2", MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	((_MDInSedimentTrappingID 	= MFVarGetID (MDVarSedimentTrapping, 	MFNoUnit,	MFInput, MFState, MFBoundary)) == MFInitial) ||
	// output
    ((_MDOutSedimentFluxID  	= MFVarGetID (MDVarSedimentFlux, 	"kg/s",	MFOutput,  MFState, MFBoundary)) == CMfailed) ||
    ((_MDOutTotalSedimentFluxID = MFVarGetID (MDVarTotalSedimentFlux,"kg/s",MFOutput, MFState, MFBoundary)) == CMfailed) ||
    ((_MDOutWashloadFluxID	    = MFVarGetID (MDVarWashloadFlux,	"kg/s", MFOutput,  MFState, MFBoundary)) == CMfailed) ||
	((_MDOutBQART_BID  			= MFVarGetID (MDVarBQART_B, 	  MFNoUnit,	MFOutput,  MFState, MFBoundary)) == CMfailed) ||
	((_MDOutBQART_Qbar_m3sID 	= MFVarGetID (MDVarBQART_Qbar_m3s, 	 "m3/s",MFOutput, MFState, MFBoundary)) == CMfailed) ||
	((_MDOutBQART_Qbar_km3yID 	= MFVarGetID (MDVarBQART_Qbar_km3y,"km3/y", MFOutput,  MFState, MFBoundary)) == CMfailed) ||
	((_MDOutBQART_AID  			= MFVarGetID (MDVarBQART_A, 		"km2", 	MFOutput,  MFState, MFBoundary)) == CMfailed) ||
	((_MDOutBQART_RID  			= MFVarGetID (MDVarBQART_R, 		"km" , 	MFOutput,  MFState, MFBoundary)) == CMfailed) ||
	((_MDOutBQART_TID  			= MFVarGetID (MDVarBQART_T, 		"degC", MFOutput,  MFState, MFBoundary)) == CMfailed) ||
   	((_MDOutPopulationAccID 	= MFVarGetID (MDVarPopulationAcc, 		"", MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	((_MDInResCapacityAccID 	= MFVarGetID (MDVarResStorageAcc, 	 "km3", MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	((_MDOutPopulationDensityID = MFVarGetID (MDVarPopulationDensity,"km2",	MFOutput, MFState, MFBoundary)) == CMfailed) ||
	((_MDOutGNPAreaAccID  		= MFVarGetID (MDVarGNPAreaAcc,  	" ", 	MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	((_MDOutMeanGNPID  			= MFVarGetID (MDVarMeanGNP,  		" ", 	MFOutput, MFState, MFBoundary)) == CMfailed) ||
	((_MDOutBQART_EhID  		= MFVarGetID (MDVarBQART_Eh, 		 " ", 	MFOutput, MFState, MFBoundary)) == CMfailed) ||
	((_MDOutBQART_TeID  		= MFVarGetID (MDVarBQART_Te, 		 " ", 	MFOutput, MFState, MFBoundary)) == CMfailed) ||
	((_MDOutQs_barID			= MFVarGetID (MDVarQs_bar, 			"kg/s",	MFOutput, MFState, MFInitial)) == CMfailed) ||
 	((_MDOutLithologyAreaAccID 	= MFVarGetID (MDVarLithologyAreaAcc,	"",	MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	((_MDOutLithologyMeanID  	= MFVarGetID (MDVarLithologyMean,	"" , 	MFOutput, MFState, MFBoundary)) == CMfailed) ||
	((_MDOutDeltaQsID			= MFVarGetID (MDDeltaQs  			,"kg/s",MFOutput, MFState, MFBoundary)) == CMfailed) ||
	((_MDOutQsConcID			= MFVarGetID (MDQsConc				,"kg/m3",MFOutput, MFState, MFBoundary)) == CMfailed) ||
	((_MDOutQsYieldID			= MFVarGetID (MDQsYield				,"kg/s/km2",MFOutput, MFState, MFBoundary)) == CMfailed) ||
	((_MDInBedloadFluxID 	  =	MDBedloadFluxDef   ())			 == CMfailed) ||
	((_MDInSuspendedBedFluxID = MDSuspendedBedFluxDef   ()) 	== CMfailed) ||
	(MFModelAddFunction (_MDSedimentFlux) == CMfailed)) return (CMfailed);

	MFDefLeaving  ("SedimentFlux");
	return (_MDOutSedimentFluxID);
}
