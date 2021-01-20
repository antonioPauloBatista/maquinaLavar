#include <Arduino.h>

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
  Serial.println(digitalRead(sensorPorta));
  Serial.println(digitalRead(sensorNivel));
  Serial.println(digitalRead(botaoLiga));

  Serial.println();
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
    time = millis() + (5 * 60 * 1000);
    while (time > millis())
    {
      switch (subState)
      {
      //3.0 - gira para frente 5 segundos
      case 0:
        subTime = millis() + (5 * 1000);
        while (subTime > millis())
        {
          liga(motorCestoHorario);
          desliga(motorCestoAntiHorario);
          subState = 1;
        }

        break;
      //3.1 - aguarda para frente 2 segundos
      case 1:
        subTime = millis() + (2 * 1000);
        while (subTime > millis())
        {
          desliga(motorCestoHorario);
        }
        subState = 2;
        break;
      //3.2 - gira para tras 5 segundos
      case 2:
        subTime = millis() + (5 * 1000);
        while (subTime > millis())
        {
          desliga(motorCestoHorario);
          liga(motorCestoAntiHorario);
        }
        //verifica se vem do passo de molho
        if (contState == 2)
        {
          state = 5;
          contState = 0;
        }
        else
        {
          state = 4;
        }
        break;
      }
    }

    break;
  //4 - molho
  case 4:
    time = millis() + (5 * 60 * 1000);
    while (time > millis())
    {
      desliga(motorCestoAntiHorario);
      desliga(motorCestoHorario);
    }
    contState++;

    state = 3;

    break;
  //5 - esvaziar
  case 5:
    //liga bomba e termo atuador,aguarda nivel e tempo para termoatuador abrir completamente
    time = millis() + (2 * 60 * 1000);
    liga(motorBombaETermoatuador);
    while (digitalRead(sensorNivel) && (time > millis()))
    {
    }

    state = 6;

    break;
  //6 - pre centrifugacao
  case 6:
    for (int i = 0; i < 3; i++)
    {
      //gira 10 segundos
      time = millis() + (10 * 1000);
      while (time > millis())
      {
        liga(motorCestoHorario);
      }
      //aguarda 2 segundo
      time = millis() + (5 * 1000);
      while (time > millis())
      {
        desliga(motorCestoHorario);
      }
    }
    desliga(motorCestoHorario);
    desliga(motorBombaETermoatuador);

    state = 7;

    break;
    //7 - Enchimento para amaciante
  case 7:
    //liga valvula amaciante por 1 minuto
    time = millis() + (1 * 60 * 1000);
    while (time > millis())
    {
      liga(valvulaAmaciante);
    }
    desliga(valvulaAmaciante);
    //liga valvula sabao ate chegar no nivel
    while (digitalRead(sensorNivel) == 0)
    {
      liga(valvulaSabao);
    }
    desliga(valvulaSabao);

    state = 8;

    break;
  //8 - lavagem amaciante
  case 8:

    subState = 0;
    // agira 5s para um lado 5 minutos para o outro durant 5 mim
    time = millis() + (5 * 60 * 1000);
    while (time > millis())
    {
      switch (subState)
      {
      //8.0 - gira para frente 5 segundos
      case 0:
        subTime = millis() + (5 * 1000);
        while (subTime > millis())
        {
          liga(motorCestoHorario);
          desliga(motorCestoAntiHorario);
          subState = 1;
        }

        break;
      //8.1 - aguarda para frente 2 segundos
      case 1:
        subTime = millis() + (2 * 1000);
        while (subTime > millis())
        {
          desliga(motorCestoHorario);
        }
        subState = 2;
        break;
      //8.2 - gira para tras 5 segundos
      case 2:
        subTime = millis() + (5 * 1000);
        while (subTime > millis())
        {
          desliga(motorCestoHorario);
          liga(motorCestoAntiHorario);
        }
        break;
      }
    }
    state = 9;
    break;
  // 9 - Esvaziamento para centrifugação
  case 9:
    //liga bomba e termo atuador,aguarda nivel e tempo para termoatuador abrir completamente
    time = millis() + (2 * 60 * 1000);
    liga(motorBombaETermoatuador);
    while (digitalRead(sensorNivel) && (time > millis()))
    {
    }

    state = 10;
    break;
  // 10 - pre centrifugacao da centrifugação
  case 10:

    for (int i = 0; i < 3; i++)
    {
      //gira 10 segundos
      time = millis() + (10 * 1000);
      while (time > millis())
      {
        liga(motorCestoHorario);
      }
      //aguarda 2 segundo
      time = millis() + (5 * 1000);
      while (time > millis())
      {
        desliga(motorCestoHorario);
      }
    }
    desliga(motorCestoHorario);
    //aguarda 5 segundo para evitar pico no motor
    time = millis() + (5 * 1000);
    while (time > millis())
    {
      desliga(motorCestoHorario);
    }

    state = 11;

    break;
  //11 - Centrifugação
  case 11:
    time = millis() + (10 * 60 * 1000);
    while (time > millis())
    {
      liga(motorCestoHorario);
    }
    desliga(motorCestoHorario);
    desliga(motorCestoAntiHorario);
    desliga(valvulaAmaciante);
    desliga(valvulaSabao);
    desliga(motorBombaETermoatuador);
    break;
  }
}
