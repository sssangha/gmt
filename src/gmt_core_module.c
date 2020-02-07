/*
 * Copyright (c) 2012-2020 by the GMT Team (https://www.generic-mapping-tools.org/team.html)
 * See LICENSE.TXT file for copying and redistribution conditions.
 */

/* gmt_core_module.c populates the external array of GMT core with
 * module parameters such as name, group, purpose and keys strings.
 * This file also contains the following convenience functions to
 * display all module purposes or just list their names:
 *
 *   void gmt_core_module_show_all (struct GMTAPI_CTRL *API);
 *   void gmt_core_module_list_all (void *API);
 *
 * These functions may be called by gmt --help and gmt --show-modules
 *
 * Developers of external APIs for accessing GMT modules will use this
 * function indirectly via GMT_Encode_Options to retrieve option keys
 * needed for module arg processing:
 *
 *   char * gmt_core_module_keys (void *API, const char *module);
 *
 * DO NOT edit this file directly! Regenerate the file by running
 * 	gmt_make_module_src.sh core
 */
#include "gmt_dev.h"
#ifndef BUILD_SHARED_LIBS
#include "gmt_core_module.h"
#endif


/* Sorted array with information for all GMT core modules */

/* name, library, and purpose for each module */
struct Gmt_moduleinfo {
	const char *mname;            /* Program (modern) name */
	const char *cname;            /* Program (classic) name */
	const char *component;        /* Component (core, supplement, custom) */
	const char *purpose;          /* Program purpose */
	const char *keys;             /* Program option info for external APIs */
#ifndef BUILD_SHARED_LIBS
	/* gmt module function pointer: */
	int (*p_func)(void*, int, void*);
#endif
};

static struct Gmt_moduleinfo g_core_module[] = {
#ifdef BUILD_SHARED_LIBS
	{"basemap", "psbasemap", "core", "Plot base maps and frames", ">X},>DA@AD)"},
	{"begin", "begin", "core", "Initiate a new GMT modern mode session", ""},
	{"blockmean", "blockmean", "core", "Block average (x,y,z) data tables by mean estimation", "<D{,>D},GG),A->"},
	{"blockmedian", "blockmedian", "core", "Block average (x,y,z) data tables by median estimation", "<D{,>D},GG),A->"},
	{"blockmode", "blockmode", "core", "Block average (x,y,z) data tables by mode estimation", "<D{,>D},GG),A->"},
	{"clear", "clear", "core", "Delete current default settings, or the cache, data or sessions directories", ""},
	{"clip", "psclip", "core", "Initialize or terminate polygonal clip paths", "<D{,>X},C-("},
	{"coast", "pscoast", "core", "Plot continents, countries, shorelines, rivers, and borders", ">?}"},
	{"colorbar", "psscale", "core", "Plot gray scale or color scale bar", "CC{,>X},ZD("},
	{"contour", "pscontour", "core", "Contour table data by direct triangulation", "<D{,AD)=t,CC(,ED(,DDD,G?(=1,>X}@<D{,AD)=t,CC(,ED(,DD),G?(=1"},
	{"dimfilter", "dimfilter", "core", "Directional filtering of grids in the space domain", "<G{,GG},>DQ"},
	{"docs", "docs", "core", "Show HTML documentation of specified module", ""},
	{"end", "end", "core", "Terminate GMT modern mode session and produce optional graphics", ""},
	{"events", "psevents", "core", "Plot event symbols and labels for a moment in time", "<D{,CC(,>X}"},
	{"figure", "figure", "core", "Set attributes for the current modern mode session figure", ""},
	{"filter1d", "filter1d", "core", "Time domain filtering of 1-D data tables", "<D{,>D},FD(1"},
	{"fitcircle", "fitcircle", "core", "Find mean position and great [or small] circle fit to points on sphere", "<D{,>D},>DF"},
	{"gmt2kml", "gmt2kml", "core", "Convert GMT data table to Google Earth KML file", "<D{,>D},CC("},
	{"gmtconnect", "gmtconnect", "core", "Connect individual lines whose end points match within tolerance", "<D{,>D},CD),LD),QD)"},
	{"gmtconvert", "gmtconvert", "core", "Convert, paste, or extract columns from data tables", "<D{,>D}"},
	{"gmtdefaults", "gmtdefaults", "core", "List current GMT default settings", ""},
	{"gmtgdal", "gmtgdal", "core", "Execute GDAL programs from GMT", "<D{,GG}"},
	{"gmtget", "gmtget", "core", "Get individual GMT default settings", ">D}"},
	{"gmtinfo", "gmtinfo", "core", "Get information about data tables", "<D{,>D}"},
	{"gmtlogo", "gmtlogo", "core", "Plot the GMT logo", ">X}"},
	{"gmtmath", "gmtmath", "core", "Reverse Polish Notation (RPN) calculator for data tables", "<D(,AD(=,TD(,>D}"},
	{"gmtread", "gmtread", "core", "Read GMT objects into external API", "-T-,<?{,>?}"},
	{"gmtregress", "gmtregress", "core", "Linear regression of 1-D data sets", "<D{,>D}"},
	{"gmtselect", "gmtselect", "core", "Select data table subsets based on multiple spatial criteria", "<D{,CD(=,FD(,LD(=,>D},GG("},
	{"gmtset", "gmtset", "core", "Change individual GMT default settings", ""},
	{"gmtsimplify", "gmtsimplify", "core", "Line reduction using the Douglas-Peucker algorithm", "<D{,>D}"},
	{"gmtspatial", "gmtspatial", "core", "Geospatial operations on points, lines and polygons", "<D{,DD(=f,ND(=,TD(,>D}"},
	{"gmtvector", "gmtvector", "core", "Operations on Cartesian vectors in 2-D and 3-D", "<D{,AD(,>D}"},
	{"gmtwhich", "gmtwhich", "core", "Find full path to specified files", ">D}"},
	{"gmtwrite", "gmtwrite", "core", "Write GMT objects from external API", "-T-,<?{,>?}"},
	{"grd2cpt", "grd2cpt", "core", "Make linear or histogram-equalized color palette table from grid", "<G{+,>C}"},
	{"grd2kml", "grd2kml", "core", "Create KML image quadtree from single grid", "<G{,CC(,IG(,WD("},
	{"grd2xyz", "grd2xyz", "core", "Convert grid to data table", "<G{+,>D}"},
	{"grdblend", "grdblend", "core", "Blend several partially overlapping grids into one larger grid", "<G{+,GG}"},
	{"grdclip", "grdclip", "core", "Clip the range of grid values", "<G{,GG}"},
	{"grdcontour", "grdcontour", "core", "Make contour map using a grid", "<G{,AD)=t,CC(,DDD,>X},G?(=1@<G{,AD)=t,CC(,DD),G?(=1"},
	{"grdconvert", "grdconvert", "core", "Convert between different grid formats", "<G{,>G}"},
	{"grdcut", "grdcut", "core", "Extract subregion from a grid", "<G{,GG}"},
	{"grdedit", "grdedit", "core", "Modify header or content of a grid", "<G{,ND(,GG}"},
	{"grdfft", "grdfft", "core", "Mathematical operations on grids in the spectral domain", "<G{+,GG},GDE"},
	{"grdfill", "grdfill", "core", "Interpolate across holes in a grid", "<G{,>G}"},
	{"grdfilter", "grdfilter", "core", "Filter a grid in the space (or time) domain", "<G{,FG(=1,GG}"},
	{"grdgradient", "grdgradient", "core", "Compute directional gradients from a grid", "<G{,AG(,GG},SG)"},
	{"grdhisteq", "grdhisteq", "core", "Perform histogram equalization for a grid", "<G{,GG},DD)"},
	{"grdimage", "grdimage", "core", "Project and plot grids or images", "<G{+,CC(,IG(,>X},>IA,<ID@<G{+,CC(,IG(,AI),<ID"},
	{"grdinfo", "grdinfo", "core", "Extract information from grids", "<G{+,>D}"},
	{"grdinterpolate", "grdinterpolate", "core", "Interpolate new layers from a 3-D netCDF data cube", "<G{+,GG}"},
	{"grdlandmask", "grdlandmask", "core", "Create a \"wet-dry\" mask grid from shoreline data base", "GG}"},
	{"grdmask", "grdmask", "core", "Create mask grid from polygons or point coverage", "<D{,GG}"},
	{"grdmath", "grdmath", "core", "Reverse Polish Notation (RPN) calculator for grids (element by element)", "<G(,=G}"},
	{"grdpaste", "grdpaste", "core", "Join two grids along their common edge", "<G{2,GG}"},
	{"grdproject", "grdproject", "core", "Forward and inverse map transformation of grids", "<G{,GG}"},
	{"grdsample", "grdsample", "core", "Resample a grid onto a new lattice", "<G{,GG}"},
	{"grdtrack", "grdtrack", "core", "Sample grids at specified (x,y) locations", "<D{,DD),E-<,GG(,>D},SD)=s"},
	{"grdtrend", "grdtrend", "core", "Fit trend surface to grids and compute residuals", "<G{,DG),TG),WG(,WG)"},
	{"grdvector", "grdvector", "core", "Plot vector field from two component grids", "<G{2,CC(,>X}"},
	{"grdview", "grdview", "core", "Create 3-D perspective image or surface mesh from a grid", "<G{,CC(,GG(,IG(,>X}"},
	{"grdvolume", "grdvolume", "core", "Calculate grid volume and area constrained by a contour", "<G{,>D}"},
	{"greenspline", "greenspline", "core", "Interpolate using Green's functions for splines in 1-3 dimensions", "<D{,AD(=,ED),ND(,TG(,CD)=f,G?},GDN"},
	{"histogram", "pshistogram", "core", "Calculate and plot histograms", "<D{,CC(,>X},>D),>DI@<D{,ID)"},
	{"image", "psimage", "core", "Plot raster or EPS images", "<I{,>X}"},
	{"inset", "inset", "core", "Manage figure inset setup and completion", ">X}"},
	{"kml2gmt", "kml2gmt", "core", "Extract GMT table data from Google Earth KML files", ">D}"},
	{"legend", "pslegend", "core", "Plot a legend", "<D{,>X}"},
	{"makecpt", "makecpt", "core", "Make GMT color palette tables", ">C},ED(,SD(,TD(,<D("},
	{"mapproject", "mapproject", "core", "Forward and inverse map transformations, datum conversions and geodesy", "<D{,LD(=,>D},W-("},
	{"mask", "psmask", "core", "Clip or mask map areas with no data table coverage", "<D{,DDD,C-(,>X},LG)@<D{,DD),C-(,LG)"},
	{"movie", "movie", "core", "Create animation sequences and movies", "<D("},
	{"nearneighbor", "nearneighbor", "core", "Grid table data using a \"Nearest neighbor\" algorithm", "<D{,GG}"},
	{"plot", "psxy", "core", "Plot lines, polygons, and symbols in 2-D", "<D{,CC(,T-<,>X},S?(=2"},
	{"plot3d", "psxyz", "core", "Plot lines, polygons, and symbols in 3-D", "<D{,CC(,T-<,>X},S?(=2"},
	{"project", "project", "core", "Project data onto lines or great circles, or generate tracks", "<D{,>D},G-("},
	{"psconvert", "psconvert", "core", "Convert [E]PS file(s) to other formats using Ghostscript", "<X{+,FI)"},
	{"rose", "psrose", "core", "Plot a polar histogram (rose, sector, windrose diagrams)", "<D{,CC(,ED(,>X},>D),>DI@<D{,ID),CC("},
	{"sample1d", "sample1d", "core", "Resample 1-D table data using splines", "<D{,ND(,>D}"},
	{"solar", "pssolar", "core", "Plot day-light terminators and other sunlight parameters", ">X},>DI,>DM@ID),MD)"},
	{"spectrum1d", "spectrum1d", "core", "Compute auto- [and cross-] spectra from one [or two] time series", "<D{,>D},T-)"},
	{"sph2grd", "sph2grd", "core", "Compute grid from spherical harmonic coefficients", "<D{,GG}"},
	{"sphdistance", "sphdistance", "core", "Create Voronoi distance, node, or natural nearest-neighbor grid on a sphere", "<D{,ND(,QD(,GG},Q-("},
	{"sphinterpolate", "sphinterpolate", "core", "Spherical gridding in tension of data on a sphere", "<D{,GG}"},
	{"sphtriangulate", "sphtriangulate", "core", "Delaunay or Voronoi construction of spherical data", "<D{,>D},ND)"},
	{"splitxyz", "splitxyz", "core", "Split xyz[dh] data tables into individual segments", "<D{,>D}"},
	{"subplot", "subplot", "core", "Manage modern mode figure subplot configuration and selection", ""},
	{"surface", "surface", "core", "Grid table data using adjustable tension continuous curvature splines", "<D{,DD(,LG(,GG}"},
	{"ternary", "psternary", "core", "Plot data on ternary diagrams", "<D{,>X},>DM,C-(@<D{,MD),C-("},
	{"text", "pstext", "core", "Plot or typeset text", "<D{,>X}"},
	{"trend1d", "trend1d", "core", "Fit [weighted] [robust] polynomial/Fourier model for y = f(x) to xy[w] data", "<D{,>D}"},
	{"trend2d", "trend2d", "core", "Fit [weighted] [robust] polynomial for z = f(x,y) to xyz[w] data", "<D{,>D}"},
	{"triangulate", "triangulate", "core", "Delaunay triangulation or Voronoi partitioning and gridding of Cartesian data", "<D{,CG(,>D},GG)"},
	{"wiggle", "pswiggle", "core", "Plot z = f(x,y) anomalies along tracks", "<D{,>X}"},
	{"xyz2grd", "xyz2grd", "core", "Convert data table to a grid", "<D{,SD),GG}"},
	{NULL, NULL, NULL, NULL, NULL} /* last element == NULL detects end of array */
#else
	{"basemap", "psbasemap", "core", "Plot base maps and frames", ">X},>DA@AD)", &GMT_basemap},
	{"begin", "begin", "core", "Initiate a new GMT modern mode session", "", &GMT_begin},
	{"blockmean", "blockmean", "core", "Block average (x,y,z) data tables by mean estimation", "<D{,>D},GG),A->", &GMT_blockmean},
	{"blockmedian", "blockmedian", "core", "Block average (x,y,z) data tables by median estimation", "<D{,>D},GG),A->", &GMT_blockmedian},
	{"blockmode", "blockmode", "core", "Block average (x,y,z) data tables by mode estimation", "<D{,>D},GG),A->", &GMT_blockmode},
	{"clear", "clear", "core", "Delete current default settings, or the cache, data or sessions directories", "", &GMT_clear},
	{"clip", "psclip", "core", "Initialize or terminate polygonal clip paths", "<D{,>X},C-(", &GMT_clip},
	{"coast", "pscoast", "core", "Plot continents, countries, shorelines, rivers, and borders", ">?}", &GMT_coast},
	{"colorbar", "psscale", "core", "Plot gray scale or color scale bar", "CC{,>X},ZD(", &GMT_colorbar},
	{"contour", "pscontour", "core", "Contour table data by direct triangulation", "<D{,AD)=t,CC(,ED(,DDD,G?(=1,>X}@<D{,AD)=t,CC(,ED(,DD),G?(=1", &GMT_contour},
	{"dimfilter", "dimfilter", "core", "Directional filtering of grids in the space domain", "<G{,GG},>DQ", &GMT_dimfilter},
	{"docs", "docs", "core", "Show HTML documentation of specified module", "", &GMT_docs},
	{"end", "end", "core", "Terminate GMT modern mode session and produce optional graphics", "", &GMT_end},
	{"events", "psevents", "core", "Plot event symbols and labels for a moment in time", "<D{,CC(,>X}", &GMT_events},
	{"figure", "figure", "core", "Set attributes for the current modern mode session figure", "", &GMT_figure},
	{"filter1d", "filter1d", "core", "Time domain filtering of 1-D data tables", "<D{,>D},FD(1", &GMT_filter1d},
	{"fitcircle", "fitcircle", "core", "Find mean position and great [or small] circle fit to points on sphere", "<D{,>D},>DF", &GMT_fitcircle},
	{"gmt2kml", "gmt2kml", "core", "Convert GMT data table to Google Earth KML file", "<D{,>D},CC(", &GMT_gmt2kml},
	{"gmtconnect", "gmtconnect", "core", "Connect individual lines whose end points match within tolerance", "<D{,>D},CD),LD),QD)", &GMT_gmtconnect},
	{"gmtconvert", "gmtconvert", "core", "Convert, paste, or extract columns from data tables", "<D{,>D}", &GMT_gmtconvert},
	{"gmtdefaults", "gmtdefaults", "core", "List current GMT default settings", "", &GMT_gmtdefaults},
	{"gmtgdal", "gmtgdal", "core", "Execute GDAL programs from GMT", "", &GMT_gmtgdal},
	{"gmtget", "gmtget", "core", "Get individual GMT default settings", ">D}", &GMT_gmtget},
	{"gmtinfo", "gmtinfo", "core", "Get information about data tables", "<D{,>D}", &GMT_gmtinfo},
	{"gmtlogo", "gmtlogo", "core", "Plot the GMT logo", ">X}", &GMT_gmtlogo},
	{"gmtmath", "gmtmath", "core", "Reverse Polish Notation (RPN) calculator for data tables", "<D(,AD(=,TD(,>D}", &GMT_gmtmath},
	{"gmtread", "gmtread", "core", "Read GMT objects into external API", "-T-,<?{,>?}", &GMT_gmtread},
	{"gmtregress", "gmtregress", "core", "Linear regression of 1-D data sets", "<D{,>D}", &GMT_gmtregress},
	{"gmtselect", "gmtselect", "core", "Select data table subsets based on multiple spatial criteria", "<D{,CD(=,FD(,LD(=,>D},GG(", &GMT_gmtselect},
	{"gmtset", "gmtset", "core", "Change individual GMT default settings", "", &GMT_gmtset},
	{"gmtsimplify", "gmtsimplify", "core", "Line reduction using the Douglas-Peucker algorithm", "<D{,>D}", &GMT_gmtsimplify},
	{"gmtspatial", "gmtspatial", "core", "Geospatial operations on points, lines and polygons", "<D{,DD(=f,ND(=,TD(,>D}", &GMT_gmtspatial},
	{"gmtvector", "gmtvector", "core", "Operations on Cartesian vectors in 2-D and 3-D", "<D{,AD(,>D}", &GMT_gmtvector},
	{"gmtwhich", "gmtwhich", "core", "Find full path to specified files", ">D}", &GMT_gmtwhich},
	{"gmtwrite", "gmtwrite", "core", "Write GMT objects from external API", "-T-,<?{,>?}", &GMT_gmtwrite},
	{"grd2cpt", "grd2cpt", "core", "Make linear or histogram-equalized color palette table from grid", "<G{+,>C}", &GMT_grd2cpt},
	{"grd2kml", "grd2kml", "core", "Create KML image quadtree from single grid", "<G{,CC(,IG(,WD(", &GMT_grd2kml},
	{"grd2xyz", "grd2xyz", "core", "Convert grid to data table", "<G{+,>D}", &GMT_grd2xyz},
	{"grdblend", "grdblend", "core", "Blend several partially overlapping grids into one larger grid", "<G{+,GG}", &GMT_grdblend},
	{"grdclip", "grdclip", "core", "Clip the range of grid values", "<G{,GG}", &GMT_grdclip},
	{"grdcontour", "grdcontour", "core", "Make contour map using a grid", "<G{,AD)=t,CC(,DDD,>X},G?(=1@<G{,AD)=t,CC(,DD),G?(=1", &GMT_grdcontour},
	{"grdconvert", "grdconvert", "core", "Convert between different grid formats", "<G{,>G}", &GMT_grdconvert},
	{"grdcut", "grdcut", "core", "Extract subregion from a grid", "<G{,GG}", &GMT_grdcut},
	{"grdedit", "grdedit", "core", "Modify header or content of a grid", "<G{,ND(,GG}", &GMT_grdedit},
	{"grdfft", "grdfft", "core", "Mathematical operations on grids in the spectral domain", "<G{+,GG},GDE", &GMT_grdfft},
	{"grdfill", "grdfill", "core", "Interpolate across holes in a grid", "<G{,>G}", &GMT_grdfill},
	{"grdfilter", "grdfilter", "core", "Filter a grid in the space (or time) domain", "<G{,FG(=1,GG}", &GMT_grdfilter},
	{"grdgradient", "grdgradient", "core", "Compute directional gradients from a grid", "<G{,AG(,GG},SG)", &GMT_grdgradient},
	{"grdhisteq", "grdhisteq", "core", "Perform histogram equalization for a grid", "<G{,GG},DD)", &GMT_grdhisteq},
	{"grdimage", "grdimage", "core", "Project and plot grids or images", "<G{+,CC(,IG(,>X},>IA,<ID@<G{+,CC(,IG(,AI),<ID", &GMT_grdimage},
	{"grdinfo", "grdinfo", "core", "Extract information from grids", "<G{+,>D}", &GMT_grdinfo},
	{"grdinterpolate", "grdinterpolate", "core", "Interpolate new layers from a 3-D netCDF data cube", "<G{+,GG}", &GMT_grdinterpolate},
	{"grdlandmask", "grdlandmask", "core", "Create a \"wet-dry\" mask grid from shoreline data base", "GG}", &GMT_grdlandmask},
	{"grdmask", "grdmask", "core", "Create mask grid from polygons or point coverage", "<D{,GG}", &GMT_grdmask},
	{"grdmath", "grdmath", "core", "Reverse Polish Notation (RPN) calculator for grids (element by element)", "<G(,=G}", &GMT_grdmath},
	{"grdpaste", "grdpaste", "core", "Join two grids along their common edge", "<G{2,GG}", &GMT_grdpaste},
	{"grdproject", "grdproject", "core", "Forward and inverse map transformation of grids", "<G{,GG}", &GMT_grdproject},
	{"grdsample", "grdsample", "core", "Resample a grid onto a new lattice", "<G{,GG}", &GMT_grdsample},
	{"grdtrack", "grdtrack", "core", "Sample grids at specified (x,y) locations", "<D{,DD),E-<,GG(,>D},SD)=s", &GMT_grdtrack},
	{"grdtrend", "grdtrend", "core", "Fit trend surface to grids and compute residuals", "<G{,DG),TG),WG(,WG)", &GMT_grdtrend},
	{"grdvector", "grdvector", "core", "Plot vector field from two component grids", "<G{2,CC(,>X}", &GMT_grdvector},
	{"grdview", "grdview", "core", "Create 3-D perspective image or surface mesh from a grid", "<G{,CC(,GG(,IG(,>X}", &GMT_grdview},
	{"grdvolume", "grdvolume", "core", "Calculate grid volume and area constrained by a contour", "<G{,>D}", &GMT_grdvolume},
	{"greenspline", "greenspline", "core", "Interpolate using Green's functions for splines in 1-3 dimensions", "<D{,AD(=,ED),ND(,TG(,CD)=f,G?},GDN", &GMT_greenspline},
	{"histogram", "pshistogram", "core", "Calculate and plot histograms", "<D{,CC(,>X},>D),>DI@<D{,ID)", &GMT_histogram},
	{"image", "psimage", "core", "Plot raster or EPS images", "<I{,>X}", &GMT_image},
	{"inset", "inset", "core", "Manage figure inset setup and completion", ">X}", &GMT_inset},
	{"kml2gmt", "kml2gmt", "core", "Extract GMT table data from Google Earth KML files", ">D}", &GMT_kml2gmt},
	{"legend", "pslegend", "core", "Plot a legend", "<D{,>X}", &GMT_legend},
	{"makecpt", "makecpt", "core", "Make GMT color palette tables", ">C},ED(,SD(,TD(,<D(", &GMT_makecpt},
	{"mapproject", "mapproject", "core", "Forward and inverse map transformations, datum conversions and geodesy", "<D{,LD(=,>D},W-(", &GMT_mapproject},
	{"mask", "psmask", "core", "Clip or mask map areas with no data table coverage", "<D{,DDD,C-(,>X},LG)@<D{,DD),C-(,LG)", &GMT_mask},
	{"movie", "movie", "core", "Create animation sequences and movies", "<D(", &GMT_movie},
	{"nearneighbor", "nearneighbor", "core", "Grid table data using a \"Nearest neighbor\" algorithm", "<D{,GG}", &GMT_nearneighbor},
	{"plot", "psxy", "core", "Plot lines, polygons, and symbols in 2-D", "<D{,CC(,T-<,>X},S?(=2", &GMT_plot},
	{"plot3d", "psxyz", "core", "Plot lines, polygons, and symbols in 3-D", "<D{,CC(,T-<,>X},S?(=2", &GMT_plot3d},
	{"project", "project", "core", "Project data onto lines or great circles, or generate tracks", "<D{,>D},G-(", &GMT_project},
	{"psconvert", "psconvert", "core", "Convert [E]PS file(s) to other formats using Ghostscript", "<X{+,FI)", &GMT_psconvert},
	{"rose", "psrose", "core", "Plot a polar histogram (rose, sector, windrose diagrams)", "<D{,CC(,ED(,>X},>D),>DI@<D{,ID),CC(", &GMT_rose},
	{"sample1d", "sample1d", "core", "Resample 1-D table data using splines", "<D{,ND(,>D}", &GMT_sample1d},
	{"solar", "pssolar", "core", "Plot day-light terminators and other sunlight parameters", ">X},>DI,>DM@ID),MD)", &GMT_solar},
	{"spectrum1d", "spectrum1d", "core", "Compute auto- [and cross-] spectra from one [or two] time series", "<D{,>D},T-)", &GMT_spectrum1d},
	{"sph2grd", "sph2grd", "core", "Compute grid from spherical harmonic coefficients", "<D{,GG}", &GMT_sph2grd},
	{"sphdistance", "sphdistance", "core", "Create Voronoi distance, node, or natural nearest-neighbor grid on a sphere", "<D{,ND(,QD(,GG},Q-(", &GMT_sphdistance},
	{"sphinterpolate", "sphinterpolate", "core", "Spherical gridding in tension of data on a sphere", "<D{,GG}", &GMT_sphinterpolate},
	{"sphtriangulate", "sphtriangulate", "core", "Delaunay or Voronoi construction of spherical data", "<D{,>D},ND)", &GMT_sphtriangulate},
	{"splitxyz", "splitxyz", "core", "Split xyz[dh] data tables into individual segments", "<D{,>D}", &GMT_splitxyz},
	{"subplot", "subplot", "core", "Manage modern mode figure subplot configuration and selection", "", &GMT_subplot},
	{"surface", "surface", "core", "Grid table data using adjustable tension continuous curvature splines", "<D{,DD(,LG(,GG}", &GMT_surface},
	{"ternary", "psternary", "core", "Plot data on ternary diagrams", "<D{,>X},>DM,C-(@<D{,MD),C-(", &GMT_ternary},
	{"text", "pstext", "core", "Plot or typeset text", "<D{,>X}", &GMT_text},
	{"trend1d", "trend1d", "core", "Fit [weighted] [robust] polynomial/Fourier model for y = f(x) to xy[w] data", "<D{,>D}", &GMT_trend1d},
	{"trend2d", "trend2d", "core", "Fit [weighted] [robust] polynomial for z = f(x,y) to xyz[w] data", "<D{,>D}", &GMT_trend2d},
	{"triangulate", "triangulate", "core", "Delaunay triangulation or Voronoi partitioning and gridding of Cartesian data", "<D{,CG(,>D},GG)", &GMT_triangulate},
	{"wiggle", "pswiggle", "core", "Plot z = f(x,y) anomalies along tracks", "<D{,>X}", &GMT_wiggle},
	{"xyz2grd", "xyz2grd", "core", "Convert data table to a grid", "<D{,SD),GG}", &GMT_xyz2grd},
	{NULL, NULL, NULL, NULL, NULL, NULL} /* last element == NULL detects end of array */
#endif
};

/* Function to exclude some special core modules from being reported by gmt --help|show-modules */
GMT_LOCAL int skip_this_module (const char *name) {
	if (!strncmp (name, "gmtread", 7U)) return 1;	/* Skip the gmtread module */
	if (!strncmp (name, "gmtwrite", 8U)) return 1;	/* Skip the gmtwrite module */
	return 0;	/* Display this one */
}

/* Function to exclude modern mode modules from being reported by gmt --show-classic */
GMT_LOCAL int skip_modern_module (const char *name) {
	if (!strncmp (name, "subplot", 7U)) return 1;	/* Skip the subplot module */
	if (!strncmp (name, "figure", 6U)) return 1;	/* Skip the figure module */
	if (!strncmp (name, "begin", 5U)) return 1;		/* Skip the begin module */
	if (!strncmp (name, "clear", 5U)) return 1;		/* Skip the clear module */
	if (!strncmp (name, "inset", 5U)) return 1;		/* Skip the inset module */
	if (!strncmp (name, "movie", 5U)) return 1;		/* Skip the movie module */
	if (!strncmp (name, "docs", 4U)) return 1;		/* Skip the docs module */
	if (!strncmp (name, "end", 3U)) return 1;		/* Skip the end module */
	return 0;	/* Display this one */
}

/* Pretty print all GMT core module names and their purposes for gmt --help */
void gmt_core_module_show_all (void *V_API) {
	unsigned int module_id = 0;
	char message[GMT_LEN256];
	struct GMTAPI_CTRL *API = gmt_get_api_ptr (V_API);
	GMT_Message (V_API, GMT_TIME_NONE, "\n===  GMT core: The main modules of the Generic Mapping Tools  ===\n");
	while (g_core_module[module_id].cname != NULL) {
		if (module_id == 0 || strcmp (g_core_module[module_id-1].component, g_core_module[module_id].component)) {
			/* Start of new supplemental group */
			snprintf (message, GMT_LEN256, "\nModule name:     Purpose of %s module:\n", g_core_module[module_id].component);
			GMT_Message (V_API, GMT_TIME_NONE, message);
			GMT_Message (V_API, GMT_TIME_NONE, "----------------------------------------------------------------\n");
		}
		if (API->external || !skip_this_module (g_core_module[module_id].cname)) {
			snprintf (message, GMT_LEN256, "%-16s %s\n",
				g_core_module[module_id].mname, g_core_module[module_id].purpose);
				GMT_Message (V_API, GMT_TIME_NONE, message);
		}
		++module_id;
	}
}

/* Produce single list on stdout of all GMT core module names for gmt --show-modules */
void gmt_core_module_list_all (void *V_API) {
	unsigned int module_id = 0;
	struct GMTAPI_CTRL *API = gmt_get_api_ptr (V_API);
	while (g_core_module[module_id].cname != NULL) {
		if (API->external || !skip_this_module (g_core_module[module_id].cname))
			printf ("%s\n", g_core_module[module_id].mname);
		++module_id;
	}
}

/* Produce single list on stdout of all GMT core module names for gmt --show-classic [i.e., classic mode names] */
void gmt_core_module_classic_all (void *V_API) {
	unsigned int module_id = 0;
	struct GMTAPI_CTRL *API = gmt_get_api_ptr (V_API);
	while (g_core_module[module_id].cname != NULL) {
		if (API->external || !(skip_this_module (g_core_module[module_id].cname) || skip_modern_module (g_core_module[module_id].cname)))
			printf ("%s\n", g_core_module[module_id].cname);
		++module_id;
	}
}

/* Lookup module id by name, return option keys pointer (for external API developers) */
const char *gmt_core_module_keys (void *API, char *candidate) {
	int module_id = 0;
	gmt_M_unused(API);

	/* Match actual_name against g_module[module_id].cname */
	while (g_core_module[module_id].cname != NULL &&
	       strcmp (candidate, g_core_module[module_id].cname))
		++module_id;

	/* Return Module keys or NULL */
	return (g_core_module[module_id].keys);
}

/* Lookup module id by name, return group char name (for external API developers) */
const char *gmt_core_module_group (void *API, char *candidate) {
	int module_id = 0;
	gmt_M_unused(API);

	/* Match actual_name against g_module[module_id].cname */
	while (g_core_module[module_id].cname != NULL &&
	       strcmp (candidate, g_core_module[module_id].cname))
		++module_id;

	/* Return Module keys or NULL */
	return (g_core_module[module_id].component);
}

#ifndef BUILD_SHARED_LIBS
/* Lookup static module id by name, return function pointer */
void *gmt_core_module_lookup (void *API, const char *candidate) {
	int module_id = 0;
	size_t len = strlen (candidate);
	gmt_M_unused(API);

	if (len < 4) return NULL;	/* All candidates should start with GMT_ */
	/* Match actual_name against g_module[module_id].cname */
	while (g_core_module[module_id].cname != NULL &&
	       strcmp (&candidate[4], g_core_module[module_id].cname))
		++module_id;

	/* Return Module function or NULL */
	return (g_core_module[module_id].p_func);
}
#endif
