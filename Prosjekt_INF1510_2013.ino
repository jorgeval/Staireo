// Importerer SD-kort og MP3shield bibliotek
#include <SPI.h>
#include <SdFat.h>
#include <SdFatUtil.h> 
#include <SFEMP3Shield.h>

SdFat sd; // Deklarerer SD-kortet
SFEMP3Shield MP3player; // Deklarerer MP3-player

// Deklarering av sensorer
int sensorOne = A0;
int sensorTwo = A1;
int sensorThree = A2;
int sensorFour = A3;
int sensorFive = A4;
int sensorSix = A5;

// Startverdi for kalibreringen, for at de ikke skal trigges prematurt
int s1cali = 255;
int s2cali = 255;
int s3cali = 255;
int s4cali = 255;
int s5cali = 255;
int s6cali = 255;

long interval = 30000; // 30 sek interval for kalibrering
long interval2 = 10000; // 10 sek interval for reset
long previousMillis2 = 0; // Til bruk i resettimer
long useMillis = 0; // Til bruk i resettimer

int buffer = 3; // Buffer for trigging av sensorer

boolean tStarted = false; // Om systemet har startet fra øverste sensor
boolean bStarted = false; // Om systemet har startet fra nederste sensor

// Boolean for om hvilke sensorer som har blitt trigget
boolean playedOne = false;
boolean playedTwo = false;
boolean playedThree = false;
boolean playedFour = false;
boolean playedFive = false;
boolean playedSix = false;

int selectSounds = 0; // For å velge sang

// Arrayer som peker på lydspor på SD-kortet
int sang1[6] = {
  1, 2, 3, 4, 5, 6};
int sang2[6] = {
  7, 8, 9, 10, 11, 12};
int sang3[6] = {
  13, 14, 15, 16, 17, 18};
int sang4[6] = {
  19, 20, 21, 22, 23, 24};

// Arrayer for å holde på arrayene med lydspor
int* sounds[3];
int* tracks;

void setup() {

  // Setter pinMode for sensorene
  pinMode(sensorOne, INPUT);
  pinMode(sensorTwo, INPUT);
  pinMode(sensorThree, INPUT);
  pinMode(sensorFour, INPUT); 
  pinMode(sensorFive, INPUT);
  pinMode(sensorSix, INPUT);

  // Åpner forbindelse med SD-kort og MP3-player
  sd.begin(SD_SEL, SPI_HALF_SPEED);
  MP3player.begin();
  MP3player.setVolume(10, 10);

  // Åpner forbindelse med serial
  Serial.begin(9600);

  /* Startkalibrering for sensorene
   * Innlesning av sensor / 4 = en verdi mellom 0-255.
   * - buffer gjør at sensorverdi må være buffer - 1 
   * eller mer for at sensoren skal trigges.
   ***************************************************/
  s1cali = (analogRead(sensorOne) / 4) - buffer;
  s2cali = (analogRead(sensorTwo) / 4) - buffer;
  s3cali = (analogRead(sensorThree) / 4) - buffer;
  s4cali = (analogRead(sensorFour) / 4) - buffer;
  s5cali = (analogRead(sensorFive) / 4) - buffer;
  s6cali = (analogRead(sensorSix) / 4) - buffer;

  // Legger lydarrayene inn i arrayet av lydarrayer
  sounds[0] = sang1;
  sounds[1] = sang2;
  sounds[2] = sang3;
  sounds[3] = sang4;

}

void loop() {

  long previousMillis = 0; // previousMillis til timeren
  unsigned long currentMillis = millis(); // currentMillis til timeren

  // Leser inn sensorverdi / 4, som gir en verdi mellom 0-255
  int value1 = analogRead(sensorOne) / 4;
  int value2 = analogRead(sensorTwo) / 4; 
  int value3 = analogRead(sensorThree) / 4;
  int value4 = analogRead(sensorFour) / 4; 
  int value5 = analogRead(sensorFive) / 4; 
  int value6 = analogRead(sensorSix) / 4; 

  // Tidtaker som printer ut tiden systemet har kjørt
  Serial.print("Tid: ");
  Serial.println(millis() / 1000);
  Serial.println(useMillis);

  /* Kalibrering av sensorene
   * Hvis ingen av sensorene er trigget og tiden har
   * passert intervallet vil sensorene kalibreres på nytt.
   *******************************************************/
  if(value1 > s1cali && value2 > s2cali && value3 > s3cali && value4 > s4cali && value5 > s5cali && value6 > s6cali) {
    if(currentMillis - previousMillis > interval) {
      previousMillis = currentMillis; 
      interval = interval + 30000;
      Serial.println("CALIBRATE!");

      s1cali = (analogRead(sensorOne) / 4) - buffer;
      s2cali = (analogRead(sensorTwo) / 4) - buffer;
      s3cali = (analogRead(sensorThree) / 4) - buffer;
      s4cali = (analogRead(sensorFour) / 4) - buffer;
      s5cali = (analogRead(sensorFive) / 4) - buffer;
      s6cali = (analogRead(sensorSix) / 4) - buffer;

    }
  }

  // Printer ut om systemet har startet og om noen av sensorene er avspilt
  Serial.print("Status avspilling bunn: " );
  Serial.print(bStarted);
  Serial.print("|    Status avspilling topp: " );
  Serial.println(tStarted);
  Serial.print("Status nr 1: " );
  Serial.print(playedOne);
  Serial.print("             | Status nr 2: " );
  Serial.print(playedTwo);
  Serial.print("             | Status nr 3: " );
  Serial.print(playedThree);
  Serial.print("             | Status nr 4: " );
  Serial.print(playedFour);  
  Serial.print("             | Status nr 5: " );
  Serial.print(playedFive);
  Serial.print("             | Status nr 6: " );
  Serial.println(playedSix);

  // Printer verdien til sensorene kontra kalibreringen
  Serial.print("Verdi sensor 1: ");
  Serial.print(value1);
  Serial.print(" / ");
  Serial.print(s1cali);
  Serial.print("  | Verdi sensor 2: ");
  Serial.print(value2);
  Serial.print(" / ");
  Serial.print(s2cali);
  Serial.print("  | Verdi sensor 3: ");
  Serial.print(value3);
  Serial.print(" / ");
  Serial.print(s3cali);
  Serial.print("  | Verdi sensor 4: ");
  Serial.print(value4);
  Serial.print(" / ");
  Serial.print(s4cali);
  Serial.print("  | Verdi sensor 5: ");
  Serial.print(value5);
  Serial.print(" / ");
  Serial.print(s5cali);
  Serial.print("  | Verdi sensor 6: ");
  Serial.print(value6);
  Serial.print(" / ");
  Serial.println(s6cali);

  /* Sensorene fra nederst til øverst
   * Hvis systemet ikke allerede er startet fra verken øverst 
   * eller nederst og første (nederste) blir trigget, starter 
   * systemet.
   *************************************************************/
  if (!tStarted && !bStarted && value1 < s1cali) {
    bStarted = true; // Setter systemet til å være startet fra nederst
    selectSounds = random(0, 4); // Velger et tilfeldig tall fra 0-4 som indikerer hvilke lydarray som velges
    tracks = sounds[selectSounds]; // Velger et array fra arrayet med lydarrayer
    Serial.println("Valgt sang: "); // Printer ut valgte sang/array
    Serial.println(selectSounds);

    playedOne = true; // Setter første sensor til å være avspilt

    // Spiller av første sang i valgte lydarray
    MP3player.playTrack(tracks[0]);
    Serial.println("Spiller nr 1");

  }

  // Starter en resettimeren som tar ca 30 sekunder
  if (bStarted) {
    useMillis += 350;

    Serial.println(previousMillis2);
    Serial.println(useMillis);
    Serial.println(interval2);

    if(useMillis - previousMillis2 > interval2) { 
      previousMillis2 = 0;
      if(bStarted) reset(); // Resetter om systemet ikke har fullført en gjennomgang
      useMillis = 0;
    }
  }

  /* Hvis systemet er startet, sensor 2 blir trigget og
   * sensor 1 er avspilt vil sensor 2 bli avspilt.
   * Dette fungerer for resten av sensorene også
   ******************************************************/
  if (bStarted && value2 < s2cali && playedOne) {

    playedTwo = true;

    MP3player.stopTrack(); // Stopper forrige lyd om den ikke er fullført
    MP3player.playTrack(tracks[1]);
    Serial.println("Spiller nr 2");
    useMillis = 0; // Resettimeren starter på nytt når sensoren blir trigget

    playedOne = false; // Resetter første

  }

  if (bStarted && value3 < s3cali && playedTwo) {
    playedThree = true;

    MP3player.stopTrack();
    MP3player.playTrack(tracks[2]);
    Serial.println("Spiller nr 3");
    useMillis = 0;

    playedTwo = false;

  }

  if (bStarted && value4 < s4cali && playedThree) {
    playedFour = true;

    MP3player.stopTrack();
    MP3player.playTrack(tracks[3]);
    Serial.println("Spiller nr 4");
    useMillis = 0;

    playedThree = false;
  }

  if (bStarted && value5 < s5cali &&  playedFour) {
    playedFive = true;

    MP3player.stopTrack();
    MP3player.playTrack(tracks[4]);
    Serial.println("Spiller nr 5");
    useMillis = 0;

    playedFour = false;
  }

  if (bStarted && value6 < s6cali && playedFive) {

    MP3player.stopTrack();
    MP3player.playTrack(tracks[5]);
    Serial.println("Spiller nr 6");

    playedFive = false;
    delay(4000);
    useMillis = 0;
    bStarted = false;
  }

  /* Sensorene fra øverst til nederst
   * Hvis systemet ikke allerede er startet fra verken nederst 
   * eller øverst og sjette (øverst) blir trigget, starter 
   * systemet. Samme metode som systemet fra nederst til øverst
   *************************************************************/
  if (!bStarted && !tStarted && value6 < s6cali) {
    tStarted = true;
    selectSounds = random(0, 4);
    tracks = sounds[0];
    Serial.println("Valgt sang: ");
    Serial.println(selectSounds);

    if (value6 < s6cali) playedSix = true;

    MP3player.playTrack(tracks[0]);
    Serial.println("Spiller nr 6");

  }

  if (tStarted) {
    useMillis += 350;

    Serial.println(previousMillis2);
    Serial.println(useMillis);
    Serial.println(interval2);

    if(useMillis - previousMillis2 > interval2) { 
      previousMillis2 = 0;
      if(tStarted) reset();
      useMillis = 0;
    }
  }

  if (tStarted && value5 < s5cali && playedSix) {

    playedFive = true;

    MP3player.stopTrack();
    MP3player.playTrack(tracks[1]);
    Serial.println("Spiller nr 5");
    useMillis = 0;

    playedSix = false;
  }

  if (tStarted && value4 < s4cali && playedFive) {

    playedFour = true;

    MP3player.stopTrack();
    MP3player.playTrack(tracks[2]);
    Serial.println("Spiller nr 4");
    useMillis = 0;

    playedFive = false;
  }

  if (tStarted && value3 < s3cali && playedFour) {

    playedThree = true;

    MP3player.stopTrack();
    MP3player.playTrack(tracks[3]);
    Serial.println("Spiller nr 3");
    useMillis = 0;

    playedFour = false;
  }

  if (tStarted && value2 < s2cali && playedThree) {
    playedTwo = true;

    MP3player.stopTrack();
    MP3player.playTrack(tracks[4]);
    Serial.println("Spiller nr 2");
    useMillis = 0;

    playedThree = false;
  }

  if (tStarted && value1 < s1cali && playedTwo) {

    MP3player.stopTrack();
    MP3player.playTrack(tracks[5]);
    Serial.println("Spiller nr 1");

    playedTwo = false;
    delay(4000);
    useMillis = 0;
    tStarted = false;
  }
}

// Resetmetode som resetter hele systemet
void reset() {
  Serial.println("RESET!");

  bStarted = false;
  tStarted = false;
  playedOne = false;
  playedTwo = false;
  playedThree = false;
  playedFour = false;
  playedFive = false;
  playedSix = false;
}
