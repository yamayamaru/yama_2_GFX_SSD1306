使用にはyama_2_GFXが別途必要になります。  
<https://github.com/yamayamaru/yama_2_GFX>  
  
yama_2_GFX.h  
yama_2_GFX.cpp  
yama_2_GFX_SSD1306.h  
yama_2_GFX_SSD1306.cpp  
yama_2_GFX_SSD1306_graphictest_i2c.cpp  
CMakeLists.txt  
  
上記ファイルを同じフォルダに置きpico-sdkでビルドしてください。  

ソースコードに書かれてるSSD1306_SDA、SSD1306_SCLの番号はGPIOの番号です  
下記画像で緑で書かれてる番号になります。  
ソースコードの例では実際のピン番号は下記のようになります。  
  
    #define SSD1306_SDA         2      // ピン番号4
    #define SSD1306_SCL         3      // ピン番号5
  
画像からGP2、GP3のI2CはI2C1なので  
ソースコードの例ではI2Cのポート番号は下記のようにi2c1を指定してます  
`#define SSD1306_I2C_PORT    i2c1`
  
SSD1306のI2Cアドレスは製品によって違います。  
私が使ったものは0x3cでしたのでソースコードの例では0x3Cを指定してます。  
`#define SSD1306_I2C_ADDRESS 0x3C`
  
SSD1306のI2Cアドレスがわからない場合はRaspberry Pi Picoで  
MicroPythonを使って調べることができます。  
ここのサイトからMicroPythonをダウンロードしてPicoに書き込みます。  
<https://micropython.org/download/rp2-pico/>  
  
次にPicoのGPIO2(ピン番号4)にSSD1306のSDAをGPIO3(ピン番号5)にSCKをつないで  
MicroPythonで下記プログラムを実行するとアドレスが表示されます。

    from machine import Pin, I2C
    I2C_SDA  = 2
    I2C_SCL  = 3
    I2C_PORT = 1
    i2c = I2C(I2C_PORT, sda=Pin(I2C_SDA), scl=Pin(I2C_SCL), freq=400000)
    for i in i2c.scan():
        print(hex(i))
  
pico-sdkのexsamplesの中のi2c/bus_scan/bus_scan.cをビルドして実行しても  
I2Cアドレスを調べることができます。  
  
![pico pinout](https://github.com/yamayamaru/yama_2_GFX_SSD1306/blob/main/img/raspberrypipicopinout.jpg)  
