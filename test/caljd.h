#ifndef _CALJD_H_
#define _CALJD_H_

// Library
#include <Arduino.h>

// ds3231
#include <DS3231.h>

// standar library
#include <math.h>
#include <stdint.h>

// proto function
String cal_to_jd(int16_t y, uint8_t m, uint8_t d, uint8_t h, uint8_t mn,
                 uint8_t s);
uint32_t cal_to_jd_from_time_t(Time tin);

/**
 * convert gregorian date to julian date
 * @method cal_to_jd
 * @param  y         year, minus is before century
 * @param  m         month , 1-12
 * @param  d         day, 1-31
 * @param  h         hour, 0-23
 * @param  mn        minute, 0-59
 * @param  s         seconds, 0-59
 * @return           String
 */
String cal_to_jd(int16_t y, uint8_t m, uint8_t d, uint8_t h, uint8_t mn,
                 uint8_t s) {
  float ya = (float)y, jy, jm, ja;

  // no year = 0 in julian
  if (y == 0) {
    return String(-1.0);
  }

  // The dates 5 through 14 October, 1582, do not exist in the Gregorian system!
  if (y == 1582 && m == 10 && d > 4 && d < 15) {
    return String(-1.0);
  }

  // before century
  if (y < 0) {
    ya = (float)y + 1.0;
  }

  // month
  if (m > 2) {
    jy = ya;
    jm = (float)m + 1.0;
  } else {
    jy = ya - 1.0;
    jm = (float)m + 13.0;
  }

  float intgr =
      floor(floor(365.25 * jy) + floor(30.6001 * jm) + (float)d + 1720995.0);

  // check for switch to Gregorian calendar
  float gregcal = 15.0 + 31.0 * (10.0 + 12.0 * 1582.0);
  if ((float)d + 31.0 * ((float)m + 12.0 * ya) >= gregcal) {
    ja = floor(0.01 * jy);
    intgr += 2.0 - ja + floor(0.25 * ja);
  }

  // correct for half-day offset
  float dayfrac = ((float)h / 24.0) - 0.5;
  if (dayfrac < 0.0) {
    dayfrac += 1.0;
    intgr -= 1.0;
  }

  // now set the fraction of a day
  float frac = dayfrac + ((((float)mn + ((float)s / 60.0)) / 60.0) / 24.0);

  return String(String((unsigned long)intgr) + "." +
                String((unsigned long)(frac * 1000000.0)));
}

/**
 * julian date from Time
 * @method cal_to_jd_from_time_t
 * @param  tin                   [description]
 * @return                       [description]
 */
uint32_t cal_to_jd_from_time_t(Time tin) {
  return cal_to_jd(tin.year, tin.mon, tin.date, tin.hour, tin.min, tin.sec)
      .toInt();
}

#endif
