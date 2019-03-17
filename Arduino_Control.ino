/* Digital In from VEX PIC */
const int vexClawOn = 52;
const int vexClawRotateOn = 50;
const int vexLowerArmOn = 48;
const int vexUpperArmOn = 46;
const int vexTurntableOn = 44;
const int vexClawForward = 42;
const int vexClawRotateForward = 40;
const int vexLowerArmForward = 38;
const int vexUpperArmForward = 36;
const int vexTurntableForward = 34;

const int vexState0 = 28;
const int vexState1 = 26;

const int vexValidRuntime = 30;

const int turntableSwitch = 22; 
const int lowerArmSwitch = 23;
const int upperArmSwitch = 24;

/* Digital Out to Arm */
const int armSendSignal = 53;
const int armChannel0 = 37;
const int armChannel1 = 49;
const int armChannel2 = 33;
const int armPhaseA = 25;
const int armPhaseB = 41;
const int armPhaseC = 29;
const int armPhaseD = 45;

/* Motor Definitions */
enum Motors {
  TURNTABLE, LOWER_ARM, UPPER_ARM, CLAW_ROTATE, CLAW
};
Motors motor;

/* State Position Values */
struct States {
  const int TURNTABLE;
  const int LOWER_ARM;
  const int UPPER_ARM;
  const int CLAW_ROTATE;
  const int CLAW;
};
States state1 = { 0, 0, 0, 0, 0 }; // Position 1
States state2 = { 0, 0, 0, 0, 0 }; // Position 2
States start = { 825, 250, 150, 0, 0 }; // Starting position

/* User Variables */
struct Steps {
  int turntable;
  int lowerArm;
  int upperArm;
  int clawRotate;
  int claw;
};
Steps steps = { 0, 0, 0, 0, 0 }; // Tracking steps for each stepper motor

struct SetPositions {
  int turntable;
  int lowerArm;
  int upperArm;
  int clawRotate;
  int claw;
};
SetPositions setPositions = { 0, 0, 0, 0, 0 }; // Positions used during preset control

boolean positionControl = false; // Boolean to track if we are using presets, or if we are doing manual control
int timeDelay = 1; // Time delay for stepping, in milliseconds
long validRuntimeHigh = 0;
long validRuntimeLow = 0;

void setup() {
  pinMode(vexClawOn, INPUT);
  pinMode(vexClawRotateOn, INPUT);
  pinMode(vexLowerArmOn, INPUT);
  pinMode(vexUpperArmOn, INPUT);
  pinMode(vexTurntableOn, INPUT);
  pinMode(vexClawForward, INPUT);
  pinMode(vexClawRotateForward, INPUT);
  pinMode(vexLowerArmForward, INPUT);
  pinMode(vexUpperArmForward, INPUT);
  pinMode(vexTurntableForward, INPUT);
  
  pinMode(vexState0, INPUT);
  pinMode(vexState1, INPUT);

  pinMode(vexValidRuntime, INPUT);

  pinMode(turntableSwitch, INPUT);
  pinMode(lowerArmSwitch, INPUT);
  pinMode(upperArmSwitch, INPUT);
    
  pinMode(armSendSignal, OUTPUT);
  pinMode(armChannel0, OUTPUT); // LSB, right side
  pinMode(armChannel1, OUTPUT);
  pinMode(armChannel2, OUTPUT);
  pinMode(armPhaseA, OUTPUT);
  pinMode(armPhaseB, OUTPUT);
  pinMode(armPhaseC, OUTPUT);
  pinMode(armPhaseD, OUTPUT);

  zeroArm();
  //Serial.begin(9600);
}

/* User Methods */
void motorSignal(int value1, int value2, int value3) {
  digitalWrite(armChannel0, value1);
  digitalWrite(armChannel1, value2);
  digitalWrite(armChannel2, value3);
}

void sendMotor(Motors selectedMotor) {
  switch(selectedMotor) {
    case TURNTABLE:
      motorSignal(LOW, HIGH, HIGH);
      break;
    case LOWER_ARM:
      motorSignal(HIGH, LOW, HIGH);
      break;
    case UPPER_ARM:
      motorSignal(LOW, LOW, HIGH);
      break;
    case CLAW_ROTATE:
      motorSignal(LOW, HIGH, LOW);
      break;
    case CLAW:
      motorSignal(HIGH, LOW, LOW);
      break; 
  }
}

void phaseOutput(int motorA, int motorB, int motorC, int motorD) {
  digitalWrite(armPhaseA, motorA);
  digitalWrite(armPhaseB, motorB);
  digitalWrite(armPhaseC, motorC);
  digitalWrite(armPhaseD, motorD);
}

void phaseStep(int phase) {
  digitalWrite(armSendSignal, HIGH);
  switch(phase) {
    case 0:
      phaseOutput(HIGH, LOW, LOW, LOW);
      break;
    case 1:
      phaseOutput(HIGH, HIGH, LOW, LOW);
      break;
    case 2:
      phaseOutput(LOW, HIGH, LOW, LOW);
      break;
    case 3:
      phaseOutput(LOW, HIGH, HIGH, LOW);
      break;
    case 4:
      phaseOutput(LOW, LOW, HIGH, LOW);
      break;
    case 5:
      phaseOutput(LOW, LOW, HIGH, HIGH);
      break;
    case 6:
      phaseOutput(LOW, LOW, LOW, HIGH);
      break;
    case 7:
      phaseOutput(HIGH, LOW, LOW, HIGH);
      break;
  }
  digitalWrite(armSendSignal, LOW);
  delay(timeDelay);
}

void sendForwardStep() {
  for(int i = 0; i <= 7; i++) {
    phaseStep(i);
  }
}

void sendReverseStep() {
  for(int i = 7; i >= 0; i--) {
    phaseStep(i);
  }
}

int readControlState() {
  boolean LSB; // Least Significant Bit
  boolean MSB; // Most Significant Bit
  
  if (digitalRead(vexState0) == HIGH) {
    LSB = true;
  } else {
    LSB = false;
  }

  if (digitalRead(vexState1) == HIGH) {
    MSB = true;
  } else {
    MSB = false;
  }

  if (!MSB && !LSB) { // State 0
    return 0;
  } else if (!MSB && LSB) { // State 1
    return 1;
  } else if (MSB && !LSB) { // State 2
    return 3; // Set to 3 for testing preset
  } else { // Error Condition - Unknown State
    Serial.print("Unknown Condition");
    return 0;
  }
}

boolean isAnyMotorEnabled() {
  if (digitalRead(vexClawOn) == HIGH) {
    return true;
  }
  
  if (digitalRead(vexClawRotateOn) == HIGH) {
    return true;
  }
  
  if (digitalRead(vexLowerArmOn) == HIGH) {
    return true;
  } 
  
  if (digitalRead(vexUpperArmOn) == HIGH) {
    return true;
  } 
  
  if (digitalRead(vexTurntableOn) == HIGH) {
    return true;
  } 
  return false;
}

void manualControl() { // INVERT MOTOR DIRECTIONS IN HERE
  if (isAnyMotorEnabled()) {
    if (digitalRead(vexClawOn) == HIGH) {
      sendMotor(CLAW);
      //Serial.println("Claw On");
      if (digitalRead(vexClawForward) == HIGH) {
        sendForwardStep();
        steps.claw++;
      } else {
        sendReverseStep();
        steps.claw--;
      }
    }
  
    if (digitalRead(vexClawRotateOn) == HIGH) {
      sendMotor(CLAW_ROTATE);
      //Serial.println("Claw On");
      if (digitalRead(vexClawRotateForward) == HIGH) {
        sendReverseStep();
        steps.clawRotate++;
      } else {
        sendForwardStep();
        steps.clawRotate--;
      }
    }
  
    if (digitalRead(vexLowerArmOn) == HIGH) {
      sendMotor(LOWER_ARM);
      //Serial.println("Lower Arm On");
      if (digitalRead(vexLowerArmForward) == HIGH && digitalRead(lowerArmSwitch) == LOW) {
        sendForwardStep();
        steps.lowerArm--;
      } else if (digitalRead(vexLowerArmForward) == LOW) {
        sendReverseStep();
        steps.lowerArm++;
      }
    } 
  
    if (digitalRead(vexUpperArmOn) == HIGH) {
      sendMotor(UPPER_ARM);
      //Serial.println("Upper Arm On");
      if (digitalRead(vexUpperArmForward) == HIGH) {
        sendReverseStep();
        steps.upperArm--;
      } else if (digitalRead(vexUpperArmForward) == LOW && digitalRead(upperArmSwitch) == LOW) {
        sendForwardStep();
        steps.upperArm++;
      }
    } 
  
    if (digitalRead(vexTurntableOn) == HIGH) {
      sendMotor(TURNTABLE);
      //Serial.println("Turntable On");
      if (digitalRead(vexTurntableForward) == HIGH && digitalRead(turntableSwitch) == LOW) {
        sendForwardStep();
        steps.turntable--;
      } else if (digitalRead(vexTurntableForward) == LOW) {
        sendReverseStep();
        steps.turntable++;
      }
    }
  }
}

void zeroArm() {
  while (digitalRead(turntableSwitch) == LOW) {
    sendMotor(TURNTABLE);
    sendForwardStep();
  }
  steps.turntable = 0;

  while (digitalRead(lowerArmSwitch) == LOW) {
    sendMotor(LOWER_ARM);
    sendForwardStep();
  }
  steps.lowerArm = 0;

  while (digitalRead(upperArmSwitch) == LOW) {
    sendMotor(UPPER_ARM);
    sendForwardStep();
  }
  steps.upperArm = 0;
}

void armPositionControl(int state) {
  switch(state) {
    case 0:
      positionControl = false;
      manualControl();
      break;
    case 1:
      positionControl = true;
      setPositions.turntable = state1.TURNTABLE;
      setPositions.lowerArm = state1.LOWER_ARM;
      setPositions.upperArm = state1.UPPER_ARM;
      setPositions.clawRotate = state1.CLAW_ROTATE;
      setPositions.claw = state1.CLAW;
      break;
    case 2:
      positionControl = true;
      setPositions.turntable = state2.TURNTABLE;
      setPositions.lowerArm = state2.LOWER_ARM;
      setPositions.upperArm = state2.UPPER_ARM;
      setPositions.clawRotate = state2.CLAW_ROTATE;
      setPositions.claw = state2.CLAW;
      break;
    case 3: // Move arm to display position
      positionControl = true;
      setPositions.turntable = start.TURNTABLE;
      setPositions.lowerArm = start.LOWER_ARM;
      setPositions.upperArm = start.UPPER_ARM;
      setPositions.clawRotate = start.CLAW_ROTATE;
      setPositions.claw = start.CLAW;
      break;
    case 4: // Move arm to home position
      zeroArm();
      break;
  }

  if (positionControl) {
    if (steps.turntable != setPositions.turntable) {
      sendMotor(TURNTABLE);
      if (steps.turntable > setPositions.turntable) {
        sendForwardStep();
        steps.turntable--;
      } else {
        sendReverseStep();
        steps.turntable++;
      }
    }

    if (steps.lowerArm != setPositions.lowerArm) {
      sendMotor(LOWER_ARM);
      if (steps.lowerArm > setPositions.lowerArm) {
        sendForwardStep();
        steps.lowerArm--;
      } else {
        sendReverseStep();
        steps.lowerArm++;
      }
    }

    if (steps.upperArm != setPositions.upperArm) {
      sendMotor(UPPER_ARM);
      if (steps.upperArm > setPositions.upperArm) {
        sendReverseStep();
        steps.upperArm--;
      } else {
        sendForwardStep();
        steps.upperArm++;
      }
    }

    /*
    if (steps.clawRotate != setPositions.clawRotate) {
      sendMotor(CLAW_ROTATE);
      if (steps.clawRotate > setPositions.clawRotate) {
        sendForwardStep();
        steps.clawRotate--;
      } else {
        sendReverseStep();
        steps.clawRotate++;
      }
    }

    if (steps.claw != setPositions.claw) {
      sendMotor(CLAW);
      if (steps.claw > setPositions.claw) {
        sendReverseStep();
        steps.claw--;
      } else {
        sendForwardStep();
        steps.claw++;
      }
    }
    */
  }
}

boolean isValidRuntime() {
  if (digitalRead(vexValidRuntime) == HIGH) {
    validRuntimeHigh = millis();
  } else {
    validRuntimeLow = millis();
  }

  if ((millis() - validRuntimeHigh) > 1000 || (millis() - validRuntimeLow) > 1000) {
    return false;
  }
  return true;
}

void debug() {
  Serial.print(steps.lowerArm);
  Serial.print(" ");
  Serial.print(steps.upperArm);
  Serial.print(" ");
  Serial.println(steps.turntable);
}

void loop() {
  /* 
   * Received Values: 
   * Motor States (Enabled/Disabled)
   * Motor Direction (Forward/Reverse)
   * State Control (000, 001, 010)
   */
  if (isValidRuntime()) {
    armPositionControl(readControlState());
  } else {
    armPositionControl(4); // Resets the arm to home position if the VEX PIC disconnects
  }
}
