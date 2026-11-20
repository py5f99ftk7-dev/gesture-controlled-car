#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

// L298N pins
#define ENA 5
#define IN1 22
#define IN2 21

#define ENB 23
#define IN3 19
#define IN4 18

// LEDC PWM channels (Arduino-ESP32 2.0.x 旧版 API 用通道号)
#define PWM_CH_A 0
#define PWM_CH_B 1

// Motor PWM settings
const int pwmFreq = 1000;
const int pwmResolution = 8;

// ESP-NOW message
typedef struct struct_message {
    char command[10];
} struct_message;

struct_message incomingData;

// ---------------- MOTOR CONTROL ----------------

void setMotorA(int speed) {
    ledcWrite(PWM_CH_A, speed);
}

void setMotorB(int speed) {
    ledcWrite(PWM_CH_B, speed);
}

void stopMotors() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);

    setMotorA(0);
    setMotorB(0);
}

void moveCar(const char* cmd) {

    int speed = 220; // default speed


    if (strcmp(cmd, "LEFT") == 0) {
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);

        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);

        setMotorA(230);
        setMotorB(230);
    }

    else if (strcmp(cmd, "RIGHT") == 0) {
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);

        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);

        setMotorA(230);
        setMotorB(230);
    }

    else if (strcmp(cmd, "FORWARD") == 0) {
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);

        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);

        setMotorA(speed);
        setMotorB(speed);
    }

    else if (strcmp(cmd, "BACKWARD") == 0) {
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);

        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);

        setMotorA(speed);
        setMotorB(speed);
    }

    else {
        stopMotors();
    }
}

// ---------------- ESP-NOW CALLBACK ----------------

void OnDataRecv(const uint8_t *mac, const uint8_t *data, int len) {
    memcpy(&incomingData, data, sizeof(incomingData));

    Serial.print("Commands Received: ");
    Serial.println(incomingData.command);

    moveCar(incomingData.command);
}

// ---------------- SETUP ----------------

void setup() {
    Serial.begin(115200);

    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);

    // LEDC 旧版 API（Arduino-ESP32 2.0.x）：先 setup 通道，再绑定引脚
    ledcSetup(PWM_CH_A, pwmFreq, pwmResolution);
    ledcAttachPin(ENA, PWM_CH_A);
    ledcSetup(PWM_CH_B, pwmFreq, pwmResolution);
    ledcAttachPin(ENB, PWM_CH_B);

    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW Init Failed");
        return;
    }

    esp_now_register_recv_cb(OnDataRecv);

    Serial.println("Receiver ready (L298N + ESP32 v3)!");
}

// ---------------- LOOP ----------------

void loop() {
    // ESP-NOW handles everything
}