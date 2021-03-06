// yama_2_GFX_SSD1306.cpp
// Version : 0.3
//    
//    このプログラムはAdafruit_SSD1306を元に作られています。
//    https://github.com/adafruit/Adafruit_SSD1306
//
//    Copyright (c) 2012 Adafruit Industries.
//    Released under the BSD License
//    https://github.com/adafruit/Adafruit_SSD1306/blob/master/license.txt
//
// yama_2_GFX_SSD1306 pico-sdk LCD SSD1306 Graphics Library (C++)
//                                 https://twitter.com/yama23238
//
//      これらのプログラムの使用に当たってはご自分の責任において使用してください
//      これらのプログラムで発生したいかなる損害、データの消失、金銭等の責任は一切負いません。
//


#include "yama_2_GFX_SSD1306.h"
#include "yama_2_GFX.h"

// SOME DEFINES AND STATIC VARIABLES USED INTERNALLY -----------------------


#define yama_2_GFX_SSD1306_swap(a, b)                                                     \
  (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b))) ///< No-temp-var swap operation

#define yama_2_GFX_SSD1306_PROGMEM 

#define yama_2_GFX_SSD1306_SELECT       SPI_CS_LOW();     ///< Device select
#define yama_2_GFX_SSD1306_DESELECT     SPI_CS_HIGH();    ///< Device deselect
#define yama_2_GFX_SSD1306_MODE_COMMAND SPI_DC_LOW();     ///< Command mode
#define yama_2_GFX_SSD1306_MODE_DATA    SPI_DC_HIGH();    ///< Data mode


#define yama_2_GFX_SSD1306_I2C_SETWIRECLOCK ///< Dummy stand-in define
#define yama_2_GFX_SSD1306_I2C_RESWIRECLOCK ///< keeps compiler happy

#define yama_2_GFX_SSD1306_SPI_TRANSACTION_START ///< Dummy stand-in define
#define yama_2_GFX_SSD1306_SPI_TRANSACTION_END   ///< keeps compiler happy


// The definition of 'transaction' is broadened a bit in the context of
// this library -- referring not just to SPI transactions (if supported
// in the version of the SPI library being used), but also chip select
// (if SPI is being used, whether hardware or soft), and also to the
// beginning and end of I2C transfers (the Wire clock may be sped up before
// issuing data to the display, then restored to the default rate afterward
// so other I2C device types still work).  All of these are encapsulated
// in the TRANSACTION_* macros.

// Check first if Wire, then hardware SPI, then soft SPI:
#define yama_2_GFX_SSD1306_TRANSACTION_START                           \
  if (wire) {                                                                  \
    yama_2_GFX_SSD1306_I2C_SETWIRECLOCK;                                       \
  } else {                                                                     \
    if (spi) {                                                                 \
      yama_2_GFX_SSD1306_SPI_TRANSACTION_START;                            \
    }                                                                          \
    yama_2_GFX_SSD1306_SELECT;                                             \
  } ///< Wire, SPI or bitbang transfer setup
#define yama_2_GFX_SSD1306_TRANSACTION_END                             \
  if (wire) {                                                                  \
    yama_2_GFX_SSD1306_I2C_RESWIRECLOCK;                                       \
  } else {                                                                     \
    yama_2_GFX_SSD1306_DESELECT;                                                          \
    if (spi) {                                                                 \
      yama_2_GFX_SSD1306_SPI_TRANSACTION_END;                              \
    }                                                                          \
  } ///< Wire, SPI or bitbang transfer end

// CONSTRUCTORS, DESTRUCTOR ------------------------------------------------

/*!
    @brief  Constructor for I2C-interfaced SSD1306 displays.
    @param  w
            Display width in pixels
    @param  h
            Display height in pixels
    @param  twi
            Pointer to an existing TwoWire instance (e.g. &Wire, the
            microcontroller's primary I2C bus).
    @param  rst_pin
            Reset pin (using Arduino pin numbering), or -1 if not used
            (some displays might be wired to share the microcontroller's
            reset pin).
    @param  clkDuring
            Speed (in Hz) for Wire transmissions in SSD1306 library calls.
            Defaults to 400000 (400 KHz), a known 'safe' value for most
            microcontrollers, and meets the SSD1306 datasheet spec.
            Some systems can operate I2C faster (800 KHz for ESP32, 1 MHz
            for many other 32-bit MCUs), and some (perhaps not all)
            SSD1306's can work with this -- so it's optionally be specified
            here and is not a default behavior. (Ignored if using pre-1.5.7
            Arduino software, which operates I2C at a fixed 100 KHz.)
    @param  clkAfter
            Speed (in Hz) for Wire transmissions following SSD1306 library
            calls. Defaults to 100000 (100 KHz), the default Arduino Wire
            speed. This is done rather than leaving it at the 'during' speed
            because other devices on the I2C bus might not be compatible
            with the faster rate. (Ignored if using pre-1.5.7 Arduino
            software, which operates I2C at a fixed 100 KHz.)
    @return yama_2_GFX_SSD1306 object.
    @note   Call the object's begin() function before use -- buffer
            allocation is performed there!
*/
yama_2_GFX_SSD1306::yama_2_GFX_SSD1306(uint8_t w, uint8_t h, i2c_inst_t *twi, int8_t rst_pin)

    : yama_2_GFX(w, h), spi(NULL), wire(twi), 
      buffer(NULL), dcPin(-1), csPin(-1), rstPin(rst_pin)
{
}

/*!
    @brief  Constructor for SPI SSD1306 displays, using software (bitbang)
            SPI.
    @param  w
            Display width in pixels
    @param  h
            Display height in pixels
    @param  mosi_pin
            MOSI (master out, slave in) pin (using Arduino pin numbering).
            This transfers serial data from microcontroller to display.
    @param  sclk_pin
            SCLK (serial clock) pin (using Arduino pin numbering).
            This clocks each bit from MOSI.
    @param  dc_pin
            Data/command pin (using Arduino pin numbering), selects whether
            display is receiving commands (low) or data (high).
    @param  rst_pin
            Reset pin (using Arduino pin numbering), or -1 if not used
            (some displays might be wired to share the microcontroller's
            reset pin).
    @param  cs_pin
            Chip-select pin (using Arduino pin numbering) for sharing the
            bus with other devices. Active low.
    @return yama_2_GFX_SSD1306 object.
    @note   Call the object's begin() function before use -- buffer
            allocation is performed there!
*/
yama_2_GFX_SSD1306::yama_2_GFX_SSD1306(uint8_t w, uint8_t h, spi_inst_t *_spi,
                                   int8_t dc_pin, int8_t rst_pin, int8_t cs_pin)
    : yama_2_GFX(w, h), spi(_spi), wire(NULL), buffer(NULL),
      dcPin(dc_pin), csPin(cs_pin), rstPin(rst_pin) {}

// LOW-LEVEL UTILS ---------------------------------------------------------

// Issue single byte out SPI, either soft or hardware as appropriate.
// SPI transaction/selection must be performed in calling function.
inline void yama_2_GFX_SSD1306::SPIwrite(uint8_t d) {
  if (spi) {
    spi_write_blocking(spi, &d, 1);
  }
}

// Issue single command to SSD1306, using I2C or hard/soft SPI as needed.
// Because command calls are often grouped, SPI transaction and selection
// must be started/ended in calling function for efficiency.
// This is a private function, not exposed (see ssd1306_command() instead).
void yama_2_GFX_SSD1306::ssd1306_command1(uint8_t c) {
  if (wire) { // I2C
    uint8_t buf[2];
    buf[0] = 0x00;    // Co = 0, D/C = 0
    buf[1] = c;
    i2c_write_blocking(wire, i2caddr, buf, 2, false);
  } else { // SPI (hw or soft) -- transaction started in calling function
    yama_2_GFX_SSD1306_MODE_COMMAND
    SPIwrite(c);
  }
}

// Issue list of commands to SSD1306, same rules as above re: transactions.
// This is a private function, not exposed.
void yama_2_GFX_SSD1306::ssd1306_commandList(const uint8_t *c, uint8_t n) {
  if (wire) { // I2C

    int n1, n2;
    uint8_t *m;
    const uint8_t *c1 = c;
    n1 = n / yama_2_GFX_SSD1306_I2C_WIRE_MAX;
    n2 = n - (n1 * yama_2_GFX_SSD1306_I2C_WIRE_MAX);
    m = i2c_buffer01;
    for (int i = 0; i < n1; i++) {
      *m = 0x00;
      memcpy(m + 1, c1, yama_2_GFX_SSD1306_I2C_WIRE_MAX);
      i2c_write_blocking(wire, i2caddr, m, yama_2_GFX_SSD1306_I2C_WIRE_MAX + 1, false);
      c1 += yama_2_GFX_SSD1306_I2C_WIRE_MAX;
    }
    if (n2 > 0) {
      *m = 0x00;
      memcpy(m + 1, c1, n2);
      i2c_write_blocking(wire, i2caddr, m, n2 + 1, false);
    }

  } else { // SPI -- transaction started in calling function
    yama_2_GFX_SSD1306_MODE_COMMAND
    while (n--)
      SPIwrite(*c++);
  }
}

// A public version of ssd1306_command1(), for existing user code that
// might rely on that function. This encapsulates the command transfer
// in a transaction start/end, similar to old library's handling of it.
/*!
    @brief  Issue a single low-level command directly to the SSD1306
            display, bypassing the library.
    @param  c
            Command to issue (0x00 to 0xFF, see datasheet).
    @return None (void).
*/
void yama_2_GFX_SSD1306::ssd1306_command(uint8_t c) {
  yama_2_GFX_SSD1306_TRANSACTION_START
  ssd1306_command1(c);
  yama_2_GFX_SSD1306_TRANSACTION_END
}

// ALLOCATE & INIT DISPLAY -------------------------------------------------

/*!
    @brief  Allocate RAM for image buffer, initialize peripherals and pins.
    @param  vcs
            VCC selection. Pass SSD1306_SWITCHCAPVCC to generate the display
            voltage (step up) from the 3.3V source, or SSD1306_EXTERNALVCC
            otherwise. Most situations with Adafruit SSD1306 breakouts will
            want SSD1306_SWITCHCAPVCC.
    @param  addr
            I2C address of corresponding SSD1306 display (or pass 0 to use
            default of 0x3C for 128x32 display, 0x3D for all others).
            SPI displays (hardware or software) do not use addresses, but
            this argument is still required (pass 0 or any value really,
            it will simply be ignored). Default if unspecified is 0.
    @param  reset
            If true, and if the reset pin passed to the constructor is
            valid, a hard reset will be performed before initializing the
            display. If using multiple SSD1306 displays on the same bus, and
            if they all share the same reset pin, you should only pass true
            on the first display being initialized, false on all others,
            else the already-initialized displays would be reset. Default if
            unspecified is true.
    @param  periphBegin
            If true, and if a hardware peripheral is being used (I2C or SPI,
            but not software SPI), call that peripheral's begin() function,
            else (false) it has already been done in one's sketch code.
            Cases where false might be used include multiple displays or
            other devices sharing a common bus, or situations on some
            platforms where a nonstandard begin() function is available
            (e.g. a TwoWire interface on non-default pins, as can be done
            on the ESP8266 and perhaps others).
    @return true on successful allocation/init, false otherwise.
            Well-behaved code should check the return value before
            proceeding.
    @note   MUST call this function before any drawing or updates!
*/
bool yama_2_GFX_SSD1306::oled_setup(uint8_t vcs, uint8_t addr) {

  if ((!buffer) && !(buffer = (uint8_t *)malloc(WIDTH * ((HEIGHT + 7) / 8))))
    return false;

  clearDisplay();

  vccstate = vcs;

  // Setup pin directions
  if (wire) { // Using I2C
    // If I2C address is unspecified, use default
    // (0x3C for 32-pixel-tall displays, 0x3D for all others).
    i2caddr = addr ? addr : ((HEIGHT == 32) ? 0x3C : 0x3D);
    // TwoWire begin() function might be already performed by the calling
    // function if it has unusual circumstances (e.g. TWI variants that
    // can accept different SDA/SCL pins, or if two SSD1306 instances
    // with different addresses -- only a single begin() is needed).

  } else { // Using one of the SPI modes, either soft or hardware
    if (spi) { // Hardware SPI
      // SPI peripheral begin same as wire check above.
      gpio_init(dcPin);
      gpio_init(csPin);
      gpio_set_dir(dcPin, GPIO_OUT);
      gpio_set_dir(csPin, GPIO_OUT);
      gpio_put(csPin, 1);
      gpio_put(dcPin, 1);
      gpio_pull_up(csPin);
      gpio_pull_up(dcPin);
    }
  }

  // Reset SSD1306 if requested and reset pin specified in constructor
  if (rstPin >= 0) {
    gpio_init(rstPin);
    gpio_set_dir(rstPin, GPIO_OUT);
    gpio_put(rstPin, 1);
    gpio_pull_up(dcPin);
    sleep_ms(10);
    gpio_put(rstPin, 0);
    sleep_ms(10);
    gpio_put(rstPin, 1);
    sleep_ms(10);
  }

  yama_2_GFX_SSD1306_TRANSACTION_START

  // Init sequence
  static const uint8_t yama_2_GFX_SSD1306_PROGMEM init1[] = {SSD1306_DISPLAYOFF,         // 0xAE
                                          SSD1306_SETDISPLAYCLOCKDIV, // 0xD5
                                          0x80, // the suggested ratio 0x80
                                          SSD1306_SETMULTIPLEX}; // 0xA8
  ssd1306_commandList(init1, sizeof(init1));
  ssd1306_command1(HEIGHT - 1);

  static const uint8_t yama_2_GFX_SSD1306_PROGMEM init2[] = {SSD1306_SETDISPLAYOFFSET, // 0xD3
                                          0x0,                      // no offset
                                          SSD1306_SETSTARTLINE | 0x0, // line #0
                                          SSD1306_CHARGEPUMP};        // 0x8D
  ssd1306_commandList(init2, sizeof(init2));

  ssd1306_command1((vccstate == SSD1306_EXTERNALVCC) ? 0x10 : 0x14);

  static const uint8_t yama_2_GFX_SSD1306_PROGMEM init3[] = {SSD1306_MEMORYMODE, // 0x20
                                          0x00, // 0x0 act like ks0108
                                          SSD1306_SEGREMAP | 0x1,
                                          SSD1306_COMSCANDEC};
  ssd1306_commandList(init3, sizeof(init3));

  uint8_t comPins = 0x02;
  contrast = 0x8F;

  if ((WIDTH == 128) && (HEIGHT == 32)) {
    comPins = 0x02;
    contrast = 0x8F;
  } else if ((WIDTH == 128) && (HEIGHT == 64)) {
    comPins = 0x12;
    contrast = (vccstate == SSD1306_EXTERNALVCC) ? 0x9F : 0xCF;
  } else if ((WIDTH == 96) && (HEIGHT == 16)) {
    comPins = 0x2; // ada x12
    contrast = (vccstate == SSD1306_EXTERNALVCC) ? 0x10 : 0xAF;
  } else {
    // Other screen varieties -- TBD
  }

  ssd1306_command1(SSD1306_SETCOMPINS);
  ssd1306_command1(comPins);
  ssd1306_command1(SSD1306_SETCONTRAST);
  ssd1306_command1(contrast);

  ssd1306_command1(SSD1306_SETPRECHARGE); // 0xd9
  ssd1306_command1((vccstate == SSD1306_EXTERNALVCC) ? 0x22 : 0xF1);
  static const uint8_t yama_2_GFX_SSD1306_PROGMEM init5[] = {
      SSD1306_SETVCOMDETECT, // 0xDB
      0x40,
      SSD1306_DISPLAYALLON_RESUME, // 0xA4
      SSD1306_NORMALDISPLAY,       // 0xA6
      SSD1306_DEACTIVATE_SCROLL,
      SSD1306_DISPLAYON}; // Main screen turn on
  ssd1306_commandList(init5, sizeof(init5));

  yama_2_GFX_SSD1306_TRANSACTION_END

  return true; // Success
}

// DRAWING FUNCTIONS -------------------------------------------------------

/*!
    @brief  Set/clear/invert a single pixel. This is also invoked by the
            yama_2_GFX library in generating many higher-level graphics
            primitives.
    @param  x
            Column of display -- 0 at left to (screen width - 1) at right.
    @param  y
            Row of display -- 0 at top to (screen height -1) at bottom.
    @param  color
            Pixel color, one of: SSD1306_BLACK, SSD1306_WHITE or SSD1306_INVERT.
    @return None (void).
    @note   Changes buffer contents only, no immediate effect on display.
            Follow up with a call to display(), or with other graphics
            commands as needed by one's own application.
*/
void yama_2_GFX_SSD1306::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if ((x >= 0) && (x < width()) && (y >= 0) && (y < height())) {
    // Pixel is in-bounds. Rotate coordinates if needed.
    switch (rotation) {
    case 1:
      yama_2_GFX_SSD1306_swap(x, y);
      x = WIDTH - x - 1;
      break;
    case 2:
      x = WIDTH - x - 1;
      y = HEIGHT - y - 1;
      break;
    case 3:
      yama_2_GFX_SSD1306_swap(x, y);
      y = HEIGHT - y - 1;
      break;
    }
    switch (color) {
    case SSD1306_WHITE:
      buffer[x + (y / 8) * WIDTH] |= (1 << (y & 7));
      break;
    case SSD1306_BLACK:
      buffer[x + (y / 8) * WIDTH] &= ~(1 << (y & 7));
      break;
    case SSD1306_INVERSE:
      buffer[x + (y / 8) * WIDTH] ^= (1 << (y & 7));
      break;
    }
  }
}

/*!
    @brief  Clear contents of display buffer (set all pixels to off).
    @return None (void).
    @note   Changes buffer contents only, no immediate effect on display.
            Follow up with a call to display(), or with other graphics
            commands as needed by one's own application.
*/
void yama_2_GFX_SSD1306::clearDisplay(void) {
  memset(buffer, 0, WIDTH * ((HEIGHT + 7) / 8));
}

/*!
    @brief  Draw a horizontal line. This is also invoked by the yama_2_GFX
            library in generating many higher-level graphics primitives.
    @param  x
            Leftmost column -- 0 at left to (screen width - 1) at right.
    @param  y
            Row of display -- 0 at top to (screen height -1) at bottom.
    @param  w
            Width of line, in pixels.
    @param  color
            Line color, one of: SSD1306_BLACK, SSD1306_WHITE or SSD1306_INVERT.
    @return None (void).
    @note   Changes buffer contents only, no immediate effect on display.
            Follow up with a call to display(), or with other graphics
            commands as needed by one's own application.
*/
void yama_2_GFX_SSD1306::drawFastHLine(int16_t x, int16_t y, int16_t w,
                                     uint16_t color) {
  bool bSwap = false;
  switch (rotation) {
  case 1:
    // 90 degree rotation, swap x & y for rotation, then invert x
    bSwap = true;
    yama_2_GFX_SSD1306_swap(x, y);
    x = WIDTH - x - 1;
    break;
  case 2:
    // 180 degree rotation, invert x and y, then shift y around for height.
    x = WIDTH - x - 1;
    y = HEIGHT - y - 1;
    x -= (w - 1);
    break;
  case 3:
    // 270 degree rotation, swap x & y for rotation,
    // then invert y and adjust y for w (not to become h)
    bSwap = true;
    yama_2_GFX_SSD1306_swap(x, y);
    y = HEIGHT - y - 1;
    y -= (w - 1);
    break;
  }

  if (bSwap)
    drawFastVLineInternal(x, y, w, color);
  else
    drawFastHLineInternal(x, y, w, color);
}

void yama_2_GFX_SSD1306::drawFastHLineInternal(int16_t x, int16_t y, int16_t w,
                                             uint16_t color) {

  if ((y >= 0) && (y < HEIGHT)) { // Y coord in bounds?
    if (x < 0) {                  // Clip left
      w += x;
      x = 0;
    }
    if ((x + w) > WIDTH) { // Clip right
      w = (WIDTH - x);
    }
    if (w > 0) { // Proceed only if width is positive
      uint8_t *pBuf = &buffer[(y / 8) * WIDTH + x], mask = 1 << (y & 7);
      switch (color) {
      case SSD1306_WHITE:
        while (w--) {
          *pBuf++ |= mask;
        };
        break;
      case SSD1306_BLACK:
        mask = ~mask;
        while (w--) {
          *pBuf++ &= mask;
        };
        break;
      case SSD1306_INVERSE:
        while (w--) {
          *pBuf++ ^= mask;
        };
        break;
      }
    }
  }
}

/*!
    @brief  Draw a vertical line. This is also invoked by the yama_2_GFX
            library in generating many higher-level graphics primitives.
    @param  x
            Column of display -- 0 at left to (screen width -1) at right.
    @param  y
            Topmost row -- 0 at top to (screen height - 1) at bottom.
    @param  h
            Height of line, in pixels.
    @param  color
            Line color, one of: SSD1306_BLACK, SSD1306_WHITE or SSD1306_INVERT.
    @return None (void).
    @note   Changes buffer contents only, no immediate effect on display.
            Follow up with a call to display(), or with other graphics
            commands as needed by one's own application.
*/
void yama_2_GFX_SSD1306::drawFastVLine(int16_t x, int16_t y, int16_t h,
                                     uint16_t color) {
  bool bSwap = false;
  switch (rotation) {
  case 1:
    // 90 degree rotation, swap x & y for rotation,
    // then invert x and adjust x for h (now to become w)
    bSwap = true;
    yama_2_GFX_SSD1306_swap(x, y);
    x = WIDTH - x - 1;
    x -= (h - 1);
    break;
  case 2:
    // 180 degree rotation, invert x and y, then shift y around for height.
    x = WIDTH - x - 1;
    y = HEIGHT - y - 1;
    y -= (h - 1);
    break;
  case 3:
    // 270 degree rotation, swap x & y for rotation, then invert y
    bSwap = true;
    yama_2_GFX_SSD1306_swap(x, y);
    y = HEIGHT - y - 1;
    break;
  }

  if (bSwap)
    drawFastHLineInternal(x, y, h, color);
  else
    drawFastVLineInternal(x, y, h, color);
}

void yama_2_GFX_SSD1306::drawFastVLineInternal(int16_t x, int16_t __y,
                                             int16_t __h, uint16_t color) {

  if ((x >= 0) && (x < WIDTH)) { // X coord in bounds?
    if (__y < 0) {               // Clip top
      __h += __y;
      __y = 0;
    }
    if ((__y + __h) > HEIGHT) { // Clip bottom
      __h = (HEIGHT - __y);
    }
    if (__h > 0) { // Proceed only if height is now positive
      // this display doesn't need ints for coordinates,
      // use local byte registers for faster juggling
      uint8_t y = __y, h = __h;
      uint8_t *pBuf = &buffer[(y / 8) * WIDTH + x];

      // do the first partial byte, if necessary - this requires some masking
      uint8_t mod = (y & 7);
      if (mod) {
        // mask off the high n bits we want to set
        mod = 8 - mod;
        // note - lookup table results in a nearly 10% performance
        // improvement in fill* functions
        // uint8_t mask = ~(0xFF >> mod);
        static const uint8_t yama_2_GFX_SSD1306_PROGMEM premask[8] = {0x00, 0x80, 0xC0, 0xE0,
                                                   0xF0, 0xF8, 0xFC, 0xFE};
        uint8_t mask = read_byte_data(&premask[mod]);
        // adjust the mask if we're not going to reach the end of this byte
        if (h < mod)
          mask &= (0XFF >> (mod - h));

        switch (color) {
        case SSD1306_WHITE:
          *pBuf |= mask;
          break;
        case SSD1306_BLACK:
          *pBuf &= ~mask;
          break;
        case SSD1306_INVERSE:
          *pBuf ^= mask;
          break;
        }
        pBuf += WIDTH;
      }

      if (h >= mod) { // More to go?
        h -= mod;
        // Write solid bytes while we can - effectively 8 rows at a time
        if (h >= 8) {
          if (color == SSD1306_INVERSE) {
            // separate copy of the code so we don't impact performance of
            // black/white write version with an extra comparison per loop
            do {
              *pBuf ^= 0xFF; // Invert byte
              pBuf += WIDTH; // Advance pointer 8 rows
              h -= 8;        // Subtract 8 rows from height
            } while (h >= 8);
          } else {
            // store a local value to work with
            uint8_t val = (color != SSD1306_BLACK) ? 255 : 0;
            do {
              *pBuf = val;   // Set byte
              pBuf += WIDTH; // Advance pointer 8 rows
              h -= 8;        // Subtract 8 rows from height
            } while (h >= 8);
          }
        }

        if (h) { // Do the final partial byte, if necessary
          mod = h & 7;
          // this time we want to mask the low bits of the byte,
          // vs the high bits we did above
          // uint8_t mask = (1 << mod) - 1;
          // note - lookup table results in a nearly 10% performance
          // improvement in fill* functions
          static const uint8_t yama_2_GFX_SSD1306_PROGMEM postmask[8] = {0x00, 0x01, 0x03, 0x07,
                                                      0x0F, 0x1F, 0x3F, 0x7F};
          uint8_t mask = read_byte_data(&postmask[mod]);
          switch (color) {
          case SSD1306_WHITE:
            *pBuf |= mask;
            break;
          case SSD1306_BLACK:
            *pBuf &= ~mask;
            break;
          case SSD1306_INVERSE:
            *pBuf ^= mask;
            break;
          }
        }
      }
    } // endif positive height
  }   // endif x in bounds
}

/*!
    @brief  Return color of a single pixel in display buffer.
    @param  x
            Column of display -- 0 at left to (screen width - 1) at right.
    @param  y
            Row of display -- 0 at top to (screen height -1) at bottom.
    @return true if pixel is set (usually SSD1306_WHITE, unless display invert
   mode is enabled), false if clear (SSD1306_BLACK).
    @note   Reads from buffer contents; may not reflect current contents of
            screen if display() has not been called.
*/
bool yama_2_GFX_SSD1306::getPixel(int16_t x, int16_t y) {
  if ((x >= 0) && (x < width()) && (y >= 0) && (y < height())) {
    // Pixel is in-bounds. Rotate coordinates if needed.
    switch (rotation) {
    case 1:
      yama_2_GFX_SSD1306_swap(x, y);
      x = WIDTH - x - 1;
      break;
    case 2:
      x = WIDTH - x - 1;
      y = HEIGHT - y - 1;
      break;
    case 3:
      yama_2_GFX_SSD1306_swap(x, y);
      y = HEIGHT - y - 1;
      break;
    }
    return (buffer[x + (y / 8) * WIDTH] & (1 << (y & 7)));
  }
  return false; // Pixel out of bounds
}

/*!
    @brief  Get base address of display buffer for direct reading or writing.
    @return Pointer to an unsigned 8-bit array, column-major, columns padded
            to full byte boundary if needed.
*/
uint8_t *yama_2_GFX_SSD1306::getBuffer(void) { return buffer; }

// REFRESH DISPLAY ---------------------------------------------------------

/*!
    @brief  Push data currently in RAM to SSD1306 display.
    @return None (void).
    @note   Drawing operations are not visible until this function is
            called. Call after each graphics command, or after a whole set
            of graphics commands, as best needed by one's own application.
*/
void yama_2_GFX_SSD1306::display(void) {
  yama_2_GFX_SSD1306_TRANSACTION_START
  static const uint8_t yama_2_GFX_SSD1306_PROGMEM dlist1[] = {
      SSD1306_PAGEADDR,
      0,                      // Page start address
      0xFF,                   // Page end (not really, but works here)
      SSD1306_COLUMNADDR, 0}; // Column start address
  ssd1306_commandList(dlist1, sizeof(dlist1));
  ssd1306_command1(WIDTH - 1); // Column end address

  uint16_t count = WIDTH * ((HEIGHT + 7) / 8);
  uint8_t *ptr = buffer;
  if (wire) { // I2C
    int n1, n2;
    uint8_t *m;
    n1 = count / yama_2_GFX_SSD1306_I2C_WIRE_MAX;
    n2 = count - (n1 * yama_2_GFX_SSD1306_I2C_WIRE_MAX);
    m = i2c_buffer01;
    for (int i = 0; i < n1; i++) {
      *m = 0x40;
      memcpy(m + 1, ptr, yama_2_GFX_SSD1306_I2C_WIRE_MAX);
      i2c_write_blocking(wire, i2caddr, m, yama_2_GFX_SSD1306_I2C_WIRE_MAX + 1, false);
      ptr += yama_2_GFX_SSD1306_I2C_WIRE_MAX;
    }
    if (n2 > 0) {
      *m = 0x40;
      memcpy(m + 1, ptr, n2);
      i2c_write_blocking(wire, i2caddr, m, n2 + 1, false);
    }

  } else { // SPI
    yama_2_GFX_SSD1306_MODE_DATA
    while (count--)
      SPIwrite(*ptr++);
  }
  yama_2_GFX_SSD1306_TRANSACTION_END
}

// SCROLLING FUNCTIONS -----------------------------------------------------

/*!
    @brief  Activate a right-handed scroll for all or part of the display.
    @param  start
            First row.
    @param  stop
            Last row.
    @return None (void).
*/
// To scroll the whole display, run: display.startscrollright(0x00, 0x0F)
void yama_2_GFX_SSD1306::startscrollright(uint8_t start, uint8_t stop) {
  yama_2_GFX_SSD1306_TRANSACTION_START
  static const uint8_t yama_2_GFX_SSD1306_PROGMEM scrollList1a[] = {
      SSD1306_RIGHT_HORIZONTAL_SCROLL, 0X00};
  ssd1306_commandList(scrollList1a, sizeof(scrollList1a));
  ssd1306_command1(start);
  ssd1306_command1(0X00);
  ssd1306_command1(stop);
  static const uint8_t yama_2_GFX_SSD1306_PROGMEM scrollList1b[] = {0X00, 0XFF,
                                                 SSD1306_ACTIVATE_SCROLL};
  ssd1306_commandList(scrollList1b, sizeof(scrollList1b));
  yama_2_GFX_SSD1306_TRANSACTION_END
}

/*!
    @brief  Activate a left-handed scroll for all or part of the display.
    @param  start
            First row.
    @param  stop
            Last row.
    @return None (void).
*/
// To scroll the whole display, run: display.startscrollleft(0x00, 0x0F)
void yama_2_GFX_SSD1306::startscrollleft(uint8_t start, uint8_t stop) {
  yama_2_GFX_SSD1306_TRANSACTION_START
  static const uint8_t yama_2_GFX_SSD1306_PROGMEM scrollList2a[] = {SSD1306_LEFT_HORIZONTAL_SCROLL,
                                                 0X00};
  ssd1306_commandList(scrollList2a, sizeof(scrollList2a));
  ssd1306_command1(start);
  ssd1306_command1(0X00);
  ssd1306_command1(stop);
  static const uint8_t yama_2_GFX_SSD1306_PROGMEM scrollList2b[] = {0X00, 0XFF,
                                                 SSD1306_ACTIVATE_SCROLL};
  ssd1306_commandList(scrollList2b, sizeof(scrollList2b));
  yama_2_GFX_SSD1306_TRANSACTION_END
}

/*!
    @brief  Activate a diagonal scroll for all or part of the display.
    @param  start
            First row.
    @param  stop
            Last row.
    @return None (void).
*/
// display.startscrolldiagright(0x00, 0x0F)
void yama_2_GFX_SSD1306::startscrolldiagright(uint8_t start, uint8_t stop) {
  yama_2_GFX_SSD1306_TRANSACTION_START
  static const uint8_t yama_2_GFX_SSD1306_PROGMEM scrollList3a[] = {
      SSD1306_SET_VERTICAL_SCROLL_AREA, 0X00};
  ssd1306_commandList(scrollList3a, sizeof(scrollList3a));
  ssd1306_command1(HEIGHT);
  static const uint8_t yama_2_GFX_SSD1306_PROGMEM scrollList3b[] = {
      SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL, 0X00};
  ssd1306_commandList(scrollList3b, sizeof(scrollList3b));
  ssd1306_command1(start);
  ssd1306_command1(0X00);
  ssd1306_command1(stop);
  static const uint8_t yama_2_GFX_SSD1306_PROGMEM scrollList3c[] = {0X01, SSD1306_ACTIVATE_SCROLL};
  ssd1306_commandList(scrollList3c, sizeof(scrollList3c));
  yama_2_GFX_SSD1306_TRANSACTION_END
}

/*!
    @brief  Activate alternate diagonal scroll for all or part of the display.
    @param  start
            First row.
    @param  stop
            Last row.
    @return None (void).
*/
// To scroll the whole display, run: display.startscrolldiagleft(0x00, 0x0F)
void yama_2_GFX_SSD1306::startscrolldiagleft(uint8_t start, uint8_t stop) {
  yama_2_GFX_SSD1306_TRANSACTION_START
  static const uint8_t yama_2_GFX_SSD1306_PROGMEM scrollList4a[] = {
      SSD1306_SET_VERTICAL_SCROLL_AREA, 0X00};
  ssd1306_commandList(scrollList4a, sizeof(scrollList4a));
  ssd1306_command1(HEIGHT);
  static const uint8_t yama_2_GFX_SSD1306_PROGMEM scrollList4b[] = {
      SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL, 0X00};
  ssd1306_commandList(scrollList4b, sizeof(scrollList4b));
  ssd1306_command1(start);
  ssd1306_command1(0X00);
  ssd1306_command1(stop);
  static const uint8_t yama_2_GFX_SSD1306_PROGMEM scrollList4c[] = {0X01, SSD1306_ACTIVATE_SCROLL};
  ssd1306_commandList(scrollList4c, sizeof(scrollList4c));
  yama_2_GFX_SSD1306_TRANSACTION_END
}

/*!
    @brief  Cease a previously-begun scrolling action.
    @return None (void).
*/
void yama_2_GFX_SSD1306::stopscroll(void) {
  yama_2_GFX_SSD1306_TRANSACTION_START
  ssd1306_command1(SSD1306_DEACTIVATE_SCROLL);
  yama_2_GFX_SSD1306_TRANSACTION_END
}

// OTHER HARDWARE SETTINGS -------------------------------------------------

/*!
    @brief  Enable or disable display invert mode (white-on-black vs
            black-on-white).
    @param  i
            If true, switch to invert mode (black-on-white), else normal
            mode (white-on-black).
    @return None (void).
    @note   This has an immediate effect on the display, no need to call the
            display() function -- buffer contents are not changed, rather a
            different pixel mode of the display hardware is used. When
            enabled, drawing SSD1306_BLACK (value 0) pixels will actually draw
   white, SSD1306_WHITE (value 1) will draw black.
*/
void yama_2_GFX_SSD1306::invertDisplay(bool i) {
  yama_2_GFX_SSD1306_TRANSACTION_START
  ssd1306_command1(i ? SSD1306_INVERTDISPLAY : SSD1306_NORMALDISPLAY);
  yama_2_GFX_SSD1306_TRANSACTION_END
}

/*!
    @brief  Dim the display.
    @param  dim
            true to enable lower brightness mode, false for full brightness.
    @return None (void).
    @note   This has an immediate effect on the display, no need to call the
            display() function -- buffer contents are not changed.
*/
void yama_2_GFX_SSD1306::dim(bool dim) {
  // the range of contrast to too small to be really useful
  // it is useful to dim the display
  yama_2_GFX_SSD1306_TRANSACTION_START
  ssd1306_command1(SSD1306_SETCONTRAST);
  ssd1306_command1(dim ? 0 : contrast);
  yama_2_GFX_SSD1306_TRANSACTION_END
}

void yama_2_GFX_SSD1306::i2c_write_byte_non_register(int addr, uint8_t val) {
    i2c_write_blocking(wire, addr, &val, 1, false);
}

void yama_2_GFX_SSD1306::setContrast(uint8_t _contrast) {
  contrast = _contrast;
  ssd1306_command1(SSD1306_SETCONTRAST);
  ssd1306_command1(contrast);
}
