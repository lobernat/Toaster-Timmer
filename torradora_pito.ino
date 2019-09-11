//Sonoff Basic
//GPIO 0 -> polsador 
//GPIO 12-> Rele
//GPIO 13-> Led
//GPIO 14-> sota GND
//Precalentat -> 50 segons aprox
//tems a arrofredar -> despres de 120 segons cal tornar a precalentar
//https://github.com/contrem/arduino-timer
//precalentat + pa congelat ->2 minuts
//congelat segona cara-> sembla que nomes cal descongelar la primera cara ->1:20 minuts



// Pin config
int RELE_SONOFF = 13;
int BUZZER_PIN = 14;
int POLSADOR_SONOFF = 0;
int PRECALENTAT = 10*1000; //Quan la torradora arranca per primer cop fa un precalentat
int DELAYPRECALENTAT = 15000;
int tempsPaNormal=1*1000; //temsp en msec 
int tempsPaCongelat=4*1000; //temsp en msec 
bool justArrancat = true; //precalentar el primer cop al arrancar


bool TORRANT = false; //desactivar el boto mentre està torrant
unsigned long tempsInicial = millis(); //temps inicial


#include <timer.h>
#define USE_SERIAL Serial


auto timer = timer_create_default(); // Temporitzador torradora

bool aturarTorradora(void *) {
  digitalWrite(RELE_SONOFF, HIGH); // Aturar torradora
  TORRANT = false;//activar altre cop el polsador
  tempsInicial = millis();//saber el moment que ha parat de torrar
  pitoAlarma();
  return true; // repeat? true
}





unsigned long tempsTranscorregut(unsigned long tempsInicial){
      unsigned long tempsQueHaPassat =  millis() - tempsInicial;
 //     Serial.println("milis");
 //     Serial.println(millis());
 //     Serial.println("temps inicial");
 //     Serial.println(tempsInicial);
 //     Serial.println("diferencia");
 //     Serial.println(millis() - tempsInicial);
      return tempsQueHaPassat;
  }


void pito( unsigned int frequency, unsigned long duration)
{   
    analogWriteFreq(frequency);
    analogWrite(BUZZER_PIN,500);
    delay(duration);
    analogWrite(BUZZER_PIN,0);
}


void pitoAlarma(void)
{
    pito(523,100);
    delay(50);
    pito(659,100);
    delay(50);
    pito(880,300);
    delay(150);
    pito(523,100);
    delay(50);
    pito(659,100);
    delay(50);
    pito(880,300);
    delay(50);
    
    /*
    pito(587,100);
    delay(150);
    pito(880,100);
    delay(50);
    pito(698,100);
    delay(50);
    pito(587,200);
    delay(50);
*/

}

void pitoNormal(void)
{
    pito(587,100);
    delay(50);
   // pito(659,200);
   // delay(50);
}

void pitoCongelat(void)
{
    pito(587,100);
    delay(50);
    pito(659,200);
    delay(50);
}



void polsatCurt(){ 
  //Pa sense congelar
    Serial.println("Torrant Pa normal");
    TORRANT = true;
    digitalWrite(RELE_SONOFF, LOW);
    timer.in((tempsPaNormal)+calPrecalentar(), aturarTorradora);
    justArrancat = false;
  }
  
void polsatLLarg(){
  //Pa congelat
    Serial.println("Torrant Pa Congelat");
    TORRANT = true;
    digitalWrite(RELE_SONOFF, LOW);
    timer.in((tempsPaCongelat)+calPrecalentar(), aturarTorradora);
    justArrancat = false;
  }

void setup() {
  USE_SERIAL.begin(115200);
  pinMode(RELE_SONOFF, OUTPUT); // set LED pin to OUTPUT
  pinMode(POLSADOR_SONOFF, INPUT);
  pinMode (BUZZER_PIN, OUTPUT );
  digitalWrite(RELE_SONOFF, HIGH); //arrancar amb el rele apagat
  Serial.println("Torradora Arrancada");

}

void loop() {
  timer.tick(); // 

  if (digitalRead(POLSADOR_SONOFF) == LOW && TORRANT == false) {         // 
    BotoApretat();
  }
  
}


 int calPrecalentar(){
    if (tempsTranscorregut(tempsInicial) > DELAYPRECALENTAT || justArrancat == true){
      Serial.println("---> Cal tornar a Precalentar!");
      Serial.print(PRECALENTAT);
      Serial.println(" milisegons");

      return PRECALENTAT;
    }
    return 0;
  }

void BotoApretat() {
  //detectar botó apretat
  int held = 0;
  while (digitalRead(POLSADOR_SONOFF) == LOW && held < 9)
  {
    delay(50);
    held++;
    Serial.println(held);
  }
  if (held < 9) {
    polsatCurt();
    pitoNormal();
  }
    else if (held >= 9) {
    polsatLLarg();
    pitoCongelat();
  }
  //Fi detectar boto

}
