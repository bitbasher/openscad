/*
 Old/Example023
 Example combining MCAD/fonts.scad with search() function.
 */

// Customizer variables

/* [Hidden] */
exNum = 23; // example number
include <example-index.scad>

echo( announceExample(exNum) );
// Visualiztion controls set to good defaults
//  for smooth curved surfaces
$fs = 0.5;
$fa = 1.0;

// had to make a private copy of this from MCAD as
//  the needed module name started with a numeric digit
//  namely: 8bit_polyfont()
use <fonts.scad>

thisFont=eight_bit_polyfont();
x_shift=thisFont[0][0];
y_shift=thisFont[0][1];

hours=["one","two","three","four","five","six","seven","eight","nine","ten","eleven","twelve"];

// this module uses search() to find the index of the correct
//  word to use in the hours array.
// loop around the hours of the clock to display the written
//  names of the hours radially around the origin.
module clock_hour_words(word_offset=20.0,word_height=2.0) {
  for(i=[0:(len(hours)-1)]) {
    hourHandAngle=(i+1)*360/len(hours);
    theseIndicies=search(hours[i],thisFont[2],1,1);
    rotate(90-hourHandAngle)
      translate([word_offset,0])
    for( j=[0:(len(theseIndicies)-1)] )
      translate([j*x_shift,-y_shift/2])
        linear_extrude(height=word_height)
          polygon(points=thisFont[2][theseIndicies[j]][6][0],paths=thisFont[2][theseIndicies[j]][6][1]);
    }
  }

clock_hour_words(word_offset=16.0,word_height=5.0);

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
