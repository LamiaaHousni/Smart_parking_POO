
#include "parking.h"

//Implémentation classe Capteur

Capteur::Capteur(int tPin, int ePin) : trigPin(tPin), echoPin(ePin)
{
  pinMode(tPin, OUTPUT);
  pinMode(ePin, INPUT);
}

bool Capteur::getStatus()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);

  // Calcul de la distance
  distanceCm = duration * SOUND_VELOCITY / 2;

  if (distanceCm <= 10.0) {
    return true;
  }
  else {
    return false;
  }
}

//Implémentation classe BarriereIN

BarriereIn::BarriereIn(int* nbr)
{
  capteur = new Capteur(D6, D5);
  myServo = new Servo;
  myServo->attach(D3);
  nbrPlacesLibres = nbr;
}

bool BarriereIn::isThereAvehicule()
{
  return capteur->getStatus();
}

void BarriereIn::tick(void){
  isThereAvehicule();
  if(isThereAvehicule() == true){
    if((*nbrPlacesLibres) > 0){
      if(servirVehicule == false){
        servirVehicule = true;
        myServo->write(180);
        (*nbrPlacesLibres)--; 
      }
    } 
  } else {
    servirVehicule = false;
    myServo->write(0);
  }
}

//Implémentation classe BarriereOUT
BarriereOut::BarriereOut(int* nbr)
{
  capteur = new Capteur(D4, D8);
  myServo = new Servo;
  myServo->attach(D7);
  nbrPlacesLibres = nbr;
}

bool BarriereOut::isThereAvehicule()
{
  return capteur->getStatus();
}

void BarriereOut::tick()
{
  isThereAvehicule();
  if (isThereAvehicule() == true) {
    if ((*nbrPlacesLibres) < CAPACITY_MAX) {
      if (servirVehicule == false) {
        servirVehicule = true;
        myServo->write(180);
        (*nbrPlacesLibres)++;
      }
    }
  }
  else {
    servirVehicule = false;
    myServo->write(0);
  }
}

//Implémentation classe Afficheur
Afficheur::Afficheur(Parking* p)
{
  parking = p;
}

void Afficheur::update()
{
  if (parking->nbrPlacesLibres > 0) {
    print(parking->nbrPlacesLibres, "", "", "     Welcome", "");
  }
  else {
    if (parking->barriere[0]->servirVehicule == false) {
      print(0, "", "Merci de", "revenir", "Come back later");
    }
    else {
      print(parking->nbrPlacesLibres, "", "", "     Welcome", "");
    }
  }
}

void Afficheur::print(int nbrPlacesLibres, const char* msg1, const char* msg2, const char* msg3, const char* msg4)
{
  char buffer[17]; // Longueur maximale d'une ligne sur un écran 16x2 
           

  lcd.clear();   // Efface l'écran

  // Affiche le nombre de places libres
  snprintf(buffer, sizeof(buffer), "    %d places", nbrPlacesLibres);
  lcd.setCursor(0, 0);
  lcd.print(buffer);
// Affiche les messages suivants sur les lignes suivantes, en tronquant si nécessaire
  lcd.setCursor(0, 1);
  lcd.print(truncateString(msg1, 16));
  lcd.setCursor(0, 2);
  lcd.print(truncateString(msg2, 16));
  lcd.setCursor(0, 3);
  lcd.print(truncateString(msg3, 16));
  lcd.setCursor(0, 4);
  lcd.print(truncateString(msg4, 16));
}
// Fonction utilitaire pour tronquer une chaîne de caractères si elle dépasse une certaine longueur
const char* Afficheur::truncateString(const char* str, size_t maxLength)
{
  static char truncated[17];// Longueur maximale d'une ligne sur un écran 16x2 

  strncpy(truncated, str, maxLength);
  truncated[maxLength] = '\0';

  return truncated;
}

//Implémentation classe Parking
Parking::Parking()
{
  //Utilisation d'exceptions
  if (CAPACITY_MAX<0) throw 1;
  nbrPlacesLibres = CAPACITY_MAX;
  barriere[B_IN] = new BarriereIn(&nbrPlacesLibres);
  barriere[B_OUT] = new BarriereOut(&nbrPlacesLibres);
  afficheur = new Afficheur(this);
}

void Parking::tick()
{
  barriere[B_IN]->tick();
  barriere[B_OUT]->tick();

  if (barriere[B_IN]->servirVehicule == true || barriere[B_OUT]->servirVehicule == true) {
    afficheur->print(nbrPlacesLibres, "", "", "     Welcome", "");
  }
  else {
    if (nbrPlacesLibres > 0) {
      afficheur->print(nbrPlacesLibres, "", "", "     Welcome", "");
    }
    else {
      afficheur->print(0, "", "", "", "Come back later");
      afficheur->update();
    }
  }

  afficheur->update();
}
//Redéfinition d'opérateur
// class Servo {
//    Servo & operator ++(int){
//       myServo->write(180);
//    return(*this);}
//    Servo & operator --(int){
//       myServo->write(0);
//    return(*this);}
//}

Parking* parking;
Afficheur* afficheur;

void setup()
{
  lcd.begin(16, 2);
  parking = new Parking;
}

void loop()
{
  try
  {
    parking->tick();
  }
  catch (int erreur)
  {
    if (erreur == 1) {
      afficheur->print("Erreur capacité max");
    }
    else {
      afficheur->print("Exception inconnue");
    }
  }
}