#include "MyNetUtils.h"
#include <ElegantOTA.h>
#include "DisplayManager.h"

NetworkState currentNetState = STATE_IDLE;
unsigned long connectionStartTime = 0;
const unsigned long CONNECTION_TIMEOUT = 15000; 

AsyncWebServer server(80);

static WifiConfig g_bootConfig;
static int g_candidateOrder[MAX_WIFI_PROFILES];
static int g_candidateCount = 0;
static int g_candidateCursor = 0;
static int g_attemptNumber = 0;

static void resetAttemptState() {
  g_candidateCount = 0;
  g_candidateCursor = 0;
  g_attemptNumber = 0;
  for (int i = 0; i < MAX_WIFI_PROFILES; i++) {
    g_candidateOrder[i] = -1;
  }
}

static void buildCandidateOrder(const WifiConfig &config) {
  int slotRssi[MAX_WIFI_PROFILES];
  bool slotHasSsid[MAX_WIFI_PROFILES];
  bool slotSelected[MAX_WIFI_PROFILES];

  for (int i = 0; i < MAX_WIFI_PROFILES; i++) {
    slotRssi[i] = -1000;
    slotHasSsid[i] = (config.profiles[i].ssid != "");
    slotSelected[i] = false;
  }

  WiFi.mode(WIFI_STA);
  delay(100);
  int n = WiFi.scanNetworks(false, false, false, 300);

  if (n > 0) {
    for (int i = 0; i < n; ++i) {
      String scannedSSID = WiFi.SSID(i);
      int scannedRSSI = WiFi.RSSI(i);

      for (int j = 0; j < MAX_WIFI_PROFILES; j++) {
        if (slotHasSsid[j] && config.profiles[j].ssid == scannedSSID) {
          if (scannedRSSI > slotRssi[j]) {
            slotRssi[j] = scannedRSSI;
          }
        }
      }
    }
  }
  WiFi.scanDelete();

  // Add visible profiles first by strongest RSSI.
  for (int pick = 0; pick < MAX_WIFI_PROFILES; pick++) {
    int bestSlot = -1;
    int bestRssi = -1000;
    for (int j = 0; j < MAX_WIFI_PROFILES; j++) {
      if (slotHasSsid[j] && !slotSelected[j] && slotRssi[j] > bestRssi) {
        bestRssi = slotRssi[j];
        bestSlot = j;
      }
    }

    if (bestSlot >= 0 && bestRssi > -1000) {
      g_candidateOrder[g_candidateCount++] = bestSlot;
      slotSelected[bestSlot] = true;
    }
  }

  // Then add non-visible saved profiles by slot order.
  for (int j = 0; j < MAX_WIFI_PROFILES; j++) {
    if (slotHasSsid[j] && !slotSelected[j]) {
      g_candidateOrder[g_candidateCount++] = j;
      slotSelected[j] = true;
    }
  }

  Serial.print("[ROAM] Candidate list built: ");
  Serial.print(g_candidateCount);
  Serial.println(" profiles");
}

static bool hasNextCandidate() {
  return g_candidateCursor < g_candidateCount;
}

static bool startNextProfileAttempt() {
  while (hasNextCandidate()) {
    int slot = g_candidateOrder[g_candidateCursor++];
    if (slot < 0 || slot >= MAX_WIFI_PROFILES) {
      continue;
    }

    String targetSSID = g_bootConfig.profiles[slot].ssid;
    String targetPass = g_bootConfig.profiles[slot].password;

    if (targetSSID == "") {
      continue;
    }

    g_attemptNumber++;

    Serial.print("[ROAM] Attempt ");
    Serial.print(g_attemptNumber);
    Serial.print("/");
    Serial.print(g_candidateCount);
    Serial.print(" -> SSID: ");
    Serial.println(targetSSID);

    drawNetworkStatus("STATION_INIT", targetSSID, "0.0.0.0",
                      "Attempt " + String(g_attemptNumber) + "/" + String(g_candidateCount));

    WiFi.mode(WIFI_STA);
    delay(500);
    WiFi.begin(targetSSID.c_str(), targetPass.c_str());

    currentNetState = STATE_CONNECTING;
    connectionStartTime = millis();
    return true;
  }

  return false;
}

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
    <h2>WiFi Configuration Matrix</h2>
    <button id="scan-btn" onclick="scanNetworks()">Scan for Networks</button>
    <div id="networks" class="network-list"><p style="text-align:center;color:#666;">No scan performed yet.</p></div>
    <form action="/save" method="POST">
      <label for="ssid">Network Name (SSID):</label>
      <input type="text" id="ssid" name="ssid" placeholder="Select or type SSID" required>
      <label for="password">Password:</label>
      <input type="password" id="password" name="password" placeholder="Enter WiFi Password">
      <input type="submit" value="Add to Stored Profiles">
    </form>
  </div>
</body>
</html>
)rawliteral";

void initNetwork(const WifiConfig &config) {
  bool profilesExist = false;
  for (int i = 0; i < MAX_WIFI_PROFILES; i++) {
    if (config.profiles[i].ssid != "") {
      profilesExist = true;
      break;
    }
  }

  if (!profilesExist) {
    Serial.println("No saved configurations found. Booting to Access Point mode...");
    startSoftAP();
    return;
  }

  Serial.println(">>> Scanning environment for stored profiles... <<<");
  drawNetworkStatus("ROAMING", "All Stored", "0.0.0.0", "Scanning Matrix...");

  g_bootConfig = config;
  resetAttemptState();
  buildCandidateOrder(g_bootConfig);

  if (g_candidateCount == 0) {
    Serial.println("[ROAM] No valid candidate profiles. Booting to SoftAP mode...");
    startSoftAP();
    return;
  }

  if (!startNextProfileAttempt()) {
    Serial.println("[ROAM] Failed to start initial profile attempt. Booting to SoftAP mode...");
    startSoftAP();
  }
}

void checkNetworkStatus() {
  if (currentNetState == STATE_CONNECTING) {
    if (WiFi.status() == WL_CONNECTED) {
      currentNetState = STATE_CONNECTED;
      Serial.println("\n>>> SUCCESS: Connected as a Station! <<<");
      Serial.print("SSID Connected: "); Serial.println(WiFi.SSID());
      Serial.print("IP Address allocated: "); Serial.println(WiFi.localIP());
      Serial.println("========================================\n");
      
      drawNetworkStatus("STATION", WiFi.SSID(), WiFi.localIP().toString(), "Connected!");
      startWebServer();
    } 
    else if (millis() - connectionStartTime > CONNECTION_TIMEOUT) {
      Serial.print("[ROAM] Timeout on SSID: ");
      Serial.println(WiFi.SSID());
      WiFi.disconnect();

      if (startNextProfileAttempt()) {
        return;
      }

      Serial.println("\n>>> All profile attempts exhausted. Falling back to SoftAP Mode... <<<");
      drawNetworkStatus("FALLBACK", "None", "0.0.0.0", "No Profile Linked");
      delay(1000); 
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
    
    drawNetworkStatus("ACCESS POINT", uniqueSSID, WiFi.softAPIP().toString(), "Portal Active");
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
    int n = WiFi.scanComplete();
    if (n == WIFI_SCAN_FAILED || n == WIFI_SCAN_RUNNING) {
      if (n == WIFI_SCAN_FAILED) WiFi.scanNetworks(true); 
      request->send(200, "text/plain", "<p style='text-align:center;color:#666;'>Scanning in progress... Click again in 2 seconds.</p>");
      return;
    }

    String response = "";
    for (int i = 0; i < n; ++i) {
      String ssid = WiFi.SSID(i);
      int rssi = WiFi.RSSI(i);
      response += "<div class='network-item' onclick=\"selectSSID('" + ssid + "')\">";
      response += ssid + " (" + String(rssi) + " dBm)</div>";
    }
    WiFi.scanDelete();
    request->send(200, "text/html", response);
  });

  server.on("/save", HTTP_POST, [](AsyncWebServerRequest *request){
    WifiConfig activeConfig;
    loadConfig(activeConfig);

    String incomingSSID = "";
    String incomingPass = "";

    if (request->hasParam("ssid", true)) incomingSSID = request->getParam("ssid", true)->value();
    if (request->hasParam("password", true)) incomingPass = request->getParam("password", true)->value();

    bool updated = false;
    for (int i = 0; i < MAX_WIFI_PROFILES; i++) {
      if (activeConfig.profiles[i].ssid == incomingSSID) {
        activeConfig.profiles[i].password = incomingPass;
        updated = true;
        break;
      }
    }

    if (!updated) {
      for (int i = 0; i < MAX_WIFI_PROFILES; i++) {
        if (activeConfig.profiles[i].ssid == "") {
          activeConfig.profiles[i].ssid = incomingSSID;
          activeConfig.profiles[i].password = incomingPass;
          updated = true;
          break;
        }
      }
    }

    if (!updated) {
      Serial.println("Profile Matrix Full. Overwriting slot 0 profile...");
      activeConfig.profiles[0].ssid = incomingSSID; 
      activeConfig.profiles[0].password = incomingPass; 
    }

    Serial.println("\n============================= Profile Storage Event =============================");
    Serial.print("Saved profile -> '"); Serial.print(incomingSSID); Serial.println("'");
    Serial.println("================================================================================\n");

    drawNetworkStatus("DATABASE", incomingSSID, "0.0.0.0", "Saving Profile...");

    request->send(200, "text/html", "<h3>Profile Saved Successfully! Restarting to scan matrix profiles...</h3>");
    delay(500); 
    saveConfig(activeConfig);
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
