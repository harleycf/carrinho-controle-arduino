#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";

bool sent = false;
bool xNoCentro = true;
bool yNoCentro = true;

int potEsqY = 0;
int potDirX = 0;

int joystickYCenter = 512;
int joystickXCenter = 512;

unsigned long delay1 = 0;
boolean enviarBotao = false;
boolean controleBotao = false;
int tempo = 0;

struct typeControleRF
{
   int joy1X = 512;
   int joy1Y = 512;
   boolean joy1Button = false;
};
typeControleRF controleRF;

void setup()
{
  Serial.begin(9600);
  Serial.println(" - Inicio - ");
  delay(200);
  
  pinMode(LED_BUILTIN, OUTPUT);

  // Farois
  pinMode(2, OUTPUT);

  // JoysTick Analog Pins
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT_PULLUP);

  // First readings for calibration
  joystickYCenter = analogRead(A1);
  joystickXCenter = analogRead(A0);

  radio.begin();
  piscaLed();
  //Serial.println(" Radio Begin ");  
  radio.setPALevel( RF24_PA_MAX);     //RF24_PA_MIN  / RF24_PA_LOW / RF24_PA_HIGH / RF24_PA_MAX
  radio.setDataRate( RF24_250KBPS );   //RF24_250KBPS / RF24_1MBPS  / RF24_2MBPS
  radio.stopListening();
  radio.openWritingPipe(address);
  piscaLed();
  
  digitalWrite(LED_BUILTIN, HIGH);
  delay(200);
}
void loop()
{
  delay(20);
  potEsqY = analogRead(A1);
  potDirX = analogRead(A0);

  if(digitalRead(A2) == LOW){
    Serial.print("BOTAO PRESSIONADO");
    if(!enviarBotao)
    {
      enviarBotao = true;
      controleBotao = true;
      delay1 = millis(); 
    }else{
      controleBotao = false;
      tempo = (millis() - delay1);
      Serial.print(" - tempo: " + String(tempo));
      if(tempo > 500) {
        enviarBotao = false;        
        delay1 = millis(); 
      }
    }
    Serial.print(" - env bot = " + String(enviarBotao));
    Serial.println(" - con bot = " + String(controleBotao));
  }else{
    enviarBotao = false;
    controleBotao = false;
  }

  controleRF.joy1X=potDirX;
  controleRF.joy1Y=potEsqY;
  controleRF.joy1Button=controleBotao;

  yNoCentro = false;
  xNoCentro = false;

  //Serial.print(" joy1X: " + String(controleRF.joy1X));
  //Serial.print(" joy1Y: " + String(controleRF.joy1Y));
  //Serial.println(" joy1Button: " + String(controleRF.joy1Button));

  if(potEsqY >= (joystickYCenter-15) && potEsqY <= (joystickYCenter+15))
  {
    yNoCentro = true;
  }
  // check forward and backward
  if(potDirX >= (joystickXCenter-20) && potDirX <= (joystickXCenter+20))
  {
    xNoCentro = true;
  }

  if(!xNoCentro || !yNoCentro || controleBotao)
  {
    //Serial.println(" VAI Enviar ");
    // envia sinal de radio
    long tmp = millis();
    while(!radio.write(&controleRF, sizeof(typeControleRF))){
      if((millis() - tmp) > 2000){
        Serial.println(" Radio DOWN FAILED");
        piscaLed();
        break;
      }
    }
  }
}

void piscaLed(){
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(2, HIGH);
  delay(200);
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(2, LOW);
  delay(200);
}
