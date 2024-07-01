/**
program is created by Arman.I used chatgpt for commenting.
Made for actually controlling servo. for soft start and stop.

Loop:
Reads the potentiometer value and maps it to an angle.
Checks the start and stop buttons to set flags for soft start and soft stop.
Updates the servo angle incrementally in the soft start and soft stop conditions.
Directly sets the servo angle if neither soft start nor soft stop is active.
Formats and prints the angle values to the LCD, ensuring to clear any residual numbers from previous prints.

**/


#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <stdbool.h>

// Initialize the LCD with I2C address 0x27 and 16 columns, 2 rows
LiquidCrystal_I2C lcd(0x27, 16, 2);
int pot = A0; // Analog pin for potentiometer
int angle_value = 0; // Variable to store the potentiometer angle value

// Define pins for start and stop buttons
int start = 2;
int stop = 4;

char angle_start_stop_str[20]; // String to store the start/stop angle value
char angle_run_str[20]; // String to store the running angle value

int servo_set_angle = 0; // Desired angle to set for the servo
int servo_set_angle_value = 0; // Current angle value of the servo
bool servo_angle_got = false; // Flag to check if servo angle is set

bool soft_start = true; // Flag for soft start
bool soft_stop = false; // Flag for soft stop
bool start_servo = true; // Flag to start the servo

Servo servo1; // Create Servo object
int servo1_pin = 3; // Pin for the servo

unsigned long previousMillis = 0; // Store the last time the servo was updated
unsigned long interval = 100; // Interval at which to update the servo
unsigned long currentMillis = 0; // Store the current time

void setup() {
  // Set the pin modes
  pinMode(A0, INPUT);
  pinMode(servo1_pin, OUTPUT);
  pinMode(start, INPUT_PULLUP);
  pinMode(stop, INPUT_PULLUP);

  // Initialize the LCD
  lcd.begin(16, 2);
  lcd.backlight();

  // Attach the servo to the specified pin
  servo1.attach(servo1_pin);
  servo1.write(0); // Initialize the servo to 0 degrees

  // Initialize serial communication for debugging
  Serial.begin(115200);
}

void loop() {
  currentMillis = millis(); // Get the current time

  // Read the analog value from the potentiometer and map it to 0-180 degrees
  angle_value = map(analogRead(A0), 0, 1023, 0, 180);

  // Check if the start button is pressed
  if (digitalRead(start) == LOW) {
    soft_start = true;
    soft_stop = false;
    servo_set_angle = angle_value;
    servo_angle_got = true;
  }

  // Check if the stop button is pressed
  if (digitalRead(stop) == LOW) {
    servo_set_angle = angle_value;
    servo_angle_got = true;
    soft_start = false;
    soft_stop = true;
  }

  // Soft start logic
  if ((servo_angle_got == true) && (soft_start == true) && (currentMillis - previousMillis) >= interval) {
    if (servo_set_angle_value <= servo_set_angle) {
      servo_set_angle_value += 1;
      servo1.write(servo_set_angle_value);
    } else {
      servo_set_angle_value = 0;
      servo_angle_got = false;
      soft_start = false;
      start_servo = true;
      previousMillis = 0;
      currentMillis = 0;
    }
  }

  // Soft stop logic
  if ((servo_angle_got == true) && (soft_start == false) && (soft_stop == true) && (currentMillis - previousMillis) >= interval) {
    if (servo_set_angle_value >= 0) {
      servo_set_angle_value -= 1;
      servo1.write(servo_set_angle_value);
    } else {
      servo_set_angle_value = 0;
      servo_angle_got = false;
      soft_stop = false;
      start_servo = false;
      previousMillis = 0;
      currentMillis = 0;
    }
  }

  // If neither soft start nor soft stop, set the servo directly
  if (soft_start == false && soft_stop == false && start_servo == true) {
    servo_set_angle_value = angle_value;
    servo1.write(servo_set_angle_value);
  }

  // Format the angle values to strings and add spaces to clear residual numbers
  sprintf(angle_start_stop_str, "%d  ", angle_value);
  sprintf(angle_run_str, "%d  ", servo_set_angle_value);

  // Display the angle values on the LCD
  lcd.setCursor(0, 0);
  lcd.print("SETANGL: ");
  lcd.setCursor(8, 0);
  lcd.print(angle_start_stop_str);
  lcd.setCursor(0, 1);
  lcd.print("RUNANG: ");
  lcd.setCursor(9, 1);
  lcd.print(angle_run_str);
}
