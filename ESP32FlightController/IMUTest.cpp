#include "FastIMU.h"
#include "Madgwick.h"
#include <Wire.h>

#define IMU_ADDRESS 0x68
#define PERFORM_CALIBRATION 
MPU6500 IMU;                

calData calib = {0};
AccelData IMUAccel;
GyroData IMUGyro;
MagData IMUMag;
Madgwick filter;
double qw, qx, qy, qz;
double yaw, roll, pitch;
double rad2deg = 180 / 3.14;

void setup()
{
    Wire.begin();
    Wire.setClock(400000); // 400khz clock

    Serial.begin(115200);
    while (!Serial)
    {
        ;
    }

    int err = IMU.init(calib, IMU_ADDRESS);
    if (err != 0)
    {
        Serial.print("Error initializing IMU: ");
        Serial.println(err);
        while (true)
        {
            ;
        }
    }

    if (err != 0)
    {
        Serial.print("Error Setting range: ");
        Serial.println(err);
        while (true)
        {
            ;
        }
    }

#ifdef PERFORM_CALIBRATION
    Serial.println("FastIMU Calibrated Quaternion example");
    if (IMU.hasMagnetometer())
    {
        delay(1000);
        Serial.println("Move IMU in figure 8 pattern until done.");
        delay(3000);
        IMU.calibrateMag(&calib);
        Serial.println("Magnetic calibration done!");
    }
    else
    {
        delay(1000);
    }
    Serial.println("Keep IMU level.");
    delay(5000);
    IMU.calibrateAccelGyro(&calib);
    Serial.println("Calibration done!");
    Serial.println("Accel biases X/Y/Z: ");
    Serial.print(calib.accelBias[0]);
    Serial.print(", ");
    Serial.print(calib.accelBias[1]);
    Serial.print(", ");
    Serial.println(calib.accelBias[2]);
    Serial.println("Gyro biases X/Y/Z: ");
    Serial.print(calib.gyroBias[0]);
    Serial.print(", ");
    Serial.print(calib.gyroBias[1]);
    Serial.print(", ");
    Serial.println(calib.gyroBias[2]);
    if (IMU.hasMagnetometer())
    {
        Serial.println("Mag biases X/Y/Z: ");
        Serial.print(calib.magBias[0]);
        Serial.print(", ");
        Serial.print(calib.magBias[1]);
        Serial.print(", ");
        Serial.println(calib.magBias[2]);
        Serial.println("Mag Scale X/Y/Z: ");
        Serial.print(calib.magScale[0]);
        Serial.print(", ");
        Serial.print(calib.magScale[1]);
        Serial.print(", ");
        Serial.println(calib.magScale[2]);
    }
    delay(5000);
    IMU.init(calib, IMU_ADDRESS);

    filter.begin(0.2f);
#endif
}

void loop()
{
    IMU.update();
    IMU.getAccel(&IMUAccel);
    IMU.getGyro(&IMUGyro);
    if (IMU.hasMagnetometer())
    {
        IMU.getMag(&IMUMag);
        filter.update(IMUGyro.gyroX, IMUGyro.gyroY, IMUGyro.gyroZ, IMUAccel.accelX, IMUAccel.accelY, IMUAccel.accelZ, IMUMag.magX, IMUMag.magY, IMUMag.magZ);
    }
    else
    {
        filter.updateIMU(IMUGyro.gyroX, IMUGyro.gyroY, IMUGyro.gyroZ, IMUAccel.accelX, IMUAccel.accelY, IMUAccel.accelZ);
    }
    qw = filter.getQuatW();
    qx = filter.getQuatX();
    qy = filter.getQuatY();
    qz = filter.getQuatZ();

    yaw = atan2(2.0 * (qx * qy + qw * qz), qw * qw + qx * qx - qy * qy - qz * qz) * rad2deg;
    pitch = atan2(2.0 * (qy * qz + qw * qx), qw * qw - qx * qx - qy * qy + qz * qz) * -rad2deg;
    roll = asin(-2.0 * (qx * qz - qw * qy)) * -rad2deg;
    Serial.print(roll); // ROLL
    Serial.print(";");
    Serial.print(pitch); // PITCH
    Serial.print(";");
    Serial.print(yaw); // YAW
    Serial.print(";");
    Serial.println(); 


    delay(50);
}