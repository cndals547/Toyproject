#include<Servo.h>

int LED_R = 8;
int LED_B = 10;
int LED_G = 11;
int button = 7;
int tmp_sensor = A0;
int servo_sensor = 9;
int trig = 2;
int echo = 3;
int a = 1;
int angle_minimum = 1;
int angle_maximum = 179;
int angle = angle_maximum;
float get_buffer_tmp[10];
float get_buffer_dist[10];
bool isPlayed = false;
Servo myservo;

float get_tmp_value()
{
  int sensor = analogRead(tmp_sensor);
  float voltage = sensor * (3300 / 1023.0);
  float celsius = (voltage - 500) / 10;
  return celsius;
}

void updateBuffer_Tmp(float T_Tmp)
{
  for (int i=8 ; i>=0; i--)// 
  {
    get_buffer_tmp[i+1] = get_buffer_tmp[i]; 
  }
  get_buffer_tmp[0] = T_Tmp;
  return;
}


void updateBuffer_dist(float D_dist)
{
  for (int i=8 ; i>=0; i--)// 
  {
    get_buffer_dist[i+1] = get_buffer_dist[i]; 
  }
  get_buffer_dist[0] = D_dist;
  return;
}

float get_average_filter_temper()
{
  int data_length = 10;
  float average_sum = 0.0;
  for(int i = 0; i < data_length; i++)
  {
    average_sum += get_buffer_tmp[i];
  }
  return average_sum / data_length;
}

float get_average_filter_dist()
{
  int data_length = 10;
  float dist_sum = 0.0;
  for(int i = 0; i < data_length; i++)
  {
    dist_sum += get_buffer_dist[i];
  }
  return dist_sum / data_length;
}

unsigned long get_dist()
{
  unsigned long dist;
  digitalWrite(trig,LOW);
  digitalWrite(echo,LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  dist = pulseIn(echo, HIGH) / 29.0 / 2.0;
  return dist;
}

void rotate_servo()
{
  myservo.write(angle);
  angle -= a;
  if(angle <= angle_minimum || angle >= angle_maximum)
  {
    a *= -1;
  }
  delay(1);
}

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  myservo.attach(servo_sensor);
  pinMode(LED_R, OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(button, INPUT);
  analogReference(EXTERNAL) ;  //3.3V
  for (int i=9 ; i>=0 ; i--)
  {
    float T_Temp = get_tmp_value();
    get_buffer_tmp[i] = T_Temp;     //데이터 초기화 
  }
  for (int i=9 ; i>=0 ; i--)
  {
    float D_dist = get_dist();
    get_buffer_dist[i] = D_dist;     //데이터 초기화 
  }
  for(int i = 0; i < 10; i++)
  {
    Serial.println(get_buffer_tmp[i]);
  }
  for(int i = 0; i < 10; i++)
  {
    Serial.println(get_buffer_dist[i]);
  }
}

void loop() {
  float tmp_value = 0.0;
  float dist_value = 0.0;
  rotate_servo();
  updateBuffer_dist(get_dist());
  dist_value = get_average_filter_dist();
  Serial.print(dist_value);
  Serial.println("cm");
  updateBuffer_Tmp(get_tmp_value());
  tmp_value = get_average_filter_temper();
  Serial.print(tmp_value);
  Serial.println("℃");
  delay(10);
  if (tmp_value>25)
  {
    digitalWrite(LED_R, HIGH) ;
  }
  else
  {
    digitalWrite(LED_R, LOW) ;
  }

  if ((tmp_value<=23) && (tmp_value>25))
  {
    digitalWrite(LED_G, HIGH) ;
  }
  else
  {
    digitalWrite(LED_G, LOW) ;
  }
  if (tmp_value<23)
  {
    digitalWrite(LED_B, HIGH) ;
  }
  else
  {
    digitalWrite(LED_B, LOW) ;
  }
}
