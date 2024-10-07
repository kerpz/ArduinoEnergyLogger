//  webserver
#include <ESP8266WebServer.h>
#include "ArduinoJson.h"

#include "webserver.h"

ESP8266WebServer webServer(80);

int getRSSIasQuality(int RSSI)
{
  int quality = 0;

  if (RSSI <= -100)
  {
    quality = 0;
  }
  else if (RSSI >= -50)
  {
    quality = 100;
  }
  else
  {
    quality = 2 * (RSSI + 100);
  }
  return quality;
}

void webserverSetup()
{
  webServer.on("/", HTTP_GET, []()
               { webServer.send(200, "text/html", index_html); });
  webServer.on("/system", HTTP_POST, []()
               {
    byte expand_system = 0;
    byte expand_wifiap = 1;
    byte expand_wifista = 1;
    byte expand_command = 1;

    if (webServer.arg("plain") != "{}") {
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, webServer.arg("plain").c_str());

      if (doc["expand_system"]) expand_system = doc["expand_system"];
      if (doc["expand_wifiap"]) expand_wifiap = doc["expand_wifiap"];
      if (doc["expand_wifista"]) expand_wifista = doc["expand_wifista"];
      if (doc["expand_command"]) expand_command = doc["expand_command"];

      if (doc["reboot"]) ESP.restart();
    }

    String json;
    
    json += "[";
    json += "{\"label\":\"System\",\"name\":\"expand_system\",\"value\":"+String(expand_system)+",\"elements\":[";
    json += "{\"type\":\"text\",\"label\":\"Chip ID\",\"name\":\"chip_id\",\"value\":\""+String(ESP.getChipId())+"\",\"attrib\":\"disabled\"},";
    json += "{\"type\":\"text\",\"label\":\"Free Heap\",\"name\":\"free_heap\",\"value\":\""+String(ESP.getFreeHeap())+"\",\"attrib\":\"disabled\"},";
    json += "{\"type\":\"text\",\"label\":\"Flash ID\",\"name\":\"flash_id\",\"value\":\""+String(ESP.getFlashChipId())+"\",\"attrib\":\"disabled\"},";
    json += "{\"type\":\"text\",\"label\":\"Flash Size\",\"name\":\"flash_size\",\"value\":\""+String(ESP.getFlashChipSize())+"\",\"attrib\":\"disabled\"}";
    //json += "{\"type\":\"text\",\"label\":\"Flash Real Size\",\"name\":\"flash_real_size\",\"value\":\""+String(ESP.getFlashChipRealSize())+"\",\"attrib\":\"disabled\"}";
    json += "]},";
    json += "{\"label\":\"Wifi AP\",\"name\":\"expand_wifiap\",\"value\":"+String(expand_wifiap)+",\"elements\":[";
    json += "{\"type\":\"text\",\"label\":\"AP MAC\",\"name\":\"ap_mac\",\"value\":\""+WiFi.softAPmacAddress()+"\",\"attrib\":\"disabled\"},";
    json += "{\"type\":\"text\",\"label\":\"AP Address\",\"name\":\"ap_address\",\"value\":\""+WiFi.softAPIP().toString()+"\",\"attrib\":\"disabled\"},";
    json += "{\"type\":\"text\",\"label\":\"AP SSID\",\"name\":\"ap_ssid\",\"value\":\""+String(APssid)+"\",\"attrib\":\"disabled\"},";
    json += "{\"type\":\"text\",\"label\":\"Connected Devices\",\"name\":\"connected_devices\",\"value\":\""+String(WiFi.softAPgetStationNum())+"\",\"attrib\":\"disabled\"}";
    json += "]},";
    json += "{\"label\":\"Wifi Station\",\"name\":\"expand_wifista\",\"value\":"+String(expand_wifista)+",\"elements\":[";
    json += "{\"type\":\"text\",\"label\":\"MAC\",\"name\":\"mac\",\"value\":\""+WiFi.macAddress()+"\",\"attrib\":\"disabled\"},";
    json += "{\"type\":\"text\",\"label\":\"Address\",\"name\":\"address\",\"value\":\""+WiFi.localIP().toString()+"\",\"attrib\":\"disabled\"},";
    json += "{\"type\":\"text\",\"label\":\"SSID\",\"name\":\"ssid\",\"value\":\""+WiFi.SSID()+"\",\"attrib\":\"disabled\"}";
    json += "]},";
    json += "{\"label\":\"Command\",\"name\":\"expand_command\",\"value\":"+String(expand_command)+",\"elements\":[";
    json += "{\"type\":\"button\",\"label\":\"REBOOT\",\"name\":\"reboot\",\"value\":\"reboot\",\"confirm\":\"Are you sure you want to reboot?\"}";
    json += "]}";
    json += "]";
    
    webServer.sendHeader("Access-Control-Allow-Origin", "*");
    webServer.send(200, "application/json", json); });
  webServer.on("/scan", HTTP_GET, []()
               {
    String json = "";

    json += "[";
    int n = WiFi.scanComplete();
    if (n == -2) {
      WiFi.scanNetworks(true);
    } else if(n) {
      for (int i = 0; i < n; ++i) {
        if (i) json += ",";
        json += "{";
        json += "\"rssi\":" + String(WiFi.RSSI(i));
        json += "\"signal\":" + String(getRSSIasQuality(WiFi.RSSI(i)));
        json += ",\"ssid\":\"" + WiFi.SSID(i) + "\"";
        json += ",\"bssid\":\"" + WiFi.BSSIDstr(i) + "\"";
        json += ",\"channel\":" + String(WiFi.channel(i));
        json += ",\"secure\":" + String(WiFi.encryptionType(i));
        json += ",\"hidden\":" + String(WiFi.isHidden(i) ? "true" : "false");
        json += "}";
      }
      WiFi.scanDelete();
      if (WiFi.scanComplete() == -2) {
        WiFi.scanNetworks(true);
      }
    }
    json += "]";

    webServer.sendHeader("Access-Control-Allow-Origin", "*");
    webServer.send(200, "application/json", json); });
  webServer.on("/config", HTTP_POST, []()
               {
    if (webServer.arg("plain") != "{}") {
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, webServer.arg("plain").c_str());

      if (doc["ssid"]) strcpy(ssid, (const char*)doc["ssid"]);
      if (doc["password"]) strcpy(password, (const char*)doc["password"]);

      if (doc["beep_enable"]) beep_enable = doc["beep_enable"];
      if (doc["analog_enable"]) analog_enable = doc["analog_enable"];
      if (doc["display_enable"]) display_enable = doc["display_enable"];
      if (doc["ads1115_enable"]) ads1115_enable = doc["ads1115_enable"];
      if (doc["post_enable"]) post_enable = doc["post_enable"];
      if (doc["api_url"]) strcpy(api_url, (const char*)doc["api_url"]);
      if (doc["api_key"]) strcpy(api_key, (const char*)doc["api_key"]);

      //if (doc["ct_enable"]) ct_enable = doc["ct_enable"];
      if (doc["ct_calibration"]) ct_enable = doc["ct_calibration"];
      if (doc["ct_voltage"]) ct_voltage = doc["ct_voltage"];
      if (doc["ct_pf"]) ct_pf = doc["ct_pf"];

      if (doc["srne_enable"]) srne_enable = doc["srne_enable"];
      if (doc["snat_enable"]) snat_enable = doc["snat_enable"];
  
      saveConfig();
      Serial.println("Save config");
    }

    String json;
    String sep;
    int n = WiFi.scanNetworks();

    json += "[";
    json += "{\"label\":\"Wifi Connect\",\"name\":\"expand_wifi\",\"value\":1,\"elements\":[";
    json += "{\"type\":\"select\",\"label\":\"SSID\",\"name\":\"ssid\",\"value\":\"" + WiFi.SSID() + "\",\"options\":[";
    if (n > 0) {
      for (int i = 0; i < n; i++) {
        json += sep+"[\""+WiFi.SSID(i)+"\",\""+WiFi.SSID(i)+" "+String(getRSSIasQuality(WiFi.RSSI(i)))+"%";
        if (WiFi.encryptionType(i) != ENC_TYPE_NONE) {
          json += " &#9906";
        }
        json += "\"]";
        sep = ",";
      }
    } else {
      json += "[\"\",\"\"]";
    }
    json += "]},";
    json += "{\"type\":\"text\",\"label\":\"Password\",\"name\":\"password\",\"value\":\"" + String(password) + "\"}";
    json += "]},";

    json += "{\"label\":\"Components\",\"name\":\"expand_component\",\"value\":1,\"elements\":[";
    json += "{\"type\":\"select\",\"label\":\"Beep\",\"name\":\"beep_enable\",\"value\":\"" + String(beep_enable) + "\",\"options\":[[\"0\",\"Disabled\"],[\"1\",\"Enabled\"]]},";
    json += "{\"type\":\"select\",\"label\":\"Analog\",\"name\":\"analog_enable\",\"value\":\"" + String(analog_enable) + "\",\"options\":[[\"0\",\"Disabled\"],[\"1\",\"Enabled\"]]},";
    json += "{\"type\":\"select\",\"label\":\"Display\",\"name\":\"display_enable\",\"value\":\"" + String(display_enable) + "\",\"options\":[[\"0\",\"Disabled\"],[\"1\",\"Enabled\"]]},";
    json += "{\"type\":\"select\",\"label\":\"ADS1115\",\"name\":\"ads1115_enable\",\"value\":\"" + String(ads1115_enable) + "\",\"options\":[[\"0\",\"Disabled\"],[\"1\",\"Enabled\"]]}";
    json += "]},";

    json += "{\"label\":\"Post\",\"name\":\"expand_post\",\"value\":1,\"elements\":[";
    json += "{\"type\":\"select\",\"label\":\"Post\",\"name\":\"post_enable\",\"value\":\"" + String(post_enable) + "\",\"options\":[[\"0\",\"Disabled\"],[\"1\",\"Enabled\"]]},";
    json += "{\"type\":\"text\",\"label\":\"API url\",\"name\":\"api_url\",\"value\":\"" + String(api_url) + "\"},";
    json += "{\"type\":\"text\",\"label\":\"API key\",\"name\":\"api_key\",\"value\":\"" + String(api_key) + "\"}";
    json += "]},";

    json += "{\"label\":\"Analog\",\"name\":\"expand_analog\",\"value\":1,\"elements\":[";
    //json += "{\"type\":\"select\",\"label\":\"CT Enable\",\"name\":\"ct_enable\",\"value\":\"" + String(ct_enable) + "\",\"options\":[[\"0\",\"Disabled\"],[\"1\",\"Enabled\"]]},";
    json += "{\"type\":\"text\",\"label\":\"CT Calibration\",\"name\":\"ct_calibration\",\"value\":\"" + String(ct_calibration) + "\"},";
    json += "{\"type\":\"text\",\"label\":\"CT Voltage\",\"name\":\"ct_voltage\",\"value\":\"" + String(ct_voltage) + "\"},";
    json += "{\"type\":\"text\",\"label\":\"CT Pf\",\"name\":\"ct_pf\",\"value\":\"" + String(ct_pf) + "\"}";
    json += "]},";

    json += "{\"label\":\"SRNE\",\"name\":\"expand_component\",\"value\":1,\"elements\":[";
    json += "{\"type\":\"select\",\"label\":\"SRNE\",\"name\":\"srne_enable\",\"value\":\"" + String(srne_enable) + "\",\"options\":[[\"0\",\"Disabled\"],[\"1\",\"Enabled\"]]}";
    json += "]},";

    json += "{\"label\":\"SNAT\",\"name\":\"expand_component\",\"value\":1,\"elements\":[";
    json += "{\"type\":\"select\",\"label\":\"SNAT\",\"name\":\"snat_enable\",\"value\":\"" + String(snat_enable) + "\",\"options\":[[\"0\",\"Disabled\"],[\"1\",\"Enabled\"]]}";
    json += "]},";

    json += "{\"label\":\"Page\",\"name\":\"expand_page\",\"value\":1,\"elements\":[";
    json += "{\"type\":\"button\",\"label\":\"UPDATE\",\"name\":\"update\",\"value\":\"update\"}";
    json += "]}";
    json += "]";

    webServer.sendHeader("Access-Control-Allow-Origin", "*");
    webServer.send(200, "application/json", json); });
  webServer.on("/app", HTTP_POST, []()
               {
    byte refresh = 2;

    byte expand_analog = 1;
    byte expand_srne = 1;
    byte expand_snat = 0;
    byte expand_errors = 0;
    byte expand_page = 0;

    if (webServer.arg("plain") != "{}") {
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, webServer.arg("plain").c_str());
      
      if (doc["expand_analog"]) expand_analog = doc["expand_analog"];
      if (doc["expand_srne"]) expand_srne = doc["expand_srne"];
      if (doc["expand_snat"]) expand_snat = doc["expand_snat"];
      if (doc["expand_errors"]) expand_errors = doc["expand_errors"];
      if (doc["expand_page"]) expand_page = doc["expand_page"];

      if (doc["refresh"]) refresh = doc["refresh"];
    }

    String json;
    
    json += "[";

    if (analog_enable) {
      json += "{\"label\":\"Analog Sensor\",\"name\":\"expand_analog\",\"value\":1,\"elements\":[";
      json += "{\"type\":\"text\",\"label\":\"Power\",\"name\":\"a_power\",\"value\":\""+String(a_power)+"\",\"attrib\":\"disabled\"},";
      json += "{\"type\":\"text\",\"label\":\"Current\",\"name\":\"a_current\",\"value\":\""+String(a_current)+"\",\"attrib\":\"disabled\"},";
      json += "{\"type\":\"text\",\"label\":\"Volt\",\"name\":\"a_voltage\",\"value\":\""+String(a_voltage)+"\",\"attrib\":\"disabled\"}";
      json += "]},";
    }

    if (srne_enable) {
      json += "{\"label\":\"SRNE Status\",\"name\":\"expand_status\",\"value\":"+String(expand_srne)+",\"elements\":[";
      json += "{\"type\":\"text\",\"label\":\"Batt Voltage\",\"name\":\"battery_voltage\",\"value\":\""+String(battery_voltage)+"\",\"attrib\":\"disabled\"},";
      json += "{\"type\":\"text\",\"label\":\"Batt Charge\",\"name\":\"battery_charge\",\"value\":\""+String(battery_charge)+"\",\"attrib\":\"disabled\"},";
      json += "{\"type\":\"text\",\"label\":\"Batt Temperature\",\"name\":\"battery_temperature\",\"value\":\""+String(battery_temperature)+"\",\"attrib\":\"disabled\"},";
      json += "{\"type\":\"text\",\"label\":\"PV Voltage\",\"name\":\"pv_voltage\",\"value\":\""+String(pv_voltage)+"\",\"attrib\":\"disabled\"},";
      json += "{\"type\":\"text\",\"label\":\"PV Power\",\"name\":\"pv_power\",\"value\":\""+String(pv_power)+"\",\"attrib\":\"disabled\"},";
      json += "{\"type\":\"text\",\"label\":\"MPPT Voltage\",\"name\":\"mppt_voltage\",\"value\":\""+String(mppt_voltage)+"\",\"attrib\":\"disabled\"},";
      json += "{\"type\":\"text\",\"label\":\"MPPT Power\",\"name\":\"mppt_power\",\"value\":\""+String(mppt_power)+"\",\"attrib\":\"disabled\"},";
      json += "{\"type\":\"text\",\"label\":\"MPPT Temperature\",\"name\":\"mppt_temperature\",\"value\":\""+String(mppt_temperature)+"\",\"attrib\":\"disabled\"},";
      json += "{\"type\":\"text\",\"label\":\"DC Voltage\",\"name\":\"dc_voltage\",\"value\":\""+String(dc_voltage)+"\",\"attrib\":\"disabled\"},";
      json += "{\"type\":\"text\",\"label\":\"DC Current\",\"name\":\"dc_current\",\"value\":\""+String(dc_current)+"\",\"attrib\":\"disabled\"},";
      json += "{\"type\":\"text\",\"label\":\"DC Power\",\"name\":\"dc_power\",\"value\":\""+String(dc_power)+"\",\"attrib\":\"disabled\"}";
      json += "]},";
    }

    if (snat_enable) {
      json += "{\"label\":\"SNAT Status\",\"name\":\"expand_snat\",\"value\":"+String(expand_snat)+",\"elements\":[";
      json += "{\"type\":\"text\",\"label\":\"Input Voltage\",\"name\":\"inv_in_voltage\",\"value\":\""+String(inv_in_voltage)+"\",\"attrib\":\"disabled\"},";
      json += "{\"type\":\"text\",\"label\":\"Input Fault Voltage\",\"name\":\"inv_in_fault_voltage\",\"value\":\""+String(inv_in_fault_voltage)+"\",\"attrib\":\"disabled\"},";
      json += "{\"type\":\"text\",\"label\":\"Output Voltage\",\"name\":\"inv_out_voltage\",\"value\":\""+String(inv_out_voltage)+"\",\"attrib\":\"disabled\"},";
      json += "{\"type\":\"text\",\"label\":\"Out Power\",\"name\":\"inv_out_power\",\"value\":\""+String(inv_out_power)+"\",\"attrib\":\"disabled\"},";
      json += "{\"type\":\"text\",\"label\":\"Frequency\",\"name\":\"inv_in_voltage\",\"value\":\""+String(inv_in_frequency)+"\",\"attrib\":\"disabled\"},";
      json += "{\"type\":\"text\",\"label\":\"Temperature\",\"name\":\"inv_temperature\",\"value\":\""+String(inv_temperature)+"\",\"attrib\":\"disabled\"},";
      json += "{\"type\":\"text\",\"label\":\"Cell Voltage\",\"name\":\"inv_cell_voltage\",\"value\":\""+String(inv_cell_voltage)+"\",\"attrib\":\"disabled\"}";
      //json += "{\"type\":\"text\",\"label\":\"Input Voltage\",\"name\":\"inv_in_voltage\",\"value\":\""+String(inv_flags)+"\",\"attrib\":\"disabled\"},";
      json += "]},";
    }

    json += "{\"label\":\"Error\",\"name\":\"expand_errors\",\"value\":"+String(expand_errors)+",\"elements\":[";
    if (srne_enable) json += "{\"type\":\"text\",\"label\":\"Srne\",\"name\":\"modbus_error\",\"value\":\""+String(modbus_error)+"\",\"attrib\":\"disabled\"},";
    if (snat_enable) json += "{\"type\":\"text\",\"label\":\"Snat\",\"name\":\"megatec_error\",\"value\":\""+String(megatec_error)+"\",\"attrib\":\"disabled\"},";
    json += "{\"type\":\"text\",\"label\":\"Wifi\",\"name\":\"wifi_error\",\"value\":\""+String(wifi_error)+"\",\"attrib\":\"disabled\"},";
    json += "{\"type\":\"text\",\"label\":\"Post\",\"name\":\"post_error\",\"value\":\""+String(post_error)+"\",\"attrib\":\"disabled\"}";
    json += "]},";

    json += "{\"label\":\"Page\",\"name\":\"expand_page\",\"value\":"+String(expand_page)+",\"elements\":[";
    json += "{\"type\":\"refresh\",\"label\":\"Refresh\",\"name\":\"refresh\",\"value\":"+String(refresh)+"}";
    json += "]}";

    json += "]";
    
    webServer.sendHeader("Access-Control-Allow-Origin", "*");
    webServer.send(200, "application/json", json); });

  webServer.on("/firmware", HTTP_POST, []()
               {
    String json;
    json += "[";
    json += "{\"label\":\"Firmware Upgrade\",\"name\":\"firmware_upgrade\",\"value\":\"1\",\"elements\":[";
    json += "{\"type\":\"file\",\"label\":\"File\",\"name\":\"file\",\"value\":\"\",\"accept\":\".bin\"},";
    json += "{\"type\":\"button\",\"label\":\"UPLOAD\",\"name\":\"upload\",\"value\":\"upload\"}";
    json += "]}";
    json += "]";

    webServer.sendHeader("Access-Control-Allow-Origin", "*");
    webServer.send(200, "application/json", json); });
  webServer.on(
      "/upload", HTTP_POST, []()
      {
    String json;
    json += "[";
    json += "{\"label\":\"Firmware Upgrade\",\"name\":\"firmware_upgrade\",\"value\":\"1\",\"elements\":[";
    if (Update.hasError())
      json += "{\"type\":\"alert\",\"value\":\"Upload failed!\"},";
    else
      json += "{\"type\":\"alert\",\"value\":\"Upload success!\"},";

    json += "{\"type\":\"file\",\"label\":\"File\",\"name\":\"file\",\"value\":\"\",\"accept\":\".bin\"},";
    json += "{\"type\":\"button\",\"label\":\"UPDATE\",\"name\":\"update\",\"value\":\"update\"}";
    json += "]}";
    json += "]";
    webServer.sendHeader("Access-Control-Allow-Origin", "*");
    webServer.send(200, "application/json", json);
    ESP.restart(); },
      []()
      {
        HTTPUpload &upload = webServer.upload();
        if (upload.status == UPLOAD_FILE_START)
        {
          Serial.setDebugOutput(true);
          // WiFiUDP::stopAll();
          Serial.printf("Update: %s\n", upload.filename.c_str());
          uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
          if (!Update.begin(maxSketchSpace))
          { // start with max available size
            Update.printError(Serial);
          }
        }
        else if (upload.status == UPLOAD_FILE_WRITE)
        {
          if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
          {
            Update.printError(Serial);
          }
        }
        else if (upload.status == UPLOAD_FILE_END)
        {
          if (Update.end(true))
          { // true to set the size to the current progress
            Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);

            String json;
            json += "[";
            json += "{\"label\":\"Firmware Upgrade\",\"name\":\"firmware_upgrade\",\"value\":\"1\",\"elements\":[";
            if (Update.hasError())
              json += "{\"type\":\"alert\",\"value\":\"Upload failed!\"},";
            else
              json += "{\"type\":\"alert\",\"value\":\"Upload success!\"},";

            json += "{\"type\":\"file\",\"label\":\"File\",\"name\":\"file\",\"value\":\"\",\"accept\":\".bin\"},";
            json += "{\"type\":\"button\",\"label\":\"UPDATE\",\"name\":\"update\",\"value\":\"update\"}";
            json += "]}";
            json += "]";
            webServer.sendHeader("Access-Control-Allow-Origin", "*");
            webServer.send(200, "application/json", json);
          }
          else
          {
            Update.printError(Serial);
          }
          Serial.setDebugOutput(false);
        }
        yield();
      });
  // Send a POST request to <IP>/post with a form field message set to <message>
  /*
  webServer.on("/post", HTTP_POST, []() {
      //nothing and dont remove it
  }, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, data);
    String json;

    serializeJson(doc, json);
    request->send(200, "application/json", json);
  });
  */
  webServer.onNotFound([]()
                       {
    String json;
    //StaticJsonDocument<20> doc;
    //doc["message"] = "Endpoint not found";
    //serializeJson(doc, json);
    json += "[";
    json += "{\"label\":\"404 Error\",\"name\":\"not_found\",\"value\":\"1\",\"elements\":[";
    json += "{\"type\":\"alert\",\"value\":\"Page not found!\"}";
    json += "]}";
    json += "]";
    webServer.sendHeader("Access-Control-Allow-Origin", "*");
    webServer.send(404, "application/json", json); });

  Serial.print("Starting Webserver ... ");
  webServer.begin();
  Serial.println("Done");
}

void webserverLoop()
{
  webServer.handleClient();
}
