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



// I2Cの場合
yama_2_GFX_SSD1306 display = yama_2_GFX_SSD1306(SSD1306_SCREEN_WIDTH, SSD1306_SCREEN_HEIGHT, SSD1306_I2C_PORT);

/*
// SPIの場合
yama_2_GFX_SSD1306 display = yama_2_GFX_SSD1306(SSD1306_SCREEN_WIDTH, SSD1306_SCREEN_HEIGHT, SSD1306_SPI_PORT, SSD1306_SPI_DC, SSD1306_SPI_RST, SSD1306_SPI_CS);
*/

void setup();
void loop();
int main() {
    stdio_init_all();

    // I2C初期化
    i2c_init(SSD1306_I2C_PORT, 400000);   // I2C clock 400kHz
    gpio_set_function(SSD1306_SDA, GPIO_FUNC_I2C);
    gpio_set_function(SSD1306_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(SSD1306_SDA);
    gpio_pull_up(SSD1306_SCL);

/*
    // SPI初期化
    spi_init(SSD1306_SPI_PORT, 8000000);  // SPI clock 8MHz
    gpio_set_function(SSD1306_SPI_SCK, GPIO_FUNC_SPI);
    gpio_set_function(SSD1306_SPI_MOSI, GPIO_FUNC_SPI);
    gpio_pull_up(SSD1306_SPI_SCK);
    gpio_pull_up(SSD1306_SPI_MOSI);
*/

    sleep_ms(100); 
    printf("\nReset:");

    display.oled_setup(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS);
    display.setRotation(0);
    display.fillScreen(SSD1306_BLACK);
    display.display();

/*
    display.oled_setup(SSD1306_SWITCHCAPVCC);
    display.setRotation(0);
    display.fillScreen(SSD1306_BLACK);
    display.display();
*/
    setup();
    while (1) {
        loop();
    }

    return 0;
}


void draw_bitmap01(int x, int y, const uint8_t *bitmap01, const uint8_t *bitmap_mask01, int width, int height, uint16_t fg, uint16_t bg);
void random_raspberry(void);
void loop() {
    random_raspberry();
}



extern const uint8_t bitmap_mask01[];
extern const uint8_t bitmap01[];

#define RASPBERRY_N  15
#define RDX           3
#define RDY           3
#define IMG_WIDTH    32
#define IMG_HEIGHT   32

int rnd(int max);

struct {
    int    x;
    int    y;
    int    dx;
    int    dy;
    int    signx;
    int    signy;
    int    c;
} raspberry_data01[RASPBERRY_N];

void setup(void){
    for (int i = 0; i < RASPBERRY_N; i++) {
        raspberry_data01[i].x = rnd(display.width() - IMG_WIDTH);
        raspberry_data01[i].y = rnd(display.height() - IMG_HEIGHT);
        raspberry_data01[i].dx = rnd(RDX) + 1;
        raspberry_data01[i].dy = rnd(RDY) + 1;
        raspberry_data01[i].signx = rnd(2) ? 1 : -1;
        raspberry_data01[i].signy = rnd(2) ? 1 : -1;
    }
}

void random_raspberry(void) {

    int temp1, temp2;
    display.fillScreen(SSD1306_BLACK);
    for (int i = 0; i < RASPBERRY_N; i++ ) {
        temp1 = raspberry_data01[i].dx * raspberry_data01[i].signx;
        temp2 = raspberry_data01[i].x + temp1;
        if (temp2 > (display.width() - IMG_WIDTH)) {
            raspberry_data01[i].signx = -1;
            raspberry_data01[i].dx = rnd(RDX) + 1;
            raspberry_data01[i].x = (display.width() - IMG_WIDTH) + raspberry_data01[i].signx * raspberry_data01[i].dx;
        } else if (temp2 < 0 ) {
            raspberry_data01[i].signx = 1;
            raspberry_data01[i].dx = rnd(RDX) + 1;
            raspberry_data01[i].x = 0 + raspberry_data01[i].signx * raspberry_data01[i].dx;
        } else {
            raspberry_data01[i].x = raspberry_data01[i].x + temp1;
        } 
        temp1 = raspberry_data01[i].dy * raspberry_data01[i].signy;
        temp2 = raspberry_data01[i].y + temp1;
        if (temp2 > (display.height() - IMG_HEIGHT)) {
            raspberry_data01[i].signy = -1;
            raspberry_data01[i].dy = rnd(RDY) + 1;
            raspberry_data01[i].y = (display.height() - IMG_HEIGHT) + raspberry_data01[i].signy * raspberry_data01[i].dy;
        } else if (temp2 < 0 ) {
            raspberry_data01[i].signy = 1;
            raspberry_data01[i].dy = rnd(RDY) + 1;
            raspberry_data01[i].y = 0 + raspberry_data01[i].signy * raspberry_data01[i].dy;
        } else {
            raspberry_data01[i].y = raspberry_data01[i].y + temp1;
        } 
        draw_bitmap01( raspberry_data01[i].x, raspberry_data01[i].y, bitmap01, bitmap_mask01, IMG_WIDTH, IMG_HEIGHT, SSD1306_WHITE, SSD1306_BLACK);
    }
    display.display(); 
    sleep_ms(10);
}

int rnd(int a) {
    return (rand() % a);
}


inline int bit_test(const uint8_t *data, int n) {
    return (data[n >> 3] >> (7 - (n & 7))) & 1;
}

// yama_2_GFXには1ピクセル1bitのデータのbitmaskを使ったbitmap描画のメソッドがないので作りました。
void draw_bitmap01(int x, int y, const uint8_t *bitmap01, const uint8_t *bitmap_mask01, int width, int height, uint16_t fg, uint16_t bg) {
    int i, j, n;
    for (j = 0; j < height; j++) {
        for (i = 0; i < width; i++) {
            n = j * width + i;
            if (bit_test(bitmap_mask01, n)) {
                display.drawPixel(x + i, y + j, (bit_test(bitmap01, n)) ? fg : bg);
            }
        }
    }
}

//  maskデータ (32x32ピクセルのmaskデータです。1ピクセル1ビットです。)
//  ピクセル列の最初がMSBから始まるデータ形式(0が透明、1が描画)
const uint8_t bitmap_mask01[] = {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x7c,0x3e,0x00,0x01,0xfe,0x7f,0x80,0x01,0xff,0xff,0x80,0x01,0xff,0xff,0x80,
    0x01,0xff,0xff,0x80,0x01,0xff,0xff,0x80,0x00,0xff,0xff,0x00,0x00,0x7f,0xfe,0x00,
    0x00,0x7f,0xfe,0x00,0x00,0xff,0xff,0x00,0x00,0xff,0xff,0x00,0x01,0xff,0xff,0x80,
    0x03,0xff,0xff,0xc0,0x03,0xff,0xff,0xc0,0x03,0xff,0xff,0xc0,0x03,0xff,0xff,0xc0,
    0x03,0xff,0xff,0xc0,0x03,0xff,0xff,0xc0,0x01,0xff,0xff,0x80,0x01,0xff,0xff,0x80,
    0x01,0xff,0xff,0x80,0x00,0xff,0xff,0x00,0x00,0x7f,0xfe,0x00,0x00,0x3f,0xfc,0x00,
    0x00,0x0f,0xf0,0x00,0x00,0x03,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

//  画像データ (32x32ピクセルのマスクデータです。1ピクセル1ビットです。)
//  ピクセル列の最初がMSBから始まるデータ形式
const uint8_t bitmap01[] = {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x7c,0x3e,0x00,0x01,0xfe,0x7f,0x80,0x01,0xef,0xf7,0x80,0x01,0x83,0xc1,0x80,
    0x01,0x8b,0xd1,0x80,0x01,0xc7,0xe3,0x80,0x00,0xe7,0xe7,0x00,0x00,0x7f,0xfe,0x00,
    0x00,0x7e,0x7e,0x00,0x00,0xcc,0x33,0x00,0x00,0xde,0x7b,0x00,0x01,0xf3,0xcf,0x80,
    0x03,0xe1,0x87,0xc0,0x03,0x41,0x82,0xc0,0x03,0x41,0x82,0xc0,0x03,0x73,0xce,0xc0,
    0x03,0x7e,0x7e,0xc0,0x03,0xfc,0x3f,0xc0,0x01,0x98,0x19,0x80,0x01,0x8c,0x31,0x80,
    0x01,0xce,0x73,0x80,0x00,0xef,0xf7,0x00,0x00,0x7e,0x7e,0x00,0x00,0x3c,0x3c,0x00,
    0x00,0x0e,0x70,0x00,0x00,0x03,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

