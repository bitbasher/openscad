/*
 *  OpenSCAD (www.openscad.org)
 *  Copyright (C) 2009-2011 Clifford Wolf <clifford@clifford.at> and
 *                          Marius Kintel <marius@kintel.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  As a special exception, you have permission to link this program
 *  with the CGAL library and distribute executables, as long as you
 *  follow the requirements of the GNU GPL in regard to all of the
 *  software in the executable aside from CGAL.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "builtin_functions_newmath.h"
#include "core/Arguments.h"
#include "core/Builtins.h"
#include "core/function.h"
#include "core/Parameters.h"
#include "utils/printutils.h"

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static inline bool check_arguments(const char *function_name, const Arguments& arguments,
                                   const Location& loc, unsigned int expected_count)
{
  if (arguments.size() != expected_count) {
    print_argCnt_warning(function_name, arguments.size(), STR(expected_count), loc,
                         arguments.documentRoot());
    return false;
  }
  return true;
}

static inline bool check_number_arg(const char *function_name, const Argument& arg, 
                                    const Location& loc, const Arguments& arguments,
                                    const char *arg_name)
{
  if (arg->type() != Value::Type::NUMBER) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "%1$s() requires %2$s to be a number, got %3$s", 
        function_name, arg_name, arg->typeName());
    return false;
  }
  return true;
}

// =============================================================================
// Math Functions
// =============================================================================

/**
 * @brief Constrain a value to a specified range.
 * 
 * @param value The value to clamp (positional arg 0)
 * @param min_val Minimum allowed value (positional arg 1)
 * @param max_val Maximum allowed value (positional arg 2)
 * @return value clamped to [min_val, max_val]
 * 
 * OpenSCAD usage: clamp(value, min, max) -> number
 * Example: clamp(15, 0, 10) returns 10
 * Example: clamp(-5, 0, 10) returns 0
 * Example: clamp(5, 0, 10) returns 5
 */
Value builtin_clamp(Arguments arguments, const Location& loc)
{
  if (!check_arguments("clamp", arguments, loc, 3)) {
    return Value::undefined.clone();
  }

  if (!check_number_arg("clamp", arguments[0], loc, arguments, "value") ||
      !check_number_arg("clamp", arguments[1], loc, arguments, "min") ||
      !check_number_arg("clamp", arguments[2], loc, arguments, "max")) {
    return Value::undefined.clone();
  }

  double value = arguments[0]->toDouble();
  double min_val = arguments[1]->toDouble();
  double max_val = arguments[2]->toDouble();

  if (min_val > max_val) {
    LOG(message_group::Warning, loc, arguments.documentRoot(),
        "clamp() min (%1$f) is greater than max (%2$f)", min_val, max_val);
    return Value::undefined.clone();
  }

  if (value < min_val) return {min_val};
  if (value > max_val) return {max_val};
  return {value};
}

/**
 * @brief Linear interpolation between two values.
 * 
 * Computes: a + (b - a) * t
 * 
 * @param a Start value (positional arg 0)
 * @param b End value (positional arg 1)
 * @param t Interpolation factor (positional arg 2)
 * @return Interpolated value (t=0 returns a, t=1 returns b)
 * 
 * OpenSCAD usage: interpolate(a, b, t) -> number
 * Example: interpolate(0, 10, 0.5) returns 5
 * Example: interpolate(0, 10, 0.25) returns 2.5
 * Note: t is not clamped, values outside [0,1] will extrapolate
 */
Value builtin_interpolate(Arguments arguments, const Location& loc)
{
  if (!check_arguments("interpolate", arguments, loc, 3)) {
    return Value::undefined.clone();
  }

  if (!check_number_arg("interpolate", arguments[0], loc, arguments, "a") ||
      !check_number_arg("interpolate", arguments[1], loc, arguments, "b") ||
      !check_number_arg("interpolate", arguments[2], loc, arguments, "t")) {
    return Value::undefined.clone();
  }

  double a = arguments[0]->toDouble();
  double b = arguments[1]->toDouble();
  double t = arguments[2]->toDouble();

  return {a + (b - a) * t};
}

/**
 * @brief Convert degrees to radians.
 * 
 * Formula: radians = degrees * π / 180
 * 
 * @param degrees Angle in degrees (positional arg 0)
 * @return Angle in radians
 * 
 * OpenSCAD usage: deg2rad(degrees) -> radians
 * Example: deg2rad(180) returns ~3.14159 (π)
 * Example: deg2rad(90) returns ~1.5708 (π/2)
 * Note: OpenSCAD trig functions use degrees; use this for external APIs expecting radians
 */
Value builtin_deg2rad(Arguments arguments, const Location& loc)
{
  if (!check_arguments("deg2rad", arguments, loc, 1)) {
    return Value::undefined.clone();
  }

  if (!check_number_arg("deg2rad", arguments[0], loc, arguments, "degrees")) {
    return Value::undefined.clone();
  }

  double degrees = arguments[0]->toDouble();
  return {degrees * M_PI / 180.0};
}

/**
 * @brief Convert radians to degrees.
 * 
 * Formula: degrees = radians * 180 / π
 * 
 * @param radians Angle in radians (positional arg 0)
 * @return Angle in degrees
 * 
 * OpenSCAD usage: rad2deg(radians) -> degrees
 * Example: rad2deg(3.14159) returns ~180
 * Example: rad2deg(1.5708) returns ~90
 * Note: OpenSCAD trig functions use degrees; use this for external APIs providing radians
 */
Value builtin_rad2deg(Arguments arguments, const Location& loc)
{
  if (!check_arguments("rad2deg", arguments, loc, 1)) {
    return Value::undefined.clone();
  }

  if (!check_number_arg("rad2deg", arguments[0], loc, arguments, "radians")) {
    return Value::undefined.clone();
  }

  double radians = arguments[0]->toDouble();
  return {radians * 180.0 / M_PI};
}

// =============================================================================
// Registration
// =============================================================================

void register_builtin_newmath()
{
  Builtins::init("clamp", new BuiltinFunction(&builtin_clamp),
                 {
                   "clamp(value, min, max) -> number",
                 });

  Builtins::init("interpolate", new BuiltinFunction(&builtin_interpolate),
                 {
                   "interpolate(a, b, t) -> number",
                 });

  Builtins::init("deg2rad", new BuiltinFunction(&builtin_deg2rad),
                 {
                   "deg2rad(degrees) -> radians",
                 });

  Builtins::init("rad2deg", new BuiltinFunction(&builtin_rad2deg),
                 {
                   "rad2deg(radians) -> degrees",
                 });
}
