# yama_2_GFX_SSD1306

yama_2_GFX_SSD1306はRaspberry Pi PicoのC/C++開発ツールのpico-sdk用のグラフィックスライブラリです。  
C++で書かれています。  
yama_2_GFX_SSD1306はSSD1306を搭載したOLED用のライブラリになってます。  
使用にあたってはyama_2_GFXが必要になります。  
<https://github.com/yamayamaru/yama_2_GFX>  
  
yama_2_GFX_SSD1306を使う場合の注意点としては  
描画をしたら、必ず表示させたいタイミングでdisplay()メソッドを実行してください。  
描画してもメモリ上のバッファに書き込まれるだけで、OLEDにはデータは転送されません。  
display()メソッドを実行することでメモリ上のバッファの内容がOLEDに転送され  
OLEDにバッファの内容が反映されます。  
  
  

yama_2_GFX_SSD1306はAdafruit_SSD1306を元に作られています  
    <https://github.com/adafruit/Adafruit_SSD1306>   
  
## 変更履歴
0.3 初版公開
