#pragma config(Sensor, dgtl8, led1, sensorDigitalOut)
#pragma config(Sensor, dgtl9, led2, sensorDigitalOut)
#pragma config(Sensor, dgtl10, led3, sensorDigitalOut)

task main()
{
    while(true)
    {
        // Step 1
        SensorValue[led1] = 1;
        SensorValue[led2] = 0;
        SensorValue[led3] = 0;
        wait1Msec(500);

        // Step 2
        SensorValue[led1] = 0;
        SensorValue[led2] = 1;
        SensorValue[led3] = 0;
        wait1Msec(500);

        // Step 3
        SensorValue[led1] = 0;
        SensorValue[led2] = 0;
        SensorValue[led3] = 1;
        wait1Msec(500);
    }
}
