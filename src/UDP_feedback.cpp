// Continuation of Mike Brodnik's code to control the stim board provided by CWRU
// Last Edit: 08.16.17 
// Kyra Rudy


#include <stdio.h>      // standard input / output functions
#include <string.h> 	// string function definitions
#include <cstring>
#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <termios.h>	// POSIX terminal control definitionss
#include <time.h>       // time calls
#include <iostream> 	// input / output streaming
#include <sys/select.h> //
#include <sys/ioctl.h>  //
#include <stropts.h>    //
#include <ostream>
#include <fstream>

// UDP
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

// Signal Handler
#include <signal.h>

// Timer
#include <time.h>
#include <sys/time.h>
#include <math.h>

#define MYPORT "8888"	// the port users will be connecting to
#define MAXBUFLEN 100
#define PULSEMAX 16
#define GAIN 0.2

// Total time can be changed on line 227

// Definition of change event variable
// Change chngevnt1[6] to the appropriate amplitude: 0x3C = 60, 0x46 = 70, 0x50 = 80
unsigned char chngevnt1[] = { 0x04, 0x80, 0x19, 0x04, 0x01, 0x00, 0x46, 0x00, 0x00 }; 


using namespace std;	// needed for the iostream
int fd0;            // Global variable to store serial port parameters for serial port ttyUSB0
int fd1;            // Global variable to store serial port parameters for serial port ttyUSB1


int open_port0(void) // open_port0 is a function that will open the first serial port to communicate with the first stim board
{
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


int open_port1(void) // open_port1 is a function that will open the second serial port to comunicate with the second stim board
{
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


int configure_port(int fd)  // configure_port establishes the settings for each serial port
{
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


int checksum(unsigned char myarray[], int m)  // checksum is a function that preforms checksums of all of the byte strings used in this code
{
	int csum = 0;
	for (int i = 0; i < m - 1; i++) {
		csum += myarray[i];
	}
	csum = ((0x00FF & csum) + (csum >> 8)) ^ 0xFF;
	return csum;
} // checksum


  //The Board_Setup function sends multiple strings of bytes in order to set up the stim board.
int Board_Setup_0(int fd)   // Board_Setup
{

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



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


volatile sig_atomic_t interrupt = false; // global interrupt variable

// Create signal handler that changes the value of interrupt
void signal_handler(int signum)
{
	interrupt = true;
}


// UDP
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


extern "C" int server(void);

int server(void)
{
	// UDP Definitions
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;
	struct sockaddr_storage their_addr;
	uint8_t buf[MAXBUFLEN];
	socklen_t addr_len;
	char s[INET6_ADDRSTRLEN];
	char strbuf[18];
	// Controller Defintions
	int pulse, PW;
	float theta;
	// Timer Definitions
	struct timeval start_t, current_t;
	double tstep = 0.1, elapsed_t, total_t = 5, old_time, new_time;
	int idx;
	// File read definitions
	int i = 0;
	float num, pulsewidth;  
	float desired_angle[500];
	float open_loop[500]; 
	
	

	FILE *fr;   // create a pointer to the file to read trajectory data in from
    	fr = fopen("/home/michael/Desktop/traj_data.txt", "r");	// open the file for reading data
    	if (fr == NULL)
    	{
        	printf("Error opening the trajectory data file\n");
        	return 0;
    	}
		

	// Copy data from text file to float array
	while(fscanf(fr, "%f", &num) > 0)
	{
		desired_angle[i] = num;
		i++;
	}
	fclose(fr);
	

	FILE *fc;   // create a pointer to the file to read open loop control data in from
    	fc = fopen("/home/michael/Desktop/open_stim.txt", "r");	// open the file for reading data
    	if (fc == NULL)
    	{
        	printf("Error opening the open loop control data file\n");
        	return 0;
    	}	

	i = 0;
	// Copy data from text file to float array
	while(fscanf(fc, "%f", &pulsewidth) > 0)
	{
		open_loop[i] = pulsewidth;
		i++;
	}
	fclose(fc);



	FILE *fp;   // create a pointer to the file for data logging
    	fp = fopen("/home/michael/Desktop/logged_data.txt", "w");	// open the file for data logging
    	if (fp == NULL)
    	{
        	printf("Error opening the data file\n");
        	return 0;
    	}

	FILE *flog;   // create a pointer to the file for recording pulse width for future open loop control
    	flog = fopen("/home/michael/Desktop/pulse_log.txt", "w");	// open the file for data logging
    	if (flog == NULL)
    	{
        	printf("Error opening the data file\n");
        	return 0;
    	}

	//UDP
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, MYPORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// UDP: loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("listener: socket");
			continue;
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("listener: bind");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "listener: failed to bind socket\n");
		return 2;
	}

	freeaddrinfo(servinfo);
	//End UDP Setup
	
	old_time = 0;

	gettimeofday(&start_t, NULL); //record time at controller start

	signal(SIGINT, signal_handler); //trigger signal handler when CTRL+C is pressed

	while(!interrupt) //loop is terminated via signal handler
	{
	
	// Get UDP packets
	addr_len = sizeof their_addr;
	if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,
		(struct sockaddr *)&their_addr, &addr_len)) == -1) {
		perror("recvfrom");
		exit(1);
	}

	//printf("Got packet from %s\n", inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s));

	// Interpret UDP packet (IEEE754 double precision 64 bit to float)
	sprintf(strbuf, "%02X%02X%02X%02X%02X%02X%02X%02X", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
	unsigned long long angle = strtoull(strbuf, NULL, 16);
	char str[5];
	sprintf(str, "%2.2f", angle);
	theta = atof(str);
	

	gettimeofday(&current_t, NULL); // record time of current loop iteration
	elapsed_t = (double) (current_t.tv_sec + current_t.tv_usec * 0.000001) - (start_t.tv_sec + start_t.tv_usec * 0.000001);
	printf("Elapsed time: %f\n", elapsed_t);

	idx = round(elapsed_t / tstep);
	if(idx > (total_t / tstep) + 1) // stop stimulation once trajectory time has elapsed
	{
		close(sockfd);		
		fclose(fp);
		fclose(flog);
		return 0;
	}

	pulse = round(open_loop[idx] + GAIN * (desired_angle[idx] - theta));	

	// Make sure that the pulse width is positive and less than the maximum defined by the participant's pain threshold
	if(pulse < 0)
	{
		PW = 0;
	}
	else if(pulse > PULSEMAX)
	{
		PW = PULSEMAX;
	}
	else
	{
		PW = pulse;
	}


	new_time = elapsed_t * 1000; // convert to ms
	if(new_time - old_time > 100)
	{
		chngevnt1[5] = PW;
		chngevnt1[(sizeof(chngevnt1) / sizeof(*chngevnt1)) - 1] = checksum(chngevnt1, (sizeof(chngevnt1) / sizeof(*chngevnt1)));
		write(fd0, chngevnt1, (sizeof(chngevnt1) / sizeof(*chngevnt1)));		
		old_time = new_time;
		fprintf(flog, "%d\n", PW);
	}



	fprintf(fp, "%2.5lf\t%2.2f\t%2.2f\t%2.2f\t%d\t%d\n", elapsed_t, desired_angle[idx], theta, open_loop[idx], pulse, PW);

	}

	close(sockfd);
	fclose(fp);
	fclose(flog);

	return 0;		
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





int main()
{



	fd0 = open_port0();                  // Open serial port ttyUSB0 and store parameters in fd0
	fd1 = open_port1();                  // Open serial port ttyUSB1 and store parameters in fd1
	configure_port(fd0);                 // Configure the parameters for serial port ttyUSB0
	Board_Setup_0(fd0);                  // Write stim board setup commands to serial port ttyUSB0
	
	server();
    
	// Set pulse width to zero at the end of stimulation
	chngevnt1[5] = 0;
	chngevnt1[(sizeof(chngevnt1) / sizeof(*chngevnt1)) - 1] = checksum(chngevnt1, (sizeof(chngevnt1) / sizeof(*chngevnt1)));
	write(fd0, chngevnt1, (sizeof(chngevnt1) / sizeof(*chngevnt1)));  

 	
    return 1;

}
