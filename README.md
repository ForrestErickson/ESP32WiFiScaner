# Summary
I am practicing using VSCode and Plaeformio.  
I am practicing using an AI, Google to write code.  
There were iterations required and the AI some times made code that would not compile.  

## Future Feature List
- [ ] Make a WiFi Manager system for saving multiple SSID / PW.  
- [ ] The device will connect to the strongest SSID as a STA. If no WiFi access point found device will become softAP.  
- [ ] There will be a way to edit and selectively enable / disable saved SSID PW pares without erasing. There will be a way to erase all SSID / PW with double user input for safety.  
- [ ] There will be display on the OLED of status of Wi-Fi events to give user visibility as to the device progress or lack there of.  
- [ ] There will be indication on the LED_BUILTIN of the WiFi Mode / status.  During normal operation as STA the LED_BUILDIN will be on for 100mS and off for 900 mS. For sofAP the LED_BUILTING will be on for 500mS and off for 500 mS.  
- [ ] If during entry of a SSID / PW the connection fails the device should try to fall back to other stored SSID / PW. The SoftAP is a last resort. 


## Claud AI summary as of V0.0.12
[Issue #6](https://github.com/ForrestErickson/ESP32WiFiScaner/issues/6) Claud analysis and Description of V0.0.12 code  


## Normal operation 

Station mode, WiFi setup portal throught Web Browser LAN  
<img width="450" alt="image" src="https://github.com/user-attachments/assets/6fc893a5-62c6-49ed-af04-23d9719089e9" />

### Some serial monitor outputs

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

