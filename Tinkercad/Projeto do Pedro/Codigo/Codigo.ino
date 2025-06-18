//library
#include <DHT11.h>
#include <DHT.h>
#include <DHT_U.h>
#include <ESP32Servo.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <PubSubClient.h>

// --- WiFi & MQTT ---
const char* ssid = "ProjetosIoT_Esp32";         //sua rede wifi
const char* password = "Sen@i134";     //senha da sua rede wifi
const char* mqtt_server = "172.16.39.80";  //endereço do broker público
const int mqtt_port = 1883;       //porta do broker público, geralmente 1883

//Tópicos
const char* topic_trava = "shift-echo/sala/trava";
const char* topic_temp = "shift-echo/sala/temperatura";
const char* topic_umid = "shift-echo/sala/umidade";
const char* topic_porta = "shift-echo/sala/porta";

//Variaveis temperatura e umidade
#define DHTPINO 32
#define DHTTYPE DHT11

//Objetos
WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPINO, DHT11);
LiquidCrystal_I2C lcd(0x27, 20, 4);

unsigned long ultimaLeitura = 0;

//Variavel buzzer
const int buzzer = 13;
 long tempoPortaAberta = 0;

//Variavel Abrir e fechar rele
const int rele = 15;
int anguloAtual = 0;

//variavel do servo
Servo motor;
const int servoMotor = 18;

//variavel senha
int senhaUsuario, senhaCorreta, erro, acerto, tentativa;



// --- Funções WiFi e MQTT ---
void conectarWiFi() {  //verifica conexão wifi para somente depois iniciar o sistema
  Serial.println("Conectando ao WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");
}



void reconectarMQTT() {  //verifica e reconecta a conexão com o broker mqtt
  while (!client.connected()) {
    Serial.print("Reconectando MQTT...");
    String clientId = "ESP32Client-";
    clientId += String(WiFi.macAddress());
    if (client.connect( clientId.c_str() )) {
      Serial.println("Conectado!");
      client.subscribe(topic_trava);  //conecta ao topico da trava assim que estabelecer ligação com o broker
      client.subscribe(topic_porta);  //conecta ao topico da porta assim que estabelecer ligação com o broker
    } else {
      Serial.print("Falha: ");
      Serial.println(client.state());
      delay(5000);
    }
  }
}



/**
  Função para tratamento das mensagens de callback/retorno do broker de cada tópico subscrito (led, porta, etc.)

  char* é uma cadeia de caracteres em C como um vetor onde cada caractter/letra está em uma posição, 
  diferente de uma String em C++ que pode ser lida completamente
*/
void tratarMensagem(char* topic, byte* payload, unsigned int length) {  //
  String mensagem = "";
  for (int i = 0; i < length; i++) {  //concatena todas os char* para se ter o texto completo em String
    mensagem += (char)payload[i];
  }

  Serial.printf("Mensagem recebida [%s]: %s\n", topic, mensagem.c_str());

  //trava
  if (strcmp(topic, topic_trava) == 0) {  //tópico atual é o da trava?
    if (mensagem == "destrancar") {
      digitalWrite(rele, HIGH);
    } else if (mensagem == "trancar") {
      digitalWrite(rele, LOW);
    }
  }


if (strcmp(topic, topic_porta) == 0) {  //tópico atual é o da trava?
    if (mensagem == "abrir") {
      motor.write(90);
      anguloAtual = 90;
    } else if (mensagem == "fechar") {
      motor.write(0);
      anguloAtual = 0;
    }
  }
}



void verificarTempEUmidade() {
  float temperatura = dht.readTemperature();
  float umidade = dht.readHumidity();

  if (isnan(temperatura) || isnan(umidade)) {  //avisa no console se deu erro
    Serial.println("Erro ao ler DHT!");
    return;
  }

  Serial.printf("Temp: %.1f °C | Umid: %.1f %%\n", temperatura, umidade);  //mostra temperatura e umidade no console

  char tempStr[10], umidStr[10];
  dtostrf(temperatura, 4, 1, tempStr);  //converte o valor da temperatura do sensor que para string (ele vem float do sensor)
  dtostrf(umidade, 4, 1, umidStr);      //converte o valor da umidade do sensor que para string (ele vem float do sensor)
  client.publish(topic_temp, tempStr);  //publica a temperatura no tópico, lá no Broker Server
  client.publish(topic_umid, umidStr);  //publica a umidade no tópico, lá no Broker Server

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
//     while (!Serial.available())  ;
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
  anguloAtual = 90;
  delay(2000);
}



void fecharPortaAutomatica() {
  motor.write(0);
  anguloAtual = 0;
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



void alarmePorta() { 
  if (anguloAtual == 90) {
    

    // Se já se passaram mais de 5 segundos com a porta aberta
    if ((millis() - tempoPortaAberta) >= 4000) {
      tone(buzzer, 1000);
      Serial.println("Porta aberta por mais de 4 segundos! Feche-a.");
    }
  } else {
    // Porta fechada: reseta o tempo e desliga o alarme
    tempoPortaAberta = 0;
      noTone(buzzer);
  }
}



void alarmeTemperatura() {
  float temperatura = dht.readTemperature();

  if (temperatura >= 26.50) {
    somDoAlarme();
    Serial.println("Temperatura da casa alta, verificar.");
  } else {
    noTone(buzzer);
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

  //WiFi
  conectarWiFi();                            //conecta no wifi
  client.setServer(mqtt_server, mqtt_port);  //conecta no broker server
  client.setCallback(tratarMensagem);        //trata as mensagens recebidas do broker

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
  if (!client.connected()) reconectarMQTT();  //se não tem conexão com o broker, tenta reconectar
  client.loop();                              //mantém a conexão com o broker serve sempre aberta

  if (millis() - ultimaLeitura > 5000) {
    ultimaLeitura = millis();    //contador de milisegundos
    verificarTempEUmidade();  //Dados do sensor de umidade e temperatura
  }

   alarmeTemperatura();
   alarmePorta();
  /* destrancarPorta();
  abrirPortaAutomatica();
  fecharPortaAutomatica();
  trancarPorta();
  verificarTempEUmidade();*/
}