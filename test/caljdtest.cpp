// arduino header file
#include <Arduino.h>

#include <DS3231.h>

// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);

#include "caljd.h"

/**
 * setup all here
 * @method setup
 */
void setup() {

  Serial.begin(115200);

  Serial.println("CAL2JD");
  Serial.println(cal_to_jd(2017, 4, 26, 16, 43, 0).toInt());
  Serial.println(cal_to_jd(2011, 4, 26, 22, 43, 30));
  Serial.println(cal_to_jd_from_time_t(rtc.getTime()));
}

/**
 * main program loop
 * @method loop
 */
void loop() {}
