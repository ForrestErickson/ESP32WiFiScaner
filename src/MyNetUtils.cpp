#include "MyNetUtils.h"
#include <ElegantOTA.h>

NetworkState currentNetState = STATE_IDLE;
unsigned long connectionStartTime = 0;
const unsigned long CONNECTION_TIMEOUT = 10000; 

AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>WiFi Setup Portal</title>
  <style>
    body { font-family: Arial, sans-serif; margin:20px; background-color:#f4f4f4; text-align:center; }
    .container { max-width:400px; margin:0 auto; background:white; padding:20px; border-radius:8px; box-shadow:0 2px 4px rgba(0,0,0,0.1); }
    h2 { color:#333; }
    input[type=text], input[type=password], select { width:100%; padding:10px; margin:8px 0; box-sizing:border-box; border:1px solid #ccc; border-radius:4px; }
    input[type=submit], button { width:100%; background-color:#4CAF50; color:white; padding:10px; margin:8px 0; border:none; border-radius:4px; cursor:pointer; font-size:16px; }
    button { background-color:#008CBA; }
    .network-list { text-align:left; margin:15px 0; max-height:150px; overflow-y:auto; border:1px solid #ddd; padding:5px; border-radius:4px; }
    .network-item { padding:8px; border-bottom:1px solid #eee; cursor:pointer; }
    .network-item:hover { background-color:#f0f0f0; }
  </style>
  <script>
    function scanNetworks() {
      var btn = document.getElementById('scan-btn');
      btn.innerText = 'Scanning...';
      btn.disabled = true;
      fetch('/scan').then(response => response.text()).then(data => {
        document.getElementById('networks').innerHTML = data;
        btn.innerText = 'Scan for Networks';
        btn.disabled = false;
      });
    }
    function selectSSID(ssid) {
      document.getElementById('ssid').value = ssid;
    }
  </script>
</head>
<body>
  <div class="container">
    <h2>WiFi Configuration</h2>
    <button id="scan-btn" onclick="scanNetworks()">Scan for Networks</button>
    <div id="networks" class="network-list"><p style="text-align:center;color:#666;">No scan performed yet.</p></div>
    <form action="/save" method="POST">
      <label for="ssid">Network Name (SSID):</label>
      <input type="text" id="ssid" name="ssid" placeholder="Select or type SSID" required>
      <label for="password">Password:</label>
      <input type="password" id="password" name="password" placeholder="Enter WiFi Password">
      <input type="submit" value="Save and Connect">
    </form>
  </div>
</body>
</html>
)rawliteral";

void initNetwork(const WifiConfig &config) {
  if (config.ssid == "") {
    Serial.println("No saved credentials found. Booting directly into Access Point mode...");
    startSoftAP();
    return;
  }

  Serial.print("Attempting to connect to SSID: ");
  Serial.println(config.ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(config.ssid.c_str(), config.password.c_str());
  
  currentNetState = STATE_CONNECTING;
  connectionStartTime = millis();
}

void checkNetworkStatus() {
  if (currentNetState == STATE_CONNECTING) {
    if (WiFi.status() == WL_CONNECTED) {
      currentNetState = STATE_CONNECTED;
      Serial.println("\n>>> SUCCESS: Connected to home network! <<<");
      Serial.print("IP Address allocated: ");
      Serial.println(WiFi.localIP());
      Serial.println("========================================\n");
      
      startWebServer();
    } 
    else if (millis() - connectionStartTime > CONNECTION_TIMEOUT) {
      Serial.println("\n>>> Connection Timeout! Falling back to SoftAP Mode... <<<");
      WiFi.disconnect();
      startSoftAP();
    }
  }
}

void startSoftAP() {
  WiFi.mode(WIFI_AP);
  String macStr = WiFi.softAPmacAddress();
  macStr.replace(":", "");
  String uniqueSSID = String(COMPANY_NAME) + macStr;
  
  if (WiFi.softAP(uniqueSSID.c_str(), NULL)) {
    currentNetState = STATE_AP_MODE;
    Serial.println("========================================");
    Serial.println("Access Point Hotspot Active!");
    Serial.print("SSID:       "); Serial.println(uniqueSSID);
    Serial.print("IP Address: "); Serial.println(WiFi.softAPIP());
    Serial.println("========================================");
    
    startWebServer();
  } else {
    Serial.println("Error: SoftAP Hotspot initialization failed!");
  }
}

void startWebServer() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", index_html);
  });

  server.on("/scan", HTTP_GET, [](AsyncWebServerRequest *request){
    int n = WiFi.scanNetworks(false, false, false, 150); 
    String response = "";
    if (n <= 0) {
      response = "<p style='text-align:center;color:red;'>No networks found.</p>";
    } else {
      for (int i = 0; i < n; ++i) {
        String ssid = WiFi.SSID(i);
        int rssi = WiFi.RSSI(i);
        response += "<div class='network-item' onclick=\"selectSSID('" + ssid + "')\">";
        response += ssid + " (" + String(rssi) + " dBm)</div>";
      }
    }
    WiFi.scanDelete();
    request->send(200, "text/html", response);
  });

  server.on("/save", HTTP_POST, [](AsyncWebServerRequest *request){
    WifiConfig newConfig;
    if (request->hasParam("ssid", true)) {
      newConfig.ssid = request->getParam("ssid", true)->value();
    }
    if (request->hasParam("password", true)) {
      newConfig.password = request->getParam("password", true)->value();
    }
    request->send(200, "text/html", "<h3>Settings Saved! ESP32 is now restarting...</h3>");
    delay(500); 
    saveConfig(newConfig);
    ESP.restart();
  });

  ElegantOTA.begin(&server);
  server.begin();
  Serial.println("Asynchronous Web Server with ElegantOTA active on port 80.");
}

String getEncryptionName(wifi_auth_mode_t authMode) {
  switch (authMode) {
    case WIFI_AUTH_OPEN:            return "Open";
    case WIFI_AUTH_WEP:             return "WEP";
    case WIFI_AUTH_WPA_PSK:         return "WPA";
    case WIFI_AUTH_WPA2_PSK:        return "WPA2";
    case WIFI_AUTH_WPA_WPA2_PSK:    return "WPA/WPA2";
    case WIFI_AUTH_WPA2_ENTERPRISE: return "WPA2-Enterprise";
    case WIFI_AUTH_WPA3_PSK:        return "WPA3";
    case WIFI_AUTH_WPA2_WPA3_PSK:   return "WPA2/WPA3";
    default:                        return "Unknown";
  }
}
