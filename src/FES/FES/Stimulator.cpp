#include <FES/Stimulator.hpp>
#include <MEL/Core/Console.hpp>
#include <MEL/Logging/Log.hpp>
#include <Windows.h>
#include <tchar.h>

using namespace mel;

namespace fes{
    Stimulator::Stimulator(std::string& name, std::string& com_port):
        name_(name),
        com_port_(com_port),
        open_(false)
    {
        print("Stimulator Created");
    }

    int Stimulator::on_enable() {
        fd0 = open_port0();                  // Open serial port ttyUSB0 and store parameters in fd0
        fd1 = open_port1();                  // Open serial port ttyUSB1 and store parameters in fd1
        configure_port(fd0);                 // Configure the parameters for serial port ttyUSB0
        Board_Setup_0(fd0);                  // Write stim board setup commands to serial port ttyUSB0
    }

    bool Stimulator::open_port(){

        std::string com_port_formatted = "\\\\.\\" + com_port_;

        std::wstring stemp = std::wstring(com_port_formatted.begin(), com_port_formatted.end());
        LPCWSTR com_port_lpcwstr = stemp.c_str();

        hComm = CreateFile(com_port_lpcwstr,             //port name
                           GENERIC_READ | GENERIC_WRITE, //Read/Write
                           0,                            // No Sharing
                           NULL,                         // No Security
                           OPEN_EXISTING,// Open existing port only
                           0,            // Non Overlapped I/O
                           NULL);        // Null for Comm Devices

        if (hComm == INVALID_HANDLE_VALUE){
            LOG(Error) << "Failed to open Stimulator " << get_name();
            return false;
        }
        else{
            LOG(Info) << "Opened Stimulator " << get_name();
        }

        return true;
    }

    int Stimulator::open_port0(void){ // open_port0 is a function that will open the first serial port to communicate with the first stim board
        int fd; // file description for the serial port

        fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);

        if (fd == -1) // if open is unsucessful
        {
            printf("open_port: Unable to open /dev/ttyUSB0. \n");
        }
        else
        {
            fcntl(fd, F_SETFL, 0);
            printf("Port is Open. \n");
        }

        return(fd);
    } //open_port0


    int Stimulator::open_port1(void){ // open_port1 is a function that will open the second serial port to comunicate with the second stim board
        int fd; // file description for the serial port

        fd = open("/dev/ttyUSB1", O_RDWR | O_NOCTTY | O_NDELAY);
        if (fd == -1) // if open is unsucessful
        {
            printf("open_port: Unable to open /dev/ttyUSB1. \n");
        }
        else
        {
            fcntl(fd, F_SETFL, 0);
            printf("Port is Open. \n");
        }

        return(fd);
    } //open_port


    int Stimulator::configure_port(int fd){  // configure_port establishes the settings for each serial port
        struct termios port_settings;      // structure to store the port settings in

        cfsetispeed(&port_settings, B9600);    // set baud rate for input  (Change 9600 to the desired value)
        cfsetospeed(&port_settings, B9600);    // set baud rate for output (Change 9600 to the desired value)

        port_settings.c_cflag &= ~PARENB;    // set no parity
        port_settings.c_cflag &= ~CSTOPB;    // set stop bits
        port_settings.c_cflag &= ~CSIZE;     // set data bits
        port_settings.c_cflag |= CS8;

        port_settings.c_cflag |= (CLOCAL | CREAD); 		// Enable the receiver and set local mode
        port_settings.c_cflag &= ~CRTSCTS;         		// Disable hardware flow control
        port_settings.c_iflag &= ~(IXON | IXOFF | IXANY);	// Disable software flow control
        port_settings.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Raw input
        port_settings.c_oflag &= ~OPOST;			  // Raw Output
        tcsetattr(fd, TCSANOW, &port_settings);    		// apply the settings to the port
        return(fd);

    } //configure_port


    int Stimulator::checksum(unsigned char myarray[], int m){  // checksum is a function that preforms checksums of all of the byte strings used in this code
        int csum = 0;
        for (int i = 0; i < m - 1; i++) {
            csum += myarray[i];
        }
        csum = ((0x00FF & csum) + (csum >> 8)) ^ 0xFF;
        return csum;
    } // checksum


    //The Board_Setup function sends multiple strings of bytes in order to set up the stim board.
    int Stimulator::Board_Setup_0(int fd){

        fd_set rdfs;
        struct timeval timeout;

        //Create byte array
        unsigned char chan_set1[] = { 0x04, 0x80, 0x47, 0x07, 0x00, 0x64, 0xFA, 0x00, 0x64, 0x11, 0x01, 0x00 };  // channel 1 setup
        unsigned char chan_set2[] = { 0x04, 0x80, 0x47, 0x07, 0x01, 0x64, 0xFA, 0x00, 0x64, 0x11, 0x23, 0x00 };  // Channel 2 Setup
        unsigned char chan_set3[] = { 0x04, 0x80, 0x47, 0x07, 0x02, 0x64, 0xFA, 0x00, 0x64, 0x11, 0x45, 0x00 };  // Channel 3 Setup
        unsigned char chan_set4[] = { 0x04, 0x80, 0x47, 0x07, 0x03, 0x64, 0xFA, 0x00, 0x64, 0x11, 0x67, 0x00 };  // Channel 4 Setup
        unsigned char crt_sched[] = { 0x04, 0x80, 0x10, 0x03, 0xAA, 0x00, 0x19, 0x00 };  // Create Schedule
        unsigned char crt_evnt1[] = { 0x04, 0x80, 0x15, 0x09, 0x01, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x20, 0x00, 0x00 }; // Create Event 1
        
        unsigned char sync_msg1[] = { 0x04, 0x80, 0x1B, 0x01, 0xAA, 0x00 }; // Sync Message 1

        chan_set1[(sizeof(chan_set1) / sizeof(*chan_set1)) - 1] = checksum(chan_set1, (sizeof(chan_set1) / sizeof(*chan_set1)));
        write(fd, chan_set1, (sizeof(chan_set1) / sizeof(*chan_set1))); // Channel 1 Setup
                                                                        
        usleep(1000000);

        chan_set2[(sizeof(chan_set2) / sizeof(*chan_set2)) - 1] = checksum(chan_set2, (sizeof(chan_set2) / sizeof(*chan_set2)));
        write(fd, chan_set2, (sizeof(chan_set2) / sizeof(*chan_set2))); // Channel 2 Setup
                                                                        
        usleep(1000000);

        chan_set3[(sizeof(chan_set3) / sizeof(*chan_set3)) - 1] = checksum(chan_set3, (sizeof(chan_set3) / sizeof(*chan_set3)));
        write(fd, chan_set3, (sizeof(chan_set3) / sizeof(*chan_set3))); // Channel 3 Setup
                                                                        
        usleep(1000000);

        chan_set4[(sizeof(chan_set4) / sizeof(*chan_set4)) - 1] = checksum(chan_set4, (sizeof(chan_set4) / sizeof(*chan_set4)));
        write(fd, chan_set4, (sizeof(chan_set4) / sizeof(*chan_set4))); // Channel 4 Setup
                                                                        
        usleep(1000000);

        crt_sched[(sizeof(crt_sched) / sizeof(*crt_sched)) - 1] = checksum(crt_sched, (sizeof(crt_sched) / sizeof(*crt_sched)));
        write(fd, crt_sched, (sizeof(crt_sched) / sizeof(*crt_sched))); // Create Schedule
                                                                        
        usleep(1000000);

        crt_evnt1[(sizeof(crt_evnt1) / sizeof(*crt_evnt1)) - 1] = checksum(crt_evnt1, (sizeof(crt_evnt1) / sizeof(*crt_evnt1)));
        write(fd, crt_evnt1, (sizeof(crt_evnt1) / sizeof(*crt_evnt1))); // Create Event 1
                                                                        
        usleep(1000000);

        sync_msg1[(sizeof(sync_msg1) / sizeof(*sync_msg1)) - 1] = checksum(sync_msg1, (sizeof(sync_msg1) / sizeof(*sync_msg1)));
        write(fd, sync_msg1, (sizeof(sync_msg1) / sizeof(*sync_msg1))); // Sync Message 1
                                                                        
        usleep(1000000);

        printf("Setup Complete. \n");

    } // Board_Setup_0

}