const int led_verte = 6; 
const int bouton_dec = 2; 
const int bouton_inc = 3;
const int hp = 8;
const int detect_in = 9;

// Fonctionnement normal :
int tempo = 120; //définition du tempo 
unsigned long int t; //définition de la période du signal
unsigned long int temp_mes = 0; // nécessaire au fonctionnement du metronome (clignotement)
int beat = 0; //compteur permettant de situer le temps dans la mesure
int sign = 4; // signature rythmique (nombre de noirs dans une mesure) 
boolean etat_led = false; // réprésente l'état des leds : au départ éteintes

// Silence :
int nb_sil = 0;
int sil = 0;

// Recherche de tempo :
unsigned long int date; // nécessaire pour se repérer dans la recherche de tempo : date de la dernière pression
unsigned long int date_init; // date de la pression initiale
int nb_coups = 0; // nombre de coups donnés dans la recherche de tempo
boolean sch_tempo = false; // représente l'activation de la recherche de tempo
float tempo_moy; // tempo moyen calculé par la recherche de tempo




void setup(){

  pinMode(bouton_dec, INPUT);
  pinMode(bouton_inc, INPUT);
  pinMode(detect_in, INPUT);
  pinMode(hp, OUTPUT);
  pinMode(led_verte, OUTPUT); 
  
  Serial.begin(9600); 
  
}



void loop(){  
  //allumage de la led / du hp :--------------------------------------------------------------------------------------------
  
  t = 60000 / tempo;
  
  if(!etat_led && (millis()-temp_mes > 9*t/10)){
    
    if( !beat ) { // beat == 0 : 1er temps de la mesure : Eclairage Intense
      analogWrite(led_verte, 0); // écriture en sortie (broche 3) d'un état BAS (rapport cyclique 0)
      temp_mes = millis();
      etat_led = true; 
      
      // HP : 
      tone(hp, 988);
    }
    
    else {
      analogWrite(led_verte, 210); // écriture en sortie (broche 3) d'un état BAS, avec une intensité inférieure      
      temp_mes = millis();
      etat_led = true;
      
      //HP :
      tone(hp, 494);
    }
  }

  //éteignage de la led / du hp:-----------------------------------------------------------------------------------------------
  
  if(etat_led && (millis()-temp_mes > t/10)){

    analogWrite(led_verte, 255); // écriture en sortie (broche 3) d'un état HAUT
    temp_mes = millis();
    etat_led = false; 
    
    //HP :
    noTone(8);
    
    if( !beat ){
      beat = sign - 1; // nombre de temps à effectuer avant le début de la prochaine mesure
    }
    else { 
      --beat; // Il y a un temps de moins avant le début de la prochaine mesure
    }      
  }
  
  // Détection de tempo : après la 1ère pression sur le bouton de détection ----------------------------------------------------
  
  sch_tempo = !digitalRead(detect_in);
  
  // Initialisation de la recherche de tempo :
  if(sch_tempo){
    
    // Eteignage led / hp :
    analogWrite(led_verte, 255);
    noTone(8);
    etat_led = false;
    
    // Initialisation des dates :
    date = millis();
    date_init = millis();
    
  }
  
  while(sch_tempo){
    if (digitalRead(detect_in) == LOW){ 
      if ( nb_coups == 0 ) {
        date_init = date = millis(); // On met en mémoire la date de la pression initiale
        ++nb_coups;
        Serial.print("Premier coup \n \r");
      }   

    else {    
      date = millis(); // On met en mémoire la date de la dernière pression
      tempo_moy = nb_coups * 60000 / (date - date_init); // On calcul le tempo en partant du temps moyen entre 2 coups
      ++nb_coups;
      Serial.print(nb_coups, DEC);
      Serial.print("  ");
      Serial.println(tempo_moy, 0);
    }

    delay(200); // Tempo maximal détectable d'environ 300bpm

    }

    if ( ( millis() - date ) >= 2000 ){ // Si 2 secondes se sont écoulés depuis le dernier coup
      nb_coups = 0; // On réinitialise la recherche de tempo
      tempo = tempo_moy; // Le tempo de fonctionnement devient celui trouvé
      beat = 0; // On remet le beat sur le 1er temps
      temp_mes = millis(); // On remet le compteur à 0
      sch_tempo = false; // On sort de la recherche de tempo
    } 
  }
  
}
