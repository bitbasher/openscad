/*
 Old/Example018
 Using children() module, draw a matrix of shapes
 */

// Customizer variables

/* [Hidden] */
exNum = 18; // example number
include <example-index.scad>

echo( announceExample(exNum) );
// Visualiztion controls set to good defaults
//  for smooth curved surfaces
$fs = 0.5;
$fa = 1.0;

// step through the children of a module to translate
//  them into position.
module step(len, mod) {
  for (i = [0:$children-1]) {
    translate([ len*(i - ($children-1)/2), 0, 0 ])
      children((i+mod) % $children);
    }
  }

for (i = [1:4]) {
  translate([0, -250+i*100, 0])
    step(100, i) {
      sphere(30);
      cube(60, true);
      cylinder(r = 30, h = 50, center = true);

      union() {
        cube(45, true);
        rotate([45, 0, 0])
          cube(50, true);
        rotate([0, 45, 0])
          cube(50, true);
        rotate([0, 0, 45])
          cube(50, true);
        }
      } // end step()
  } // end for loop
