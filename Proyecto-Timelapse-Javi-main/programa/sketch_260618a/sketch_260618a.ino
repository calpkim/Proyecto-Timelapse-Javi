// =========================================================================
// PLAN B: MÁXIMO PAR DE TORSIÓN (FUERZA BRUTA) + 2S DESCANSO
// =========================================================================

const int pinPulso = 8;          // Cable de pulsos en D8
const int pasosPorVuelta = 400;  // Switches en ON-OFF-ON

void setup() {
  pinMode(pinPulso, OUTPUT);
}

void loop() {
  // --- FASE DE GIRO: Máxima fuerza magnética por paso ---
  // Usamos un retraso de 4000 microsegundos por semiciclo.
  // Esto mantiene la bobina energizada el tiempo óptimo para el máximo torque.
  for (int i = 0; i < pasosPorVuelta; i++) {
    digitalWrite(pinPulso, HIGH);
    delayMicroseconds(4000); 
    digitalWrite(pinPulso, LOW);
    delayMicroseconds(4000); 
  }

  // --- FASE DE DESCANSO: 2 Segundos detenido ---
  delay(2000); 
}