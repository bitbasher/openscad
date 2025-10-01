/*
 Old/Example002
 cone minus three orthogonal cubes
 */

/* [Hidden] */
exampleNumber = 2;
exampleGroup = "Old";
exampleName = "Cone minus three orthogonal cubes";
exampleFile = "-cone-with-holes";

echo( str( "\n\t**\n\tSCAD Version ", version(),
    "\n\t", exampleName,"\n\t**" ) );
// visualiztion controls set to good defaults
//  for smooth curved surfaces
$fs = 0.5;
$fa = 1.0;

module example002()
{
  /* Intersection keeps the part(s) of the difference
      child that are also part of the cone child, the
      tapered cylinder */
  intersection() {
    difference() {
      union() {
        // cube outside cone to flatten its side
        cube([30, 30, 30], center = true);
        // add a block under the cylinder
        translate([0, 0, -25])
          cube([15, 15, 50], center = true);
      }
      // subtract the three interior cubes
      union() {
        cube([50, 10, 10], center = true);
        cube([10, 50, 10], center = true);
        cube([10, 10, 50], center = true);
      }
    }
    translate([0, 0, 5])
      // draw the cone from Radius 20 upwards to Radius 5
      cylinder(h = 50, r1 = 20, r2 = 5, center = true);
  }
}

example002();

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