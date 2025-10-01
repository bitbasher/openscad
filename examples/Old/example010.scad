/*
 Old/Example010
 Using surface() to present data imported from a data file

 a data filem "example010.dat" has been generated using octave:
  d = (sin(1:0.2:10)' * cos(1:0.2:10)) * 10;
  save("example010.dat", "d");
 The data is a 46x46 matrix
 */

// Customizer variables

/* [Hidden] */
exNum = 10; // example number
include <example-index.scad>

echo( announceExample(exNum) );
// Visualiztion controls set to good defaults
//  for smooth curved surfaces
$fs = 0.5;
$fa = 1.0;

intersection() {
  surface(file = "example010.dat", center = true, convexity = 5);

  rotate(45, [0, 0, 1]) // rotate 45 deg about the Z axis
    surface(file = "example010.dat", center = true, convexity = 5);
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
