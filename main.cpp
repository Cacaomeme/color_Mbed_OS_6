#include "mbed.h"

I2C i2c(PB_7, PB_6); //pins for I2C communication (SDA, SCL)
static BufferedSerial pc(USBTX, USBRX);

int sensor_addr = 41 << 1;

DigitalOut green(LED1);
DigitalOut red(PA_3);
DigitalOut blue(PA_4);

int max(int num1, int num2) {
    int max_value; /* 最大値 */

    /* ２つの値を比較して大きい方を最大値とする */
    if (num1 > num2) {
        max_value = num1;
    } else {
        max_value = num2;
    }

    return max_value;
}

int min(int num1, int num2) {
    int min_value; /* 最大値 */

    /* ２つの値を比較して大きい方を最大値とする */
    if (num1 < num2) {
        min_value = num1;
    } else {
        min_value = num2;
    }

    return min_value;
}

int main() {
    pc.set_baud(9600);
    green = 1; // off
    
    // Connect to the Color sensor and verify whether we connected to the correct sensor. 
    
    i2c.frequency(100000);
    
    char id_regval[1] = {146};
    char data[1] = {0};
    i2c.write(sensor_addr,id_regval,1, true);
    i2c.read(sensor_addr,data,1,false);
    
    if (data[0]==68) {
        green = 0;
        ThisThread::sleep_for(2s); 
        green = 1;
        } else {
        green = 1; 
    }
    
    // Initialize color sensor
    
    char timing_register[2] = {129,0};
    i2c.write(sensor_addr,timing_register,2,false);
    
    char control_register[2] = {143,0};
    i2c.write(sensor_addr,control_register,2,false);
    
    char enable_register[2] = {128,3};
    i2c.write(sensor_addr,enable_register,2,false);
    
    // Read data from color sensor (Clear/Red/Green/Blue)

    double Hue;
    double Saturation;
    double Value;
    
    while (true) { 
        
        char red_reg[1] = {150};
        char red_data[2] = {0,0};
        i2c.write(sensor_addr,red_reg,1, true);
        i2c.read(sensor_addr,red_data,2, false);
        
        double red_value = ((int)red_data[1] << 8) | red_data[0];
        
        char green_reg[1] = {152};
        char green_data[2] = {0,0};
        i2c.write(sensor_addr,green_reg,1, true);
        i2c.read(sensor_addr,green_data,2, false);
        
        double green_value = ((int)green_data[1] << 8) | green_data[0];
        
        char blue_reg[1] = {154};
        char blue_data[2] = {0,0};
        i2c.write(sensor_addr,blue_reg,1, true);
        i2c.read(sensor_addr,blue_data,2, false);
        
        double blue_value = ((int)blue_data[1] << 8) | blue_data[0];
        
        // print sensor readings

        double MAX = max((max(red_value,green_value)),blue_value);
	    double MIN = min((min(red_value,green_value)),blue_value);
	    Value = MAX/256*100;

	    if(MAX==MIN){
		Hue = 0;
		Saturation = 0;
	    } else {
		if(MAX == red_value){
            Hue = 60.0*(green_value-blue_value)/(MAX-MIN) + 0;
        } else if (MAX == green_value){
            Hue = 60.0*(blue_value-red_value)/(MAX-MIN) + 120.0;
        } else if (MAX == blue_value){
            Hue = 60.0*(red_value-green_value)/(MAX-MIN) + 240.0;
        }

		if(Hue > 360.0){
            Hue = Hue - 360.0;
        } else if (Hue < 0){
            Hue = Hue + 360.0;
        }
		Saturation = (MAX-MIN)/(MAX)*100;
	}
        
        printf("H (%lf), S (%lf), V (%lf)", Hue, Saturation, Value);
        if (Hue > 235 && Hue < 350){
           printf(", color=purple\n");
           red = 1;
           blue = 1;
        } else if ((Hue > 350 && Hue < 360) || (Hue > 0 && Hue < 30)){
           printf(", color=red\n");
           blue = 0;
           red = 1;
        } else if (Hue > 210 && Hue < 235){
           printf(", color=blue\n");
           red = 0;
           blue = 1;
        } else {
           printf(", color=other\n");
           red = 0;
           blue = 0;
        }
        ThisThread::sleep_for(10ms);
    }  
}