// Test hex and decimal parameter combinations

/* [RGB Color Components (Hex)] */
red = 0xFF;            // [0x00:0xFF]
green = 0x80;          // [0x00:0xFF]
blue = 0x40;           // [0x00:0xFF]

/* [RGBA with Alpha (Hex)] */
alpha = 0xFF;          // [0x00:0xFF]

/* [Dimensions (Normal)] */
width = 10;            // [1:100]
height = 20;           // [1:100]
depth = 30;            // [1:100]

/* [Scale Factors (Normal)] */
scale_x = 1.0;         // [0.1:0.1:5.0]
scale_y = 1.0;         // [0.1:0.1:5.0]

/* [Hex Offsets] */
offset_x = 0x10;       // [0x00:0x100]
offset_y = 0x20;       // [0x00:0x100]

echo("Testing mixed hex and decimal parameters");

// Test hex color components
echo(red=red, green=green, blue=blue, alpha=alpha);
assert(red == 255, "red should be 255");
assert(green == 128, "green should be 128");
assert(blue == 64, "blue should be 64");
assert(alpha == 255, "alpha should be 255");

// Test normal parameters
echo(width=width, height=height, depth=depth);
assert(width == 10, "width should be 10");
assert(height == 20, "height should be 20");
assert(depth == 30, "depth should be 30");

// Test scale factors (decimal)
echo(scale_x=scale_x, scale_y=scale_y);
assert(scale_x == 1.0, "scale_x should be 1.0");
assert(scale_y == 1.0, "scale_y should be 1.0");

// Test hex offsets
echo(offset_x=offset_x, offset_y=offset_y);
assert(offset_x == 16, "offset_x should be 16");
assert(offset_y == 32, "offset_y should be 32");

// Verify hex and decimal can coexist in same file
assert(red != width, "hex and decimal parameters should be independent");

echo("All mixed parameter tests passed");
