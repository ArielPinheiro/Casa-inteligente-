//library
#include <DHT11.h>
#include <DHT.h>
#include <DHT_U.h>
#include <ESP32Servo.h>
#include <LiquidCrystal_I2C.h>

//Variaveis temperatura e umidade
#define DHTPINO 32
#define DHTTYPE DHT11

DHT dht(DHTPINO, DHT11);
LiquidCrystal_I2C lcd(0x27, 20, 4);

//Variavel buzzer
const int buzzer = 12;

//Variavel Abrir e fechar rele
const int rele = 15;

//variavel do servo
Servo motor;
const int servoMotor = 18;

//variavel senha
int senhaUsuario, senhaCorreta, erro, acerto, tentativa;



void verificarTempEUmidade() {
  float temperatura = dht.readTemperature();
  float umidade = dht.readHumidity();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temperatura: " + String(temperatura) + "C");
  lcd.setCursor(0, 1);
  lcd.print("Umidade: " + String(umidade) + "%");

  delay(5000);
}



// void pedirSenha() {
//   Serial.println("Ola, vamos configurar sua senha?");
//   Serial.println("Digite a senha que preferir!");
//   while (!Serial.available())
//     ;
//   senhaUsuario = Serial.parseInt();
// }



// void destrancarPorta() {
//   do {
//     Serial.println("Digite sua senha");
//     while (!Serial.available())
//       ;
//     senhaCorreta = Serial.parseInt();

//     if (senhaUsuario == senhaCorreta) {
//       motor.write(90);  //destranca porta
//       Serial.println("Senha correta. Porta destrancada");
//       delay(5000);
//       motor.write(0);
//       tentativa = 0;

//     } else {
//       motor.write(0);
//       Serial.println("Senha incorreta. Tente novamente \n");
//       tentativa++;

//       if (tentativa == 3) {
//         Serial.println("Muitas tentativas incorretas, bloqueio de 2 minutos");
//         delay(120000);
//       }
//     }
//   } while (tentativa != 0);
// }



void abrirPortaAutomatica() {
  motor.write(90);
  delay(2000);
}



void fecharPortaAutomatica() {
  motor.write(0);
  delay(2000);
}



void trancarPorta() {
  digitalWrite(rele, LOW);
  Serial.println("Porta destrancada");
}



void destrancarPorta() {
  digitalWrite(rele, HIGH);
  Serial.println("Porta destrancada");
  delay(500);
}




void alarmeTemperatura() {
  float temperatura = dht.readTemperature();
  if (temperatura >= 25) {
    somDoAlarme();
    Serial.println("Temperatura da casa alta, verificar.");
  } else {
    digitalWrite(buzzer, LOW);
  }
}



void somDoAlarme() {
  const int freqAlta = 2000;
  const int freqBaixa = 800;
  const int duracaoTom = 250;

  tone(buzzer, freqAlta, duracaoTom);
  delay(duracaoTom);
  tone(buzzer, freqBaixa, duracaoTom);
  delay(duracaoTom);
}



void setup() {
  //Inicializações
  Serial.begin(115200);
  dht.begin();
  lcd.init();
  lcd.backlight();

  //Tipos de Entrada
  pinMode(buzzer, OUTPUT);
  motor.attach(servoMotor);
  pinMode(rele, OUTPUT);

  //Estados Iniciais
  motor.write(0);
  digitalWrite(rele, LOW);

  //chamar
  //pedirSenha();

  //lcd
  lcd.setCursor(0, 0);  //primeira linha
  lcd.print("Iniciando leituras");
  lcd.setCursor(0, 1);  //segunda linha
  lcd.print("Obrigada!");

  Serial.println("Sensores sendo calibrados, aguente firme!!");
  delay(5000);
  Serial.println("Sensores calibrados! Obrigada por esperar.");
}



void loop() {
  destrancarPorta();
  abrirPortaAutomatica();
  fecharPortaAutomatica();
  trancarPorta();
  alarmeTemperatura();
  verificarTempEUmidade();
}