/*
 Old/Example005
 A small pavillion in cylinders using a for loop to place columns.
 */

// Customizer variables

/* [Hidden] */
exNum = 5; // example number
include <example-index.scad>

echo( announceExample(exNum) );
// Visualiztion controls set to good defaults
//  for smooth curved surfaces
$fs = 0.5;
$fa = 1.0;

module example005() {
    translate([0, 0, -120]) {
        difference() {
            cylinder(h = 50, r = 100);
            translate([0, 0, 10])
                cylinder(h = 50, r = 80);
            translate([100, 0, 35])
                cube(50, center = true);
            }
    for (i = [0:5]) {
        if(debug_on) echo(360*i/6, sin(360*i/6)*80, cos(360*i/6)*80);
        translate([sin(360*i/6)*80, cos(360*i/6)*80, 0 ])
            cylinder(h = 200, r=10);
        }
    translate([0, 0, 200])
        cylinder(h = 80, r1 = 120, r2 = 0);
    }
}

example005();

/*
 Written by
 Clifford Wolf <clifford@clifford.at>
 and
 Marius Kintel <marius@kintel.net>

 update 2025 Jeff Hayes <vulcan_at_mac_com>

 To the extent possible under law, the author(s) have dedicated all
 copyright and related and neighboring rights to this software to the
 public domain worldwide. This software is distributed without any
 warranty.

 You should have received a copy of the CC0 Public Domain
 Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
 */
