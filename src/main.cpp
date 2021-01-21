#include <Arduino.h>

const int DEBUG = 1;

const int botaoLiga = 10;
const int sensorPorta = 11;
const int sensorNivel = 12;
const int ledIndicadorNivelOk = 13;
const int motorCestoHorario = 2;
const int motorCestoAntiHorario = 3;
const int motorBombaETermoatuador = 4;
const int valvulaSabao = 5;
const int valvulaAmaciante = 6;
unsigned long time;
unsigned long subTime;
int state = 0;
int subState = 0;
int contState = 0;

void liga(int pin)
{
  digitalWrite(pin, 0);
}

void desliga(int pin)
{
  digitalWrite(pin, 1);
}

void estabilizar(){
   subTime = millis() + (2 * 1000UL);
    while (subTime > millis())
        {}
}
void printStatus()
{
  Serial.print("state: ");
  Serial.print(state);
  Serial.print(" ");
  Serial.print("subState: ");
  Serial.print(subState);
  Serial.print(" time: ");
  Serial.print(time);
  Serial.print(" ");
  Serial.print("milis: ");
  Serial.print(millis());
  Serial.print(" ");
  Serial.println("");
}

// the setup function runs once when you press reset or power the board
void setup()
{
  //configira entradas
  pinMode(botaoLiga, INPUT);
  pinMode(sensorPorta, INPUT);
  pinMode(sensorNivel, INPUT);
  pinMode(motorCestoHorario, OUTPUT);
  pinMode(motorCestoAntiHorario, OUTPUT);
  pinMode(motorBombaETermoatuador, OUTPUT);
  pinMode(valvulaSabao, OUTPUT);
  pinMode(valvulaAmaciante, OUTPUT);
  pinMode(ledIndicadorNivelOk, OUTPUT);
  Serial.begin(9600);

  desliga(motorCestoHorario);
  desliga(motorCestoAntiHorario);
  desliga(valvulaAmaciante);
  desliga(valvulaSabao);
  desliga(motorBombaETermoatuador);
}

// the loop function runs over and over again forever
void loop()
{
  if (DEBUG)
  {
    printStatus();
  }

  switch (state)
  {

  //0 - verifica se botao liga pressionado
  case 0:

    //verifica se botao acionado
    if (digitalRead(sensorPorta))
    {
      state = 1;
    }

    break;
  //1 - verifica se porta fechada
  case 1:

    if (digitalRead(botaoLiga))
    {
      liga(valvulaSabao);
      state = 2;
    }
    break;

  //2 - verifica se nivel de agua foi atingido
  case 2:
    if (digitalRead(sensorNivel))
    {
      desliga(valvulaSabao);
      state = 3;
      contState = 0;
      subState = 0;
    }
    break;
  //3 -inicia processo de lavagem
  case 3:
    // agira 5s para um lado 5 minutos para o outro durant 5 mim
    time = millis() + (5 * 60 * 1000UL);
    while (time > millis())
    {

      if (DEBUG)
      {
        printStatus();
      }

      switch (subState)
      {
      //3.0 - gira para frente 3 segundos
      case 0:
        subTime = millis() + (2 * 1000UL);
        while (subTime > millis())
        {
          liga(motorCestoHorario);
          desliga(motorCestoAntiHorario);
        }
        subState = 1;
        break;
      //3.1 - aguarda para frente 2 segundos
      case 1:
        subTime = millis() + (2 * 1000UL);
        while (subTime > millis())
        {
          desliga(motorCestoHorario);
          desliga(motorCestoAntiHorario);
        }
        estabilizar();
        subState = 2;
        break;
      //3.2 - gira para tras 3 segundos
      case 2:
        subTime = millis() + (2 * 1000UL);
        while (subTime > millis())
        {
          desliga(motorCestoHorario);
          liga(motorCestoAntiHorario);
        }
        subState = 3;
        break;
        //3.3 - aguarda para frente 2 segundos
      case 3:
        subTime = millis() + (2 * 1000UL);
        while (subTime > millis())
        {
          desliga(motorCestoHorario);
          desliga(motorCestoAntiHorario);
        }
        subState = 0;
        break;
      }
    }
    if (contState == 2)
    {
      estabilizar();
      state = 5;
      contState = 0;
    }
    else
    {
      estabilizar();
      state = 4;
    }

    break;
  //4 - molho
  case 4:
    time = millis() + (5 * 60 * 1000UL);
    while (time > millis())
    {
      if (DEBUG)
      {
        printStatus();
      }

      desliga(motorCestoAntiHorario);
      desliga(motorCestoHorario);
    }
    contState++;
    estabilizar();
    state = 3;
    subState = 0;
    break;
  //5 - esvaziar
  case 5:
    //liga bomba e termo atuador,aguarda nivel e tempo para termoatuador abrir completamente
    liga(motorBombaETermoatuador);
    while (digitalRead(sensorNivel))
    {
      if (DEBUG)
      {
        printStatus();
      }
    }

    time = millis() + (4 * 60 * 1000UL);
    while (time > millis())
    {
      if (DEBUG)
      {
        printStatus();
      }
    }
    estabilizar();
    state = 6;

    break;
  //6 - pre centrifugacao
  case 6:
    for (int i = 0; i < 3; i++)
    {
      if (DEBUG)
      {
        printStatus();
      }
      //gira 10 segundos
      time = millis() + (10 * 1000UL);
      while (time > millis())
      {
        liga(motorCestoHorario);
      }
      //aguarda 5 segundo
      time = millis() + (5 * 1000UL);
      while (time > millis())
      {
        desliga(motorCestoHorario);
      }
    }
    desliga(motorCestoHorario);
    desliga(motorBombaETermoatuador);
    estabilizar();
    state = 7;

    break;
    //7 - Enchimento para amaciante
  case 7:
    //liga valvula amaciante por 1 minuto
    time = millis() + (1 * 60 * 1000UL);
    while (time > millis())
    {
      liga(valvulaAmaciante);
    }
    desliga(valvulaAmaciante);
    //liga valvula sabao ate chegar no nivel
    while (digitalRead(sensorNivel) == 0)
    {
      if (DEBUG)
      {
        printStatus();
      }
      liga(valvulaSabao);
    }
    desliga(valvulaSabao);
    estabilizar();
    state = 8;

    break;
  //8 - lavagem amaciante
  case 8:

    subState = 0;
    // agira 5s para um lado 5 minutos para o outro durant 5 mim
    time = millis() + (5 * 60 * 1000UL);
    while (time > millis())
    {

      if (DEBUG)
      {
        printStatus();
      }
      switch (subState)
      {
      //8.0 - gira para frente 3 segundos
      case 0:
        subTime = millis() + (2 * 1000UL);
        while (subTime > millis())
        {
          liga(motorCestoHorario);
          desliga(motorCestoAntiHorario);
        }
        subState = 1;
        break;
      //8.1 - aguarda para frente 2 segundos
      case 1:
        subTime = millis() + (2 * 1000UL);
        while (subTime > millis())
        {
          desliga(motorCestoHorario);
          desliga(motorCestoAntiHorario);
        }
        subState = 2;
        break;
      //8.2 - gira para tras 3 segundos
      case 2:
        subTime = millis() + (2 * 1000UL);
        while (subTime > millis())
        {
          desliga(motorCestoHorario);
          liga(motorCestoAntiHorario);
        }
        subState = 3;
        break;
        //8.3 - aguarda para frente 2 segundos
      case 3:
        subTime = millis() + (2 * 1000UL);
        while (subTime > millis())
        {
          desliga(motorCestoHorario);
          desliga(motorCestoAntiHorario);
        }
        subState = 0;
        break;
      }
    }
    estabilizar();
    state = 9;
    break;
  // 9 - Esvaziamento para centrifugação
  case 9:
    //liga bomba e termo atuador,aguarda nivel e tempo para termoatuador abrir completamente
    liga(motorBombaETermoatuador);
    while (digitalRead(sensorNivel))
    {

      if (DEBUG)
      {
        printStatus();
      }
    }

   time = millis() + (2 * 60 * 1000UL);

    while (time > millis())
    {

      if (DEBUG)
      {
        printStatus();
      }
    }
    estabilizar();
    state = 10;
    break;
  // 10 - pre centrifugacao da centrifugação
  case 10:

    for (int i = 0; i < 3; i++)
    {
      if (DEBUG)
      {
        printStatus();
      }
      //gira 10 segundos
      time = millis() + (10 * 1000UL);
      while (time > millis())
      {
        liga(motorCestoHorario);
      }
      //aguarda 2 segundo
      time = millis() + (10 * 1000UL);
      while (time > millis())
      {
        desliga(motorCestoHorario);
      }
    }
    desliga(motorCestoHorario);
    //aguarda 5 segundo para evitar pico no motor
    time = millis() + (5 * 1000UL);
    while (time > millis())
    {
      desliga(motorCestoHorario);
    }
    estabilizar();
    state = 11;

    break;
  //11 - Centrifugação
  case 11:
    time = millis() + (10 * 60 * 1000UL);
    while (time > millis())
    {

      if (DEBUG)
      {
        printStatus();
      }

      liga(motorCestoHorario);
    }
    desliga(motorCestoHorario);
    desliga(motorCestoAntiHorario);
    desliga(valvulaAmaciante);
    desliga(valvulaSabao);
    desliga(motorBombaETermoatuador);
    estabilizar();
    state = 12;

    break;
      case 12:

    //verifica se botao acionado
    if (digitalRead(sensorPorta) == 0)
    {
      state = 13;
    }

    break;

        //1 - verifica se porta fechada
  case 13:

    if (digitalRead(botaoLiga) == 0)
    {
      state = 0;
    }
    break;
  }

}
