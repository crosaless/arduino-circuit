#include <IRremote.hpp>
#include <Servo.h>
#include <LiquidCrystal.h>


// Constantes y variables globales
const unsigned long BTN_ONOFF = 0xFF00BF00;
const unsigned long BTN_1 = 0xEF10BF00;
const unsigned long BTN_2 = 0xEE11BF00;
const int esperaLarga = 1000;
const int IR_RECEIVE_PIN = 7;
const int PIN_SERVO = 2;


bool sistemaEncendido = false;
int posicionActual = 0;


// Configuración de LED y servo
Servo servo;
const int LED_VERDE = 5;
const int LED_ROJO = 4;
const int LED_AMARILLO = 3;


// Configuración del LCD
LiquidCrystal lcd(8, 9, 10, 11, 12, 6);


void setup() {
    Serial.begin(9600);
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);


    // Inicialización de LCD y LEDs
    lcd.begin(16, 2);
    pinMode(LED_VERDE, OUTPUT);
    pinMode(LED_ROJO, OUTPUT);
    pinMode(LED_AMARILLO, OUTPUT);


    // Configuración del servo
    servo.attach(PIN_SERVO, 500, 2500);
    servo.write(0);
}


void encenderLuces(bool verde, bool rojo, bool amarillo) {
    digitalWrite(LED_VERDE, verde ? HIGH : LOW);
    digitalWrite(LED_AMARILLO, amarillo ? HIGH : LOW);
    digitalWrite(LED_ROJO, rojo ? HIGH : LOW);
}


void rotacion(int posicionFinal) {
    int incremento = (posicionActual < posicionFinal) ? 1 : -1;
    for (int pos = posicionActual; pos != posicionFinal; pos += incremento) {
        servo.write(pos);
        delay(15);
    }
    posicionActual = posicionFinal;
}


void mostrarMensaje(const String& mensaje1, const String& mensaje2 = "") {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(mensaje1);
    if (mensaje2.length() > 0) {
        lcd.setCursor(0, 1);
        lcd.print(mensaje2);
    }
}


void encenderSistema() {
    encenderLuces(true, false, true);
    mostrarMensaje("Sistema:", "ON");
    delay(esperaLarga);
    mostrarMensaje("Bienvenidos");
    delay(esperaLarga);
    mostrarMensaje("1-Serv a 90°", "2-Serv a 0°");
}


void apagarSistema() {
    rotacion(0);
    encenderLuces(false, false, false);
    mostrarMensaje("Sistema:", "OFF");
    delay(esperaLarga);
}


void accion1() {
    encenderLuces(true, false, true);
    rotacion(90);
    mostrarMensaje("Servo a 90°");
}


void accion2() {
    encenderLuces(false, true, false);
    mostrarMensaje("Led rojo encendido");
    delay(esperaLarga);
    mostrarMensaje("Servo a 180°");
    rotacion(180);
}


void loop() {
    if (IrReceiver.decode()) {
        unsigned long codigoIR = IrReceiver.decodedIRData.decodedRawData;
        Serial.println(codigoIR, HEX);


        switch (codigoIR) {
            case BTN_ONOFF:
                sistemaEncendido = !sistemaEncendido;
                sistemaEncendido ? encenderSistema() : apagarSistema();
                break;


            case BTN_1:
                if (sistemaEncendido) accion1();
                break;


            case BTN_2:
                if (sistemaEncendido) accion2();
                break;


            default:
                if (sistemaEncendido) {
                    mostrarMensaje("Comando no", "reconocido");
                }
                break;
        }


        IrReceiver.resume();
    }
}
