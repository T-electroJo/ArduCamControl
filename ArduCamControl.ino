#include <IRremote.h>
#include <Servo.h>
#include <EEPROM.h>

#include "remote.h"

const int servo_pan_pin = 2;
const int servo_tilt_pin = 3;
const int ir_pin = 4;

int default_pan_pos = 0;
int default_tilt_pos = 0;
int pan_pos;
int tilt_pos;
int old_pan_pos;
int old_tilt_pos;
const int tilt_max = 150;

bool highspeed = false;
bool writing_mode = false;
bool delete_mode = false;


bool checkFrequencies = false; // Configuration mode, more on github: https://github.com/T-electroJo/ArduCamControl


Servo pan;
Servo tilt;
IRrecv irrecv(ir_pin);

decode_results results;
String result;

void setup() {
  Serial.begin(9600);
  Serial.println("Setup begin...");
  irrecv.enableIRIn(); // Enable IR Input
  pan.attach(servo_pan_pin); // Enable Servo for Pan
  tilt.attach(servo_tilt_pin); // Enable Servo for Tilt

  int pan_value = EEPROM.read(8);
  int tilt_value = EEPROM.read(9);
  pan_pos = pan_value == 255 ? default_pan_pos : pan_value;
  tilt_pos = tilt_value == 255 ? default_tilt_pos : tilt_value;
  move_servos(); // Apply movement

  if (checkFrequencies) {
    Serial.begin(9600); // Open Serial monitor if configure mode is enabled
  }

  int default_disable = EEPROM.read(0);
  if (default_disable != 0 || default_disable != 1) {
    default_disable = 0;
    EEPROM.write(0, default_disable);
  }
  Serial.println("Setup done");
}

void loop() {
  if (irrecv.decode(&results) && !checkFrequencies) {
    result = String(results.value, HEX).substring(2); // Take the last 2 digits from the HEX-String of the IR code.
    
    if (result == remote_UP) {
      write_delete_check();
      if (tilt_pos < tilt_max) {
        tilt_pos++;
        if (highspeed && tilt_pos < tilt_max-4) {
          tilt_pos += 4;
        }
      }
    }
    else if (result == remote_DOWN) {
      write_delete_check();
      if (tilt_pos > 0) {
        tilt_pos--;
        if (highspeed && tilt_pos > 4) {
          tilt_pos -= 4;
        }
      }
    }
    else if (result == remote_LEFT) {
      write_delete_check();
      if (pan_pos < 180) {
        pan_pos++;
        if (highspeed && pan_pos < 176) {
          pan_pos += 4;
        }
      }
    }
    else if (result == remote_RIGHT) {
      write_delete_check();
      if (pan_pos > 0) {
        pan_pos--;
        if (highspeed && pan_pos > 4) {
          pan_pos -= 4;
        }
      }
    }
    else if (result == remote_HOME) {
      write_delete_check();
      pan_pos = default_pan_pos;
      tilt_pos = default_tilt_pos;
    }
    else if (result == remote_SPEED) {
      write_delete_check();
      highspeed = !highspeed;
      delay(500);
    }
    else if (result == remote_DISABLE) {
      write_delete_check();
      if (EEPROM.read(5) == 1) {
        pan.detach();
        tilt.detach();
        EEPROM.write(5, 0);
      } else {
        pan.attach(servo_pan_pin);
        tilt.attach(servo_tilt_pin);
        EEPROM.write(5, 1);
      }
      delay(500);
    }
    else if (result == remote_DELETE) {
      if (writing_mode) {
        delete_mode = true;
      }
    }
    else if (result == remote_PROGRAM) {
      writing_mode = !writing_mode;
      Serial.print("Writing mode ");
      Serial.println(writing_mode ? "enabled" : "disabled");
      delay(500);
    }
    else if (result == remote_OK) {
      if (writing_mode && delete_mode) {
        EEPROM.write(8, 255);
        EEPROM.write(9, 255);
        writing_mode = false;
        delete_mode = false;
        delay(500);
      }
      else if (writing_mode) {
        EEPROM.write(8, pan_pos);
        EEPROM.write(9, tilt_pos);
        writing_mode = false;
        delay(500);
      } else {
        byte pan_value = EEPROM.read(8);
        byte tilt_value = EEPROM.read(9);
        if (pan_value <= 180 && tilt_value <= tilt_max) {
          pan_pos = pan_value;
          tilt_pos = tilt_value;
        }
      }
    }
    else if (result == remote_ZERO) {
      if (writing_mode && delete_mode) {
        EEPROM.write(10, 255);
        EEPROM.write(11, 255);
        writing_mode = false;
        delete_mode = false;
        delay(500);
      }
      else if (writing_mode) {
        EEPROM.write(10, pan_pos);
        EEPROM.write(11, tilt_pos);
        writing_mode = false;
        delay(500);
      } else {
        byte pan_value = EEPROM.read(10);
        byte tilt_value = EEPROM.read(11);
        if (pan_value <= 180 && tilt_value <= tilt_max) {
          pan_pos = pan_value;
          tilt_pos = tilt_value;
        }
      }
    }
    else if (result == remote_ONE) {
      if (writing_mode && delete_mode) {
        EEPROM.write(12, 255);
        EEPROM.write(13, 255);
        writing_mode = false;
        delete_mode = false;
        delay(500);
      }
      else if (writing_mode) {
        EEPROM.write(12, pan_pos);
        EEPROM.write(13, tilt_pos);
        writing_mode = false;
        delay(500);
      } else {
        byte pan_value = EEPROM.read(12);
        byte tilt_value = EEPROM.read(13);
        if (pan_value <= 180 && tilt_value <= tilt_max) {
          pan_pos = pan_value;
          tilt_pos = tilt_value;
        }
      }
    }
    else if (result == remote_TWO) {
      if (writing_mode && delete_mode) {
        EEPROM.write(14, 255);
        EEPROM.write(15, 255);
        writing_mode = false;
        delete_mode = false;
        delay(500);
      }
      else if (writing_mode) {
        EEPROM.write(14, pan_pos);
        EEPROM.write(15, tilt_pos);
        writing_mode = false;
        delay(500);
      } else {
        byte pan_value = EEPROM.read(14);
        byte tilt_value = EEPROM.read(15);
        if (pan_value <= 180 && tilt_value <= tilt_max) {
          pan_pos = pan_value;
          tilt_pos = tilt_value;
        }
      }
    }
    else if (result == remote_THREE) {
      if (writing_mode && delete_mode) {
        EEPROM.write(16, 255);
        EEPROM.write(17, 255);
        writing_mode = false;
        delete_mode = false;
        delay(500);
      }
      else if (writing_mode) {
        EEPROM.write(16, pan_pos);
        EEPROM.write(17, tilt_pos);
        writing_mode = false;
        delay(500);
      } else {
        byte pan_value = EEPROM.read(16);
        byte tilt_value = EEPROM.read(17);
        if (pan_value <= 180 && tilt_value <= tilt_max) {
          pan_pos = pan_value;
          tilt_pos = tilt_value;
        }
      }
    }
    else if (result == remote_FOUR) {
      if (writing_mode && delete_mode) {
        EEPROM.write(18, 255);
        EEPROM.write(19, 255);
        writing_mode = false;
        delete_mode = false;
        delay(500);
      }
      else if (writing_mode) {
        EEPROM.write(18, pan_pos);
        EEPROM.write(19, tilt_pos);
        writing_mode = false;
        delay(500);
      } else {
        byte pan_value = EEPROM.read(18);
        byte tilt_value = EEPROM.read(19);
        if (pan_value <= 180 && tilt_value <= tilt_max) {
          pan_pos = pan_value;
          tilt_pos = tilt_value;
        }
      }
    }
    else if (result == remote_FIVE) {
      if (writing_mode && delete_mode) {
        EEPROM.write(20, 255);
        EEPROM.write(21, 255);
        writing_mode = false;
        delete_mode = false;
        delay(500);
      }
      else if (writing_mode) {
        EEPROM.write(20, pan_pos);
        EEPROM.write(21, tilt_pos);
        writing_mode = false;
        delay(500);
      } else {
        byte pan_value = EEPROM.read(20);
        byte tilt_value = EEPROM.read(21);
        if (pan_value <= 180 && tilt_value <= tilt_max) {
          pan_pos = pan_value;
          tilt_pos = tilt_value;
        }
      }
    }
    else if (result == remote_SIX) {
      if (writing_mode && delete_mode) {
        EEPROM.write(22, 255);
        EEPROM.write(23, 255);
        writing_mode = false;
        delete_mode = false;
        delay(500);
      }
      else if (writing_mode) {
        EEPROM.write(22, pan_pos);
        EEPROM.write(23, tilt_pos);
        writing_mode = false;
        delay(500);
      } else {
        byte pan_value = EEPROM.read(22);
        byte tilt_value = EEPROM.read(23);
        if (pan_value <= 180 && tilt_value <= tilt_max) {
          pan_pos = pan_value;
          tilt_pos = tilt_value;
        }
      }
    }
    else if (result == remote_SEVEN) {
      if (writing_mode && delete_mode) {
        EEPROM.write(24, 255);
        EEPROM.write(25, 255);
        writing_mode = false;
        delete_mode = false;
        delay(500);
      }
      else if (writing_mode) {
        EEPROM.write(24, pan_pos);
        EEPROM.write(25, tilt_pos);
        writing_mode = false;
        delay(500);
      } else {
        byte pan_value = EEPROM.read(24);
        byte tilt_value = EEPROM.read(25);
        if (pan_value <= 180 && tilt_value <= tilt_max) {
          pan_pos = pan_value;
          tilt_pos = tilt_value;
        }
      }
    }
    else if (result == remote_EIGHT) {
      if (writing_mode && delete_mode) {
        EEPROM.write(26, 255);
        EEPROM.write(27, 255);
        writing_mode = false;
        delete_mode = false;
        delay(500);
      }
      else if (writing_mode) {
        EEPROM.write(26, pan_pos);
        EEPROM.write(27, tilt_pos);
        writing_mode = false;
        delay(500);
      } else {
        byte pan_value = EEPROM.read(26);
        byte tilt_value = EEPROM.read(27);
        if (pan_value <= 180 && tilt_value <= tilt_max) {
          pan_pos = pan_value;
          tilt_pos = tilt_value;
        }
      }
    }
    else if (result == remote_NINE) {
      if (writing_mode && delete_mode) {
        EEPROM.write(28, 255);
        EEPROM.write(29, 255);
        writing_mode = false;
        delete_mode = false;
        delay(500);
      }
      else if (writing_mode) {
        EEPROM.write(28, pan_pos);
        EEPROM.write(29, tilt_pos);
        writing_mode = false;
        delay(500);
      } else {
        byte pan_value = EEPROM.read(28);
        byte tilt_value = EEPROM.read(29);
        if (pan_value <= 180 && tilt_value <= tilt_max) {
          pan_pos = pan_value;
          tilt_pos = tilt_value;
        }
      }
    }
    if (!writing_mode && delete_mode) {
      delete_mode = false;
    }

    move_servos();
    irrecv.resume();
  } else if (checkFrequencies) {
    if (irrecv.decode(&results)) {
      result = String(results.value, HEX).substring(2);
      Serial.println(result);
    }
  }
}

void move_servos() {
  if (pan_pos > 180) {
    Serial.print("Pan overdrive: ");
    Serial.println(pan_pos);
    pan_pos = 180;
  }
  if (tilt_pos > tilt_max) {
    Serial.print("Tilt overdrive: ");
    Serial.print(tilt_pos);
    Serial.print(">");
    Serial.println(tilt_max);
    tilt_pos = tilt_max;
  }
  if (pan.attached() && old_pan_pos != pan_pos) {
    Serial.print("Moving pan to ");
    Serial.print(pan_pos);
    Serial.println("°");
    pan.write(pan_pos);
    old_pan_pos = pan_pos;
  }
  if (tilt.attached() && old_tilt_pos != tilt_pos) {
    Serial.print("Moving tilt to ");
    Serial.print(tilt_pos);
    Serial.println("°");
    tilt.write(tilt_pos);
    old_tilt_pos = tilt_pos;
  }
}

void write_delete_check() {
  if (writing_mode || delete_mode) {
    writing_mode = false;
    delete_mode = false;
  }
}