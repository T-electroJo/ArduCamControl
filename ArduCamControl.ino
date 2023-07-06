#include <IRremote.h>
#include <Servo.h>
#include <EEPROM.h>

#include "remote.h"

const int servo_pan_pin = 2;
const int servo_tilt_pin = 3;
const int ir_pin = 4;

int default_pan_pos = 90;
int default_tilt_pos = 90;
int pan_pos = default_pan_pos;
int tilt_pos = default_tilt_pos;
const int tilt_max = 150;

bool status = false;
bool highspeed = false;
bool writing_mode = false;
bool delete_mode = false;

Servo pan;
Servo tilt;
IRrecv irrecv(ir_pin);
decode_results results;
String result;

void setup() {
  irrecv.enableIRIn();
  pan.attach(servo_pan_pin);
  tilt.attach(servo_tilt_pin);
  move_servos();
}

void loop() {
  if (irrecv.decode(&results)) {
    result = String(results.value, HEX).substring(2);
    
    if (result == remote_UP) {
      if (writing_mode || delete_mode) {
        writing_mode = false;
        delete_mode = false;
      }
      if (tilt_pos < tilt_max) {
        tilt_pos++;
        if (highspeed && tilt_pos < tilt_max-4) {
          tilt_pos += 4;
        }
      }
    }
    else if (result == remote_DOWN) {
      if (writing_mode || delete_mode) {
        writing_mode = false;
        delete_mode = false;
      }
      if (tilt_pos > 0) {
        tilt_pos--;
        if (highspeed && tilt_pos > 4) {
          tilt_pos -= 4;
        }
      }
    }
    else if (result == remote_LEFT) {
      if (writing_mode || delete_mode) {
        writing_mode = false;
        delete_mode = false;
      }
      if (pan_pos < 180) {
        pan_pos++;
        if (highspeed && pan_pos < 176) {
          pan_pos += 4;
        }
      }
    }
    else if (result == remote_RIGHT) {
      if (writing_mode || delete_mode) {
        writing_mode = false;
        delete_mode = false;
      }
      if (pan_pos > 0) {
        pan_pos--;
        if (highspeed && pan_pos > 4) {
          pan_pos -= 4;
        }
      }
    }
    else if (result == remote_SOURCE) {
      if (writing_mode || delete_mode) {
        writing_mode = false;
        delete_mode = false;
      }
      pan_pos = default_pan_pos;
      tilt_pos = default_tilt_pos;
    }
    else if (result == remote_MINIMIZE) {
      if (writing_mode || delete_mode) {
        writing_mode = false;
        delete_mode = false;
      }
      highspeed = !highspeed;
      delay(500);
    }
    else if (result == remote_TV) {
      if (writing_mode || delete_mode) {
        writing_mode = false;
        delete_mode = false;
      }
      if (pan.attached() || tilt.attached()) {
        pan.detach();
        tilt.detach();
      } else {
        pan.attach(servo_pan_pin);
        tilt.attach(servo_tilt_pin);
      }
      delay(500);
    }
    else if (result == remote_MUTE) {
      if (writing_mode) {
        delete_mode = true;
      }
    }
    else if (result == remote_RADIO) {
      writing_mode = !writing_mode;
      delay(500);
    }
    else if (result == remote_ZERO) {
      if (writing_mode && delete_mode) {
        EEPROM.write(0, 255);
        EEPROM.write(1, 255);
        writing_mode = false;
        delete_mode = false;
      }
      else if (writing_mode) {
        EEPROM.write(0, pan_pos);
        EEPROM.write(1, tilt_pos);
        writing_mode = false;
      } else {
        byte pan_value = EEPROM.read(0);
        byte tilt_value = EEPROM.read(1);
        if (pan_value <= 180 && tilt_value <= tilt_max) {
          pan_pos = pan_value;
          tilt_pos = tilt_value;
        }
      }
    }
    else if (result == remote_ONE) {
      if (writing_mode && delete_mode) {
        EEPROM.write(2, 255);
        EEPROM.write(3, 255);
        writing_mode = false;
        delete_mode = false;
      }
      else if (writing_mode) {
        EEPROM.write(2, pan_pos);
        EEPROM.write(3, tilt_pos);
        writing_mode = false;
      } else {
        byte pan_value = EEPROM.read(2);
        byte tilt_value = EEPROM.read(3);
        if (pan_value <= 180 && tilt_value <= tilt_max) {
          pan_pos = pan_value;
          tilt_pos = tilt_value;
        }
      }
    }
    else if (result == remote_TWO) {
      if (writing_mode && delete_mode) {
        EEPROM.write(4, 255);
        EEPROM.write(5, 255);
        writing_mode = false;
        delete_mode = false;
      }
      else if (writing_mode) {
        EEPROM.write(4, pan_pos);
        EEPROM.write(5, tilt_pos);
        writing_mode = false;
      } else {
        byte pan_value = EEPROM.read(4);
        byte tilt_value = EEPROM.read(5);
        if (pan_value <= 180 && tilt_value <= tilt_max) {
          pan_pos = pan_value;
          tilt_pos = tilt_value;
        }
      }
    }
    else if (result == remote_THREE) {
      if (writing_mode && delete_mode) {
        EEPROM.write(6, 255);
        EEPROM.write(7, 255);
        writing_mode = false;
        delete_mode = false;
      }
      else if (writing_mode) {
        EEPROM.write(6, pan_pos);
        EEPROM.write(7, tilt_pos);
        writing_mode = false;
      } else {
        byte pan_value = EEPROM.read(6);
        byte tilt_value = EEPROM.read(7);
        if (pan_value <= 180 && tilt_value <= tilt_max) {
          pan_pos = pan_value;
          tilt_pos = tilt_value;
        }
      }
    }
    else if (result == remote_FOUR) {
      if (writing_mode && delete_mode) {
        EEPROM.write(8, 255);
        EEPROM.write(9, 255);
        writing_mode = false;
        delete_mode = false;
      }
      else if (writing_mode) {
        EEPROM.write(8, pan_pos);
        EEPROM.write(9, tilt_pos);
        writing_mode = false;
      } else {
        byte pan_value = EEPROM.read(8);
        byte tilt_value = EEPROM.read(9);
        if (pan_value <= 180 && tilt_value <= tilt_max) {
          pan_pos = pan_value;
          tilt_pos = tilt_value;
        }
      }
    }
    else if (result == remote_FIVE) {
      if (writing_mode && delete_mode) {
        EEPROM.write(10, 255);
        EEPROM.write(11, 255);
        writing_mode = false;
        delete_mode = false;
      }
      else if (writing_mode) {
        EEPROM.write(10, pan_pos);
        EEPROM.write(11, tilt_pos);
        writing_mode = false;
      } else {
        byte pan_value = EEPROM.read(10);
        byte tilt_value = EEPROM.read(11);
        if (pan_value <= 180 && tilt_value <= tilt_max) {
          pan_pos = pan_value;
          tilt_pos = tilt_value;
        }
      }
    }
    else if (result == remote_SIX) {
      if (writing_mode && delete_mode) {
        EEPROM.write(12, 255);
        EEPROM.write(13, 255);
        writing_mode = false;
        delete_mode = false;
      }
      else if (writing_mode) {
        EEPROM.write(12, pan_pos);
        EEPROM.write(13, tilt_pos);
        writing_mode = false;
      } else {
        byte pan_value = EEPROM.read(12);
        byte tilt_value = EEPROM.read(13);
        if (pan_value <= 180 && tilt_value <= tilt_max) {
          pan_pos = pan_value;
          tilt_pos = tilt_value;
        }
      }
    }
    else if (result == remote_SEVEN) {
      if (writing_mode && delete_mode) {
        EEPROM.write(14, 255);
        EEPROM.write(15, 255);
        writing_mode = false;
        delete_mode = false;
      }
      else if (writing_mode) {
        EEPROM.write(14, pan_pos);
        EEPROM.write(15, tilt_pos);
        writing_mode = false;
      } else {
        byte pan_value = EEPROM.read(14);
        byte tilt_value = EEPROM.read(15);
        if (pan_value <= 180 && tilt_value <= tilt_max) {
          pan_pos = pan_value;
          tilt_pos = tilt_value;
        }
      }
    }
    else if (result == remote_EIGHT) {
      if (writing_mode && delete_mode) {
        EEPROM.write(16, 255);
        EEPROM.write(17, 255);
        writing_mode = false;
        delete_mode = false;
      }
      else if (writing_mode) {
        EEPROM.write(16, pan_pos);
        EEPROM.write(17, tilt_pos);
        writing_mode = false;
      } else {
        byte pan_value = EEPROM.read(16);
        byte tilt_value = EEPROM.read(17);
        if (pan_value <= 180 && tilt_value <= tilt_max) {
          pan_pos = pan_value;
          tilt_pos = tilt_value;
        }
      }
    }
    else if (result == remote_NINE) {
      if (writing_mode && delete_mode) {
        EEPROM.write(18, 255);
        EEPROM.write(19, 255);
        writing_mode = false;
        delete_mode = false;
      }
      else if (writing_mode) {
        EEPROM.write(18, pan_pos);
        EEPROM.write(19, tilt_pos);
        writing_mode = false;
      } else {
        byte pan_value = EEPROM.read(18);
        byte tilt_value = EEPROM.read(19);
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
  }
}

void move_servos() {
  if (pan.attached()) {
    pan.write(pan_pos);
  }
  if (tilt.attached()) {
    tilt.write(tilt_pos);
  }
}
