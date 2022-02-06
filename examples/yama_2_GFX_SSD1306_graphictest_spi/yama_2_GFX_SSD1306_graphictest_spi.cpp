//    
//    Copyright (c) 2012 Adafruit Industries.
//    Released under the BSD License
//    https://github.com/adafruit/Adafruit-GFX-Library/blob/master/license.txt
//

#include <stdio.h>
#include "pico/stdlib.h"
#include "yama_2_GFX.h"
#include "yama_2_GFX_SSD1306.h"

#define SSD1306_SPI_RST         8
#define SSD1306_SPI_DC          9
#define SSD1306_SPI_MOSI        11
#define SSD1306_SPI_SCK         10
#define SSD1306_SPI_CS          13
#define SSD1306_SPI_PORT        spi1

#define SSD1306_SDA         2
#define SSD1306_SCL         3
#define SSD1306_I2C_PORT    i2c1
// 製品によってI2Cアドレスが違うので確認してください
#define SSD1306_I2C_ADDRESS 0x3C

#define SSD1306_SCREEN_WIDTH 128   // OLED display width, in pixels
#define SSD1306_SCREEN_HEIGHT 64   // OLED display height, in pixels

/*
// I2Cの場合
yama_2_GFX_SSD1306 display = yama_2_GFX_SSD1306(SSD1306_SCREEN_WIDTH, SSD1306_SCREEN_HEIGHT, SSD1306_I2C_PORT);
*/

// SPIの場合
yama_2_GFX_SSD1306 display = yama_2_GFX_SSD1306(SSD1306_SCREEN_WIDTH, SSD1306_SCREEN_HEIGHT, SSD1306_SPI_PORT, SSD1306_SPI_DC, SSD1306_SPI_RST, SSD1306_SPI_CS);

unsigned long long testFillScreen();
unsigned long long testLines(uint16_t color);
unsigned long long testFastLines(uint16_t color1, uint16_t color2);
unsigned long long testRects(uint16_t color);
unsigned long long testFilledRects(uint16_t color1, uint16_t color2);
unsigned long long testFilledCircles(uint8_t radius, uint16_t color);
unsigned long long testCircles(uint8_t radius, uint16_t color);
unsigned long long testTriangles();
unsigned long long testFilledTriangles();
unsigned long long testRoundRects();
unsigned long long testFilledRoundRects();

void loop();
int main() {
    stdio_init_all();


    // SPIの初期化
    spi_init(SSD1306_SPI_PORT, 8000000);  // SPI clock 8MHz
    gpio_set_function(SSD1306_SPI_SCK, GPIO_FUNC_SPI);
    gpio_set_function(SSD1306_SPI_MOSI, GPIO_FUNC_SPI);
    gpio_pull_up(SSD1306_SPI_SCK);
    gpio_pull_up(SSD1306_SPI_MOSI);


/*
    // I2Cの初期化
    i2c_init(SSD1306_I2C_PORT, 400000);   // I2C clock 400kHz
    gpio_set_function(SSD1306_SDA, GPIO_FUNC_I2C);
    gpio_set_function(SSD1306_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(SSD1306_SDA);
    gpio_pull_up(SSD1306_SCL);
*/

    sleep_ms(100); 
    printf("\nReset:");


/*
    // I2Cの場合
    display.oled_setup(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS);
    display.fillScreen(SSD1306_BLACK);
*/

    // SPIの場合
    display.oled_setup(SSD1306_SWITCHCAPVCC);
    display.fillScreen(SSD1306_BLACK);

    int r = 0;
    while (1) {
        display.setRotation(r);
        display.fillScreen(SSD1306_BLACK);
        display.display();

        loop();

        sleep_ms(5000);
        r++;
        if (r >= 4) r = 0;
    }

    return 0;
}

void loop() {
   printf("Benchmark                Time (microseconds)\r\n");
  sleep_ms(10);
   printf("Screen fill              ");
   printf("%lld\r\n", testFillScreen());
  sleep_ms(500);

   printf("Lines                    ");
   printf("%lld\r\n", testLines(SSD1306_WHITE));
  sleep_ms(500);

   printf("Horiz/Vert Lines         ");
   printf("%lld\r\n", testFastLines(SSD1306_WHITE, SSD1306_BLACK));
  sleep_ms(500);

   printf("Rectangles (outline)     ");
   printf("%lld\r\n", testRects(SSD1306_WHITE));
  sleep_ms(500);

   printf("Rectangles (filled)      ");
   printf("%lld\r\n", testFilledRects(SSD1306_WHITE, SSD1306_BLACK));
  sleep_ms(500);

   printf("Circles (filled)         ");
   printf("%lld\r\n", testFilledCircles(5, SSD1306_WHITE));

   printf("Circles (outline)        ");
   printf("%lld\r\n", testCircles(5, SSD1306_BLACK));
  sleep_ms(500);

   printf("Triangles (outline)      ");
   printf("%lld\r\n", testTriangles());
  sleep_ms(500);

   printf("Triangles (filled)       ");
   printf("%lld\r\n", testFilledTriangles());
  sleep_ms(500);

   printf("Rounded rects (outline)  ");
   printf("%lld\r\n", testRoundRects());
  sleep_ms(500);

   printf("Rounded rects (filled)   ");
   printf("%lld\r\n", testFilledRoundRects());
  sleep_ms(500);

   printf("Done!\r\n");

   sleep_ms(5000);
}

void  yield(){};

unsigned long long testFillScreen() {
  unsigned long long start = time_us_64();
  display.fillScreen(SSD1306_BLACK);
  display.display();
  yield();
  yield();
  display.fillScreen(SSD1306_WHITE);
  display.display();
  yield();
  display.fillScreen(SSD1306_BLACK);
  display.display();
  yield();
  return time_us_64() - start;
}

unsigned long long testLines(uint16_t color) {
  unsigned long long start, t;
  int           x1, y1, x2, y2,
                w = display.width(),
                h = display.height();

  display.fillScreen(SSD1306_BLACK);
  display.display();
  yield();
  
  x1 = y1 = 0;
  y2    = h - 1;
  start = time_us_64();
  for(x2=0; x2<w; x2+=6) {
    display.drawLine(x1, y1, x2, y2, color);
    display.display();
  }
  x2    = w - 1;
  for(y2=0; y2<h; y2+=6) {
    display.drawLine(x1, y1, x2, y2, color);
    display.display();
  }
  t     = time_us_64() - start; // fillScreen doesn't count against timing

  yield();
  display.fillScreen(SSD1306_BLACK);
  display.display();
  yield();

  x1    = w - 1;
  y1    = 0;
  y2    = h - 1;
  start = time_us_64();
  for(x2=0; x2<w; x2+=6) {
    display.drawLine(x1, y1, x2, y2, color);
    display.display();
  }
  x2    = 0;
  for(y2=0; y2<h; y2+=6) {
    display.drawLine(x1, y1, x2, y2, color);
    display.display();
  }
  t    += time_us_64() - start;

  yield();
  display.fillScreen(SSD1306_BLACK);
  display.display();
  yield();

  x1    = 0;
  y1    = h - 1;
  y2    = 0;
  start = time_us_64();
  for(x2=0; x2<w; x2+=6) {
    display.drawLine(x1, y1, x2, y2, color);
    display.display();
  }
  x2    = w - 1;
  for(y2=0; y2<h; y2+=6) {
    display.drawLine(x1, y1, x2, y2, color);
    display.display();
  }
  t    += time_us_64() - start;

  yield();
  display.fillScreen(SSD1306_BLACK);
  display.display();
  yield();

  x1    = w - 1;
  y1    = h - 1;
  y2    = 0;
  start = time_us_64();
  for(x2=0; x2<w; x2+=6) {
    display.drawLine(x1, y1, x2, y2, color);
    display.display();
  }
  x2    = 0;
  for(y2=0; y2<h; y2+=6) {
    display.drawLine(x1, y1, x2, y2, color);
    display.display();
  }

  yield();
  return time_us_64() - start;
}

unsigned long long testFastLines(uint16_t color1, uint16_t color2) {
  unsigned long long start;
  int           x, y, w = display.width(), h = display.height();

  display.fillScreen(SSD1306_BLACK);
  display.display();
  start = time_us_64();
  for(y=0; y<h; y+=5) {
    display.drawFastHLine(0, y, w, color1);
    display.display();
  }
  for(x=0; x<w; x+=5) {
    display.drawFastVLine(x, 0, h, color2);
    display.display();
  }

  return time_us_64() - start;
}

unsigned long long testRects(uint16_t color) {
  unsigned long long start;
  int           n, i, i2,
                cx = display.width()  / 2,
                cy = display.height() / 2;

  display.fillScreen(SSD1306_BLACK);
  display.display();
  n     = yama_2_GFX_min(display.width(), display.height());
  start = time_us_64();
  for(i=2; i<n; i+=6) {
    i2 = i / 2;
    display.drawRect(cx-i2, cy-i2, i, i, color);
    display.display();
  }

  return time_us_64() - start;
}

unsigned long long testFilledRects(uint16_t color1, uint16_t color2) {
  unsigned long long start, t = 0;
  int           n, i, i2,
                cx = display.width()  / 2 - 1,
                cy = display.height() / 2 - 1;

  display.fillScreen(SSD1306_BLACK);
  display.display();
  n = yama_2_GFX_min(display.width(), display.height());
  for(i=n; i>0; i-=6) {
    i2    = i / 2;
    start = time_us_64();
    display.fillRect(cx-i2, cy-i2, i, i, color1);
    display.display();
    t    += time_us_64() - start;
    // Outlines are not included in timing results
    display.drawRect(cx-i2, cy-i2, i, i, color2);
    display.display();
    yield();
  }

  return t;
}

unsigned long long testFilledCircles(uint8_t radius, uint16_t color) {
  unsigned long long start;
  int x, y, w = display.width(), h = display.height(), r2 = radius * 2;

  display.fillScreen(SSD1306_BLACK);
  display.display();
  start = time_us_64();
  for(x=radius; x<w; x+=r2) {
    for(y=radius; y<h; y+=r2) {
      display.fillCircle(x, y, radius, color);
      display.display();
    }
  }

  return time_us_64() - start;
}

unsigned long long testCircles(uint8_t radius, uint16_t color) {
  unsigned long long start;
  int           x, y, r2 = radius * 2,
                w = display.width()  + radius,
                h = display.height() + radius;

  // Screen is not cleared for this one -- this is
  // intentional and does not affect the reported time.
  start = time_us_64();
  for(x=0; x<w; x+=r2) {
    for(y=0; y<h; y+=r2) {
      display.drawCircle(x, y, radius, color);
      display.display();
    }
  }

  return time_us_64() - start;
}

unsigned long long testTriangles() {
  unsigned long long start;
  int           n, i, cx = display.width()  / 2 - 1,
                      cy = display.height() / 2 - 1;

  display.fillScreen(SSD1306_BLACK);
  display.display();
  n     = yama_2_GFX_min(cx, cy);
  start = time_us_64();
  for(i=0; i<n; i+=5) {
    display.drawTriangle(
      cx    , cy - i, // peak
      cx - i, cy + i, // bottom left
      cx + i, cy + i, // bottom right
      SSD1306_WHITE);
    display.display();
  }

  return time_us_64() - start;
}

unsigned long long testFilledTriangles() {
  unsigned long long start, t = 0;
  int           i, cx = display.width()  / 2 - 1,
                   cy = display.height() / 2 - 1;

  display.fillScreen(SSD1306_BLACK);
  display.display();
  start = time_us_64();
  for(i=yama_2_GFX_min(cx,cy); i>5; i-=5) {
    start = time_us_64();
    display.fillTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
      SSD1306_WHITE);
    display.display();
    t += time_us_64() - start;
    display.drawTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
      SSD1306_BLACK);
    display.display();
    yield();
  }

  return t;
}

unsigned long long testRoundRects() {
  unsigned long long start;
  int           w, i, i2,
                cx = display.width()  / 2 - 1,
                cy = display.height() / 2 - 1;

  display.fillScreen(SSD1306_BLACK);
  display.display();
  w     = yama_2_GFX_min(display.width(), display.height());
  start = time_us_64();
  for(i=0; i<w; i+=3) {
    i2 = i / 2;
    display.drawRoundRect(cx-i2, cy-i2, i, i, i/8, SSD1306_WHITE);
    display.display();
  }

  return time_us_64() - start;
}

unsigned long long testFilledRoundRects() {
  unsigned long long start;
  int           i, i2,
                cx = display.width()  / 2 - 1,
                cy = display.height() / 2 - 1;

  display.fillScreen(SSD1306_BLACK);
  display.display();
  start = time_us_64();
  int n = 0;
  for(i=yama_2_GFX_min(display.width(), display.height()); i>10; i-=3) {
    i2 = i / 2;
    display.fillRoundRect(cx-i2, cy-i2, i, i, i/8, ((n % 2) ? SSD1306_BLACK : SSD1306_WHITE));
    display.display();
    yield();
    n++;
  }

  return time_us_64() - start;
}
