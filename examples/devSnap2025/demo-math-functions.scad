// ============================================================================
// OpenSCAD Math Functions Demo
// Demonstrates clamp(), interpolate(), deg2rad(), and rad2deg()
// ============================================================================

echo("╔════════════════════════════════════════════════════════════════════╗");
echo("║  OpenSCAD Math Functions Demo - devSnap2025                       ║");
echo("╚════════════════════════════════════════════════════════════════════╝");

// ============================================================================
// PART 1: clamp() - Constrain Values to Range
// ============================================================================

echo("\n--- Part 1: clamp() - Constrain to Range ---\n");

// Basic clamping examples
echo("1. Clamp value within range [0, 100]:");
echo("   clamp(-10, 0, 100):", clamp(-10, 0, 100), "(below min → 0)");
echo("   clamp(50, 0, 100):", clamp(50, 0, 100), "(within range → 50)");
echo("   clamp(150, 0, 100):", clamp(150, 0, 100), "(above max → 100)");

// Percentage clamping
echo("\n2. Clamp percentage to valid range [0, 1]:");
values = [-0.5, 0, 0.25, 0.5, 0.75, 1, 1.5];
echo("   Input values:", values);
for (v = values) {
    echo(str("   clamp(", v, ", 0, 1) = ", clamp(v, 0, 1)));
}

// Temperature example
echo("\n3. Clamp temperature to safe range [-40, 85]°C:");
temps = [-100, -40, 0, 25, 85, 200];
echo("   Temperatures:", temps);
for (t = temps) {
    clamped = clamp(t, -40, 85);
    echo(str("   ", t, "°C → ", clamped, "°C"));
}

// Negative ranges
echo("\n4. Clamp to negative range:");
echo("   clamp(-15, -10, -5):", clamp(-15, -10, -5), "→ -10");
echo("   clamp(-7, -10, -5):", clamp(-7, -10, -5), "→ -7");
echo("   clamp(0, -10, -5):", clamp(0, -10, -5), "→ -5");

// ============================================================================
// PART 2: interpolate() - Linear Interpolation (lerp)
// ============================================================================

echo("\n--- Part 2: interpolate() - Linear Interpolation ---\n");

// Basic interpolation
echo("5. Interpolate between 0 and 100:");
echo("   interpolate(0, 100, 0.0):", interpolate(0, 100, 0.0), "(0% → 0)");
echo("   interpolate(0, 100, 0.25):", interpolate(0, 100, 0.25), "(25% → 25)");
echo("   interpolate(0, 100, 0.5):", interpolate(0, 100, 0.5), "(50% → 50)");
echo("   interpolate(0, 100, 0.75):", interpolate(0, 100, 0.75), "(75% → 75)");
echo("   interpolate(0, 100, 1.0):", interpolate(0, 100, 1.0), "(100% → 100)");

// Color blending example (conceptual)
echo("\n6. Color blending (grayscale 0=black, 255=white):");
black = 0;
white = 255;
for (t = [0:0.2:1]) {
    gray = interpolate(black, white, t);
    echo(str("   t=", t, " → gray=", gray));
}

// Negative to positive range
echo("\n7. Interpolate across zero:");
echo("   interpolate(-50, 50, 0.0):", interpolate(-50, 50, 0.0), "→ -50");
echo("   interpolate(-50, 50, 0.5):", interpolate(-50, 50, 0.5), "→ 0");
echo("   interpolate(-50, 50, 1.0):", interpolate(-50, 50, 1.0), "→ 50");

// Extrapolation (t outside [0,1])
echo("\n8. Extrapolation (t < 0 or t > 1):");
echo("   interpolate(10, 20, -0.5):", interpolate(10, 20, -0.5), "(extrapolate backward)");
echo("   interpolate(10, 20, 1.5):", interpolate(10, 20, 1.5), "(extrapolate forward)");

// Animation example (conceptual)
echo("\n9. Animation: object position over time (0-10mm):");
start_pos = 0;
end_pos = 10;
for (frame = [0:2:10]) {
    t = frame / 10;
    pos = interpolate(start_pos, end_pos, t);
    echo(str("   Frame ", frame, "/10: position = ", pos, "mm"));
}

// ============================================================================
// PART 3: deg2rad() - Degrees to Radians
// ============================================================================

echo("\n--- Part 3: deg2rad() - Degrees to Radians ---\n");

// Common angles
echo("10. Convert common angles to radians:");
angles_deg = [0, 30, 45, 60, 90, 180, 270, 360];
for (deg = angles_deg) {
    rad = deg2rad(deg);
    echo(str("   ", deg, "° = ", rad, " rad"));
}

// Circle calculations
echo("\n11. Full circle: 360° in radians:");
full_circle = deg2rad(360);
echo("   deg2rad(360):", full_circle);
echo("   Expected: 2π ≈ 6.28319");

// Negative angles
echo("\n12. Negative angles:");
echo("   deg2rad(-90):", deg2rad(-90), "rad");
echo("   deg2rad(-180):", deg2rad(-180), "rad");

// ============================================================================
// PART 4: rad2deg() - Radians to Degrees
// ============================================================================

echo("\n--- Part 4: rad2deg() - Radians to Degrees ---\n");

// Common radian values
echo("13. Convert common radian values to degrees:");
angles_rad = [0, PI/6, PI/4, PI/3, PI/2, PI, 3*PI/2, 2*PI];
names = ["0", "π/6", "π/4", "π/3", "π/2", "π", "3π/2", "2π"];
for (i = [0:len(angles_rad)-1]) {
    deg = rad2deg(angles_rad[i]);
    echo(str("   ", names[i], " (", angles_rad[i], ") = ", deg, "°"));
}

// Negative radians
echo("\n14. Negative radians:");
echo("   rad2deg(-π/2):", rad2deg(-PI/2), "°");
echo("   rad2deg(-π):", rad2deg(-PI), "°");

// ============================================================================
// PART 5: Round-Trip Conversions
// ============================================================================

echo("\n--- Part 5: Round-Trip Conversions ---\n");

// Verify conversions are reversible
echo("15. Verify round-trip accuracy:");
test_angles = [0, 45, 90, 135, 180, 225, 270, 315];
for (original = test_angles) {
    rad = deg2rad(original);
    back = rad2deg(rad);
    echo(str("   ", original, "° → ", rad, " rad → ", back, "°"));
}

// ============================================================================
// PART 6: Practical Use Cases
// ============================================================================

echo("\n--- Part 6: Practical Use Cases ---\n");

// Use case: Constrain user input
echo("16. Constrain rotation angle to valid range:");
user_rotation = 450; // Invalid: > 360
safe_rotation = clamp(user_rotation, 0, 360);
echo("   User input:", user_rotation, "°");
echo("   Clamped:", safe_rotation, "°");

// Use case: Animate rotation
echo("\n17. Smooth rotation animation (0° to 360° in 5 steps):");
for (step = [0:1:5]) {
    t = step / 5;
    angle = interpolate(0, 360, t);
    echo(str("   Step ", step, ": rotate([0, 0, ", angle, "])"));
}

// Use case: Calculate arc length
radius = 10;
angle_deg = 60;
echo("\n18. Calculate arc length (r=10, angle=60°):");
angle_rad = deg2rad(angle_deg);
arc_length = radius * angle_rad;
echo("   Angle:", angle_deg, "° =", angle_rad, "rad");
echo("   Arc length = r × θ =", radius, "×", angle_rad, "=", arc_length);

// Use case: Fade opacity
echo("\n19. Fade object opacity from 0% to 100%:");
for (step = [0:25:100]) {
    t = step / 100;
    opacity = interpolate(0, 1, t);
    echo(str("   ", step, "% → opacity = ", opacity));
}

// Use case: Combined clamping and interpolation
echo("\n20. Safe interpolation with clamped parameter:");
unsafe_t = 1.5; // Outside [0,1]
safe_t = clamp(unsafe_t, 0, 1);
result = interpolate(0, 100, safe_t);
echo("   Unsafe t:", unsafe_t, "→ clamped to:", safe_t);
echo("   interpolate(0, 100, safe_t):", result);

// Use case: Convert angle for trigonometry
echo("\n21. Use deg2rad for trig functions:");
angle = 30;
rad = deg2rad(angle);
sine_val = sin(rad);
cosine_val = cos(rad);
echo("   sin(30°) = sin(", rad, " rad) =", sine_val);
echo("   cos(30°) = cos(", rad, " rad) =", cosine_val);

// ============================================================================
// Summary
// ============================================================================

echo("\n╔════════════════════════════════════════════════════════════════════╗");
echo("║  Demo Complete!                                                    ║");
echo("║                                                                    ║");
echo("║  New Math Functions:                                               ║");
echo("║  • clamp(value, min, max) - Constrain to range                    ║");
echo("║  • interpolate(a, b, t) - Linear interpolation (lerp)             ║");
echo("║  • deg2rad(degrees) - Convert degrees to radians                  ║");
echo("║  • rad2deg(radians) - Convert radians to degrees                  ║");
echo("╚════════════════════════════════════════════════════════════════════╝");
