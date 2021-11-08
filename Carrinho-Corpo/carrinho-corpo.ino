#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define pinMotorEsq1 5
#define pinMotorEsq2 6
#define pinMotorDir1 9
#define pinMotorDir2 10

RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";

struct typeControleRF
{
   int joy1X = 512;
   int joy1Y = 512;
   boolean joy1Button = false;
};
typeControleRF controleRF;

int velocity = 255;
int potEsqY = 0;
int potDirX = 0;

int joystickYCenter = 512;
int joystickXCenter = 512;

bool ligarFarois = false;

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  // farois
  pinMode(2, OUTPUT);

  // ponteH PINS
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);

  Serial.begin(9600);

  Serial.println(" - Inicio - ");

  Serial.println(" Radio PA Level");

  radio.begin();
  radio.setPALevel( RF24_PA_LOW );     //RF24_PA_MIN  / RF24_PA_LOW / RF24_PA_HIGH / RF24_PA_MAX
  radio.setDataRate( RF24_250KBPS );   //RF24_250KBPS / RF24_1MBPS  / RF24_2MBPS
  Serial.println(" Radio Begin ");
  radio.openReadingPipe(0, address);
  Serial.println(" Radio Reading Open");
  radio.startListening();
  Serial.println(" Radio START Listening");

}

// the loop function runs over and over again forever
void loop() {
  potEsqY = analogRead(A1);
  potDirX = analogRead(A0);

  if(ligarFarois){
    digitalWrite(2, HIGH);
    delay(2000);
  }else{
    digitalWrite(2, LOW);
  }
  
  if (radio.available())
  {
    Serial.print(" Radio Received : ");
    char text[32] = "";
    int receiv = 0;
    //radio.read(&text, sizeof(text));
    radio.read(&controleRF, sizeof(typeControleRF));

    potEsqY = controleRF.joy1Y;
    potDirX = controleRF.joy1X;
    Serial.print(" joy1X: " + String(controleRF.joy1X));
    Serial.print(" joy1Y: " + String(controleRF.joy1Y));
    Serial.println(" joy1Button: " + String(controleRF.joy1Button));

    if(controleRF.joy1Button)
    {
      Serial.print(" -- BOTAO APERTADO! -- ");
      Serial.print(" DE:" + String(ligarFarois));
      Serial.println(" PARA:" + String(!ligarFarois));
      ligarFarois = !ligarFarois;
    }

    if(potEsqY >= (joystickYCenter-15) && potEsqY <= (joystickYCenter+15))
    {
      // check forward and backward
      if(potDirX >= (joystickXCenter-20) && potDirX <= (joystickXCenter+20))
      {
        // analogicos no centro -> Freia
        velocity = 0;
        motorStop();
      }else{
        if(potDirX < joystickXCenter){
          velocity = map(potDirX,510,0,0,255);
          motorLeft(velocity);
        }else{
          velocity = map(potDirX,510,1023,0,255);
          motorRight(velocity);
        }
      }
    }else{
      if(potEsqY < joystickYCenter){
        velocity = map(potEsqY,510,0,0,255);
        motorForward(velocity);
      }else{
        velocity = map(potEsqY,510,1023,0,255);
        motorBackward(velocity);
      }
    }

  }else{
    // nada no Radio, freia
    //Serial.println(" RADIO Received : NOTHING");
    //digitalWrite(LED_BUILTIN, LOW);
    //piscaLed();
    velocity = 0;
    motorStop();
  }
  
  delay(20);
}

void motorForward(int power) {
  Serial.println(" Carrinho FRENTE!");
  // motor esquerdo
  analogWrite(pinMotorEsq1, power);
  analogWrite(pinMotorEsq2, 0);

  // motor direito
  analogWrite(pinMotorDir1, power-75);
  analogWrite(pinMotorDir2, 0);
}
void motorBackward(int power) {
  Serial.println(" Carrinho TRAZ!");
  // motor esquerdo
  analogWrite(pinMotorEsq1, 0);
  analogWrite(pinMotorEsq2, power);

  // motor direito
  analogWrite(pinMotorDir1, 0);
  analogWrite(pinMotorDir2, power-75);
}
void motorLeft(int power) {
  Serial.println(" Carrinho ESQUERDA!");
  // motor esquerdo
  analogWrite(pinMotorEsq1, 0);
  analogWrite(pinMotorEsq2, 0);

  // motor direito
  analogWrite(pinMotorDir1, (power-30));
  analogWrite(pinMotorDir2, 0);
}
void motorRight(int power) {
  Serial.println(" Carrinho DIREITA!");
  // motor esquerdo
  analogWrite(pinMotorEsq1, power);
  analogWrite(pinMotorEsq2, 0);

  // motor direito
  analogWrite(pinMotorDir1, 0);
  analogWrite(pinMotorDir2, 0);
}

void motorStop() {
  //Serial.println(" Carrinho PARADO!");
  // motor esquerdo
  analogWrite(pinMotorEsq1, 0);
  analogWrite(pinMotorEsq2, 0);

  // motor direito
  analogWrite(pinMotorDir1, 0);
  analogWrite(pinMotorDir2, 0);
}

void piscaLed(){
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(3, HIGH);
  delay(200);
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(3, LOW);
  delay(200);
}
