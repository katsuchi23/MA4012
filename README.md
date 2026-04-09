# MA4012 Robot Scripts (VEX Cortex / RobotC)

This repo now has a clean script set for your competition robot:

- `individual_test/` for single-component debugging.
- `functionality_testing/` for behavior debugging.
- `competition_main.c` as the integrated competition code.

All scripts assume this hardware mapping:

- Line sensors (digital): `dgtl12` front-left, `dgtl11` front-right, `dgtl10` back-left, `dgtl9` back-right.
- Light sensors (analog): `in4` front-below, `in3` front-upper, `in2` center, `in1` back.
- Compass (digital): `dgtl3` west, `dgtl4` south, `dgtl5` east, `dgtl6` noth.
- Unused digital reserved: `dgtl1`, `dgtl2`, `dgtl7`, `dgtl8` as `sensorNone`.
- Motors: `port6` gate, `port7` collector, `port8` left wheel, `port9` right wheel (reversed).

If your wiring is different, only update the `#pragma config` block in each file.

## Directory Guide

### `individual_test/`

- `motor_test.c`
  - Runs each motor one by one (forward/backward for wheels, open/close for gate).
  - Use this first after rewiring.

- `light_sensor_test.c`
  - Streams analog values for all 4 light sensors.
  - Also prints quick flags: ball candidate, obstacle candidate, center ball loaded, back sensor near collection.

- `line_sensor_test.c`
  - Streams all 4 line sensor states and prints detected pattern.
  - Confirms arena boundary logic trigger points.

- `compass_test.c`
  - Prints raw compass states `W/S/E/N` and mapped heading.
  - Compass is active-low (`0 = active direction`), so rotate robot manually and verify.

- `forward_motion_test.c`
  - Simple straight forward wheel motion test.
  - Useful to quickly verify both wheel motors move robot forward correctly.

- `west_alignment_test.c`
  - Wheel-only heading alignment test to WEST using compass.
  - Rotates until WEST is detected (active-low compass), then stops.

### `functionality_testing/`

- `searching_phase.c`
  - SEARCHING phase logic module.
  - Forward/search cycle with ball detection checks.
  - Runnable standalone once for debugging (`task main` is one-shot).

- `collecting_phase.c`
  - COLLECTING phase logic module.
  - Forward collect motion with center-sensor timeout behavior.
  - Runnable standalone once for debugging (`task main` is one-shot).

- `depositing_phase.c`
  - DEPOSITING phase logic module.
  - West alignment, reverse cycle, and back-sensor stability checks.
  - Runnable standalone once for debugging (`task main` is one-shot).

- `ball_deposit_test.c`
  - Runs deposit behavior:
    - aligns robot to East using compass,
    - reverses to collection area using back line + back light sensors,
    - opens/closes gate to drop ball.

- `boundary_check_test.c`
  - Roomba-style movement:
    - drive forward continuously,
    - when line sensors trigger, reverse (~1500ms) then compass-align to West,
    - continue moving.

### Main Competition Code

- `competition_main.c`
  - Integrates boundary handling, ball collection, and ball deposit into one loop.
  - This is the file intended for competition runs.

## Tuning Constants (Important)

Each script has constants at the top for easy tuning, for example:

- `FRONT_BELOW_BALL_MIN` / `FRONT_BELOW_BALL_MAX`
- `FRONT_UPPER_OBSTACLE_MIN`
- `CENTER_BALL_COLLECT_MIN`
- `BACK_DEPOSIT_TRIGGER_MIN`
- `GATE_OPEN_SPEED` / `GATE_CLOSE_SPEED`

You should tune these on your real arena before match day.

## Recommended Bring-Up Order

1. Run `individual_test/motor_test.c`.
2. Run `individual_test/forward_motion_test.c`.
3. Run `individual_test/west_alignment_test.c`.
4. Run `individual_test/line_sensor_test.c`.
5. Run `individual_test/light_sensor_test.c`.
6. Run `individual_test/compass_test.c`.
7. Run `functionality_testing/ball_deposit_test.c`.
8. Run `functionality_testing/boundary_check_test.c`.
9. Run `past_codes/main_competition.c` (includes searching/collecting/depositing phase files).
10. Run `competition_main.c` if you want the non-split integrated version.

## Debugging Notes

- If robot turns wrong way, swap signs in wheel helper functions.
- If gate opens in wrong direction, swap `GATE_OPEN_SPEED` and `GATE_CLOSE_SPEED`.
- Compass logic is active-low in these scripts: `0 = direction active`.
- Line sensor logic is active-low in these scripts: `0 = boundary detected`, `1 = normal`.
- If ball detection is unstable, tune sensor thresholds first before changing logic.
