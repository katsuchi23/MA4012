/*
  collecting_phase.h
  Collecting phase logic.
  Requires the including file to define before this include:
    setDrive(int leftPower, int rightPower)
    stopDrive()
    setStatusLeds(int yellowOn, int greenOn, int redOn)
*/

const int CENTER_BALL_COLLECT_MIN       = 1300;
const int COLLECT_TIMEOUT_MS            = 1000;
const int COLLECT_EXTRA_SPIN_TIMEOUT_MS = 5000;
const int DRIVE_COLLECT_POWER           = 90;
const int COLLECT_COLLECTOR_POWER       = 200;

const int COLLECT_RESULT_TIMEOUT = 0;
const int COLLECT_RESULT_SUCCESS = 1;

int isBallCollectedAtCenter() {
  return SensorValue[centerLight] > CENTER_BALL_COLLECT_MIN;
}

int runCollectingPhase() {
  int startTime = nSysTime;
  writeDebugStreamLine("COLLECTING started.");
  setStatusLeds(0, 0, 0);

  if (isBallCollectedAtCenter()) {
    stopDrive();
    setStatusLeds(0, 1, 0);
    writeDebugStreamLine("COLLECTING immediate success (ball already at center).");
    return COLLECT_RESULT_SUCCESS;
  }

  while ((nSysTime - startTime) < COLLECT_TIMEOUT_MS) {
    if (isBallCollectedAtCenter()) {
      stopDrive();
      setStatusLeds(0, 1, 0);
      writeDebugStreamLine("COLLECTING success before timeout.");
      return COLLECT_RESULT_SUCCESS;
    }

    setDrive(DRIVE_COLLECT_POWER, DRIVE_COLLECT_POWER);
    setStatusLeds(1, 0, 0);
    wait1Msec(20);
  }

  stopDrive();
  setStatusLeds(0, 0, 0);
  writeDebugStreamLine("COLLECTING forward timeout (1s). Extra collector spin starts.");

  startTime = nSysTime;
  motor[collectorMotor] = COLLECT_COLLECTOR_POWER;
  while ((nSysTime - startTime) < COLLECT_EXTRA_SPIN_TIMEOUT_MS) {
    if (isBallCollectedAtCenter()) {
      stopDrive();
      setStatusLeds(0, 1, 0);
      writeDebugStreamLine("COLLECTING success during extra collector spin.");
      return COLLECT_RESULT_SUCCESS;
    }
    wait1Msec(20);
  }

  setStatusLeds(0, 0, 1);
  writeDebugStreamLine("COLLECTING timeout after extra 5s collector spin.");
  return COLLECT_RESULT_TIMEOUT;
}
