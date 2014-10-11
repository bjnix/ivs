/*
   Exit if no packet received within 2 seconds (except when we haven't received any packets yet).
   Allow access to the frustum arguments passed to the clients.
   Maybe provide some helper functions that set up the frustums automatically.
 */

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
#include <pthread.h>
#include <string>
#include <sstream>
#include <vector>

#include <GL/glew.h>
#include <GL/glut.h>

#include "STLCommon.h"

using namespace std;



STLCommon::STLCommon(bool isMasterIn, int argc, char **argv)
{
	isMaster = isMasterIn;

	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

	if(isMaster)
	{
		if (argc != 2) {
			printf("USAGE: %s relay-ip-address\n", argv[0]);
			exit(1);
		}
		RELAY_IP = argv[1];

		glutInitWindowSize(800, 800);
		glutInitWindowPosition(100, 100);
		glutCreateWindow(argv[0]);
	}
	else
	{
		frust_left    = atof(argv[1]);
		frust_right   = atof(argv[2]);
		frust_bottom  = atof(argv[3]);
		frust_top	  = atof(argv[4]);
		screen_width  = atoi(argv[5]);
		screen_height = atoi(argv[6]);


		// setup window
		glutInitWindowSize(screen_width, screen_height);
		glutInitWindowPosition(0, 0);
		glutCreateWindow("STL Slave Node");
	}

	if(isMaster)
		setupSocketMaster();
	else
		setupSocketSlave();

	sharedStruct = NULL;
	sharedStructSize = 0;


}


void STLCommon::getSharedData(void* ptr, unsigned long size)
{
	if(sharedStructSize == size)
	{
		memcpy(ptr,sharedStruct,size);
		if( *((char*) ptr) == 0)
		{
			printf("Received exit code");
			exit(1);
		}
	}
	else
	{
		printf("getSharedData(): You requested data with length %d, but we received data with %d\n", size, sharedStructSize);
	}



}
void STLCommon::setSharedData(void* ptr, unsigned long size)
{
	if(size != sharedStructSize)
	{
		if(sharedStruct != NULL)
			free(sharedStruct);
		sharedStruct = malloc(size);
		sharedStructSize = size;
	}
	memcpy(sharedStruct, ptr, size);

}



// Function used by master process
void * STLCommon::sender_thread() {
	unsigned long timestamp = 0;
	
	while (true) {
		timestamp++;
		printf("Sending timestamp: %d\n", timestamp);
		int curFrag = 1; // which fragment are we on?
		int needToSend = sharedStructSize; // how many bytes do we still need to send
		int sent = 0; // how many byte have we sent?
		while(needToSend > 0)
		{
			// allocate buffer for up to 1024 bytes of struct and for the header
			int BUFLEN = sizeof(unsigned long)+2*sizeof(unsigned int)+1024;
			char buf[BUFLEN];

			// add timestamp header
			memcpy(&(buf[0]), &timestamp, sizeof(unsigned long));
			// add current fragment number.
			memcpy(&(buf[sizeof(unsigned long)]), &curFrag, sizeof(int));
			int last = 1;
			int thisPacketSize = needToSend;
			if(needToSend > 1024)
			{
				last = 0;
				thisPacketSize = 1024;
			}
			// indicate if this is the last packet we need to send.
			memcpy(&(buf[sizeof(unsigned long)+sizeof(int)]), &last, sizeof(int));
			memcpy(&(buf[sizeof(unsigned long)+sizeof(int)*2]), ((char*)sharedStruct)+sent, thisPacketSize);

			curFrag++;
			needToSend = needToSend - sent;
			sent = sent + thisPacketSize;
			
			if (sendto(sockfd, buf, thisPacketSize+sizeof(unsigned long)+sizeof(int)*2,
					   0, (struct sockaddr*)&si_other, sizeof(si_other)) == -1)
				error ("ERROR sender_thread sendto()");
			
		}
		usleep(32000);
	}
	return NULL;
}

// Function used by slave process
void * STLCommon::receiver_thread() {
	// TODO: We are hardcoding a 100 fragment limit!
	int BUFLEN = 1024*100; // size of largest assembled packet (a fragment is 1024 bytes)
	char buf[BUFLEN]; // buffer for assembled packet
	char *bufLoc = buf; // pointer to where we are written into buffer
	int bufWritten = 0; // bytes written to buffer so far
	int expectTimeStamp = 0; // timestamp we are expecting
	
	while (true) {

		int isLastFrag = 0; // is this the last fragment?
		do {
			int expectfragNum = 1; // expected fragment number
			
			int FRAGLEN=sizeof(unsigned long)+2*sizeof(unsigned int)+1024; // size of fragment+header
			char fragbuf[FRAGLEN]; // buffer to store fragment

			// read fragment from socket
			socklen_t slen = sizeof(si_other);
			ssize_t amtRecvd = recvfrom(sockfd, fragbuf, FRAGLEN, 0, (struct sockaddr*)&si_other, &slen);
			if(amtRecvd == -1)
				error("ERROR recvfrom()");
			// parse out header
			long timeStamp;
			memcpy(&timeStamp, fragbuf, sizeof(unsigned long));
			int fragNum;
			memcpy(&fragNum, &fragbuf[sizeof(unsigned long)], sizeof(unsigned int));
			memcpy(&isLastFrag, &fragbuf[sizeof(unsigned long)+sizeof(unsigned int)], sizeof(unsigned int));

			// if we are on a timestamp we haven't seen before
			if(timeStamp > expectTimeStamp)
			{
				if(fragNum == 1)
				{
					// timestamp advanced and we are on the first fragment.
					expectTimeStamp = timeStamp;
					// copy payload into packet buffer
					memcpy((void*)bufLoc, &fragbuf[sizeof(unsigned long)+sizeof(unsigned int)*2],
						   amtRecvd-sizeof(unsigned long)+sizeof(unsigned int)*2);
					bufWritten += amtRecvd-sizeof(unsigned long)-sizeof(unsigned int)*2;
					bufLoc = buf + bufWritten;
				}
				else
				{
					// timestamp advanced, but we missed the first fragment
					bufLoc = buf;
					expectfragNum = 1;
					bufWritten = 0;
					isLastFrag = 0;
				}
			}
			else if(timeStamp == expectTimeStamp)
			{
				if(fragNum == expectfragNum) // timestamp and fragment # are as expected
				{
					printf("read expect timestamp frag=1\n");
					memcpy((void*)bufLoc, &fragbuf[sizeof(unsigned long)+sizeof(unsigned int)*2],
						   amtRecvd-sizeof(unsigned long)+sizeof(unsigned int)*2);
					bufWritten += amtRecvd-sizeof(unsigned long)-sizeof(unsigned int)*2;
					bufLoc = buf + bufWritten;
				}
				else
				{ // timestamp is correct, but we found an unexpected fragment
					bufLoc = buf;
					expectfragNum = 1;
					bufWritten = 0;
					isLastFrag = 0;
				}
			}
		} while(isLastFrag == 0);

		if(bufWritten > sharedStructSize)
		{
			if(sharedStruct != NULL)
				free(sharedStruct);
			sharedStruct = malloc(bufWritten);
			sharedStructSize = bufWritten;
		}

		memcpy(sharedStruct, buf, bufWritten);
	}
	return NULL;
}

/* helper functions to allow us to use pthreads with c++ */
void * Launch_receiver_thread(void *obj)
{
	return ((STLCommon *)obj)->receiver_thread();
}
void * Launch_sender_thread(void *obj)
{
	return ((STLCommon *)obj)->sender_thread();
}

void STLCommon::setupSocketMaster()
{
	if ((sockfd=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
		error("ERROR socket");

	int so_broadcast = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &so_broadcast, sizeof(so_broadcast));
	
	memset((char *) &si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(RELAY_LISTEN_PORT);
	if (inet_aton(RELAY_IP, &si_other.sin_addr) == 0) {
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}
	
	pthread_t thread;
	if (pthread_create(&thread, NULL, &Launch_sender_thread, (void*)this) != 0) {
		perror("Can't start sender thread, terminating\n");
		exit(1);
	}
}

void STLCommon::setupSocketSlave()
{
	if ((sockfd=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
		error("ERROR socket");

	memset((char *) &si_me, 0, sizeof(si_me));
	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(SLAVE_LISTEN_PORT);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(sockfd, (struct sockaddr*)&si_me, sizeof(si_me)) == -1)
		error("ERROR bind");
	
	// listen for updates
	pthread_t thread;
	if (pthread_create(&thread, NULL, &Launch_receiver_thread, (void*)this) != 0) {
		perror("Can't start receiver thread, terminating");
		exit(1);
	}
}


void STLCommon::error(const char *msg) {
	perror(msg);
	exit(1);
}



void STLCommon::accFrustum(float myleft, float myright, // left/right frustum
			   float mybottom, float mytop, // bottom/top frustum
			   float mynear, float myfar,   // near/far frustum
			   float mypixdx, float mypixdy, 
			   float myeyedx, float myeyedy, // eye position
			   float myfocus)
{
	float xwsize, ywsize;
	float dx, dy;
	int viewport[4];

	glGetIntegerv (GL_VIEWPORT, viewport);

	xwsize = myright - myleft; // frustum width
	ywsize = mytop - mybottom; // frustum height
	dx = -((mypixdx*xwsize)/(float)viewport[2] + (myeyedx*mynear)/myfocus);
	dy = -(mypixdy*ywsize/(float) viewport[3] + myeyedy*mynear/myfocus);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum (myleft + dx, myright + dx, mybottom + dy, mytop + dy, mynear, myfar);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef (-myeyedx, -myeyedy, 0.0);
	glFrustum(myleft, myright, mybottom, mytop, mynear, myfar);
}

void STLCommon::setViewFrustum(float myfovy, float myaspect,
			       float mynear, float myfar, float mypixdx, float mypixdy,
			       float myeyedx, float myeyedy, float myfocus)
{
    float myleft,myright,mybottom,mytop;

    if(isMaster)
    {
        float fov2;
	fov2 = ((myfovy*M_PI) / 180.0) / 2.0;
	mytop = mynear/(cos(fov2)/sin(fov2));
	mybottom = -mytop;
	myright = mytop * myaspect;
	myleft = -myright;
    }
    else
    {
        myleft = frust_left;
	myright = frust_right;
	mytop = frust_top;
	mybottom = frust_bottom;
    }

    accFrustum(myleft, myright,
	       mybottom, mytop,
	       mynear, myfar,
	       mypixdx, mypixdy, myeyedx, myeyedy, myfocus);
}



