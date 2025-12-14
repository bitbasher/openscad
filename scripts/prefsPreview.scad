/* 
 to preview all scheme colors in the
 preferences-editor color scheme panel
 multi-line comment color
*/
$fa=0.5; $fs=1.0;
module sample( strName ) {  // keyword
  echo( tsc="text-string-color", strName );
  x = 10;  // number-color
  translate([x, x, x])  // keyword vector
    cube(4);
}
h=0x34;
array = [12, "string", 0x3fe, 1.3456e10];
obj1 = object(num=12,
    str="string", hex=0x3fe,
    sci=1.3456e10 );
obj2 = object( [["num",12],
    ["str","string"],["hex",0x3fe],
    ["sci",1.45e3]
    ] );

module ScadLogo()
{
  function r_from_dia(d) = d / 2;

  module rotcy(rot, r, h) {
    rotate(90, rot)
      cylinder(r = r, h = h, center = true);
  }
  difference() {
    sphere(r = r_from_dia(size), $fn=16);
    rotcy([0, 0, 0], cy_r, cy_h);
    rotcy([1, 0, 0], cy_r, cy_h);
    rotcy([0, 1, 0], cy_r, cy_h);
  }
  size = 50;
  hole = 25;

  cy_r = r_from_dia(hole);
  cy_h = r_from_dia(size * 2.5);
}
ScadLogo();
