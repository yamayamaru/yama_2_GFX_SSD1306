// yama_2_GFX_SSD1306.h
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

#include "pico/stdlib.h"
#include <stdio.h>
#include <stdlib.h>
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "yama_2_gfx.h"

#ifndef _YAMA_2_GFX_SSD1306_H
#define _YAMA_2_GFX_SSD1306_H

#define SSD1306_BLACK 0   ///< Draw 'off' pixels
#define SSD1306_WHITE 1   ///< Draw 'on' pixels
#define SSD1306_INVERSE 2 ///< Invert pixels

#define SSD1306_MEMORYMODE 0x20          ///< See datasheet
#define SSD1306_COLUMNADDR 0x21          ///< See datasheet
#define SSD1306_PAGEADDR 0x22            ///< See datasheet
#define SSD1306_SETCONTRAST 0x81         ///< See datasheet
#define SSD1306_CHARGEPUMP 0x8D          ///< See datasheet
#define SSD1306_SEGREMAP 0xA0            ///< See datasheet
#define SSD1306_DISPLAYALLON_RESUME 0xA4 ///< See datasheet
#define SSD1306_DISPLAYALLON 0xA5        ///< Not currently used
#define SSD1306_NORMALDISPLAY 0xA6       ///< See datasheet
#define SSD1306_INVERTDISPLAY 0xA7       ///< See datasheet
#define SSD1306_SETMULTIPLEX 0xA8        ///< See datasheet
#define SSD1306_DISPLAYOFF 0xAE          ///< See datasheet
#define SSD1306_DISPLAYON 0xAF           ///< See datasheet
#define SSD1306_COMSCANINC 0xC0          ///< Not currently used
#define SSD1306_COMSCANDEC 0xC8          ///< See datasheet
#define SSD1306_SETDISPLAYOFFSET 0xD3    ///< See datasheet
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5  ///< See datasheet
#define SSD1306_SETPRECHARGE 0xD9        ///< See datasheet
#define SSD1306_SETCOMPINS 0xDA          ///< See datasheet
#define SSD1306_SETVCOMDETECT 0xDB       ///< See datasheet

#define SSD1306_SETLOWCOLUMN 0x00  ///< Not currently used
#define SSD1306_SETHIGHCOLUMN 0x10 ///< Not currently used
#define SSD1306_SETSTARTLINE 0x40  ///< See datasheet

#define SSD1306_EXTERNALVCC 0x01  ///< External display voltage source
#define SSD1306_SWITCHCAPVCC 0x02 ///< Gen. display voltage from 3.3V

#define SSD1306_RIGHT_HORIZONTAL_SCROLL 0x26              ///< Init rt scroll
#define SSD1306_LEFT_HORIZONTAL_SCROLL 0x27               ///< Init left scroll
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29 ///< Init diag scroll
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 0x2A  ///< Init diag scroll
#define SSD1306_DEACTIVATE_SCROLL 0x2E                    ///< Stop scroll
#define SSD1306_ACTIVATE_SCROLL 0x2F                      ///< Start scroll
#define SSD1306_SET_VERTICAL_SCROLL_AREA 0xA3             ///< Set scroll range

// Deprecated size stuff for backwards compatibility with old sketches
#define SSD1306_128_64 1
#define SSD1306_128_64_LCDWIDTH 128 ///< DEPRECATED: width w/SSD1306_128_64 defined
#define SSD1306_128_64_LCDHEIGHT 64 ///< DEPRECATED: height w/SSD1306_128_64 defined
#define SSD1306_128_32 2
#define SSD1306_128_32_LCDWIDTH 128 ///< DEPRECATED: width w/SSD1306_128_32 defined
#define SSD1306_128_32_LCDHEIGHT 32 ///< DEPRECATED: height w/SSD1306_128_32 defined
#define SSD1306_96_16  3
#define SSD1306_96_16_LCDWIDTH 96  ///< DEPRECATED: width w/SSD1306_96_16 defined
#define SSD1306_96_16_LCDHEIGHT 16 ///< DEPRECATED: height w/SSD1306_96_16 defined

#define yama_2_GFX_SSD1306_DEFAULT_I2C_PORT  i2c0
#define yama_2_GFX_SSD1306_EFAULT_I2C_ADDR   0x3C

#define yama_2_GFX_SSD1306_SSD1306_DEFAULT_SPI_PORT spi1
#define yama_2_GFX_SSD1306_SSD1306_DEFAULT_PIN_CS     13
#define yama_2_GFX_SSD1306_SSD1306_DEFAULT_PIN_DC      9

#define yama_2_GFX_SSD1306_I2C_WIRE_MAX 256 ///< Use common Arduino core default

// ONE of the following three lines must be #defined:
//#define SSD1306_128_64 ///< DEPRECTAED: old way to specify 128x64 screen
//#define SSD1306_128_32 ///< DEPRECATED: old way to specify 128x32 screen
//#define SSD1306_96_16  ///< DEPRECATED: old way to specify 96x16 screen
// This establishes the screen dimensions in old yama_2_GFX_SSD1306 sketches
// (NEW CODE SHOULD IGNORE THIS, USE THE CONSTRUCTORS THAT ACCEPT WIDTH
// AND HEIGHT ARGUMENTS).


/// fit into the SSD1306_ naming scheme

/*!
    @brief  Class that stores state and functions for interacting with
            SSD1306 OLED displays.
*/
class yama_2_GFX_SSD1306 : public yama_2_GFX {
public:
  // NEW CONSTRUCTORS -- recommended for new projects
  yama_2_GFX_SSD1306(uint8_t w, uint8_t h,
                   i2c_inst_t *twi = yama_2_GFX_SSD1306_DEFAULT_I2C_PORT, 
                   int8_t rst_pin = -1);

  yama_2_GFX_SSD1306(uint8_t w, uint8_t h, spi_inst_t *_spi = yama_2_GFX_SSD1306_SSD1306_DEFAULT_SPI_PORT,
                   int8_t dc_pin = yama_2_GFX_SSD1306_SSD1306_DEFAULT_PIN_DC,
                   int8_t rst_pin = -1, 
                   int8_t cs_pin = yama_2_GFX_SSD1306_SSD1306_DEFAULT_PIN_CS);

  bool oled_setup(uint8_t switchvcc = SSD1306_SWITCHCAPVCC, uint8_t addr = 0);
  void display(void);
  void clearDisplay(void);
  void invertDisplay(bool i);
  void dim(bool dim);
  void drawPixel(int16_t x, int16_t y, uint16_t color);
  virtual void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
  virtual void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  void startscrollright(uint8_t start, uint8_t stop);
  void startscrollleft(uint8_t start, uint8_t stop);
  void startscrolldiagright(uint8_t start, uint8_t stop);
  void startscrolldiagleft(uint8_t start, uint8_t stop);
  void stopscroll(void);
  void setContrast(uint8_t _contrast);
  void ssd1306_command(uint8_t c);
  bool getPixel(int16_t x, int16_t y);
  uint8_t *getBuffer(void);

  void SPI_CS_HIGH(void) {
    asm volatile("nop \n nop \n nop");
    gpio_put(csPin, 1);  // Active low
    asm volatile("nop \n nop \n nop");
  }

  void SPI_CS_LOW(void) {
    asm volatile("nop \n nop \n nop");
    gpio_put(csPin, 0);
    asm volatile("nop \n nop \n nop");
  }

  void SPI_DC_HIGH(void) {
    asm volatile("nop \n nop \n nop");
    gpio_put(dcPin, 1);  // Active low
    asm volatile("nop \n nop \n nop");
  }

  void SPI_DC_LOW(void) {
    asm volatile("nop \n nop \n nop");
    gpio_put(dcPin, 0);
    asm volatile("nop \n nop \n nop");
  }

  void i2c_write_byte_non_register(int addr, uint8_t val);

protected:
  inline void SPIwrite(uint8_t d) __attribute__((always_inline));
  void drawFastHLineInternal(int16_t x, int16_t y, int16_t w, uint16_t color);
  void drawFastVLineInternal(int16_t x, int16_t y, int16_t h, uint16_t color);
  void ssd1306_command1(uint8_t c);
  void ssd1306_commandList(const uint8_t *c, uint8_t n);
  inline const uint8_t read_byte_data(const uint8_t *adr) {
        return *adr;
  }

  spi_inst_t *spi = NULL;
  i2c_inst_t *wire = NULL;
  uint8_t    *buffer;
  int8_t     i2caddr, vccstate, page_end;
  int8_t     dcPin, csPin, rstPin;
  uint8_t    contrast; // normal contrast setting for this device
  uint8_t    i2c_buffer01[yama_2_GFX_SSD1306_I2C_WIRE_MAX + 1];
};

#endif // _YAMA_2_GFX_SSD1306_H
