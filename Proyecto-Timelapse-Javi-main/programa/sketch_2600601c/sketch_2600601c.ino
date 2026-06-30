// Control motor según posición resistencia - Versión de BAJO CONSUMO (sin retención)

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

  int velocidadRpm = 0;
  int pasosPorCiclo = 0;        
  int pausa = 1;        
  long distanciaFinalMm = 0; 
  
  switch (modo) {
      case 1:
          velocidadRpm = 12;   
          pasosPorCiclo = 40;         // Avanza 10 mm (1 cm) por ciclo
          pausa = 5;                 // Espera 10 segundos
          distanciaFinalMm = 2000;    // Recorrido de 200 cm (2000 mm)
          break;

      case 2:
          velocidadRpm = 30;
          pasosPorCiclo = 50;       
          pausa = 10;          
          distanciaFinalMm = 1000;    // Recorrido de 100 cm (1000 mm)
          break;

      case 3:
          velocidadRpm = 30;
          pasosPorCiclo = 50;      
          pausa = 10;          
          distanciaFinalMm = 500;     // Recorrido de 50 cm (500 mm)
          break;
          
      default:
          Serial.println("Error de programación: modo desconocido");
  }

  // CONTROL DE FIN DE SECUENCIA AUTOMÁTICO
  long limitePasosTotales = distanciaFinalMm * 5;

  if (pasosAcumulados >= limitePasosTotales) {
      Serial.println("--- SECUENCIA FINALIZADA ---");
      Serial.print("Se ha alcanzado el limite de: "); Serial.print(distanciaFinalMm); Serial.println(" mm");
      ponerMotorEnNeutro(); // Se apaga la corriente definitivamente al terminar
      delay(5000); 
      return; 
  }

  // Acumulación del avance
  pasosAcumulados = pasosAcumulados + pasosPorCiclo;
  
  // Monitor Serie
  Serial.print("Velocidad: "); Serial.println(velocidadRpm);
  Serial.print("Pasos a dar: "); Serial.println(pasosPorCiclo); 
  Serial.print("Progreso: "); Serial.print(pasosAcumulados / 5.0); Serial.print(" / "); Serial.print(distanciaFinalMm); Serial.println(" mm");
  Serial.print("Pausa: "); Serial.println(pausa);
    
  // Movimiento
  controlador.setSpeed(velocidadRpm);
  controlador.step(pasosPorCiclo);  
  
  // CORRECCIÓN: Activamos el neutro para cortar la corriente en cada pausa
  ponerMotorEnNeutro(); 
  
  delay(pausa * 1000); 
  Serial.println("Fin pausa, loop nuevo");

}