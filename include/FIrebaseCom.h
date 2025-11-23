#ifndef FIREBASECOM_H
#define FIREBASECOM_H

#define ENABLE_USER_AUTH
#define ENABLE_DATABASE

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <FirebaseClient.h>

// --- Ganti sesuai kredensialmu ---
#define WIFI_SSID "ADAN"
#define WIFI_PASSWORD "titanasri"


// Pin & path contoh
#define LED_PIN     2
#define SENSOR_PATH "/sensorValue"
#define LED_PATH    "/ledStatus"

// Forward-declare library types used by FirebaseClient
using AsyncClient = AsyncClientClass;

// ================= FirebaseManager Class =================
// Mengemas semua fungsi Firebase: init, set/get berbagai tipe, penanganan error
class FirebaseManager {
public:
  using FBCallback = void (*)(AsyncResult &aResult); // callback tipe yang sama dengan library

  FirebaseManager(WiFiClientSecure &sslClient,
                  const String &apiKey,
                  const String &databaseUrl,
                  const String &userEmail,
                  const String &userPass)
    : _sslClient(sslClient),
      _userAuth(apiKey, userEmail, userPass),
      _app(),
      _aClient(_sslClient),
      _db(),
      _apiKey(apiKey),
      _dbUrl(databaseUrl),
      _lastError("")
  {
    _userCallback = nullptr;
  }

  // Inisialisasi koneksi Firebase dan callback global
  void begin(FBCallback userCallback = nullptr) {
    // simpan callback user
    _userCallback = userCallback;

    // konfigurasi SSL/TLS
    _sslClient.setInsecure();
    _sslClient.setConnectionTimeout(1000);
    _sslClient.setHandshakeTimeout(5);

    // initialize firebase app dengan wrapper callback internal
    initializeApp(_aClient, _app, getAuth(_userAuth), FirebaseManager::internalCallback, "FirebaseTask");
    _app.getApp<RealtimeDatabase>(_db);
    _db.url(_dbUrl);

    _lastInit = millis();
  }

  // Harus dipanggil di loop() supaya auth/background works
  void loop() {
    _app.loop();
    // ping sederhana untuk menjaga koneksi: kalau app tidak ready dalam waktu lama -> catat error
    if (!_app.ready() && millis() - _lastInit > 15000) {
      _lastError = "Firebase not ready (auth or network issue)";
    }
  }

  // --- Setter generik untuk berbagai tipe ---
  // Semua fungsi set akan memanggil callback internal sehingga userCallback akan menerima AsyncResult
  void setString(const String &path, const String &value, const String &taskId = "RTDB_Set") {
    _db.set<String>(_aClient, path.c_str(), value, FirebaseManager::internalCallback, taskId.c_str());
  }

  void setInt(const String &path, int value, const String &taskId = "RTDB_Set") {
    _db.set<int>(_aClient, path.c_str(), value, FirebaseManager::internalCallback, taskId.c_str());
  }

  void setFloat(const String &path, float value, const String &taskId = "RTDB_Set") {
    _db.set<float>(_aClient, path.c_str(), value, FirebaseManager::internalCallback, taskId.c_str());
  }

  void setBool(const String &path, bool value, const String &taskId = "RTDB_Set") {
    _db.set<bool>(_aClient, path.c_str(), value, FirebaseManager::internalCallback, taskId.c_str());
  }

  // --- Getter ---
  // get dengan callback akan meneruskan AsyncResult ke userCallback bila ada
  void get(const String &path, bool shallow = false, const String &taskId = "RTDB_Get") {
    _db.get(_aClient, path.c_str(), FirebaseManager::internalCallback, shallow, taskId.c_str());
  }

  // helper convenience: get dan parse ke tipe sederhana synchronously via callback user
  // (library tetap async, jadi parsing di callback user lebih aman)

  // --- Utility ---
  String lastError() const { return _lastError; }

  bool ready() const { return _app.ready(); }

  // Memungkinkan user meregistrasi callback lagi di runtime
  void setUserCallback(FBCallback cb) { _userCallback = cb; }

private:
  WiFiClientSecure &_sslClient;
  UserAuth _userAuth;
  FirebaseApp _app;
  AsyncClient _aClient;
  RealtimeDatabase _db;

  String _apiKey;
  String _dbUrl;

  unsigned long _lastInit = 0;
  String _lastError;

  // pointer ke callback milik user (boleh NULL)
  static FBCallback _userCallback;

  // Internal callback yang dipakai oleh library
  static void internalCallback(AsyncResult &aResult) {
    // Default error handling
    if (!aResult.isResult()) return;

    if (aResult.isError()) {
      String err = "[Firebase Error] ";
      err += aResult.error().message().c_str();
      Serial.println(err);
      // juga terusan ke user callback agar bisa tangani khusus
      if (_userCallback) _userCallback(aResult);
      return;
    }

    // Jika ada user callback terdaftar, teruskan saja
    if (_userCallback) {
      _userCallback(aResult);
      return;
    }

    // Jika tidak ada callback user, tampilkan ringkasan minimal
    if (aResult.available()) {
      Serial.printf("FB Result uid=%s payload=%s\n", aResult.uid(), aResult.c_str());
    }
  }
};

// Definisikan static member
FirebaseManager::FBCallback FirebaseManager::_userCallback = nullptr;

// ================= Example sketch menggunakan FirebaseManager =================

// Objek global
WiFiClientSecure ssl_client;
FirebaseManager *fb = nullptr;

unsigned long lastSendTime = 0;
const unsigned long sendInterval = 10000;
int sensorValue = 0;

// User callback menerima AsyncResult dan bisa mem-parsing task id (uid) untuk routing
void userFbCallback(AsyncResult &aResult) {
  if (!aResult.isResult()) return;
  if (aResult.isError()) {
    Serial.printf("User callback - Firebase error: %s\n", aResult.error().message().c_str());
    return;
  }
  if (!aResult.available()) return;

  String uid = aResult.uid();
  String payload = aResult.c_str();
  Serial.printf("User callback - uid=%s payload=%s\n", uid.c_str(), payload.c_str());

  // routing berdasarkan uid (taskId)
  if (uid == "RTDB_GetLED") {
    int ledState = payload.toInt();
    digitalWrite(LED_PIN, ledState);
    Serial.printf("[Action] LED diatur %d\n", ledState);
  }
}

void connectWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println(" connected");
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  connectWiFi();

  // buat instance FirebaseManager
  static FirebaseManager firebaseMgr(ssl_client, Web_API_KEY, DATABASE_URL, USER_EMAIL, USER_PASS);
  fb = &firebaseMgr;
  fb->begin(userFbCallback);
}

void loop() {
  fb->loop();

  if (fb->ready()) {
    unsigned long now = millis();
    if (now - lastSendTime >= sendInterval) {
      lastSendTime = now;
      // kirim sensor
      String s = String(sensorValue++);
      fb->setString(SENSOR_PATH, s, "RTDB_SetSensor");
      Serial.println("Kirim sensor: " + s);
      // minta status LED
      fb->get(LED_PATH, false, "RTDB_GetLED");
    }
  } else {
    // not ready - boleh handling reconnect atau log
    if (millis() % 5000 < 50) Serial.println("Firebase not ready yet...");
  }

  delay(10);
}

// ================= End of file =================
#endif