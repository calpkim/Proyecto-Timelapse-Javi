// Control motor según posición resistencia

#include <Stepper.h>

// número de pasos en una rotación completa (por ejemplo, para un NEMA 17)
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

// Convierte la posición del potenciómetro (entre 0 y 1023) a un selector de modo
int selectorSegunPotenciometro(int potenciometro) {
    
  if (comprendidoEntre(potenciometro, 0, 400)) {
      return 1;
  }
  // CORREGIDO: Ahora dice potenciometro correctamente
  if (comprendidoEntre(potenciometro, 400, 800)) {
      return 2;
  }
  if (comprendidoEntre(potenciometro, 800, 1024)) {
      return 3;
  }
    
  return 0;
}

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
  int pasos = 0;        
  int pausa = 1;        // en segundos
  
  switch (modo) {
      case 1:
          velocidadRpm = 30;
          pasos = 50;        
          pausa = 1;
          break;

      case 2:
          velocidadRpm = 20;
          pasos = 50;       
          pausa = 6;
          break;

      case 3:
          velocidadRpm = 100;
          pasos = 1000;      
          pausa = 1;
          break;
          
      default:
          Serial.println("Error de programación: modo desconocido");
  }

  Serial.print("Velocidad: "); Serial.println(velocidadRpm);
  Serial.print("Pasos a dar: "); Serial.println(pasos); 
  Serial.print("Pausa: "); Serial.println(pausa);
    
  controlador.setSpeed(velocidadRpm);
  controlador.step(pasos);  
  
  ponerMotorEnNeutro();
  delay(pausa * 1000);
  Serial.println("Fin pausa, loop nuevo");

}