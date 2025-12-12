// Test hex parameter validation and edge cases
// Expected warnings:
//   hex_neg_min: negative minimum -100 clamped to 0
//   hex_neg_max: negative maximum -100 clamped to 0
//   hex_neg_step: step -2 < 1 clamped to 1
//   hex_zero_step: step 0 < 1 clamped to 1

/* [Invalid Values - Should Show Warnings] */
hex_neg_min = 0xFF;    // [-100:0xFF]
hex_neg_max = 0x10;    // [0x00:-100]
hex_neg_step = 0x80;   // [0x00:-2:0xFF]
hex_zero_step = 0x10;  // [0x00:0:0x100]

/* [Large Values] */
hex_large1 = 0xFFFFFF;     // [0x000000:0xFFFFFF]
hex_large2 = 0x10000;      // [0x00000:0x100000]

/* [Small Steps] */
hex_fine = 0x80;       // [0x00:1:0xFF]
hex_coarse = 0x80;     // [0x00:0x10:0xFF]

/* [Boundary Values] */
hex_zero = 0x00;       // [0x00:0x100]
hex_one = 0x01;        // [0x00:0xFF]
hex_255 = 0xFF;        // [0x00:0xFF]
hex_256 = 0x100;       // [0x000:0xFFF]

echo("Testing hex parameter validation");

// Values should still parse correctly despite warnings
echo(hex_neg_min=hex_neg_min);
echo(hex_neg_max=hex_neg_max);
echo(hex_neg_step=hex_neg_step);
echo(hex_zero_step=hex_zero_step);

assert(hex_neg_min == 255, "hex_neg_min should be 0xFF (255)");
assert(hex_neg_max == 16, "hex_neg_max should be 0x10 (16)");
assert(hex_neg_step == 128, "hex_neg_step should be 0x80 (128)");
assert(hex_zero_step == 16, "hex_zero_step should be 0x10 (16)");

// Test large values
echo(hex_large1=hex_large1);
echo(hex_large2=hex_large2);
assert(hex_large1 == 16777215, "0xFFFFFF should equal 16777215");
assert(hex_large2 == 65536, "0x10000 should equal 65536");

// Test boundary values
echo(hex_zero=hex_zero);
echo(hex_one=hex_one);
echo(hex_255=hex_255);
echo(hex_256=hex_256);
assert(hex_zero == 0, "0x00 should equal 0");
assert(hex_one == 1, "0x01 should equal 1");
assert(hex_255 == 255, "0xFF should equal 255");
assert(hex_256 == 256, "0x100 should equal 256");

echo("All hex validation tests passed");
