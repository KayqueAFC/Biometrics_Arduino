#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Fingerprint.h>

// Endereço do LCD 16x2 com módulo I2C
#define LCD_ADDRESS 0x27
LiquidCrystal_I2C lcd(LCD_ADDRESS, 16, 2);

SoftwareSerial mySerial(2, 3); // Definir pinos de comunicação serial
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

uint8_t id;

void setup() {
  Serial.begin(9600);
  
  // Inicializa o LCD
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.print("Iniciando...");

  while (!Serial);  
  delay(100);
  Serial.println("\n\nEnrolamento de Impressao Digital");

  finger.begin(57600);

  if (finger.verifyPassword()) {
    Serial.println("Sensor de impressao encontrado!");
    lcd.clear();
    lcd.print("Sensor OK!");
  } else {
    Serial.println("Sensor nao encontrado :(");
    lcd.clear();
    lcd.print("Sensor nao OK");
    while (1) { delay(1); }
  }

  finger.getParameters();
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacidade: ")); Serial.println(finger.capacity);
  Serial.print(F("Nivel de seguranca: ")); Serial.println(finger.security_level);
  Serial.print(F("Endereco do dispositivo: ")); Serial.println(finger.device_addr, HEX);
  Serial.print(F("Comprimento do pacote: ")); Serial.println(finger.packet_len);
  Serial.print(F("Taxa de baud: ")); Serial.println(finger.baud_rate);
}

uint8_t readnumber() {
  uint8_t num = 0;

  while (num == 0) {
    while (!Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

void loop() {
  Serial.println("Pronto para cadastrar uma impressao digital!");
  Serial.println("Digite o ID (de 1 a 127) para salvar a impressao...");
  lcd.clear();
  lcd.print("Digite ID no PC:");

  id = readnumber();
  if (id == 0) return; // ID 0 não é permitido
  
  Serial.print("Cadastrando ID #");
  Serial.println(id);
  lcd.clear();
  lcd.print("Cadastrando ID ");
  lcd.print(id);

  while (!getFingerprintEnroll());
}

uint8_t getFingerprintEnroll() {
  int p = -1;
  Serial.print("Aguardando dedo para cadastrar ID #"); Serial.println(id);
  lcd.clear();
  lcd.print("Aguardando dedo");

  // Aguarda até que um dedo seja colocado
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Imagem capturada");
        lcd.clear();
        lcd.print("Imagem capturada");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Erro de comunicacao");
        lcd.clear();
        lcd.print("Erro comunicacao");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Falha ao capturar imagem");
        lcd.clear();
        lcd.print("Falha na imagem");
        break;
      default:
        Serial.println("Erro desconhecido");
        lcd.clear();
        lcd.print("Erro desconhecido");
        break;
    }
  }

  // Converte imagem
  p = finger.image2Tz(1);
  if (p != FINGERPRINT_OK) {
    Serial.println("Erro ao converter imagem");
    return p;
  }
  Serial.println("Imagem convertida");
  lcd.clear();
  lcd.print("Imagem convertida");

  // Aguarda dedo ser removido
  Serial.println("Remova o dedo");
  lcd.clear();
  lcd.print("Remova o dedo");
  delay(2000);
  
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }

  // Segunda leitura
  Serial.println("Coloque o mesmo dedo novamente");
  lcd.clear();
  lcd.print("Coloque dedo");
  
  p = -1;
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Imagem capturada");
        lcd.clear();
        lcd.print("Imagem capturada");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Erro de comunicacao");
        lcd.clear();
        lcd.print("Erro comunicacao");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Falha ao capturar imagem");
        lcd.clear();
        lcd.print("Falha na imagem");
        break;
      default:
        Serial.println("Erro desconhecido");
        lcd.clear();
        lcd.print("Erro desconhecido");
        break;
    }
  }

  p = finger.image2Tz(2);
  if (p != FINGERPRINT_OK) {
    Serial.println("Erro ao converter imagem");
    return p;
  }
  Serial.println("Imagem convertida");
  lcd.clear();
  lcd.print("Imagem convertida");

  // Cria modelo e armazena
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Impressao digital combinada!");
    lcd.clear();
    lcd.print("Cadastro OK");
  } else {
    Serial.println("Falha ao criar modelo");
    return p;
  }

  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("ID armazenado com sucesso!");
    lcd.clear();
    lcd.print("ID armazenado!");
  } else {
    Serial.println("Erro ao armazenar ID");
    lcd.clear();
    lcd.print("Erro no armazenamento");
    return p;
  }

  return true;
}
