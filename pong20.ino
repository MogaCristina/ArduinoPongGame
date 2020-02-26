#include <U8glib.h>

#include "U8glib.h"

//pinii lcd-ului
U8GLIB_PCD8544 u8g(7, 6, 4, 5, 3);  // CLK=7, DIN=6, CE=4, DC=5, RST=3
#define backlight 11

int Jucator1Pin = A1; // Pin potentiomentru jucator stanga
int Jucator2Pin = A0; // Pin potentiomentru jucator dreapta
int ScorCastigator = 7; // Scorul pentru Winner
int VitezaInit = 30; // Viteza initiala pentru inceput de joc


u8g_uint_t LatimeTeren, InaltimeTeren, HalfLatimeTeren;
u8g_uint_t MarimeMinge = 4;
u8g_uint_t Minge0X;
u8g_uint_t DirectieMinge0X = 1;
u8g_uint_t Minge0Y;
u8g_uint_t DirectieMinge0Y= 1;//directia se va incrementa cu 1
u8g_uint_t LBaraJucator = 2;//latimea barii jucatorului este de 2
u8g_uint_t HBaraJucator = 8;//inaltimea este de 8, jocul este cu barele pe verticala
u8g_uint_t HalfHBaraJucator = HBaraJucator/2;
u8g_uint_t JucatorStangaPoz0Y; // Jucator stanga pozitie pe verticala
u8g_uint_t JucatorDreaptaPoz0Y; //Jucator dreapta pozitie pe verticala

int ScorJucatorStanga, ScorJucatorDreapta; // Scoruri jucatori stanga si dreapta
bool JocTerminat = false;
unsigned long MutareMinge; // vom stoca timpul in functie de care se va misca mingea
int VitezaActuala = VitezaInit; // Viteza curenta a mingii 

//Partea de desenare a jocului

//cand jocul e terminal afiseaza pe ecran "Joc Terminat"

void JocGata(){
  u8g.setFont( u8g_font_timB14);
  u8g.setColorIndex(1);
  u8g.setFontPosCenter();
  char str1[] = "Joc";
  char str2[] = "Terminat";
  u8g_uint_t s1 = (LatimeTeren - u8g.getStrPixelWidth(str1)) / 2;
  u8g_uint_t s2 = (LatimeTeren - u8g.getStrPixelWidth(str2)) / 2;
  u8g.drawStr(s1, 20, str1);
  u8g.drawStr(s2, 40, str2);
  int Jucator1Acord=analogRead(Jucator1Pin);
  int Jucator2Acord=analogRead(Jucator2Pin);
  if(Jucator1Acord==0)
     if(Jucator2Acord==0){
    delay(2000);
    JocTerminat=false;
    ScorJucatorStanga=0;
    ScorJucatorDreapta=0;
    }
  
}

//afiseaza scorul actual

void Scor(){
  u8g.setFont(u8g_font_unifont);
  u8g.setColorIndex(1);//index 1 -> pixel on
  char strScorJucatorStanga[] = " "; 
  char strScorJucatorDreapta[] = " ";
  strScorJucatorStanga[0] = '0' + ScorJucatorStanga;
  strScorJucatorDreapta[0] = '0' + ScorJucatorDreapta;
  u8g.setFont(u8g_font_04b_03b);
  //afla latimea intului in pixeli
  u8g_uint_t latimescor = u8g.getStrPixelWidth(strScorJucatorStanga);
  u8g_uint_t scoreY = 7;//am dat int-ul pt scorul maxim ca poz Y in functie
  //am scazut si adunat 7 ca sa aliniez mai bine socrurile/distantele dintre ele
  u8g.drawStr( HalfLatimeTeren - 7 - latimescor, scoreY, strScorJucatorStanga);
  u8g.drawStr( HalfLatimeTeren + 7, scoreY, strScorJucatorDreapta);
}

//desenarea barilor de joc pentru playeri
void BaraUnJucator(int L, int H){
  u8g.setFont(u8g_font_profont11);
  u8g.drawBox(L, H, LBaraJucator, HBaraJucator);
}

void BareJucatori(){
  u8g.setFont(u8g_font_profont11);
  u8g.setColorIndex(1);
  
  //citirea valorilor de miscare se face de pe potentiometre
  int rawDataStg=analogRead(Jucator1Pin);//jucator stanga
  JucatorStangaPoz0Y = map(rawDataStg, 0, 1023, 0, InaltimeTeren - HBaraJucator);
  int rawDataDr=analogRead(Jucator2Pin);//jucator dreapta
  JucatorDreaptaPoz0Y = map(rawDataDr, 0, 1023, 0, InaltimeTeren - HBaraJucator);
  
  //desenez bara pentru jucatorul din stanga
  BaraUnJucator(0, JucatorStangaPoz0Y);

 //desenez bara pt jucatorul din dreapta
  BaraUnJucator(LatimeTeren - LBaraJucator, JucatorDreaptaPoz0Y);
}

//Partea de joc in sine

//bara nelovita, lovitura ratata

bool PeLangaBara(u8g_uint_t py){
  u8g_uint_t MingeBottom = Minge0Y;
  u8g_uint_t MingeTop= Minge0Y + MarimeMinge - 1;
  u8g_uint_t BaraBottom = py;
  u8g_uint_t BaraTop = py + HBaraJucator - 1;
  return MingeBottom > BaraTop || MingeTop < BaraBottom;
}
//daca a ratat jucatorul din stanga mingea
void JucatorStangaMiss(){
  // Mingea se va servi de pe mijlocul barii de joc a jucatorului din dreapta
  Minge0X = LatimeTeren - MarimeMinge - 1;
  Minge0Y = JucatorDreaptaPoz0Y + HalfHBaraJucator;
  //creste scorul jucatorului din dreapta
  ScorJucatorDreapta++;
  //se revine la viteza initiala de joc
  VitezaActuala = VitezaInit;
  //testez daca e castigator
  if(ScorJucatorDreapta == ScorCastigator) 
    JocTerminat = true;
}
//daca a ratat jucatorul din dreapta mingea
void JucatorDreaptaMiss(){
  //  Mingea se va servi de pe mijlocul barii de joc a jucatorului din stanga
  Minge0X = 1;
  Minge0Y = JucatorStangaPoz0Y + HalfHBaraJucator;
  ScorJucatorStanga++;
  VitezaActuala = VitezaInit;
  if (ScorJucatorStanga == ScorCastigator) 
    JocTerminat = true;
}
//in cazul in care nu e miss se testeaza directia pe care se afla mingea si se schimba
//astfel,de exemplu, daca a ajuns pe bara din stanga ea va ricosa in dreapta
void Ricoseaza0X(){
  DirectieMinge0X = -DirectieMinge0X;//schimb directia
  VitezaActuala--; // Cand viteza se decrementeaza se va mari viteza in joc
}
//modificarea vitezei mingiei merge astfel: la timpul actual se adauga viteza actuala a mingii, cu cat e mai mica, mingea se va "muta" in teren mai repede
//de aceea se scade viteza initiala a mingiei pe parcursul jocului
void UpdatePozitieMinge(){
  if (millis() > MutareMinge) {
    Minge0X=Minge0X + DirectieMinge0X;//se incrementeaza cu 1 pozitia mingii
    //iese din teren in stanga
    if (Minge0X <= 0)
        if (PeLangaBara(JucatorStangaPoz0Y)) 
          JucatorStangaMiss(); 
        else 
          Ricoseaza0X();
  //iese din teren in dreapta
    if (Minge0X >= (LatimeTeren - MarimeMinge))
        if (PeLangaBara(JucatorDreaptaPoz0Y)) 
          JucatorDreaptaMiss(); 
        else 
          Ricoseaza0X();

    Minge0Y=Minge0Y + DirectieMinge0Y;
    //in cazul in care se "iese din teren", adica a lovit placa de joc se modifica directia
    if (Minge0Y >= (InaltimeTeren - MarimeMinge) || Minge0Y <= 0) {
      DirectieMinge0Y= -DirectieMinge0Y;
      VitezaActuala--;
    }
   MutareMinge = millis() + VitezaActuala;
  }
}

//initial dau doar marimile terenului de joc adica ale LCD-ului
void setup(void) {
   LatimeTeren = 84;
   InaltimeTeren = 48;
   HalfLatimeTeren = LatimeTeren / 2;
   Serial.begin(9600);
}
//in loop se testeaza daca s-a atins scorul pentru a fi un winner si daca da se termina jocul si deseneaza pe ecran mesajul de terminare joc
//daca nu, se face update la pozitia mingiei,barelor jucatorilor si a scorului
void loop(void) {
  u8g.firstPage();
  do {
    if (JocTerminat)
       JocGata();
    else
      UpdatePozitieMinge();
     //afisez scorul curent
    Scor();
    //desenare minge pe ecran
    u8g.drawBox(Minge0X, Minge0Y, MarimeMinge, MarimeMinge);
    //desenare bare jucatori pe ecran
    BareJucatori();
  } while(u8g.nextPage());
}
