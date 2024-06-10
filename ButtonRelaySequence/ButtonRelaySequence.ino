/*
  Button to Relays sequence

  Cycle through 8 relays (0..7), when pressing a pushbutton attached to pin 8.
*/

const int buttonPin           = 8; // the number of the pushbutton pin
const int nbRelays            = 8; // the number of relays

const int relayPins[nbRelays] = {0, 1, 2, 3, 4, 5, 6, 7};  // Relays pins sequence

const int ledPin              = 13; // status led pin


int activeRelay = 0;

void setup() {
  // initialize the relays pins as outputs:
  for(int i = 0; i < nbRelays; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], HIGH);
  }

  // initialize led in as output
  pinMode(ledPin, OUTPUT);
  
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (digitalRead(buttonPin) == LOW) {
    // turn current relay on:
    digitalWrite(relayPins[activeRelay], LOW);
    // feedback on status led
    digitalWrite(ledPin, HIGH);
    delay(10);
    // digitalWrite(relayPins[activeRelay], HIGH);
    digitalWrite(ledPin, LOW);
    
    // wait for release
    while((digitalRead(buttonPin) == LOW)) {
      delay(10);
    }
    
    activeRelay++;
    
    if (activeRelay == nbRelays)
    {
      activeRelay = 0;
      delay(30000);
      for(int i = 0; i < nbRelays; i++) {
        digitalWrite(relayPins[i], HIGH);
      }
    }
  }
}
