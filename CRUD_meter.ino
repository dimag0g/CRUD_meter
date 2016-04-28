
// Configuration parameters
const int PIN_IN =   A0;
const int PIN_1K =   9;
const int PIN_10K =  11;
const int PIN_100K = 10;
const int PIN_1M =   12;

// Calibration parameters
const float REF_V =    3.5;  // [V]
const float REF_1K =   1.00; // [kOhm]
const float REF_10K =  10.0; // [kOhm]
const float REF_100K = 100;  // [kOhm]
const float REF_1M =   1000; // [kOhm]

void setup() {
  Serial.begin(9600); // PC config: stty -F /dev/ttyUSB0 9600 -cooked -crtscts
  while(!Serial);
  
  Serial.println();
  Serial.print("ADC on pin "); Serial.print(A0); Serial.print(", Vref = "); Serial.print(REF_V); Serial.println("V");  
  Serial.print("R = "); Serial.print(REF_1K,2); Serial.print("k on pin "); Serial.println(PIN_1K);
  Serial.print("R = "); Serial.print(REF_10K,1); Serial.print("k on pin "); Serial.println(PIN_10K);
  Serial.print("R = "); Serial.print(REF_100K,0); Serial.print("k on pin "); Serial.println(PIN_100K);
  Serial.print("R = "); Serial.print(REF_1M,0); Serial.print("k on pin "); Serial.println(PIN_1M);
  Serial.println("Available modes: C R U D");
  
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  pinMode(PIN_1K, INPUT);
  pinMode(PIN_10K, INPUT);
  pinMode(PIN_100K, INPUT);
  pinMode(PIN_1M, INPUT);
}

int meas_490Hz_step(byte pin) {
  int i, j, v, vmin, vmax, result;
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
  for(i = 0; i < 10000; i++) {
    v = analogRead(PIN_IN);
    if(v > 511) break;
  }
  digitalWrite(pin, LOW);
  for(i = 0; i < 10000; i++) {
    v = analogRead(PIN_IN);
    if(v < 512) break;
  }
  analogWrite(pin, 127);
  delay(100);
  result = 0;
  for(i = 0; i < 32; i++) {
    vmin = 1023; vmax = 0;
    for(j = 0; j < 25; j++) {
      v = analogRead(PIN_IN);
      vmin = min(vmin, v);
      vmax = max(vmax, v);
    }
    result += vmax - vmin;
  }
  pinMode(pin, INPUT);
  return result >> 5;
}

float meas_490Hz() {
  int value;
  value = meas_490Hz_step(PIN_1K);
  if(value < 200) return 600000.0 / value;
  value = meas_490Hz_step(PIN_10K);
  if(value < 200) return 60000.0 / value;
  value = meas_490Hz_step(PIN_100K);
  if(value < 200) return 6000.0 / value;
  else return 0.0;
}

long meas_C_step(byte pin) {
  int i, v1, v2;
  long t1, t2;
  pinMode(pin, OUTPUT);  
  digitalWrite(pin, HIGH);
  for(i = 0; i < 10000; i++) {
    t1 = micros();
    v1 = analogRead(PIN_IN);
    if(v1 > 767) break;
  }
  digitalWrite(pin, LOW);
  for(i = 0; i < 10000; i++) {
    t2 = micros();
    v2 = analogRead(PIN_IN);
    if(v2 < 282) break;
  }
  pinMode(pin, INPUT);
  if((v1-v2) > 485+64) return 0;
  else if((v1-v2) < 485-64) return -1;
  return (t2-t1)*485/(v1-v2);
}

float meas_C() {
  long value = 0;
  value = meas_C_step(PIN_1K);
  if(value > 0) return value/REF_1K;
  value = meas_C_step(PIN_10K);
  if(value > 0) return value/REF_10K;
  value = meas_C_step(PIN_100K);
  if(value > 0) return value/REF_100K;
  value = meas_C_step(PIN_1M);
  if(value > 0) return value/REF_1M;
  else return 0.0;
}

int meas_R_step(byte pin) {
  int value;
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
  delay(1);
  value = analogRead(PIN_IN);
  pinMode(pin, INPUT);
  return value;
}

float meas_R() {
  long value = 0;
  value = meas_R_step(PIN_1K);
  if(value < 768) return value*REF_1K/(1023.0 - value);
  value = meas_R_step(PIN_10K);
  if(value < 768) return value*REF_10K/(1023.0 - value);
  value = meas_R_step(PIN_100K);
  if(value < 768) return value*REF_100K/(1023.0 - value);
  value = meas_R_step(PIN_1M);
  if(value < 1000) return value*REF_1M/(1023.0 - value);
  else return 0.0;
}

float meas_U() {
  int value = analogRead(PIN_IN);
  return value * REF_V / 1023.0;
}

float meas_D() {
  int value;
  pinMode(PIN_1K, OUTPUT);
  digitalWrite(PIN_1K, HIGH);
  delay(1);
  value = analogRead(PIN_IN);
  pinMode(PIN_1K, INPUT);
  if(value < 1020) return value * REF_V / 1023.0;
  else return 0.0;
}

void loop() {
  static byte cmd = 'U';
  static float(*meas)() = meas_U;
  static char *unit = "V";
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.write(cmd); Serial.print(" = "); Serial.print(meas()); Serial.println(unit);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
  if(Serial.available() > 0) {
    cmd = Serial.read() & 0xDF; // uppercase
    switch(cmd) {
      case 'C': meas = meas_C; unit = "nF";break;
      case 'R': meas = meas_R; unit = "kOhm"; break;
      case 'U': meas = meas_U; unit = "V";break;
      case 'D': meas = meas_D; unit = "V";break;
      default: cmd = 'U'; meas = meas_U; unit = "V";
    }
  }
}



