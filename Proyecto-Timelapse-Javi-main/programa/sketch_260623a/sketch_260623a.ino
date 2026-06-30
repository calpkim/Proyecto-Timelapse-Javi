// =========================================================================
// Control motor según posición resistencia - Versión de BAJO CONSUMO (TB6600)
// Proyecto: Timelapse sobre raíles
// =========================================================================

// Definición de pines fijos para el TB6600 (Señales Negativas)
const int pinPulso = 8;      // PUL- conectado a D8
const int pinDireccion = 9;  // DIR- conectado a D9
const int pinEnable = 10;    // ENA- conectado a D10

// Configuración física: 1/4 de paso en el TB6600 = 400 pasos por vuelta.
const int pasosPorVuelta = 400; 

// Variable global para contar el progreso del viaje (en pasos)
long pasosAcumulados = 0; 

// --- FUNCIONES DE CONTROL DEL MOTOR ---

void ponerMotorEnNeutro() {
  // En lógica de ánodo común, un HIGH en ENA- corta la corriente de las bobinas.
  // El motor queda libre, no consume batería y no se calienta durante la pausa.
  digitalWrite(pinEnable, HIGH); 
}

void habilitarMotor() {
  // Un LOW activa el driver y energiza el motor dejándolo bloqueado y listo.
  digitalWrite(pinEnable, LOW); 
}

void darPasos(long pasos, int rpm) {
  if (pasos == 0) return;

  habilitarMotor(); // Activamos el motor antes de moverlo
  delay(10);        // Pequeño margen físico para que el driver se estabilice

  // Determinar sentido de giro según el signo de los pasos
  if (pasos > 0) {
    digitalWrite(pinDireccion, HIGH); // Avance / Sentido Horario
  } else {
    digitalWrite(pinDireccion, LOW);  // Retroceso / Sentido Antihorario
    pasos = -pasos;                   // Convertimos a positivo para el bucle for
  }

  // CÁLCULO FÍSICO DE VELOCIDAD:
  // Calculamos el tiempo en microsegundos que debe durar cada semiperiodo de la onda cuadrada.
  // Fórmula: microsegundos = (60 seg * 1,000,000 us) / (RPM * pasosPorVuelta * 2)
  long retrasoMicrosegundos = (60000000L) / ((long)rpm * pasosPorVuelta * 2);

  retrasoMicrosegundos = 3000;

  Serial.print("Retraso: ");
  Serial.print(retrasoMicrosegundos);
  Serial.println(" μs");

  /*
  // Generación del tren de pulsos (Onda cuadrada)
  for (long i = 0; i < pasos; i++) {
    digitalWrite(pinPulso, HIGH);
    delayMicroseconds(retrasoMicrosegundos);    
    digitalWrite(pinPulso, LOW);
    delayMicroseconds(retrasoMicrosegundos);
  }
  */

  Serial.println("Diez segundos, 600 pulsos. Debería resultar en 10 vueltas, a una por segundo");
  for (int s = 0; s < 10; s++) {
    Serial.print("Vuelta "); Serial.println(s);
    for (int i = 0; i < 600; i++) {
      digitalWrite(pinPulso, HIGH);
      delayMicroseconds(833);
      digitalWrite(pinPulso, LOW);
      delayMicroseconds(833);
    }
  }
}

// --- FUNCIONES DE LÓGICA Y SELECCIÓN ---

int comprendidoEntre(int v, int a, int b) {
  if ((v >= a) && (v < b)) {
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

// --- CONFIGURACIÓN INICIAL ---

void setup() {
  Serial.begin(9600);
  
  // Configurar los pines de control como salidas
  pinMode(pinPulso, OUTPUT);
  pinMode(pinDireccion, OUTPUT);
  pinMode(pinEnable, OUTPUT);
  
  // Iniciamos en reposo absoluto (Bajo consumo)
  ponerMotorEnNeutro();
  Serial.println("--- SISTEMA INICIALIZADO (ESPERANDO ENTRADA A0) ---");
}

// --- BUCLE PRINCIPAL ---

void loop() {

  int posicionResistencia = analogRead(A0);
  Serial.print("Resistencia: ");
  Serial.println(posicionResistencia);
  
  int modo = selectorSegunPotenciometro(posicionResistencia);
  Serial.print("Modo elegido: ");
  Serial.println(modo);

  int velocidadRpm = 0;
  long pasosPorCiclo = 0;        
  int pausa = 1;        
  long distanciaFinalMm = 0; 
  
  // Ajuste de pasos y distancias para resolución de 400 pasos/vuelta (Micropasos 1/4)
  switch (modo) {
      case 1:
          velocidadRpm = 20;   
          pasosPorCiclo = 1000;        
          pausa = 2;                
          distanciaFinalMm = 2000;  
          break;

      case 2:
          velocidadRpm = 40;
          pasosPorCiclo = 1000;       
          pausa = 2;          
          distanciaFinalMm = 1000;  
          break;

      case 3:
          velocidadRpm = 80;
          pasosPorCiclo = 1000;      
          pausa = 2;          
          distanciaFinalMm = 500; 
          break;
          
      default:
          Serial.println("Error: modo desconocido");
  }

  // CONTROL DE FIN DE SECUENCIA AUTOMÁTICO
  // Ajuste matemático: Al usar micropasos, el multiplicador pasa a ser 10 para mantener la escala real en mm
  long limitePasosTotales = distanciaFinalMm * 10;

  if (abs(pasosAcumulados) >= limitePasosTotales) {
      Serial.println("--- SECUENCIA FINALIZADA ---");
      Serial.print("Se ha alcanzado el limite de: "); Serial.print(distanciaFinalMm); Serial.println(" mm");
      ponerMotorEnNeutro(); // Apagado de seguridad definitivo
      delay(5000); 
      return; 
  }

  // Acumulación del progreso
  
  // TODO: debug
  // pasosAcumulados = pasosAcumulados + pasosPorCiclo;
  
  // Impresión de Telemetría en el Monitor Serie
  Serial.print("Velocidad: "); Serial.print(velocidadRpm); Serial.println(" RPM");
  Serial.print("Pasos a dar: "); Serial.println(pasosPorCiclo); 
  Serial.print("Progreso: "); Serial.print(abs(pasosAcumulados) / 10.0); Serial.print(" / "); Serial.print(distanciaFinalMm); Serial.println(" mm");
  Serial.print("Pausa activa: "); Serial.print(pausa); Serial.println(" seg");
    
  // Ejecutar el movimiento físico
  darPasos(pasosPorCiclo, velocidadRpm);
  
  // BAJO CONSUMO ELECTRÓNICO: Cortamos la energía inmediatamente después de movernos
  ponerMotorEnNeutro(); 
  
  // Tiempo muerto para la captura fotográfica del Timelapse
  delay(pausa * 1000); 
  Serial.println("------------------------------------");

}
