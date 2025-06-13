//Library
#include <LiquidCrystal_I2C.h>
#include <DHT11.h>
#include <DHT.h>
#include <DHT_U.h>
//#include <Servo.h>
#include <ESP32Servo.h>

//atalho de arrumar o codigo(SHIFT + ALT + f)

//Variaveis acenderLEDAoDetectarPresenca
const int PIR = 5; //GPIO que o PIR esta plugado
const int LedVermelho = 2;

//Variaveis verificarVazamentoDegas
const int MQ135 = 34;
const int buzzer = 12;

//Variaveis GLobais Abrir e fechar trava
const int rele = 15;

//Variaveis temperatura e umidade
#define DHTPINO 23
#define DHTTYPE DHT11

DHT dht(DHTPINO, DHT11);
LiquidCrystal_I2C lcd(0x27, 20, 4);

//variavel do tipo servo
Servo motor;
const int servoMotor = 18;

void acenderLEDAoDetectarPresenca() {
  int estadoPIR = digitalRead(PIR);  //

  if (estadoPIR == HIGH) {
    digitalWrite(LedVermelho, HIGH);
    Serial.println("LED ligado");
  } else {
    digitalWrite(LedVermelho, LOW);
    Serial.println("LED desligado");
  }
}

void verificarVazamentoDegas() {
  int estadoMQ135 = analogRead(MQ135);

  //Serial.println(estadoMQ135);

  if (estadoMQ135 >= 200) {
    alarme_dois_tons();
  } else {
    noTone(buzzer);  //Para de tocar o alarme pelo buzzer.
  }
}

void alarme_dois_tons() {
  int freqAlta = 2000;
  int freqBaixa = 800;
  int duracaoTom = 250;

  tone(buzzer, freqAlta, duracaoTom);
  delay(duracaoTom);
  tone(buzzer, freqBaixa, duracaoTom);
  delay(duracaoTom);
}

void verificarTemperaturaEUmidade() {
  float temperatura = dht.readTemperature();
  float umidade = dht.readHumidity();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temperatura: " + String(temperatura) + "C");
  lcd.setCursor(0, 1);
  lcd.print("Umidade: " + String(umidade) + "%");

  //Serial.println("Temperatura: " + String(temperatura) + "C");
  //Serial.println("Umidade: " + String(umidade) + "%");
  delay(5000);
}

void abrirPortaAutomatica(){
motor.write(90);
delay(5000);
}

void fecharPortaAutomatica(){
motor.write(0);
delay(5000);
}

void trancarPorta(){
 digitalWrite(rele, LOW);
 Serial.println("Porta trancada...");
 delay(1500);
}

void destrancarPorta(){
   String senhaDigitada;

 Serial.println("Por gentileza, digite a senha para destrancar a porta.");
 while(! Serial.available());
 senhaDigitada = Serial.readString();

 if(senhaDigitada == "senhaCerta"){
digitalWrite(rele, HIGH);
Serial.println("Porta destrancada...");

abrirPortaAutomatica();//chama a função que irá abrir a porta automática,juy
 fecharPortaAutomatica(); //fecha a porta
 }else{
 if(motor.read() != 0){
   fecharPortaAutomatica();
}

Serial.println("Senha inválida...cuidado, na terceira tentativa a policia será acionada...");
delay(3000);
 }

 digitalWrite(rele, HIGH);
 Serial.println("Porta destrancada...");
 delay(1500);
}



void setup() {
  Serial.begin(9600);  //inicia a comunicação com o monitor

  dht.begin();  //Inicializando o DHT11
  lcd.init();   //lcd.begin();
  lcd.backlight();

  //Fala ao programa se é uma entrada ou uma saida
  pinMode(PIR, INPUT);
  pinMode(LedVermelho, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(MQ135, INPUT);
  motor.attach(servoMotor); //variavel servoMotor armazena o GPIO que o motor está
  pinMode(rele, OUTPUT);

  //deixa trancado a mini solenoide
  digitalWrite(rele, LOW); //muda o estado inicial do pino ligado/desligado

  //
  motor.write(0);//por enquanto no angulo 0

  Serial.println("Sensores sendo calibrados, aguente firme!!");
  delay(5000);
  Serial.println("Sensores calibrados! Obrigada por esperar.");

  lcd.setCursor(0, 0);  //primeira linha
  lcd.print("Iniciando leituras");
  lcd.setCursor(0, 1);  //segunda linha
  lcd.print("Obrigada!");
}



void loop() {
  destrancarPorta();
  trancarPorta();
  //acenderLEDaoDetectarPresenca();
  //verificarVazamentoDeGas();
  //alarme_dois_tons();
  //verificarTemperaturaEUmidade();
  //abrirPortaAutomatica();
  //fecharPortaAutomatica();
}
