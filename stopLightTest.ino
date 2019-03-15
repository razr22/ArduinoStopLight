/*
 * @author: Zain Quraishi
 * @date: January 31, 2019
 * @filename: stopLightTest.ino
 * @description: Identifies objects moving towards arduino, sets led strip colour to represent distance to object.
 */
 //-------------------------------------------------------------------------------------------------------------------------------//
 
  int trigPin = 11; //US data pins
  int echoPin = 10;
  
  int redPin = 5; //LED RGB data pins
  int greenPin = 6;
  int bluePin = 7;
  
  long duration;  //vars to determine object location, speed, direction.
  int newDistance = 0;
  int prevDistance = 0;
  int displacement[10];
  int flag = 0;
  int approaching = 0;
  int i = 0;
  int j = 0;
  const int BUFFER = 5;

  //PRODUCTION VALUES
  const int redZoneMin = 10;
  const int redZoneMax = 20;

  const int orangeZoneMin = 20;
  const int orangeZoneMax = 80;

  const int greenZoneMin = 80;
  const int greenZoneMax = 130;

  const int detectionZoneMin = 130;
  const int detectionZoneMax = 200;


//-------------------------------------------------------------------------------------------------------------------------------//

void setup() {
  pinMode(trigPin, OUTPUT); //init I/O pins for US sensor.
  pinMode(echoPin, INPUT);
  
  pinMode(redPin, OUTPUT);  //init output pins for LED strip.
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  setColour(0,0,0);
  
  Serial.begin(9600); //init connection between arduino and US sensor.
}

//-------------------------------------------------------------------------------------------------------------------------------//

void loop() {
  //Send and retrieve US signal.
  delay(50);
  digitalWrite(trigPin, HIGH);
  delay(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);

  //Calculate object distance = v*t/2
  newDistance = 0.034*duration/2;

  //Check if object is detected and is approaching the sensor.
  if (approaching == 0){
    isDetected(newDistance, j);
  }
  else if (approaching == 1 && newDistance <= detectionZoneMax){
    objectAlert(newDistance);
  }
}

//-------------------------------------------------------------------------------------------------------------------------------//

//Function provides functionality to determine what distance zone object is currently in, notified on the led strip.
void objectAlert(int dist){
  if (dist < prevDistance && dist >= redZoneMin){ 
      if (flag != 1 && dist > redZoneMin && dist <= redZoneMax) {
      while (i < 6){
        setColour(255,0,0);
        delay(250);
        setColour(0,0,0);
        delay(250);
        i++;
      }
    }
    else if (flag != 1 && dist > orangeZoneMin && dist <= orangeZoneMax) {setColour(255,35,0);}
    else if (flag != 1 && dist > greenZoneMin && dist <= greenZoneMax) {setColour(0,255,0);}
    prevDistance = dist;
  }
  //Reset alert if object backs away from sensor.
  else if (dist > (prevDistance+BUFFER) && prevDistance != 0){
    setColour(0,0,0);
    flag = 0;
    approaching = 0;
    i = 0;
    j = 0;
    memset(displacement, 0, 20);
    prevDistance = dist;
  }
}

//-------------------------------------------------------------------------------------------------------------------------------//

//Function determines whether from a given vector array an object is approaching 
void isDetected(int distance, int index){
  //Add distances to vector array if they fall in detection zone range.
  if (distance <= detectionZoneMax && distance >= detectionZoneMin && approaching != 1 && j < 10){
    displacement[j] = distance;
    j++;
  }
  else if (distance > detectionZoneMax+BUFFER || distance < detectionZoneMin-BUFFER && approaching != 1 && j < 10){
//    Serial.println("insufficient data...");
    j = 0;
    memset(displacement, 0, 20);
  }
  
  //If current distance is less than average of previous distances ...
  int avg = getAverage(displacement);

  if (displacement[9] > redZoneMin) {
    //Check if last distance < the avg of the rest of distances, trend towards approaching sensor.
    if (displacement[9] < avg){
      prevDistance = displacement[9];
      approaching = 1;
    }
    else {//Clear vector if object is not approaching.
      j = 0;
      memset(displacement, 0, 20);
    }
  }
}

//-------------------------------------------------------------------------------------------------------------------------------//

//Retrieves average distance to object compared to final distance.
int getAverage(int* arr){
  int sum = 0;
  for (int l=0; l < 9; l++){
    sum += arr[l];
  }
  return (sum/9);
}

//-------------------------------------------------------------------------------------------------------------------------------//

//Sets led strip colour
void setColour(int red, int green, int blue){
    analogWrite(redPin, (255-red));
    analogWrite(greenPin, (255-green));
    analogWrite(bluePin, (255-blue));
}
