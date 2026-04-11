/*
  searching_phase.h
  Searching phase logic.
  Requires the including file to define before this include:
    setDrive(int leftPower, int rightPower)
    stopDrive()
    setDebugLeds(int yellowOn, int redOn, int greenOn)
*/

const int FRONT_LOWER_BALL_MIN        = 1000;
const int FRONT_UPPER_BALL_MAX        = 700;
const int SEARCH_FORWARD_MS           = 1000;
const int SEARCH_PRE_ALIGN_SCAN_MS    = 2000;
const int SEARCH_ALIGN_TURN_POWER     = 45;
const int SEARCH_ALIGN_REVERSE_MS     = 50;
const int SEARCH_MAX_CYCLES           = 4;
const int SEARCH_ALIGN_TIMEOUT_MS     = 5000;
const int BALL_CONFIRM_SAMPLES        = 5;
const int BALL_DELTA_MIN              = 500;
const int UPPER_SAFETY_BLOCK_MIN      = 1800;
const int SEARCH_SAFETY_REVERSE_MS    = 700;
const int SEARCH_SAFETY_REVERSE_POWER = 100;
const int DRIVE_FORWARD_POWER         = 85;

const int SEARCH_RESULT_NO_BALL    = 0;
const int SEARCH_RESULT_BALL_FOUND = 1;

int searchLowerTriggered = 0;
int searchLowerPeak      = 0;
int searchLowerSum       = 0;
int searchLowerSamples   = 0;

int isBallCandidate() {
  int lower = SensorValue[frontBelowLight];
  int upper = SensorValue[frontUpperLight];

  // Strict search-phase ball detection rule:
  // detect only if:
  // 1) lower > FRONT_LOWER_BALL_MIN
  // 2) upper < FRONT_UPPER_BALL_MAX
  // 3) (lower - upper) >= BALL_DELTA_MIN
  return (lower > FRONT_LOWER_BALL_MIN &&
          upper < FRONT_UPPER_BALL_MAX &&
          (lower - upper) >= BALL_DELTA_MIN);
}

int isFrontLowerTriggered() {
  return SensorValue[frontBelowLight] > FRONT_LOWER_BALL_MIN;
}

int isFacingWest() {
  return (SensorValue[west] == 1 &&
          SensorValue[south] == 1 &&
          SensorValue[east] == 0 &&
          SensorValue[north] == 1);
}

void resetSearchLowerTracking() {
  searchLowerTriggered = 0;
  searchLowerPeak      = 0;
  searchLowerSum       = 0;
  searchLowerSamples   = 0;
}

void updateSearchLowerTracking() {
  int lower = SensorValue[frontBelowLight];
  if (lower > searchLowerPeak) searchLowerPeak = lower;
  searchLowerSum += lower;
  searchLowerSamples++;
  if (isFrontLowerTriggered()) searchLowerTriggered = 1;
}

int handleUpperSafetyReverse() {
  if (SensorValue[frontUpperLight] <= UPPER_SAFETY_BLOCK_MIN) {
    return 0;
  }

  writeDebugStreamLine(
    "SEARCH safety reverse: upper=%d (> %d)",
    SensorValue[frontUpperLight],
    UPPER_SAFETY_BLOCK_MIN
  );
  reverseStraight(SEARCH_SAFETY_REVERSE_POWER);
  wait1Msec(SEARCH_SAFETY_REVERSE_MS);
  stopDrive();
  return 1;
}

int runForwardAndCheckBall(int durationMs) {
  int startTime = nSysTime;
  int ballConfirmCount = 0;

  while ((nSysTime - startTime) < durationMs) {
    updateSearchLowerTracking();

    if (handleUpperSafetyReverse()) {
      ballConfirmCount = 0;
      continue;
    }

    if (isBallCandidate()) {
      ballConfirmCount++;
      if (ballConfirmCount >= BALL_CONFIRM_SAMPLES) {
        stopDrive();
        return 1;
      }
    } else {
      ballConfirmCount = 0;
    }

    setDrive(DRIVE_FORWARD_POWER, DRIVE_FORWARD_POWER);
    wait1Msec(20);
  }

  stopDrive();
  return 0;
}

int rotateCCWForScanAndCheckBall(int durationMs) {
  int startTime = nSysTime;
  int ballConfirmCount = 0;

  while ((nSysTime - startTime) < durationMs) {
    updateSearchLowerTracking();

    if (handleUpperSafetyReverse()) {
      ballConfirmCount = 0;
      continue;
    }

    if (isBallCandidate()) {
      ballConfirmCount++;
      if (ballConfirmCount >= BALL_CONFIRM_SAMPLES) {
        stopDrive();
        return 1;
      }
    } else {
      ballConfirmCount = 0;
    }

    turnLeftInPlace(SEARCH_ALIGN_TURN_POWER);
    wait1Msec(20);
  }

  stopDrive();
  return 0;
}

int rotateCCWToWestAndCheckBall(int timeoutMs) {
  int startTime = nSysTime;
  int ballConfirmCount = 0;

  while ((nSysTime - startTime) < timeoutMs) {
    updateSearchLowerTracking();

    if (handleUpperSafetyReverse()) {
      ballConfirmCount = 0;
      continue;
    }

    if (isBallCandidate()) {
      ballConfirmCount++;
      if (ballConfirmCount >= BALL_CONFIRM_SAMPLES) {
        stopDrive();
        return 1;
      }
    } else {
      ballConfirmCount = 0;
    }

    if (isFacingWest()) {
      // Counter rotation offsets motor stop lag so heading does not overshoot WEST.
      setDrive(-SEARCH_ALIGN_TURN_POWER, SEARCH_ALIGN_TURN_POWER);
      wait1Msec(SEARCH_ALIGN_REVERSE_MS);
      stopDrive();
      return 0;
    }

    turnLeftInPlace(SEARCH_ALIGN_TURN_POWER);
    wait1Msec(10);
  }

  stopDrive();
  return 0;
}

int runSearchingPhase() {
  int cycle = 0;

  // Searching in progress: yellow ON.
  setDebugLeds(1, 0, 0);
  writeDebugStreamLine("SEARCH phase started (one-shot, max %d cycles).", SEARCH_MAX_CYCLES);

  for (cycle = 0; cycle < SEARCH_MAX_CYCLES; cycle++) {
    resetSearchLowerTracking();
    writeDebugStreamLine("SEARCH cycle %d/%d", cycle + 1, SEARCH_MAX_CYCLES);

    if (runForwardAndCheckBall(SEARCH_FORWARD_MS)) {
      writeDebugStreamLine("Ball detected during forward search.");
      return SEARCH_RESULT_BALL_FOUND;
    }

    if (rotateCCWForScanAndCheckBall(SEARCH_PRE_ALIGN_SCAN_MS)) {
      writeDebugStreamLine("Ball detected during pre-alignment scan rotation.");
      return SEARCH_RESULT_BALL_FOUND;
    }

    if (rotateCCWToWestAndCheckBall(SEARCH_ALIGN_TIMEOUT_MS)) {
      writeDebugStreamLine("Ball detected while aligning to WEST.");
      return SEARCH_RESULT_BALL_FOUND;
    }

    if (searchLowerSamples > 0) {
      writeDebugStreamLine(
        "SEARCH lower-tracking: triggered=%d peak=%d avg=%d",
        searchLowerTriggered,
        searchLowerPeak,
        (searchLowerSum / searchLowerSamples)
      );
    }
  }

  writeDebugStreamLine("SEARCH finished all %d cycles with no ball.", SEARCH_MAX_CYCLES);
  return SEARCH_RESULT_NO_BALL;
}
