#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>

enum WiFiManagerState
{
    WIFI_STATE_IDLE,
    WIFI_STATE_CONNECTING,
    WIFI_STATE_CONNECTED,
    WIFI_STATE_AP_MODE
};

class WiFiManager
{
public:
    /*
     * Explicit constructor required because
     * WebServer is not copyable.
     */
    WiFiManager();

    void begin();
    void update();

    bool connected() const;
    bool provisioningActive() const;

    WiFiManagerState getState() const;

    const char* getSSID() const;

    String getIPAddress() const;
    String getAccessPointSSID() const;

    void startProvisioning();

    void resetCredentialsAndStartProvisioning();

private:
    static constexpr uint32_t
        CONNECT_TIMEOUT_MS = 20000UL;

    static constexpr uint32_t
        RECONNECT_INTERVAL_MS = 30000UL;

    static constexpr uint16_t
        DNS_PORT = 53;

    static constexpr uint16_t
        HTTP_PORT = 80;

    WiFiManagerState _state =
        WIFI_STATE_IDLE;

    /*
     * Do not initialize with:
     *
     * WebServer _server = WebServer(HTTP_PORT);
     *
     * WebServer is not copyable.
     * It is initialized in the constructor list.
     */
    WebServer _server;

    DNSServer _dnsServer;

    String _apSSID;

    uint32_t _connectStartedAt = 0;
    uint32_t _lastReconnectAttempt = 0;

    bool _ntpStarted = false;
    bool _routesConfigured = false;

    void startStation();
    void startNtp();

    void configurePortalRoutes();

    void handlePortalRoot();
    void handlePortalSave();
    void handlePortalNotFound();

    String buildPortalPage();
    String buildSSIDOptions();

    String htmlEscape(
        const String& value) const;

    bool validCredentials() const;
};