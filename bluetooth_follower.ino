#include <Servo.h>        //biblioteca para controle dos servos
#include <SoftwareSerial.h>
SoftwareSerial bt(11, 12);
#define Ldirpin 10     //pino digital para controle da direção do motor direito  - MOTOR 1
#define Lspeedpin 9   //pino analógico para controle da velocidade do motor direito -  MOTOR 1
#define Rdirpin 8   //pino digital para controle da direção do motor direito- MOTOR 2
#define Rspeedpin 7  //pino analógico para controle da velocidade do motor direito -  MOTOR 2 
#define sensorleftPin 2   //pino digital para leitura do sensor de linha esquerdo
#define sensorrightPin 4  //pino digital para leitura do sensor de linha direito
#define Servo1pin 3       //pino analógico para controle do primeiro servomotor
#define Servo2pin 5       //pino analógico para controle do segundo servomotor
                          //todos defines serão alterados para os pinos correspondentes da PACA

int svr=0;                //declara variável de leitura do sensor de linha direito
int svl=0;                //declara variável de leitura do sensor de linha esquerdo
int max_speedL = 200;      //máximo valor analógico de velocidade do motor esquerdo
int max_speedR = 150;      //máximo valor analógico de velocidade do motor direito
int min_speed = 0;        //mínimo valor analógico de velocidade
int speed = 0;              //declara variável de controle da velocidade (escala de 0 a 255) dos motores CC
int speedpercent;           //declara variável auxiliar 
int servo_pos = 0;        //declara variável d e controle da posição angular dos servomotores
char data;                 //caracter para leitura de dados via comunicação serial (no tinkercad, no carrinho em si será via bluetooth)
Servo servo1;               //instancia classe Servo
Servo servo2;               //dois servomotores - 1 para trava esquerda outro para trava direita

int motorR_direction = 1;
int motorL_direction = 1;
int Rmotor_speed = 0;
int Lmotor_speed = 0;

void setup(){
  Serial.begin(9600);         //comunicação serial é iniciada com baud rate 9600
  bt.begin(9600);

  pinMode(Rdirpin,OUTPUT); 
  pinMode(Rspeedpin,OUTPUT);      
  pinMode(Lspeedpin, OUTPUT);   //seta pinos associados aos motores CC e servos como saída  
  pinMode(Ldirpin, OUTPUT);
  pinMode(Servo1pin, OUTPUT);
  pinMode(Servo2pin, OUTPUT);

  pinMode(sensorleftPin, INPUT);  //seta pinos dos sensores como entrada
  pinMode(sensorrightPin, INPUT);

  servo1.attach(Servo1pin);       //associa objetos dos servomotores aos respectivos pinos
  servo2.attach(Servo2pin);
  servo1.write(90);   //inicia os servos na posição 90º
  servo2.write(90);



}



void rotate_lever(int desired_pos){             //função de controle do servomotor
  if(desired_pos== 180){                                                   
      servo_pos = (servo_pos<desired_pos?servo_pos+10:desired_pos);       //incrementa ângulo caso seja menor que 180, caso contrário mantém em 180
  }
  else if(desired_pos== 0){                                                 
      servo_pos = (servo_pos>desired_pos?servo_pos-10:desired_pos);   //decrementa ângulo caso seja maior que 0, caso contrário mantém em 90
  }
  servo1.write(servo_pos);                                                  //envia os ângulos desejados
  servo2.write(180-servo_pos);                                                  //para os servomotores
} 

void bluetooth_control(){
    data = bt.read();                                 //lê caracteres enviados via serial (para testes no tinkercad, no carrinho físico será utilizado SpftwareSerial e pinos RX, TX para comunicação Bluetooth)
    if (data == 'a'){                                     //botão superior do joystick esquerdo é pressionado 
      motorR_direction = 1;
      motorL_direction = 0;                             //acelera com a velocidade setada acima
      Rmotor_speed = 255;
      Lmotor_speed= 255;
        }
    else if (data == 'c'){                                //botão inferior do joystick esquerdo é pressionado
      motorR_direction = 0;
      motorL_direction = 1;                           //acelera para trás 
      Rmotor_speed = 255;
      Lmotor_speed = 255;
    }
    else if (data == 'b'){   
      motorR_direction = 0;                             //botão da direita do joystick esquerdo é pressionado
      motorL_direction = 0;
      Rmotor_speed = 255;                                //vira para direita sem alterar velocidadee
      Lmotor_speed = 255;   
    }
    else if (data == 'd'){                                //botão da esquerda do joystick esquerdo é pressionado
      motorR_direction = 1;                             //botão da direita do joystick esquerdo é pressionado
      motorL_direction = 1;
      Rmotor_speed = 255;                                //vira para esquerda
      Lmotor_speed = 255;
    }
    else if(data == 'A'){                                   //botão 'x' do joystick direito é pressionado
        rotate_lever(180);                                 //desce ambas as travas para a horizontal
    }
    else if(data =='C'){                                   //botão triângulo do joytick direito é pressionado
        rotate_lever(0);                                  //volta ambas as travas para a vertical
    }
    else{
      Rmotor_speed = min_speed;                                //velocidade zero  
      Lmotor_speed = min_speed;        
    }
  speedpercent = (int)(100*speed)/255;                  //velocidade em percentagem
}

void followLine(){
  svr=digitalRead(sensorrightPin);    //leitura do sensor de linha direito 
  svl=digitalRead(sensorleftPin);     //leitura do sensor de linh esquerdo
  Serial.print(svr);
  Serial.print(svl);
  //Serial.print("Autônomo.... ");
  if(svl==LOW && svr==LOW)    //ambos sensores não detectam linha
  {
    motorR_direction = 1;
    motorL_direction = 0;        //movimento para frente
    Rmotor_speed = max_speedR;
    Lmotor_speed = max_speedL;
  }

  else if(svl==LOW   && svr==HIGH) //apenas sensor esquerdo detecta a linha 
  {
    motorR_direction = 1;                             //botão da direita do joystick esquerdo é pressionado
    motorL_direction = 1;
    Rmotor_speed = max_speedR;  
    Lmotor_speed =max_speedL;
  }
 
  else if(svl==HIGH && svr==LOW)     //apenas sensor da direta detecta a linha 
  { 
    motorR_direction = 0;                             //botão da direita do joystick esquerdo é pressionado
    motorL_direction = 0;
    Rmotor_speed = max_speedR;          //rotaçaão para direta
    Lmotor_speed = max_speedL;
  }
  
  else if(svl==HIGH && svr==HIGH)      //ambos sensores detecta a linha
  {
    Rmotor_speed = min_speed;
    Lmotor_speed = min_speed;
  }
}


void loop() {
  static unsigned long startTime = millis();
  //if (millis() - startTime < 30000) {    //primeiros 30 segundos
    //Serial.print("Autônomo.... ");
    //followLine();                        //funcionamento autônomo - seguidor de linha
    //digitalWrite(Rdirpin, motorR_direction);      
    //digitalWrite(Ldirpin, motorL_direction);
    //analogWrite(Rspeedpin, Rmotor_speed);
    //analogWrite(Lspeedpin, Lmotor_speed);
  //} 
  //else{ 
  if (bt.available()) {                   

    Serial.print("bluetooth.... ");
    bluetooth_control();                //controle por bluetooth - seta direção de cada motor e velocidade
    digitalWrite(Rdirpin, motorR_direction);      
    digitalWrite(Ldirpin, motorL_direction);
    analogWrite(Rspeedpin, Rmotor_speed);
    analogWrite(Lspeedpin, Lmotor_speed);
    //delay(1000);
    Serial.print("velocidade ");        
    Serial.println(speed);
    Serial.print("velocidade %: ");           //imprime no monitor serial(utilizado para testes apenas)
    Serial.println(speedpercent);
    Serial.print("data: ");
    Serial.println(data);
  }
  //}
}
