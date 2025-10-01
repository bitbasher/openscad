/*
 Old/Example021
 Use of projection to make a shape, and and to slice with option "cut"
 */

// Customizer variables
show_projection = "thing"; // ["thing","projection","cut","all"]

/* [Hidden] */
exNum = 21; // example number
include <example-index.scad>

echo( announceExample(exNum) );
// Visualiztion controls set to good defaults
//  for smooth curved surfaces
$fs = 0.5;
$fa = 1.0;

if( show_projection=="thing")
  thing();
else if( show_projection=="projection") 
  demo_projection();
else if( show_projection=="cut")
  demo_projection_cut();
else if( show_projection=="all") {
  translate([30,0,0])
    demo_projection();
  translate([-30,0,0])
    demo_projection_cut();
  }
else
  echo("nothing selected");

// a sphere minus three tapered cylinders
module thing() {
  $fa = 30;
  difference() {
    sphere(r = 25);
    cylinder(h = 62.5, r1 = 12.5, r2 = 6.25,
          center = true);
    rotate(90, [ 1, 0, 0 ])
      cylinder(h = 62.5, r1 = 12.5, r2 = 6.25, center = true);
    rotate(90, [ 0, 1, 0 ])
      cylinder(h = 62.5, r1 = 12.5, r2 = 6.25, center = true);
    }
  }

// projecting the shape onto the X-Y plan from
//  above and below creates a 2D shape.
// The linear_extrude draws the shape as a 3D
//  shape that can be rendered
module demo_projection() {
  linear_extrude(center = true, height = 0.5)
    projection(cut = false)
      thing();
    // show the shape before projection
    % thing();  // using background modifier
  }

// projecting the shape with the cut option.
// The linear_extrude draws the slice as a 3D
//  shape that can be rendered
module demo_projection_cut() {
  for (i=[-20:5:+20]) {
    rotate(-30, [ 1, 1, 0 ])
      translate([ 0, 0, -i ])
        linear_extrude(center = true, height = 0.5)
          projection(cut = true)
            translate([ 0, 0, i ])
              rotate(+30, [ 1, 1, 0 ])
                thing();
    }
  // show the shape before projection
  % thing(); // using background modifier 
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
