// network
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>

#include "network.h"

DNSServer dnsServer;

/* Soft AP network parameters */
IPAddress apIP(192, 168, 4, 1);
// IPAddress subnet(255, 255, 255, 0);
// IPAddress gateway(192, 168, 4, 1);

uint16_t wifi_retry = 0;

boolean connectToSta()
{
  static byte l = 30; // wifi connect timeout limit
  static byte c = 0;
  WiFi.mode(WIFI_AP_STA);
  Serial.print("Connecting to " + String(sta_ssid) + " ");
  WiFi.begin(sta_ssid, sta_key);
  while (c < l)
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println(" Success");

      Serial.print("Local IP address: ");
      Serial.println(WiFi.localIP());
      break;
    }
    delay(500);
    Serial.print(".");
    c++;
  }
  boolean isConnected = (c < l);
  if (!isConnected)
  {
    Serial.println(" Failed");
    Serial.println("AP mode only!");
    WiFi.mode(WIFI_AP);
  }
  return isConnected;
}

void networkSetup()
{

  Serial.println();

  if (sta_enable)
  {
    connectToSta();
  }
  else
  {
    Serial.println("AP mode only!");
    WiFi.mode(WIFI_AP);
  }

  // Serial.print("Setting soft-AP configuration ... ");
  // Serial.println(WiFi.softAPConfig(apIP, gateway, subnet) ? "Ready" : "Failed!");

  Serial.print("Starting Soft-AP (" + String(ap_ssid) + ") ... ");
  Serial.println(WiFi.softAP(ap_ssid, ap_key) ? "Success" : "Failed!");

  Serial.print("Soft-AP IP address: ");
  Serial.println(WiFi.softAPIP());

  Serial.print("Starting MDNS (" + String(APPCODE) + ".local) ... ");
  Serial.println(MDNS.begin(APPCODE) ? "Success." : "Failed!");
  MDNS.addService("http", "tcp", 80);

  WiFi.hostname(APPCODE);
  Serial.print("Starting DNS (" + String(APPCODE) + ") ... ");
  dnsServer.setTTL(300);
  dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);
  dnsServer.start(53, "*", apIP);
  Serial.println("DNS started!");
}

void networkLoop()
{
  static uint32_t msTick = millis();
  // check sta availability
  if (sta_enable && WiFi.status() != WL_CONNECTED && millis() - msTick >= 10000) // 10000ms refresh rate
  {
    msTick = millis();
    // Serial.println("Checking availability: " + String(sta_ssid));
    int n = WiFi.scanNetworks();
    for (int i = 0; i < n; i++)
    {
      if (WiFi.SSID(i) == sta_ssid)
      {
        connectToSta();
        wifi_retry = 0;
        break;
      }
    }
    if (wifi_retry < 65500)
      wifi_retry++;
  }
  // dns server
  dnsServer.processNextRequest();
  MDNS.update();
}
