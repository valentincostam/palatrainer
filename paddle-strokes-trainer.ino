// Pines
const int PULSADOR_SUBIR = 2; // Pulsador que aumenta el número de paladas por minuto.
const int PULSADOR_BAJAR = 3; // Pulsador que disminuye el número de paladas por minuto.
const int LED_VERDE      = 4; // LED de color azul.
const int LED_ROJO       = 5; // LED de color verde.
const int BUZZER         = 6; // Buzzer.

// Constantes
const int MAXIMO_PALADAS    = 150; // Número máximo de paladas por minuto.
const int MINIMO_PALADAS    = 30;  // Número mínimo de paladas por minuto.
const int FACTOR_PALADAS    = 5;   // Valor en el que aumenta/disminuye el número de paladas.
const int DURACION_DESTELLO = 200; // Número de milisegundos que dura el destello de los LEDs.

// Variables
unsigned long intervalo        = 1200;      // Milisegundos que debe haber entre las paladas.
unsigned int  paladas          = 50;        // Número de paladas por minuto incial.
unsigned long ultima_pulsacion = millis();  // Milisegundos que pasaron desde que se inició el programa hasta que se tocó un pulsador.
unsigned long ultimo_destello  = millis();  // Milisegundos que pasaron desde que se inició el programa hasta que se hizo alguna palada.
boolean display_on             = true;      // Indica si el display está prendido (1) o apagado (0).
boolean buzzer_on              = false;     // Indica si el buzzer está prendido (1) o apagado (0).
int led_on                     = LED_ROJO;  // Indica qué LED está prendido.
int led_off                    = LED_VERDE; // Indica qué LED está apagado.


/*
 * Inicializa los pines, el serial y el display.
 */
void setup() {
  // Define pines de entrada
  pinMode(PULSADOR_SUBIR, INPUT);
  pinMode(PULSADOR_BAJAR, INPUT);

  // Define pines de salida
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_ROJO,  OUTPUT);
  pinMode(BUZZER,    OUTPUT);
  
  // Inicializa los LEDs
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_ROJO,  HIGH);
    
  // Inicializa el serial
  Serial.begin(9600);
  Serial.println("Serial andando!");
} // Fin de setup()


/*
 * Actualiza el intervalo de tiempo que debe haber entre las paladas y el
 * número de paladas por minuto en el display.
 */
void actualizarRitmo() {
  intervalo = (long)(60 / (float)paladas * 1000);
   
  Serial.print("Paladas por minuto: ");
  Serial.println(paladas, DEC);
} // Fin de actualizarRitmo()


/*
 * Aumenta el número de paladas por minuto.
 */
void aumentarPaladas() {
  if (paladas == MAXIMO_PALADAS)
    paladas = MINIMO_PALADAS;
  else
    paladas += FACTOR_PALADAS;  

  actualizarRitmo();

  // Detiene el programa hasta que no se suelte el pulsador para evitar ingresar de nuevo a la función
  while (digitalRead(PULSADOR_SUBIR) == HIGH && digitalRead(PULSADOR_BAJAR) == LOW)
    delay(100);
} // Fin de aumentarPaladas()


/*
 * Disminuye el número de paladas por minuto.
 */
void disminuirPaladas() {
  if (paladas == MINIMO_PALADAS)
    paladas = MAXIMO_PALADAS;
  else
    paladas -= FACTOR_PALADAS;
  
  actualizarRitmo();

  // Detiene el programa hasta que no se suelte el pulsador para evitar ingresar de nuevo a la función
  while (digitalRead(PULSADOR_BAJAR) == HIGH && digitalRead(PULSADOR_SUBIR) == LOW)
    delay(100);
} // Fin de disminuirPaladas()


/*
 * Enciende y apaga los LEDs de acuerdo al intervalo de tiempo entre paladas.
 */
void destellar() {
  int led;

  if ((millis() - ultimo_destello) > DURACION_DESTELLO && digitalRead(led_on) == HIGH) {
    digitalWrite(led_on, LOW);
    if (buzzer_on)
      analogWrite(BUZZER, 0);
  }

  if ((millis() - ultimo_destello) >= intervalo) {
    digitalWrite(led_off, HIGH);

    if (buzzer_on)
      analogWrite(BUZZER, 210);
    
    led     = led_on;
    led_on  = led_off;
    led_off = led;

    ultimo_destello = millis();
  }
} // Fin de destellar()


/*
 * Detecta si un pulsador es presionado para aumentar/disminuir el número de
 * paladas por minuto.
 */
void detectarPulsadores() {
  boolean subir = digitalRead(PULSADOR_SUBIR) == HIGH && digitalRead(PULSADOR_BAJAR) == LOW;
  boolean bajar = digitalRead(PULSADOR_SUBIR) == LOW  && digitalRead(PULSADOR_BAJAR) == HIGH;
  boolean ambos = digitalRead(PULSADOR_SUBIR) == HIGH && digitalRead(PULSADOR_BAJAR) == HIGH;

  if (subir) {
    delay(100);

    if (subir) {
      aumentarPaladas();
      ultima_pulsacion = millis();
    }    
  }

  if (bajar) {
    delay(100);

    if (bajar) {
      disminuirPaladas();
      ultima_pulsacion = millis();
    }
  }

  if (ambos) {
    buzzer_on = !buzzer_on;
    
    if (buzzer_on)
      analogWrite(BUZZER, 210);
    else
      analogWrite(BUZZER, 0);

    Serial.print("Buzzer: ");
    Serial.println(buzzer_on, DEC);

    while (digitalRead(PULSADOR_SUBIR) == HIGH && digitalRead(PULSADOR_BAJAR) == HIGH)
      delay(100);
  }
}

/* 
 * Bucle que se repite infinitamente. Enciende de forma intermitente los LEDs y
 * detecta si algún pulsador fue presionado.
 */
void loop() {  
  destellar();
  detectarPulsadores();
} // Fin de loop ()
