/*
 * Version 2 de l'horloge Arduino, On ajoute ici l'option d'activer une alarme à une heure définie
 * Utilise la bibliothèque Time
 */

#include "SevSeg.h"
#include <Time.h>
#include <TimeLib.h>

SevSeg sevseg; //Initialise l'objet sevseg, qui tire ses attributs de la bibliothèque SevSeg.h
const int bouton = 14; //initialise la pin sur laquelle le bouton est branché, ici A0
const int buzzer = 15; //Intialise la pin sur laquelle le buzzer est branché, ici A1

//bouton pressé ou non ?    // Fonctions des boutons :
bool presseJaune = false;   // pause, confirmer
bool presseBlanc = false;   // décrémentation
bool presseBleu = false;    // incrémentation
bool presseRouge = false;   // stop, mode

//heure, minute, seconde, jour, mois, année
int hr = 0, minut = 0, sec = 0, d = 1, mnth = 1, yr = 2017;
// et pour l'alarme
int hr_alarm = 0, minut_alarm = 0;
// alarme active ?
bool alarm = false;

void setup()
{

  byte numDigits = 4;

  byte digitPins[] = {2, 3, 4, 5};

  byte segmentPins[] = {6, 7, 8, 9, 10, 11, 12, 13};

  bool resistorsOnSegments = false;

  sevseg.begin(COMMON_ANODE, numDigits, digitPins, segmentPins, resistorsOnSegments);

  sevseg.setBrightness(90);

  pinMode(bouton, INPUT);
  pinMode(buzzer, OUTPUT);

}

void loop()
{
  short choix = 1; // Choix de l'application //
  int analog, maximum = 0, num = 0;          // 1) set de l'horloge
  int minut = 0, sec = 0;                    // 2) affichage de l'horloge
                                             // 3) set d'une alarme
                                             // 4) chrono
  float tension;                             // 5) timer
  sevseg.refreshDisplay(); //Doit être appelé régulièrement et ne pas être gêné par des delay() pour faire fonctionner l'afficheur
  presseJaune = false;
  presseBlanc = false;
  presseBleu = false;
  presseRouge = false;

  while (!presseJaune) //Choix d'une application
  {
    analog = analogRead(bouton);
    tension = (float)analog * 5/1023;

    if (tension < 4.2)
    {
      if (tension < 2.5)
      {
        if (tension < 1.6 )
        {
          if (tension < 0.5)
          {
            //Bouton jaune => Confirme
            presseJaune = true;
            digitalWrite(buzzer, HIGH);
            wait(100);
            digitalWrite(buzzer, LOW);
            wait(100); //Nécessaire pour avoir le temps de lâcher le bouton
          }
          else
          {
            //Bouton blanc => Décrémente
            choix--;
            if (choix <= 0)
              choix = 5;
            digitalWrite(buzzer, HIGH);
            wait(100);
            digitalWrite(buzzer, LOW);
            wait(100); //Nécessaire pour avoir le temps de lâcher le bouton
          }
        }
        else
        {
          //Bouton bleu => Incrémente
          choix++;
          if (choix >= 6)
            choix = 1;
          digitalWrite(buzzer, HIGH);
          wait(100);
          digitalWrite(buzzer, LOW);
          wait(100); //Nécessaire pour avoir le temps de lâcher le bouton
        }
      }
    }

    switch (choix)
    {
      case 1 : // set horloge
        sevseg.setChars("SET");
        break;

      case 2 : // affichage horloge
        sevseg.setChars("CLOC");
        break;

      case 3 : // set alarme
        sevseg.setChars("ALAR");
        break;

      case 4 : // chrono
        sevseg.setChars("CHRO");
        break;

      case 5 : // timer
        sevseg.setChars("TIME");
        break;

    }
    sevseg.refreshDisplay();
  }
  presseJaune = false;

  switch (choix)
  {
    case 1 : // set horloge
      setClock();
      break;

    case 2 : // affichage horloge
      myClock();
      break;

    case 3 : // set alarme
      setAlarm();
      break;

    case 4 : // chrono
      chrono(1000);
      break;

    case 5 : // timer
    minut = 0;
    sec = 0;
    while (!presseJaune)
    {
      sevseg.refreshDisplay();

      analog = analogRead(bouton);
      tension = (float)analog * 5/1023;

      minut = maximum/60;
      sec = maximum%60;
      num = minut*100 + sec;
      /*
      num = (maximum/60) * 100 + (maximum%60);
      */

      if(maximum >= 60)
      {
          sevseg.setNumber(num,2);
      }
      else
        sevseg.setNumber(num,-1);

      if (tension < 4.2)
      {
        if (tension < 2.5)
        {
          if (tension < 1.6 )
          {
            if (tension < 0.5)
            {
              //Bouton jaune
              presseJaune = true;
              digitalWrite(buzzer, HIGH);
              wait(100);
              digitalWrite(buzzer, LOW);
              wait(100); //Nécessaire pour avoir le temps de lâcher le bouton
            }
            else
            {
              //Bouton blanc
              if (choix == 1) // On règle les minutes
              {
                if (maximum >= 60) // On ne peut pas retirer une minute si on a moins de 60 secondes
                  maximum = maximum - 60;
              }
              else // On règle les secondes
              {
                if (maximum > 0) // On ne peut pas retirer une seconde si on est déjà à 0 secondes
                  maximum--;
              }
              digitalWrite(buzzer, HIGH);
              wait(100);
              digitalWrite(buzzer, LOW);
              wait(100); //Nécessaire pour avoir le temps de lâcher le bouton
            }
          }
          else
          {
            //Bouton bleu
            if (choix == 1) // On règle les minutes
            {
              if (maximum < 5940) // On ne peut pas ajouter une minute si on est déjà à 99 minutes (99 * 60 = 5940 sec)
                maximum = maximum + 60;
            }
            else // On règle les secondes
            {
              if (maximum < 5999) // On ne peut pas ajouter de seconde si on a déjà atteint le max affichable de 99:59 (99 * 60 + 59 = 5999 secondes)
                maximum++;
            }
            digitalWrite(buzzer, HIGH);
            wait(100);
            digitalWrite(buzzer, LOW);
            wait(100); //Nécessaire pour avoir le temps de lâcher le bouton
          }
        }
        else
        {
          //Bouton rouge
          choix++;
          if (choix >= 3)
            choix = 1;
          sevseg.setNumber(choix);
          digitalWrite(buzzer, HIGH);
          wait(100);
          digitalWrite(buzzer, LOW);
          wait(100); //Nécessaire pour avoir le temps de lâcher le bouton
        }
      }
    }
      presseJaune = false;
      timer(maximum,1000);
      break;

  }

  sevseg.refreshDisplay();
}

void wait(int temps) //Fonction pour forcer une attente du temps désiré (en ms). Remplace le delay() pour être compatible avec la bibliothèque SevSeg.h
{
  unsigned long t1, t2;

  t1 = millis();
  do
  {
    sevseg.refreshDisplay();
    t2 = millis();
  } while (t2-t1 < temps);
}

void wait_and_see(int temps, int bouton) //Fonction pour forcer une attente du temps désiré (en ms). Remplace le delay() pour être compatible avec la bibliothèque SevSeg.h
{                                        // Et écoute si un bouton est pressé. (Résistances : pullup = 10 k
  presseJaune = false;                   //                                                   bouton jaune = 1k => tension = 1/11 * 5V = 0.454V
  presseBlanc = false;                   //                                                   bouton blanc = 4.7k => tension = 4.7/14.7 * 5V = 1.600V
  presseBleu = false;                    //                                                   bouton bleu = 10k => tension = 10/20 * 5V = 2.5V
  presseRouge = false;                   //                                                   bouton rouge = 47k => tension = 47/57 * 5V = 4.123V )
  unsigned long t1, t2;

  t1 = millis(); //Récupère le temps t1
  do
  {
    sevseg.refreshDisplay();

    int analog = analogRead(bouton);
    float tension = (float)analog * 5/1023;

    if (tension < 4.2)
    {
      if (tension < 2.5)
      {
        if (tension < 1.6 )
        {
          if (tension < 0.5)
          {
            //Bouton jaune
            presseJaune = true;
            digitalWrite(buzzer, HIGH);
            wait(100);
            digitalWrite(buzzer, LOW);
            wait(100); //Nécessaire pour avoir le temps de lâcher le bouton
          }
          else
          {
            //Bouton blanc
            presseBlanc = true;
            digitalWrite(buzzer, HIGH);
            wait(100);
            digitalWrite(buzzer, LOW);
            wait(100); //Nécessaire pour avoir le temps de lâcher le bouton
          }
        }
        else
        {
          //Bouton bleu
          presseBleu = true;
          digitalWrite(buzzer, HIGH);
          wait(100);
          digitalWrite(buzzer, LOW);
          wait(100); //Nécessaire pour avoir le temps de lâcher le bouton
        }
      }
      else
      {
        //Bouton rouge
        presseRouge = true;
        digitalWrite(buzzer, HIGH);
        wait(100);
        digitalWrite(buzzer, LOW);
        wait(100); //Nécessaire pour avoir le temps de lâcher le bouton
      }
    }

    else
    {
      t2 = millis(); //Récupère le temps t2
    }
  } while (t2-t1 < temps);
}

void pause() //Fonction qui pause un programme en attendant l'appui du bouton jaune
{
  presseJaune = true; //Simule un bouton pressé
  while(presseJaune)
  {
    int analog = analogRead(bouton);
    float tension = (float)analog * 5/1023;

    if (tension < 0.5)
    {
      //Bouton jaune
      presseJaune = false;
      digitalWrite(buzzer, HIGH);
      wait(100);
      digitalWrite(buzzer, LOW);
      wait(100); //Nécessaire pour avoir le temps de lâcher le bouton
    }
    else
      sevseg.refreshDisplay();
  }
}

void timer(int maximum, int interval) //Fonction de minuterie qui demande de combien on part et la durée de l'intervalle entre chaque numéro (en ms)
{
  unsigned long t1, t2;
  short i, j;
  int num = 0, minut_timer = 0, sec_timer = 0;

  if (maximum > 6000 || maximum < 0 || interval < 0 ) //Vérifie qu'il n'y a pas d'erreur dans les valeurs demandées (6000 car max possible en mm:ss est 99:60 = 100 min = 100 * 60 sec = 6000 sec)
  {
    sevseg.refreshDisplay();
    sevseg.setChars("ERR");
    wait(5000);
  }

  else
  {
    while(maximum >= 0)
    {
      minut = maximum/60;
      sec = maximum%60;
      //num = (maximum/60) * 100 + (maximum%60);
      num = minut*100 + sec;
      sevseg.refreshDisplay();
      if(minut >= 1)
      {
        if (maximum % 2 == 0)             //Fait clignoter le point décimal entre les min et sec
          sevseg.setNumber(num,2);
        else
          sevseg.setNumber(num,-1);
      }
      else
        sevseg.setNumber(num,-1);
      maximum--;
      wait_and_see(interval, bouton);
      if (presseRouge)
      {
        sevseg.setChars("END");
        break;
      }
    }
    sevseg.refreshDisplay();
    sevseg.setNumber(num,2);

    if (presseRouge)
    {
      sevseg.refreshDisplay();
      presseRouge = false;
    }
    else
    {
      while (!presseJaune)
      {
        for (i=0;i<3;i++)
        {
          for (j=0;j<3;j++)
          {

            digitalWrite(buzzer, HIGH);
            wait_and_see(100, bouton);
            if (presseJaune)
              break;
            digitalWrite(buzzer, LOW);
            wait_and_see(100, bouton);
            if (presseJaune)
              break;
          }
          if (presseJaune)
            break;
          wait_and_see(1000, bouton);
          if (presseJaune)
            break;
        }
      }
      presseJaune = false;
    }
  }
}

void chrono(int interval) //Fonction de chronomètre avec choix de la durée de l'intervalle (en ms) et bouton start/stop
{
  int num=0;
  int minut = 0, sec = 0;

  if (interval < 0)
  {
    sevseg.refreshDisplay();
    sevseg.setChars("ERR");
    wait(5000);
  }

  else
  {
    while (minut < 100)
    {
      num = minut * 100 + sec;
      sevseg.refreshDisplay();
      if (minut>=1)
      {
        if (sec % 2 == 0)             //Fait clignoter le point décimal entre les min et sec
          sevseg.setNumber(num,2);
        else
          sevseg.setNumber(num,-1);
      }
      else
        sevseg.setNumber(num,-1);
      wait_and_see(interval, bouton);
      if (presseRouge)
      {
        sevseg.setChars("END");
        break;
      }
      if (presseJaune)
      {
        if (minut >= 1)
          sevseg.setNumber(num,2);
        else
          sevseg.setNumber(num,-1);
        pause();
      }
      else
      {
        sec++;
        if (sec == 60) //Incrémente la minute suivante et remet donc sec à 0
        {
          minut++;
          sec = 0;
        }
      }
    }
    if (presseRouge)
      presseRouge = false;
  }
}

void myClock() //clock étant un nom réservé je créée la fonction myClock qui est comme son nom l'indique une horloge
{              //Si l'horloge n'a pas été d'abord reglé avec setClock elle commencera au temps UNIX, càd le 01/01/1970 à 00:00:00 !
  int num, analog;
  short choix = 1, i, j;
  float tension;

  hr = hour();
  minut = minute();
  sec = second();
  d = day();
  mnth = month();
  yr = year();

  while (!presseRouge) // Boucle de l'horloge. On peut utiliser les bouton bleu et blanc pour naviguer entre les affichages :
  {                    // 1) hh:mm
    hr = hour();       // 2) jj:mm
    minut = minute();  // 3) aaaa
    sec = second();    // 4) alarme hh:mm
    d = day();
    mnth = month();
    yr = year();

    sevseg.refreshDisplay();

    analog = analogRead(bouton);
    tension = (float)analog * 5/1023;

    if (tension < 4.2)
    {
      if (tension < 2.5)
      {
        if (tension < 1.6 )
        {
          if (tension < 0.5)
          {
            //Bouton jaune => Active/désactive l'alarme
            if (alarm)
            {
              digitalWrite(buzzer, HIGH);
              wait(100);
              digitalWrite(buzzer, LOW);
              wait(100); //Nécessaire pour avoir le temps de lâcher le bouton
              alarm = false;
            }
            else
            {
              for (i = 0; i < 3; i++) // buzzer beep 3 fois pour confirmer que l'alarme est activée
              {
                digitalWrite(buzzer, HIGH);
                wait(100);
                digitalWrite(buzzer, LOW);
                wait(100);
              }
              alarm = true;
            }
          }
          else
          {
            //Bouton blanc => retire
            choix--;
            if (choix <= 0)
              choix = 4;
            digitalWrite(buzzer, HIGH);
            wait(100);
            digitalWrite(buzzer, LOW);
            wait(100); //Nécessaire pour avoir le temps de lâcher le bouton
          }
        }
        else
        {
          //Bouton bleu => ajoute
          choix++;
          if (choix >= 5)
            choix = 1;
          digitalWrite(buzzer, HIGH);
          wait(100);
          digitalWrite(buzzer, LOW);
          wait(100); //Nécessaire pour avoir le temps de lâcher le bouton
        }
      }
      else
      {
        //Bouton rouge => Quitte l'horloge
        presseRouge = true;
        digitalWrite(buzzer, HIGH);
        wait(100);
        digitalWrite(buzzer, LOW);
        wait(100); //Nécessaire pour avoir le temps de lâcher le bouton
      }
    }

    if (choix == 1) // Affiche hh:mm
    {
      num = hr * 100 + minut;
      if (hr >= 1)
      {
        if( sec % 2 )
        {
          sevseg.setNumber(num,2);
        }
        else
        {
          sevseg.setNumber(num,-1);
        }
      }
      else
      {
        sevseg.setNumber(num,-1);
      }
    }

    if (choix == 2) // Affiche jj:mm
    {
      num = d * 100 + mnth;
      sevseg.setNumber(num,2);
    }

    if (choix == 3) // Affiche aaaa
    {
      sevseg.setNumber(yr,-1);
    }

    if (choix == 4) // Affiche l'alarme
    {
      num = hr_alarm * 100 + minut_alarm;
      if (hr_alarm >= 1)
        sevseg.setNumber(num, 2);
      else
        sevseg.setNumber(num, -1);
    }

    // alarme
    if (alarm)
    {
      if (hr == hr_alarm && minut == minut_alarm)
      {

        while (!presseJaune)
        {
          for (i=0;i<3;i++)
          {
            for (j=0;j<3;j++)
            {

              digitalWrite(buzzer, HIGH);
              wait_and_see(100, bouton);
              if (presseJaune)
                break;
              digitalWrite(buzzer, LOW);
              wait_and_see(100, bouton);
              if (presseJaune)
                break;
            }
            if (presseJaune)
              break;
            wait_and_see(1000, bouton);
            if (presseJaune)
              break;
          }
        }
        presseJaune = false;
        alarm = false;
      }
    }
  }
}

void setClock() // Permet de changer l'heure et la date de l'horloge
{               // Ne vérifie pas si l'année est bisextile ou non, trop à mettre en place pour peu d'intérêt...
  int num = 0; //Je pars de l'année 2017, année où j'écris ce programme, pour plus de facilité d'utilisation.
  int analog;
  float tension;
  short choix = 1;
  bool err = false;

  sevseg.refreshDisplay();
  sevseg.setChars("AAAA");
  pause();

  presseJaune = false;

  while (!presseJaune) //Première boucle pour regler l'année (aaaa)
  {
    sevseg.refreshDisplay();

    analog = analogRead(bouton);
    tension = (float)analog * 5/1023;

    num = yr;

    sevseg.setNumber(num,-1);

    if (tension < 4.2)
    {
      if (tension < 2.5)
      {
        if (tension < 1.6 )
        {
          if (tension < 0.5)
          {
            //Bouton jaune
            presseJaune = true;
            digitalWrite(buzzer, HIGH);
            wait(100);
            digitalWrite(buzzer, LOW);
            wait(100); //Nécessaire pour avoir le temps de lâcher le bouton
          }
          else
          {
            //Bouton blanc => retire
            if (yr >= 1970) // On ne peut pas descendre au delà de 1970, vu que le temps UNIX utilisé ici commence le 01/01/1970 à 00:00
              yr--;
            digitalWrite(buzzer, HIGH);
            wait(100);
            digitalWrite(buzzer, LOW);
            wait(100); //Nécessaire pour avoir le temps de lâcher le bouton
          }
        }
        else
        {
          //Bouton bleu => ajoute
          if (yr <= 9999) // On ne peut pas ajouter au delà de 9999, limite affichable sur le 28 segment
            yr++;
          digitalWrite(buzzer, HIGH);
          wait(100);
          digitalWrite(buzzer, LOW);
          wait(100); //Nécessaire pour avoir le temps de lâcher le bouton
        }
      }
      else
      {
        //Bouton rouge => ne fait rien ici
      }
    }
  }

  sevseg.refreshDisplay();
  sevseg.setChars("date");
  pause();

  presseJaune = false;

  do
  {
    while (!presseJaune) //Seconde boucle pour regler la date (jj:mm)
    {
      sevseg.refreshDisplay();

      analog = analogRead(bouton);
      tension = (float)analog * 5/1023;

      num = 100 * d + mnth;

      sevseg.setNumber(num,2);

      if (tension < 4.2)
      {
        if (tension < 2.5)
        {
          if (tension < 1.6 )
          {
            if (tension < 0.5)
            {
              //Bouton jaune => Confirme (fait sortir de la boucle)
              presseJaune = true;
              digitalWrite(buzzer, HIGH);
              wait(100);
              digitalWrite(buzzer, LOW);
              wait(100); //Nécessaire pour avoir le temps de lâcher le bouton
            }
            else
            {
              //Bouton blanc => retire
              if (choix == 1) // On règle les jours
              {
                if (d <= 1) // On ne peut pas descendre en dessous du 1e du mois
                {
                  d = 31;
                }
                else // Cas normal
                  d--;
              }
              else // On règle les mois
              {
                if (mnth <= 1) // On ne peut pas descendre en dessous de janvier
                  mnth = 12;
                else // Cas normal
                  mnth--;
              }
              digitalWrite(buzzer, HIGH);
              wait(100);
              digitalWrite(buzzer, LOW);
              wait(100); //Nécessaire pour avoir le temps de lâcher le bouton
            }
          }
          else
          {
            //Bouton bleu => ajoute
            if (choix == 1) // On règle les jours
            {
              if (d >= 31)
                d = 1;
              else // Cas normal, on incrémente
                d++;
            }
            else // On règle les mois
            {
              if (mnth >= 12) // On ne peut pas monter au delà de décembre
                mnth = 1;
              else
                mnth++;
            }
            digitalWrite(buzzer, HIGH);
            wait(100);
            digitalWrite(buzzer, LOW);
            wait(100); //Nécessaire pour avoir le temps de lâcher le bouton
          }
        }
        else
        {
          //Bouton rouge => change entre les jours et les mois
          choix++;
          if (choix >= 3)
            choix = 1;
          sevseg.setNumber(choix);
          digitalWrite(buzzer, HIGH);
          wait(100);
          digitalWrite(buzzer, LOW);
          wait(100); //Nécessaire pour avoir le temps de lâcher le bouton
        }
      }
    }

    if (d > 28) // On vérifie que la date est valide
    {
      if (mnth == 2)
        err = true;
      else if (d > 30)
      {
        if (mnth == 4 || mnth == 6 || mnth == 9 || mnth == 11)
          err = true;
        else if (d > 31)
          err = true;
        else
          err = false;
      }
      else
        err = false;
    }
    else if (d <= 0)
      err = true;
    else
      err = false;

    if (err)
    {
      sevseg.refreshDisplay();
      sevseg.setChars("Err");
      pause();
    }
  } while(err);

  sevseg.refreshDisplay();
  sevseg.setChars("hour");
  pause();

  presseJaune = false;

  while (!presseJaune) //Troisième boucle, règle heure et minute (hh:mm)
  {
    sevseg.refreshDisplay();

    analog = analogRead(bouton);
    tension = (float)analog * 5/1023;

    num = 100 * hr + minut;

    sevseg.setNumber(num,2);

    if (tension < 4.2)
    {
      if (tension < 2.5)
      {
        if (tension < 1.6 )
        {
          if (tension < 0.5)
          {
            //Bouton jaune => Confirme (fait sortir de la boucle)
            presseJaune = true;
            digitalWrite(buzzer, HIGH);
            wait(100);
            digitalWrite(buzzer, LOW);
            wait(100); //Nécessaire pour avoir le temps de lâcher le bouton
          }
          else
          {
            //Bouton blanc => retire
            if (choix == 1) // On règle les heures
            {
              if (hr <= 0) // Si on est à 00:xx, on retourne à 23:xx
                hr = 23;
              else // Cas normal, on retire une heures
                hr--;
            }
            else // On règle les minutes
            {
              if (minut <= 0) // Si on est à xx:00, on passe à l'heure inférieure
              {
                if (hr <= 0) // Il est 00:00, on doit passer à 23:59
                {
                  hr = 23;
                  minut = 59;
                }
                else // Il est xx:00, il faut donc passer à xx(-1):59
                {
                  hr--;
                  minut = 59;
                }
              }
              else // Cas normal, on retire juste une minutes
                minut--;
            }
            digitalWrite(buzzer, HIGH);
            wait(100);
            digitalWrite(buzzer, LOW);
            wait(100); //Nécessaire pour avoir le temps de lâcher le bouton
          }
        }
        else
        {
          //Bouton bleu => ajoute
          if (choix == 1) // On règle les heures
          {
            if (hr < 23) // On ne peut pas ajouter une heure si on est déjà à 23 heures
              hr++;
            else //Il est 23h (peu importe les minutes)
              hr = 0;
          }
          else // On règle les minutes
          {
            if (minut >= 59) //On ne peut pas aller au delà de 59 minutes
            {
              if (hr >= 23) //On ne peut pas aller au delà de 23:59
              {
                hr = 0; //On met à 00:00 si c'est le cas
                minut = 0;
              }
              else //Il est xx:59 => il faut passer à l'heure suivante
              {
                hr++;
                minut = 0; //Et on reset évidemment les minutes
              }
            }
            else //Pas un cas particulier, on incrémente juste les minutes
              minut++;
          }
          digitalWrite(buzzer, HIGH);
          wait(100);
          digitalWrite(buzzer, LOW);
          wait(100); //Nécessaire pour avoir le temps de lâcher le bouton
        }
      }
      else
      {
        //Bouton rouge => change entre les heures et les minutes
        choix++;
        if (choix >= 3)
          choix = 1;
        sevseg.setNumber(choix);
        digitalWrite(buzzer, HIGH);
        wait(100);
        digitalWrite(buzzer, LOW);
        wait(100); //Nécessaire pour avoir le temps de lâcher le bouton
      }
    }
  }

  //On déinit maintenant l'horloge de l'Arduino grâce à la fonction setTime() de la bibliothèque TimeLib
  setTime(hr,minut,0,d,mnth,yr); //setTime(hr,min,sec,day,mnth,yr);

  //On appelle alors l'horloge
  myClock();
}

void setAlarm()
{
  int num, choix = 1;
  int analog;
  float tension;
  while (!presseJaune) //Troisième boucle, règle heure et minute (hh:mm)
  {
    sevseg.refreshDisplay();

    analog = analogRead(bouton);
    tension = (float)analog * 5/1023;

    num = 100 * hr_alarm + minut_alarm;

    sevseg.setNumber(num,2);

    if (tension < 4.2)
    {
      if (tension < 2.5)
      {
        if (tension < 1.6 )
        {
          if (tension < 0.5)
          {
            //Bouton jaune => Confirme (fait sortir de la boucle)
            presseJaune = true;
            digitalWrite(buzzer, HIGH);
            wait(100);
            digitalWrite(buzzer, LOW);
            wait(100); //Nécessaire pour avoir le temps de lâcher le bouton
          }
          else
          {
            //Bouton blanc => retire
            if (choix == 1) // On règle les heures
            {
              if (hr_alarm <= 0) // Si on est à 00:xx, on retourne à 23:xx
                hr_alarm = 23;
              else // Cas normal, on retire une heures
                hr_alarm--;
            }
            else // On règle les minutes
            {
              if (minut_alarm <= 0) // Si on est à xx:00, on passe à l'heure inférieure
              {
                if (hr_alarm <= 0) // Il est 00:00, on doit passer à 23:59
                {
                  hr_alarm = 23;
                  minut_alarm = 59;
                }
                else // Il est xx:00, il faut donc passer à xx(-1):59
                {
                  hr_alarm--;
                  minut_alarm = 59;
                }
              }
              else // Cas normal, on retire juste une minutes
                minut_alarm--;
            }
            digitalWrite(buzzer, HIGH);
            wait(100);
            digitalWrite(buzzer, LOW);
            wait(100); //Nécessaire pour avoir le temps de lâcher le bouton
          }
        }
        else
        {
          //Bouton bleu => ajoute
          if (choix == 1) // On règle les heures
          {
            if (hr_alarm < 23) // On ne peut pas ajouter une heure si on est déjà à 23 heures
              hr_alarm++;
            else //Il est 23h (peu importe les minutes)
              hr_alarm = 0;
          }
          else // On règle les minutes
          {
            if (minut_alarm >= 59) //On ne peut pas aller au delà de 59 minutes
            {
              if (hr_alarm >= 23) //On ne peut pas aller au delà de 23:59
              {
                hr_alarm = 0; //On met à 00:00 si c'est le cas
                minut_alarm = 0;
              }
              else //Il est xx:59 => il faut passer à l'heure suivante
              {
                hr_alarm++;
                minut_alarm = 0; //Et on reset évidemment les minutes
              }
            }
            else //Pas un cas particulier, on incrémente juste les minutes
              minut_alarm++;
          }
          digitalWrite(buzzer, HIGH);
          wait(100);
          digitalWrite(buzzer, LOW);
          wait(100); //Nécessaire pour avoir le temps de lâcher le bouton
        }
      }
      else
      {
        //Bouton rouge => change entre les heures et les minutes
        choix++;
        if (choix >= 3)
          choix = 1;
        sevseg.setNumber(choix);
        digitalWrite(buzzer, HIGH);
        wait(100);
        digitalWrite(buzzer, LOW);
        wait(100); //Nécessaire pour avoir le temps de lâcher le bouton
      }
    }
  }

  presseJaune = false;
  alarm = true;

  myClock();
}
