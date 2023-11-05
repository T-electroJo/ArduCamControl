#include <IRremote.hpp>
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
const int disable_time = 5000; // Servo disable time in milliseconds after last move.
unsigned long lastmillis = 0;
const int automatic_pan_slow_skips = 50;
int automatic_pan_slow_skipped = 0;

bool automatic_pan_dir = false;
bool automatic_pan = false;
bool disable_servos = false;
bool highspeed = false;
bool writing_mode = false;
bool delete_mode = false;

bool checkFrequencies = false; // Configuration mode, more on github: https://github.com/T-electroJo/ArduCamControl


Servo pan;
Servo tilt;

String result;

void setup() {
  Serial.begin(9600);
  Serial.println("Setup begin...");
  IrReceiver.begin(ir_pin, ENABLE_LED_FEEDBACK); // Enable IR Input with LED_BUILTIN as receive indicator

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
  if (disable_servos) {
    if (millis() - lastmillis > disable_time) {
      disable_servos = false;
      if (pan.attached()) {
        pan.detach();
      }
      if (tilt.attached()) {
        tilt.detach();
      }
      Serial.println("--------- Servos deactivated ---------");
      lastmillis = millis();
    }
  }
  if (automatic_pan) {
    if (automatic_pan_dir) {
      if (pan_pos > 0) {
        if (highspeed) {
          pan_pos--;
        } else {
          if (automatic_pan_slow_skipped < automatic_pan_slow_skips) {
            automatic_pan_slow_skipped++;
          } else {
            automatic_pan_slow_skipped = 0;
            pan_pos--;
          }
        }
      } else {
        automatic_pan_dir = false;
      }
    } else {
      if (pan_pos < 180) {
        if (highspeed) {
          pan_pos++;
        } else {
          if (automatic_pan_slow_skipped < automatic_pan_slow_skips) {
            automatic_pan_slow_skipped++;
          } else {
            automatic_pan_slow_skipped = 0;
            pan_pos++;
          }
        }
      } else {
        automatic_pan_dir = true;
      }
    }
    move_servos();
  }
  if (IrReceiver.decode() && !checkFrequencies) {
    result = IrReceiver.decodedIRData.command;
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
      pan_pos = 90;
      tilt_pos = 90;
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
      automatic_pan = !automatic_pan;
      delay(500);
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
    IrReceiver.resume();
  } else if (checkFrequencies) {
    if (IrReceiver.decode()) {
      Serial.println(IrReceiver.decodedIRData.command);
    }
  }
}

void move_servos() {
  if (old_pan_pos != pan_pos) {
    pan.attached() ?: pan.attach(servo_pan_pin);
    Serial.print("Moving pan to ");
    Serial.print(pan_pos);
    Serial.println("°");
    pan.write(pan_pos);
  }
  if (old_tilt_pos != tilt_pos) {
    tilt.attached() ?: tilt.attach(servo_tilt_pin);
    Serial.print("Moving tilt to ");
    Serial.print(tilt_pos);
    Serial.println("°");
    tilt.write(tilt_pos);
  }
  if (old_pan_pos != pan_pos || old_tilt_pos != tilt_pos) {
    disable_servos = true;
    lastmillis = millis();
  }

  old_pan_pos = pan_pos;
  old_tilt_pos = tilt_pos;
}

void write_delete_check() {
  if (writing_mode || delete_mode) {
    writing_mode = false;
    delete_mode = false;
  }
}