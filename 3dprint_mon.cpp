//-----------------------------------------------------------------------
// File : 3dprint_mon.cpp
//
// Programmer: Joseph Dinsmoor
//
// Program #: Final Project
//
// Due Date: 05/03/2022
//
// Course: EGRE 347, Spring 2022
//
// Pledge: I have neither given nor received unauthorized aid on this program.
//
// Description: This program is designed to be operated in conjunction with a 3d printer.
//It uses a USB webcam connected with the OpenCV library, and a temperature sensor mounted directly to the print bed, which uses the WiringPi and WiringPiI2C libraries.
//These libraries enable the pi to interface with external sensors using the GPIO pins.
//The program has four options. Option 1 will return the current bed temperature in degrees fahrenheit, at the time the option is selected.
//Option 2 will capture a realtime image of the print bed, outputting to a filename of the user's choice. If no filename is entered, the default will be image.png.
//Option 3 is automatic print monitoring. Once a print is started, you will enter the estimated time given by the printer, in seconds. The program will then take that time, capture a picture of the initial layer of the print, and also take a picture of the print partway through.
//Option 4 is used to exit the program.
//
//-----------------------------------------------------------------------

#include "3dprint_mon.hpp"
#include <iostream>
#include <opencv2/opencv.hpp>
//For USB webcam interfacing
#include <iomanip>
extern "C" {
#include <wiringPi.h>
#include <wiringPiI2C.h>
//for temperature sensor interfacing
}
#include <csignal>
#include <unistd.h>
#include <thread>
#include <time.h>
//needed for timing pictures during a print

using namespace std;
using namespace cv;

const int DEVICE_ID = 0x48;

const int REG_TEMP_DATA = 0x00;
const int REG_CONFIG = 0x01;

const int SET_RES_12 = 0x60;
//Setting device ID and resolution constants for the temperature sensor

bool RUNNING = true;
//For enabling an exit funciton
void my_handler(int s) {
    cout << "Detected CTRL-C signal no. " << s << '\n';
    RUNNING = FALSE;
}
//Purely for stopping script if print runs into errors
double convert_temp(unsigned short int val) {
    val = (( val << 8) & 0xff00) | ((val >> 8) & 0x00ff);
    double temperature = (val * 0.0625) * (9.0/5.0) + 32.0;
    temperature = temperature / 10;
    temperature = temperature + 40;
    return(temperature);
}
//Converts temperature from hex into Fahrenheit. I used the code that I previously used for the 4th in class assignment in order to write this function.

void capPic(string cap){
    VideoCapture capture(0);
    capture.set(CAP_PROP_FRAME_WIDTH,1280);
    capture.set(CAP_PROP_FRAME_HEIGHT,720);
    if(!capture.isOpened()){
        cout << "Camera Connection Failed!\n";
    }
    Mat frame;
    capture >> frame;
    imwrite(cap, frame);
} // Captures still image of print bed, returning to specific png files for each point in the print. Can be used in conjunction with Option 2, capturing a current image of the print status

int menu(){
    int choice;
    cout << "\nEnter Option:\n";
    cout << "\t(1) Print current bed temperature\n";
    cout << "\t(2) Capture image of current print state\n";
    cout << "\t(3) Automatic Print monitoring, with starting and midpoint photos\n";
    cout << "\t(4) Exit Monitoring\n";
    cout << "\t\tChoice?";
    cin >> choice;
    fflush(stdout);
    cout << "\n";
    return(choice);
}
//Menu for use with switch function. 4 options. Option 1 will return the current bed temperature in degrees fahrenheit, at the time the option is selected. Option 2 will capture a realtime image of the print bed, outputting to a filename of the user's choice. If no filename is entered, the default will be image.png. Option 3 is automatic print monitoring. Once a print is started, you will enter the estimated time given by the printer, in seconds. The program will then take that time, capture a picture of the initial layer of the print, and also take a picture of the print partway through. Option 4 is used to exit the program.

int main(){
    signal(SIGINT, my_handler);
    unsigned short int temp_data;
    double temperature;
    //Used for temperature data reading and conversion
    string filename;
    string extension = ".png";
    //used for inputting custom filenames for realtime photo capture
    wiringPiSetupGpio();
    int eta;
    int input;
    while(RUNNING){
        //Main Switch function
        input = menu();
        //Opens menu and presents user with list of options
        int fd = wiringPiI2CSetup(DEVICE_ID);
        if(fd == -1){
            cout << "I2C initialization failed.\n";
            return -1;
        }
        //Initializes I2C temperature sensor
        wiringPiI2CWriteReg8(fd, REG_CONFIG, SET_RES_12);
        unsigned short int temp_value = wiringPiI2CReadReg16(fd, REG_TEMP_DATA);
        temperature = convert_temp(temp_value);
        //Prepares realtime bed temperature in the event that option 1 is called
        switch (input){
            case 1:
                cout << fixed << setprecision(3) << temperature << " Degrees Fahrenheit\n";
                break;
                //Returns realtime bed temperature in degrees fahrenheit. Relies on convert_temp function above
            case 2:
                cout <<"Enter Desired output file name with .png extension (default image.png): ";
                cin >> filename;
                if(!filename.find(extension)){
                    capPic("image.png");
                    cout << "\n File saved in image.png\n";
                }
                else{
                    capPic(filename);
                    cout << "\n File saved in " << filename << "\n";
                }
                break;
                //Captures image of print bed in realtime, outputs to either custom filename, or image.png by default. User is prompted for custom filename.
            case 3:
                cout << "Please input estimated print time in seconds\n For example, 8 hours and 42 minutes would be 31320 seconds:";
                cin >> eta;
                cout << "\n";
                cout << "\n-- Capturing Initial Print Setup -- \n";
                capPic("initial_cap.png");
                cout << "\n-- Capture stored in ""initial_cap.png"" -- \n";
                sleep(eta / 2);
                cout << " -- Capturing Mid Print State, stored in ""mid_cap.png"" -- \n";
                capPic("mid_cap.png");
                cout << "\n-- Capture stored in ""mid_cap.png"" -- \n";
                break;
                //Print monitoring function. Input is taken from user on estimated print time. Once input is given, program captures the initial layer of the print, and stores it in "initial-cap.png". It then waits exactly half the amount of time and takes another photo so that the user can ensure that their print is going well. It stores this photo in "mid_cap.png".
            case 4:
                RUNNING = FALSE;
                break;
                //Exits the monitoring program
            default:
                cout << "Option " << input << "not found, please enter a selection 1-3 or 4 to exit\n";
                //If option is not 1-4, this will be output, and will return to the menu.
        }
    }
    
    return 0;
}
