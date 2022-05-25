// post

int postCsv() {
  int ret = 0;
  // https
  //std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
  WiFiClientSecure client;
  HTTPClient http;

  if (WiFi.status() == WL_CONNECTED) {
    // https insecure
    //client.setFingerprint(fingerprint);
    client.setInsecure();

    if (http_timeout > 0) http.setTimeout(http_timeout); // ms
    if (http.begin(client, api_url)) {
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");

      String postStr = "api_key="+String(api_key)+"&csv=";

      // csv
      postStr += "" + String(ct_voltage);
      postStr += "," + String(ct_power);

      postStr += "," + String(pv_voltage);
      postStr += "," + String(pv_power);
      postStr += "," + String(battery_voltage);
      postStr += "," + String(battery_charge);
      postStr += "," + String(mppt_voltage);
      postStr += "," + String(mppt_power);
      postStr += "," + String(battery_temperature);
      postStr += "," + String(mppt_temperature);
      postStr += "," + String(dc_voltage);
      postStr += "," + String(dc_power);

      postStr += "," + String(inv_in_voltage);
      postStr += "," + String(inv_in_fault_voltage);
      postStr += "," + String(inv_out_voltage);
      postStr += "," + String(inv_out_power); // watts
      postStr += "," + String(inv_in_frequency);
      postStr += "," + String(inv_cell_voltage);
      postStr += "," + String(inv_temperature);
      postStr += "," + String(inv_flags);

      postStr += "\n\n";
      int httpCode = http.POST(postStr);

      if (httpCode == HTTP_CODE_OK) {
        //Serial.printf("[HTTP] POST... code: %d\n", httpCode);
        String payload = http.getString();
        //Serial.println("received payload:\n<<");
        //Serial.println(payload);
        //Serial.println(">>");

        if (payload != "1") {
          post_error++;
          ret = -1;
        }
        //Serial.print("error: ");
        //Serial.println(error);
      } else {
        //Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
        http_error++;
        http_code=httpCode;
        ret = -1;
      }
      http.end();
    }
  }
  else {
    wifi_error++;
    ret = -1;
  }
  return ret;
}

int postInflux() {
  int ret = 0;
  // http
  WiFiClient client;
  HTTPClient http;

  if (WiFi.status() == WL_CONNECTED) {
    // https insecure
    //client.setFingerprint(fingerprint);
    //client.setInsecure();

    // http://localhost:8086/write?db=home_energy

    if (http_timeout > 0) http.setTimeout(http_timeout); // ms
    if (http.begin(client, api_url)) {
      http.addHeader("Content-Type", "application/binary");

      // home_energy,api_key=NIJCG7UI28O9CAYD ct_voltage=$P1,ct_power=$P2,pv_voltage=$P3,pv_power=$P4,battery_voltage=$P5,battery_charge=$P6,mppt_voltage=$P7,mppt_power=$P8,battery_temperature=$P9,mppt_temperature=$P10,dc_voltage=$P11,dc_power=$P12,input_voltage=$P13,input_fault_voltage=$P14,inv_voltage=$P15,inv_power=$P16,input_frequency=$P17,cell_voltage=$P18,inv_temperature=$P19" -database="home_energy"
      String postStr = "home_energy,api_key=" + String(api_key) + " ";

      // csv
      postStr += "ct_voltage=" + String(ct_voltage);
      postStr += ",ct_power=" + String(ct_power);

      postStr += ",pv_voltage=" + String(pv_voltage);
      postStr += ",pv_voltage=" + String(pv_power);
      postStr += ",battery_voltage=" + String(battery_voltage);
      postStr += ",battery_charge=" + String(battery_charge);
      postStr += ",mppt_voltage=" + String(mppt_voltage);
      postStr += ",mppt_power=" + String(mppt_power);
      postStr += ",battery_temperature=" + String(battery_temperature);
      postStr += ",mppt_temperature=" + String(mppt_temperature);
      postStr += ",dc_voltage=" + String(dc_voltage);
      postStr += ",dc_power=" + String(dc_power);

      postStr += ",inv_in_voltage=" + String(inv_in_voltage);
      postStr += ",inv_in_fault_voltage=" + String(inv_in_fault_voltage);
      postStr += ",inv_out_voltage=" + String(inv_out_voltage);
      postStr += ",inv_out_power=" + String(inv_out_power); // watts
      postStr += ",inv_in_frequency=" + String(inv_in_frequency);
      postStr += ",inv_cell_voltage=" + String(inv_cell_voltage);
      postStr += ",inv_temperature=" + String(inv_temperature);
      postStr += ",inv_flags=" + String(inv_flags);

      postStr += "\n\n";
      int httpCode = http.POST(postStr);

      if (httpCode == HTTP_CODE_OK) {
        //Serial.printf("[HTTP] POST... code: %d\n", httpCode);
        String payload = http.getString();
        //Serial.println("received payload:\n<<");
        //Serial.println(payload);
        //Serial.println(">>");

        if (payload == "") {
          post_error++;
          ret = -1;
        }
        //Serial.print("error: ");
        //Serial.println(error);
      } else {
        //Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
        http_error++;
        http_code=httpCode;
        ret = -1;
      }
      http.end();
    }
  }
  else {
    wifi_error++;
    ret = -1;
  }
  return ret;
}

int postJson() {
  int ret = 0;
  // http
  WiFiClient client;
  HTTPClient http;

  if (WiFi.status() == WL_CONNECTED) {
    // https insecure
    //client.setFingerprint(fingerprint);
    //client.setInsecure();

    // http://localhost:8086/write?db=home_energy

    if (http_timeout > 0) http.setTimeout(http_timeout); // ms
    if (http.begin(client, api_url)) {
      http.addHeader("Content-Type", "application/json");

      // home_energy,api_key=NIJCG7UI28O9CAYD ct_voltage=$P1,ct_power=$P2,pv_voltage=$P3,pv_power=$P4,battery_voltage=$P5,battery_charge=$P6,mppt_voltage=$P7,mppt_power=$P8,battery_temperature=$P9,mppt_temperature=$P10,dc_voltage=$P11,dc_power=$P12,input_voltage=$P13,input_fault_voltage=$P14,inv_voltage=$P15,inv_power=$P16,input_frequency=$P17,cell_voltage=$P18,inv_temperature=$P19" -database="home_energy"
      String postStr = "{api_key:" + String(api_key);

      // json
      postStr += ",ct_voltage:" + String(ct_voltage);
      postStr += ",ct_power:" + String(ct_power);

      postStr += ",pv_voltage:" + String(pv_voltage);
      postStr += ",pv_voltage:" + String(pv_power);
      postStr += ",battery_voltage:" + String(battery_voltage);
      postStr += ",battery_charge:" + String(battery_charge);
      postStr += ",mppt_voltage:" + String(mppt_voltage);
      postStr += ",mppt_power:" + String(mppt_power);
      postStr += ",battery_temperature:" + String(battery_temperature);
      postStr += ",mppt_temperature:" + String(mppt_temperature);
      postStr += ",dc_voltage:" + String(dc_voltage);
      postStr += ",dc_power:" + String(dc_power);

      postStr += ",inv_in_voltage:" + String(inv_in_voltage);
      postStr += ",inv_in_fault_voltage:" + String(inv_in_fault_voltage);
      postStr += ",inv_out_voltage:" + String(inv_out_voltage);
      postStr += ",inv_out_power:" + String(inv_out_power); // watts
      postStr += ",inv_in_frequency:" + String(inv_in_frequency);
      postStr += ",inv_cell_voltage:" + String(inv_cell_voltage);
      postStr += ",inv_temperature:" + String(inv_temperature);
      postStr += ",inv_flags:" + String(inv_flags);

      postStr += "}\n\n";
      int httpCode = http.POST(postStr);

      if (httpCode == HTTP_CODE_OK) {
        //Serial.printf("[HTTP] POST... code: %d\n", httpCode);
        String payload = http.getString();
        //Serial.println("received payload:\n<<");
        //Serial.println(payload);
        //Serial.println(">>");

        if (payload == "") {
          post_error++;
          ret = -1;
        }
        //Serial.print("error: ");
        //Serial.println(error);
      } else {
        //Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
        http_error++;
        http_code=httpCode;
        ret = -1;
      }
      http.end();
    }
  }
  else {
    wifi_error++;
    ret = -1;
  }
  return ret;
}
