/*--------------------------------------------------------------------
 *
 *	Copyright (c) 1991-2020 by the GMT Team (https://www.generic-mapping-tools.org/team.html)
 *	See LICENSE.TXT file for copying and redistribution conditions.
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU Lesser General Public License as published by
 *	the Free Software Foundation; version 3 or any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU Lesser General Public License for more details.
 *
 *	Contact info: www.generic-mapping-tools.org
 *--------------------------------------------------------------------*/
/*
 * Author:	Joaquim Luis
 * Date:	6-Feb-2020
 * Version:	6 API
 *
 * Brief synopsis: 
 *
 */

#include "gmt_dev.h"

#define THIS_MODULE_CLASSIC_NAME	"gmtgdal"
#define THIS_MODULE_MODERN_NAME	"gmtgdal"
#define THIS_MODULE_LIB		"core"
#define THIS_MODULE_PURPOSE	"Execute GDAL programs from GMT"
#define THIS_MODULE_KEYS	"<D{,GG}"
#define THIS_MODULE_NEEDS	""
#define THIS_MODULE_OPTIONS "->RVbhi"

/* Control structure for gmtwrite */

struct GMTGDAL_CTRL {
	struct GMTGDAL_A {	/* GDAL prog name */
		bool active;
		char *prog_name;
		char *dem_method;		/* The method name for gdaldem */
		char *dem_cpt;			/* A CPT name to use in gdaldem color-relief method */
	} A;
	struct GMTGDAL_F {	/* -F<gdal options> */
		bool active;
		char *opts;
	} F;
	struct GMTGDAL_G {	/* -G<grdfile> */
		bool active;
		char *file;
	} G;
	struct GMTGDAL_M {	/* -M[+r+w] which read-write machinery. GMT or GDAL  */
		bool active;
		bool read_gdal, write_gdal;
	} M;
	struct GMTGDAL_W {	/* -W sets output data fname when writen by GDAL */
		bool active;
		char *file;
	} W;
	char *fname_in;
};

GMT_LOCAL void *New_Ctrl (struct GMT_CTRL *GMT) {	/* Allocate and initialize a new control structure */
	struct GMTGDAL_CTRL *C;

	C = gmt_M_memory (GMT, NULL, 1, struct GMTGDAL_CTRL);
	/* Initialize values whose defaults are not 0/false/NULL */
	return (C);
}

GMT_LOCAL void Free_Ctrl (struct GMT_CTRL *GMT, struct GMTGDAL_CTRL *C) {	/* Deallocate control structure */
	if (!C) return;
	if (C->A.dem_method) gmt_M_str_free (C->A.dem_method);
	if (C->A.dem_cpt) gmt_M_str_free (C->A.dem_cpt);
	gmt_M_free (GMT, C);
}

GMT_LOCAL int usage (struct GMTAPI_CTRL *API, int level) {
	const char *name = gmt_show_name_and_purpose (API, THIS_MODULE_LIB, THIS_MODULE_CLASSIC_NAME, THIS_MODULE_PURPOSE);
	if (level == GMT_MODULE_PURPOSE) return (GMT_NOERROR);
	GMT_Message (API, GMT_TIME_NONE, "usage: %s <infile> -A<prog> [-F<gd opts>] [-G<outgrid>] [-M[+r+w]] [-W<outds>] [%s] [%s]\n", name, GMT_Rx_OPT, GMT_V_OPT);

	if (level == GMT_SYNOPSIS) return (GMT_MODULE_SYNOPSIS);

	GMT_Message (API, GMT_TIME_NONE, "\t   Specify input file name\n");
	GMT_Message (API, GMT_TIME_NONE, "\t-A Specify the GDAL program name.\n");
	GMT_Message (API, GMT_TIME_NONE, "\t-F list of GDAL options for the selected program in -A wrapped in double quotes.\n");
	GMT_Message (API, GMT_TIME_NONE, "\t-G Name of output grid.\n");
	GMT_Message (API, GMT_TIME_NONE, "\t-M Means to both read and write with GDAL. Use +r to only read or +w to write.\n");
	GMT_Message (API, GMT_TIME_NONE, "\t-W Name of output dataset when writen by GDAL.\n");
	GMT_Message (API, GMT_TIME_NONE, "\n\tOPTIONS:\n");
	GMT_Option (API, "R,V,.");

	return (GMT_MODULE_USAGE);
}

GMT_LOCAL int parse (struct GMT_CTRL *GMT, struct GMTGDAL_CTRL *Ctrl, struct GMT_OPTION *options) {
	/* This parses the options provided to grdcut and sets parameters in CTRL.
	 * Any GMT common options will override values set previously by other commands.
	 * It also replaces any file names specified as input or output with the data ID
	 * returned when registering these sources/destinations with the API.
	 */
	char txt_a[GMT_LEN16] = {""}, txt_b[GMT_LEN256] = {""}, *p;
	unsigned int n_errors = 0, n_files = 0;
	struct GMT_OPTION *opt = NULL;

	for (opt = options; opt; opt = opt->next) {	/* Process all the options given */

		switch (opt->option) {
			/* Processes program-specific parameters */
			case '<':	/* Input file(s) */
				if (gmt_check_filearg (GMT, '<', opt->arg, GMT_IN, GMT_IS_GRID) ||
				    gmt_check_filearg (GMT, '<', opt->arg, GMT_IN, GMT_IS_DATASET)) {
					Ctrl->fname_in = opt->arg;
					n_files++;
				}
				else
					n_errors++;
				break;

			case 'A':	/* GDAL prog name */
				Ctrl->A.active = true;
				Ctrl->A.prog_name = opt->arg;
				if (gmt_get_modifier (opt->arg, 'm', txt_a))
					Ctrl->A.dem_method = strdup(txt_a);
				if (gmt_get_modifier (opt->arg, 'c', txt_b))
					Ctrl->A.dem_cpt = strdup(txt_b);
				if (p = strchr (Ctrl->A.prog_name, '+'))
					p[0] = '\0';			/* Strip the modifiers part */
				break;

			case 'F':	/* -F<gdal options> */
				Ctrl->F.active = true;
				Ctrl->F.opts = strdup(opt->arg);
				break;

			case 'G':	/* Output file */
				if ((Ctrl->G.active = gmt_check_filearg (GMT, 'G', opt->arg, GMT_OUT, GMT_IS_GRID)) != 0)
					Ctrl->G.file = opt->arg;
				else
					n_errors++;
				break;

			case 'I':	/* Grid spacings */
				n_errors += gmt_parse_inc_option (GMT, 'I', opt->arg);
				break;

			case 'M':	/* -M[+r+w] which read-write machinery. GMT or GDAL */
				Ctrl->M.active = true;
				if (strstr(opt->arg, "+r")) Ctrl->M.read_gdal  = true;
				else if (strstr(opt->arg, "+w")) Ctrl->M.write_gdal = true;
				else if (opt->arg[0] == '\0') Ctrl->M.read_gdal = Ctrl->M.write_gdal = true;
				break;

			case 'W':	/* -W<fname> sets output VECTOR data fname when writen by GDAL */
				Ctrl->W.active = true;
				Ctrl->W.file = opt->arg;
				Ctrl->M.write_gdal = true;
				break;

			default:	/* Report bad options */
				n_errors += gmt_default_error (GMT, opt->option);
				break;
		}
	}

	n_errors += gmt_M_check_condition (GMT, n_files == 0, "No input files given\n");
	n_errors += gmt_M_check_condition (GMT, Ctrl->A.active && strcmp(Ctrl->A.prog_name, "grid") && strcmp(Ctrl->A.prog_name, "info") && strcmp(Ctrl->A.prog_name, "dem"), "Option -A: Must select dem, grid, or info\n");
	
	return (n_errors ? GMT_PARSE_ERROR : GMT_NOERROR);
}

#define bailout(code) {gmt_M_free_options (mode); return (code);}
#define Return(code) {Free_Ctrl (GMT, Ctrl); gmt_end_module (GMT, GMT_cpy); bailout (code);}

int GMT_gmtgdal (void *V_API, int mode, void *args) {
	int error = 0;
	struct GMTGDAL_CTRL *Ctrl = NULL;
	struct GMT_CTRL *GMT = NULL, *GMT_cpy = NULL;
	struct GMT_OPTION *options = NULL;
	struct GMTAPI_CTRL *API = gmt_get_api_ptr (V_API);	/* Cast from void to GMTAPI_CTRL pointer */
	struct GMT_GDALLIBRARIFIED_CTRL *st = NULL;

	/*----------------------- Standard module initialization and parsing ----------------------*/

	if (API == NULL) return (GMT_NOT_A_SESSION);
	if (mode == GMT_MODULE_PURPOSE) return (usage (API, GMT_MODULE_PURPOSE));	/* Return the purpose of program */
	options = GMT_Create_Options (API, mode, args);	if (API->error) return (API->error);	/* Set or get option list */

	if ((error = gmt_report_usage (API, options, 0, usage)) != GMT_NOERROR) bailout (error);	/* Give usage if requested */

	/* Parse the command-line arguments */

	if ((GMT = gmt_init_module (API, THIS_MODULE_LIB, THIS_MODULE_CLASSIC_NAME, THIS_MODULE_KEYS, THIS_MODULE_NEEDS, NULL, &options, &GMT_cpy)) == NULL) bailout (API->error); /* Save current state */
	if (GMT_Parse_Common (API, THIS_MODULE_OPTIONS, options)) Return (API->error);
	Ctrl = New_Ctrl (GMT);	/* Allocate and initialize a new control structure */
	if ((error = parse (GMT, Ctrl, options)) != 0) Return (error);

	/*---------------------------- This is the gmtgdal main code ----------------------------*/

	if ((st = gmt_M_memory (GMT, NULL, 1, struct GMT_GDALLIBRARIFIED_CTRL)) == NULL) {
		GMT_Report (API, GMT_MSG_ERROR, "Memory allocation failure\n");
		Return (GMT_MEMORY_ERROR);
	}
	st->fname_in  = Ctrl->fname_in;
	st->fname_out = Ctrl->G.file;
	st->opts = Ctrl->F.opts;
	st->M.read_gdal  = Ctrl->M.read_gdal;
	st->M.write_gdal = Ctrl->M.write_gdal;
	if (!Ctrl->A.active || !strcmp(Ctrl->A.prog_name, "grid"))
		gmt_gdal_grid(GMT, st);
	else if (!strcmp(Ctrl->A.prog_name, "info"))
		gmt_gdal_info (GMT, Ctrl->fname_in, Ctrl->F.opts);
	else if (!strcmp(Ctrl->A.prog_name, "dem")) {
		if (Ctrl->A.dem_method) st->dem_method = Ctrl->A.dem_method;
		if (Ctrl->A.dem_cpt) st->dem_cpt = Ctrl->A.dem_cpt;
		GMT_Report (GMT->parent, GMT_MSG_DEBUG, "PROG = dem\nMETHOD = %s\nCPT = %s\n", st->dem_method, st->dem_cpt);
		gmt_gdal_dem(GMT, st);
	}
	else
		GMT_Report (GMT->parent, GMT_MSG_ERROR, "PROG-> \"%s\" is unknown or not implemented\n", Ctrl->A.prog_name);

	gmt_M_free (GMT, st);

	Return (error);
}
