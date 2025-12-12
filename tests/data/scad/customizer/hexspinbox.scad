/* [Hex Values] */
hexValue = 0xFF;  // [0x00:0xFF]
color = 0x80;     // [0x000:0xFeF]

/* [Normal Values] */
normalValue = 10; // [0:100]

translate([-150,-150,0])
cube( normalValue );


translate([150,150,0])
sphere( r = hexValue );
