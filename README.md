# MA4012 Robot Scripts (VEX Cortex / RobotC)

This repo now has a clean script set for your competition robot:

- `individual_test/` for single-component debugging.
- `functionality_testing/` for behavior debugging.
- `competition_main.c` as the integrated competition code.

All scripts assume this hardware mapping:

- Line sensors (digital): `dgtl1` front-left, `dgtl2` front-right, `dgtl3` back-left, `dgtl4` back-right.
- Light sensors (analog): `in1` front-below, `in2` front-upper, `in3` center, `in4` back.
- Compass (digital): `dgtl9` LSB, `dgtl10` bit1, `dgtl11` bit2, `dgtl12` MSB.
- Unused digital reserved: `dgtl5`-`dgtl8` as `sensorNone`.
- Motors: `port1` right wheel, `port2` left wheel, `port3` collector, `port4` gate.

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
  - Prints raw compass bits, decimal code, and mapped heading (N/NE/E/SE/S/SW/W/NW).
  - Rotate robot manually and verify sequence.

### `functionality_testing/`

- `ball_collection_test.c`
  - Runs collection behavior:
    - collector motor spins continuously,
    - uses front-below + front-upper to decide ball vs obstacle,
    - approaches ball,
    - stops when center sensor confirms ball inside.

- `ball_deposit_test.c`
  - Runs deposit behavior:
    - aligns robot to North using compass,
    - reverses to collection area using back line + back light sensors,
    - opens/closes gate to drop ball.

- `boundary_check_test.c`
  - Roomba-style movement:
    - drive forward continuously,
    - when line sensors trigger, reverse + recover turn,
    - continue moving.

### Main Competition Code

- `competition_main.c`
  - Integrates boundary handling, ball collection, and ball deposit into one loop.
  - This is the file intended for competition runs.

## Tuning Constants (Important)

Each script has constants at the top for easy tuning, for example:

- `FRONT_BELOW_BALL_MIN` / `FRONT_BELOW_BALL_MAX`
- `FRONT_UPPER_OBSTACLE_MIN`
- `CENTER_BALL_INSIDE_MIN`
- `BACK_COLLECTION_NEAR_MIN`
- `GATE_OPEN_SPEED` / `GATE_CLOSE_SPEED`

You should tune these on your real arena before match day.

## Recommended Bring-Up Order

1. Run `individual_test/motor_test.c`.
2. Run `individual_test/line_sensor_test.c`.
3. Run `individual_test/light_sensor_test.c`.
4. Run `individual_test/compass_test.c`.
5. Run `functionality_testing/ball_collection_test.c`.
6. Run `functionality_testing/ball_deposit_test.c`.
7. Run `functionality_testing/boundary_check_test.c`.
8. Run `competition_main.c`.

## Debugging Notes

- If robot turns wrong way, swap signs in wheel helper functions.
- If gate opens in wrong direction, swap `GATE_OPEN_SPEED` and `GATE_CLOSE_SPEED`.
- If compass headings are wrong, verify bit wiring order (`LSB` to `MSB`).
- If ball detection is unstable, tune sensor thresholds first before changing logic.
