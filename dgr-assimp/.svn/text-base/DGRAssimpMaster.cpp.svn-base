#include <GL/glut.h>

#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <math.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>

#define SEND_IP "255.255.255.255"  // broadcast address
#define BUFLEN 512
#define PORT 25884

// DGR variables
struct sockaddr_in si_other;
int slen, s;
char buf [BUFLEN];
int so_broadcast = 1;

using namespace std;

const GLdouble SCREEN_WIDTH = (1920*6)/8.0;  
const GLdouble SCREEN_HEIGHT = (1080.0*4)/8.0;
const float screenAspectRatio = SCREEN_WIDTH/SCREEN_HEIGHT;

float rotation = 0.0f;

void error(const char *msg) {
  perror(msg);
  exit(1);
}

void closeProgram() {
  exit(0);
}

int main(int argc, char** argv) {

  slen=sizeof(si_other);
  if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) error("ERROR socket");
  setsockopt(s, SOL_SOCKET, SO_BROADCAST, &so_broadcast, sizeof(so_broadcast));
  memset((char *) &si_other, 0, sizeof(si_other));
  si_other.sin_family = AF_INET;
  si_other.sin_port = htons(PORT);
  if (inet_aton(SEND_IP, &si_other.sin_addr) == 0) {
    fprintf(stderr, "inet_aton() failed\n");
    exit(1);
  }


  while (true) {
    rotation += 0.01f;
    sprintf(buf, "%f", rotation);
    if (sendto(s, buf, BUFLEN, 0, (struct sockaddr*)&si_other, slen) == -1) error("ERROR sendto()");
    usleep(4000);
  }

  return 0;
}
