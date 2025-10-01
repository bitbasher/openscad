/*
 Old/Example016
 Carve letter M's into a a block. Import letter shapes frp, an STL. 

 Chopped_blocks.stl is derived from Basics/LetterBlock.scad
 renamed to "example016.stl"
 The exported STL was converted to binary using MeshLab
 */

// Customizer variables

/* [Hidden] */
exNum = 16; // example number
include <example-index.scad>

echo( announceExample(exNum) );
// Visualiztion controls set to good defaults
//  for smooth curved surfaces
$fs = 0.5;
$fa = 1.0;

module blk1() {
  cube([ 65, 28, 28 ], center = true);
}

module blk2() {
  difference() {
    translate([ 0, 0, 7.5 ])
      cube([ 60, 28, 14 ], center = true);
    cube([ 8, 32, 32 ], center = true);
  }
}

module chop() {
  translate([ -18, 0, 0 ])
    import(file = "example016.stl", convexity = 12);
}

echo(version=version());

difference() {
  blk1();
  for (alpha = [0, 90, 180, 270]) {
    rotate(alpha, [ 1, 0, 0])
      render(convexity = 12)
        difference() {
          blk2();
          chop();
        }
  }
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
