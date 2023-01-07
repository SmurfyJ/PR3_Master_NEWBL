#include "Arduino.h"
#include <util/twi.h>

void ERROR();

uint16_t data_in = 0;
unsigned long timestamp = 0;

int main() {

    init();
    Serial.begin(9600);

    // internal Pull Up
    PORTC |= (1 << PORTC4) | (1 << PORTC5);

    // Uebertragungsgeschwindigkeit
    TWBR = 72;

    while (1) {

        if (millis() > (timestamp + 1000)) {

            timestamp = millis();

            // Start senden
            TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

            // Warten
            while (!(TWCR & (1 << TWINT)));

            // Status ueberpruefen: Start wurde gesendet
            if (TW_STATUS != TW_START) ERROR();

            // Adresse senden
            byte SLA_W = 2;
            TWDR = SLA_W << 1;
            TWDR |= (1 << 0); // Master Receiver - Slave Transmitter (Write=1)
            TWCR = (1 << TWINT) | (1 << TWEN);

            // Warten
            while (!(TWCR & (1 << TWINT)));

            // Status ueberpruefen: Slave hat Adresse bestaetigt
            if (TW_STATUS != TW_MR_SLA_ACK) ERROR();

            // Empfang starten, ACK fuer Daten vorbereitet (weil nicht letztes Paket)
            TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);

            // Warten
            while (!(TWCR & (1 << TWINT)));

            // Status ueberpruefen: Daten empfangen
            if (TW_STATUS != TW_MR_DATA_ACK) ERROR();

            // Highbyte auslesen
            data_in = (TWDR << 8);

            // Empfang starten, NACK fuer Daten vorbereitet (weil letztes Paket)
            TWCR = (1 << TWINT) | (1 << TWEN);

            // Warten
            while (!(TWCR & (1 << TWINT)));

            // Status ueberpruefen: Daten empfangen
            if (TW_STATUS != TW_MR_DATA_NACK) ERROR();

            // Lowbyte auslesen
            data_in |= TWDR;

            // Stop senden
            TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);

            Serial.print("Daten: ");
            Serial.println(data_in);
            Serial.println("----------");

        }

    }
}

void ERROR() {
    Serial.print("TWCR: ");
    Serial.println(TWCR, BIN);
    Serial.print("TW_STATUS: ");
    Serial.println(TW_STATUS, HEX);
}