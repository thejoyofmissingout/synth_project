//4051 mux example using this diagram https://github.com/BleepLabs/Dadageek-August20/wiki/Multiplexing-info

unsigned long cm;
unsigned long prev[8];
byte mux_select;
int pot_readings1[8];
int pot_readings2[8];
int led_outputs1[8];
int digital_readings1[8];
int digital_readings2[8];

int ctrl_pins[4] {1, 2, 3};
#define ctrl_pin1 1
#define ctrl_pin2 2
#define ctrl_pin3 3
#define digital_read_pin1 8
#define digital_read_pin2 6
#define led_outputs1_pin 20
int led_state[8];

#define analog_pin1 A1
#define analog_pin2 A2



void setup() {
  pinMode(ctrl_pin1, OUTPUT);
  pinMode(ctrl_pin2, OUTPUT);
  pinMode(ctrl_pin3, OUTPUT);
  pinMode(digital_read_pin1, INPUT_PULLUP);
  pinMode(digital_read_pin2, INPUT_PULLUP);
  pinMode(led_outputs1_pin, OUTPUT);
    digitalWrite(led_outputs1_pin, HIGH);


  analogReadResolution(12);
  analogWriteResolution(12);
  analogReadAveraging(64);

}

void loop() {
  cm = millis();
 
//   looping through all 8 LEDs
//    for (int jj=0;jj<8;jj++){
//      // turn on LEDs based on bit conversion
//      for(int ii=0;ii<3;ii++){
//        digitalWrite(led_outputs1[ii],bitRead(jj,ii));
//      }
//      delay(100); // delay .1 sec
//    }

  // Each time this code happens a new input is selected and read
  //you could put this in the loop directly for even more speed or do it as a "for" instead.
  //I like this way as doing analog reads too fast can cause noise

  for (int mux_select = 0; mux_select < 8; mux_select++) {

    //mux_sel is a regular number but we need the bits inside it to select which pins
    // are outputting high or low to sell the 4051s which pin to select;
    digitalWrite(ctrl_pin1, bitRead(mux_select, 0));
    digitalWrite(ctrl_pin2, bitRead(mux_select, 1));
    digitalWrite(ctrl_pin3, bitRead(mux_select, 2));
    
    

    //mux_select is also used to put the reading in the arrays
    //if you're not using one of these just comment it out

    //smooth(channel, number of readings to average, input)
    //smooth needs a different channel number for each separate reading

    pot_readings1[mux_select] = smooth(mux_select, 31, analogRead(analog_pin1));
    pot_readings2[mux_select] = smooth(mux_select + 8, 31, analogRead(analog_pin2));
    digital_readings1[mux_select] = digitalRead(digital_read_pin1);
    digital_readings2[mux_select] = digitalRead(digital_read_pin2);

    // looping through all 8 LEDs
  for (int jj=0;jj<8;jj++){
    // turn on LEDs based on bit conversion
    for(int ii=0;ii<3;ii++){
      digitalWrite((ctrl_pins)[ii],bitRead(jj,ii)); 
    }
    delay(100); // delay .1 sec
  }

//     led_outputs1[mux_select]; { analogWrite(led_outputs1_pin, 4095);
//
//     }
    
    
//    digitalWrite(led_outputs1_pin, HIGH);

//    if (cm - prev[1] > 30) {
//      prev[1] = cm;
//
//      if (led_state[mux_select] == LOW) {
//        led_state[mux_select] = HIGH;
//      }
//      else {
//        led_state[mux_select] = LOW;
//      }
//      if (digital_readings2[mux_select] == 0) {
//        digitalWrite(led_outputs1, bitRead(mux_select, HIGH));
//      }
//      if (digital_readings2[mux_select] == 1) {
//        digitalWrite(led_outputs1[mux_select], LOW);
//      }

//    }

  }





  if (cm - prev[0] > 100) {
    prev[0] = cm;
    //print 8 readings. Change where mux_print starts and stops to see specific ones
    for (int mux_print = 0; mux_print < 8; mux_print++) {
            Serial.print(mux_print);
            Serial.print("-");
            Serial.print(pot_readings2[mux_print]);
            Serial.print("  ");
            Serial.print(mux_print);
            Serial.print("-");
            Serial.print(digital_readings2[mux_print]);
            Serial.print("  ");
            Serial.print(mux_print);
            Serial.print("-");
            Serial.print(led_outputs1[mux_print]);
            Serial.print("  ");


    }
    Serial.println();
  }

} //end of loop

//Smooth
// based on https://playground.arduino.cc/Main/DigitalSmooth/
// This function continuously samples an input and puts it in an array that is "samples" in length.
// This array has a new "raw_in" value added to it each time "smooth" is called and an old value is removed
// It throws out the top and bottom 15% of readings and averages the rest

#define maxarrays 16 //max number of different variables to smooth
#define maxsamples 51 //max number of points to sample and 
//reduce these numbers to save RAM

unsigned int smoothArray[maxarrays][maxsamples];

// sel should be a unique number for each occurrence
// samples should be an odd number greater that 7. It's the length of the array. The larger the more smooth but less responsive
// raw_in is the input. positive numbers in and out only.

unsigned int smooth(byte sel, unsigned int samples, unsigned int raw_in) {
  int j, k, temp, top, bottom;
  long total;
  static int i[maxarrays];
  static int sorted[maxarrays][maxsamples];
  boolean done;

  i[sel] = (i[sel] + 1) % samples;    // increment counter and roll over if necessary. -  % (modulo operator) rolls over variable
  smoothArray[sel][i[sel]] = raw_in;                 // input new data into the oldest slot

  for (j = 0; j < samples; j++) { // transfer data array into anther array for sorting and averaging
    sorted[sel][j] = smoothArray[sel][j];
  }

  done = 0;                // flag to know when we're done sorting
  while (done != 1) {      // simple swap sort, sorts numbers from lowest to highest
    done = 1;
    for (j = 0; j < (samples - 1); j++) {
      if (sorted[sel][j] > sorted[sel][j + 1]) {    // numbers are out of order - swap
        temp = sorted[sel][j + 1];
        sorted[sel] [j + 1] =  sorted[sel][j] ;
        sorted[sel] [j] = temp;
        done = 0;
      }
    }
  }

  // throw out top and bottom 15% of samples - limit to throw out at least one from top and bottom
  bottom = max(((samples * 15)  / 100), 1);
  top = min((((samples * 85) / 100) + 1  ), (samples - 1));   // the + 1 is to make up for asymmetry caused by integer rounding
  k = 0;
  total = 0;
  for ( j = bottom; j < top; j++) {
    total += sorted[sel][j];  // total remaining indices
    k++;
  }
  return total / k;    // divide by number of samples
}
