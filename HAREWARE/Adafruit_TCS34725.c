#include "Adafruit_TCS34725.h"
#include "math.h"

uint8_t dataT[3]= {0x13,1,0};
uint8_t dataR[3]= {0,0,0};
uint16_t timeOut = 1;

int _tcs34725Initialised;
tcs34725Gain_t _tcs34725Gain;
tcs34725IntegrationTime_t _tcs34725IntegrationTime; 

void tcs3272_init( void )
{
	int testSensor= 0;
	
	testSensor=begin();

}

uint16_t tcs3472_test( void) 
{
	uint16_t r, g, b, c, colorTemp, lux;
	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
	tcs3272_init();
	
	getRawData(&r, &g, &b, &c);
	colorTemp = calculateColorTemperature(r, g, b);
	lux = calculateLux(r, g, b);
	
	HAL_Delay(500);
	return lux;

}

void write8 (uint8_t reg, uint32_t value)
{
	uint8_t pkt[2];
	
  pkt[0] = (TCS34725_COMMAND_BIT | reg);
  pkt[1] = (value & 0xFF);
	/////////////////
	HAL_I2C_Master_Transmit(&hi2c1, TCS34725_ADDRESS, pkt, 2, timeOut);
}

/**************************************************************************/
/*!
    @brief  Reads an 8 bit value over I2C
*/
/**************************************************************************/
uint8_t read8(uint8_t reg)
{
	
	uint8_t pkt[2];
	
  pkt[0] = (TCS34725_COMMAND_BIT | reg);
	
	HAL_I2C_Master_Transmit(&hi2c1, TCS34725_ADDRESS, pkt, 1, timeOut);
  
	HAL_I2C_Master_Receive(&hi2c1, TCS34725_ADDRESS, pkt, 1, timeOut);
	
	return pkt[0];
}

/**************************************************************************/
/*!
    @brief  Reads a 16 bit values over I2C
*/
/**************************************************************************/
uint16_t read16(uint8_t reg)
{
  uint16_t x;
	uint8_t Tpkt[2],Rpkt[2];

  Tpkt[0] = (TCS34725_COMMAND_BIT | reg);
	HAL_I2C_Master_Transmit(&hi2c1, TCS34725_ADDRESS, Tpkt, 1, timeOut);
  
	
	HAL_I2C_Master_Receive(&hi2c1, TCS34725_ADDRESS, Rpkt, 2, timeOut);
	
	x = Rpkt[0];
  x <<= 8;
  x |= Rpkt[1];
	
  return x;
}

void enable(void)
{
  write8(TCS34725_ENABLE, TCS34725_ENABLE_PON);
  write8(TCS34725_ENABLE, TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN);  
}

void disable(void)
{
  /* Turn the device off to save power */
  uint8_t reg = 0;
  reg = read8(TCS34725_ENABLE);
  write8(TCS34725_ENABLE, reg & ~(TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN));
}


int begin(void) 
{
  /* Make sure we're actually connected */
  uint8_t x = read8(TCS34725_ID);
  if ((x != 0x44) && (x != 0x10))
  {
    return false;
  }
  _tcs34725Initialised = true;

  /* Set default integration time and gain */
  setIntegrationTime(TCS34725_INTEGRATIONTIME_2_4MS);
  setGain(_tcs34725Gain);

  /* Note: by default, the device is in power down mode on bootup */
  enable();

  return true;
}

void setIntegrationTime(tcs34725IntegrationTime_t it)
{
  if (!_tcs34725Initialised) begin();

  /* Update the timing register */
  write8(TCS34725_ATIME, it);

  /* Update value placeholders */
  _tcs34725IntegrationTime = it;
}

void setGain(tcs34725Gain_t gain)
{
  if (!_tcs34725Initialised) begin();

  /* Update the timing register */
  write8(TCS34725_CONTROL, gain);

  /* Update value placeholders */
  _tcs34725Gain = gain;
}

void getRawData (uint16_t *r, uint16_t *g, uint16_t *b, uint16_t *c)
{
  if (!_tcs34725Initialised) begin();

  *c = read16(TCS34725_CDATAL);
  *r = read16(TCS34725_RDATAL);
  *g = read16(TCS34725_GDATAL);
  *b = read16(TCS34725_BDATAL);
  
  /* Set a delay for the integration time */
  switch (_tcs34725IntegrationTime)
  {
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

void getRGB(uint16_t *r, uint16_t *g, uint16_t *b) {
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


uint16_t calculateColorTemperature(uint16_t r, uint16_t g, uint16_t b)
{
  float X, Y, Z;      /* RGB to XYZ correlation      */
  float xc, yc;       /* Chromaticity co-ordinates   */
  float n;            /* McCamy's formula            */
  float cct;

  /* 1. Map RGB values to their XYZ counterparts.    */
  /* Based on 6500K fluorescent, 3000K fluorescent   */
  /* and 60W incandescent values for a wide range.   */
  /* Note: Y = Illuminance or lux                    */
  X = (-0.14282F * r) + (1.54924F * g) + (-0.95641F * b);
  Y = (-0.32466F * r) + (1.57837F * g) + (-0.73191F * b);
  Z = (-0.68202F * r) + (0.77073F * g) + ( 0.56332F * b);

  /* 2. Calculate the chromaticity co-ordinates      */
  xc = (X) / (X + Y + Z);
  yc = (Y) / (X + Y + Z);

  /* 3. Use McCamy's formula to determine the CCT    */
  n = (xc - 0.3320F) / (0.1858F - yc);

  /* Calculate the final CCT */
  cct = (449.0F * powf(n, 3)) + (3525.0F * powf(n, 2)) + (6823.3F * n) + 5520.33F;

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

  /* Analog/Digital saturation:
   *
   * (a) As light becomes brighter, the clear channel will tend to
   *     saturate first since R+G+B is approximately equal to C.
   * (b) The TCS34725 accumulates 1024 counts per 2.4ms of integration
   *     time, up to a maximum values of 65535. This means analog
   *     saturation can occur up to an integration time of 153.6ms
   *     (64*2.4ms=153.6ms).
   * (c) If the integration time is > 153.6ms, digital saturation will
   *     occur before analog saturation. Digital saturation occurs when
   *     the count reaches 65535.
   */
  if ((256 - _tcs34725IntegrationTime) > 63) {
    /* Track digital saturation */
    sat = 65535;
  } else {
    /* Track analog saturation */
    sat = 1024 * (256 - _tcs34725IntegrationTime);
  }

  /* Ripple rejection:
   *
   * (a) An integration time of 50ms or multiples of 50ms are required to
   *     reject both 50Hz and 60Hz ripple.
   * (b) If an integration time faster than 50ms is required, you may need
   *     to average a number of samples over a 50ms period to reject ripple
   *     from fluorescent and incandescent light sources.
   *
   * Ripple saturation notes:
   *
   * (a) If there is ripple in the received signal, the value read from C
   *     will be less than the max, but still have some effects of being
   *     saturated. This means that you can be below the 'sat' value, but
   *     still be saturating. At integration times >150ms this can be
   *     ignored, but <= 150ms you should calculate the 75% saturation
   *     level to avoid this problem.
   */
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

/**************************************************************************/
/*!
    @brief  Converts the raw R/G/B values to lux
*/
/**************************************************************************/
uint16_t calculateLux(uint16_t r, uint16_t g, uint16_t b)
{
  float illuminance;

  /* This only uses RGB ... how can we integrate clear or calculate lux */
  /* based exclusively on clear since this might be more reliable?      */
  illuminance = (-0.32466F * r) + (1.57837F * g) + (-0.73191F * b);

  return (uint16_t)illuminance;
}



void setInterrupt(int i) {
  uint8_t r = read8(TCS34725_ENABLE);
  if (i) {
    r |= TCS34725_ENABLE_AIEN;
  } else {
    r &= ~TCS34725_ENABLE_AIEN;
  }
  write8(TCS34725_ENABLE, r);
}

void clearInterrupt(void) {
  
	uint8_t pkt[2];
	
  pkt[0] = (TCS34725_COMMAND_BIT | 0x66);
 
	/////////////////
	HAL_I2C_Master_Transmit(&hi2c1, TCS34725_ADDRESS, pkt, 1, timeOut);
	
}

void setIntLimits(uint16_t low, uint16_t high) {
   write8(0x04, low & 0xFF);
   write8(0x05, low >> 8);
   write8(0x06, high & 0xFF);
   write8(0x07, high >> 8);
}
