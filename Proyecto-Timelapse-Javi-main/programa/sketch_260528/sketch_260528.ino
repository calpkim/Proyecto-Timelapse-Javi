// Control motor según posición resistencia

#include <Stepper.h>

// número de pasos en una rotación completa
const int pasosPorVuelta = 200;  

// le indica al Arduino en qué pines está enchufado el controlador
Stepper controlador(pasosPorVuelta, 8, 9, 10, 11);

void ponerMotorEnNeutro() {
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);
  digitalWrite(10, LOW);
  digitalWrite(11, LOW);
}

int comprendidoEntre(int v, int a, int b) {
  if ( (v >= a) && (v < b) ) {
    return 1;
  }
  return 0;
}

// Convierte la posición del potenciómetro (entre 0 y 1023)
// a un selector de modo
int selectorSegunPotenciometro(int potenciometro) {
    
  if (comprendidoEntre(potenciometro, 0, 400)) {
      return 1;
  }
  if (comprendidoEntre(potenciometro, 400, 800)) {
      return 2;
  }
  if (comprendidoEntre(potenciometro, 800, 1024)) {
      return 3;
  }
    
  return 0;
}

// Modos
// - Modo parado
// - Modo radio
// - Modo regreso
// - Modo temporizador

// Configura el puerto serie a 9600 baudios
void setup() {
  Serial.begin(9600);
}


void loop() {

  int posicionResistencia = analogRead(A0);
  Serial.print("Resistencia: ");
  Serial.println(posicionResistencia);
  
  int modo = selectorSegunPotenciometro(posicionResistencia);
  Serial.print("Modo elegido: ");
  Serial.println(modo);
  
  int velocidadRpm = 0;
  int vueltas = 0;
  int pausa = 1; // en segundos
  
  switch (modo) {
      case 1:
          velocidadRpm = 30;
          vueltas = 1;
          pausa = 1;
          break;

      case 2:
          velocidadRpm = 50;
          vueltas = 1;
          pausa = 5;
          break;

      case 3:
          velocidadRpm = 100;
          vueltas = 5;
          pausa = 1;
          break;
          

      default:
          Serial.println("Error de programación: modo desconocido");
  }

  Serial.print("Velocidad: "); Serial.println(velocidadRpm);
    
  Serial.print("Vueltas: "); Serial.println(vueltas);
    
  Serial.print("Pausa: "); Serial.println(pausa);
    
  controlador.setSpeed(velocidadRpm);
  controlador.step(pasosPorVuelta * vueltas);
  ponerMotorEnNeutro();
  delay(pausa * 1000);

  Serial.println("Fin pausa, loop nuevo");

}
