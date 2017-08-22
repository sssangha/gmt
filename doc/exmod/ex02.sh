#!/bin/bash
#		GMT EXAMPLE 02
#		$Id$
#
# Purpose:	Make two color images based gridded data
# GMT modules:	gmtset, grd2cpt, grdimage, makecpt, psscale, pstext
# Unix progs:	rm
#
export GMT_PPID=1
gmt begin ex02 ps
 gmt set MAP_ANNOT_OBLIQUE 0 PS_MEDIA letter
 gmt makecpt -Crainbow -T-2/14/2 > g.cpt
 gmt grd2cpt @HI_topo_02.nc -Crelief -Z > t.cpt
 gmt subplot begin 2x1 -A+JTL+o0.1i/0 -Mm1i -Mp0.2i -LWESN -T"H@#awaiian@# T@#opo and @#G@#eoid@#"
  gmt subplot 2,1 -Ce1.1i
  gmt grdimage @HI_geoid_02.nc -R160/20/220/30+r -JOc190/25.5/292/69 -B10 -E50 -Cg.cpt
  gmt psscale -Cg.cpt -DJRM+o0.6i/0+e+mc -Bx2+lGEOID -By+lm
  gmt subplot 1,1 -Ce1.15i
  gmt grdimage @HI_topo_02.nc -I+a0 -E50 -Ct.cpt -B
  gmt psscale -Ct.cpt -DJRM+o0.6i/0+mc -I0.3 -Bx2+lTOPO -By+lkm
 gmt subplot end
gmt end
