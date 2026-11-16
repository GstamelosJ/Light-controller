#include "wifi_manager.h"

#include <Arduino.h>
#include <cstring>
#include <functional>
#include "../common/globals.h"

constexpr uint32_t WiFiManager::CONNECT_TIMEOUT_MS;

constexpr uint32_t WiFiManager::RECONNECT_INTERVAL_MS;

constexpr uint16_t WiFiManager::DNS_PORT;

constexpr uint16_t WiFiManager::HTTP_PORT;

namespace
{
    constexpr const char*
        PROVISIONING_AP_PASSWORD =
            "LightSetup123";
}

WiFiManager::WiFiManager()
    : _server(HTTP_PORT)
{
}

void WiFiManager::begin()
{
    WiFi.persistent(false);
    WiFi.setAutoReconnect(true);

    if(validCredentials())
    {
        startStation();
    }
    else
    {
        startProvisioning();
    }
}

void WiFiManager::update()
{
    if(_state == WIFI_STATE_AP_MODE)
    {
        _dnsServer.processNextRequest();
        _server.handleClient();

        return;
    }

    wl_status_t status =
        WiFi.status();

    if(status == WL_CONNECTED)
    {
        if(_state != WIFI_STATE_CONNECTED)
        {
            _state =
                WIFI_STATE_CONNECTED;

            Serial.print(
                "[WIFI] Connected. IP: ");

            Serial.println(
                WiFi.localIP());

            startNtp();
        }

        return;
    }

    if(_state == WIFI_STATE_CONNECTING)
    {
        if(millis() - _connectStartedAt >=
           CONNECT_TIMEOUT_MS)
        {
            Serial.println(
                "[WIFI] Connection timeout.");

            startProvisioning();
        }

        return;
    }

    if(_state == WIFI_STATE_CONNECTED)
    {
        Serial.println(
            "[WIFI] Connection lost.");

        _state =
            WIFI_STATE_IDLE;

        _lastReconnectAttempt =
            millis();

        _ntpStarted =
            false;
    }

    if(_state == WIFI_STATE_IDLE &&
       validCredentials() &&
       millis() - _lastReconnectAttempt >=
           RECONNECT_INTERVAL_MS)
    {
        startStation();
    }
}

void WiFiManager::startStation()
{
    Serial.print(
        "[WIFI] Connecting to: ");

    Serial.println(
        Config.config
            .network
            .wifiSSID);

    _server.stop();
    _dnsServer.stop();

    WiFi.softAPdisconnect(true);

    delay(50);

    WiFi.mode(WIFI_STA);

    WiFi.begin(
        Config.config
            .network
            .wifiSSID,
        Config.config
            .network
            .wifiPASS);

    _connectStartedAt =
        millis();

    _lastReconnectAttempt =
        millis();

    _ntpStarted =
        false;

    _state =
        WIFI_STATE_CONNECTING;
}

void WiFiManager::startNtp()
{
    if(_ntpStarted)
    {
        return;
    }

    /*
     * Greece timezone:
     *
     * Standard time: UTC+2
     * Daylight saving time: UTC+3
     */
    configTzTime(
        "EET-2EEST,M3.5.0/3,M10.5.0/4",
        "pool.ntp.org",
        "time.nist.gov");

    _ntpStarted =
        true;

    Serial.println(
        "[NTP] Synchronization started.");
}

void WiFiManager::startProvisioning()
{
    Serial.println(
        "[WIFI] Starting provisioning AP.");

    _server.stop();
    _dnsServer.stop();

    WiFi.disconnect(true);

    delay(100);

    /*
     * AP + STA is required so that WiFi scanning
     * remains available while the portal is active.
     */
    WiFi.mode(WIFI_AP_STA);

    uint64_t chipId =
        ESP.getEfuseMac();

    uint32_t suffixValue =
        static_cast<uint32_t>(
            chipId & 0xFFFFFFULL);

    char suffix[7];

    snprintf(
        suffix,
        sizeof(suffix),
        "%06lX",
        static_cast<unsigned long>(
            suffixValue));

    _apSSID =
        String("LightingController-") +
        suffix;

    bool accessPointStarted =
        WiFi.softAP(
    _apSSID.c_str(),
    PROVISIONING_AP_PASSWORD);

    if(!accessPointStarted)
    {
        Serial.println(
            "[WIFI] Failed to start AP.");

        _state =
            WIFI_STATE_IDLE;

        _lastReconnectAttempt =
            millis();

        return;
    }

    configurePortalRoutes();

    _dnsServer.start(
        DNS_PORT,
        "*",
        WiFi.softAPIP());

    _server.begin();

    _state =
        WIFI_STATE_AP_MODE;

    Serial.print(
        "[WIFI] AP SSID: ");

    Serial.println(
        _apSSID);

    Serial.print(
        "[WIFI] AP password: ");

	Serial.println(
		PROVISIONING_AP_PASSWORD);

    Serial.print(
        "[WIFI] Portal IP: ");

    Serial.println(
        WiFi.softAPIP());
}

void WiFiManager::resetCredentialsAndStartProvisioning()
{
    Serial.println(
        "[WIFI] Clearing stored credentials.");

    Config.config
        .network
        .wifiSSID[0] = '\0';

    Config.config
        .network
        .wifiPASS[0] = '\0';

    Config.save();

    startProvisioning();
}

bool WiFiManager::connected() const
{
    return
        _state == WIFI_STATE_CONNECTED &&
        WiFi.status() == WL_CONNECTED;
}

bool WiFiManager::provisioningActive() const
{
    return
        _state == WIFI_STATE_AP_MODE;
}

WiFiManagerState WiFiManager::getState() const
{
    return _state;
}

const char* WiFiManager::getSSID() const
{
    return
        Config.config
            .network
            .wifiSSID;
}

String WiFiManager::getIPAddress() const
{
    if(connected())
    {
        return
            WiFi.localIP().toString();
    }

    if(provisioningActive())
    {
        return
            WiFi.softAPIP().toString();
    }

    return "0.0.0.0";
}

String WiFiManager::getAccessPointSSID() const
{
    return _apSSID;
}

bool WiFiManager::validCredentials() const
{
    size_t ssidLength =
        strnlen(
            Config.config
                .network
                .wifiSSID,
            sizeof(
                Config.config
                    .network
                    .wifiSSID));

    return
        ssidLength > 0 &&
        ssidLength <
            sizeof(
                Config.config
                    .network
                    .wifiSSID);
}

String WiFiManager::htmlEscape(
    const String& value) const
{
    String escaped;

    escaped.reserve(
        value.length() + 16);

    for(size_t i = 0;
        i < value.length();
        i++)
    {
        char character =
            value.charAt(i);

        switch(character)
        {
            case '&':
                escaped += "&amp;";
                break;

            case '<':
                escaped += "&lt;";
                break;

            case '>':
                escaped += "&gt;";
                break;

            case '"':
                escaped += "&quot;";
                break;

            case '\'':
                escaped += "&#39;";
                break;

            default:
                escaped += character;
                break;
        }
    }

    return escaped;
}

String WiFiManager::buildSSIDOptions()
{
    String options;

    int networkCount =
        WiFi.scanNetworks(
            false,
            true);

    if(networkCount <= 0)
    {
        options +=
            "<option value=\"\">"
            "No networks found"
            "</option>";

        WiFi.scanDelete();

        return options;
    }

    for(int index = 0;
        index < networkCount;
        index++)
    {
        String ssid =
            WiFi.SSID(index);

        if(ssid.length() == 0)
        {
            continue;
        }

        options +=
            "<option value=\"";

        options +=
            htmlEscape(ssid);

        options +=
            "\">";

        options +=
            htmlEscape(ssid);

        options +=
            " (";

        options +=
            String(
                WiFi.RSSI(index));

        options +=
            " dBm)</option>";
    }

    WiFi.scanDelete();

    return options;
}

String WiFiManager::buildPortalPage()
{
    String page;

    page.reserve(4000);

    page +=
        "<!doctype html>"
        "<html lang=\"en\">"
        "<head>"
        "<meta charset=\"utf-8\">"
        "<meta name=\"viewport\" "
        "content=\"width=device-width,"
        "initial-scale=1\">"
        "<title>Lighting Controller</title>"
        "<style>"
        "body{"
        "font-family:Arial,sans-serif;"
        "max-width:520px;"
        "margin:32px auto;"
        "padding:0 16px;"
        "background:#f4f6f8;"
        "color:#202124;"
        "}"
        ".card{"
        "background:white;"
        "padding:24px;"
        "border-radius:12px;"
        "box-shadow:0 2px 12px rgba(0,0,0,.15);"
        "}"
        "h1{font-size:24px;margin-top:0;}"
        "label{"
        "display:block;"
        "margin-top:16px;"
        "font-weight:bold;"
        "}"
        "select,input{"
        "width:100%;"
        "box-sizing:border-box;"
        "padding:12px;"
        "margin-top:6px;"
        "border:1px solid #bbb;"
        "border-radius:6px;"
        "}"
        "button{"
        "width:100%;"
        "padding:13px;"
        "margin-top:22px;"
        "border:0;"
        "border-radius:6px;"
        "background:#0067b8;"
        "color:white;"
        "font-size:16px;"
        "}"
        ".info{"
        "font-size:13px;"
        "color:#555;"
        "margin-top:16px;"
        "}"
        "</style>"
        "</head>"
        "<body>"
        "<div class=\"card\">"
        "<h1>Lighting Controller</h1>"
        "<p>Select the WiFi network and enter "
        "the network password.</p>"
        "\"/save\""
        "<label for=\"ssid\">WiFi network</label>"
        "<select id=\"ssid\" name=\"ssid\" required>";

    page +=
        buildSSIDOptions();

    page +=
        "</select>"
        "<label for=\"password\">Password</label>"
        "<input "
        "id=\"password\" "
        "name=\"password\" "
        "type=\"password\" "
        "maxlength=\"63\" "
        "autocomplete=\"current-password\">"
        "<button type=\"submit\">"
        "Save and connect"
        "</button>"
        "</form>"
        "<div class=\"info\">"
        "Provisioning AP: ";

    page +=
        htmlEscape(_apSSID);

    page +=
        "<br>Portal address: ";

    page +=
        WiFi.softAPIP().toString();

    page +=
        "</div>"
        "</div>"
        "</body>"
        "</html>";

    return page;
}

void WiFiManager::configurePortalRoutes()
{
    if(_routesConfigured)
    {
        return;
    }

    _server.on(
        "/",
        HTTP_GET,
        std::bind(
            &WiFiManager::handlePortalRoot,
            this));

    _server.on(
        "/save",
        HTTP_POST,
        std::bind(
            &WiFiManager::handlePortalSave,
            this));

    // Android captive portal detection.
    _server.on(
        "/generate_204",
        HTTP_GET,
        std::bind(
            &WiFiManager::handlePortalRoot,
            this));

    _server.on(
        "/gen_204",
        HTTP_GET,
        std::bind(
            &WiFiManager::handlePortalRoot,
            this));

    // Apple captive portal detection.
    _server.on(
        "/hotspot-detect.html",
        HTTP_GET,
        std::bind(
            &WiFiManager::handlePortalRoot,
            this));

    _server.on(
        "/library/test/success.html",
        HTTP_GET,
        std::bind(
            &WiFiManager::handlePortalRoot,
            this));

    // Microsoft captive portal detection.
    _server.on(
        "/fwlink",
        HTTP_GET,
        std::bind(
            &WiFiManager::handlePortalRoot,
            this));

    _server.on(
        "/connecttest.txt",
        HTTP_GET,
        std::bind(
            &WiFiManager::handlePortalRoot,
            this));

    _server.on(
        "/ncsi.txt",
        HTTP_GET,
        std::bind(
            &WiFiManager::handlePortalRoot,
            this));

    _server.onNotFound(
        std::bind(
            &WiFiManager::handlePortalNotFound,
            this));

    _routesConfigured = true;
}

void WiFiManager::handlePortalRoot()
{
    _server.send(
        200,
        "text/html",
        buildPortalPage());
}

void WiFiManager::handlePortalSave()
{
    if(!_server.hasArg("ssid"))
    {
        _server.send(
            400,
            "text/plain",
            "SSID is required.");

        return;
    }

    String ssid =
        _server.arg("ssid");

    String password =
        _server.arg("password");

    ssid.trim();

    if(ssid.length() == 0 ||
       ssid.length() >=
           sizeof(
               Config.config
                   .network
                   .wifiSSID))
    {
        _server.send(
            400,
            "text/plain",
            "Invalid SSID.");

        return;
    }

    if(password.length() >=
       sizeof(
           Config.config
               .network
               .wifiPASS))
    {
        _server.send(
            400,
            "text/plain",
            "Password is too long.");

        return;
    }

    memset(
        Config.config
            .network
            .wifiSSID,
        0,
        sizeof(
            Config.config
                .network
                .wifiSSID));

    memset(
        Config.config
            .network
            .wifiPASS,
        0,
        sizeof(
            Config.config
                .network
                .wifiPASS));

    ssid.toCharArray(
        Config.config
            .network
            .wifiSSID,
        sizeof(
            Config.config
                .network
                .wifiSSID));

    password.toCharArray(
        Config.config
            .network
            .wifiPASS,
        sizeof(
            Config.config
                .network
                .wifiPASS));

    Config.save();

    _server.send(
        200,
        "text/html",
        "<!doctype html>"
        "<html>"
        "<head>"
        "<meta charset=\"utf-8\">"
        "<meta name=\"viewport\" "
        "content=\"width=device-width,"
        "initial-scale=1\">"
        "<title>WiFi saved</title>"
        "</head>"
        "<body>"
        "<h2>WiFi settings saved</h2>"
        "<p>The controller will restart "
        "and connect to the selected network.</p>"
        "</body>"
        "</html>");

    delay(750);

    ESP.restart();
}

void WiFiManager::handlePortalNotFound()
{
    String redirectAddress =
        String("http://") +
        WiFi.softAPIP().toString();

    _server.sendHeader(
        "Location",
        redirectAddress,
        true);

    _server.send(
        302,
        "text/plain",
        "");
}
