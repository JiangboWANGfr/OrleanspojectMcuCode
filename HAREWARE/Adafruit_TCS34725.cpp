/*!
 *  @file Adafruit_TCS34725.cpp
 *
 *  @mainpage Driver for the TCS34725 digital color sensors.
 *
 *  @section intro_sec Introduction
 *
 *  Adafruit invests time and resources providing this open source code,
 *  please support Adafruit and open-source hardware by purchasing
 *  products from Adafruit!
 *
 *  @section author Author
 *
 *  KTOWN (Adafruit Industries)
 *
 *  @section license License
 *
 *  BSD (see license.txt)
 *
 *  @section HISTORY
 *
 *  v1.0 - First release
 */

#include <math.h>
#include <stdlib.h>
#include "i2c.h"
#include "Adafruit_TCS34725.h"

extern I2C_HandleTypeDef hi2c1;

#define TRUE  1
#define FALSE 0

static I2C_HandleTypeDef *_hi2c;
static uint8_t _i2caddr;
static uint8_t _tcs34725Initialised = false;
static tcs34725IntegrationTime_t _tcs34725IntegrationTime = TCS34725_INTEGRATIONTIME_700MS;
static tcs34725Gain_t _tcs34725Gain = TCS34725_GAIN_1X;


 
float mypowf(const float x, const float y) {
  return (float)(pow((double)x, (double)y));
}

void write8(uint8_t reg, uint32_t value) {
    uint8_t data =value & 0xFF;
    if(HAL_I2C_Mem_Write(&hi2c1, (uint16_t)_i2caddr<<1, TCS34725_COMMAND_BIT | reg, 1, &data, 1, 100) != HAL_OK){
        while(1);
    }
}

uint8_t read8(uint8_t reg) {
    uint8_t data =0;
    if(HAL_I2C_Mem_Read(&hi2c1, (uint16_t)_i2caddr<<1, TCS34725_COMMAND_BIT | reg, 1, &data, 1, 100) != HAL_OK){
        while(1);
    }
    return data;
}


uint16_t read16(uint8_t reg) {
  uint16_t x;
  uint16_t t;
  uint8_t data[2];
  if(HAL_I2C_Mem_Read(_hi2c, (uint16_t)_i2caddr<<1, TCS34725_COMMAND_BIT | reg, 1, data, 2, 100) != HAL_OK){
      while(1);
  }
  t = (uint16_t) data[0];
  x = (uint16_t) data[1];
  x <<= 8;
  x |= t;
  return x;
}


void enable() {
  write8(TCS34725_ENABLE, TCS34725_ENABLE_PON);
  HAL_Delay(3);
  write8(TCS34725_ENABLE, TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN);

  switch (_tcs34725IntegrationTime) {
  case TCS34725_INTEGRATIONTIME_2_4MS:
    HAL_Delay(3);
    break;
  case TCS34725_INTEGRATIONTIME_24MS:
    HAL_Delay(24);
    break;
  case TCS34725_INTEGRATIONTIME_50MS:
    HAL_Delay(50);
    break;
  case TCS34725_INTEGRATIONTIME_101MS:
    HAL_Delay(101);
    break;
  case TCS34725_INTEGRATIONTIME_154MS:
    HAL_Delay(154);
    break;
  case TCS34725_INTEGRATIONTIME_700MS:
    HAL_Delay(700);
    break;
  }
}


void disable() {
  /* Turn the device off to save power */
  uint8_t reg = 0;
  reg = read8(TCS34725_ENABLE);
  write8(TCS34725_ENABLE, reg & ~(TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN));
}



bool begin() {
  _i2caddr = TCS34725_ADDRESS;
  _hi2c = &hi2c1;

  return init();
}


bool init() {

  /* Make sure we're actually connected */

  uint8_t x = read8(TCS34725_ID);
  if ((x != 0x44) && (x != 0x10)) {
    return false;
  }
  _tcs34725Initialised = true;

  /* Set default integration time and gain */
  setIntegrationTime(_tcs34725IntegrationTime);
  setGain(_tcs34725Gain);

  /* Note: by default, the device is in power down mode on bootup */
  enable();

  return true;
}

/*!
 *  @brief  Sets the integration time for the TC34725
 *  @param  it
 *          Integration Time
 */
void setIntegrationTime(tcs34725IntegrationTime_t it) {
  if (!_tcs34725Initialised)
    begin();

  /* Update the timing register */
  write8(TCS34725_ATIME, it);

  /* Update value placeholders */
  _tcs34725IntegrationTime = it;
}


void setGain(tcs34725Gain_t gain) {
  if (!_tcs34725Initialised)
    begin();

  /* Update the timing register */
  write8(TCS34725_CONTROL, gain);

  /* Update value placeholders */
  _tcs34725Gain = gain;
}


void getRawData(uint16_t *r, uint16_t *g, uint16_t *b,
                                   uint16_t *c) {
  if (!_tcs34725Initialised)
    begin();

  *c = read16(TCS34725_CDATAL);
  *r = read16(TCS34725_RDATAL);
  *g = read16(TCS34725_GDATAL);
  *b = read16(TCS34725_BDATAL);

  /* Set a delay for the integration time */
  switch (_tcs34725IntegrationTime) {
  case TCS34725_INTEGRATIONTIME_2_4MS:
    HAL_Delay(3);
    break;
  case TCS34725_INTEGRATIONTIME_24MS:
    HAL_Delay(24);
    break;
  case TCS34725_INTEGRATIONTIME_50MS:
    HAL_Delay(50);
    break;
  case TCS34725_INTEGRATIONTIME_101MS:
    HAL_Delay(101);
    break;
  case TCS34725_INTEGRATIONTIME_154MS:
    HAL_Delay(154);
    break;
  case TCS34725_INTEGRATIONTIME_700MS:
    HAL_Delay(700);
    break;
  }
}


void getRawDataOneShot(uint16_t *r, uint16_t *g, uint16_t *b,
                                          uint16_t *c) {
  if (!_tcs34725Initialised)
    begin();

  enable();
  getRawData(r, g, b, c);
  disable();
}


void getRGB(float *r, float *g, float *b) {
  uint16_t red, green, blue, clear;
  getRawData(&red, &green, &blue, &clear);
  uint32_t sum = clear;

  // Avoid divide by zero errors ... if clear = 0 return black
  if (clear == 0) {
    *r = *g = *b = 0;
    return;
  }

  *r = (float)red / sum * 255.0;
  *g = (float)green / sum * 255.0;
  *b = (float)blue / sum * 255.0;
}


uint16_t calculateColorTemperature(uint16_t r, uint16_t g,
                                                      uint16_t b) {
  float X, Y, Z; /* RGB to XYZ correlation      */
  float xc, yc;  /* Chromaticity co-ordinates   */
  float n;       /* McCamy's formula            */
  float cct;

  if (r == 0 && g == 0 && b == 0) {
    return 0;
  }

  /* 1. Map RGB values to their XYZ counterparts.    */
  /* Based on 6500K fluorescent, 3000K fluorescent   */
  /* and 60W incandescent values for a wide range.   */
  /* Note: Y = Illuminance or lux                    */
  X = (-0.14282F * r) + (1.54924F * g) + (-0.95641F * b);
  Y = (-0.32466F * r) + (1.57837F * g) + (-0.73191F * b);
  Z = (-0.68202F * r) + (0.77073F * g) + (0.56332F * b);

  /* 2. Calculate the chromaticity co-ordinates      */
  xc = (X) / (X + Y + Z);
  yc = (Y) / (X + Y + Z);

  /* 3. Use McCamy's formula to determine the CCT    */
  n = (xc - 0.3320F) / (0.1858F - yc);

  /* Calculate the final CCT */
  cct = (449.0F * mypowf(n, 3)) + (3525.0F * mypowf(n, 2)) + (6823.3F * n) + 5520.33F;

  /* Return the results in degrees Kelvin */
  return (uint16_t)cct;
}


uint16_t calculateColorTemperature_dn40(uint16_t r,
                                                           uint16_t g,
                                                           uint16_t b,
                                                           uint16_t c) {
  uint16_t r2, b2; /* RGB values minus IR component */
  uint16_t sat;    /* Digital saturation level */
  uint16_t ir;     /* Inferred IR content */

  if (c == 0) {
    return 0;
  }

  if ((256 - _tcs34725IntegrationTime) > 63) {
    /* Track digital saturation */
    sat = 65535;
  } else {
    /* Track analog saturation */
    sat = 1024 * (256 - _tcs34725IntegrationTime);
  }

 
  if ((256 - _tcs34725IntegrationTime) <= 63) {
    /* Adjust sat to 75% to avoid analog saturation if atime < 153.6ms */
    sat -= sat / 4;
  }

  /* Check for saturation and mark the sample as invalid if true */
  if (c >= sat) {
    return 0;
  }

  /* AMS RGB sensors have no IR channel, so the IR content must be */
  /* calculated indirectly. */
  ir = (r + g + b > c) ? (r + g + b - c) / 2 : 0;

  /* Remove the IR component from the raw RGB values */
  r2 = r - ir;
  b2 = b - ir;

  if (r2 == 0) {
    return 0;
  }

  /* A simple method of measuring color temp is to use the ratio of blue */
  /* to red light, taking IR cancellation into account. */
  uint16_t cct = (3810 * (uint32_t)b2) / /** Color temp coefficient. */
                     (uint32_t)r2 +
                 1391; /** Color temp offset. */

  return cct;
}


uint16_t calculateLux(uint16_t r, uint16_t g, uint16_t b) {
  float illuminance;

  /* This only uses RGB ... how can we integrate clear or calculate lux */
  /* based exclusively on clear since this might be more reliable?      */
  illuminance = (-0.32466F * r) + (1.57837F * g) + (-0.73191F * b);

  return (uint16_t)illuminance;
}

/*!
 *  @brief  Sets inerrupt for TCS34725
 *  @param  i
 *          Interrupt (True/False)
 */
void setInterrupt(bool i) {
  uint8_t r = read8(TCS34725_ENABLE);
  if (i) {
    r |= TCS34725_ENABLE_AIEN;
  } else {
    r &= ~TCS34725_ENABLE_AIEN;
  }
  write8(TCS34725_ENABLE, r);
}

/*!
 *  @brief  Clears inerrupt for TCS34725
 */
void clearInterrupt() {
    uint8_t data = TCS34725_COMMAND_BIT | 0x66;
    if(HAL_I2C_Master_Transmit(_hi2c, (uint16_t)_i2caddr<<1, &data, 1, 100) != HAL_OK){
        while(1);
    }
}

/*!
 *  @brief  Sets inerrupt limits
 *  @param  low
 *          Low limit
 *  @param  high
 *          High limit
 */
void setIntLimits(uint16_t low, uint16_t high) {
  write8(0x04, low & 0xFF);
  write8(0x05, low >> 8);
  write8(0x06, high & 0xFF);
  write8(0x07, high >> 8);
}
