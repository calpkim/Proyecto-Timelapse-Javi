// Control motor según posición resistencia - Límites personalizables en mm por modo

#include <Stepper.h>

const int pasosPorVuelta = 200; 
Stepper controlador(pasosPorVuelta, 8, 9, 10, 11);

// Variable global para contar el progreso del viaje
long pasosAcumulados = 0; 

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

  // Declaramos las variables que va a rellenar el switch
  int velocidadRpm = 0;
  int pasosPorCiclo = 0;        
  int pausa = 1;        
  long distanciaFinalMm = 0; // <-- Nueva variable para definir el fin en cada caso
  
  switch (modo) {
      case 1:
          velocidadRpm = 100;   
          pasosPorCiclo = 50;         // Avanza 10 mm (1 cm) por paso
          pausa = 10;                 // Espera 10 segundos
          distanciaFinalMm = 2000;    // <-- AQUÍ CONFIGURAS TU FINAL EN MM (2000 mm = 200 cm)
          break;

      case 2:
          velocidadRpm = 30;
          pasosPorCiclo = 20;       
          pausa = 6;          
          distanciaFinalMm = 5000;    // Ejemplo: Final de recorrido a los 100 cm (1000 mm)
          break;

      case 3:
          velocidadRpm = 30;
          pasosPorCiclo = 50;      
          pausa = 10;          
          distanciaFinalMm = 500;     // Ejemplo: Final de recorrido a los 50 cm (500 mm)
          break;
          
      default:
          Serial.println("Error de programación: modo desconocido");
  }

  // --- CONTROL DE FIN DE SECUENCIA AUTOMÁTICO ---
  // Traducimos los mm configurados a pasos totales (1mm = 5 pasos)
  long limitePasosTotales = distanciaFinalMm * 5;

  if (pasosAcumulados >= limitePasosTotales) {
      Serial.println("--- SECUENCIA FINALIZADA ---");
      Serial.print("Se ha alcanzado el limite de: "); Serial.print(distanciaFinalMm); Serial.println(" mm");
      ponerMotorEnNeutro(); // Desconectamos corriente al acabar del todo
      delay(5000); 
      return; 
  }

  // Sumamos el avance actual al progreso acumulado
  pasosAcumulados = pasosAcumulados + pasosPorCiclo;
  
  // Monitorización por el puerto serie
  Serial.print("Velocidad: "); Serial.println(velocidadRpm);
  Serial.print("Pasos a dar: "); Serial.println(pasosPorCiclo); 
  Serial.print("Progreso: "); Serial.print(pasosAcumulados / 5.0); Serial.print(" / "); Serial.print(distanciaFinalMm); Serial.println(" mm");
  Serial.print("Pausa: "); Serial.println(pausa);
    
  // Movimiento mecánico
  controlador.setSpeed(velocidadRpm);
  controlador.step(pasosPorCiclo);  
  
  delay(pausa * 1000); 
  Serial.println("Fin pausa, loop nuevo");

}