使用にはyama_2_GFXが別途必要になります。  
<https://github.com/yamayamaru/yama_2_GFX>  
  
yama_2_GFX.h  
yama_2_GFX.cpp  
yama_2_GFX_SSD1306.h  
yama_2_GFX_SSD1306.cpp  
yama_2_GFX_SSD1306_random_raspberry_spi.cpp 
CMakeLists.txt  
  
上記ファイルを同じフォルダに置きpico-sdkでビルドしてください。  

ソースコードに書かれてるSSD1306_SPI_MOSI、SSD1306_SPI_SCK、SSD1306_SPI_CSや
SSD1306_SPI_RST、SSD1306_SPI_DCの番号はGPIOの番号です。  
下記画像で緑で書かれてる番号になります。  
ソースコードの例では実際のピン番号は下記のようになります。  
  
    #define SSD1306_SPI_RST         8      // ピン番号11
    #define SSD1306_SPI_DC          9      // ピン番号12
    #define SSD1306_SPI_MOSI        11     // ピン番号15
    #define SSD1306_SPI_SCK         10     // ピン番号14
    #define SSD1306_SPI_CS          13     // ピン番号17
  
画像からGP10、GP11、GP13のSPIはSPI1なので  
ソースコードの例ではSPIのポート番号は下記のようにspi1を指定してます  
`#define SSD1306_SPI_PORT        spi1`
  
![pico pinout](https://github.com/yamayamaru/yama_2_GFX_SSD1306/blob/main/img/raspberrypipicopinout.jpg)  

