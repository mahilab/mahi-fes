#include <FES/Stimulator.hpp>
#include <MEL/Core/Console.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Core/Timer.hpp>
#include <MEL/Utility/System.hpp>
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
        enable();
    }

    Stimulator::~Stimulator(){
        if(is_enabled()){
            CloseHandle(hComm);
            print("Stimulator Closed");
        }
    }

    // Open serial port ttyUSB0 and store parameters in fd0
    bool Stimulator::enable() {
        open_ = open_port(hComm); // open the comport with read/write permissions
        configure_port(hComm);    // Configure the parameters for serial port ttyUSB0
        initialize_board(hComm);  // Write stim board setup commands to serial port ttyUSB0
    }

    bool Stimulator::open_port(HANDLE& hComm_){

        std::string com_port_formatted = "\\\\.\\" + com_port_;

        // the comport must be formatted as an LPCWSTR, so we need to get it into that form from a std::string
        std::wstring stemp = std::wstring(com_port_formatted.begin(), com_port_formatted.end());
        LPCWSTR com_port_lpcwstr = stemp.c_str();

        hComm_ = CreateFile(com_port_lpcwstr,             //port name
                           GENERIC_READ | GENERIC_WRITE, //Read/Write
                           0,                            // No Sharing
                           NULL,                         // No Security
                           OPEN_EXISTING,// Open existing port only
                           0,            // Non Overlapped I/O
                           NULL);        // Null for Comm Devices

        if (hComm_ == INVALID_HANDLE_VALUE){
            LOG(Error) << "Failed to open Stimulator" << get_name();
            return false;
        }
        else{
            LOG(Info) << "Opened Stimulator" << get_name();
        }

        return true;
    }

    bool Stimulator::configure_port(HANDLE& hComm_){  // configure_port establishes the settings for each serial port

        // http://bd.eduweb.hhs.nl/micprg/pdf/serial-win.pdf

        dcbSerialParams.DCBlength = sizeof(DCB);

        if (!GetCommState(hComm_, &dcbSerialParams)) {
            LOG(Error) << "Error getting serial port state";
            return false;
        }

        // set parameters to use for serial communication

        // set the baud rate that we will communicate at to 9600
        dcbSerialParams.BaudRate = CBR_9600;

        // 8 bits in the bytes transmitted and received.
        dcbSerialParams.ByteSize = 8;
        
        // Specify that we are using one stop bit
        dcbSerialParams.StopBits = ONESTOPBIT;

        // Specify that we are using no parity 
        dcbSerialParams.Parity = NOPARITY;

        // Disable all parameters dealing with flow control
        dcbSerialParams.fOutX = FALSE;
        dcbSerialParams.fInX  = FALSE;
        dcbSerialParams.fRtsControl = RTS_CONTROL_DISABLE;
        dcbSerialParams.fDtrControl = DTR_CONTROL_DISABLE;
        // dcbSerialParams.fOutxCtsFlow = FALSE;
        // dcbSerialParams.fOutxDsrFlow = FALSE;

        if(!SetCommState(hComm_, &dcbSerialParams)){
            LOG(Error) << "Error setting serial port state";
            return false;
        }

        COMMTIMEOUTS timeouts={0};
        timeouts.ReadIntervalTimeout=50;
        timeouts.ReadTotalTimeoutConstant=50;
        timeouts.ReadTotalTimeoutMultiplier=10;
        timeouts.WriteTotalTimeoutConstant=50;
        timeouts.WriteTotalTimeoutMultiplier=10;
        if(!SetCommTimeouts(hComm_, &timeouts)){
            LOG(Error) << "Error setting serial port timeouts";
            return false;
        }

        // SET SOFTWARE/HARDWARE FLOW CONTROL???????????

        return true;
    }

    int Stimulator::checksum(unsigned char myarray[], int m){  // checksum is a function that preforms checksums of all of the byte strings used in this code
        int csum = 0;
        for (int i = 0; i < m - 1; i++) {
            csum += myarray[i];
        }
        csum = ((0x00FF & csum) + (csum >> 8)) ^ 0xFF;
        return csum;
    } // checksum

    bool Stimulator::initialize_board(HANDLE& hComm_){

        // delay time after sending setup messages of serial comm
        Time setup_time = milliseconds(100);

        //Create byte array
        //Structure of byte arrays:
        //  Destination address - always 0x04
        //  Source address - always 0x80
        //  Message type - see message commands in header file
        //  Message Length - length of message without header or checksum
        //  Message - Bytes of the message - must be of length Message Length
        //  Checksum Calculation - add each of the bytes, mask the lower byte of sum and add cary byte, then invert sum

        //Structure of channel setup messages:
        //  1 byte:  Port channel - lower 4 bits are channel, upper for bits are port (always 0): aka always 0x0a where a is channel number
        //  1 byte:  Amplitude limit - max amplitude the channel will outport
        //  1 byte:  Pulse Width Limit - max pulse with that channel will output
        //  2 bytes: Interphase Delay in usec - space between phases of the waveform (can be from 10-65535)
        //  1 byte:  Aspect Ratio - designates the proportion of the amplitude of the first phase to the second phase.
        //             - The lower 4 bits represent the first phase and the upper 4 bits represent the second phase. 0x11 is 1 to 1 ratio
        //  1 byte:  AnodeCathode - for 4 bipolar channels (what we have) these are 0x01, 0x23, 0x45, 0x67 respectively
        
        //                          /Destination address  /Source address /Message type
        unsigned char chan_set1[] = { DESTINATION_ADDRESS, SOURCE_ADDRESS, CHANNEL_SETUP_MSG, CHANNEL_SET_LEN, 0x00, 0x64, 0xFA, 0x00, 0x64, 0x11, 0x01, 0x00 };  // channel 1 setup
        unsigned char chan_set2[] = { DESTINATION_ADDRESS, SOURCE_ADDRESS, CHANNEL_SETUP_MSG, CHANNEL_SET_LEN, 0x01, 0x64, 0xFA, 0x00, 0x64, 0x11, 0x23, 0x00 };  // Channel 2 Setup
        unsigned char chan_set3[] = { DESTINATION_ADDRESS, SOURCE_ADDRESS, CHANNEL_SETUP_MSG, CHANNEL_SET_LEN, 0x02, 0x64, 0xFA, 0x00, 0x64, 0x11, 0x45, 0x00 };  // Channel 3 Setup
        unsigned char chan_set4[] = { DESTINATION_ADDRESS, SOURCE_ADDRESS, CHANNEL_SETUP_MSG, CHANNEL_SET_LEN, 0x03, 0x64, 0xFA, 0x00, 0x64, 0x11, 0x67, 0x00 };  // Channel 4 Setup

        write_setup_message(hComm_, chan_set1, "Channel 1", setup_time);
        write_setup_message(hComm_, chan_set2, "Channel 2", setup_time);
        write_setup_message(hComm_, chan_set3, "Channel 3", setup_time);
        write_setup_message(hComm_, chan_set4, "Channel 4", setup_time);

        unsigned char crt_sched[] = { DESTINATION_ADDRESS, SOURCE_ADDRESS, CREATE_SCHEDULE_MSG, 0x03, 0xAA, 0x00, 0x19, 0x00 };                          // Create Schedule

        write_setup_message(hComm_, crt_sched, "Schedule" , setup_time);

        unsigned char crt_evnt1[] = { DESTINATION_ADDRESS, SOURCE_ADDRESS, CREATE_EVENT_MSG, 0x09, 0x01, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x20, 0x00, 0x00 }; // Create Event 1

        write_setup_message(hComm_, crt_evnt1, "Event 1"  , setup_time);

        unsigned char sync_msg1[] = { DESTINATION_ADDRESS, SOURCE_ADDRESS, SYNC_MSG, 0x01, 0xAA, 0x00 }; // Sync Message 1

        write_setup_message(hComm_, crt_evnt1, "Sync Msg" , setup_time);

        print("Setup Complete. \n");

        return true;
    }

    bool Stimulator::is_enabled(){
        return open_;
    }

    bool Stimulator::write_setup_message(HANDLE& handle_, unsigned char setup_message_[], std::string message_string_,const mel::Time delay_time_){

        DWORD dwBytesWritten = 0;

        setup_message_[(sizeof(setup_message_) / sizeof(*setup_message_)) - 1] = checksum(setup_message_, (sizeof(setup_message_) / sizeof(*setup_message_)));
        if(!WriteFile(handle_, setup_message_, (sizeof(setup_message_) / sizeof(*setup_message_)),&dwBytesWritten,NULL)){
            LOG(Error) << "Error in " << message_string_ << "Setup.";
            return false;
        }
        else{
            LOG(Error) << message_string_ << "Setup was successful.";
            return false;
        }

        sleep(delay_time_);
        return true;
    }


    
}




    // int Stimulator::open_port0(void){ // open_port0 is a function that will open the first serial port to communicate with the first stim board
    //     int fd; // file description for the serial port

    //     fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);

    //     if (fd == -1) // if open is unsucessful
    //     {
    //         printf("open_port: Unable to open /dev/ttyUSB0. \n");
    //     }
    //     else
    //     {
    //         fcntl(fd, F_SETFL, 0);
    //         printf("Port is Open. \n");
    //     }

    //     return(fd);
    // } //open_port0


    // int Stimulator::open_port1(void){ // open_port1 is a function that will open the second serial port to comunicate with the second stim board
    //     int fd; // file description for the serial port

    //     fd = open("/dev/ttyUSB1", O_RDWR | O_NOCTTY | O_NDELAY);
    //     if (fd == -1) // if open is unsucessful
    //     {
    //         printf("open_port: Unable to open /dev/ttyUSB1. \n");
    //     }
    //     else
    //     {
    //         fcntl(fd, F_SETFL, 0);
    //         printf("Port is Open. \n");
    //     }

    //     return(fd);
    // } //open_port

        // int Stimulator::configure_port(int fd){  // configure_port establishes the settings for each serial port
    //     struct termios port_settings;      // structure to store the port settings in

    //     cfsetispeed(&port_settings, B9600);    // set baud rate for input  (Change 9600 to the desired value)
    //     cfsetospeed(&port_settings, B9600);    // set baud rate for output (Change 9600 to the desired value)

    //     port_settings.c_cflag &= ~PARENB;    // set no parity
    //     port_settings.c_cflag &= ~CSTOPB;    // set stop bits
    //     port_settings.c_cflag &= ~CSIZE;     // set data bits
    //     port_settings.c_cflag |= CS8;

    //     port_settings.c_cflag |= (CLOCAL | CREAD); 		// Enable the receiver and set local mode
    //     port_settings.c_cflag &= ~CRTSCTS;         		// Disable hardware flow control
    //     port_settings.c_iflag &= ~(IXON | IXOFF | IXANY);	// Disable software flow control
    //     port_settings.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Raw input
    //     port_settings.c_oflag &= ~OPOST;			  // Raw Output
    //     tcsetattr(fd, TCSANOW, &port_settings);    		// apply the settings to the port
    //     return(fd);

    // } //configure_port

    //The Board_Setup function sends multiple strings of bytes in order to set up the stim board.
    // int Stimulator::Board_Setup_0(int fd){

    //     fd_set rdfs;
    //     struct timeval timeout;

    //     //Create byte array
    //     unsigned char chan_set1[] = { 0x04, 0x80, 0x47, 0x07, 0x00, 0x64, 0xFA, 0x00, 0x64, 0x11, 0x01, 0x00 };  // channel 1 setup
    //     unsigned char chan_set2[] = { 0x04, 0x80, 0x47, 0x07, 0x01, 0x64, 0xFA, 0x00, 0x64, 0x11, 0x23, 0x00 };  // Channel 2 Setup
    //     unsigned char chan_set3[] = { 0x04, 0x80, 0x47, 0x07, 0x02, 0x64, 0xFA, 0x00, 0x64, 0x11, 0x45, 0x00 };  // Channel 3 Setup
    //     unsigned char chan_set4[] = { 0x04, 0x80, 0x47, 0x07, 0x03, 0x64, 0xFA, 0x00, 0x64, 0x11, 0x67, 0x00 };  // Channel 4 Setup
    //     unsigned char crt_sched[] = { 0x04, 0x80, 0x10, 0x03, 0xAA, 0x00, 0x19, 0x00 };  // Create Schedule
    //     unsigned char crt_evnt1[] = { 0x04, 0x80, 0x15, 0x09, 0x01, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x20, 0x00, 0x00 }; // Create Event 1
        
    //     unsigned char sync_msg1[] = { 0x04, 0x80, 0x1B, 0x01, 0xAA, 0x00 }; // Sync Message 1

    //     chan_set1[(sizeof(chan_set1) / sizeof(*chan_set1)) - 1] = checksum(chan_set1, (sizeof(chan_set1) / sizeof(*chan_set1)));
    //     write(fd, chan_set1, (sizeof(chan_set1) / sizeof(*chan_set1))); // Channel 1 Setup
                                                                        
    //     usleep(1000000);

    //     chan_set2[(sizeof(chan_set2) / sizeof(*chan_set2)) - 1] = checksum(chan_set2, (sizeof(chan_set2) / sizeof(*chan_set2)));
    //     write(fd, chan_set2, (sizeof(chan_set2) / sizeof(*chan_set2))); // Channel 2 Setup
                                                                        
    //     usleep(1000000);

    //     chan_set3[(sizeof(chan_set3) / sizeof(*chan_set3)) - 1] = checksum(chan_set3, (sizeof(chan_set3) / sizeof(*chan_set3)));
    //     write(fd, chan_set3, (sizeof(chan_set3) / sizeof(*chan_set3))); // Channel 3 Setup
                                                                        
    //     usleep(1000000);

    //     chan_set4[(sizeof(chan_set4) / sizeof(*chan_set4)) - 1] = checksum(chan_set4, (sizeof(chan_set4) / sizeof(*chan_set4)));
    //     write(fd, chan_set4, (sizeof(chan_set4) / sizeof(*chan_set4))); // Channel 4 Setup
                                                                        
    //     usleep(1000000);

    //     crt_sched[(sizeof(crt_sched) / sizeof(*crt_sched)) - 1] = checksum(crt_sched, (sizeof(crt_sched) / sizeof(*crt_sched)));
    //     write(fd, crt_sched, (sizeof(crt_sched) / sizeof(*crt_sched))); // Create Schedule
                                                                        
    //     usleep(1000000);

    //     crt_evnt1[(sizeof(crt_evnt1) / sizeof(*crt_evnt1)) - 1] = checksum(crt_evnt1, (sizeof(crt_evnt1) / sizeof(*crt_evnt1)));
    //     write(fd, crt_evnt1, (sizeof(crt_evnt1) / sizeof(*crt_evnt1))); // Create Event 1
                                                                        
    //     usleep(1000000);

    //     sync_msg1[(sizeof(sync_msg1) / sizeof(*sync_msg1)) - 1] = checksum(sync_msg1, (sizeof(sync_msg1) / sizeof(*sync_msg1)));
    //     write(fd, sync_msg1, (sizeof(sync_msg1) / sizeof(*sync_msg1))); // Sync Message 1
                                                                        
    //     usleep(1000000);

    //     printf("Setup Complete. \n");

    // } // Board_Setup_0

    // std::string Stimulator::get_name(){
    //     return name_;
    // }
