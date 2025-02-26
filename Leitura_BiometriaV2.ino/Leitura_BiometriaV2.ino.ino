#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Fingerprint.h>

#define LCD_ADDRESS 0x27
LiquidCrystal_I2C lcd(LCD_ADDRESS, 16, 2);
SoftwareSerial mySerial(2, 3);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

void setup() {
  Serial.begin(9600);
  
  // Inicializa o LCD e mostra a tela de inicialização
  lcd.begin(16, 2); 
  lcd.backlight();
  lcd.print("Inicializando..."); // Tela de inicialização
  delay(2000); // Aguarda 2 segundos

  lcd.clear();
  lcd.print("Verificando");
  lcd.setCursor(0, 1);
  lcd.print("sensor..."); // Exibe a mensagem de verificação do sensor

  while (!Serial);  
  delay(100);
  Serial.println("\n\nTeste de sensor de impressao digital");

  finger.begin(57600);
  delay(5);
  
  if (finger.verifyPassword()) {
    Serial.println("Sensor de impressao encontrado!");
    lcd.clear();
    lcd.print("Sensor OK!");
    delay(2000); // Mantém a mensagem por 2 segundos
  } else {
    Serial.println("Sensor de impressao nao encontrado :(");
    lcd.clear();
    lcd.print("Sensor nao OK!");
    while (1) { delay(1); }
  }

  lcd.clear();
  lcd.print("Aguardando dedo"); // Mensagem padrão ao finalizar a inicialização

  Serial.println(F("Lendo parametros do sensor"));
  finger.getParameters();
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacidade: ")); Serial.println(finger.capacity);
  Serial.print(F("Nivel de seguranca: ")); Serial.println(finger.security_level);
  Serial.print(F("Endereco do dispositivo: ")); Serial.println(finger.device_addr, HEX);
  Serial.print(F("Comprimento do pacote: ")); Serial.println(finger.packet_len);
  Serial.print(F("Taxa de baud: ")); Serial.println(finger.baud_rate);

  finger.getTemplateCount();

  if (finger.templateCount == 0) {
    Serial.print("O sensor nao possui dados de impressao digital. Execute o exemplo 'enroll'.");
  } else {
    Serial.print("O sensor contem "); Serial.print(finger.templateCount); Serial.println(" templates");
  }
}

void loop() {
  getFingerprintID();
  delay(2000); // Espera 2 segundos entre as leituras
}

uint8_t getFingerprintID() {
  Serial.println("Aguardando dedo...");
  lcd.clear(); // Limpa a tela do LCD
  lcd.print("Aguardando dedo..."); // Mostra mensagem no LCD

  uint8_t p = finger.getImage();
  while (p != FINGERPRINT_OK) {
    // Apenas espera até que um dedo seja colocado
    p = finger.getImage();
  }

  Serial.println("Imagem capturada");

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) {
    Serial.println("Erro ao converter imagem");
    return p;
  }

  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.print("ID #"); Serial.println(finger.fingerID);
    lcd.clear(); // Limpa a tela do LCD
    lcd.print("ID "); lcd.print(finger.fingerID); // Mostra o ID no LCD
    delay(2000); // Espera 2 segundos antes de pedir o dedo novamente
  } else {
    Serial.println("Nao foi encontrada uma correspondencia");
    lcd.clear(); // Limpa a tela do LCD
    lcd.print("Nao encontrado");
    delay(2000); // Espera 2 segundos antes de pedir o dedo novamente
  }

  // Exibe a mensagem para colocar o dedo novamente
  lcd.clear(); // Limpa a tela do LCD
  lcd.print("Aguardando dedo...");
  
  return finger.fingerID;
}
