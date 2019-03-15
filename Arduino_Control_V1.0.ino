/* Digital In from VEX PIC */
const int in1 = 52;
const int in2 = 50;
const int in3 = 48;
const int in4 = 46;
const int in5 = 44;
const int in6 = 42;
const int in7 = 40;
const int in8 = 38;
const int in9 = 36;
const int in10 = 34;

/* Digital Out to Arm */
const int armLatch = 53;
const int armChannel0 = 37;
const int armChannel1 = 49;
const int armChannel2 = 33;
const int armPhaseA = 25;
const int armPhaseB = 41;
const int armPhaseC = 29;
const int armPhaseD = 45;

/* Motor Definitions */
enum 

/* State Position Values */
struct States {
  const int TURNTABLE;
  const int LOWER_ARM;
  const int UPPER_ARM;
  const int CLAW_ROTATE;
  const int CLAW;
};
States state1 = { 0, 0, 0, 0, 0 };
States state2 = { 0, 0, 0, 0, 0 };
States state3 = { 0, 0, 0, 0, 0 };

/* User Variables */
struct Steps {
  int turntable;
  int lowerArm;
  int upperArm;
  int clawRotate;
  int claw;
}
Steps steps = { 0, 0, 0, 0, 0 };

struct SetPositions {
  int turntable;
  int lowerArm;
  int upperArm;
  int clawRotate;
  int claw;
};
SetPositions setPositions = { 0, 0, 0, 0, 0 };

boolean positionControl = false;

void setup() {
  pinMode(in1, INPUT);
  pinMode(in2, INPUT);
  pinMode(in3, INPUT);
  pinMode(in4, INPUT);
  pinMode(in5, INPUT);
  pinMode(in6, INPUT);
  pinMode(in7, INPUT);
  pinMode(in8, INPUT);
  pinMode(in9, INPUT);
  pinMode(in10, INPUT);

  pinMode(armLatch, OUTPUT);
  pinMode(armChannel0, OUTPUT);
  pinMode(armChannel1, OUTPUT);
  pinMode(armChannel2, OUTPUT);
  pinMode(armPhaseA, OUTPUT);
  pinMode(armPhaseB, OUTPUT);
  pinMode(armPhaseC, OUTPUT);
  pinMode(armPhaseD, OUTPUT);
}

/* User Methods */
void sendMotor(

void armPositionControl(int state) {
  switch(state) {
    case 0:
      positionControl = false;
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
    case 3:
      positionContrl = true;
      setPositions.turntable = state3.TURNTABLE;
      setPositions.lowerArm = state3.LOWER_ARM;
      setPositions.upperArm = state3.UPPER_ARM;
      setPositions.clawRotate = state3.CLAW_ROTATE;
      setPositions.claw = state3.CLAW;
      break;
  }

  if (positionControl) {
    if (steps.turntable != setPositions.turntable) {
      if (steps.turntable > setPositions.turntable) {
        
      } else {
        
      }
    }
  }
}

void loop() {
  /* 
   * Received Values: 
   * Motor States (Enabled/Disabled)
   * Motor Direction (Forward/Reverse)
   * State Control (000, 001, 010)
   */ 
  
}
