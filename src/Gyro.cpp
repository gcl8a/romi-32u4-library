#include "Gyro.h"

Gyro::Gyro() {}

void Gyro::setup()
{
  Wire.begin();

  if (!imu.init())
  {
    // Failed to detect the LSM6.
    while (1)
    {
      Serial.println(F("Failed to detect the LSM6."));
      delay(100);
    }
  }
  imu.enableDefault();
  imu.setFullScaleGyro(LSM6::GYRO_FS1000);
  imu.setGyroDataOutputRate(LSM6::GYRO_ODR104);

  reset();
}

void Gyro::reset()
{
  // Sample the z velocities
  long sum = 0;
  int samples = 2001;
  for (int i = 0; i < samples; i++)
  {
    imu.readGyro();
    sum += imu.g.z;
    delay(10);
  }
  average = sum / samples;
  heading = 0.0;
  gyroTimer = millis() + 10;
  lastTime = 0;
  Serial.print("Average: ");
  Serial.println(average);
}

float Gyro::getHeading() { return heading; }

void Gyro::loop()
{
  if (imu.getStatus() & 0x02) 
  {
    imu.readGyro();
    heading += (imu.g.z - average) * 0.035 / 104;
  }
}
