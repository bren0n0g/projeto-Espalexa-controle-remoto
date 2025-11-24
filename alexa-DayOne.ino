#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <Espalexa.h>

// --- DEFINIÇÃO DOS PINOS DOS LEDS ---
#define PIN_LED1 25


// prototypes
boolean connectWifi();
bool modoAnimacao = false;
bool modoBlink = false;

//callback functions
void firstLightChanged(uint8_t brightness);


// --- CONFIGURAÇÃO DO WIFI (ALTERE AQUI) ---
const char* ssid = "Machado_2G_Ext";
const char* password = "20091975Fe";

boolean wifiConnected = false;

Espalexa espalexa;

EspalexaDevice* device3;

void setup() {
  Serial.begin(115200);

  // --- CONFIGURAÇÃO DOS PINOS COMO SAÍDA ---
  pinMode(PIN_LED1, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  // Inicializa wifi
  wifiConnected = connectWifi();

  if (wifiConnected) {

    // Define os dispositivos (O nome aqui é o que você fala pra Alexa: "Alexa, ligue a Luz 1")
    espalexa.addDevice("LED", firstLightChanged);
    espalexa.begin();

  } else {
    while (1) {
      Serial.println("Cannot connect to WiFi. Please check data and reset the ESP.");
      delay(2500);
    }
  }
}

void loop() {
  espalexa.loop();


  // --- FUNÇÕES DE CONTROLE (CALLBACKS) ---

  if (modoAnimacao) {
    static int i = 1;      // Mantém o valor do contador
    static bool subindo = true; // Nova variável: define a direção

    // Executa o cálculo
    dacWrite(PIN_LED1, i / 2 + 128);

    // Lógica do "Vai e Volta"
    if (subindo) {
      i++;
      if (i >= 120) {
        subindo = false; // Chegou no topo, hora de descer
      }
    } else {
      i--;
      if (i <= 1) {
        subindo = true; // Chegou no fundo, hora de subir
      }
    }

    delay(15); 
  }

  if (modoBlink) {
    static int i = 1;      // Mantém o valor do contador
    static bool subindo = true; // Nova variável: define a direção

    // Executa o cálculo
    dacWrite(PIN_LED1, 5*i + 128);

    // Lógica do "Vai e Volta"
    if (subindo) {
      i++;
      if (i >= 10) {
        subindo = false; // Chegou no topo, hora de descer
      }
    } else {
      i--;
      if (i <= 1) {
        subindo = true; // Chegou no fundo, hora de subir
      }
    }

    delay(10); 
  }
}

// Controle do LED no GPIO 4
void firstLightChanged(uint8_t brightness) {

  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.print("Device 1 changed to ");

  if (brightness == 35) {  // Gatilho para o "Modo Secreto"
    Serial.println("Modo Animação ATIVADO");
    modoAnimacao = true;
  } else {
    // Se for qualquer outro valor, desliga a animação e acende normal
    modoAnimacao = false;
  }
  if (brightness == 78) {  // Gatilho para o "Modo Secreto"
    Serial.println("Modo Blink ATIVADO");
    modoBlink = true;
  } else {
    // Se for qualquer outro valor, desliga a animação e acende normal
    modoBlink = false;
  }
  if (brightness > 0) {
    Serial.print("ON, brightness ");
    Serial.println(brightness);
    dacWrite(PIN_LED1, brightness / 2 + 128);
  } else {
    Serial.println("OFF");
    dacWrite(PIN_LED1, 0);
  }
}



// connect to wifi – returns true if successful or false if not
boolean connectWifi() {
  boolean state = true;
  int i = 0;

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 20) {
      state = false;
      break;
    }
    i++;
  }
  Serial.println("");
  if (state) {
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    digitalWrite(LED_BUILTIN, HIGH);
    delay(150);
    digitalWrite(LED_BUILTIN, LOW);

  } else {
    Serial.println("Connection failed.");
  }
  return state;
}