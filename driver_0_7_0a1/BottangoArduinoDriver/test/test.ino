const int feedbackL = 32;
const int feedbackR = 33;


void setup(){
  Serial.begin(115200);

  pinMode(feedbackL, INPUT);
  pinMode(feedbackR, INPUT);


}

void loop(){
  int valL = analogRead(feedbackL);
  int valR = analogRead(feedbackR);
  Serial.printf("L: %f R: %f \n",valL,valR);
}