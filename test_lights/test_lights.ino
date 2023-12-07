struct PinPair {
  uint8_t in;
  uint8_t out;
};

static const struct PinPair PIN_PAIRS[] = {
  {14, 31},
  {15, 33},
  {16, 35}, 
  {17, 37},
  {18, 39},
  {19, 41},
};


void setup() {
  // put your setup code here, to run once:
  for (size_t i=0; i<6; i++) {
    uint8_t out_pin = PIN_PAIRS[i].out;
    uint8_t in_pin = PIN_PAIRS[i].in;
    pinMode(out_pin, OUTPUT);
    digitalWrite(out_pin, LOW);
    pinMode(in_pin, INPUT_PULLUP);
  }
}

void loop() {
  for (size_t i=0; i<6; i++) {
    uint8_t in_pin = PIN_PAIRS[i].in;
    uint8_t out_pin = PIN_PAIRS[i].out;
    if (digitalRead(in_pin) == LOW) {
      digitalWrite(out_pin, HIGH);  
    } else {
      digitalWrite(out_pin, LOW);
    }
  }
  delay(1);
}
