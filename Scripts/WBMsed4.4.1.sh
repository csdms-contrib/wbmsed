#!/bin/bash
if [ "${0%/*}" != "." ]; then PROJECTDIR="${0%/*}"; PROJECTDIR="${PROJECTDIR%/Scripts}"; else PROJECTDIR=".."; fi
               SCRIPT="${0##*/}"; SCRIPT="${SCRIPT%.sh}";

      USAGE="Usage: <Domain> [dist|prist] <Resolution>"
      MODEL="${PROJECTDIR}/Model_new/WBMplus/bin/wbmsed.bin"

       DOMAIN="${1}"; shift
CONFIGURATION="${1}"; shift
   RESOLUTION="${1}"; shift

if [[ "${DOMAIN}" == "" || "${CONFIGURATION}" == "" || "${RESOLUTION}" == "" ]]
then
	echo "${USAGE}"
	exit 0
fi

EXPERIMENT="4p4p1"

if [ "${GHAASDIR}" == "" ]; then GHAASDIR="/bighome/scohen2/ghaas"; fi
source "${GHAASDIR}/Scripts/fwFunctions30.sh"


#MODEL="${GHAASDIR}/bin/WBMstable.bin"
RGISRESULTS="/grps2/scohen2/WBMsedOutput_4p4p1"

RGISARCHIVE="/grps2/scohen2/InputData/RGISarchive2"
RGISCSDMS="/grps2/scohen2/InputData/RGISarchiveCSDMS" # New input directory for WBMsed
MEANINPUTS="/grps2/scohen2/InputData/SDMLmeanInputs"

 RGISBINDIR="${GHAASDIR}/bin"

             STARTYEAR=1960
               ENDYEAR=2019

        AIRTEMP_STATIC=$(RGISfile "${RGISARCHIVE}" "${DOMAIN}+" "air_temperature"              "TerraClimate"     "${RESOLUTION}+" "TS" "monthly" "1984")
       AIRTEMP_DYNAMIC=$(RGISfile "${RGISARCHIVE}" "${DOMAIN}+" "air_temperature"              "TerraClimate"     "${RESOLUTION}+" "TS" "monthly" "xxxx")
 MONTHLY_PRECIP_STATIC=$(RGISfile "${RGISARCHIVE}" "${DOMAIN}+" "precipitation"                "TerraClimate"     "${RESOLUTION}+" "TS" "monthly" "1984")
MONTHLY_PRECIP_DYNAMIC=$(RGISfile "${RGISARCHIVE}" "${DOMAIN}+" "precipitation"                "TerraClimate"     "${RESOLUTION}+" "TS" "monthly" "xxxx")
    PRECIP_FRAC_STATIC=$(RGISfile "${RGISARCHIVE}" "${DOMAIN}+" "daily_precipitation_fraction" "NCEP"     "${RESOLUTION}+" "LT" "daily"   "2001")
    PRECIP_FRAC_DYNAMIC=$(RGISfile "${RGISARCHIVE}" "${DOMAIN}+" "daily_precipitation_fraction" "NCEP"     "${RESOLUTION}+" "TS" "daily"   "xxxx")
   # PRECIP_FRAC_STATIC=$(RGISfile "${RGISARCHIVE}" "${DOMAIN}+" "daily_precipitation_fraction" "GPCP1DDv11"     "${RESOLUTION}+" "LT" "daily"   "2001")

case ${CONFIGURATION} in
	(dist)
		DISTURBED="on"
		EXPERIMENT="${EXPERIMENT}+Dist"
	;;
	(prist)
		DISTURBED="off"
		EXPERIMENT="${EXPERIMENT}+Prist"
	;;
	(*)
		echo "${USAGE}"
		exit 1
	;;
esac

case ${RESOLUTION} in
	(30min)
	           NETVERSION="PotSTNv602"
	;;
	(06min|05min|03min|01min)
	        	NETVERSION="HydroSTN30v3"
	           #NETVERSION="HydroSTN30"
	           #NETVERSION="PotSTNv204"
	         #  NETVERSION="STN+HydroSHEDS" - old!
	;;
		(0.3sec|01sec)
	           NETVERSION="NED"
	;;
	(*)
		echo "${USAGE}"
		exit 1
		;;
esac
               NETWORK=$(RGISfile ${RGISARCHIVE} ${DOMAIN} network ${NETVERSION} ${RESOLUTION} static)

                IRRMAP="FAO"
              #CROPFILE="/bighome/scohen2/ccny/ASCII/Cropfile_FourCrops_2008-06-07.txt"
              CROPFILE="${PROJECTDIR}/ASCII/SAGE_FourCrops.tsv"

echo "Configuration: ${PROJECTDIR} ${SCRIPT} ${EXPERIMENT} ${RESOLUTION} ${STARTYEAR} ${ENDYEAR}"

FwArguments -s off -f on -n 2 -D off -p on $* || exit -1
#FwArguments -s on -f on -n 2 -D off $* || exit -1

RGISarchiveFormat gzipped  #(for daily)  
#RGISarchiveFormat netcdf

FwInit    "${MODEL}" "${DOMAIN}" "${NETWORK}" "${PROJECTDIR}/GDS" "${RGISRESULTS}" "${RGISBINDIR}" || exit -1

(( DataNum = 0 ))
DATASOURCES[${DataNum}]="AirTemperature                static  ${EXPERIMENT} file  ${AIRTEMP_STATIC}";         (( ++DataNum ))
DATASOURCES[${DataNum}]="AirTemperature                dynamic ${EXPERIMENT} file  ${AIRTEMP_DYNAMIC}";        (( ++DataNum ))

DATASOURCES[${DataNum}]="MonthlyPrecipitation          static  ${EXPERIMENT} file  ${MONTHLY_PRECIP_STATIC}";  (( ++DataNum ))
DATASOURCES[${DataNum}]="MonthlyPrecipitation          dynamic ${EXPERIMENT} file  ${MONTHLY_PRECIP_DYNAMIC}"; (( ++DataNum ))
DATASOURCES[${DataNum}]="PrecipitationFraction         static  ${EXPERIMENT} file  ${PRECIP_FRAC_STATIC}";   	(( ++DataNum ))
DATASOURCES[${DataNum}]="PrecipitationFraction         dynamic ${EXPERIMENT} file  ${PRECIP_FRAC_DYNAMIC}";    (( ++DataNum ))
DATASOURCES[${DataNum}]="RelativeHumidity        		 static  ${EXPERIMENT} file  $(RGISfile ${RGISARCHIVE} ${DOMAIN}+ relative_humidity              NCEP         ${RESOLUTION}+ TS monthly 1990)";     (( ++DataNum ))
DATASOURCES[${DataNum}]="RelativeHumidity        		dynamic  ${EXPERIMENT} file   $(RGISfile ${RGISARCHIVE} ${DOMAIN}+ relative_humidity              NCEP         ${RESOLUTION}+ TS monthly xxxx)";       (( ++DataNum ))
DATASOURCES[${DataNum}]="AirPressure        		static  ${EXPERIMENT}  file   $(RGISfile ${RGISARCHIVE} ${DOMAIN}+ surface_air_pressure              NCEP         ${RESOLUTION}+ TS daily 1990)";     (( ++DataNum ))
DATASOURCES[${DataNum}]="AirPressure        		dynamic  ${EXPERIMENT} file   $(RGISfile ${RGISARCHIVE} ${DOMAIN}+ surface_air_pressure              NCEP         ${RESOLUTION}+ TS daily xxxx)";      (( ++DataNum ))
DATASOURCES[${DataNum}]="WindSpeed        				static  ${EXPERIMENT}  file   $(RGISfile ${RGISARCHIVE} ${DOMAIN}+ wind_speed             TerraClimate         ${RESOLUTION}+ TS monthly 1990)";     (( ++DataNum ))
DATASOURCES[${DataNum}]="WindSpeed        				dynamic  ${EXPERIMENT} file   $(RGISfile ${RGISARCHIVE} ${DOMAIN}+ wind_speed              TerraClimate         ${RESOLUTION}+ TS monthly xxxx)";       (( ++DataNum ))
DATASOURCES[${DataNum}]="FieldCapacity                 static  ${EXPERIMENT} file  $(RGISfile ${RGISARCHIVE} ${DOMAIN}+ field_capacity              WBM         ${RESOLUTION}+ static)";         (( ++DataNum ))
DATASOURCES[${DataNum}]="RootingDepth                  static  ${EXPERIMENT} file  $(RGISfile ${RGISARCHIVE} ${DOMAIN}+ rooting_depth               WBM         ${RESOLUTION}+ static)";         (( ++DataNum ))
DATASOURCES[${DataNum}]="WiltingPoint                  static  ${EXPERIMENT} file  $(RGISfile ${RGISARCHIVE} ${DOMAIN}+ wilting_point               WBM         ${RESOLUTION}+ static)";         (( ++DataNum ))
DATASOURCES[${DataNum}]="RicePondingDepth              static  ${EXPERIMENT} const 50";                                                                                                          (( ++DataNum ))
DATASOURCES[${DataNum}]="RiverbedSlope                 static  ${EXPERIMENT} const 0.10"; 																										 (( ++DataNum ))
DATASOURCES[${DataNum}]="ImpFracSpatial                static  ${EXPERIMENT} const 0.0";                                                                                                         (( ++DataNum ))
DATASOURCES[${DataNum}]="HCIA                          static  ${EXPERIMENT} const 0.2";                                                                                                         (( ++DataNum ))

DATASOURCES[${DataNum}]="GrowingSeason1_Start          static  ${EXPERIMENT} file  $(RGISfile ${RGISARCHIVE} ${DOMAIN}+ growing_season1    Computed-CRU+FAO     ${RESOLUTION}+ LT annual)";      (( ++DataNum ))
DATASOURCES[${DataNum}]="GrowingSeason2_Start          static  ${EXPERIMENT} file  $(RGISfile ${RGISARCHIVE} ${DOMAIN}+ growing_season2    Computed-CRU+FAO     ${RESOLUTION}+ LT annual)";      (( ++DataNum ))

DATASOURCES[${DataNum}]="IrrigatedAreaFraction         static  ${EXPERIMENT} file  $(RGISfile ${RGISARCHIVE} ${DOMAIN}+ irrigated_area_fraction     GMIA        ${RESOLUTION}+ LT annual 2008)"; (( ++DataNum ))
DATASOURCES[${DataNum}]="IrrigatedAreaFraction         dynamic ${EXPERIMENT} file  $(RGISfile ${RGISARCHIVE} ${DOMAIN}+ irrigated_area_fraction     GMIA        ${RESOLUTION}+ TS annual xxxx)"; (( ++DataNum ))
DATASOURCES[${DataNum}]="IrrigationIntensity           static  ${EXPERIMENT} file  $(RGISfile ${RGISARCHIVE} ${DOMAIN}+ cropping_intensity          DWisser     ${RESOLUTION}+ LT annual)";      (( ++DataNum ))
DATASOURCES[${DataNum}]="IrrigationEfficiency          static  ${EXPERIMENT} file  $(RGISfile ${RGISARCHIVE} ${DOMAIN}+ irrigation_efficiency       DWisser     ${RESOLUTION}+ LT annual)";      (( ++DataNum ))

DATASOURCES[${DataNum}]="CropFraction_01               static  ${EXPERIMENT} file  $(RGISfile ${RGISARCHIVE} ${DOMAIN}+ perennial_crop_fraction     SAGE        ${RESOLUTION}+ LT annual)";      (( ++DataNum ))
DATASOURCES[${DataNum}]="CropFraction_02               static  ${EXPERIMENT} file  $(RGISfile ${RGISARCHIVE} ${DOMAIN}+ vegetables_crop_fraction    SAGE        ${RESOLUTION}+ LT annual)";      (( ++DataNum ))
DATASOURCES[${DataNum}]="CropFraction_03               static  ${EXPERIMENT} file  $(RGISfile ${RGISARCHIVE} ${DOMAIN}+ other_crop_fraction         SAGE        ${RESOLUTION}+ LT annual)";      (( ++DataNum ))
DATASOURCES[${DataNum}]="CropFraction_04               static  ${EXPERIMENT} file  $(RGISfile ${RGISARCHIVE} ${DOMAIN}+ rice_crop_fraction          SAGE        ${RESOLUTION}+ LT annual)";      (( ++DataNum ))

DATASOURCES[${DataNum}]="ReservoirCapacity             static  ${EXPERIMENT} file  $(RGISfile ${RGISCSDMS} ${DOMAIN}+ reservoir_capacity          GRanD1p3-HydroSTN30      ${RESOLUTION}+ TS annual 1980)"; (( ++DataNum ))
DATASOURCES[${DataNum}]="ReservoirCapacity             dynamic ${EXPERIMENT} file  $(RGISfile ${RGISCSDMS} ${DOMAIN}+ reservoir_capacity          GRanD1p3-HydroSTN30      ${RESOLUTION}+ TS annual xxxx)"; (( ++DataNum ))

DATASOURCES[${DataNum}]="RicePercolationRate           static  ${EXPERIMENT} file  $(RGISfile ${RGISARCHIVE} ${DOMAIN}+ rice_percolation_rate       DWisser     ${RESOLUTION}+ LT annual)";      (( ++DataNum ))
DATASOURCES[${DataNum}]="SmallReservoirStorageFraction static  ${EXPERIMENT} file  $(RGISfile ${RGISARCHIVE} ${DOMAIN}+ small_reservoir_coefficient GMIAderived ${RESOLUTION}+ LT annual 2008)"; (( ++DataNum ))
DATASOURCES[${DataNum}]="SmallReservoirStorageFraction dynamic ${EXPERIMENT} file  $(RGISfile ${RGISARCHIVE} ${DOMAIN}+ small_reservoir_coefficient GMIAderived ${RESOLUTION}+ TS annual xxxx)"; (( ++DataNum ))

DATASOURCES[${DataNum}]="ShapeExponent                 static  Common        const 0.1";	(( ++DataNum ))

#DATASOURCES[${DataNum}]="CloudCover					   static  ${EXPERIMENT} file  $(RGISfile ${RGISARCHIVE} ${DOMAIN}+ cloud_cover     CRU-TSv401        ${RESOLUTION}+ TS monthly 1990)"; (( ++DataNum ))
#DATASOURCES[${DataNum}]="CloudCover				       dynamic ${EXPERIMENT} file  $(RGISfile ${RGISARCHIVE} ${DOMAIN}+ cloud_cover     CRU-TSv401        ${RESOLUTION}+ TS monthly xxxx)"; (( ++DataNum ))
DATASOURCES[${DataNum}]="SolarRadiation				   static  ${EXPERIMENT} file  $(RGISfile ${RGISARCHIVE} ${DOMAIN}+ shortwave_downwelling_radiation CRUCloud        ${RESOLUTION}+ LT monthly 1901-1995)"; (( ++DataNum ))

DATASOURCES[${DataNum}]="ContributingArea				static  ${EXPERIMENT} file  $(RGISfile ${RGISCSDMS} ${DOMAIN}+ ContributingArea    HydroSTN30v3 ${RESOLUTION}+ static)";      		(( ++DataNum ))

DATASOURCES[${DataNum}]="ReliefMax    					static  ${EXPERIMENT} file  $(RGISfile ${RGISCSDMS} ${DOMAIN}+ Relief-Max    HydroSTN30v3 ${RESOLUTION}+ static)";      		(( ++DataNum ))
DATASOURCES[${DataNum}]="IceCover    					static  ${EXPERIMENT} file  $(RGISfile ${RGISCSDMS} ${DOMAIN}+ Ice-Cover    ICE5Gv102 ${RESOLUTION}+ static)";      		(( ++DataNum ))
#DATASOURCES[${DataNum}]="TimeSteps    					static  ${EXPERIMENT} file  $(RGISfile ${MEANINPUTS} ${DOMAIN}+ TimeSteps  SDML ${RESOLUTION}+ static)";      		(( ++DataNum ))
#DATASOURCES[${DataNum}]="AirTempAcc_time    			static  ${EXPERIMENT} file  $(RGISfile ${MEANINPUTS} ${DOMAIN}+ AirTempAcc_time SDML ${RESOLUTION}+ static)";      (( ++DataNum ))
#DATASOURCES[${DataNum}]="DischargeAcc    				static  ${EXPERIMENT} file  $(RGISfile ${MEANINPUTS} ${DOMAIN}+ DischargeAcc  SDML ${RESOLUTION}+ static)";      (( ++DataNum ))
DATASOURCES[${DataNum}]="BQART_GNP    					static  ${EXPERIMENT} file  $(RGISfile ${RGISCSDMS} ${DOMAIN}+ BQART-GNP    BQART ${RESOLUTION}+ static)";      (( ++DataNum ))
DATASOURCES[${DataNum}]="BQART_Lithology   			    static  ${EXPERIMENT} file  $(RGISfile ${RGISCSDMS} ${DOMAIN}+ BQART-Lithology SnM07 ${RESOLUTION}+ static)";      (( ++DataNum ))

DATASOURCES[${DataNum}]="Population  					static  ${EXPERIMENT} file  $(RGISfile ${RGISARCHIVE} ${DOMAIN}+ population    GPWv4AdjUN2015_Inter ${RESOLUTION}+ TS annual 2000)";      (( ++DataNum ))
DATASOURCES[${DataNum}]="Population  					dynamic  ${EXPERIMENT} file  $(RGISfile ${RGISARCHIVE} ${DOMAIN}+ population    GPWv4AdjUN2015_Inter ${RESOLUTION}+ TS annual xxxx)";      (( ++DataNum ))

DATASOURCES[${DataNum}]="RiverSlope  					static  ${EXPERIMENT} file  $(RGISfile ${RGISCSDMS} ${DOMAIN}+ RiverSlope   Lin2019smooth2 ${RESOLUTION}+ static)";      (( ++DataNum ))
DATASOURCES[${DataNum}]="SedimentTrapping				static  ${EXPERIMENT} file  $(RGISfile ${RGISCSDMS} ${DOMAIN}+ SedimentTrapping   Grill ${RESOLUTION}+ static)";      (( ++DataNum ))

DATASOURCES[${DataNum}]="Bankfull_Qn                	static  ${EXPERIMENT} const 0";  (( ++DataNum ))
#define the reacurance bankfull discharge 1 for input 0 for no Qbf simulation 
DATASOURCES[${DataNum}]="FlowCoefficient              	static  ${EXPERIMENT} const 1"; (( ++DataNum )) #define the return flow (from floodplain to the river) coefficient
DATASOURCES[${DataNum}]="BankfullQ    					static  ${EXPERIMENT} file  $(RGISfile ${RGISCSDMS} ${DOMAIN}+ Bankfull-Q  newHydroSTN30v9.3 ${RESOLUTION}+ static)";      (( ++DataNum ))
#DATASOURCES[${DataNum}]="BQART_An		      			static  ${EXPERIMENT} file  $(RGISfile ${RGISARCHIVE1} ${DOMAIN}+ bqart_an  		    SDML 	${RESOLUTION}+ static)";	 (( ++DataNum ))
DATASOURCES[${DataNum}]="SedPristine                 	static  ${EXPERIMENT}  const 0"; 				(( ++DataNum ))
#1=pristine sediment simulation (Eh=1, Te=0)                      
#2=semi-pristine sediment simulation (Te=0) 
#3=input sediment trapping layer
DATASOURCES[${DataNum}]="VelocityTH                 	static  ${EXPERIMENT}  const 0.0"; 				(( ++DataNum ))
#0.0 value will lead to no trashold
DATASOURCES[${DataNum}]="BedloadEquation               	static  ${EXPERIMENT}  const 2"; 				(( ++DataNum ))
#1 = Modified Bagnold (1966)
#2 = Lammers & Bledsoe (2018)
#3 = Ashley Empirical equation 
#4 = Syvitski et al. (2019) -- Do not use - own module
DATASOURCES[${DataNum}]="CalculateTotalSediment        	static  ${EXPERIMENT}  const 0"; 				(( ++DataNum ))
#0 = NO - only calculate suspended sediment 
#1 = YES - will calculate bedload and suspended bed fluxes

#TP2M inputs:
DATASOURCES[${DataNum}]="Technology1             static      ${EXPERIMENT}  const 0";  (( ++DataNum ))
DATASOURCES[${DataNum}]="Technology2             static      ${EXPERIMENT}  const 0"; 				(( ++DataNum ))                              
DATASOURCES[${DataNum}]="Technology3             static      ${EXPERIMENT}  const 0"; 				(( ++DataNum ))                                
DATASOURCES[${DataNum}]="Technology4             static      ${EXPERIMENT}  const 0"; 				(( ++DataNum ))                              
DATASOURCES[${DataNum}]="Technology1             dynamic     ${EXPERIMENT}  const 0"; 				(( ++DataNum ))                              
DATASOURCES[${DataNum}]="Technology2             dynamic      ${EXPERIMENT}  const 0"; 				(( ++DataNum ))                          
DATASOURCES[${DataNum}]="Technology3             dynamic      ${EXPERIMENT}  const 0"; 				(( ++DataNum ))                             
DATASOURCES[${DataNum}]="Technology4             dynamic      ${EXPERIMENT}  const 0"; 				(( ++DataNum ))                               
DATASOURCES[${DataNum}]="NamePlate1              static       ${EXPERIMENT}  const 0"; 				(( ++DataNum ))
DATASOURCES[${DataNum}]="NamePlate2              static      ${EXPERIMENT}  const 0"; 				(( ++DataNum ))
DATASOURCES[${DataNum}]="NamePlate3              static       ${EXPERIMENT}  const 0"; 				(( ++DataNum ))
DATASOURCES[${DataNum}]="NamePlate4              static      ${EXPERIMENT}  const 0"; 				(( ++DataNum ))
DATASOURCES[${DataNum}]="NamePlate1              dynamic      ${EXPERIMENT}  const 0"; 				(( ++DataNum ))
DATASOURCES[${DataNum}]="NamePlate2              dynamic      ${EXPERIMENT}  const 0"; 				(( ++DataNum ))
DATASOURCES[${DataNum}]="NamePlate3              dynamic     ${EXPERIMENT}  const 0"; 				(( ++DataNum ))
DATASOURCES[${DataNum}]="NamePlate4              dynamic     ${EXPERIMENT}  const 0"; 				(( ++DataNum ))                   
DATASOURCES[${DataNum}]="Efficiency1             static      ${EXPERIMENT}  const 0"; 				(( ++DataNum ))                      
DATASOURCES[${DataNum}]="Efficiency2             static      ${EXPERIMENT}  const 0"; 				(( ++DataNum ))
DATASOURCES[${DataNum}]="Efficiency3             static      ${EXPERIMENT}  const 0"; 				(( ++DataNum ))                     
DATASOURCES[${DataNum}]="Efficiency4             static      ${EXPERIMENT}  const 0"; 				(( ++DataNum ))                      
DATASOURCES[${DataNum}]="Efficiency1             dynamic     ${EXPERIMENT}  const 0"; 				(( ++DataNum ))                     
DATASOURCES[${DataNum}]="Efficiency2             dynamic      ${EXPERIMENT}  const 0"; 				(( ++DataNum ))                       
DATASOURCES[${DataNum}]="Efficiency3             dynamic      ${EXPERIMENT}  const 0"; 				(( ++DataNum ))                      
DATASOURCES[${DataNum}]="Efficiency4             dynamic      ${EXPERIMENT}  const 0"; 				(( ++DataNum ))                        
DATASOURCES[${DataNum}]="Demand1                 static       ${EXPERIMENT}  const 0"; 				(( ++DataNum ))
DATASOURCES[${DataNum}]="Demand2                 static     ${EXPERIMENT}  const 0"; 				(( ++DataNum ))
DATASOURCES[${DataNum}]="Demand3                 static      ${EXPERIMENT}  const 0"; 				(( ++DataNum ))
DATASOURCES[${DataNum}]="Demand4                 static      ${EXPERIMENT}  const 0"; 				(( ++DataNum ))
DATASOURCES[${DataNum}]="Demand1                 dynamic     ${EXPERIMENT}  const 0"; 				(( ++DataNum ))
DATASOURCES[${DataNum}]="Demand2                 dynamic     ${EXPERIMENT}  const 0"; 				(( ++DataNum ))
DATASOURCES[${DataNum}]="Demand3                 dynamic     ${EXPERIMENT}  const 0"; 				(( ++DataNum ))
DATASOURCES[${DataNum}]="Demand4                 dynamic    ${EXPERIMENT}  const 0"; 				(( ++DataNum ))
DATASOURCES[${DataNum}]="FuelType1               static     ${EXPERIMENT}  const 0"; 				(( ++DataNum ))
DATASOURCES[${DataNum}]="FuelType2               static      ${EXPERIMENT}  const 0"; 				(( ++DataNum ))
DATASOURCES[${DataNum}]="FuelType3               static      ${EXPERIMENT}  const 0"; 				(( ++DataNum ))
DATASOURCES[${DataNum}]="FuelType4               static     ${EXPERIMENT}  const 0"; 				(( ++DataNum ))
DATASOURCES[${DataNum}]="FuelType1               dynamic      ${EXPERIMENT}  const 0"; 				(( ++DataNum ))
DATASOURCES[${DataNum}]="FuelType2               dynamic   ${EXPERIMENT}  const 0"; 				(( ++DataNum ))
DATASOURCES[${DataNum}]="FuelType3               dynamic    ${EXPERIMENT}  const 0"; 				(( ++DataNum ))
DATASOURCES[${DataNum}]="FuelType4               dynamic   ${EXPERIMENT}  const 0"; 				(( ++DataNum ))
DATASOURCES[${DataNum}]="LakeOcean1              static    ${EXPERIMENT}  const 0"; 				(( ++DataNum ))
DATASOURCES[${DataNum}]="CWA_Delta               static    ${EXPERIMENT}  const 0"; 				(( ++DataNum ))
DATASOURCES[${DataNum}]="CWA_Limit               static    ${EXPERIMENT}  const 0"; 				(( ++DataNum ))
DATASOURCES[${DataNum}]="GW_Temp                 static    ${EXPERIMENT}  const 10"; 				(( ++DataNum ))
DATASOURCES[${DataNum}]="CWA_OnOff               static     ${EXPERIMENT}  const 0"; 				(( ++DataNum ))# 0=off, 1=delta limit, 2=strict threshold
DATASOURCES[${DataNum}]="Downstream_OnOff        static     ${EXPERIMENT}  const 0"; 				(( ++DataNum ))  # 0=off, 1=on
DATASOURCES[${DataNum}]="CWA_316b_OnOff        static      ${EXPERIMENT}  const 0"; 				(( ++DataNum )) # 0=off, 1=on

(( OptNum  = 0 ))
OPTIONS[${OptNum}]="CropParameterFileName   ${CROPFILE}";       (( ++OptNum ))
OPTIONS[${OptNum}]="Model                   sedimentflux";      (( ++OptNum )) #sedimentflux balance discharge bedloadflux waterdensity particulatenutrients
OPTIONS[${OptNum}]="Discharge               calculate";         (( ++OptNum ))
OPTIONS[${OptNum}]="DischargeMean           calculate";         (( ++OptNum ))
OPTIONS[${OptNum}]="DoubleCropping          FirstSeason";       (( ++OptNum ))
OPTIONS[${OptNum}]="GroundWaterBETA         0.01666667";        (( ++OptNum ))
OPTIONS[${OptNum}]="InfiltrationFraction    0.5";               (( ++OptNum ))
OPTIONS[${OptNum}]="Muskingum               static";            (( ++OptNum ))
OPTIONS[${OptNum}]="Precipitation           fraction";          (( ++OptNum ))
#OPTIONS[${OptNum}]="Precipitation           input";          (( ++OptNum ))
OPTIONS[${OptNum}]="RainInterception        none";              (( ++OptNum ))
OPTIONS[${OptNum}]="RainInfiltration        simple";            (( ++OptNum ))
#OPTIONS[${OptNum}]="RainPET                 Hamon";             	(( ++OptNum )) #Hamon Jensen Turc
OPTIONS[${OptNum}]="RainPET                 Turc";             	(( ++OptNum )) #Hamon Jensen Turc
OPTIONS[${OptNum}]="SolarRadiation          input";             (( ++OptNum )) #cloud sun
OPTIONS[${OptNum}]="GrossRadiance          standard";           (( ++OptNum )) 
OPTIONS[${OptNum}]="Riverbed                slope-independent"; (( ++OptNum ))
OPTIONS[${OptNum}]="Routing                 muskingum";         (( ++OptNum ))
OPTIONS[${OptNum}]="Runoff                  calculate";         (( ++OptNum ))
OPTIONS[${OptNum}]="RunoffVolume            calculate";         (( ++OptNum ))
OPTIONS[${OptNum}]="SoilMoisture            bucket";            (( ++OptNum ))
OPTIONS[${OptNum}]="SoilTemperature         none";              (( ++OptNum ))
OPTIONS[${OptNum}]="SoilMoistureALPHA       5.0";               (( ++OptNum ))
OPTIONS[${OptNum}]="SoilWaterCapacity       calculate";         (( ++OptNum ))
OPTIONS[${OptNum}]="SurfRunoffPool          none";              (( ++OptNum ))
OPTIONS[${OptNum}]="WetlandAreaFraction     none";              (( ++OptNum ))
OPTIONS[${OptNum}]="CloudCover	     		calculate";         (( ++OptNum ))
OPTIONS[${OptNum}]="SpecificHumidity             calculate";            (( ++OptNum )) # 'input' 'calculate' and'none'
OPTIONS[${OptNum}]="RelativeHumidity             input";        (( ++OptNum )) # 'input' 'calculate' and'none'
#OPTIONS[${OptNum}]="ThermalInputs3          input";                (( ++OptNum ))   

(( OutNum  = 0 ))
#OUTPUTS[${OutNum}]="AirTemperature";                            (( ++OutNum ))
OUTPUTS[${OutNum}]="Discharge";                                 (( ++OutNum ))
OUTPUTS[${OutNum}]="DischargeMean";					(( ++OutNum )) #NEW !!!!!!! (S.C)
#OUTPUTS[${OutNum}]="Evapotranspiration";                        (( ++OutNum ))
#OUTPUTS[${OutNum}]="GroundWaterChange";                         (( ++OutNum ))
#OUTPUTS[${OutNum}]="MonthlyPrecipitation";                      (( ++OutNum ))
#OUTPUTS[${OutNum}]="Precipitation";                             (( ++OutNum ))
#OUTPUTS[${OutNum}]="PrecipitationFraction";                     (( ++OutNum ))
#OUTPUTS[${OutNum}]="RainPET";                                   (( ++OutNum ))
#OUTPUTS[${OutNum}]="RainSurfaceRunoff";                         (( ++OutNum ))
#OUTPUTS[${OutNum}]="Runoff";                                    (( ++OutNum ))
#OUTPUTS[${OutNum}]="SnowPackChange";                            (( ++OutNum ))
#OUTPUTS[${OutNum}]="SoilMoisture";                              (( ++OutNum ))
#OUTPUTS[${OutNum}]="SoilMoistureChange";                        (( ++OutNum ))
#OUTPUTS[${OutNum}]="RelativeSoilMoisture";                      (( ++OutNum ))
#OUTPUTS[${OutNum}]="WaterBalance";                              (( ++OutNum ))
OUTPUTS[${OutNum}]="SedimentFlux";                              	(( ++OutNum )) #NEW !!!!!!! (S.C)
OUTPUTS[${OutNum}]="BedloadFlux";					(( ++OutNum )) #NEW !!!!!!! (S.C)
OUTPUTS[${OutNum}]="SuspendedBedFlux";					(( ++OutNum )) #NEW !!!!!!! (S.C)
#OUTPUTS[${OutNum}]="TotalSedimentFlux";					(( ++OutNum )) #NEW !!!!!!! (S.C)
OUTPUTS[${OutNum}]="WashloadFlux";					(( ++OutNum )) #NEW !!!!!!! (S.C)
#OUTPUTS[${OutNum}]="LithologyMean";                              	(( ++OutNum )) #NEW !!!!!!! (S.C)
#OUTPUTS[${OutNum}]="BQART_A";                          		(( ++OutNum )) #NEW !!!!!!! (S.C)
#OUTPUTS[${OutNum}]="BQART_B";                    		        (( ++OutNum )) #NEW !!!!!!! (S.C)
#OUTPUTS[${OutNum}]="BQART_R";           		                (( ++OutNum )) #NEW !!!!!!! (S.C)
#OUTPUTS[${OutNum}]="BQART_T";           		                (( ++OutNum )) #NEW !!!!!!! (S.C)
#OUTPUTS[${OutNum}]="BQART_Qbar_m3s";					(( ++OutNum ))
#OUTPUTS[${OutNum}]="BQART_Qbar_km3y";           	                (( ++OutNum )) #NEW !!!!!!! (S.C)
#OUTPUTS[${OutNum}]="NewDischargeAcc";           	                (( ++OutNum )) #NEW !!!!!!! (S.C)
#OUTPUTS[${OutNum}]="NewAirTempAcc_time";           		        (( ++OutNum )) #NEW !!!!!!! (S.C)
#OUTPUTS[${OutNum}]="AirTempAcc_space"; 				(( ++OutNum )) #NEW !!!!!!! (S.C)
#OUTPUTS[${OutNum}]="NewTimeSteps";           		                (( ++OutNum )) #NEW !!!!!!! (S.C)
#OUTPUTS[${OutNum}]="AverageNSteps";							(( ++OutNum )) #NEW !!!!!!! (S.C)
#OUTPUTS[${OutNum}]="PopulationAcc";            	        	(( ++OutNum )) #NEW !!!!!!! (S.C)
#OUTPUTS[${OutNum}]="PopulationDensity";           	                (( ++OutNum )) #NEW !!!!!!! (S.C)
#OUTPUTS[${OutNum}]="GNPAreaAcc";           		                (( ++OutNum )) #NEW !!!!!!! (S.C)
#OUTPUTS[${OutNum}]="MeanGNP";           		                (( ++OutNum )) #NEW !!!!!!! (S.C)
#OUTPUTS[${OutNum}]="BQART_Eh";   					(( ++OutNum ))
#OUTPUTS[${OutNum}]="BQART_Te";   					(( ++OutNum ))
OUTPUTS[${OutNum}]="Qs_bar";   						(( ++OutNum ))
#OUTPUTS[${OutNum}]="TeAacc";   					(( ++OutNum ))
#OUTPUTS[${OutNum}]="MDVarContributingAreaAcc";   					(( ++OutNum ))
#OUTPUTS[${OutNum}]="ReservoirCapacity";			(( ++OutNum ))
#OUTPUTS[${OutNum}]="ResStorageAcc";					(( ++OutNum ))
#OUTPUTS[${OutNum}]="SmallReservoirCapacity"; (( ++OutNum ))
OUTPUTS[${OutNum}]="Qb_bar_Ashley";					(( ++OutNum ))
#OUTPUTS[${OutNum}]="DeltaBedload";					(( ++OutNum )) #NEW !!!!!!! (S.C)
OUTPUTS[${OutNum}]="DeltaQs";						(( ++OutNum )) #NEW !!!!!!! (S.C)
#OUTPUTS[${OutNum}]="QsConc";						(( ++OutNum )) #NEW !!!!!!! (S.C)
OUTPUTS[${OutNum}]="QsYield";						(( ++OutNum )) #NEW !!!!!!! (S.C)
#OUTPUTS[${OutNum}]="OverBankQ";					(( ++OutNum )) #NEW !!!!!!! (S.C)
#OUTPUTS[${OutNum}]="POCarbonConcentration";				(( ++OutNum )) #NEW !!!!!!! (S.C)
#OUTPUTS[${OutNum}]="POCarbonYield";					(( ++OutNum )) #NEW !!!!!!! (S.C)
#OUTPUTS[${OutNum}]="POCarbonPercent";					(( ++OutNum )) #NEW !!!!!!! (S.C)
#OUTPUTS[${OutNum}]="POCarbonFlux";					(( ++OutNum )) #NEW !!!!!!! (S.C)
#OUTPUTS[${OutNum}]="PNitrogenConcentration";				(( ++OutNum )) #NEW !!!!!!! (S.C)
#OUTPUTS[${OutNum}]="PNitrogenYield";					(( ++OutNum )) #NEW !!!!!!! (S.C)
#OUTPUTS[${OutNum}]="PNitrogenPercent";					(( ++OutNum )) #NEW !!!!!!! (S.C)
#OUTPUTS[${OutNum}]="PNitrogenFlux";					(( ++OutNum )) #NEW !!!!!!! (S.C)
#OUTPUTS[${OutNum}]="PPhosphorusConcentration";				(( ++OutNum )) #NEW !!!!!!! (S.C)
#OUTPUTS[${OutNum}]="PPhosphorusYield";					(( ++OutNum )) #NEW !!!!!!! (S.C)
#OUTPUTS[${OutNum}]="PPhosphorusPercent";				(( ++OutNum )) #NEW !!!!!!! (S.C)
#OUTPUTS[${OutNum}]="PPhosphorusFlux";					(( ++OutNum )) #NEW !!!!!!! (S.C)
#OUTPUTS[${OutNum}]="RiverbedWidth";					(( ++OutNum ))
#OUTPUTS[${OutNum}]="WaterDensity";					(( ++OutNum ))
#OUTPUTS[${OutNum}]="QxT_WaterTemp";					(( ++OutNum ))
#OUTPUTS[${OutNum}]="DeltaWd";					 	(( ++OutNum ))
#OUTPUTS[${OutNum}]="UpStreamWd";					(( ++OutNum ))
#OUTPUTS[${OutNum}]="RiverbedSlope";						(( ++OutNum ))
#OUTPUTS[${OutNum}]="RiverbedAvgDepthMean";				(( ++OutNum ))
#OUTPUTS[${OutNum}]="RiverbedWidthMean";					(( ++OutNum ))
#OUTPUTS[${OutNum}]="RiverbedVelocityMean";				(( ++OutNum ))
#OUTPUTS[${OutNum}]="KinematicViscosity";							(( ++OutNum ))
#OUTPUTS[${OutNum}]="SettlingVelocity";				(( ++OutNum ))
OUTPUTS[${OutNum}]="StreamPower";				(( ++OutNum ))
OUTPUTS[${OutNum}]="CritStreamPower";				(( ++OutNum ))
OUTPUTS[${OutNum}]="ParticleSize";				(( ++OutNum ))

if [[ "${DISTURBED}" == "on" ]]
then
	OPTIONS[${OptNum}]="IrrigatedAreaMap        ${IRRMAP}";      (( ++OptNum ))
	OPTIONS[${OptNum}]="Irrigation              calculate";      (( ++OptNum ))
	OPTIONS[${OptNum}]="IrrReferenceETP         Hamon";          (( ++OptNum ))
	OPTIONS[${OptNum}]="IrrUptakeRiver          calculate";      (( ++OptNum ))
	OPTIONS[${OptNum}]="IrrUptakeGrdWater       calculate";      (( ++OptNum ))
	OPTIONS[${OptNum}]="Reservoirs              calculate";      (( ++OptNum ))
	OPTIONS[${OptNum}]="SmallReservoirCapacity  calculate";      (( ++OptNum ))

	#OUTPUTS[${OutNum}]="IrrGrossDemand";                         (( ++OutNum ))
	#OUTPUTS[${OutNum}]="IrrReturnFlow";                          (( ++OutNum ))
	#OUTPUTS[${OutNum}]="IrrUptakeExcess";                        (( ++OutNum ))
	#OUTPUTS[${OutNum}]="IrrUptakeExternal";                      (( ++OutNum ))
	#OUTPUTS[${OutNum}]="IrrUptakeGroundWater";                   (( ++OutNum ))
	#OUTPUTS[${OutNum}]="IrrUptakeRiver";                         (( ++OutNum ))
	#OUTPUTS[${OutNum}]="ReservoirStorage";                       (( ++OutNum ))

	#OUTPUTS[${OutNum}]="SmallReservoirStorage";                  (( ++OutNum ))
	#OUTPUTS[${OutNum}]="SmallReservoirStorageChange";            (( ++OutNum ))
	#OUTPUTS[${OutNum}]="SmallReservoirCapacity";                 (( ++OutNum ))
	#OUTPUTS[${OutNum}]="IrrUptakeBalance";                       (( ++OutNum ))
	#OUTPUTS[${OutNum}]="IrrWaterBalance";                        (( ++OutNum ))
else
	OPTIONS[${OptNum}]="Irrigation              none";           (( ++OptNum ))
	OPTIONS[${OptNum}]="Reservoirs              none";           (( ++OptNum ))
fi
echo "${DATASOURCES[@]}"
FwDataSrc     "${DATASOURCES[@]}" || exit -1
FwOptions         "${OPTIONS[@]}" || exit -1
FwOutputs         "${OUTPUTS[@]}" || exit -1
FwRun  "${EXPERIMENT}" "${STARTYEAR}" "${ENDYEAR}" || exit -1
