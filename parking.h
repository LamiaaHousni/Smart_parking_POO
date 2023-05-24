#ifndef PARKING_H
#define PARKING_H

#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>
#include "rgb_lcd.h"

#define CAPACITY_MAX 4
#define B_IN  0
#define B_OUT 1
#define SOUND_VELOCITY 0.034


class Capteur
{
  const int trigPin;
  const int echoPin;
  long duration;
  float distanceCm;
public:
  Capteur(int tPin, int ePin);

  bool getStatus();
};

class Barriere
{
protected:
  int* nbrPlacesLibres;
  Capteur* capteur;
public:
  virtual ~Barriere() {}
  virtual bool isThereAvehicule() = 0;
  bool servirVehicule = false;
  Servo* myServo;
  virtual void tick() = 0;
};

class BarriereIn : public Barriere
{
public:
  BarriereIn(int* nbr);

  bool isThereAvehicule() override;
  void tick() override;
};

class BarriereOut : public Barriere
{
public:
  BarriereOut(int* nbr);

  bool isThereAvehicule() override;
  void tick() override;
};
//Forward declaration
class Parking;

class Afficheur
{
public:
  Parking* parking;
  rgb_lcd lcd;

  Afficheur(Parking* p);

  void update();
  void print(int nbrPlacesLibres, const char* msg1, const char* msg2, const char* msg3, const char* msg4);
  const char* truncateString(const char* str, size_t maxLength);
};

class Vehicules
{
};

class Voiture : public Vehicules
{
};

class Camion : public Vehicules
{
};

class Parking
{
private:
  Vehicules* vehicules[CAPACITY_MAX];

public:
  int nbrPlacesLibres;
  Barriere* barriere[2];
  Afficheur* afficheur;

  Parking();
  void tick();
};

#endif
