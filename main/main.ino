#include <WiFiClientSecure.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <TimeLib.h> 
#include <ArduinoJson.h>
#include <Base64.h>
#include "mbedtls/rsa.h"
#include "mbedtls/pem.h"
#include "mbedtls/md.h"
#include "esp_log.h"
#include "string.h"
#include "mbedtls/base64.h"


#define SOIL_MOISTURE_PIN 34  

const char* ssid = "Ninh-T2N";  
const char* password = "ninhkhongbiet"; 

const char* deviceId = "0123456789";

WiFiUDP udp;
NTPClient timeClient(udp, "pool.ntp.org", 3600 * 7);  // Cấu hình thời gian GMT+7 (7 giờ chênh lệch)

const char* certificate = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDejCCAmKgAwIBAgIQf+UwvzMTQ77dghYQST2KGzANBgkqhkiG9w0BAQsFADBX\n" \
"MQswCQYDVQQGEwJCRTEZMBcGA1UEChMQR2xvYmFsU2lnbiBudi1zYTEQMA4GA1UE\n" \
"CxMHUm9vdCBDQTEbMBkGA1UEAxMSR2xvYmFsU2lnbiBSb290IENBMB4XDTIzMTEx\n" \
"NTAzNDMyMVoXDTI4MDEyODAwMDA0MlowRzELMAkGA1UEBhMCVVMxIjAgBgNVBAoT\n" \
"GUdvb2dsZSBUcnVzdCBTZXJ2aWNlcyBMTEMxFDASBgNVBAMTC0dUUyBSb290IFI0\n" \
"MHYwEAYHKoZIzj0CAQYFK4EEACIDYgAE83Rzp2iLYK5DuDXFgTB7S0md+8Fhzube\n" \
"Rr1r1WEYNa5A3XP3iZEwWus87oV8okB2O6nGuEfYKueSkWpz6bFyOZ8pn6KY019e\n" \
"WIZlD6GEZQbR3IvJx3PIjGov5cSr0R2Ko4H/MIH8MA4GA1UdDwEB/wQEAwIBhjAd\n" \
"BgNVHSUEFjAUBggrBgEFBQcDAQYIKwYBBQUHAwIwDwYDVR0TAQH/BAUwAwEB/zAd\n" \
"BgNVHQ4EFgQUgEzW63T/STaj1dj8tT7FavCUHYwwHwYDVR0jBBgwFoAUYHtmGkUN\n" \
"l8qJUC99BM00qP/8/UswNgYIKwYBBQUHAQEEKjAoMCYGCCsGAQUFBzAChhpodHRw\n" \
"Oi8vaS5wa2kuZ29vZy9nc3IxLmNydDAtBgNVHR8EJjAkMCKgIKAehhxodHRwOi8v\n" \
"Yy5wa2kuZ29vZy9yL2dzcjEuY3JsMBMGA1UdIAQMMAowCAYGZ4EMAQIBMA0GCSqG\n" \
"SIb3DQEBCwUAA4IBAQAYQrsPBtYDh5bjP2OBDwmkoWhIDDkic574y04tfzHpn+cJ\n" \
"odI2D4SseesQ6bDrarZ7C30ddLibZatoKiws3UL9xnELz4ct92vID24FfVbiI1hY\n" \
"+SW6FoVHkNeWIP0GCbaM4C6uVdF5dTUsMVs/ZbzNnIdCp5Gxmx5ejvEau8otR/Cs\n" \
"kGN+hr/W5GvT1tMBjgWKZ1i4//emhA1JG1BbPzoLJQvyEotc03lXjTaCzv8mEbep\n" \
"8RqZ7a2CPsgRbuvTPBwcOMBBmuFeU88+FSBX6+7iP0il8b4Z0QFqIwwMHfs/L6K1\n" \
"vepuoxtGzi4CZ68zJpiq1UvSqTbFJjtbD4seiMHl\n" \
"-----END CERTIFICATE-----\n";

// "-----BEGIN CERTIFICATE-----\n" \
// "MIIEVzCCAj+gAwIBAgIRAIOPbGPOsTmMYgZigxXJ/d4wDQYJKoZIhvcNAQELBQAw\n" \
// "TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n" \
// "cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMjQwMzEzMDAwMDAw\n" \
// "WhcNMjcwMzEyMjM1OTU5WjAyMQswCQYDVQQGEwJVUzEWMBQGA1UEChMNTGV0J3Mg\n" \
// "RW5jcnlwdDELMAkGA1UEAxMCRTUwdjAQBgcqhkjOPQIBBgUrgQQAIgNiAAQNCzqK\n" \
// "a2GOtu/cX1jnxkJFVKtj9mZhSAouWXW0gQI3ULc/FnncmOyhKJdyIBwsz9V8UiBO\n" \
// "VHhbhBRrwJCuhezAUUE8Wod/Bk3U/mDR+mwt4X2VEIiiCFQPmRpM5uoKrNijgfgw\n" \
// "gfUwDgYDVR0PAQH/BAQDAgGGMB0GA1UdJQQWMBQGCCsGAQUFBwMCBggrBgEFBQcD\n" \
// "ATASBgNVHRMBAf8ECDAGAQH/AgEAMB0GA1UdDgQWBBSfK1/PPCFPnQS37SssxMZw\n" \
// "i9LXDTAfBgNVHSMEGDAWgBR5tFnme7bl5AFzgAiIyBpY9umbbjAyBggrBgEFBQcB\n" \
// "AQQmMCQwIgYIKwYBBQUHMAKGFmh0dHA6Ly94MS5pLmxlbmNyLm9yZy8wEwYDVR0g\n" \
// "BAwwCjAIBgZngQwBAgEwJwYDVR0fBCAwHjAcoBqgGIYWaHR0cDovL3gxLmMubGVu\n" \
// "Y3Iub3JnLzANBgkqhkiG9w0BAQsFAAOCAgEAH3KdNEVCQdqk0LKyuNImTKdRJY1C\n" \
// "2uw2SJajuhqkyGPY8C+zzsufZ+mgnhnq1A2KVQOSykOEnUbx1cy637rBAihx97r+\n" \
// "bcwbZM6sTDIaEriR/PLk6LKs9Be0uoVxgOKDcpG9svD33J+G9Lcfv1K9luDmSTgG\n" \
// "6XNFIN5vfI5gs/lMPyojEMdIzK9blcl2/1vKxO8WGCcjvsQ1nJ/Pwt8LQZBfOFyV\n" \
// "XP8ubAp/au3dc4EKWG9MO5zcx1qT9+NXRGdVWxGvmBFRAajciMfXME1ZuGmk3/GO\n" \
// "koAM7ZkjZmleyokP1LGzmfJcUd9s7eeu1/9/eg5XlXd/55GtYjAM+C4DG5i7eaNq\n" \
// "cm2F+yxYIPt6cbbtYVNJCGfHWqHEQ4FYStUyFnv8sjyqU8ypgZaNJ9aVcWSICLOI\n" \
// "E1/Qv/7oKsnZCWJ926wU6RqG1OYPGOi1zuABhLw61cuPVDT28nQS/e6z95cJXq0e\n" \
// "K1BcaJ6fJZsmbjRgD5p3mvEf5vdQM7MCEvU0tHbsx2I5mHHJoABHb8KVBgWp/lcX\n" \
// "GWiWaeOyB7RP+OfDtvi2OsapxXiV7vNVs7fMlrRjY1joKaqmmycnBvAq14AEbtyL\n" \
// "sVfOS66B8apkeFX2NY4XPEYV4ZSCe8VHPrdrERk2wILG3T/EGmSIkCYVUMSnjmJd\n" \
// "VQD9F6Na/+zmXCc=\n" \
// "-----END CERTIFICATE-----\n";

const char* privateKey = \
"-----BEGIN PRIVATE KEY-----\n" \
"MIICdgIBADANBgkqhkiG9w0BAQEFAASCAmAwggJcAgEAAoGBAJKGtY13Xo8XeyHw\n" \
"okGrcGYrUTu0INKFBOxC1GetH915ITGdNNppBPgy4iCNo7Wj07BXmpPXKDNVrAdB\n" \
"qDnaQqRchyCK/7EuKmFzcb79V3eRjBfbG6XLX9B/X2PlwhdJJXvaAFfkvAjmy4sE\n" \
"btz1CXe1dyUISk/pCLELvWOGl0rtAgMBAAECgYB735CrGUiCtjot1k6GMBak+WgC\n" \
"jmTVNQmVZ2ZEg/MVfGsYk6v0HabK4dGT7cqCLA8x3M3cwJ/Gzl3SAKrkxeVh2mx7\n" \
"2MziyeRsTAopDNf3/rWNrgKCt9zMYfxtWFovgUsBMyDqTPzLgZm2AXrgOySuKTIP\n" \
"uAFC/hZBFMc8kzxEzQJBAO1GVWz2vJ1dLYrulrM9gt0cSqWx2ccZXhlrlmb+pNMz\n" \
"AZNs8y9r0r5x1amfbHfn8J8NfysCVabFwMTgbnmDsesCQQCeFvsn07zx34Wy1pb4\n" \
"K5MdxKo4Lhne6ncK+aQ69aBrOaeNMyx1XpadIhMUxLPhBu0KOQRuqnyy/LlviT2x\n" \
"V2iHAkEAjnrfrVD0riJXyMfAot1bCd+FS75iodYv1V/vruVWoKROW/LejSlYIZhF\n" \
"z1e2UMJskXYPWQpL2JuDSDXk4P8eBQJAI5F2FoLcGus4cIE5XEKpieQKjDNLHM1R\n" \
"1UVPfvhujFFfcjRWR13tsxLMijqrYfCl+kvTxHZVMgwMSJzvJWxOFwJAPK1GJzd+\n" \
"Ny3VdqwIVpZtmjy9pAzqnEKqa4EPkKHxxSBA4xcT7N6w8R7x74BqNFylLuatBv9I\n" \
"8GYfnFwviqM14A==\n" \
"-----END PRIVATE KEY-----";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  
  // Kết nối WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Khởi động NTPClient
  timeClient.begin();
}

String formatTime(int year, int month, int day, int hour, int minute, int second) {
  String formattedTime = String(year) + "-";
  if (month < 10) formattedTime += "0";
  formattedTime += String(month) + "-";
  if (day < 10) formattedTime += "0";
  formattedTime += String(day) + "T";

  if (hour < 10) formattedTime += "0";
  formattedTime += String(hour) + ":";
  if (minute < 10) formattedTime += "0";
  formattedTime += String(minute) + ":";
  if (second < 10) formattedTime += "0";
  formattedTime += String(second);
  
  return formattedTime;
}

String getTime(){
  // Cập nhật thời gian từ NTP server
  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime();  // Lấy thời gian Unix từ NTPClient

  // Thiết lập thời gian hệ thống từ epochTime
  setTime(epochTime);  

  // Lấy thời gian hiện tại với múi giờ GMT+7
  int secondVal = second();  // Giây
  int minuteVal = minute();  // Phút
  int hourVal = hour();      // Giờ
  int dayVal = day();        // Ngày
  int monthVal = month();    // Tháng
  int yearVal = year();      // Năm

  String currentTime = formatTime(yearVal, monthVal, dayVal, hourVal, minuteVal, secondVal);

  return currentTime;
}

String signData(String data) {
    String signatureBase64 = "";
    const mbedtls_md_info_t *md_info = nullptr;
    mbedtls_pk_context pk;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    uint8_t signature[MBEDTLS_MPI_MAX_SIZE];
    size_t sig_len = 0;
    int ret;

    // Khởi tạo các context
    mbedtls_pk_init(&pk);
    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&ctr_drbg);

    // Thiết lập entropy và CTR_DRBG
    const char *pers = "mbedtls_pk_sign";
    ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, 
                                (const unsigned char *)pers, strlen(pers));
    if (ret != 0) {
        Serial.printf("mbedtls_ctr_drbg_seed failed: -0x%04X\n", -ret);
        goto cleanup;
    }

    // Đọc khóa riêng từ PEM
    ret = mbedtls_pk_parse_key(
        &pk,
        (const unsigned char *)privateKey,
        strlen(privateKey) + 1,
        NULL, 0,
        mbedtls_ctr_drbg_random,
        &ctr_drbg
    );
    if (ret != 0) {
        Serial.printf("Failed to parse private key: -0x%04X\n", -ret);
        goto cleanup;
    }

    // Băm SHA256 từ dữ liệu
    uint8_t hash[32];
    mbedtls_md_context_t sha_ctx;
    mbedtls_md_init(&sha_ctx);
    md_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
    mbedtls_md_setup(&sha_ctx, md_info, 0);
    mbedtls_md_starts(&sha_ctx);
    mbedtls_md_update(&sha_ctx, (const unsigned char *)data.c_str(), data.length());
    mbedtls_md_finish(&sha_ctx, hash);
    mbedtls_md_free(&sha_ctx);

    // Ký dữ liệu đã băm
    ret = mbedtls_pk_sign(&pk, MBEDTLS_MD_SHA256, hash, 0, signature, sizeof(signature), &sig_len, mbedtls_ctr_drbg_random, &ctr_drbg);
    if (ret != 0) {
        Serial.printf("RSA signing failed: -0x%04X\n", -ret);
        goto cleanup;
    }

    unsigned char base64_signature[MBEDTLS_MPI_MAX_SIZE * 2]; // Đảm bảo đủ lớn
    size_t base64_len;
    ret = mbedtls_base64_encode(base64_signature, sizeof(base64_signature), &base64_len, signature, sig_len);
    if (ret == 0) {
        // Gán chuỗi Base64 vào `signatureBase64`
        base64_signature[base64_len] = '\0'; // Đảm bảo chuỗi kết thúc
        signatureBase64 = String((char*)base64_signature);
    } else {
        Serial.printf("Base64 encoding failed: -0x%04X\n", -ret);
        goto cleanup;
    }

cleanup:
    // Giải phóng bộ nhớ
    mbedtls_pk_free(&pk);
    mbedtls_entropy_free(&entropy);
    mbedtls_ctr_drbg_free(&ctr_drbg);

    return ret == 0 ? signatureBase64 : "ERROR";
}

String createPayload(String time, float soilMoisturePercentage, String deviceId, String signature) {
    StaticJsonDocument<256> doc;

    doc["time"] = time;
    doc["soilMoistureValue"] = soilMoisturePercentage;
    doc["deviceId"] = deviceId;
    doc["signature"] = signature;

    String jsonPayload;
    serializeJson(doc, jsonPayload);
    return jsonPayload;
}


void loop() {
  String time = getTime();

  int soilMoistureValue = analogRead(SOIL_MOISTURE_PIN);  // Đọc giá trị độ ẩm từ cảm biến
  float soilMoisturePercentage = 100 - (soilMoistureValue / 4095.0) * 100;

  // Tạo JSON payload
  String dataraw = deviceId + time + String(soilMoisturePercentage, 3);
  Serial.print("Data raw: ");
  Serial.println(dataraw);
  String sig = signData(dataraw);
  String payload = createPayload(time, soilMoisturePercentage, deviceId, sig);
    
  Serial.print("Payload: ");
  Serial.println(payload);
  
  // gửi yêu cầu POST
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setCACert(certificate);
    String serverUrl = "https://final-iot-v2.onrender.com/api/soilMoisture"; 
    if (client.connect("final-iot-v2.onrender.com", 443)) {
      Serial.println("Connected to server.");
      client.print(String("POST ") + "/api/soilMoisture" + " HTTP/1.1\r\n" +
                   "Host: " + "final-iot-v2.onrender.com" + "\r\n" +  
                   "Content-Type: application/json\r\n" +
                   "Content-Length: " + payload.length() + "\r\n" +  
                   "Connection: close\r\n\r\n" +  
                   payload);  
      // Đọc phản hồi từ server và in ra mã trạng thái
      String response = "";
      String statusCode = "";
      unsigned long timeout = millis() + 5000;  // Chờ tối đa 5 giây để nhận phản hồi

      while (client.available() || millis() < timeout) {
        if (client.available()) {
          char c = client.read();
          response += c;
          
          // Trích xuất mã trạng thái HTTP từ dòng đầu tiên
          if (statusCode == "" && response.indexOf("HTTP/1.1") == 0) {
            int statusStart = response.indexOf(" ") + 1;
            int statusEnd = response.indexOf(" ", statusStart);
            statusCode = response.substring(statusStart, statusEnd);  // Mã trạng thái HTTP
          }
        }
      }

      // // In ra mã trạng thái HTTP
      // if (statusCode.length() > 0) {
      //   Serial.print("HTTP Status Code: ");
      //   Serial.println(statusCode);
      // } else {
      //   Serial.println("No status code received.");
      // }

      // // In ra toàn bộ phản hồi
      // if (response.length() > 0) {
      //   Serial.println("Response: ");
      //   Serial.println(response);
      // } else {
      //   Serial.println("No response or connection timeout.");
      // }


    } else {
      Serial.println("Connection failed!");
    }
    // Đóng kết nối
    client.stop();
  }

  delay( 5 * 60 * 1000 );  
}
