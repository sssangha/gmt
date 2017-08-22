#!/bin/bash
export GMT_PPID=1
gmt begin ex14 ps
  gmt set MAP_GRID_PEN_PRIMARY thinnest,-
  # calculate mean data and grids
  gmt blockmean @table_5.11 -R0/7/0/7 -I1 > mean.xyz
  gmt surface mean.xyz -Gdata.nc
  gmt grdtrend data.nc -N10 -Ttrend.nc
  gmt project -C0/0 -E7/7 -G0.1 -N > track
  # Sample along diagonal
  gmt grdtrack track -Gdata.nc -o2,3 > data.d
  gmt grdtrack track -Gtrend.nc -o2,3 > trend.d
  gmt psxy -Ra -JX6i/1.4i data.d -Wthick -Bx1 -By50 -BWSne -P
  gmt psxy trend.d -Wthinner,-
  gmt subplot begin 2x2 -M0.05i -Ff6i/6i+d -LWSne -Y1.8i
    # First draw network and label the nodes
    gmt psxy @table_5.11 -R0/7/0/7 -Jx -Sc0.05i -Gblack -c1,1
    gmt pstext @table_5.11 -D0.1c/0 -F+f6p+jLM -N
    # Then draw gmt blockmean cells and label data values using one decimal
    gmt psxy mean.xyz -Ss0.05i -Gblack -c1,2
    gmt pstext -D0.15c/0 -F+f6p+jLM+z%.1f -Gwhite -W -C0.01i -N mean.xyz
    # Then gmt surface and contour the data
    gmt grdcontour data.nc -C25 -A50 -Gd3i -S4 -c2,1
    gmt psxy mean.xyz -Ss0.05i -Gblack
    # Fit bicubic trend to data and compare to gridded gmt surface
    gmt grdcontour trend.nc -C25 -A50 -Glct/cb -S4 -c2,2
    gmt psxy track -Wthick,.
  gmt subplot end
  gmt psxy -Ra -JX6i/1.4i data.d -Wthick -Bx1 -By50 -BWSne -Y6.4i
 gmt psxy trend.d -Wthinner,-
gmt end
rm -f mean.xyz track trend.nc data.nc data.d trend.d gmt.conf
