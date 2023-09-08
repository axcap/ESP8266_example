#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <LittleFS.h>
#include <GitHubOTA.h>
#include <GitHubFsOTA.h>
#include <semver_extensions.h>
#include <iostream>

// This string should correspond to github tag used for Releasing (via. Github Actions)
#define VERSION "0.0.1"

auto current_version = semver_from_string(VERSION);
// Replace your_username/your_repo with your values (ex. axcap/Esp-GitHub-OTA)
// This is a link to repo where your firmware updates will be pulled from
// #define RELEASE_URL "https://api.github.com/repos/your_username/your_repo/releases/latest"

// Use this version of the URL together with 
// Line 22: GitHubOTA GitHubOTA(VERSION, RELEASE_URL, "firmware.bin", "filesystem.bin", "/.fs_update_pending", false);
// under debugging to spare yourself from getting timeout from GitHub API
#define RELEASE_URL "https://github.com/your_username/your_repo/releases/latest"

#define DELAY_MS 1000

#define SSID ""
#define PASSWORD ""

// Remove the 'true' argument to use GitHub api to get the latest release
GitHubOTA OsOta(VERSION, RELEASE_URL, "firmware.bin", true);
GitHubFsOTA FsOta(VERSION, RELEASE_URL, "filesystem.bin", true);

void setup_wifi();
void listRoot();

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  while(!Serial);

  Serial.printf("Current firmware version: %s\n", VERSION); 
  Serial.printf("Current filesystem version: %s\n", VERSION); 

  LittleFS.begin();
  listRoot();

  setup_wifi();

  // Automatically update when available update detected
  // FsOta.handle();
  // OsOta.handle();

  // Manually fetch newest firmware version and update if needed
  auto newest_version = OsOta.get_newest_version();

  Serial.printf(semver_to_string(&newest_version).c_str());
  if(newest_version > current_version){
    OsOta.update_firmware();
  }
}

void loop()
{
  // Your code goes here
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(DELAY_MS);                  // wait for a second
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  delay(DELAY_MS);                  // wait for a second
}

void listRoot(){
  Serial.printf("Listing root directory\r\n");

  File root = LittleFS.open("/", "r");
  File file = root.openNextFile();

  while(file){
    Serial.printf("  FILE: %s\r\n", file.name());
    file = root.openNextFile();
  }
}

void setup_wifi(){
  Serial.println("Initialize WiFi");
  WiFi.begin(SSID, PASSWORD);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}