/*
 Old/Example006
 Using hull() and a List Comprehension to define a data vector to draw a dice. 
 */

// Customizer variables

/* [Hidden] */
exNum = 6; // example number
include <example-index.scad>

echo( announceExample(exNum) );
// Visualiztion controls set to good defaults
//  for smooth curved surfaces
$fs = 0.5;
$fa = 1.0;

/*
 Extend the cube module to have rounded corners
  and edges using the hull() module to draw surfaces
  between the spheres at the eight corners.
 Size as a vector gives the size of the cube in each
  of the three axes
 r is the radius of the fillet on the edges. The
  size is reduced by the radius so that the outer
  surfaces of the resulting figure are at the
  dimension given by "size"
 center: an optional parameter that when true causes
  the cube to be drawn centered at the origin. A
  translate is used to shift the model.
 */
module rounded_cube(size,r,center=false) {
    s = is_list(size) ?
        size // given a vector use it 
    : 
        [size,size,size] // vector with all sides equal
    ;

    translate(center ? -s/2 : [0,0,0])
        hull() {
            translate([    r,    r,    r]) sphere(r=r);
            translate([    r,    r,s.z-r]) sphere(r=r);
            translate([    r,s.y-r,    r]) sphere(r=r);
            translate([    r,s.y-r,s.z-r]) sphere(r=r);
            translate([s.x-r,    r,    r]) sphere(r=r);
            translate([s.x-r,    r,s.z-r]) sphere(r=r);
            translate([s.x-r,s.y-r,    r]) sphere(r=r);
            translate([s.x-r,s.y-r,s.z-r]) sphere(r=r);
            }
    }

/*
 each element of the dice data is a vector
  Rotation about Z,
  Rotation about X,
  vector of tranlations:
    distance in X,
    distance in Z
  where the vector has an element for each dot
   on every face of the cube
 */
diceData = [
    [0,0,[
        [0, 0] // one dot
        ]
    ],
    [  90, 0, [  // two dots to right of one
        [-20, -20],
        [+20, +20]
        ]
    ],
    [ 180, 0, [ // 6 opposite to one
        [-20, -25], [-20, 0], [-20, +25],
        [+20, -25], [+20, 0], [+20, +25]
        ]
    ],
    [ 270, 0, [ // 5 opposite to two
        [0, 0], [-25, -25], [+25, -25],
        [-25, +25], [+25, +25]
        ]
    ],
    [ 0, 90, [ // 3 on bottom of cube
        [-25, -25], [0, 0], [+25, +25]
        ]
    ],
    [ 0, -90, [ // 4 on top
        [-25, -25], [+25, -25], [-25, +25], [+25, +25]
        ]
    ]
    ];

module example006() {
    difference() {
        rounded_cube(100, 10, center=true);
        union() {
            for (i = diceData) {
                rotate(i[0], [0, 0, 1])
                    rotate(i[1], [1, 0, 0])
                        translate([0, -50, 0])
                            for (j = i[2]) {
                                translate([j[0], 0, j[1]])
                                    sphere(10);
                                }
                    } // end outer for loop
            } // end union
        } // end difference
    }

example006();

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
