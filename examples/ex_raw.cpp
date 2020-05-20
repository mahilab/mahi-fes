#include <Mahi/Fes.hpp>
#include <Mahi/Util.hpp>
#include <mutex>
#include <thread>

using namespace mahi::util;
using namespace mahi::fes;

// create global stop variable CTRL-C handler function
ctrl_bool stop(false);
bool      handler(CtrlEvent event) {
    stop = true;
    return true;
}

int checksum(unsigned char myarray[], int m)  // checksum is a function that preforms checksums of all of the byte strings used in this code
{
	int csum = 0;
	for (int i = 0; i < m - 1; i++) {
		csum += myarray[i];
	}
	csum = ((0x00FF & csum) + (csum >> 8)) ^ 0xFF;
	return csum;
} // checksum

int main() {
    // register ctrl-c handler
    register_ctrl_handler(handler);

    std::vector<Channel> channels;

    Channel bicep("bicep", CH_1, AN_CA_1, 100, 250);
    channels.push_back(bicep);

    Channel tricep("tricep", CH_2, AN_CA_2, 100, 250);
    channels.push_back(tricep);

    Channel forearm("forearm", CH_3, AN_CA_3, 100, 250);
    channels.push_back(forearm);

    Channel wrist("wrist", CH_4, AN_CA_4, 100, 250);
    channels.push_back(wrist);

    // Create stim board with a name, comport, and channels to add
    Stimulator stim("UECU Board", "COM11", channels, channels.size(), false);

    // Initialize scheduler with the sync character and frequency of scheduler in hertz
    stim.create_scheduler(0xAA, 40);

    // unsigned char chan_set1[] = { 0x04, 0x80, 0x47, 0x07, 0x00, 0x64, 0xFA, 0x00, 0x64, 0x11, 0x01, 0x00 };  // channel 1 setup
    // chan_set1[(sizeof(chan_set1) / sizeof(*chan_set1)) - 1] = checksum(chan_set1, (sizeof(chan_set1) / sizeof(*chan_set1)));

	// unsigned char chan_set2[] = { 0x04, 0x80, 0x47, 0x07, 0x01, 0x64, 0xFA, 0x00, 0x64, 0x11, 0x23, 0x00 };  // Channel 2 Setup
    // chan_set2[(sizeof(chan_set2) / sizeof(*chan_set2)) - 1] = checksum(chan_set2, (sizeof(chan_set2) / sizeof(*chan_set2)));

	// unsigned char chan_set3[] = { 0x04, 0x80, 0x47, 0x07, 0x02, 0x64, 0xFA, 0x00, 0x64, 0x11, 0x45, 0x00 };  // Channel 3 Setup
	// chan_set3[(sizeof(chan_set3) / sizeof(*chan_set3)) - 1] = checksum(chan_set3, (sizeof(chan_set3) / sizeof(*chan_set3)));
    
    // unsigned char chan_set4[] = { 0x04, 0x80, 0x47, 0x07, 0x03, 0x64, 0xFA, 0x00, 0x64, 0x11, 0x67, 0x00 };  // Channel 4 Setup
    // chan_set4[(sizeof(chan_set4) / sizeof(*chan_set4)) - 1] = checksum(chan_set4, (sizeof(chan_set4) / sizeof(*chan_set4)));
    
    // unsigned char crt_sched[] = { 0x04, 0x80, 0x10, 0x03, 0xAA, 0x00, 0x19, 0x00 };  // Create Schedule
    // crt_sched[(sizeof(crt_sched) / sizeof(*crt_sched)) - 1] = checksum(crt_sched, (sizeof(crt_sched) / sizeof(*crt_sched)));

	unsigned char crt_evnt1[] = { 0x04, 0x80, 0x15, 0x09, 0x01, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x20, 0x00, 0x00 }; // Create Event 1
    crt_evnt1[(sizeof(crt_evnt1) / sizeof(*crt_evnt1)) - 1] = checksum(crt_evnt1, (sizeof(crt_evnt1) / sizeof(*crt_evnt1)));
	
	unsigned char sync_msg1[] = { 0x04, 0x80, 0x1B, 0x01, 0xAA, 0x00 }; // Sync Message 1
    sync_msg1[(sizeof(sync_msg1) / sizeof(*sync_msg1)) - 1] = checksum(sync_msg1, (sizeof(sync_msg1) / sizeof(*sync_msg1)));

    unsigned char chngevnt1[] = { 0x04, 0x80, 0x19, 0x04, 0x01, 0x0A, 0x3C, 0x00, 0x00 }; 
    chngevnt1[(sizeof(chngevnt1) / sizeof(*chngevnt1)) - 1] = checksum(chngevnt1, (sizeof(chngevnt1) / sizeof(*chngevnt1)));    

    PurgeComm(stim.m_hComm,PURGE_TXABORT);
    PurgeComm(stim.m_hComm,PURGE_RXABORT);
    PurgeComm(stim.m_hComm,PURGE_RXCLEAR);
	PurgeComm(stim.m_hComm,PURGE_TXCLEAR);

    DWORD dwBytesWritten = 0;
    // WriteFile(stim.m_hComm, chan_set1, (sizeof(chan_set1) / sizeof(*chan_set1)), &dwBytesWritten, NULL);
    // sleep(5_ms);
    // WriteFile(stim.m_hComm, chan_set2, (sizeof(chan_set2) / sizeof(*chan_set2)), &dwBytesWritten, NULL);
    // sleep(5_ms);
    // WriteFile(stim.m_hComm, chan_set3, (sizeof(chan_set3) / sizeof(*chan_set3)), &dwBytesWritten, NULL);
    // sleep(5_ms);
    // WriteFile(stim.m_hComm, chan_set4, (sizeof(chan_set4) / sizeof(*chan_set4)), &dwBytesWritten, NULL);
    // sleep(5_ms);
    // WriteFile(stim.m_hComm, crt_sched, (sizeof(crt_sched) / sizeof(*crt_sched)), &dwBytesWritten, NULL);
    // sleep(5_ms);
    stim.add_event(bicep);
    // WriteFile(stim.m_hComm, sync_msg1, (sizeof(sync_msg1) / sizeof(*sync_msg1)), &dwBytesWritten, NULL);
    stim.begin();
    sleep(5_ms);
    WriteFile(stim.m_hComm, chngevnt1, (sizeof(chngevnt1) / sizeof(*chngevnt1)), &dwBytesWritten, NULL);

    while(!stop){

    }

    stim.disable();

    // viz_thread.join();

    return 0;
}
