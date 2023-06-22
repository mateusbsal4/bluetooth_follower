#include <Servo.h>        //biblioteca para controle dos servos
#include <SoftwareSerial.h>
#include <SoftPWM.h>
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
int max_speedL = 150;      //máximo valor analógico de velocidade do motor esquerdo
int max_speedR = 150;      //máximo valor analógico de velocidade do motor direito
int min_speed = 0;        //mínimo valor analógico de velocidade
int motorR_direction = 1;   //direção do motor direito   
int motorL_direction = 1;   //direção do motor esquerdo
int Rmotor_speed = 0;     //variavel que guarda a velocidade do motor direito   
int Lmotor_speed = 0;     //variavel que guarda a velocidade do motor esquerdo

int servo_pos = 0;        //declara variável d e controle da posição angular dos servomotores
Servo servo1;               //instancia classe Servo
Servo servo2;               //dois servomotores - 1 para trava esquerda outro para trava direita



char data;                 //caracter para leitura de dados via bluetooth
int bluetooth_setup = 1;




void setup(){
  Serial.begin(9600);         //comunicação serial é iniciada com baud rate 9600
  bt.begin(9600);

  pinMode(Rdirpin,OUTPUT); 
  pinMode(Rspeedpin,OUTPUT);      
  pinMode(Lspeedpin, OUTPUT);   //seta pinos associados aos motores CC como saída  
  pinMode(Ldirpin, OUTPUT);

  pinMode(sensorleftPin, INPUT);  //seta pinos dos sensores como entrada
  pinMode(sensorrightPin, INPUT);
  SoftPWMBegin();
  SoftPWMSet(7, 0);




}

void rotate_lever(int desired_pos){             //função de controle do servomotor
  if(desired_pos== 90){                                                   
      servo_pos = (servo_pos<desired_pos?servo_pos+10:desired_pos);       //incrementa ângulo caso seja menor que 180, caso contrário mantém em 180
  }
  else if(desired_pos== 0){                                                 
      servo_pos = (servo_pos>desired_pos?servo_pos-10:desired_pos);   //decrementa ângulo caso seja maior que 0, caso contrário mantém em 90
  }
  servo1.write(servo_pos);                                                  //envia os ângulos desejados
  servo2.write(180-servo_pos);                                                  //para os servomotores
} 

void bluetooth_control(){
    data = bt.read();                                 //lê caracteres enviados via bluetooth
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
    else if(data == 'A'){                                   //botão de cima do joystick direito é pressionado
        rotate_lever(90);                                 //desce ambas as travas para a horizontal
    }
    else if(data =='C'){                                   //botão de baixo do joytick direito é pressionado
        rotate_lever(0);                                  //volta ambas as travas para a vertical
    }
    else if(data =='B'){                                   //botão de baixo do joytick direito é pressionado
      Rmotor_speed = min_speed;                                //velocidade zero  - para o carrinho
      Lmotor_speed = min_speed; 
    }
    else{                                                 //se ultimo caracter recebido for qualquer outro diferente dos acima
      Rmotor_speed = min_speed;                                //velocidade zero  - para o carrinho
      Lmotor_speed = min_speed;        
    }
}

void followLine(){
  svr=digitalRead(sensorrightPin);    //leitura do sensor de linha direito 
  svl=digitalRead(sensorleftPin);     //leitura do sensor de linh esquerdo
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

  }
}


void loop() {
  static unsigned long startTime = millis();
  if (millis() - startTime < 30000) {    //primeiros 30 segundos
    Serial.print("Autônomo.... ");
    followLine();                        //funcionamento autônomo - seguidor de linha
  } 
  else{ 
     if(bluetooth_setup){
        Rmotor_speed = 0;
        Lmotor_speed = 0;     //para o carrinho na transição entre autonomo e bluetooth
        //SoftPWMSet(7, 0);  
        //SoftPWMSet(9, 0);  
        pinMode(Servo1pin, OUTPUT);
        pinMode(Servo2pin, OUTPUT);
        servo1.attach(Servo1pin);       //associa objetos dos servomotores aos respectivos pinos
        servo2.attach(Servo2pin);
        servo1.write(90);   //inicia os servos na posição 90º
        servo2.write(90);
        bluetooth_setup = 0;    //entra nesse if uma vez só - só para o setup do bluetooth
      }
    else if (bt.available()) {                   
      Serial.print("bluetooth.... ");
      bluetooth_control();                //controle por bluetooth - seta direção de cada motor e velocidade
    }
  }
  digitalWrite(Rdirpin, motorR_direction);      
  digitalWrite(Ldirpin, motorL_direction);
  SoftPWMSet(7, Rmotor_speed);   //seta as direções e velocidades (determinadas por followLine ou bluetooth_control)
  SoftPWMSet(9, Lmotor_speed);   //dos motores
  delay(50);     //se necessario adicionar delay (é em ms)
}