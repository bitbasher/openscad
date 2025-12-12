// Test hex parameter parsing and display

/* [Basic Hex Values] */
hex1 = 0xFF;           // [0x00:0xFF]
hex2 = 0x10;           // [0x00:0x100]
hex3 = 0xABCD;         // [0x0000:0xFFFF]

/* [Hex with Steps] */
hex_step1 = 0x10;      // [0x00:0x10:0x100]
hex_step2 = 0x80;      // [0x00:16:0xFF]
hex_step3 = 0x100;     // [0x00:0x100:0x1000]

/* [Hex Mixed Case] */
hex_lower = 0xff;      // [0x00:0xff]
hex_upper = 0xFF;      // [0x00:0xFF]
hex_mixed = 0xAbCd;    // [0x0000:0xFFFF]

/* [Edge Cases] */
hex_min = 0x00;        // [0x00:0xFF]
hex_max = 0xFFFF;      // [0x0000:0xFFFF]
hex_single = 0xF;      // [0x0:0xF]

/* [Normal Parameters for Comparison] */
normal1 = 255;         // [0:255]
normal2 = 16;          // [0:16:256]

// Test that hex values parse correctly
echo("Testing hex parameter parsing");
echo(hex1=hex1);
echo(hex2=hex2);
echo(hex3=hex3);

// Verify hex equals decimal equivalent
assert(hex1 == 255, "0xFF should equal 255");
assert(hex2 == 16, "0x10 should equal 16");
assert(hex3 == 43981, "0xABCD should equal 43981");

// Test case insensitivity
assert(hex_lower == 255, "0xff should equal 255");
assert(hex_upper == 255, "0xFF should equal 255");
assert(hex_mixed == 43981, "0xAbCd should equal 43981");

// Test edge cases
assert(hex_min == 0, "0x00 should equal 0");
assert(hex_max == 65535, "0xFFFF should equal 65535");
assert(hex_single == 15, "0xF should equal 15");

// Test steps
echo(hex_step1=hex_step1);
echo(hex_step2=hex_step2);
echo(hex_step3=hex_step3);

// Test normal parameters still work
assert(normal1 == 255, "normal parameter should work");
assert(normal2 == 16, "normal parameter with step should work");

echo("All hex parameter tests passed");
