/*
 Old/Example004
 Cube minus sphere
 */

// Customizer variables

// Diameter of the sphere
cube_size = 30; // [1:1:50]
sph_size  = 20; // [1:1:50]

/* [Hidden] */
exNum = 4; // example number
include <example-index.scad>

echo( announceExample(exNum) );
// Visualiztion controls set to good defaults
//  for smooth curved surfaces
$fs = 0.5;
$fa = 1.0;

module example004()
    {
    difference() {
        cube(30, center = true);
            sphere(20);
        }
    }

example004();

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
