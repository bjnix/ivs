
// Master will sent UDP packets to a relay which listens on this port
#define RELAY_LISTEN_PORT 25885
#define SLAVE_LISTEN_PORT 25884

#include <vector>
#include <string>
using namespace std;

class STLCommon
{
  public:
	STLCommon(bool isMaster, int argc, char **argv);
	void setupSocket();



	vector<string> &split(const string &s, char delim, vector<string> &elems);
	vector<string> split(const string &s, char delim);
	void* sender_thread();
	void* receiver_thread();

	void setViewFrustum(float myfovy, float myaspect,
			    float mynear, float myfar, float mypixdx, float mypixdy,
			    float myeyedx, float myeyedy, float myfocus);
	void accFrustum(float myleft, float myright, // left/right frustum
			float mybottom, float mytop, // bottom/top frustum
			float mynear, float myfar,   // near/far frustum
			float mypixdx, float mypixdy, 
			float myeyedx, float myeyedy, // eye position
			float myfocus);

	void getSharedData(void* ptr, unsigned long size);
	void setSharedData(void* ptr, unsigned long size);
	
		
	
  private:
	void setupSocketMaster();
	void setupSocketSlave();

	void* sharedStruct;
	unsigned long sharedStructSize;
	
	static void error(const char *msg);
	static void exitCallback();

	// unorganized variables
	float tranx;
	float trany;
	float tranz;
	float angle;
	float m_curquat[4];

	
	
	// variables used by both master and slave
	bool isMaster;
	int sockfd;
	sockaddr_in si_other;

	// variables used by master
	char *RELAY_IP;

	// variables used by slave
	struct sockaddr_in si_me;

	float frust_left, frust_right, frust_bottom, frust_top;
	float screen_width, screen_height; // used only by slave

};
		
