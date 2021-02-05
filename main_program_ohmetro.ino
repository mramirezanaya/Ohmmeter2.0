#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Keypad.h>
 
// Definir constantes
#define ANCHO_PANTALLA 128 // ancho pantalla OLED
#define ALTO_PANTALLA 64 // alto pantalla OLED
#define BUFFER 20

#define BANDA1 1
#define BANDA2 2
#define BANDA3 3
#define BANDA4 4

#define modo_numero_a_color 0
#define modo_color_a_numero 1
#define modo_lee_resistencia 2
#define inicio 3

#define negro 0
#define marron 1
#define rojo 2
#define naranja 3
#define amarillo 4
#define verde 5
#define azul 6
#define violeta 7
#define gris 8
#define blanco 9

#define PERIODO 500

int x=0;
unsigned estado = inicio;

//globales del modo A
char a='x';
String cadena; 
char caracteres[BUFFER];
unsigned iA=0;
unsigned iB=0;

//globales modo C
unsigned long tiempo;
bool control=true;
float valor_pot, valor_res; //voltios
unsigned long r_pot=0, r_res=0, r_pot_ant=0, r_res_ant=0; //ohmios
unsigned long r_res_muestra=0, n_muestras=0;
unsigned long r_pot_muestra=0, n_muestras_pot=0;

//FUNCIONES
void pantalla(String cadena,unsigned tam,unsigned x,unsigned y);  //imprime por pantalla
void mostrar_resistencia(char valor[BUFFER]);                     
void colorea_banda(unsigned color,unsigned banda);
void rellena_color(unsigned array_color[3], unsigned color);
void apagar_resistencia();
void valor_a_comercial(char valor[BUFFER]);
unsigned short binary_lookup(int *A, int key, int imin, int imax);
void num_to_array(int long n, char res[BUFFER]);
int to_comercial(int valor);

// Objeto de la clase Adafruit_SSD1306
Adafruit_SSD1306 display(ANCHO_PANTALLA, ALTO_PANTALLA, &Wire, -1);

const byte ROWS = 4; // rows
const byte COLS = 4; // columns
//define the symbols on the buttons of the keypads
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'.','0','k','D'}
};
byte rowPins[ROWS] = {36, 34, 32,30}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {28, 26, 24, 22}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Adafruit_Keypad customKeypad = Adafruit_Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  // put your setup code here, to run once:

#ifdef __DEBUG__
  Serial.begin(9600);
  delay(100);
  Serial.println("Iniciando pantalla OLED");
#endif
 
  // Iniciar pantalla OLED en la dirección 0x3C
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
#ifdef __DEBUG__
    Serial.println("No se encuentra la pantalla OLED");
#endif
    while (true);
  }
// Color del texto
display.setTextColor(SSD1306_WHITE);
display.clearDisplay();
display.display();
customKeypad.begin();

  
  
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(46, OUTPUT);
  pinMode(44, OUTPUT);
  pinMode(42, OUTPUT);
  pinMode(40, OUTPUT);
  pinMode(52, OUTPUT);
  pinMode(50, OUTPUT);


}

void loop() {
  customKeypad.tick();
  display.display();
  switch (estado){
      case inicio:
        apagar_resistencia();
        digitalWrite(44, LOW); //A
        digitalWrite(42, LOW);  //B
        digitalWrite(40, LOW);  //C
        pantalla("Select mode",1,8,5);
        pantalla("A:Num->col\nB:Col->num\nC:Read",2,0,15);
          while(customKeypad.available()){
             keypadEvent e = customKeypad.read();
            if(e.bit.EVENT == KEY_JUST_PRESSED){
              display.clearDisplay();
              switch((char)e.bit.KEY){
                case 'A':
                  a='x';
                  pantalla("A",3,50,20);
                  display.display();
                  estado=modo_numero_a_color;
                  delay(1000);
                  display.clearDisplay();
                break;
                case 'B':
                  pantalla("B",3,50,20);
                  display.display();
                  estado=modo_color_a_numero;
                  cadena="";
                  apagar_resistencia();
                  a='x';
                  delay(1000);
                  display.clearDisplay();
                  pantalla("Enter Col & press D",1,0,0);
                  pantalla("0 Black     1 Brown\n2 Red       3 Orange\n4 Yellow    5 Green\n6 Blue      7 Purple\n8 Grey      9 White",1,0,10);
                break;
                case 'C':
                  pantalla("C",3,50,20);
                  display.display();
                  estado=modo_lee_resistencia;
                  delay(1000);
                  display.clearDisplay();
                break;
              }
            }
          }
      break;
      case modo_numero_a_color:                     ////  A /////////////////////////////
        digitalWrite(44, HIGH); //A
        digitalWrite(42, LOW);  //B
        digitalWrite(40, LOW);  //C

        pantalla("Introduce Value",1,0,0);
        
        if(a!='D'){
          
          while(customKeypad.available()){
             keypadEvent e = customKeypad.read();
             // Limpiar buffer
            if(e.bit.EVENT == KEY_JUST_PRESSED){
              if(iA==0){
                display.clearDisplay();
                pantalla("Introduce Value",1,0,0);
              }
              a=(char)e.bit.KEY;
              caracteres[iA]=a;
              iA++;
              if(a!='D'){
                cadena=cadena+(char)e.bit.KEY;
                pantalla(cadena,2,10,15);
              }else{
                cadena=cadena + " ohm";
                pantalla(cadena,2,10,15);
                apagar_resistencia(); //retardo antes de montrar
                delay(500);
              }
            }
          }
        }else{
          valor_a_comercial(caracteres);
          mostrar_resistencia(caracteres);
          cadena=""; 
          iA=0;
          while(customKeypad.available()){
             keypadEvent e = customKeypad.read();
             // Limpiar buffer
            if(e.bit.EVENT == KEY_JUST_PRESSED){
              display.clearDisplay();
              switch((char)e.bit.KEY){
                case 'A':
                  display.clearDisplay();
                  apagar_resistencia();
                  a='x';
                  pantalla("Introduce Value",1,0,0);
                break;
                case 'B':
                  pantalla("B",3,50,20);
                  display.display();
                  estado=modo_color_a_numero;
                  apagar_resistencia();
                  cadena="";
                  a='x';
                  delay(1000);
                  display.clearDisplay();
                  pantalla("Enter Col & press D",1,0,0);
                  pantalla("0 Black     1 Brown\n2 Red       3 Orange\n4 Yellow    5 Green\n6 Blue      7 Purple\n8 Grey      9 White",1,0,10);
                break;
                case 'C':
                  pantalla("C",3,50,20);
                  display.display();
                  estado=modo_lee_resistencia;
                  apagar_resistencia();
                  delay(1000);
                  display.clearDisplay();
                break;
                case 'D':
                  estado = inicio;
                  display.clearDisplay();
                  delay(500);
                break;
                default:
                  if(iA==0){
                    display.clearDisplay();
                    pantalla("Introduce Value",1,0,0);
                  }
                  a=(char)e.bit.KEY;
                  caracteres[iA]=a;
                  iA++;
                  if(a!='D'){
                    cadena=cadena+(char)e.bit.KEY;
                    pantalla(cadena,2,10,15);
                  }else{
                    cadena=cadena + " ohm";
                    pantalla(cadena,2,10,15);
                  }
                break;
              }
            }
          }
        }
        
       
      
      break;
      case modo_color_a_numero:     ///// B ///////////////////////////////
        digitalWrite(44, LOW); //A
        digitalWrite(42, HIGH);  //B
        digitalWrite(40, LOW);  //C
        
        if(a!='D'){
          
          while(customKeypad.available()){
             keypadEvent e = customKeypad.read();
             // Limpiar buffer
            if(e.bit.EVENT == KEY_JUST_PRESSED){
              a=(char)e.bit.KEY;
              colorea_banda(int(a)-48,iB+1);
              if(a!='D'){
                if(iB==2){
                  for(int j=0;j<int(a)-48;j++){
                      cadena=cadena+'0'; 
                  }
                  if(int(a)-48>=3 && int(a)-48<6){
                    cadena.remove(cadena.length()-3,3);
                    cadena=cadena + " k";
                  }else if(int(a)-48>=6){
                    cadena.remove(cadena.length()-6,6);
                    cadena=cadena + " M";
                  }else{
                    cadena=cadena + " ";
                  }
                }else if(iB==3){
                  if(int(a)-48==amarillo){
                      cadena=cadena+"ohm\n\n        %5";
                  }else if(int(a)-48==gris){
                      cadena=cadena+"ohm\n\n       %10";
                  }else if(int(a)-48==rojo){
                      cadena=cadena+"ohm\n\n        %2";
                  }else{
                    cadena=cadena+"";
                  }
                }else{
                  cadena=cadena+a;
                }
                iB++;
              }
            }
          }
        }else{
          digitalWrite(46, HIGH); //blanca
          iB=0;
          display.clearDisplay();
          pantalla(cadena,2,0,15);
          
          while(customKeypad.available()){
             keypadEvent e = customKeypad.read();
             // Limpiar buffer
            if(e.bit.EVENT == KEY_JUST_PRESSED){
              display.clearDisplay();
              switch((char)e.bit.KEY){
                case 'A':
                  pantalla("A",3,50,20);
                  display.display();
                  a='x';
                  cadena=""; 
                  iA=0;
                  apagar_resistencia();
                  estado=modo_numero_a_color;
                  delay(1000);
                  display.clearDisplay();
                break;
                case 'B':
                  apagar_resistencia();
                  a='x';
                  cadena="";
                  display.clearDisplay();
                  pantalla("Enter Col & press D",1,0,0);
                  pantalla("0 Black     1 Brown\n2 Red       3 Orange\n4 Yellow    5 Green\n6 Blue      7 Purple\n8 Grey      9 White",1,0,10);
                break;
                case 'C':
                  pantalla("C",3,50,20);
                  display.display();
                  apagar_resistencia();
                  estado=modo_lee_resistencia;
                  delay(1000);
                  display.clearDisplay();
                break;
                case 'D':
                  estado = inicio;
                  display.clearDisplay();
                  delay(500);
                break;
                default:
                  
                break;
              }
            }
          }
        }
      break;
      case modo_lee_resistencia:      ///// C ///////////////////////////////
        digitalWrite(44, LOW); //A
        digitalWrite(42, LOW);  //B
        digitalWrite(40, HIGH);  //C
        
        if(millis()-tiempo< PERIODO){
            if(control){ //lee potenciometro
              digitalWrite(52, HIGH);
              digitalWrite(50, LOW);
              valor_pot=analogRead(A2)/4;
              r_pot_muestra=(22000*(valor_pot*5/255))/(5-(valor_pot*5/255));
              r_pot=r_pot + r_pot_muestra;
              n_muestras_pot++;
            }else{  //lee bornas
              digitalWrite(52, LOW);
              digitalWrite(50, HIGH);
              valor_res=analogRead(A7)/4;
              r_res_muestra=(255*(65000-r_pot)/valor_res) - (65000-r_pot); //calibrar ///////////////////////////////////////////////////
              r_res=r_res + r_res_muestra;
              n_muestras++;
            }
            
          
        }else{
          control=!control;
          tiempo=millis(); 
          r_res=r_res/n_muestras;
          r_pot=r_pot/n_muestras_pot;
          //display.clearDisplay();
          if(control){
            if(abs(r_res-r_res_ant)>10 || abs(r_pot-r_pot_ant)>10){
                display.clearDisplay();
                pantalla("Rango (ohm)",1,0,0);
                if(valor_pot<50){
                   pantalla("100-1k",2,0,10);
                }else if(valor_pot<100){
                   pantalla("1k-10k",2,0,10);
                }else if(valor_pot<150){
                    pantalla("10k-100k",2,0,10);
                }else if(valor_pot<200){
                    pantalla("100k-1M",2,0,10);
                }
  
                if(r_res>1000000){
                  pantalla("INF",2,0,40);
                  apagar_resistencia();
                }else if(r_res==0){
                  pantalla("0",2,0,40);
                  apagar_resistencia();
                }else{
                  pantalla(String(r_res)+" ohm",2,0,40);
                  num_to_array(to_comercial(r_res),caracteres);
                  mostrar_resistencia(caracteres);
                }
                
              }
              r_res_ant=r_res;
              r_pot_ant=r_pot;
              r_res=0;
              n_muestras=0;
              r_pot=0;
              n_muestras_pot=0;
          }
        }
        
          while(customKeypad.available()){
             keypadEvent e = customKeypad.read();
             // Limpiar buffer
            if(e.bit.EVENT == KEY_JUST_PRESSED){
              display.clearDisplay();
              switch((char)e.bit.KEY){
                case 'A':
                  a='x';
                  apagar_resistencia();
                  pantalla("A",3,50,20);
                  display.display();
                  estado=modo_numero_a_color;
                  delay(1000);
                  display.clearDisplay();
                break;
                case 'B':
                  pantalla("B",3,50,20);
                  display.display();
                  estado=modo_color_a_numero;
                  apagar_resistencia();
                  a='x';
                  cadena="";
                  delay(1000);
                  display.clearDisplay();
                  pantalla("Enter Col & press D",1,0,0);
                  pantalla("0 Black     1 Brown\n2 Red       3 Orange\n4 Yellow    5 Green\n6 Blue      7 Purple\n8 Grey      9 White",1,0,10);
                break;
                case 'C':
                  pantalla("C",3,50,20);
                  display.display();
                  estado=modo_lee_resistencia;
                  delay(1000);
                  display.clearDisplay();
                break;
                case 'D':
                  estado = inicio;
                  display.clearDisplay();
                  delay(500);
                break;
                default:
                  
                break;
              }
          }
        }
      break;
      default:
      break; 
  }

}

void pantalla(String cadena,unsigned tam,unsigned x,unsigned y){
  // Tamaño del texto
  display.setTextSize(tam);
  //Pos
  display.setCursor(x, y);
  // Escribir texto
  display.println(cadena);

  //display.display();
}

void mostrar_resistencia(char valor[BUFFER]){ //1kD i=1  x=0  cont=3
  unsigned i=0, x=0;
  int cont=0;
  while(valor[i]!='D'){
    if(i<2+x && valor[i]!='.' && valor[i]!='k'){
      colorea_banda(int((valor[i]))-48,i+1-x);
    }else if(valor[i]=='0'){
       cont++;
    }else if(valor[i]=='k'){
       cont=cont+3;
    }
    else if(valor[i]=='.'){
      x++;
      cont=cont-1;
    }
    i++;
  }  
  colorea_banda(cont,3);
  colorea_banda(amarillo,4);
  digitalWrite(46, HIGH); //blanca
}

void colorea_banda(unsigned color,unsigned banda){
    unsigned array_color[3]; 
    rellena_color(array_color,color);
    switch (banda){
      case BANDA1:
        analogWrite(11,array_color[0]); //rojo 
        analogWrite(12,array_color[1]); //verde 
        analogWrite(13,array_color[2]); //azul
      break;
      case BANDA2:
        analogWrite(8,array_color[0]); //rojo 
        analogWrite(9,array_color[1]); //verde 
        analogWrite(10,array_color[2]); //azul
      break;
      case BANDA3:
        analogWrite(5,array_color[0]); //rojo 
        analogWrite(6,array_color[1]); //verde 
        analogWrite(7,array_color[2]); //azul
      break;
      case BANDA4:
        analogWrite(2,array_color[0]); //rojo 
        analogWrite(3,array_color[1]); //verde 
        analogWrite(4,array_color[2]); //azul
      break;
    }
}

void rellena_color(unsigned array_color[3], unsigned color){
    switch(color){
      case negro:
        array_color[0]=0;
        array_color[1]=0;
        array_color[2]=0;
      break;
      case marron:
        array_color[0]=30;
        array_color[1]=10;
        array_color[2]=0;
      break;
      case rojo:
        array_color[0]=255;
        array_color[1]=0;
        array_color[2]=0;
      break;
      case naranja:
        array_color[0]=255;
        array_color[1]=100;
        array_color[2]=0;
      break;
      case amarillo:
        array_color[0]=255;
        array_color[1]=255;
        array_color[2]=0;
      break;
      case verde:
        array_color[0]=0;
        array_color[1]=255;
        array_color[2]=0;
      break;
      case azul:
        array_color[0]=0;
        array_color[1]=0;
        array_color[2]=255;
      break;
      case violeta:
        array_color[0]=50;
        array_color[1]=0;
        array_color[2]=120;
      break;
      case gris:
        array_color[0]=10;
        array_color[1]=10;
        array_color[2]=10;
      break;
      case blanco:
        array_color[0]=255;
        array_color[1]=255;
        array_color[2]=255;
      break;
      default:
        array_color[0]=0;
        array_color[1]=0;
        array_color[2]=0;
      break;
    }
}

void apagar_resistencia(){
  digitalWrite(46, LOW); //blanca
    // BANDA 4
  analogWrite(2,0); //rojo 
  analogWrite(3,0); //verde 
  analogWrite(4,0); //azul 

// BANDA 3
  analogWrite(5,0); //rojo 
  analogWrite(6,0); //verde 
  analogWrite(7,0); //azul 
  
// BANDA 2
  analogWrite(8,0); //rojo 
  analogWrite(9,0); //verde 
  analogWrite(10,0); //azul 

// BANDA 1
  analogWrite(11,0); //rojo 
  analogWrite(12,0); //verde 
  analogWrite(13,0); //azul 

}

void valor_a_comercial(char valor[BUFFER]){
    int comerciales[14]={10,12,15,18,22,27,33,39,47,51,56,68,82,100};
    int valor_comercial;
    char copia[BUFFER];
    String cadena="";
    int z=0;
    int i=2;
    char sig;
    cadena=cadena + valor[0];
    if(valor[1]=='.'){
      cadena=cadena + valor[2];
    }else{
      cadena=cadena + valor[1];
    }
    valor_comercial=comerciales[binary_lookup(comerciales,cadena.toInt(),0,13)]; ///////cambio
    valor[0]=char((valor_comercial/10) + 48);
    if(valor[1]=='.'){
      valor[2]=char((valor_comercial%10) + 48);
      if(valor_comercial==100){
        i=1;
        while(valor[i+1]!='D'){ // 9.9kD -> 10kkD -> 10kD
          valor[i]=valor[i+1];
          i++;
        }
        valor[i]=valor[i+1];//10kDD
      }
    }else{
      valor[1]=char((valor_comercial%10) + 48);
      if(valor_comercial==100){
        for(int j=0;j<BUFFER;j++){
          copia[j]=valor[j];
        }
        i=2;
        while(copia[i]!='D'){
          valor[i+1]=copia[i];
          i++;
        }
        valor[i+1]=copia[i];
        valor[2]='0';
      }
    }
    
    cadena="";
    while(valor[z]!='D'){
      cadena=cadena+valor[z];
      z++;
    }
    cadena=cadena+ " ohm";
    pantalla("Commercial Value",1,0,35);
    pantalla(cadena,2,10,50);
    
}

int to_comercial(int valor){
  unsigned long mult=0;
  int comerciales[14]={10,12,15,18,22,27,33,39,47,51,56,68,82,100};
  if(valor<100){
      mult=1;
  }else if(valor<1000){
      mult=10;
  }else if(valor<10000){
      mult=100;
  }else if(valor<100000){
      mult=1000;
  }else if(valor<1000000){
      mult=10000;
  }
  valor=valor/mult;
  valor=comerciales[binary_lookup(comerciales,valor,0,13)]*mult;
  return valor;
}

void num_to_array(int long n, char res[BUFFER]){
  unsigned long mult=0;
  int i=2,m=10;
  if(n<100){
      mult=10;
  }else if(n<1000){
      mult=100;
  }else if(n<10000){
      mult=1000;
  }else if(n<100000){
      mult=10000;
  }else if(n<1000000){
      mult=100000;
  }
  res[0]=char (n/mult) + 48;
  res[1]=char(((n%mult)/(mult/10)))+48;
  while(m<mult){
    res[i]='0';
    m=m*10;
    i++;
  }
  res[i]='D';
}

unsigned short binary_lookup(int *A, int key, int imin, int imax)
{
   int pos=0;
  int w=0;
  int diferencia,diferencia_ant=100;

  while (w<=imax)
    {
      diferencia=abs(key-A[w]);
      if(diferencia<diferencia_ant){
        pos=w;
      }
      diferencia_ant=diferencia;
      w++;
    }
    return pos;    //Al final imax=imin y en dicha posicion hay un numero mayor o igual que el buscado
}
