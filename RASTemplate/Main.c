#include <raslib/inc/common.h>
#include <raslib/inc/gpio.h>
#include <raslib/inc/time.h>
#include <raslib/inc/motor.h>
#include <raslib/inc/linesensor.h>
#include <raslib/inc/adc.h>

#include "RASDemo.h"

tBoolean led_on;

int blink= 4;

void blinky(void) {
    if (blink ==1){
        SetPin(PIN_F1, led_on);
        SetPin(PIN_F2, 0);
        SetPin(PIN_F3, 0);
    }
    if (blink ==2){
        SetPin(PIN_F3, 0);
        SetPin(PIN_F2, led_on);
        SetPin(PIN_F1, 0);
    }
    if (blink ==3){
        SetPin(PIN_F1, 0);
        SetPin(PIN_F2, 0);
        SetPin(PIN_F3, led_on);
    }
     if(blink==4)
     {
         SetPin(PIN_F3, led_on);
         SetPin(PIN_F2, led_on);
         SetPin(PIN_F1, led_on);
     }
    led_on = !led_on;
  }
static tMotor *left;
static tMotor *right;

static tADC *adc[8];
static tADC *adc2[2];
static tBoolean initialized = false;

void initGPIOLineSensor(void)
{
    if(initialized)
    {
        return;
    }
    initialized = true;

    //set four pins for ADC input - you can use these 4 I/O pins or select your own
   //we need 8 pins for our line follower
    adc[0] = InitializeADC(PIN_E4);
    adc[1] = InitializeADC(PIN_E5);
    adc[2] = InitializeADC(PIN_D2);
    adc[3] = InitializeADC(PIN_D3);
    adc[4] = InitializeADC(PIN_E1);
    adc[5] = InitializeADC(PIN_E2);
    adc[6] = InitializeADC(PIN_E3);
    adc[7] = InitializeADC(PIN_B5);

}

void gpioLineSensorDemo(void){
    Printf("Press any key to quit\n");

    //loop until key is pressed
    while(!KeyWasPressed()){
        Printf(
             "Line Sensor values:  %1.3f,  %1.3f,  %1.3f,  %1.3f, %1.3f,  %1.3f,  %1.3f,  %1.3f\r",
            ADCRead(adc[0]),
            ADCRead(adc[1]),
            ADCRead(adc[2]),
            ADCRead(adc[3]),
            ADCRead(adc[4]),
            ADCRead(adc[5]),
            ADCRead(adc[6]),
            ADCRead(adc[7])
            );
    }
    Printf("\n");
}

void initIRSensor(void){
    //we don't want to accidentally reinitialize the GPIO pins again
    // if(initialized)
    //     return;
    // initialized = true;

    //set four pins for ADC input - you can use these 4 I/O pins or select your own
    adc2[0] = InitializeADC(PIN_B4);
    adc2[1] = InitializeADC(PIN_E0);

}
void IRSensorDemo(void){
    Printf("Press any key to quit\n");

    //loop until key is pressed
    while(!KeyWasPressed()){
        Printf(
            "IR values: %1.3f, %1.3f\r",
            ADCRead(adc2[0]),
           ADCRead(adc2[1])
            );
    }
    Printf("\n");
}
#define threshold 0.2

int main() {
    InitializeMCU();
    left = InitializeServoMotor(PIN_B6, false);
    right = InitializeServoMotor(PIN_B7, true);
    initGPIOLineSensor();
    //gpioLineSensorDemo();
    initIRSensor();
    CallEvery(blinky, 0, 0.20F);
    SetMotor(left,0.4);
    SetMotor(right,0.4);
    Wait(1);
    Printf("screen working");
    //gpioLineSensorDemo();
    //IRSensorDemo();
    tBoolean LineFollow= true;
    tBoolean passLineFollowing = false;
    while(1){

    // double IRBoi[2];
    // for (int x=0;x<2;x++)
    // {
    //     IRBoi[x]=ADCRead(adc2[x]);
    // }
    // double IRsum=-1.5*IRBoi[0]+1.5*IRBoi[1];
    // if (IRBoi[0]>.7)
    // {
    //     SetMotor(left,(.5+IRsum*(-1))/4);
    //     SetMotor(right,(.5-IRsum)/4);
    // }
    // else
    // {
    //     SetMotor(left,0.1);
    //     SetMotor(left,0.1);
    // }
    int counter=0;
     double sensors[8];
    if (LineFollow==true)
    {
        for (int n=0;n<8;n++){
            sensors[n]=ADCRead(adc[n]);
        }
    }

    if(sensors[0]<threshold && sensors[1]<threshold && sensors[2]<threshold && sensors[3]<threshold  && sensors[4]<threshold && sensors[5]<threshold  && sensors[6]<threshold  && sensors[7]<threshold){
            counter++;
            if (( counter ==3) && (passLineFollowing == true)){
                LineFollow=false;
            }
            double IRBoi[2];
            for (int x=0;x<2;x++)
            {
                IRBoi[x]=ADCRead(adc2[x]);
            }
            double IRsum=-1.5*IRBoi[0]+1.5*IRBoi[1];

            //BOTH
            if  ((IRBoi[0]>.7)&& (IRBoi[1]>.7))
            {
                SetMotor(left,(.9+IRsum/10));
                SetMotor(right,(-.9-IRsum/10));
                blink=1; //red
                Wait(.7);
            }

            // LEFT DETECTS, FRONT DOESNT
            // else if ((IRBoi[0]>.7))
            // {
            //
            //     blink=2;//blue
            // }

            //ONLY FRONT
            else if ((IRBoi[1]>.7))
            {
                SetMotor(left,(.5+IRsum/10));
                SetMotor(right,(-.5-IRsum/10));
                blink=3;//green
            }
            //NEITHER
            else
            {
                float err = (IRBoi[0] - 0.7) * 2;
                SetMotor(left,.5 + err);
                SetMotor(right,.6 - err);
                // SetMotor(left,0.4);
                // SetMotor(right,0.4);
                blink=4;//white
            }

    }else{
            double sum= 1.4*sensors[0]+1.0*sensors[1]+0.750*sensors[2]+0.0*sensors[3]+0.0*sensors[4]-.75*sensors[5]-1.0*sensors[6]-1.4*sensors[7];
            counter=0;
            if(sensors[0]>0.3 && sensors[1]>0.3 && sensors[2]>0.3 && sensors[3]>0.3  && sensors[4]>0.3 && sensors[5]>0.3  && sensors[6]>0.3  && sensors[7]>0.3){

                passLineFollowing=true;
                Wait(.5);
                SetMotor(left,.3);
                SetMotor(right,.5);
                Wait(3);

            }
             if (sum>-.03 && sum<.03){
                SetMotor(left,.15);
                SetMotor(right,.15);
            } else if (sum>.03){
                SetMotor(left,.1+sum/3);
                SetMotor(right,.1-sum/3);
            }else if (sum<-.03){
                SetMotor(left,.1-sum*(-1)/3);
                SetMotor(right,.1+sum*(-1)/3);
            }

        }
















        //this works i promise
        // double sensors[8];
        // for (int n=0;n<8;n++){
        //     sensors[n]=ADCRead(adc[n]);
        // }
        // double sum= 1.4*sensors[0]+1.0*sensors[1]+0.750*sensors[2]+0.0*sensors[3]+0.0*sensors[4]-.75*sensors[5]-1.0*sensors[6]-1.4*sensors[7];
        //
        // if(sensors[0]>0.3 && sensors[1]>0.3 && sensors[2]>0.3 && sensors[3]>0.3  && sensors[4]>0.3 && sensors[5]>0.3  && sensors[6]>0.3  && sensors[7]>0.3){
        //      SetMotor(left, 0);
        //      SetMotor(right, 0);
        //
        // }else if (sum>-.1 && sum<.1){
        //     SetMotor(left,.25);
        //     SetMotor(right,.25);
        // } else if (sum>.1){
        //     SetMotor(left,(.5+sum)/2);
        //     SetMotor(right,(.5-sum)/2);
        // }else if (sum<-.1){
        //     SetMotor(left,(.5-sum*(-1))/2);
        //     SetMotor(right,(.5+sum*(-1))/2);
        // }









        // if(ADCRead(adc[0])>0.3 && ADCRead(adc[1])>0.3 && ADCRead(adc[2])>0.3 && ADCRead(adc[3])>0.3  && ADCRead(adc[4])>0.3 && ADCRead(adc[5])>0.3  && ADCRead(adc[6])>0.3  && ADCRead(adc[7])>0.3){
        //      SetMotor(left, 0);
        //      SetMotor(right, 0);
        // }else if(((ADCRead(adc[0])>0.3 && ADCRead(adc[1])>0.3 && ADCRead(adc[2])>0.3)  && (ADCRead(adc[3])<0.3 &&ADCRead(adc[4])<0.3 && ADCRead(adc[5])<0.3  && ADCRead(adc[6])<0.3  && ADCRead(adc[7])<0.3))||((ADCRead(adc[0])>0.3 && ADCRead(adc[1])>0.3 && ADCRead(adc[2])>0.3  && ADCRead(adc[3])>0.3) && (ADCRead(adc[5])<0.3 && ADCRead(adc[5])<0.3  && ADCRead(adc[6])<0.3  && ADCRead(adc[7])<0.3))){
        //     for(int c=0;c<=15;c++)
        //     {SetMotor(left, 0.25);
        //     SetMotor(right, -0.25);}
        // }else if(((ADCRead(adc[7])>0.3 && ADCRead(adc[6])>0.3 && ADCRead(adc[5])>0.3 && ADCRead(adc[4])>0.3) &&(ADCRead(adc[3])<0.3 && ADCRead(adc[2])<0.3  && ADCRead(adc[1])<0.3  && ADCRead(adc[0])<0.3))||((ADCRead(adc[7])>0.3 && ADCRead(adc[6])>0.3 && ADCRead(adc[5])>0.3  && ADCRead(adc[4])>0.3 && ADCRead(adc[3])>0.3) && (ADCRead(adc[2])<0.3  && ADCRead(adc[1])<0.3  && ADCRead(adc[0])<0.3))){
        //         for(int c=0;c<=15;c++)
        //         {SetMotor(left, -0.25);
        //         SetMotor(right, 0.25);}
        // }else if((ADCRead(adc[0])>0.3 || ADCRead(adc[1])>0.3 || ADCRead(adc[2])>0.3) && (ADCRead(adc[3])<0.3  && ADCRead(adc[4])<0.3 && ADCRead(adc[5])<0.3  && ADCRead(adc[6])<0.3  && ADCRead(adc[7])<0.3)){
        //     SetMotor(left, 0.3);
        //     SetMotor(right, 0.1);
        // }else if((ADCRead(adc[3])>0.3  || ADCRead(adc[4])>0.3) && (ADCRead(adc[0])<0.3 && ADCRead(adc[1])<0.3 && ADCRead(adc[2])<0.3&& ADCRead(adc[5])<0.3  && ADCRead(adc[6])<0.3  && ADCRead(adc[7])<0.3)){
        //     SetMotor(left, 0.45);
        //     SetMotor(right, 0.45);
        // }else if((ADCRead(adc[5])>0.3 || ADCRead(adc[6])>0.3  || ADCRead(adc[7])>0.3) && (ADCRead(adc[0])<0.3 && ADCRead(adc[1])<0.3 && ADCRead(adc[2])<0.3 && ADCRead(adc[3])<0.3  && ADCRead(adc[4])<0.3 )){
        //     SetMotor(left, 0.1);
        //     SetMotor(right, 0.3);
        // }
    }
}
