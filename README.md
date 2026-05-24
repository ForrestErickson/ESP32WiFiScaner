# Summary
I am practicing using VSCode and Plaeformio.  
I am practicing using an AI, Google to write code.  
There were iterations required and the AI some times made code that would not compile.  

## Normal operation 
Some serial monitor outputs

```
rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:2
load:0x3fff0030,len:1184
load:0x40078000,len:13232
load:0x40080400,len:3028
entry 0x400805e4
==== ESP23WiFiScaner, Ver: 0.0.3 ====
Built: May 24 2026 14:39:01
========================================

Setup done. Ready.
Button pressed! Starting async scan...
4 networks found:
1: Erickson [Ch: 6] (-39 dBm) - WPA2
2: VRX [Ch: 6] (-64 dBm) - WPA2
3: NETGEAR48 [Ch: 10] (-91 dBm) - WPA2
4: JimandJess1990 [Ch: 1] (-92 dBm) - WPA2/WPA3

```


## History
URL of my dialog with the AI
https://www.google.com/search?q=I+have+a+coding+problem.+The+conditional+test+%22if+%28digitalRead%280%29+%3D%3D+LOW%29%22+never+works+so+I+commented+it+out+.+The+full+code+is+as+follows%3A+%23include+%3CArduino.h%3E+%23include+%22WiFi.h%22+%23define+LED_BUILTIN+2+%23define+BOOT_BUTTON+GPIO_NUM_0+%2F%2F+put+function+declarations+here%3A+int+myFunction%28int%2C+int%29%3B+void+setup%28%29+%7B+pinMode%28LED_BUILTIN%2C+OUTPUT%29%3B+digitalWrite%28LED_BUILTIN%2C+HIGH%29%3B+Serial.begin%28115200%29%3B+while+%28%21Serial%29+%7B+%3B+%7D+pinMode%28BOOT_BUTTON%2C+INPUT_PULLUP%29%3B+%2F%2F+Set+WiFi+to+station+mode+and+disconnect+if+previously+connected+WiFi.mode%28WIFI_STA%29%3B+WiFi.disconnect%28%29%3B+delay%28100%29%3B+Serial.println%28%22Setup+done.+Scanning...%22%29%3B+%2F%2F+put+your+setup+code+here%2C+to+run+once%3A+int+result+%3D+myFunction%282%2C+3%29%3B+Serial.println%28%29%3B+digitalWrite%28LED_BUILTIN%2C+LOW%29%3B+%7D+%2F%2F+end+of+setup%28%29+void+loop%28%29+%7B+%2F%2F+put+your+main+code+here%2C+to+run+repeatedly%3A+%2F%2F+if+%28LOW+%3D%3D+digitalRead%28BOOT_BUTTON%29%29+%2F%2F+if+%28digitalRead%280%29+%3D%3D+LOW%29+if+%28true%29+%7B+Serial.println%28%22Button+pressed%21%22%29%3B+delay%285000%29%3B+%2F%2F+Wait+5+seconds+before+scanning+again+int+n+%3D+WiFi.scanNetworks%28%29%3B+if+%28n+%3D%3D+0%29+%7B+Serial.println%28%22No+networks+found.%22%29%3B+%7D+else+%7B+Serial.print%28n%29%3B+Serial.println%28%22+networks+found%3A%22%29%3B+for+%28int+i+%3D+0%3B+i+%3C+n%3B+%2B%2Bi%29+%7B+%2F%2F+Print+SSID+and+Signal+Strength+%28RSSI%29+Serial.print%28i+%2B+1%29%3B+Serial.print%28%22%3A+%22%29%3B+Serial.print%28WiFi.SSID%28i%29%29%3B+Serial.print%28%22+%28%22%29%3B+Serial.print%28WiFi.RSSI%28i%29%29%3B+Serial.println%28%22%29%22%29%3B+%7D+%7D+Serial.println%28%29%3B+%7D+%2F%2F+end+if+button+delay%2850%29%3B+%7D+%2F%2F+put+function+definitions+here%3A+int+myFunction%28int+x%2C+int+y%29+%7B+return+x+%2B+y%3B+%7D&sca_esv=24dc76abce12f870&sxsrf=ANbL-n6V82e45fTNF_V1x81bWR4745qA3g%3A1779625473830&source=hp&fbs=ADc_l-aN0CWEZBOHjofHoaMMDiKpmAsnXCN5UBx17opt8eaTX-kA7ovRmAIYWC70eULDzbpnQTYs94Mq8g6LHC3WhbQO7M01KHLtT-pJxbMt2v0OUFU0wV2M909hYUiXTjQ4AbORV-HkImh-iq_OfzwLMl_feRJUj1Kp_se3lbxa83kr8uifZU1AfeXV9Ut8JJlZ0nmNloKrZElpWe9VnE9ep4weKSXDqA&aep=1&ntc=1&sa=X&ved=2ahUKEwj0s83B9dGUAxX74MkDHcQNMRIQ2J8OegQIEBAD&biw=995&bih=862&dpr=1&mstk=AUtExfDu5aO_A8HHBz_Gd60DHcJczifCLgvNHG5UP30o0rfU0fuIYAAAgW5jNav6qCGSmHHcqBeQmT9q4CX3xfj7AIVCiFDMZeqL9ffdavGDWIGcssPYlYl5NWfKN4Y1gEc8sh4DNTcum1y4b4Bjj3jstggG1imkvfd9KGp42aIwO3IsIrS9KVu21sQ_EkA9wIG5oxj1AzvggEtQg2Sl2BwGUAyyAWSzSuQcZsSpHpqMFOYeWT7ZBLIauoIgi3PMMsn-8oDAqEXcEilg8VkJY5j4IBfWuAz94L0eoG9L_ej5tQ_tglFhsfKov41ni3_UpA6n4ewm6socuYWRxA&csuir=1&mtid=Du4SaqbWFaiSwbkP7NDo0AY&udm=50
