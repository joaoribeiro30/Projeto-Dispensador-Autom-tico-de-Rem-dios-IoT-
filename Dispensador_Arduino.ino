#include <Wire.h>
#include "RTClib.h"
#include <LiquidCrystal_I2C.h>
#include <Stepper.h> 
#include <SoftwareSerial.h> 

RTC_DS3231 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2); 
SoftwareSerial meuBluetooth(7, 5); 

const int pinoBotao1 = 2; 
const int pinoBuzzer = 6; 
const int pinoMotor1 = 8;
const int pinoMotor2 = 9;
const int pinoMotor3 = 10;
const int pinoMotor4 = 11;

const int passosPorVolta = 2038; 
const int passosPorCompartimento = 1000; 
Stepper meuMotor(passosPorVolta, pinoMotor1, pinoMotor3, pinoMotor2, pinoMotor4);

int alarmeHora = 8;     
int alarmeMinuto = 0;
bool alarmeAtivo = false;
bool alarmeJaTocou = false;
int modo = 0; 
bool precisaRedesenhar = true; 
String comandoBluetooth = ""; 

int ultimoEstadoBtn1 = HIGH;
unsigned long tempoApertado = 0;
const long tempoLimiteClique = 150; 
const long tempoLimiteDiminuir = 2000; 
const long intervaloRepeticao = 200; 
unsigned long tempoAnteriorAjuste = 0;

unsigned long tempoAnteriorTela = 0; 
const long intervaloTela = 1000;     
unsigned long tempoAnteriorAlarme = 0;
const long intervaloBlink = 300; 
bool luzLcdLigada = true;

int ultimoSegundo = -1;
int ultimoMinuto = -1;
int ultimaHora = -1;

void mostrarHora(DateTime now);
void mostrarTelaAjuste(int ajusteModo);
void tocarAlarme(bool ligar); 
void desligarMotor();
void reiniciarI2C(); 
void verificarBluetooth(); 
void processarComando(String cmd); 

void desligarMotor() {
  digitalWrite(pinoMotor1, LOW);
  digitalWrite(pinoMotor2, LOW);
  digitalWrite(pinoMotor3, LOW);
  digitalWrite(pinoMotor4, LOW);
}

void reiniciarI2C() {
  Wire.begin(); 
  if (!rtc.begin()) { 
     Serial.println("Falha ao reiniciar o RTC!");
  }
  lcd.init(); 
  lcd.backlight();
  delay(100); 
}

void processarComando(String cmd) {
    
    String upperCmd = cmd;
    upperCmd.toUpperCase(); 

    if (upperCmd.startsWith("A") && upperCmd.length() == 5) {
        int h = upperCmd.substring(1, 3).toInt(); 
        int m = upperCmd.substring(3, 5).toInt(); 

        if (h >= 0 && h <= 23 && m >= 0 && m <= 59) {
            alarmeHora = h;
            alarmeMinuto = m;
            precisaRedesenhar = true;
            meuBluetooth.println("OK: ALARME");
        } else {
            meuBluetooth.println("ERRO: HORA");
        }
        
    } else if (upperCmd.startsWith("RODAR")) {
        meuMotor.step(passosPorCompartimento);
        desligarMotor();
        meuBluetooth.println("OK: MOTOR");
        
    } else if (upperCmd.startsWith("PARAR")) {
        if (alarmeAtivo) {
          alarmeAtivo = false; 
          alarmeJaTocou = true; 
          tocarAlarme(false); 
          modo = 0; 
          precisaRedesenhar = true;
          meuBluetooth.println("OK: ALARME PARADO");
        } else {
          meuBluetooth.println("OK: ALARME JA PARADO");
        }
        
    } else {
        meuBluetooth.println("ERRO: COMANDO");
    }
}

void verificarBluetooth() {
  while (meuBluetooth.available()) {
    char c = meuBluetooth.read(); 
    
    if (c != '\n' && c != '\r') {
        comandoBluetooth += c;
    }
    
    if (c == '\n' || c == '\r') {
      if (comandoBluetooth.length() > 0) {
        processarComando(comandoBluetooth);
        comandoBluetooth = "";
      }
    }
    
    else if (comandoBluetooth.length() > 6) { 
        processarComando(comandoBluetooth);
        comandoBluetooth = "";
    }
  }
}

void mostrarHora(DateTime now) {
  if (precisaRedesenhar) {
    lcd.clear();
    ultimoSegundo = -1; ultimoMinuto = -1; ultimaHora = -1;
  }
  if (now.hour() != ultimaHora) {
    lcd.setCursor(6, 0); if (now.hour() < 10) lcd.print("0"); lcd.print(now.hour()); ultimaHora = now.hour();
  }
  if (now.minute() != ultimoMinuto) {
    lcd.setCursor(9, 0); if (now.minute() < 10) lcd.print("0"); lcd.print(now.minute()); ultimoMinuto = now.minute();
  }
  if (now.second() != ultimoSegundo) {
    lcd.setCursor(12, 0); if (now.second() < 10) lcd.print("0"); lcd.print(now.second()); ultimoSegundo = now.second();
  }
  if (precisaRedesenhar) {
    lcd.setCursor(0, 0); lcd.print("Hora: "); lcd.setCursor(8, 0); lcd.print(":"); lcd.setCursor(11, 0); lcd.print(":");
    
    lcd.setCursor(0, 1); lcd.print("Alarme: "); 
    if (alarmeHora < 10) lcd.print("0"); lcd.print(alarmeHora);
    lcd.print(":");
    if (alarmeMinuto < 10) lcd.print("0"); lcd.print(alarmeMinuto);
    
    precisaRedesenhar = false; 
  }
}

void mostrarTelaAjuste(int ajusteModo) {
  if (precisaRedesenhar) {
    lcd.clear();
    lcd.setCursor(0, 0);
    
    if (ajusteModo == 1) { lcd.print("Ajustar HORA (+/-)"); }
    else { lcd.print("Ajustar MINUTO (+/-)"); }

    lcd.setCursor(0, 1); 
    if (ajusteModo == 1) {
      lcd.print(">"); 
      if (alarmeHora < 10) lcd.print("0"); lcd.print(alarmeHora);
      lcd.print(":");
      if (alarmeMinuto < 10) lcd.print("0"); lcd.print(alarmeMinuto);
    } else {
      if (alarmeHora < 10) lcd.print("0"); lcd.print(alarmeHora);
      lcd.print(":");
      lcd.print(">"); 
      if (alarmeMinuto < 10) lcd.print("0"); lcd.print(alarmeMinuto);
    }
    
    precisaRedesenhar = true;
    ultimoSegundo = -1; ultimoMinuto = -1; ultimaHora = -1;
  }
}

void tocarAlarme(bool ligar) {
  if (ligar == false) {
    analogWrite(pinoBuzzer, 0); 
    lcd.backlight(); 
    desligarMotor(); 
    precisaRedesenhar = true;
    ultimoSegundo = -1; ultimoMinuto = -1; ultimaHora = -1;
  }
}

void setup () {
  Serial.begin(9600); 
  meuBluetooth.begin(9600); 

  Wire.begin();
  lcd.init();
  lcd.backlight();

  if (!rtc.begin()) {
    Serial.println("ERRO: RTC não encontrado!");
    lcd.clear();
    lcd.print("Erro no RTC!");
    while (1); 
  }

  pinMode(pinoBotao1, INPUT_PULLUP);
  pinMode(pinoBuzzer, OUTPUT);
  pinMode(pinoMotor1, OUTPUT);
  pinMode(pinoMotor2, OUTPUT);
  pinMode(pinoMotor3, OUTPUT);
  pinMode(pinoMotor4, OUTPUT);
  desligarMotor(); 

  meuMotor.setSpeed(12); 
  analogWrite(pinoBuzzer, 0); 
  
}

void loop () {
  
  verificarBluetooth();

  DateTime now = rtc.now(); 

  if (!now.isValid()) {
    reiniciarI2C(); 
    precisaRedesenhar = true; 
    return; 
  }

  int estadoBtn1 = digitalRead(pinoBotao1);

  if (estadoBtn1 == LOW) { 
    if (ultimoEstadoBtn1 == HIGH) { 
      tempoApertado = millis(); 
      
      if (alarmeAtivo) { 
        alarmeAtivo = false; 
        alarmeJaTocou = true; 
        tocarAlarme(false); 
        modo = 0; 
        precisaRedesenhar = true;
      }
    } else { 
      unsigned long duracao = millis() - tempoApertado;

      if (duracao > tempoLimiteClique && duracao <= tempoLimiteDiminuir) {
         if (millis() - tempoAnteriorAjuste >= intervaloRepeticao) {
            tempoAnteriorAjuste = millis();
            
            if (modo == 1) { alarmeHora++; if (alarmeHora > 23) alarmeHora = 0; precisaRedesenhar = true; }
            else if (modo == 2) { alarmeMinuto++; if (alarmeMinuto > 59) alarmeMinuto = 0; precisaRedesenhar = true; }
         }
      } 
      else if (duracao > tempoLimiteDiminuir) {
         if (millis() - tempoAnteriorAjuste >= intervaloRepeticao) {
            tempoAnteriorAjuste = millis();
            
            if (modo == 1) { alarmeHora--; if (alarmeHora < 0) alarmeHora = 23; precisaRedesenhar = true; }
            else if (modo == 2) { alarmeMinuto--; if (alarmeMinuto < 0) alarmeMinuto = 59; precisaRedesenhar = true; }
         }
      }
    }
  } else { 
    if (ultimoEstadoBtn1 == LOW) { 
      if (millis() - tempoApertado < tempoLimiteClique) { 
        if (!alarmeAtivo) { 
            modo++; 
            if (modo > 2) modo = 0; 
            precisaRedesenhar = true; 
        }
      }
      tempoApertado = 0; 
    }
  }
  ultimoEstadoBtn1 = estadoBtn1; 
  
  if (now.minute() != alarmeMinuto) {
    alarmeJaTocou = false; 
  }
  if (now.hour() == alarmeHora && now.minute() == alarmeMinuto && alarmeJaTocou == false && modo == 0) {
    alarmeAtivo = true;
    modo = 3; 
    precisaRedesenhar = true; 
    
    meuMotor.step(passosPorCompartimento);
    desligarMotor(); 
  }

  switch (modo) {
    case 0: 
      if (millis() - tempoAnteriorTela >= intervaloTela) {
        tempoAnteriorTela = millis(); 
        mostrarHora(now); 
      }
      if (precisaRedesenhar) {
        mostrarHora(now);
      }
      break;
    case 1: 
    case 2: 
      if (precisaRedesenhar) { mostrarTelaAjuste(modo); precisaRedesenhar = false; }
      
      lcd.setCursor(11, 1); 
      if (now.hour() < 10) lcd.print("0"); lcd.print(now.hour());
      lcd.print(":");
      if (now.minute() < 10) lcd.print("0"); lcd.print(now.minute());
      break;
      
    case 3: 
      if (precisaRedesenhar) { 
        lcd.clear();
        lcd.setCursor(0, 0); lcd.print("!! ALARME !!");
        lcd.setCursor(0, 1); lcd.print("Hora de tomar!");
        precisaRedesenhar = false;
        tempoAnteriorAlarme = millis(); 
        luzLcdLigada = true;
      }
      if (millis() - tempoAnteriorAlarme >= intervaloBlink) {
        tempoAnteriorAlarme = millis();
        if (luzLcdLigada) {
          lcd.noBacklight();
          analogWrite(pinoBuzzer, 0); 
          luzLcdLigada = false;
        } else {
          lcd.backlight();
          analogWrite(pinoBuzzer, 100); 
          luzLcdLigada = true;
        }
      }
      break;
  }
}
