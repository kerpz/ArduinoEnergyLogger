void webserverSetup() {
  webServer.on("/", handleRoot);
  webServer.on("/info", handleInfo);
  webServer.on("/config", handleConfig);
  webServer.on("/configsave", handleConfigSave);
  webServer.on("/reset", handleReset);
  //webServer.on("/generate_204", handleRoot);  //Android captive portal. Maybe not needed. Might be handled by notFound handler.
  //webServer.on("/fwlink", handleRoot);  //Microsoft captive portal. Maybe not needed. Might be handled by notFound handler.
  webServer.on("/update", handleUpdate);
  webServer.on("/updatesave", HTTP_POST, []() {
    webServer.sendHeader("Connection", "close");
    webServer.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = webServer.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.setDebugOutput(true);
      //WiFiUDP::stopAll();
      Serial.printf("Update: %s\n", upload.filename.c_str());
      uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
      if (!Update.begin(maxSketchSpace)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
      Serial.setDebugOutput(false);
    }
    yield();
  });
  webServer.onNotFound(handleNotFound);
  webServer.begin(); // Web server start
}


const char HTTP_STYLE[] PROGMEM = "<style>"
  "body {color: #333;font-family: Century Gothic, sans-serif;font-size: 16px;line-height: 24px;margin: 0;padding: 0}"
  "nav{background: #3861d1;color: #fff;display: block;font-size: 1.3em;padding: 1em}"
  "nav b{display: block;font-size: 1.5em;margin-bottom: 0.5em}"
  "textarea,input,select{outline: 0;font-size: 14px;border: 1px solid #ccc;padding: 8px;width: 100%;box-sizing: border-box}"
  "input[type='checkbox']{float: left;width: 20px}"
  "textarea:focus,input:focus,select:focus{border-color: #5ab}"
  ".container{margin: auto;width: 90%}"
  "@media(min-width:1200px){.container{margin: auto;width: 30%}}"
  "@media(min-width:768px) and (max-width:1200px){.container{margin: auto;width: 50%}}"
  //"h1{font-size: 3em}"
  //".btn{background: #0ae;border-radius: 4px;border: 0;color: #fff;cursor: pointer;font-size: 1.5em;display: inline-block;margin: 2px 0;padding: 10px 14px 11px;width: 100%}"
  ".btn{background: #0ae;border-radius: 4px;border: 0;color: #fff;cursor: pointer;font-size: 1.5em;display: inline-block;margin: 2px 0;padding: 10px 0px 11px;width: 100%;text-align: center;text-decoration: none}"
  ".btn:hover{background: #09d}"
  ".btn:active,.btn:focus{background: #08b}"
  "label>*{display: inline}"
  "form>*{display: block;margin-bottom: 10px}"
  ".msg{background: #def;border-left: 5px solid #59d;padding: 1.5em}"
  //".q{float: right;width: 64px;text-align: right}"
  //".l{background: url('data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAMAAABEpIrGAAAALVBMVEX///8EBwfBwsLw8PAzNjaCg4NTVVUjJiZDRUUUFxdiZGSho6OSk5Pg4eFydHTCjaf3AAAAZElEQVQ4je2NSw7AIAhEBamKn97/uMXEGBvozkWb9C2Zx4xzWykBhFAeYp9gkLyZE0zIMno9n4g19hmdY39scwqVkOXaxph0ZCXQcqxSpgQpONa59wkRDOL93eAXvimwlbPbwwVAegLS1HGfZAAAAABJRU5ErkJggg==') no-repeat left center;background-size: 1em}"
  ".table {table-layout : fixed;width: 100%}"
  ".table td{padding:.5em;text-align:left}"
  ".table tbody>:nth-child(2n-1){background:#ddd}"
  "</style>";
//const char HTTP_SCRIPT[] PROGMEM = "<script>function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();}</script>";

String createPage(String body) {
  String html = "";
  html += F( "<!DOCTYPE html>"
             "<html lang='en'>"
             "<head><title>ESSMonitor v1.0</title>"
             "<meta name='viewport' content='width=device-width,initial-scale=1,user-scalable=no'>" );
  html += FPSTR(HTTP_STYLE);
  //html += FPSTR(HTTP_SCRIPT);
  html += F( "</head>"
             "<body>"
             "<div class='container'>" );
  html += body;
  html += F( "</div>"
             "</body>"
             "</html>" );
  return html;
}

void handleRoot() {
  webServer.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  webServer.sendHeader("Pragma", "no-cache");
  webServer.sendHeader("Expires", "-1");

  String body = F( "<nav><b>ESSMonitor v1.0</b>Main Page</nav><br>" );
  
  body += F( "<a class=\"btn\" href=\"/info\">Information</a><br><br>"
             "<a class=\"btn\" href=\"/config\">Configuration</a><br><br>"
             "<a class=\"btn\" href=\"/update\">Update</a><br><br>" );

  body += F( "<div class=\"msg\">" );

  if (WiFi.SSID() != "") {
    body += F( "Configured to connect to access point " );
    body += WiFi.SSID();
    if (WiFi.status() == WL_CONNECTED) {
      body += F( " and <strong>currently connected</strong> on IP <a href=\"http://" );
      body += WiFi.localIP().toString();
      body += F( "/\">" );
      body += WiFi.localIP().toString();
      body += F( "</a>" );
    }
    else {
      body += F(" but <strong>not currently connected</strong> to network.");
    }
  }
  else {
    body += F("No network currently configured.");
  }

  body += F( "</div><br><br>" );

  webServer.send(200, "text/html", createPage(body));
}

void handleInfo() {
  webServer.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  webServer.sendHeader("Pragma", "no-cache");
  webServer.sendHeader("Expires", "-1");

  String body = F( "<nav><b>ESSMonitor v1.0</b>Information</nav><br>" );

  body += F("<h3>Device Data</h3>");
  body += F("<table class=\"table\"><tbody><tr><td>Chip ID</td><td>");
  body += ESP.getChipId();
  body += F("</td></tr>");
  body += F("<tr><td>Flash Chip ID</td><td>");
  body += ESP.getFlashChipId();
  body += F("</td></tr>");
  body += F("<tr><td>IDE Flash Size</td><td>");
  body += ESP.getFlashChipSize();
  body += F(" bytes</td></tr>");
  body += F("<tr><td>Real Flash Size</td><td>");
  body += ESP.getFlashChipRealSize();
  body += F(" bytes</td></tr>");
  body += F("<tr><td>Access Point IP</td><td>");
  body += WiFi.softAPIP().toString();
  body += F("</td></tr>");
  body += F("<tr><td>Access Point MAC</td><td>");
  body += WiFi.softAPmacAddress();
  body += F("</td></tr>");

  body += F("<tr><td>SSID</td><td>");
  body += WiFi.SSID();
  body += F("</td></tr>");

  body += F("<tr><td>Station IP</td><td>");
  body += WiFi.localIP().toString();
  body += F("</td></tr>");

  body += F("<tr><td>Station MAC</td><td>");
  body += WiFi.macAddress();
  body += F("</td></tr>");
  body += F("</tbody></table><br>");

  body += F("<h3>CT Data</h3>");
  body += F("<table class=\"table\"><tbody><tr><td>CT Calibration</td><td>");
  body += String(ct_calibration);
  body += F(" amps</td></tr><tr><td>CT Voltage</td><td>");
  body += String(ct_voltage);
  body += F(" volts</td></tr><tr><td>CT Power</td><td>");
  body += String(ct_power / 1000.0, 3);
  body += F(" kW</td></tr><tr><td>CT Pf</td><td>");
  body += String(ct_pf);
  body += F("</td></tr></tbody></table><br>");

  body += F("<h3>SRNE Data</h3>");
  body += F("<table class=\"table\"><tbody><tr><td>Battery Voltage</td><td>");
  body += String(battery_voltage);
  body += F(" volts</td></tr><tr><td>Battery Charge</td><td>");
  body += String(battery_charge);
  body += F(" %</td></tr><tr><td>PV Voltage</td><td>");
  body += String(pv_voltage);
  body += F(" volts</td></tr><tr><td>PV Power</td><td>");
  body += String(pv_power);
  body += F(" watts</td></tr><tr><td>Mppt Voltage</td><td>");
  body += String(mppt_voltage);
  body += F(" volts</td></tr><tr><td>Mppt Power</td><td>");
  body += String(mppt_power);
  body += F(" watts</td></tr><tr><td>Battery Temperature</td><td>");
  body += String(battery_temperature);
  body += F(" C</td></tr><tr><td>Mppt Temperature</td><td>");
  body += String(mppt_temperature);
  body += F(" C</td></tr><tr><td>DC Voltage</td><td>");
  body += String(dc_voltage);
  body += F(" volts</td></tr><tr><td>DC Power</td><td>");
  body += String(dc_power);
  body += F("</td></tr></tbody></table><br>");

  body += F("<h3>SNAT Data</h3>");
  body += F("<table class=\"table\"><tbody><tr><td>Input Voltage</td><td>");
  body += String(inv_in_voltage);
  body += F(" volts</td></tr><tr><td>Input Fault Voltage</td><td>");
  body += String(inv_in_fault_voltage);
  body += F(" volts</td></tr><tr><td>Output Voltage</td><td>");
  body += String(inv_out_voltage);
  body += F(" volts</td></tr><tr><td>Output Power</td><td>");
  body += String(inv_out_power);
  body += F(" watts</td></tr><tr><td>Input Frequency</td><td>");
  body += String(inv_in_frequency);
  body += F(" hertz</td></tr><tr><td>Cell Voltage</td><td>");
  body += String(inv_cell_voltage);
  body += F(" volts</td></tr><tr><td>Status Flags</td><td>");
  body += inv_flags;
  body += F("</td></tr></tbody></table><br>");

  body += F("<h3>Error Counter</h3>");
  body += F("<table class=\"table\"><tbody><tr><td>Wifi Error</td><td>");
  body += String(wifi_error);
  body += F("</td></tr><tr><td>SRNE Error</td><td>");
  body += String(modbus_error);
  body += F("</td></tr><tr><td>SNAT Error</td><td>");
  body += String(megatec_error);
  body += F("</td></tr><tr><td>Post Error</td><td>");
  body += String(post_error);
  body += F("</td></tr><tr><td>Http Error</td><td>");
  body += String(http_error);
  body += F("</td></tr><tr><td>Http Code</td><td>");
  body += String(http_code);
  body += F("</td></tr><tr><td>Run Time (minutes)</td><td>");
  body += String(run_time);
  body += F("</td></tr></tbody></table><br><br>");

  body += F( "<a class=\"btn\" href=\"/reset\">Reset</a><br>" );
  body += F( "<a class=\"btn\" href=\"/\">Back</a><br><br>" );
  
  webServer.send(200, "text/html", createPage(body));
}

void handleConfig() {
  Serial.println("scan start");
  int n = WiFi.scanNetworks();
  Serial.println("scan done");

  webServer.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  webServer.sendHeader("Pragma", "no-cache");
  webServer.sendHeader("Expires", "-1");

  String body = F( "<nav><b>ESSMonitor v1.0</b>Configuration</nav><br>" );
  
  body += F( "<form method=\"post\" action=\"/configsave\">"
             "<h3>Network Config</h3>"
             "<label>SSID</label>"
             "<select name=\"n\">" );
  if (n > 0) {
    for (int i = 0; i < n; i++) {
      body += F( "<option value=\"" );
      body += WiFi.SSID(i);
      if (String(ssid) == WiFi.SSID(i)) {
        body += F( "\" selected>" );
      }
      else {
        body += F( "\">" );
      }
      
      body += WiFi.SSID(i);
      body += F( " " );
      body += getRSSIasQuality(WiFi.RSSI(i));
      body += F( "% " );
      if (WiFi.encryptionType(i) != ENC_TYPE_NONE) {
        body += F( " &#9906" );
      }
      body += F( "</option>" );
    }
  } else {
    body += F( "<option value=\"\"></option>" );
  }
  body += F( "</select><label>Password</label><input type=\"password\" name=\"p\" length=32 placeholder=\"password\" value=\"" );
  body += password;
  body += F( "\"><br>" );

  body += F( "<h3>Post Config</h3>"
             "<label>Post</label>"
             "<select name=\"post_e\">"
             "<option value=\"0\"" );
  if (post_enable == 0) body += F( " selected" );
  body += F( ">Disable</option><option value=\"1\"" );
  if (post_enable == 1) body += F( " selected" );
  body += F( ">CSV</option><option value=\"2\"" );
  if (post_enable == 2) body += F( " selected" );
  body += F( ">Influx</option></select><label>API Url</label><input type=\"text\" name=\"u\" length=256 placeholder=\"api_url\" value=\"" );
  body += api_url;
  body += F( "\"><label>API Key</label><input type=\"text\" name=\"k\" length=32 placeholder=\"api_key\" value=\"" );
  body += api_key;
  body += F( "\"><label>HTTP Timeout</label><input type=\"text\" name=\"t\" length=10 placeholder=\"http_timeout (ms)\" value=\"" );
  body += http_timeout;
  body += F( "\"><br>" );

  body += F( "<h3>CT Config</h3>"
             "<label>CT</label>"
             "<select name=\"ct_e\">"
             "<option value=\"0\"" );
  if (ct_enable == 0) body += F( " selected" );
  body += F( ">Disable</option><option value=\"1\"" );
  if (ct_enable == 1) body += F( " selected" );
  body += F( ">Enable</option></select><label>Calibration</label><input type=\"text\" name=\"c\" length=3 placeholder=\"calibration (amps)\" value=\"" );
  body += ct_calibration;
  body += F( "\"><label>Voltage</label><input type=\"text\" name=\"v\" length=3 placeholder=\"voltage\" value=\"" );
  body += ct_voltage;
  body += F( "\"><label>Power Factor</label><input type=\"text\" name=\"f\" length=3 placeholder=\"pf (0.0 - 1.0)\" value=\"" );
  body += ct_pf;
  body += F( "\"><br>" );

  body += F( "<h3>SRNE Config</h3>"
             "<label>SRNE</label>"
             "<select name=\"srne_e\">"
             "<option value=\"0\"" );
  if (srne_enable == 0) body += F( " selected" );
  body += F( ">Disable</option><option value=\"1\"" );
  if (srne_enable == 1) body += F( " selected" );
  body += F( ">Enable</option>"
             "</select><br>" );

  body += F( "<h3>SNAT Config</h3>"
             "<label>SNAT</label>"
             "<select name=\"snat_e\">"
             "<option value=\"0\"" );
  if (snat_enable == 0) body += F( " selected" );
  body += F( ">Disable</option><option value=\"1\"" );
  if (snat_enable == 1) body += F( " selected" );
  body += F( ">Enable</option></select><br>" );

  body += F( "<h3>Display Config</h3>"
             "<label>Display</label>"
             "<select name=\"display_e\">"
             "<option value=\"0\"" );
  if (display_enable == 0) body += F( " selected" );
  body += F( ">Disable</option><option value=\"1\"" );
  if (display_enable == 1) body += F( " selected" );
  body += F( ">Enable</option>"
             "</select><br>" );

  body += F( "<h3>Beep Config</h3>"
             "<label>Beep</label>"
             "<select name=\"beep_e\">"
             "<option value=\"0\"" );
  if (beep_enable == 0) body += F( " selected" );
  body += F( ">Disable</option><option value=\"1\"" );
  if (beep_enable == 1) body += F( " selected" );
  body += F( ">Enable</option>"
             "</select><br>"
             "<button class=\"btn\" type=\"submit\">Save</button>"
             "</form>"
             "<a class=\"btn\" href=\"/\">Back</a><br><br>" );

  webServer.send(200, "text/html", createPage(body));
}


void handleConfigSave() {
  Serial.println("config save");
  webServer.arg("n").toCharArray(ssid, sizeof(ssid) - 1);
  webServer.arg("p").toCharArray(password, sizeof(password) - 1);
  post_enable = webServer.arg("post_e").toInt();
  webServer.arg("u").toCharArray(api_url, sizeof(api_url) - 1);
  webServer.arg("k").toCharArray(api_key, sizeof(api_key) - 1);
  http_timeout = webServer.arg("t").toInt();
  
  ct_enable = webServer.arg("ct_e").toInt();
  ct_calibration = webServer.arg("c").toDouble();
  ct_voltage = webServer.arg("v").toDouble();
  ct_pf = webServer.arg("f").toDouble();

  srne_enable = webServer.arg("srne_e").toInt();
  snat_enable = webServer.arg("snat_e").toInt();
  display_enable = webServer.arg("display_e").toInt();
  beep_enable = webServer.arg("beep_e").toInt();

  webServer.sendHeader("Location", "config", true);
  webServer.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  webServer.sendHeader("Pragma", "no-cache");
  webServer.sendHeader("Expires", "-1");
  webServer.send(302, "text/plain", "");    // Empty content inhibits Content-length header so we have to close the socket ourselves.
  webServer.client().stop(); // Stop is needed because we sent no content length
  saveConfig();
}

void handleReset() {
  wifi_error = 0;
  post_error = 0;
  http_error = 0;
  http_code = 0;

  modbus_error = 0;
  megatec_error = 0;

  run_time = 0;

  webServer.sendHeader("Location", "/", true);
  webServer.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  webServer.sendHeader("Pragma", "no-cache");
  webServer.sendHeader("Expires", "-1");
  webServer.send(302, "text/plain", "");    // Empty content inhibits Content-length header so we have to close the socket ourselves.
  webServer.client().stop(); // Stop is needed because we sent no content length
}

void handleUpdate() {
  webServer.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  webServer.sendHeader("Pragma", "no-cache");
  webServer.sendHeader("Expires", "-1");

  String body = F( "<nav><b>ESSMonitor v1.0</b>Update</nav><br>" );
  
  body += F( "<h3>Upload Firmware</h3>"
             "<form method=\"post\" action=\"updatesave\" enctype=\"multipart/form-data\">"
             "<input type=\"file\" name=\"update\"><br>"
             "<button class=\"btn\" type=\"submit\">Upload</button>"
             "</form>"
             "<a class=\"btn\" href=\"/\">Back</a><br><br>" );

  webServer.send(200, "text/html", createPage(body));
}

void handleNotFound() {
  String message = F("File Not Found\n\n");
  message += F("URI: ");
  message += webServer.uri();
  message += F("\nMethod: ");
  message += (webServer.method() == HTTP_GET) ? "GET" : "POST";
  message += F("\nArguments: ");
  message += webServer.args();
  message += F("\n");

  for (uint8_t i = 0; i < webServer.args(); i++) {
    message += String(F(" ")) + webServer.argName(i) + F(": ") + webServer.arg(i) + F("\n");
  }
  webServer.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  webServer.sendHeader("Pragma", "no-cache");
  webServer.sendHeader("Expires", "-1");
  webServer.send(404, "text/plain", message);
}
