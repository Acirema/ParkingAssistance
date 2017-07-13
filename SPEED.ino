#define PIN_LED 13
#define PIN_TRIGGER 8
#define PIN_ECHO 9
#define PIN_BUZZ 12
#define MIN_DISTANCE 30          //in cm
#define MED_DISTANCE 90          //in cm
#define MAX_DISTANCE 150         //in cm
#define MAX_RANGE 400            //in cm . Maximum accurate distance detected by the sensor is 4 meters
#define SERIAL_DR 9600
#define SPEED_OF_SOUND 345.17   // @20 C
#define DISTANCE_SAMPLES  10    //Number of distance measurements to promediate
#define FAR         2000
#define NEAR        750
#define TOO_CLOSE   500

const int readings_echo=100;
int       v=0;
uint8_t   r=0;
long      duration, cm, inches;
long distance=0,last_distance=0;
float last_speed_s=0,speed_s=0;
/*set_trigger
 * This function is used to set the trigger pin to HIGH
 * Input:none
 * Output:none
*/
void set_trigger(){
	//Serial.print("\nSetting Trigger\n");
	digitalWrite(PIN_TRIGGER,LOW);
	delayMicroseconds(2);
	digitalWrite(PIN_TRIGGER,HIGH);
	delayMicroseconds(10);
	digitalWrite(PIN_TRIGGER,LOW);
}
/*Read_echo
 * This function returns the time that the echo pin is HIGH in microseconds
 * Input:none
 * Output: long times
*/
long read_echo(){
	long times;
  while(!digitalRead(PIN_ECHO)) times=micros();
  while(digitalRead(PIN_ECHO));
  times=micros()-times;
	// r = (v==readings_echo)?1:0;  
	return times;	
}
/* Read_distance
 *  This function returns in cm the distance detected
 * Input:none
 * Output: long cm 
*/
long read_distance(){
  /* Consider Speed of sound as 331.4 + 0.6 T (where T is temperature expressed in Celsius)
   * Exactly, the speed of sound is = 20.05 sqrt(T-273.5) , T in Celsius ;  http://230nsc1.phy-astr.gsu.edu/hbase/Sound/souspe3.html#c1
   * Speed of sound = 345.17m/s (@23°C, @20°C:343.42 ,@19°C:342.8, @17°C:341.6) =345.17 *10`2 *10`-6 cm/us  =34,52*10`-3 cm/us
   * time of echo signal in us (send ´til receive) = duration
   * distance = duration/2 * speed
  */
  set_trigger();
  duration= read_echo(); // duration in us between sending and receiving the signal  
  cm = (duration/2)*SPEED_OF_SOUND*0.0001;
  return cm;  
}

/*measure_speed
 * This function reads takes 2 measure of distance and determinates the speed of the vehicle
 * Input:none
 * Output: long speed_ inmeters per second
*/
double measure_speed(){
 double speed_=0,d1=0,d2=0;
  double times=0;
  times=micros();  
    d1=read_distance();
  Serial.print("D1: ");  Serial.println(d1);
    
    d2=read_distance();
  Serial.print("D2: ");  Serial.println(d2);
  times=micros()-times;
  if (abs(d2-d1) > 2) speed_=abs(d2-d1)*10000/times;
  Serial.print("SPEED: ");
  Serial.println(speed_);
  return speed_;
}

/*filter_distance
 * This function reads a number of values of distance and returns the average
 * The number of valued read is set by DISTANCE_SAMPLES
 * Input:none
 * Output: long prom
*//*
long filter_distance(){
  char measure[DISTANCE_SAMPLES]={0,0,0};
  long dist=0,a=0;
  a = read_distance();

  
  for(int i=0; i<DISTANCE_SAMPLES;i++){
      long a=read_distance();
      while(a>100) a=read_distance();
      measure[i]=a;
      prom=prom+measure[i];  
  }
  prom/=DISTANCE_SAMPLES;
  return prom;  
}
*/

/*buzzer_on
 * This function sets the tone to a frequency of 261 Hz
 * Input:freq which can be FAR, NEAR or TOO_CLOSE
 * Output:none
*/
void buzzer_1(){
  tone(PIN_BUZZ,261);
  delay(230);
  noTone(PIN_BUZZ);  
}
void buzzer_2(){
  tone(PIN_BUZZ,261);
  delay(150);
  noTone(PIN_BUZZ);  
}
void buzzer_3(){
  tone(PIN_BUZZ,261);
  delay(80);
  noTone(PIN_BUZZ);  
}

 

void setup() {
/*Pins Initialization*/	
	pinMode(PIN_LED,OUTPUT);
	pinMode(PIN_TRIGGER,OUTPUT);
	pinMode(PIN_ECHO,INPUT);
  pinMode(PIN_BUZZ,OUTPUT);
/**********************/	
	Serial.begin(SERIAL_DR);
}

void loop() {
  //distance = prom_distance();
  distance=read_distance();
  
  //Serial.print("\nlast-recent: ");
  //Serial.println(last_distance-distance);
  
  if (abs(last_distance-distance >20));// if this happens, do nothing because this measure is not trustable
      else if(distance ==0);        // if distance equals 0, it is an error, dismiss it  
      else if (distance>=MAX_RANGE)   Serial.println("OUT OF RANGE\n");
      else if((distance <= MAX_DISTANCE) && (distance >= MED_DISTANCE) ){
        Serial.println("\nCAUTION: Object close. Less than 1 meter\n");
        buzzer_1();
      }
      
      else if((distance < MED_DISTANCE)&& (distance >= MIN_DISTANCE)){
        Serial.println("\nCAUTION: Object closer. Less than 50 centimeters\n");
        buzzer_2();
      }      
      else if(distance < MIN_DISTANCE){
        Serial.println("\nCAUTION: OBJECT TOO CLOSE. Less than 10 centimeters\n");
        buzzer_3();
      }
      else                            Serial.print("...Driving safely...\n"); 
  
  Serial.print("Distance in cm: ");
  Serial.println(distance);
  last_distance=distance;
  speed_s=measure_speed();
  if (abs(last_speed_s-speed_s)>15) speed_s=last_speed_s; 
  last_speed_s=speed_s;
  Serial.print("Speed in m/s: ");
  Serial.print(speed_s); 
  Serial.print("          Speed in Km/h: ");
  Serial.println(speed_s*3600*0.001); 
  
  /*sensorValue = analogRead(0);       // read analog input pin 0
  Serial.print("Sensor value:");
  Serial.println(sensorValue, DEC);  // prints the value read
  */delay(250);
  //delayMicroseconds(20);
}
