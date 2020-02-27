#include <FES/Utility/VirtualStim.hpp>
#include <Windows.h>
#include <tchar.h>
#include <codecvt>

using namespace fes;
using namespace mel;

VirtualStim::VirtualStim(const std::string& com_port_):
com_port(com_port_)
{
    open_port();
    configure_port();
    while(true){
        poll();
    }
}

VirtualStim::~VirtualStim(){

}

bool VirtualStim::open_port(){

    // the comport must be formatted as an LPCWSTR, so we need to get it into that form from a std::string
    std::wstring com_prefix = L"\\\\.\\";
    std::wstring com_suffix = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(com_port);
    std::wstring comID = com_prefix + com_suffix;
    
    // std::wstring stemp = std::wstring(com_port_formatted.begin(), com_port_formatted.end());
    // LPCWSTR com_port_lpcwstr = stemp.c_str();

    hComm = CreateFileW(comID.c_str(),                // port name
                        GENERIC_READ | GENERIC_WRITE, // Read/Write
                        0,                            // No Sharing
                        NULL,                         // No Security
                        OPEN_EXISTING,                // Open existing port only
                        0,                            // Non Overlapped I/O
                        NULL);                        // Null for Comm Devices

    // Check if creating the comport was successful or not and log it
    if (hComm == INVALID_HANDLE_VALUE){
        LOG(Error) << "Failed to open Virtual Stimulator";
        return false;
    }
    else{
        LOG(Info) << "Opened Virtual Stimulator";
    }

    return true;
}

bool VirtualStim::configure_port(){  // configure_port establishes the settings for each serial port

    // http://bd.eduweb.hhs.nl/micprg/pdf/serial-win.pdf

    dcbSerialParams.DCBlength = sizeof(DCB);

    if (!GetCommState(hComm, &dcbSerialParams)) {
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

    // Set communication parameters for the serial port
    if(!SetCommState(hComm, &dcbSerialParams)){
        LOG(Error) << "Error setting serial port state";
        return false;
    }

    COMMTIMEOUTS timeouts={0};
    timeouts.ReadIntervalTimeout=50;
    timeouts.ReadTotalTimeoutConstant=50;
    timeouts.ReadTotalTimeoutMultiplier=10;
    timeouts.WriteTotalTimeoutConstant=50;
    timeouts.WriteTotalTimeoutMultiplier=10;
    if(!SetCommTimeouts(hComm, &timeouts)){
        LOG(Error) << "Error setting serial port timeouts";
        return false;
    }
    return true;
}

void VirtualStim::poll(){
    bool done_reading = false;
    while (!done_reading){
        
        DWORD header_size = 4;
        unsigned char msg_header[4];
        
        DWORD dwBytesRead = 0;
        
        if(!ReadFile(hComm, msg_header, header_size, &dwBytesRead, NULL)){
            done_reading = true;
        }
        else{
            std::cout << "Message Header: ";
            for (unsigned int i = 0; i < header_size; i++){
                std::cout << (unsigned int)msg_header[i];
                if(i != (header_size-1)) std::cout << ", ";
            }
            std::cout << std::endl;

            DWORD body_size = (unsigned int)msg_header[3]+1;
            unsigned char *msg_body = new unsigned char[body_size];
            if(!ReadFile(hComm, msg_body, body_size, &dwBytesRead, NULL)){
                LOG(Error) << "Could not read message body";
                done_reading = true;
            }
            else{
                std::cout << "Message: ";
                for (unsigned int i = 0; i < body_size; i++){
                    std::cout << (unsigned int)msg_body[i];
                    if(i != (body_size-1)) std::cout << ", ";
                }
                std::cout << std::endl;
            }
            // clear up memory from declaring char vector with new operator
            delete[] msg_body;
        }        

        
        
    }
    
}