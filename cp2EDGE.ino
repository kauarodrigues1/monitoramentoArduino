#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include <EEPROM.h>
#include "DHT.h"
#include <Wire.h>

#define LOG_OPTION 1    // Ativar a leitura do log
#define SERIAL_OPTION 1 // Ativar comunicação serial
#define UTC_OFFSET -3   // Ajuste de fuso horário para UTC-3

// Configurações do DHT11
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

LiquidCrystal_I2C lcd(0x27, 16, 2);
RTC_DS1307 RTC;

//Define a logo "VS"
byte vsLogo[8] = {
    0b00000,
    0b01110,
    0b10001,
    0b10001,
    0b01010,
    0b00100,
    0b01010,
    0b00000
};

// Define a taça com base
byte cup[8] = {
    0b00110,
    0b01111,
    0b10001,
    0b10001,
    0b01110,
    0b00100,
    0b11111, // Base do copo
    0b00000
};
 
int cupPosition = 0; // Posição inicial da taça
bool movingRight = true; // Direção do movimento

// Configurações da EEPROM
const int maxRecords = 100;      // Máximo de registros
const int recordSize = 8;        // Tamanho de cada registro em bytes
int startAddress = 0;            // Endereço inicial
int endAddress = maxRecords * recordSize; // Endereço final
int currentAddress = 0;          // Endereço atual para escrita

// Pinos dos LEDs e do Buzzer
int ledGreen = 13; // LED verde
int ledYellow = 9; // LED amarelo
int ledRed = 8;    // LED vermelho
int buzzer = 7;    // Buzzer
float intensidadeLuz; // Variável para intensidade da luz
int ledGreen2 = 4; // LED verde2
int ledYellow2 = 11; // LED amarelo2
int ledRed2 = 10; // LED vermelho2
int ledRed3 = 3;
int ledYellow3 = 5;
int ledGreen3 = 6;

//controle slideshow
unsigned long currentMillis;
unsigned long lastSlideshowUpdate= 0;
const unsigned long slideshowInterval=5000;
int displayMode = 0; // 0: Data/Hora, 1: Umidade, 2: Temperatura, 3: Luminosidade
String alertMessage;
unsigned long millisAnterior=0;

int lastLoggedMinute = -1;
unsigned long previousMillis = 0; // Tempo anterior
const long updateInterval = 1000; // Intervalo de atualização

// Controle do buzzer
unsigned long buzzerMillis = 100; // Tempo do buzzer
bool buzzerState = false; // Estado do buzzer
const long buzzerOnDuration = 100; // Tempo ligado
const long buzzerOffDuration = 150; // Tempo desligado

// constantes de temperautra, lu
const float TEMP_MIN= 16;
const float TEMP_MAX= 30;
const float HUM_MIN=60;
const float HUM_MAX=80;

void setup() {
  //iniciar lcd e backlight
    lcd.init();
    lcd.backlight();
    // Carrega os caracteres personalizados
    lcd.createChar(0, vsLogo);
    lcd.createChar(1, cup);
    lcd.setCursor(0, 0); // Primeira linha, primeira coluna
    lcd.write(byte(0));  // Mostra a logo "VS"

    //leds
    pinMode(ledGreen, OUTPUT);
    pinMode(ledYellow, OUTPUT);
    pinMode(ledRed, OUTPUT);
    pinMode(ledGreen2, OUTPUT);
    pinMode(ledYellow2, OUTPUT);
    pinMode(ledRed2, OUTPUT);
    pinMode(ledRed3, OUTPUT);
    pinMode(ledYellow3, OUTPUT);
    pinMode(ledGreen3, OUTPUT);


    pinMode(buzzer, OUTPUT);
    dht.begin();
    Serial.begin(9600);
    RTC.begin();
   

    // Ajustar o RTC apenas uma vez, se necessário
    //RTC.adjust(DateTime(2024, 10, 26, 22,22, 30)); // Comentar após a primeira execução

    EEPROM.begin();
    // Limpar EEPROM
    for (int i = startAddress; i < endAddress; i++) {
        EEPROM.write(i, 0xFF);
    }
}

void get_log() {
    Serial.println("Log dos dados:");
    for (int i = startAddress; i < currentAddress; i += recordSize) {
        unsigned long timestamp;
        int tempInt, humiInt;

        EEPROM.get(i, timestamp);
        EEPROM.get(i + 4, tempInt);
        EEPROM.get(i + 6, humiInt);

        Serial.print("Timestamp: ");
        Serial.print(timestamp);
        Serial.print(" | Temp: ");
        Serial.print(tempInt / 100.0);
        Serial.print(" C | Humi: ");
        Serial.println(humiInt / 100.0);
    }
}

void getNextAddress() {
    currentAddress += recordSize;
    if (currentAddress >= endAddress) {
        currentAddress = startAddress; // Reinicia quando chega ao final
    }
}

void loop() {
    int LDR = analogRead(A0);
    intensidadeLuz = map(LDR, 0, 705, 0, 100);
    float humidity = dht.readHumidity();
    unsigned long currentMillis = millis();
    float temperature = dht.readTemperature();

    if (currentMillis - lastSlideshowUpdate >= slideshowInterval){
        lastSlideshowUpdate = currentMillis;
        displayMode = (displayMode +1) % 5;
    }
   
    
    // Atualiza a hora a cada segundo
    if (currentMillis - previousMillis >= updateInterval) { 
        previousMillis = currentMillis; // Atualiza o tempo

        DateTime now = RTC.now(); // Lê a hora atual do RTC
        DateTime adjustedTime = DateTime(now.unixtime() + UTC_OFFSET * 3600); // Ajuste de fuso horário

        // Atualiza o display com base no modo de exibição
        lcd.clear();
        if (displayMode == 0) {
            // Mostrar Data e Hora
            lcd.setCursor(0, 0);
            lcd.print("DATA: ");
            lcd.print(adjustedTime.day() < 10 ? "0" : ""); // Zero à esquerda
            lcd.print(adjustedTime.day());
            lcd.print("/");
            lcd.print(adjustedTime.month() < 10 ? "0" : ""); // Zero à esquerda
            lcd.print(adjustedTime.month());
            lcd.print("/");
            lcd.print(adjustedTime.year());

            lcd.setCursor(0, 1);
            lcd.print("HORA: ");
            lcd.print(adjustedTime.hour() < 10 ? "0" : ""); // Zero à esquerda
            lcd.print(adjustedTime.hour());
            lcd.print(":");
            lcd.print(adjustedTime.minute() < 10 ? "0" : ""); // Zero à esquerda
            lcd.print(adjustedTime.minute());
            lcd.print(":");
            lcd.print(adjustedTime.second() < 10 ? "0" : ""); // Zero à esquerda
            lcd.print(adjustedTime.second());

        } else if (displayMode ==1){
            lcd.setCursor(cupPosition + 4, 0); // Quarta coluna + posição da taça
            lcd.write(' '); // Limpa a taça

            // Atualiza a posição da taça
            if (movingRight) {
                cupPosition++;
            if (cupPosition > 10) { // Alcança o limite direito
            movingRight = false;
        }
        }   else {
        cupPosition--;
            if (cupPosition < 0) { // Alcança o limite esquerdo
            movingRight = true;
        }
        }
      // Exibe a taça na nova posição
      lcd.setCursor(cupPosition + 4, 0); // Quarta coluna + nova posição da taça
      lcd.write(byte(1)); // Mostra a taça

      // Exibe "VINHARIA FIAP" na segunda linha
      lcd.setCursor(0, 1); // Segunda linha
      lcd.print("VINICULA FIAP"); // Texto a ser exibido
      delay(200); // Ajuste a velocidade do movimento

        } else if (displayMode == 2) {
            // Mostrar Umidade
            lcd.setCursor(0, 0);
            lcd.print("Umidade: ");
            lcd.print(humidity);
            lcd.print(" %");
            lcd.setCursor(0, 1);
            lcd.print("umidade ok");
        } else if (displayMode == 3) {
            // Mostrar Temperatura
            lcd.setCursor(0, 0);
            lcd.print("Temp: ");
            lcd.print(temperature);
            lcd.print(" C");
            lcd.setCursor(0, 1);
            lcd.print("temperatura ok");
        } else if (displayMode == 4) {
            // Mostrar Luminosidade
            lcd.setCursor(0, 0);
            lcd.print("Luz: ");
            lcd.print(intensidadeLuz);
            lcd.print(" %");
        }

        // Log e armazenamento de dados
        if (LOG_OPTION && adjustedTime.minute() != lastLoggedMinute) {
            lastLoggedMinute = adjustedTime.minute();

            // Verifica se a leitura é válida
            if (!isnan(humidity) && !isnan(temperature)) {
                // Gravar dados na EEPROM
                int tempInt = (int)(temperature * 100);
                int humiInt = (int)(humidity * 100);

                EEPROM.put(currentAddress, now.unixtime());
                EEPROM.put(currentAddress + 4, tempInt);
                EEPROM.put(currentAddress + 6, humiInt);

                // Atualiza o endereço para o próximo registro
                getNextAddress();

                // Chama get_log() para imprimir os dados no Serial Monitor
                get_log();
            }
        }
        // Verifica condições e ativa buzzer e LED
            if (temperature >= 10 && temperature <= 16) {
                digitalWrite(buzzer, HIGH);
                digitalWrite(ledRed2, HIGH);
                digitalWrite(ledGreen2, LOW);
                digitalWrite(ledYellow2, LOW);
                alertMessage = "Temp critica!";
                lcd.setCursor(0, 1);
                lcd.print("Temperatura: ");
                lcd.print(temperature);
                lcd.print("C");
          } else if (temperature >6  && temperature < 9){
                digitalWrite(buzzer, HIGH);
                digitalWrite(ledRed2, HIGH);
                digitalWrite(ledGreen2, LOW);
                digitalWrite(ledYellow2, LOW);
                alertMessage = "Temp alta!";
                lcd.setCursor(0, 1);
                lcd.print("Temperatura: ");
                lcd.print(temperature);
                lcd.print("C");
        }   else if (temperature >= 1 && temperature<=5) {
                digitalWrite(buzzer, HIGH);
                digitalWrite(ledRed2, HIGH);
                digitalWrite(ledGreen2, LOW);
                digitalWrite(ledYellow2, LOW);
                alertMessage = "Temp baixa!";
                lcd.setCursor(0, 1);
                lcd.print("Temperatura: ");
                lcd.print(temperature);
                lcd.print("C");
          } else if (humidity > HUM_MAX) {
                digitalWrite(buzzer, HIGH);
                digitalWrite(ledRed3, HIGH);
                digitalWrite(ledGreen3, LOW);
                digitalWrite(ledYellow3,LOW);
                alertMessage = "Umid alta!";
                lcd.setCursor(0, 1);
                lcd.print("Umidade: ");
                lcd.print(humidity);
                lcd.print(" %");
          } else if (humidity < HUM_MIN) {
                digitalWrite(buzzer, HIGH);
                digitalWrite(ledRed3, HIGH);
                digitalWrite(ledGreen3, LOW);
                digitalWrite(ledYellow3, LOW);
                alertMessage= "Umid baixa!";
                lcd.setCursor(0, 1);
                lcd.print("Umidade: ");
                lcd.print(humidity);
                lcd.print(" %");
          }else if (intensidadeLuz>70){
                digitalWrite(buzzer, HIGH);
                digitalWrite(ledRed3, HIGH);
                digitalWrite(ledGreen3, LOW);
                digitalWrite(ledYellow3, LOW);
                alertMessage= "Lum critica!";
                lcd.setCursor(0, 1);
                lcd.print("Umidade: ");
                lcd.print(intensidadeLuz);
                lcd.print(" %");
          }else if (intensidadeLuz<20 && intensidadeLuz<70){
                digitalWrite(buzzer, HIGH);
                digitalWrite(ledRed3, LOW);
                digitalWrite(ledGreen3, LOW);
                digitalWrite(ledYellow3, HIGH);
                alertMessage= "Lum alta!";
                lcd.setCursor(0, 1);
                lcd.print("Umidade: ");
                lcd.print(intensidadeLuz);
                lcd.print(" %");
                
          }else {
            digitalWrite(buzzer, LOW);
            digitalWrite(ledRed3, LOW);
                digitalWrite(ledGreen3, LOW);
                digitalWrite(ledYellow3, LOW);
                alertMessage = ""; // Reseta a mensagem de alerta
                
      delay(500);  
  }
  
  // Exibe a mensagem de alerta se houver
  if (alertMessage.length() > 0) {
    lcd.clear();
    lcd.print(alertMessage);
    delay(10000); // Exibe o alerta por 2 segundos
  }

  delay(500); // Aguarda meio segundo antes de repetir
        // Saída Serial
        if (SERIAL_OPTION) {
            Serial.print(adjustedTime.timestamp(DateTime::TIMESTAMP_FULL));
            Serial.print("\t");
            Serial.print(temperature);
            Serial.print(" C\t\t");
            Serial.print(humidity);
            Serial.println(" %");
        }
    }

    // Controle dos LEDs com base na intensidade de luz
    if (intensidadeLuz < 20) {
        digitalWrite(ledGreen, HIGH); // Acende o LED verde
        digitalWrite(ledYellow, LOW); // Apaga o LED amarelo
        digitalWrite(ledRed, LOW); // Apaga o LED vermelho
        digitalWrite(buzzer, LOW); // Apaga o buzzer
   }else if (intensidadeLuz >= 21 && intensidadeLuz < 70) {
        digitalWrite(ledGreen, LOW); // Apaga o LED verde
        digitalWrite(ledYellow, HIGH); // Acende o LED amarelo
        digitalWrite(ledRed, LOW); // Apaga o LED vermelho
        digitalWrite(buzzer, HIGH); // Acende o buzzer
        delay(50); // Mantém o buzzer ligado por 50 ms
        digitalWrite(buzzer, LOW); // Apaga o buzzer
        delay(1000); // Espera 1000 ms antes de repetir
   }else {
        digitalWrite(ledGreen, LOW); // Apaga o LED verde
        digitalWrite(ledYellow, LOW); // Apaga o LED amarelo
        digitalWrite(ledRed, HIGH); // Acende o LED vermelho
        digitalWrite(buzzer, HIGH); // Acende o buzzer
        lcd.setCursor(0, 1);
        lcd.print("Luz: ");
        lcd.print(intensidadeLuz);
        lcd.print(" %");
    }
}
