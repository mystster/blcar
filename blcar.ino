#include <Ps3Controller.h>

// left wheel
uint8_t ena = 14;
uint8_t in1 = 27;
uint8_t in2 = 26;

// right wheel
uint8_t in3 = 25;
uint8_t in4 = 33;
uint8_t enb = 32;

uint8_t rightPwmChannel = 0;
uint8_t leftPwmChannel = 1;
double pwmFreq = 10000;
uint8_t pwmBit = 7;

int battery = 0;

struct wheelData {
    int now;
    int last;
};

wheelData left, right;

void controlWheel(int ry, int ly) {
    auto cw = [](int y, uint8_t inA, uint8_t inB, uint8_t pwmChannel, wheelData *data) {
        if (abs(y) < 10) {
            y = 0;
        }else {
            y = -y;
        }
        if (y * data->last >= 0) {
            data->last = data->now;
            data->now = y;

            if (data->now > 0) {
                digitalWrite(inA, HIGH);
                digitalWrite(inB, LOW);
                ledcWrite(pwmChannel, data->now);
            } else if (data->now < 0) {
                digitalWrite(inA, LOW);
                digitalWrite(inB, HIGH);
                ledcWrite(pwmChannel, -data->now);
            } else {
                digitalWrite(inA, LOW);
                digitalWrite(inB, LOW);
            }
        } else {
            data->last = y;
        }
        Serial.print(data->now);
    };
    Serial.print("left:");
    cw(ly, in1, in2, leftPwmChannel, &left);
    Serial.print(", right:");
    cw(ry, in3, in4, rightPwmChannel, &right);
    Serial.println();
}

void ps3ControllerNotify() {
    //--- Digital cross/square/triangle/circle button events ---
    if (Ps3.event.button_down.cross)
        Serial.println("Started pressing the cross button");
    if (Ps3.event.button_up.cross) Serial.println("Released the cross button");

    if (Ps3.event.button_down.square)
        Serial.println("Started pressing the square button");
    if (Ps3.event.button_up.square)
        Serial.println("Released the square button");

    if (Ps3.event.button_down.triangle)
        Serial.println("Started pressing the triangle button");
    if (Ps3.event.button_up.triangle)
        Serial.println("Released the triangle button");

    if (Ps3.event.button_down.circle)
        Serial.println("Started pressing the circle button");
    if (Ps3.event.button_up.circle)
        Serial.println("Released the circle button");

    //--------------- Digital D-pad button events --------------
    if (Ps3.event.button_down.up)
        Serial.println("Started pressing the up button");
    if (Ps3.event.button_up.up) Serial.println("Released the up button");

    if (Ps3.event.button_down.right)
        Serial.println("Started pressing the right button");
    if (Ps3.event.button_up.right) Serial.println("Released the right button");

    if (Ps3.event.button_down.down)
        Serial.println("Started pressing the down button");
    if (Ps3.event.button_up.down) Serial.println("Released the down button");

    if (Ps3.event.button_down.left)
        Serial.println("Started pressing the left button");
    if (Ps3.event.button_up.left) Serial.println("Released the left button");

    //------------- Digital shoulder button events -------------
    if (Ps3.event.button_down.l1)
        Serial.println("Started pressing the left shoulder button");
    if (Ps3.event.button_up.l1)
        Serial.println("Released the left shoulder button");

    if (Ps3.event.button_down.r1)
        Serial.println("Started pressing the right shoulder button");
    if (Ps3.event.button_up.r1)
        Serial.println("Released the right shoulder button");

    //-------------- Digital trigger button events -------------
    if (Ps3.event.button_down.l2)
        Serial.println("Started pressing the left trigger button");
    if (Ps3.event.button_up.l2)
        Serial.println("Released the left trigger button");

    if (Ps3.event.button_down.r2)
        Serial.println("Started pressing the right trigger button");
    if (Ps3.event.button_up.r2)
        Serial.println("Released the right trigger button");

    //--------------- Digital stick button events --------------
    if (Ps3.event.button_down.l3)
        Serial.println("Started pressing the left stick button");
    if (Ps3.event.button_up.l3)
        Serial.println("Released the left stick button");

    if (Ps3.event.button_down.r3)
        Serial.println("Started pressing the right stick button");
    if (Ps3.event.button_up.r3)
        Serial.println("Released the right stick button");

    //---------- Digital select/start/ps button events ---------
    if (Ps3.event.button_down.select)
        Serial.println("Started pressing the select button");
    if (Ps3.event.button_up.select)
        Serial.println("Released the select button");

    if (Ps3.event.button_down.start)
        Serial.println("Started pressing the start button");
    if (Ps3.event.button_up.start) Serial.println("Released the start button");

    if (Ps3.event.button_down.ps)
        Serial.println("Started pressing the Playstation button");
    if (Ps3.event.button_up.ps)
        Serial.println("Released the Playstation button");

    //---------------- Analog stick value events ---------------
    if (abs(Ps3.event.analog_changed.stick.lx) +
                abs(Ps3.event.analog_changed.stick.ly) >
            2 ||
        abs(Ps3.event.analog_changed.stick.rx) +
                abs(Ps3.event.analog_changed.stick.ry) >
            2) {
        // int w = Ps3.data.analog.stick.ly;
        // Serial.print("Moved the left stick:");
        // Serial.print(" x=");
        // Serial.print(Ps3.data.analog.stick.lx, DEC);
        // Serial.print(" y=");
        // Serial.print(w, DEC);
        // Serial.println();

        controlWheel(Ps3.data.analog.stick.ry, Ps3.data.analog.stick.ly);
    }

    // if (abs(Ps3.event.analog_changed.stick.rx) +
    //         abs(Ps3.event.analog_changed.stick.ry) >
    //     2) {
    //     Serial.print("Moved the right stick:");
    //     Serial.print(" x=");
    //     Serial.print(Ps3.data.analog.stick.rx, DEC);
    //     Serial.print(" y=");
    //     Serial.print(Ps3.data.analog.stick.ry, DEC);
    //     Serial.println();
    // }
    //---------------------- Battery events ---------------------
    if (battery != Ps3.data.status.battery) {
        battery = Ps3.data.status.battery;
        Serial.print("The controller battery is ");
        if (battery == ps3_status_battery_charging)
            Serial.println("charging");
        else if (battery == ps3_status_battery_full){
            Serial.println("FULL");
            Ps3.setPlayer(1);
        }
        else if (battery == ps3_status_battery_high){
            Serial.println("HIGH");
            Ps3.setPlayer(2);
        }
        else if (battery == ps3_status_battery_low){
            Serial.println("LOW");
            Ps3.setPlayer(3);
        }
        else if (battery == ps3_status_battery_dying){
            Serial.println("DYING");
            Ps3.setPlayer(4);
        }
        else if (battery == ps3_status_battery_shutdown){
            Serial.println("SHUTDOWN");
            Ps3.setPlayer(5);
        }
        else{
            Serial.println("UNDEFINED");
            Ps3.setPlayer(10);
        }
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("setting up!!!");
    Serial.println("PS3 controller setup");
    Ps3.attach(ps3ControllerNotify);
    Ps3.attachOnConnect([]() { Serial.println("Connected."); });
    Ps3.begin("08:3A:F2:AC:24:72");

    Serial.println("circuit setup");
    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);
    digitalWrite(in1, HIGH);
    digitalWrite(in2, HIGH);
    ledcSetup(leftPwmChannel, pwmFreq, pwmBit);
    ledcSetup(rightPwmChannel, pwmFreq, pwmBit);
    ledcAttachPin(ena, leftPwmChannel);
    ledcAttachPin(enb, rightPwmChannel);

    Serial.println("Ready.");
}

void loop() {
    if (!Ps3.isConnected()) return;
    //Serial.println(leftWheel);
    // if(leftWheel > 0){
    //     digitalWrite(in1, HIGH);
    //     digitalWrite(in2, LOW);
    // }else if(leftWheel < 0){
    //     digitalWrite(in1, LOW);
    //     digitalWrite(in2, HIGH);
    // }else{
    //     digitalWrite(in1, LOW);
    //     digitalWrite(in2, LOW);
    // }

    delay(200);
}
