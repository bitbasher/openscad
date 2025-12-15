// Test file for new math functions: clamp, interpolate, deg2rad, rad2deg

echo("=== Testing clamp() ===");
echo("clamp(5, 0, 10) =", clamp(5, 0, 10));           // Expected: 5
echo("clamp(-5, 0, 10) =", clamp(-5, 0, 10));         // Expected: 0
echo("clamp(15, 0, 10) =", clamp(15, 0, 10));         // Expected: 10
echo("clamp(0, 0, 10) =", clamp(0, 0, 10));           // Expected: 0
echo("clamp(10, 0, 10) =", clamp(10, 0, 10));         // Expected: 10
echo("clamp(7.5, 2.5, 8.5) =", clamp(7.5, 2.5, 8.5)); // Expected: 7.5

echo("");
echo("=== Testing interpolate() ===");
echo("interpolate(0, 10, 0) =", interpolate(0, 10, 0));       // Expected: 0
echo("interpolate(0, 10, 1) =", interpolate(0, 10, 1));       // Expected: 10
echo("interpolate(0, 10, 0.5) =", interpolate(0, 10, 0.5));   // Expected: 5
echo("interpolate(0, 10, 0.25) =", interpolate(0, 10, 0.25)); // Expected: 2.5
echo("interpolate(0, 10, 0.75) =", interpolate(0, 10, 0.75)); // Expected: 7.5
echo("interpolate(5, 15, 0.5) =", interpolate(5, 15, 0.5));   // Expected: 10
echo("interpolate(10, 0, 0.5) =", interpolate(10, 0, 0.5));   // Expected: 5 (reverse)

echo("");
echo("=== Testing deg2rad() ===");
echo("deg2rad(0) =", deg2rad(0));         // Expected: 0
echo("deg2rad(90) =", deg2rad(90));       // Expected: ~1.5708 (π/2)
echo("deg2rad(180) =", deg2rad(180));     // Expected: ~3.14159 (π)
echo("deg2rad(360) =", deg2rad(360));     // Expected: ~6.28318 (2π)
echo("deg2rad(45) =", deg2rad(45));       // Expected: ~0.7854 (π/4)
echo("deg2rad(-90) =", deg2rad(-90));     // Expected: ~-1.5708 (-π/2)

echo("");
echo("=== Testing rad2deg() ===");
echo("rad2deg(0) =", rad2deg(0));                    // Expected: 0
echo("rad2deg(1.5708) =", rad2deg(1.5708));          // Expected: ~90
echo("rad2deg(3.14159) =", rad2deg(3.14159));        // Expected: ~180
echo("rad2deg(6.28318) =", rad2deg(6.28318));        // Expected: ~360
echo("rad2deg(0.7854) =", rad2deg(0.7854));          // Expected: ~45
echo("rad2deg(-1.5708) =", rad2deg(-1.5708));        // Expected: ~-90

echo("");
echo("=== Round-trip conversion test ===");
echo("rad2deg(deg2rad(45)) =", rad2deg(deg2rad(45)));     // Expected: 45
echo("rad2deg(deg2rad(90)) =", rad2deg(deg2rad(90)));     // Expected: 90
echo("rad2deg(deg2rad(180)) =", rad2deg(deg2rad(180)));   // Expected: 180
echo("deg2rad(rad2deg(1)) =", deg2rad(rad2deg(1)));       // Expected: 1
echo("deg2rad(rad2deg(2.5)) =", deg2rad(rad2deg(2.5)));   // Expected: 2.5

echo("");
echo("=== Combined usage example ===");
// Clamp an interpolated value
t = 0.75;
interpolated = interpolate(0, 100, t);
clamped = clamp(interpolated, 0, 50);
echo("interpolate(0, 100, 0.75) clamped to [0,50] =", clamped); // Expected: 50

// Use deg2rad with trig functions
angle_deg = 30;
angle_rad = deg2rad(angle_deg);
echo("sin(30°) via rad =", sin(rad2deg(angle_rad))); // Should match sin(30)
echo("cos(30°) via rad =", cos(rad2deg(angle_rad))); // Should match cos(30)

echo("");
echo("=== All tests complete ===");
