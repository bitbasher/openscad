/*
 Old/Example001
 Ball minus three orthogonal cylinders
 draws the OpenSCAD logo
 */

// Customizer variables

// Diameter of the sphere
sph_size = 50; // [1:1:50]

/* [Hidden] */
exNum = 1; // example number
include <example-index.scad>

echo( announceExample(exNum) );
// Visualiztion controls set to good defaults
//  for smooth curved surfaces
$fs = 0.5;
$fa = 1.0;

// The holes thru the sphere should be half
//  the sphere's diameter
hole = sph_size / 2;

module example001()
{
  // Radius is half of diameter
  function r_from_dia(d) = d / 2;

  // Apply a 90 degree rotation and
  //  create one cylinder of Radius "r"
  //  and Height "h"
  // Around the vector given by "axis"
  module rotcy(axis, r, h) {
    rotate(90, axis)
      cylinder(r = r, h = h, center = true);
  }

  difference() {
    sphere(d=sph_size); // need to use named
        // param "d" for the sphere's diameter
    rotcy([0, 0, 0], cy_r, cy_h);
    #rotcy([1, 0, 0], cy_r, cy_h); // # -> debug mode 
    rotcy([0, 1, 0], cy_r, cy_h);
  }

  // Calculate the size of the cylinders that
  //  will be subracted out of the sphere.
  // It does not matter that these definitions
  //  are here at the end - variables have their
  //  values assigned before modules are invoked
  cy_r = r_from_dia(hole);
  cy_h = r_from_dia(sph_size * 2.5);
}

example001();

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
