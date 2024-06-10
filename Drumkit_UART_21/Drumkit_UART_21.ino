#define NB_PADS 5

#define button_hh 2

int button[NB_PADS] = {3, 4, 5, 13, 6};

int state[NB_PADS] = {0};

int state_snare = 0;

int treshold = 70;

float sens = 1000;

byte pad = 0;

byte volume = 0;

void setup() {
  for(pad = 0; pad < NB_PADS; pad ++) {
    pinMode(button[pad], INPUT_PULLUP);
  }

  pinMode(button_hh, INPUT_PULLUP);

  Serial.begin(115200);

}

void loop() {
  volume = 12;
  for(pad = 0; pad < NB_PADS; pad ++) {
    // Front detection
    if(digitalRead(button[pad]) == LOW && state[pad] == 0) {
      // HH special case : closed
      if(pad == 4 && digitalRead(button_hh) == LOW) { 
        // Pad + volume concat : [MSB : volume; LSB : pad]
        Serial.write(pad+1 + (volume << 4));
      } else {
        Serial.write(pad + (volume << 4));
      }
      state[pad] = 1;
    }
    // Release detection
    if(digitalRead(button[pad]) == HIGH && state[pad] == 1) {
      state[pad] = 0;
    }
  }
  
  volume = analogRead(A5);
  if(volume > treshold && state_snare == 0) {
    volume *= sens;
    volume  /= 64; // 4 bits scaling
    state_snare  = 1;
    Serial.write(1 + (volume << 4));
  }
  if(volume < treshold && state_snare == 1) {
    state_snare = 0;
  }

  delayMicroseconds(250);
}

