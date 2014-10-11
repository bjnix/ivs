#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <pthread.h>
#include <string>
#include <sstream>
#include <vector>

#define RELAY_LISTEN_PORT 25885
#define SLAVE_LISTEN_PORT 25884

// TODO set maximum packet payload to a reasonable size.
#define BUFLEN 2000
char *RELAY_OUT_IP = NULL;


using namespace std;

int s_R, s_S, milliseconds;
struct timespec req;
pthread_t receiverThread;
int so_broadcast = 1;

struct sockaddr_in si_me_R, si_other_R;
socklen_t slen_R;
struct sockaddr_in si_me_S, si_other_S;
int slen_S;

// autoclose
int framesPassed = 0;
bool receivedPacket = false;

// pose data
float tranx, trany, tranz, angle;
float m_curquat[4];

vector<string> &split(const string &s, char delim, vector<string> &elems) {
	stringstream ss(s);
	string item;
	while (getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

vector<string> split(const string &s, char delim) {
	vector<string> elems;
	return split(s, delim, elems);
}

void error(const char *msg)
{
	perror(msg);
	exit(1);
}


void *receiver(void *data) {
	char buf[BUFLEN];
	while (true) {
		// receive data
		ssize_t amtRecvd = recvfrom(s_R, buf, BUFLEN, 0, (struct sockaddr*)&si_other_R, &slen_R);
		if (amtRecvd == -1)
			error("ERROR recvfrom()");

		if(amtRecvd > 0)
		{
			// forward data
			printf("forwarding %d\n", amtRecvd);
			if (sendto(s_S, buf, amtRecvd, 0, (struct sockaddr*)&si_other_S,
					   slen_S) == -1) error ("ERROR sendto()");
			receivedPacket = true;
			framesPassed = 0;
		}
		
		nanosleep(&req, (struct timespec *)NULL);
	}
	return NULL;
}

int main(int argc, char **argv) {
	if (argc != 2) {
		printf("USAGE: %s ip-to-send-to\n", argv[0]);
		return 1;
	}
	RELAY_OUT_IP=argv[1];
	
	milliseconds = 16;
	req.tv_sec = 0;
	req.tv_nsec = milliseconds * 1000000L;

	// socket stuff
	slen_R=sizeof(si_other_R);
	if ((s_R=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) error("ERROR socket");
	memset((char *) &si_me_R, 0, sizeof(si_me_R));
	si_me_R.sin_family = AF_INET;
	si_me_R.sin_port = htons(RELAY_LISTEN_PORT);
	si_me_R.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(s_R, (struct sockaddr*)&si_me_R, sizeof(si_me_R)) == -1) error("ERROR bind");

	// listen for updates
	if (pthread_create(&receiverThread, NULL, &receiver, NULL) != 0) {
		perror("Can't start thread, terminating");
		return 1;
	}

	// master init
	slen_S=sizeof(si_other_S);

	if ((s_S=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) error("ERROR socket");

	setsockopt(s_S, SOL_SOCKET, SO_BROADCAST, &so_broadcast, sizeof(so_broadcast));

	memset((char *) &si_other_S, 0, sizeof(si_other_S));
	si_other_S.sin_family = AF_INET;
	si_other_S.sin_port = htons(SLAVE_LISTEN_PORT);
	if (inet_aton(RELAY_OUT_IP, &si_other_S.sin_addr) == 0) {
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}

	printf("Relay initialization complete.\n");

	while (true) {
		usleep(1000000);
		framesPassed++;
		if (receivedPacket) {
			if (framesPassed > 3) exit(0); // time out after >3 seconds w/o receiving a packet
		} else {
			if (framesPassed > 15) exit(0); // give it longer if it hasn't initialized yet
		}
	}

}
