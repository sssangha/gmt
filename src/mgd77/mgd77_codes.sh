#!/bin/sh
#	$Id: mgd77_codes.sh,v 1.3 2006-01-31 00:18:34 pwessel Exp $
#
#	Convert the trkdas.cod file into two useable files:
#	1. mgd77_codes.h for inclusion by mgd77info.c
#	2. mgd77_codes.txt for inclusion by man page mgd77info.txt

#rm -f trkdas.cod
#curl http://www.ngdc.noaa.gov/mgg/dat/geodas/inventories/trkdas.cod --remote-name
sed -e 's/ *$//g' < trkdas.cod > $$.dat
awk '{if (length($1) == 2) {printf "\t{\"%s\", \"%s\"},\n", $1, substr($0, 6)}}' $$.dat > $$.1
awk 'BEGIN {code = -1; last = "--"}; {if (length($1) > 2) {printf "\t{ %d, \"%s\", \"%s\"},\n", code, $1, substr($0, 6)} else {code++}}' $$.dat > $$.2
awk '{if (length($1) == 2) {printf "(%s) %s,\n", $1, substr($0, 6)}}' $$.dat > $$.4
n_agencies=`cat $$.1 | wc -l | awk '{printf "%d\n", $1}'`
n_vessels=`cat $$.2 | wc -l | awk '{printf "%d\n", $1}'`
awk -F, '{if (NR == "'${n_agencies}'") {printf "%s.\n", $1} else {printf "%s,\n", $1}}' $$.4 > mgd77_codes.txt
cat << EOF > mgd77_codes.h
/* Automatically created by mgd77_codes.sh via GNUmakefile */

#define MGD77_N_AGENCIES ${n_agencies}
#define MGD77_N_VESSELS ${n_vessels}

struct MGD77_AGENCY {
	char code[4];
	char name[80];
};

struct MGD77_AGENCY MGD77_agency[MGD77_N_AGENCIES] = {
EOF
cat $$.1 >> mgd77_codes.h
cat << EOF >> mgd77_codes.h
};

struct MGD77_VESSEL {
	int agent;
	char code[8];
	char name[80];
};

struct MGD77_VESSEL MGD77_vessel[MGD77_N_VESSELS] = {
EOF
cat $$.2 >> mgd77_codes.h
echo "};"  >> mgd77_codes.h
rm -f $$.*
