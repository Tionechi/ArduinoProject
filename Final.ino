#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();
#include <TimeLib.h>
#include <MemoryFree.h>

#define sync 0
#define main 1
#define select 2

int state = sync;

struct Cars {
  String CarReg;
  char CarType;
  String CarPark;
  boolean Paid;
  String EntryTime;
  String ExitTime;
};

const int MaxCars = 7;     // Maximum number of cars that can stored
Cars CarLog[MaxCars];      // Array in which each instance of the struct will be stored
int currentIndex = 0;      // Variable to keep track of the index for the next insertion
int i = 0;                 // Variable to keep track of which index in the array is being displayed on the lcd
bool ClearScreen = false;  //Variable to keep track of whether the screen needs to be cleared


// Function to parse input and populate the structure
Cars parseInput(String input) {
  Cars result;

  // Find the position of the first field separator ("-")
  int CarReg = input.indexOf('-');

  // Find the position of the second field separator ("-") after the first one
  int CarType = input.indexOf('-', CarReg + 1);

  // Find the position of the third field separator ("-") after the second one
  int CarPark = input.indexOf('-', CarType + 1);

  // Extract the first part of the string as CarReg
  result.CarReg = input.substring(0, CarReg);

  // Extract the second part of the string as CarType
  result.CarType = input.charAt(CarReg + 1);

  // Extract the third part of the string as CarPark
  result.CarPark = input.substring(CarType + 1, CarPark);

  // Extract the fourth part of the string as Paid
  String Paidstr = input.substring(CarPark + 1);
  result.Paid = Paidstr.equalsIgnoreCase("PD");

  return result;
}

// Function to parse input and insert into the struct array
void insertIntoCarLog(String input) {
  // Check if there is space in the array
  if (currentIndex < MaxCars) {
    // Parse the input and insert into the array
    CarLog[currentIndex] = parseInput(input);
    // Insert the entry time into the struct array
    CarLog[currentIndex].EntryTime = getTime();
    CarLog[currentIndex].ExitTime = " ";
    currentIndex++;

    // Print the values to verify input
    Serial.print("DEBUG:InsertedAtIndex");
    Serial.println(currentIndex - 1);
    Serial.print("DEBUG:CarReg:");
    Serial.println(CarLog[currentIndex - 1].CarReg);
    Serial.print("DEBUG:CarType:");
    Serial.println(CarLog[currentIndex - 1].CarType);
    Serial.print("DEBUG:CarPark:");
    Serial.println(CarLog[currentIndex - 1].CarPark);
    Serial.print("DEBUG:Paid:");
    Serial.println(CarLog[currentIndex - 1].Paid ? "PD" : "NPD");
    Serial.print("DEBUG:EntryTime:");
    Serial.println(CarLog[currentIndex - 1].EntryTime);
    Serial.println("DONE!");
  } else {
    Serial.println("DEBUG:CarLogIsFull.CannotInsertMoreCars.");
    Serial.println("DONE!");
  }
}


// Function to update payment status
void UpdatePayment(String input) {
  // Find the index of the entry in the array based on the registration
  int updateIndex = FindCarReg(input);
  // Check if the entry was found
  if (updateIndex != -1) {
    // Update the payment status
    CarLog[updateIndex].Paid = parsePaid(input);
    Serial.print("DEBUG:UpdatedAtIndex");
    Serial.println(updateIndex);
    Serial.print("DEBUG:CarReg:");
    Serial.println(CarLog[updateIndex].CarReg);
    Serial.print("DEBUG:Paid:");
    Serial.println(CarLog[updateIndex].Paid ? "PD" : "NPD");
    if (CarLog[updateIndex].Paid == 1) {
      CarLog[updateIndex].ExitTime = getTime();
      Serial.print("DEBUG:ExitTime:");
      Serial.println(CarLog[updateIndex].ExitTime);
      Serial.println("DONE!");
    } else {
      CarLog[updateIndex].ExitTime = " ";
      CarLog[updateIndex].EntryTime = getTime();
    }


  } else {
    Serial.println("ERROR:CarNotFound,PleaseTryAgain.");
  }
}

// Function to find the index of an entry in the array based on CarReg
int FindCarReg(String input) {
  // Find the position of the first field separator ("-")
  int CarReg = input.indexOf('-');

  // Extract the first part of the input as CarReg
  String searchCarReg = input.substring(0, CarReg);

  // Search for the entry in the array based on the CarReg
  for (int i = 0; i < currentIndex; i++) {
    if (CarLog[i].CarReg.equals(searchCarReg)) {
      // Return the index if found
      return i;
    }
  }
  // Return -1 if not found
  return -1;
}

// Function to parse the updated payment status into the array
boolean parsePaid(String input) {
  // Find the position of the first field separator ("-")
  int sp = input.indexOf('-');

  // Extract the second part of the input as the updated payment status
  String UpdatedPaid = input.substring(sp + 1);

  return UpdatedPaid.equals("PD");
}

//Function to update Cartype
void UpdateCarType(String input) {
  // Find the index of the entry in the array based on the registration
  int updateIndex = FindCarReg(input);


  // Check if the entry was found and if the car has a payment status of paid
  if (updateIndex != -1 && CarLog[updateIndex].Paid) {
    // Update the Cartype
    CarLog[updateIndex].CarType = parseCarType(input);
    Serial.print("DEBUG:UpdatedAtIndex ");
    Serial.println(updateIndex);
    Serial.print("DEBUG:CarReg:");
    Serial.println(CarLog[updateIndex].CarReg);
    Serial.print("DEBUG:CarTpe:");
    Serial.println(CarLog[updateIndex].CarType);
    Serial.println("DONE!");
  } else {
    Serial.println("ERROR:InvalidInput,CouldNotUpdateCarType");
  }
}

// Function to parse the updated Car type into the array
char parseCarType(String input) {
  // Find the position of the first field separator ("-")
  int sp = input.indexOf('-');

  // Extract the second part of the input as the CarType
  return input.charAt(sp + 1);
}

//Function to update Car Park
void UpdateCarPark(String input) {
  // Find the index of the entry in the array based on the registration
  int updateIndex = FindCarReg(input);

  // Check if the entry was found and if the car has a payment status of paid
  if (updateIndex != -1 && CarLog[updateIndex].Paid) {
    // Update the Cartype
    CarLog[updateIndex].CarPark = parseCarPark(input);
    Serial.print("DEBUG:UpdatedAtIndex");
    Serial.println(updateIndex);
    Serial.print("DEBUG:CarReg:");
    Serial.println(CarLog[updateIndex].CarReg);
    Serial.print("DEBUG:CarPark:");
    Serial.println(CarLog[updateIndex].CarPark);
    Serial.println("DONE!");
  } else {
    Serial.println("ERROR:InvalidInput,CouldNotUpdateCarPark");
  }
}
// Function to parse the updated Car Park into the array
String parseCarPark(String input) {
  // Find the position of the first field separator ("-")
  int sp = input.indexOf('-');

  // Extract the second part of the string as the updated Car park
  return input.substring(sp + 1);
}

// Function to delete a car from the array
void DeleteCar(String input) {
  // Find the index of the entry in the array based on the first part of the input
  int DeletedIndex = FindCarReg(input);

  // Check if the entry was found
  if (DeletedIndex != -1 && CarLog[DeletedIndex].Paid) {
    // Move all entries after the deleted one to fill the gap left by the deleted car
    for (int i = DeletedIndex; i < currentIndex - 1; i++) {
      CarLog[i] = CarLog[i + 1];
    }

    // Decrement the index since an entry was deleted
    currentIndex--;
    lcd.clear();
    delay(10);

    Serial.print("DEBUG:DeletedACarAtIndex");
    Serial.println(DeletedIndex);
    Serial.println("DONE!");
  } else {
    Serial.println("ERROR:InvalidInput,PleaseTryAgain");
  }
}

// Function to display Car records on the LCD screen and be able to view different records with a push of a button
void Display() {
  uint8_t buttons = lcd.readButtons();
  lcd.setCursor(1, 0);
  lcd.print(CarLog[i].CarReg);
  lcd.setCursor(9, 0);
  lcd.print(CarLog[i].CarPark);
  lcd.setCursor(1, 1);
  lcd.print(CarLog[i].CarType);
  lcd.setCursor(3, 1);
  lcd.print(CarLog[i].Paid ? "PD" : "NPD");
  lcd.setCursor(7, 1);
  lcd.print(CarLog[i].EntryTime);
  lcd.setCursor(12, 1);
  lcd.print(CarLog[i].ExitTime);

  if (buttons & BUTTON_DOWN) {  // Down button to scroll down the records
    lcd.clear();                // Clear the lcd screen between each button press to get rid of any leftover characters
    delay(10);
    // Logic to ensure i doesn't increment past the limits of the array
    if ((currentIndex - i) > 1) {
      ++i;
    }
  } else if (buttons & BUTTON_UP) {  // Up button to scroll up the records
    lcd.clear();
    delay(10);
    // Logic to ensure i doesn't deincrement past the limits of the array
    if (i > 0) {
      --i;
    }
  }
  // Logic to ensure the screen is set to Yellow for "NPD" and Green for "PD"
  if (CarLog[i].Paid == 0) {
    lcd.setBacklight(3);
  } else if (CarLog[i].Paid == 1) {
    lcd.setBacklight(2);
  }
}

// function to display arrow characters
void arrows() {
  byte Up_arrow[8] = {
    0b00100,
    0b01110,
    0b11111,
    0b00100,
    0b00100,
    0b00000,
    0b00000,
    0b00000
  };

  byte Down_arrow[8] = {
    0b00000,
    0b00000,
    0b00000,
    0b00100,
    0b00100,
    0b11111,
    0b01110,
    0b00100
  };
  lcd.createChar(0, Up_arrow);
  lcd.createChar(1, Down_arrow);
  if (i != 0) {  // Logic to ensure the up arrow only appears if there is a Car record to be viewed above
    lcd.setCursor(0, 0);
    lcd.write(byte(0));
  };
  if ((currentIndex - i) > 1) {  // Logic to enure the down arrow only appears if there are more records to be viewed at the bottom
    lcd.setCursor(0, 1);
    lcd.write(byte(1));
  };
}

// Function to check if the Car Park already contains a car with the a certain registration number
boolean containsCarReg(String input) {
  int CarReg = input.indexOf('-');
  // Ensure the program is only looking at the Car Reg part of the input string
  String searchCarReg = input.substring(0, CarReg);
  for (int a = 0; a <= currentIndex; a++) {
    if (CarLog[a].CarReg.equals(searchCarReg)) {
      return true;  // Return true if the value is found
    }
  }
  return false;  // Return false if the value is not found
}

// Function to get the current time
String getTime() {
  String HourStr = String(hour());
  String MinuteStr = String(minute());

  // Pad single-digit hour or minute with a leading zero to ensure HHMM format is maintained
  if (hour() < 10) {
    HourStr = "0" + HourStr;
  }
  if (minute() < 10) {
    MinuteStr = "0" + MinuteStr;
  }

  return HourStr + MinuteStr;
}

// Function to check if the car type is correct
boolean correctCarType(String input) {
  if (input == "C" || input == "M" || input == "V" || input == "L" || input == "B") {
    return true;  // Return true if the value is found
  } else {
    return false;  // Return false if the value is not found
  }
}

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  setTime(0, 0, 0, 1, 1, 2023);
}


void loop() {

  switch (state) {
    case sync:
      {
        lcd.setBacklight(5);  //sets the lcd screen to purple
        Serial.print("Q");
        delay(1000);  //adds a delay of 1 second between each output of "Q" to the serial monitor

        if (Serial.available() > 0) {
          char input = Serial.read();
          char input2 = Serial.read();


          if (input == 'X' && (input2 != '\n' && input2 != '\r')) {
            state = main;
            Serial.println("DEBUG:UDCHARS,FREERAM\n");
            lcd.setBacklight(7);  //sets the lcd screen to white
          } else {
            Serial.print("ERROR:Invalid input");
          }
        }
      }
      break;
    case main:
      {
        // Logic to clear screen before code is executed in this state
        if (!ClearScreen) {
          lcd.clear();
          ClearScreen = true;
        }
        static uint8_t button_hold = 0;
        static unsigned long c_timer = 0;
        uint8_t buttons = lcd.readButtons();
        if ((buttons != 0) && (buttons != button_hold)) {
          c_timer = millis();
        } else if (buttons == 0) {
          c_timer = millis();
        }
        button_hold = buttons;
        // Logic to swap to SELECT state
        if (buttons & BUTTON_SELECT) {
          if ((millis() - c_timer) > 1000) {
            state = select;
            ClearScreen = false;
          }
        }
        if (Serial.available() > 0) {
          // Read the input from the serial monitor
          String input = Serial.readStringUntil('\n');  // Read until newline character

          // Check if the input begins with "A-"
          if (input.startsWith("A-")) {
            String check_Reg = input.substring(2);
            int SP1 = input.indexOf('-');
            int SP2 = input.indexOf('-', SP1 + 1);  // locate the car registration in the substring
            int SP3 = input.indexOf('-', SP2 + 1);  // locate the car type in the substring
            int SP4 = input.indexOf('-', SP3 + 1);  // locate the car park in the substring
            String Reglen_check = input.substring(2, SP2);
            String Ty_check = input.substring(SP2 + 1, SP3);
            String Palen_check = input.substring(SP3, SP4);
            // Check if the length of the Car registration is correct
            if (Reglen_check.length() == 7) {
              // Check if the Car type is valid
              if (correctCarType(Ty_check) == true) {
                // check if the length of the Car Park is correct
                if ((Palen_check.length() < 13) && (Palen_check.length() > 1)) {
                  // Check if the Car Reg already exists in the array
                  if (containsCarReg(check_Reg) != true) {
                    // Insert into the array
                    insertIntoCarLog(check_Reg);
                  } else {
                    Serial.println("ERROR:EntryWithTheSameCarRegistrationAlreadyExists");
                  }
                } else {
                  Serial.println("ERROR:FormatOfCarParkInvalid");
                }
              } else {
                Serial.println("ERROR:InvalidCarType");
              }
            } else {
              Serial.println("ERROR:FormatOfCarRegInvalid.");
            }
          }
          //Check if input begins with ("S-")
          else if (input.startsWith("S-")) {
            int SP1 = input.indexOf('-');
            int SP2 = input.indexOf('-', SP1 + 1);
            int SP3 = input.indexOf('-', SP2 + 1);
            String Paid = input.substring(SP2 + 1, SP3);
            if (Paid == "PD" || Paid == "NPD") {
              //Upate payment status of a car already in CarLog
              UpdatePayment(input.substring(2));
            } else {
              Serial.println("ERROR:InvalidInput");
            }
          }
          //Check if input begins with ("T-")
          else if (input.startsWith("T-")) {
            int SP1 = input.indexOf('-');
            int SP2 = input.indexOf('-', SP1 + 1);
            int SP3 = input.indexOf('-', SP2 + 1);
            String CarType = input.substring(SP2 + 1, SP3);
            // Check if the Car type is valid
            if (correctCarType(CarType) == true) {
              //Update CarType
              UpdateCarType(input.substring(2));
            } else {
              Serial.println("ERROR:InvalidCarType");
            }
          }
          //Check if the input begins with ("L-")
          else if (input.startsWith("L-")) {
            int SP1 = input.indexOf('-');
            int SP2 = input.indexOf('-', SP1 + 1);
            int SP3 = input.indexOf('-', SP2 + 1);
            String Palen_check = input.substring(SP2, SP3);
            // Check that the Car park is valid
            if (Palen_check.length() < 13) {
              //Update Car Park
              UpdateCarPark(input.substring(2));
            } else {
              Serial.println("ERROR:InvalidCarPark");
            }
          }
          //Check if the input begins with ("R-")
          else if (input.startsWith("R-")) {
            //Delete Car from array
            DeleteCar(input.substring(2));
          } else {
            // Print an error message
            Serial.println("ERROR:InputNotRecognized");
          }
        }
        // Logic added in to ensure system only displays data once the car park is populated
        if (!currentIndex == 0) {
          Display();
          arrows();
        } else {
          lcd.setBacklight(7);
        }
      }
      break;
    case select:
      {
        uint8_t buttons = lcd.readButtons();
        // logic to ensure the screen is cleared before code in this state is executed
        if (!ClearScreen) {
          lcd.clear();
          ClearScreen = true;
        }
        // Logic to swap back to main state
        if (!(buttons & BUTTON_SELECT)) {
          state = main;
          ClearScreen = false;
        }
        lcd.setBacklight(5);
        lcd.setCursor(0, 0);
        lcd.print("F312575");
        lcd.setCursor(0, 1);
        lcd.print("SRAM:");
        lcd.setCursor(5, 1);
        lcd.print(freeMemory());
      }
  }
}
