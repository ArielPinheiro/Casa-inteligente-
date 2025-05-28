/* ISSO AQ VAI SER O CODIGO DA PORTA 







*/

/* ISSO AQ É DE TEMPERATURA
#include <LiquidCrystal.h>
LiquidCrystal LCD(7,6,5,4,3,2);
int SensorTempPino= 0;

void setup() {
	LCD.begin(16,2);
	LCD.print("Temperatura:");
	LCD.setCursor(0,1);
	LCD.print("     C     ");
}

void loop() {
	int SensorTempTensao=analogRead(SensorTempPino);
	float Tensao=SensorTempTensao*5;
	Tensao/=1024;
	float TemperaturaC=(Tensao-0.5)*100;
  	LCD.setCursor(0,1);
	LCD.print(TemperaturaC);
	LCD.setCursor(15,1);
  	delay(1000);
}
*/
  
  
/* ISSO AQ É O CODIGO DO GAS E DO SENSOR E DO BUZZER
const int mq135 = A0;
const int buzzer = 8;
const int ledAmarelo = 13; 
const int ledAmarelo2 = 12; 
const int PIR = 2; 
int contador = 0;

void alarme_dois_tons() {
  int freqAlta = 2000;
  int freqBaixa = 800;
  int duracaoTom = 250;
  tone(buzzer, freqAlta, duracaoTom);
  delay(duracaoTom);
  tone(buzzer, freqBaixa, duracaoTom);
  delay(duracaoTom);
}

void verificarVazamentoDeGas() {
  int estadoMQ135 = analogRead(mq135);
	Serial.println("Valor Do gas " + String(estadoMQ135));
  
  
  if (estadoMQ135 > 700) {
    if (contador == 0) {
      Serial.println("Detectamos vazamento de gas. Alarme em:");
      Serial.println("3");
      delay(1000);
      Serial.println("2");
      delay(1000);
      Serial.println("1");
      delay(1000);
      contador = 1;
    }
    alarme_dois_tons();
    digitalWrite(ledAmarelo, HIGH);
  } else {
    if (contador != 0) {
      noTone(buzzer);
      contador = 0;
      Serial.println("Ufa! Nao ha deteccao de gas no ambiente.");
    }
    digitalWrite(ledAmarelo, LOW);
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(ledAmarelo, OUTPUT);
  pinMode(ledAmarelo2, OUTPUT);
  pinMode(PIR, INPUT);
  pinMode(buzzer, OUTPUT); 

  Serial.println("Calibrando o sensor PIR...");
  Serial.println("Por favor, nao se mova na frente do sensor.");
  delay(15000);
  Serial.println("Sensor PIR calibrado e pronto!");
}

void loop() {
  verificarVazamentoDeGas();

  int estadoPIR = digitalRead(PIR);
  Serial.println("Valor Do PIR " + String(estadoPIR));
  if (estadoPIR == HIGH) {
    digitalWrite(ledAmarelo2, HIGH);
    Serial.println("Movimento detectado! LED aceso.");
    delay(1000); // trava por 10 segundos
  } else {
    digitalWrite(ledAmarelo2, LOW);
    Serial.println("Sem movimento. LED apagado.");
    delay(5000);
  }

  delay(100); // pequena pausa para estabilidade
}
*/