#include <FES/Utility/VirtualStim.hpp>
#include <Windows.h>
#include <tchar.h>
#include <codecvt>
#include <mahi/gui.hpp>
#include <thread>
#include <mutex>
#include <MEL/Core/Clock.hpp>

using namespace fes;
using namespace mel;

VirtualStim::VirtualStim(const std::string& com_port_):
Application(1300,800,"Stimulator Visualization"),
com_port(com_port_)
{
    open_port();
    configure_port();

    ImGui::StyleColorsLight();
    // hideWindow();
    poll_thread = std::thread(&VirtualStim::poll, this);
    run();
}

VirtualStim::~VirtualStim(){
    poll_thread.join();
    run_thread.join();
}

void VirtualStim::begin(){
    run();
}

void VirtualStim::update(){
    ImGui::Begin("Virtual Stimulator Receiver", &open);
    add_monitor(recent_message);
    add_monitor(channel_setup_message);
    add_monitor(scheduler_setup_message);
    add_monitor(scheduler_sync_message);
    add_monitor(event_create_message);
    add_monitor(event_edit_1_message);
    add_monitor(event_edit_2_message);
    add_monitor(event_edit_3_message);
    add_monitor(event_edit_4_message);
    add_monitor(unknown_message);
    // ImGui::Text("Hello");
    ImGui::End();
}

void VirtualStim::add_monitor(SerialMessage ser_msg){
    std::string temp_msg_unc = "|";
    std::string temp_msg_int = "|";
    
    for (auto i = 0; i < ser_msg.message.size(); i++){
        char char_buff[4];
        sprintf(char_buff, "0x%02X", (unsigned int)ser_msg.message[i]);
        temp_msg_unc += char_buff;

        char int_buff[4];
        sprintf(int_buff, "%04i", (unsigned int)ser_msg.message[i]);
        temp_msg_int += int_buff;
        if(i == 3){
            temp_msg_unc += " | ";
            temp_msg_int += " | ";
        }
        else if(i != (ser_msg.message.size()-1)){
            temp_msg_unc += ", ";
            temp_msg_int += ", ";
        }
        else{
            temp_msg_unc += "|";
            temp_msg_int += "|";
        }
    }

    char time_buff[7];
    sprintf(time_buff, "%4.2f", ser_msg.time);
    
    ImGui::Separator();
    ImGui::Separator();
    ImGui::Text((ser_msg.message_type + " at " + time_buff).c_str());
    ImGui::Separator();
    // ImGui::Text("           |        HEADER         |            BODY             |");
    ImGui::Text(("INT format:" + temp_msg_int).c_str());
    ImGui::Text(("HEX format:" + temp_msg_unc).c_str());
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
    // bool done_reading = false;
    Clock poll_clock;
    poll_clock.restart();
    while (true){
        
        DWORD header_size = 4;
        unsigned char msg_header[4];
        
        DWORD dwBytesRead = 0;
        
        if(!ReadFile(hComm, msg_header, header_size, &dwBytesRead, NULL)){
            // done_reading = true;
        }
        else{
            // std::cout << "Message Header: ";
            // for (unsigned int i = 0; i < header_size; i++){
                // std::cout << (unsigned int)msg_header[i];
                // if(i != (header_size-1)) std::cout << ", ";
            // }
            // std::cout << std::endl;

            DWORD body_size = (unsigned int)msg_header[3]+1;
            // unsigned char *msg_body = new unsigned char[body_size];
            std::unique_ptr<unsigned char[]> msg_body(new unsigned char[body_size]);
            if(!ReadFile(hComm, msg_body.get(), body_size, &dwBytesRead, NULL)){
                LOG(Error) << "Could not read message body";
                // done_reading = true;
            }
            else{
                // std::cout << "Message: ";
                // for (unsigned int i = 0; i < body_size; i++){
                //     std::cout << (unsigned int)msg_body[i];
                //     if(i != (body_size-1)) std::cout << ", ";
                // }
                // std::cout << std::endl;
            }
            if(msg_header[0] == (unsigned char)0x04 && msg_header[1] == (unsigned char)0x80){
                std::vector<unsigned char> msg;
                for (auto i = 0; i < (header_size + body_size); i++){
                    if (i < header_size) msg.push_back(msg_header[i]) ;
                    else msg.push_back(msg_body[i-header_size]);
                }
                recent_message.message = msg;
                recent_message.time = poll_clock.get_elapsed_time().as_seconds();
                switch (msg_header[2]){
                    case (unsigned char)0x47:
                        channel_setup_message.message = msg;
                        channel_setup_message.time = poll_clock.get_elapsed_time().as_seconds();
                        break;
                    case (unsigned char)0x10:
                        scheduler_setup_message.message = msg;
                        scheduler_setup_message.time = poll_clock.get_elapsed_time().as_seconds();
                        break;
                    case (unsigned char)0x1B:
                        scheduler_sync_message.message = msg;
                        scheduler_sync_message.time = poll_clock.get_elapsed_time().as_seconds();
                        break;
                    case (unsigned char)0x15:
                        event_create_message.message = msg;
                        event_create_message.time = poll_clock.get_elapsed_time().as_seconds();
                        break;
                    case (unsigned char)0x19:
                        if (msg_body[0] == (unsigned char)0x01){
                            event_edit_1_message.message = msg;
                            event_edit_1_message.time = poll_clock.get_elapsed_time().as_seconds();
                        }
                        else if (msg_body[0] == (unsigned char)0x02){
                            event_edit_2_message.message = msg;
                            event_edit_2_message.time = poll_clock.get_elapsed_time().as_seconds();
                        }
                        else if (msg_body[0] == (unsigned char)0x03){
                            event_edit_3_message.message = msg;
                            event_edit_3_message.time = poll_clock.get_elapsed_time().as_seconds();
                        }
                        else if (msg_body[0] == (unsigned char)0x04){
                            event_edit_4_message.message = msg;
                            event_edit_4_message.time = poll_clock.get_elapsed_time().as_seconds();
                        }
                        
                        
                        break;
                    default:
                        unknown_message.message = msg;
                        unknown_message.time = poll_clock.get_elapsed_time().as_seconds();
                        break;
                }
            }   
        }        
        
    }
}