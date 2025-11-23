#pragma once
#include <Arduino.h>
#include <FirebaseClient.h>
#include <WiFiClientSecure.h>

class FirebaseRTDB {
public:
    FirebaseRTDB(const char* apiKey,
                 const char* userEmail,
                 const char* userPass,
                 const char* databaseURL)
        : userAuth(apiKey, userEmail, userPass), dbURL(databaseURL) {}

    void begin() {
        ssl_client.setInsecure();
        ssl_client.setConnectionTimeout(1000);
        ssl_client.setHandshakeTimeout(5);

        initializeApp(aClient, app, getAuth(userAuth), std::bind(&FirebaseRTDB::defaultCallback, this, std::placeholders::_1),"AuthTask");

        app.getApp<RealtimeDatabase>(db);
        db.url(dbURL);
    }

    void loop() {
        app.loop();
    }

    bool ready() {
        return app.ready();
    }

    // -------------------------------
    // TEMPLATE: SET DATA (ANY TYPE)
    // -------------------------------
    template <typename T>
    void set(const String& path, const T& value, const String& taskName = "") {
        if (taskName.length() > 0)
            db.set<T>(aClient, path, value, callback, taskName);
        else
            db.set<T>(aClient, path, value, callback);
    }

    // -------------------------------
    // GET DATA (STRING RESULT)
    // -------------------------------
    void get(const String& path, const String& taskName = "") {
        if (taskName.length() > 0)
            db.get(aClient, path, callback, false, taskName);
        else
            db.get(aClient, path, callback);
    }

    // Callback dari user
    void setCallback(std::function<void(String task, String payload)> cb) {
        userCallback = cb;
    }

private:
    UserAuth userAuth;
    FirebaseApp app;
    WiFiClientSecure ssl_client;
    using AsyncClient = AsyncClientClass;
    AsyncClient aClient = AsyncClient(ssl_client);
    RealtimeDatabase db;
    const char* dbURL;

    std::function<void(String, String)> userCallback;

    // Callback internal default
    void defaultCallback(AsyncResult &res) {
        if (!res.isResult()) return;

        if (res.isError()) {
            Serial.printf("[Firebase Error] %s\n", res.error().message().c_str());
            return;
        }

        if (res.available()) {
            String task = res.uid();
            String payload = res.c_str();
            if (userCallback) userCallback(task, payload);
        }
    }

    // Menggunakan defaultCallback
    auto callback = std::bind(&FirebaseRTDB::defaultCallback, this, std::placeholders::_1);
};
