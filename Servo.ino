#define IRSENSORA 5
#define IRSENSORB 11
#define FORWARD 9
#define BACKWARD 10
char incomingByte[6];
int x;
int sign;
int number;
int checksum;
int detectA;
int detectA_old;
int detectB;
int detectB_old;
int out = 0;
int A = 0;
int B = 0;
int sp = 0;
int i = 0;
int j = 0;
float enspeed = 0;
float theta_des = 0;
float theta = 0;
float error = 0;
float output = 0;
float s = 0;
float v = 0;
float Kp = 0;
float Kpxl = 0.45;
float Kpl = 0.85;
float Kpm = 1.75;
float Kps = 3.15;
float kg = 0.0133333;
float time1 = 0;
float time2 = 0;


int setSpeed(float s) {
  if (s > 1.0) {
    s = 1.0;
  }
  else if (s < 0.0) {
    s = 0;
  }
  out = int(s * 225.0);
  return out;
}

int setVelocity(float velocity) {
  s = velocity;
  if (s < 0) {
    s = -1 * s;
  }
  int s_i = setSpeed(s);
  //Serial.print("Speed is: ");
  //Serial.println((s_i));
  if (velocity > 0) {
    analogWrite(BACKWARD, 0);
    analogWrite(FORWARD, (int)s_i);
  }
  else if (velocity < 0) {
    analogWrite(FORWARD, 0);
    analogWrite(BACKWARD, (int)s_i);
  }
  else {
    analogWrite(FORWARD, 0);
    analogWrite(BACKWARD, 0);
  }
}

float getTheta(int count) {
  //float(theta) = (((count) / (8)) * 360)*(kg);
  float(theta) = count*(2.4);
  //Serial.print("theta is: ");
  //Serial.println(theta);
  return theta;
}

float positionControl(float theta_des, float theta) {
  error = theta_des - theta;
  //Serial.print("Error is: ");
  //Serial.println(error);
  if (abs(error) >= 360){
    Kp = Kpxl;
  }
  else if (abs(error) < 360 && abs(error) >= 180){
    Kp = Kpl;  
  }
  else if (abs(error) > 90 && abs(error) < 180){
    Kp = Kpm;
  }
  else{
    Kp = Kps;
  }
  output = Kp * error;
  //Serial.print("Kp output is: ");
  //Serial.println(output);  
  return output;
}

void setup() {
  // put your setup code here, to run once:
  pinMode(IRSENSORA, INPUT);
  pinMode(IRSENSORB, INPUT);
  pinMode(FORWARD, OUTPUT);
  pinMode(BACKWARD, OUTPUT);
  Serial1.begin(9600);
  Serial.begin(19200);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial1.available() >= 6) {
  //read the incoming byte:
    for (byte i=0; i<7; i++){
      incomingByte[i] = Serial1.read();
    }
  
  if ((incomingByte[0] == char(0xFF))&&(incomingByte[1] == char(0xFF))){
    if ((char(0xFF)%(incomingByte[2]+incomingByte[3]+incomingByte[4])) == incomingByte[5]){
      if (char(0x00) == incomingByte[4]){
        x = int(incomingByte[3]) << int(4);
      }
      else{
         x = int(incomingByte[3]);
      }
       sign = int(incomingByte[2]);
       number = x;
       //i = 0;
       if (char(0x01) == sign){
         number = (-1)*number;
      }
    }
    else{
      Serial1.write("Error: Checksum is Wrong");
    }
  }
  else{
    Serial1.write("Error: Begining is Wrong");
  }
  theta_des = number;
  if (theta_des > 720){
    theta_des = 720;
  }
  if (theta_des < -720){
    theta_des = -720;
  }
  //while (theta_des > 360) {
    //theta_des = theta_des - 360;
  //}
  //while (theta_des < -360) {
    //theta_des = theta_des + 360;
  //}
  time1 = micros();
  time2 = micros();
  }

  float delta_t = (time2 - time1);
  while (delta_t < float(1000000)) {
    detectA = digitalRead(IRSENSORA);
    detectB = digitalRead(IRSENSORB);
    if (detectA != detectA_old && detectA == LOW) {
      if (detectB == detectA) {
        i = i + 1;
        //Serial.print("i is: ");
        //Serial.println((i));
      }
      else {
        i = i - 1;
        //Serial.print("B, i is: ");
        //Serial.println((i));
      }
    }
      detectA_old = detectA;
      time2 = micros();
      delta_t = (time2 - time1);
    }
  theta = getTheta(i);
  Serial.print("Theta is: ");
  Serial.println(theta);
  output = positionControl(theta_des, theta);
  output = output / 360.0;
  //Serial.print("Output is: ");
  //Serial.println(output);  
  setVelocity(output);
  //i = 0;
  time1 = micros();
}
