// This example reads the raw values from the LSM6DS33
// accelerometer and gyro and prints those raw values to the
// serial monitor.
//
// The accelerometer readings can be converted to units of g
// using the conversion factors specified in the "Mechanical
// characteristics" table in the LSM6DS33 datasheet.  We use a
// full scale (FS) setting of +/- 16 g, so the conversion factor
// is 0.488 mg/LSB (least-significant bit).  A raw reading of
// 2048 would correspond to 1 g.
//
// The gyro readings can be converted to degrees per second (dps)
// using the "Mechanical characteristics" table in the LSM6DS33
// datasheet.  We use a full scale (FS) of +/- 1000 dps so the
// conversion factor is 35 mdps/LSB.  A raw reading of 2571
// would correspond to 90 dps.
//
// To run this sketch, you will need to install the LSM6 library:
//
// https://github.com/pololu/lsm6-arduino

#include <Romi32U4.h>

LSM6 imu;

char report[120];

void setup()
{
  Wire.begin();

  if (!imu.init())
  {
    // Failed to detect the LSM6.
    ledRed(1);
    while(1)
    {
      Serial.println(F("Failed to detect the LSM6."));
      delay(100);
    }
  }

  imu.enableDefault();

  // Set the gyro full scale to 1000 dps because the default
  // value is too low, and leave the other settings the same.
  imu.setFullScaleGyro(LSM6::GYRO_FS1000);
  imu.setGyroDataOutputRate(LSM6::GYRO_ODR52);

  // Set the accelerometer full scale to 16 g because the default
  // value is too low, and leave the other settings the same.
  imu.setFullScaleAcc(LSM6::ACC_FS16);
}

Romi32U4ButtonA buttonA;
Romi32U4ButtonB buttonB;
Romi32U4ButtonC buttonC;
Romi32U4Motors motors;

bool showAcc = true;
bool showGyro = false;


void loop()
{
  if(buttonA.isPressed()) motors.setEfforts(200, 200);
  else motors.setEfforts(0,0);

  if(buttonB.getSingleDebouncedPress()) showAcc = !showAcc;
  if(buttonC.getSingleDebouncedPress()) showGyro = !showGyro;

  if(imu.getStatus() & 0x02)
  {
    imu.read();

    if(showAcc)
    {
      Serial.print(imu.a.x);
      Serial.print(' ');
      Serial.print(imu.a.y);
      Serial.print(' ');
      Serial.print(imu.a.z);
      Serial.print(' ');
    }

    if(showGyro)
    {
      Serial.print(imu.g.x);
      Serial.print(' ');
      Serial.print(imu.g.y);
      Serial.print(' ');
      Serial.print(imu.g.z);
      Serial.print(' ');
    }

    Serial.print('\n');
  }
}
