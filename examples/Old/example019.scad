/*
 Old/Example019
 Use a lookup() module to extrapolate the heights of a row of cylinders
 */

// Customizer variables

/* [Hidden] */
exNum = 19; // example number
include <example-index.scad>

echo( announceExample(exNum) );
// Visualiztion controls set to good defaults
//  for smooth curved surfaces
$fs = 0.5;
$fa = 1.0;

function get_cylinder_h(p) =
  lookup(p, [
    [ -200, 5 ],
    [ -50, 20 ],
    [ -20, 18 ],
    [ +80, 25 ],
    [ +150, 2 ]
    ]);


for (i = [-100:5:+100]) {
  translate([ i, 0, -30 ])
    cylinder(r1 = 6, r2 = 2, h = get_cylinder_h(i)*3);
}

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
