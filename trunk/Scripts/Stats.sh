#!/bin/bash

if [ "${0%/*}" != "." ]; then PROJECTDIR="${0%/*}"; PROJECTDIR="${PROJECTDIR%/Scripts}"; else PROJECTDIR=".."; fi 
     SCRIPT="${0##*/}"; SCRIPT="${SCRIPT%.sh}";
RGISARCHIVE="${PROJECTDIR}/RGISarchive"
  RGISSTATS="${PROJECTDIR}/RGISstats"
RGISRESULTS="${PROJECTDIR}/RGISresults"
 RGISBINDIR="/usr/local/share/ghaas/bin"

  STARTYEAR="1901"
    ENDYEAR="2006"

function Append ()
{
	local     domain="${1}"
	local   scenario="${2}"
	local   variable="${3}"
	local      tstep="${4}"
	local resolution="${5}"

	local      files=""
	local separator=" "
	for (( year = "${STARTYEAR}"; year <= "${ENDYEAR}"; ++year ))
	do
		local  filename="${RGISRESULTS}/${variable}/${domain}_${scenario}_${variable}_${tstep}TS${year}_${resolution}.gdbc"
		local     files="${files}${separator}${filename}"
		local separator=" -a "
	done

	local filename="${RGISSTATS}/${variable}/${domain}_${scenario}_${variable}_${tstep}TS${STARTYEAR}-${ENDYEAR}_${resolution}.gdbc"

	grdAppendLayers -t "${domain}, ${scenario} ${variable} (AnnualTS, ${STARTYEAR}-${ENDYEAR}, ${resolution} V1.0" \
	                -u "${variable}" -d "${domain}"  ${files}     "${filename}"
}

function Climatology ()
{
	local     domain="${1}"
	local   scenario="${2}"
	local   variable="${3}"
	local resolution="${4}"

	local tsFile="${RGISSTATS}/${variable}/${domain}_${scenario}_${variable}_mTS${STARTYEAR}-${ENDYEAR}_${resolution}.gdbc"
	local ltFile="${RGISSTATS}/${variable}/${domain}_${scenario}_${variable}_mLT${STARTYEAR}-${ENDYEAR}_${resolution}.gdbc"

	grdCycleMean	 -t "${domain}, ${scenario} ${variable} (MonthlyLT, ${STARTYEAR}-${ENDYEAR}, ${resolution})" \
	                -n 12 -s blue -d "${domain}" -u "${variable}" "${tsFile}" - |\
	grdDateLayers   -e "month" - "${ltFile}"
}

function CellStats ()
{
	local     domain="${1}"
	local   scenario="${2}"
	local   variable="${3}"
	local resolution="${4}"

	local filename="${RGISSTATS}/${variable}/${domain}_${scenario}_${variable}_aTS${STARTYEAR}-${ENDYEAR}_${resolution}.gdbc"
	local statname="${RGISSTATS}/${variable}/${domain}_${scenario}_${variable}_aStats${STARTYEAR}-${ENDYEAR}_${resolution}.gdbc"
	grdCellStats    -t "${domain}, ${scenario} ${variable} (Stats, ${STARTYEAR}-${ENDYEAR}, ${resolution} V1.0" \
	                -u "${variable}" -d "${domain}" "${filename}" "${statname}"
}

function Process ()
{
    local     domain="${1}"
    local   scenario="${2}"
    local   variable="${3}"
    local resolution="${4}"

	[ -e "${RGISSTATS}/${variable}" ] || mkdir -p "${RGISSTATS}/${variable}"
	Append "${domain}" "${scenario}"  "${variable}" "a" "${resolution}"
	Append "${domain}" "${scenario}"  "${variable}" "m" "${resolution}"

    Climatology "${domain}" "${scenario}"  "${variable}" "${resolution}"
    CellStats   "${domain}" "${scenario}"  "${variable}" "${resolution}"
    rm "${RGISSTATS}/${variable}/${domain}_${scenario}_${variable}_aTS${STARTYEAR}-${ENDYEAR}_${resolution}.gdbc"
    rm "${RGISSTATS}/${variable}/${domain}_${scenario}_${variable}_mTS${STARTYEAR}-${ENDYEAR}_${resolution}.gdbc"
}

Process "Global" "UDel"      "Precipitation"      "30min"
Process "Global" "UDelprist" "Discharge"          "30min"
Process "Global" "UDeldist"  "Discharge"          "30min"
Process "Global" "UDelprist" "Runoff"             "30min"
Process "Global" "UDeldist"  "Runoff"             "30min"
Process "Global" "UDelprist" "Evapotranspiration" "30min"
Process "Global" "UDeldist"  "Evapotranspiration" "30min"
Process "Global" "UDelprist" "SoilMoisture"       "30min"
Process "Global" "UDeldist"  "SoilMoisture"       "30min"
