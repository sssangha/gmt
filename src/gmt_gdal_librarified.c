/*--------------------------------------------------------------------
 *
 *	Copyright (c) 1991-2020 by the GMT Team (https://www.generic-mapping-tools.org/team.html)
 *	See LICENSE.TXT file for copying and redistribution conditions.
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU Lesser General Public License as published by
 *      the Free Software Foundation; version 3 or any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU Lesser General Public License for more details.
 *
 *	Contact info: www.generic-mapping-tools.org
 *--------------------------------------------------------------------*/

#include "gmt_dev.h"
#include "gmt_internals.h"

#if defined(HAVE_GDAL) && ((GDAL_VERSION_MAJOR >= 2) && (GDAL_VERSION_MINOR >= 1)) || (GDAL_VERSION_MAJOR >= 3)

#include <gdal_utils.h>

GMT_LOCAL GDALDatasetH gdal_vector (struct GMT_CTRL *GMT, char *filename);

/* ------------------------------------------------------------------------------------------------------------ */
GMT_LOCAL char **breakMe(struct GMT_CTRL *GMT, char *in) {
	/* Breake a string "-aa -bb -cc dd" into tokens "-aa" "-bb" "-cc" "dd" */
	/* Based on GMT_Create_Options() */
	unsigned int pos = 0, k;
	int  n_args = 0;
	bool quoted;
	size_t n_alloc = GMT_SMALL_CHUNK;
	char p[GMT_LEN512] = {""}, *txt_in;	/* Passed a single text string */
	char **args = NULL;
	
	if (!in)		/* If empty, return empty */
		return NULL;

	txt_in = strdup (in);
	args = gmt_M_memory (GMT, NULL, n_alloc, char *);

	/* txt_in can contain options that take multi-word text strings, e.g., -B+t"My title".  We avoid the problem of splitting
	 * these items by temporarily replacing spaces inside quoted strings with ASCII 31 US (Unit Separator), do the strtok on
	 * space, and then replace all ASCII 31 with space at the end (we do the same for tab using ASCII 29 GS (group separator) */
	for (k = 0, quoted = false; txt_in[k]; k++) {
		if (txt_in[k] == '\"') quoted = !quoted;	/* Initially false, becomes true at start of quote, then false when exit quote */
		else if (quoted && txt_in[k] == '\t') txt_in[k] = GMT_ASCII_GS;
		else if (quoted && txt_in[k] == ' ')  txt_in[k] = GMT_ASCII_US;
	}
	while ((gmt_strtok (txt_in, " ", &pos, p))) {	/* Break up string into separate words, and strip off double quotes */
		int i, o;
		for (k = 0; p[k]; k++) {
			if (p[k] == GMT_ASCII_GS)
				p[k] = '\t';
			else if (p[k] == GMT_ASCII_US)
				p[k] = ' ';						/* Replace spaces and tabs masked above */
		}
		for (i = o = 0; p[i]; i++)
			if (p[i] != '\"') p[o++] = p[i];	/* Ignore double quotes */
		p[o] = '\0';
		args[n_args++] = strdup(p);

		if (n_args == n_alloc) {
			n_alloc += GMT_SMALL_CHUNK;
			args = gmt_M_memory(GMT, args, n_alloc, char *);
		}
	}
	for (k = 0; txt_in[k]; k++)	/* Restore input string to prestine condition */
		if (txt_in[k] == GMT_ASCII_GS) txt_in[k] = '\t';
		else if (txt_in[k] == GMT_ASCII_US) txt_in[k] = ' ';	/* Replace spaces and tabs masked above */

	free (txt_in);
	return args;
}

/* ------------------------------------------------------------------------------------------------------------ */
GMT_LOCAL int save_grid_with_GMT(struct GMT_CTRL *GMT, GDALDatasetH hDstDS, struct GMT_GRID *Grid, char *fname) {
	/* Save a grid using the GMT machinery */
	unsigned char *tmp = NULL;
	int   nXSize, nYSize, nPixelSize, gdal_code;
	GDALRasterBandH	hBand;

	hBand = GDALGetRasterBand(hDstDS, 1);
	nPixelSize = GDALGetDataTypeSize(GDALGetRasterDataType(hBand)) / 8;	/* /8 because return value is in BITS */
	nXSize = GDALGetRasterXSize(hDstDS);
	nYSize = GDALGetRasterYSize(hDstDS);

	if ((tmp = calloc((size_t)nYSize * (size_t)nXSize, nPixelSize)) == NULL) {
		GMT_Report (GMT->parent, GMT_MSG_ERROR, "gmtgdal: failure to allocate enough memory\n");
		return -1;
	}

	if ((gdal_code = GDALRasterIO(hBand, GF_Read, 0, 0, nXSize, nYSize, tmp,
	                              nXSize, nYSize, GDALGetRasterDataType(hBand), 0, 0)) != CE_None) {
		GMT_Report (GMT->parent, GMT_MSG_ERROR, "GDALRasterIO failed to open band [err = %d]\n", gdal_code);
	}
	gmt_grd_flip_vertical (tmp, (unsigned)nXSize, (unsigned)nYSize, 0, nPixelSize);
	Grid->data = (float *)tmp;
	if (GMT_Write_Data (GMT->parent, GMT_IS_GRID, GMT_IS_FILE, GMT_IS_SURFACE, GMT_CONTAINER_AND_DATA,
						NULL, fname, Grid) != GMT_NOERROR)
		return GMT->parent->error;
	return 0;
}

/* ------------------------------------------------------------------------------------------------------------ */
char *out_name(struct GMT_GDALLIBRARIFIED_CTRL *GDLL) {
	/* Pick the right output name when saving grids depending on if that writting is done with GMT or GDAL */
	if (GDLL->M.write_gdal)			/* Write grid with the GDAL machinery */
		return GDLL->fname_out;
	else
#ifdef WIN32
		return "NUL";
#else
		return "/dev/null";
#endif
}

/* ------------------------------------------------------------------------------------------------------------ */
GMT_LOCAL void add_defaults(struct GMT_CTRL *GMT, struct GMT_GDALLIBRARIFIED_CTRL *GDLL, char *ext_opts) {
	/* Add defaults to a netCDF output when file is to be written by GDAL, or just set a MEM driver if for GMT */
	if (GDLL->opts) strcat(ext_opts, GDLL->opts); 
	if (GDLL->M.write_gdal) {
		char *ext;
		if ((ext = gmt_get_ext (GDLL->fname_out)) != NULL) {
			if (!strcasecmp(ext, "nc") || !strcasecmp(ext, "grd")) {
				if (!GDLL->opts) {
					strcat(ext_opts, " -of netCDF -co FORMAT=NC4 -co ZLEVEL=5 -co COMPRESS=DEFLATE -co CHUNKING=YES");
				}
				else {
					if (!strstr(GDLL->opts, "netCDF=")) strcat(ext_opts, " -of netCDF");
					if (!strstr(GDLL->opts, "FORMAT=")) strcat(ext_opts, " -co FORMAT=NC4");
					if (!strstr(GDLL->opts, "ZLEVEL=")) strcat(ext_opts, " -co ZLEVEL=5");
					if (!strstr(GDLL->opts, "COMPRESS=")) strcat(ext_opts, " -co COMPRESS=DEFLATE");
					if (!strstr(GDLL->opts, "CHUNKING=")) strcat(ext_opts, " -co CHUNKING=YES");
				}
			}
		}
	}
	else
		strcat(ext_opts, " -of MEM");	/* For GMT we need the data in the MEM driver */

	GMT_Report (GMT->parent, GMT_MSG_INFORMATION, "gdal options used: %s\n", ext_opts);
}

/* ------------------------------------------------------------------------------------------------------------ */
int gmt_gdal_info (struct GMT_CTRL *GMT, char *gdal_filename, char *opts) {
	GDALDatasetH	hDataset;
	GDALInfoOptions *psOptions;

	GDALAllRegister();

	hDataset = GDALOpen(gdal_filename, GA_ReadOnly);

	if (hDataset == NULL) {
		GMT_Report (GMT->parent, GMT_MSG_ERROR, "GDALOpen failed %s\n", CPLGetLastErrorMsg());
		return -1;
	}

	psOptions = GDALInfoOptionsNew(breakMe(GMT, opts), NULL);
	GMT_Message (GMT->parent, GMT_TIME_NONE, "GDAL Info\n\n%s\n", GDALInfo(hDataset, psOptions));

	GDALClose(hDataset);
	GDALInfoOptionsFree(psOptions);
	GDALDestroyDriverManager();
	return 0;
}


/* ------------------------------------------------------------------------------------------------------------ */
int gmt_gdal_grid(struct GMT_CTRL *GMT, struct GMT_GDALLIBRARIFIED_CTRL *GDLL) {
	char ext_opts[GMT_LEN512] = {""};
	int   bUsageError, error = 0;
	double dx = 0, dy = 0;
	struct GMT_GRID *Grid = NULL;
	GDALDatasetH	hSrcDS, hDstDS;
	GDALGridOptions *psOptions;

	GDALAllRegister();

	if (GDLL->M.read_gdal) 		/* Read input data with the GDAL machinery */
		hSrcDS = GDALOpenEx(GDLL->fname_in, GDAL_OF_VECTOR | GDAL_OF_VERBOSE_ERROR, NULL, NULL, NULL);
	else
		hSrcDS = gdal_vector (GMT, GDLL->fname_in);

	if (hSrcDS == NULL) {
		GMT_Report (GMT->parent, GMT_MSG_NORMAL, "GDALOpen failed %s\n", CPLGetLastErrorMsg());
		return -1;
	}

	if ((Grid = GMT_Create_Data (GMT->parent, GMT_IS_GRID, GMT_IS_SURFACE, GMT_CONTAINER_ONLY, NULL, NULL, NULL,
								 GMT_GRID_DEFAULT_REG, 0, NULL)) == NULL)
		return GMT->parent->error;

	if (GDLL->M.write_gdal && Grid->header->registration == 0) {
		/* Since GDAL writes only pixel-reg grids, expand limits so that inc is respected */
		dx = Grid->header->inc[0] / 2;		dy = Grid->header->inc[1] / 2;
	}
	sprintf(ext_opts, "-ot Float32 -txe %lf %lf -tye %lf %lf -outsize %d %d ",
			Grid->header->wesn[XLO]-dx, Grid->header->wesn[XHI]+dx, Grid->header->wesn[YLO]-dy,
			Grid->header->wesn[YHI]+dy, Grid->header->n_columns, Grid->header->n_rows);

	add_defaults(GMT, GDLL, &ext_opts[0]);

	psOptions = GDALGridOptionsNew(breakMe(GMT, ext_opts), NULL);
	hDstDS = GDALGrid(out_name(GDLL), hSrcDS, psOptions, &bUsageError);

	if (bUsageError == TRUE) {
		GMT_Report (GMT->parent, GMT_MSG_ERROR, "gdal_grid: failure\n");
		error++;
	}

	if (!error && !GDLL->M.write_gdal) 		/* Write grid with the GMT machinery */
		error = save_grid_with_GMT(GMT, hDstDS, Grid, GDLL->fname_out);

	GDALClose(hSrcDS);
	GDALGridOptionsFree(psOptions);
	GDALDestroyDriverManager();
	return error;
}

/* ------------------------------------------------------------------------------------------------------------ */
int gmt_gdal_dem (struct GMT_CTRL *GMT, struct GMT_GDALLIBRARIFIED_CTRL *GDLL) {
	char ext_opts[GMT_LEN512] = {""};
	int   bUsageError, error = 0;
	struct GMT_GRID *Grid = NULL;
	GDALDatasetH	hSrcDS, hDstDS;
	GDALDEMProcessingOptions *psOptions;

	GDALAllRegister();

	hSrcDS = GDALOpen(GDLL->fname_in, GA_ReadOnly);

	if (hSrcDS == NULL) {
		GMT_Report (GMT->parent, GMT_MSG_ERROR, "GDALOpen failed %s\n", CPLGetLastErrorMsg());
		return -1;
	}

	if ((Grid = GMT_Create_Data (GMT->parent, GMT_IS_GRID, GMT_IS_SURFACE, GMT_CONTAINER_ONLY, NULL, NULL, NULL,
								 GMT_GRID_DEFAULT_REG, 0, NULL)) == NULL)
		return GMT->parent->error;

	add_defaults(GMT, GDLL, &ext_opts[0]);

	psOptions = GDALDEMProcessingOptionsNew(breakMe(GMT, ext_opts), NULL);
	hDstDS = GDALDEMProcessing(out_name(GDLL), hSrcDS, "hillshade", NULL, psOptions, &bUsageError);

	if (bUsageError == TRUE) {
		GMT_Report (GMT->parent, GMT_MSG_ERROR, "gdal_dem: failure\n");
		error++;
	}

	if (!error && !GDLL->M.write_gdal) 		/* Write grid with the GMT machinery */
		error = save_grid_with_GMT(GMT, hDstDS, Grid, GDLL->fname_out);

	GDALClose(hSrcDS);
	GDALDEMProcessingOptionsFree(psOptions);
	GDALDestroyDriverManager();
	return error;
}

/* ------------------------------------------------------------------------------------------------------------ */
GMT_LOCAL GDALDatasetH gdal_vector (struct GMT_CTRL *GMT, char *fname) {
	/* Write data into a GDAL Vector memory dataset */
	unsigned int nt, ns, nr;
	double x, y, z;
	GDALDriverH hDriver;
	GDALDatasetH hDS;
	OGRLayerH hLayer;
	OGRFieldDefnH hFieldDefn;
	OGRFeatureH hFeature;
	OGRGeometryH hPt;
	struct GMT_DATASET *D = NULL;

	D = GMT_Read_Data (GMT->parent, GMT_IS_DATASET, GMT_IS_FILE, GMT_IS_PLP, GMT_READ_NORMAL, NULL, fname, NULL);
	if (D->n_columns != 3) {
		GMT_Report (GMT->parent, GMT_MSG_ERROR, "This dataset doesn't have 3 columns as required.\n");
		return NULL;
	}

	GDALAllRegister();

	hDriver = GDALGetDriverByName("Memory");			/* Intrmediary MEM diver to use as arg to GDALCreateCopy method */

	hDS = GDALCreate(hDriver, "mem", 0, 0, 0, GDT_Unknown, NULL);
	if (hDS == NULL) {
		GMT_Report (GMT->parent, GMT_MSG_ERROR, "Creation of MEM file failed - %d\n%s\n", CPLGetLastErrorNo(), CPLGetLastErrorMsg());
		GDALDestroyDriverManager();
		return NULL;
	}

	hLayer = GDALDatasetCreateLayer(hDS, "point_out", NULL, wkbPoint, NULL);
	if (hLayer == NULL ) {
		GMT_Report (GMT->parent, GMT_MSG_ERROR, "Layer creation failed.\n");
		GDALDestroyDriverManager();
		return NULL;
	}

	hFieldDefn = OGR_Fld_Create("Name", OFTString);

	OGR_Fld_SetWidth(hFieldDefn, 32);

	if (OGR_L_CreateField(hLayer, hFieldDefn, TRUE) != OGRERR_NONE) {
		GMT_Report (GMT->parent, GMT_MSG_ERROR, "Creating Name field failed.\n");
		GDALDestroyDriverManager();
		return NULL;
	}

	OGR_Fld_Destroy(hFieldDefn);

	for (nt = 0; nt < D->n_tables; nt++) {
		for (ns = 0; ns < D->table[nt]->n_segments; ns++) {
			for (nr = 0; nr < D->table[nt]->segment[ns]->n_rows; nr++) {
				x = D->table[nt]->segment[ns]->data[0][nr];
				y = D->table[nt]->segment[ns]->data[1][nr];
				z = D->table[nt]->segment[ns]->data[2][nr];
				hFeature = OGR_F_Create(OGR_L_GetLayerDefn(hLayer));
				OGR_F_SetFieldString(hFeature, OGR_F_GetFieldIndex(hFeature, "Name"), "0");

				hPt = OGR_G_CreateGeometry(wkbPoint);
				OGR_G_SetPoint(hPt, 0, x, y, z);

				OGR_F_SetGeometry(hFeature, hPt);
				OGR_G_DestroyGeometry(hPt);

				if (OGR_L_CreateFeature(hLayer, hFeature) != OGRERR_NONE) {
					GMT_Report (GMT->parent, GMT_MSG_ERROR, "Failed to create feature in dataset.\n");
					GDALDestroyDriverManager();
					return NULL;
				}
				OGR_F_Destroy(hFeature);
			}
		}
	}

	return hDS;
}

#endif		//defined(HAVE_GDAL) && ...