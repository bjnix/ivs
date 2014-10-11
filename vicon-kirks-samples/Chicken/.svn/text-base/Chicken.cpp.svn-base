#include "Client.h"

#include <iostream>
#include <fstream>
#include <cassert>
#include <ctime>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <vector>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

#include "OPaudio/OPaudio.h"
#include "OPaudio/OPsoundEmitter.h"

#define VecSub(out, v1, v2){\
	for(int i = 3; i--;)\
	out[i] = v1[i] - v2[i];\
}\

#define VecCpy(dest, src){\
	for(int i = 3; i--;)\
	dest[i] = src[i];\	
}\

#define VecLen(len, vec){\
	double sum=0;\
	for(int i = 3; i--; sum += vec[i]*vec[i]);\
	len = sqrt(sum);\	
}

#define STATE_JOINING 0
#define STATE_GAMESTARTING 1
#define STATE_PLAYING 2
#define STATE_GAMEOVER 3

using namespace ViconDataStreamSDK::CPP;
using namespace std;

typedef struct{
	int index;
	double Position[3];
	double CombineSpeed;
	double VelocityTolerance;
}Player;

/*--- Globals ---------------------------------------------------------------*/
// Make a new client
Client MyClient;

// cut off point which eliminates a player
double MaxVelocity = 20.0; // 10cm/sec?

// four players maximum are allowed
Player Players[4];
bool PlayersVisible[4];
vector<int> ActivePlayers;

// names of vicon subjects which will represent players
static string names[] = {
	"player1", "player2",
	"player3", "player4"
};


// Current game state
int GameState = STATE_JOINING;

OPsound Anoun_player[4];
OPsound Anoun_count, Anoun_Eliminated, Anoun_Joins, Anoun_wins;

OPSoundEmitter *AnounClip1, *AnounClip2;
/*--- Functions -------------------------------------------------------------*/
int isVisible(string subjectName){
	int MarkerCount = MyClient.GetMarkerCount( subjectName ).MarkerCount;

	for(int i = MarkerCount; i--;){
		string markerName = MyClient.GetMarkerName( subjectName, i ).MarkerName;
		Output_GetMarkerGlobalTranslation posData = 
				MyClient.GetMarkerGlobalTranslation( subjectName, markerName );

		if(!posData.Occluded){
			return 1;
		}
	}

	return 0;
}

int GetSegmentPosition(string subjectName, double* position){
	//unsigned int segmentCount = MyClient.GetSegmentCount(subjectName).SegmentCount;
	int MarkerCount = MyClient.GetMarkerCount( subjectName ).MarkerCount;
	double sum[3] = {0};

	for(int i = MarkerCount; i--;){
		string markerName = MyClient.GetMarkerName( subjectName, i ).MarkerName;
		Output_GetMarkerGlobalTranslation posData = 
				MyClient.GetMarkerGlobalTranslation( subjectName, markerName );
		for(int j = 3; j--; sum[j]+=posData.Translation[j]);
	}

	/*for(unsigned int i = segmentCount; i--;){
		string segmentName = MyClient.GetSegmentName(subjectName, i).SegmentName;

		Output_GetSegmentGlobalTranslation res = MyClient.GetSegmentGlobalTranslation(subjectName, segmentName);
		for(int j = 0; j--; sum[i]+=res.Translation[i]);
	}*/

	// calculate the average position of all the segments
	for(int j = 3; j--; position[j] = sum[j]/MarkerCount);

	return 1;
}

int Update(Player *player, int index, float dt){
	// keep the last position of the player's object
	double lastPos[3]; VecCpy(lastPos, player->Position);

	// vector difference between the last two positions
	GetSegmentPosition(names[index], player->Position);
	double diff[3]; VecSub(diff, player->Position, lastPos);

	// calculate the length of the difference vector
	double len = 0;
	VecLen(len, diff);

	// add the len of the the diff vector (velocity) to
	// the combine speed of the player
	player->CombineSpeed += len;

	player->CombineSpeed *= 0.85;

	if(player->CombineSpeed > player->VelocityTolerance){
		return 1; // the player has lost
	}
	else
		return 0; // the player is still going
}

int kbhit(void){
  struct termios oldt, newt;
  int ch;
  int oldf;
 
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
 
  ch = getchar();
 
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);
 
  if(ch != EOF)
  {
    //ungetc(ch, stdin);
    return 1;
  }
 
  return 0;
}

int main( int argc, char* argv[] ){
	if(argc < 2){
		printf("Please provide the server's host name.\n");
		exit(-1);
	}
	else{
		string hostName = argv[1];
		OPAudio::Init(); // initialize openAL
		Anoun_player[0] = ReadWave("Assets/Sound/anouncer_player1.wav");
		Anoun_player[1] = ReadWave("Assets/Sound/anouncer_player2.wav");
		Anoun_player[2] = ReadWave("Assets/Sound/anouncer_player3.wav");
		Anoun_player[3] = ReadWave("Assets/Sound/anouncer_player4.wav");
		Anoun_count = ReadWave("Assets/Sound/anouncer_countDown.wav");
		Anoun_Eliminated = ReadWave("Assets/Sound/anouncer_playerEliminated.wav");
		Anoun_Joins = ReadWave("Assets/Sound/anouncer_playerJoins.wav");
		Anoun_wins = ReadWave("Assets/Sound/anouncer_playerWins.wav");

		AnounClip1 = new OPSoundEmitter(&Anoun_player[0]);
		AnounClip2 = new OPSoundEmitter(&Anoun_Joins);

		AnounClip1->SetGain(4.0f);
		AnounClip2->SetGain(4.0f);

		//AnounClip1->Play();
		//while(1){
		//	AnounClip1->Update();
		//}
// --- Connect to the system --------------------------------------------------
		for(int i=0; i != 3; ++i){ // repeat to check disconnecting doesn't wreck next connect
	
			// Connect to a server
			cout << "Connecting to " << hostName << "...\n" << flush;
			while( !MyClient.IsConnected().Connected ){
				// Direct connection
				bool ok = ( MyClient.Connect( hostName ).Result == Result::Success );
				if(!ok) cout << "Warning - connect failed..." << endl;

				// wait for a second
				sleep(1);
			}
		}
// --- Enable request only marker data ----------------------------------------
		MyClient.EnableMarkerData();

// --- Set streaming mode, and axis orientation -------------------------------
		MyClient.SetStreamMode( StreamMode::ServerPush );
		
		MyClient.SetAxisMapping(
			Direction::Left, 
			Direction::Up, 
			Direction::Forward
		); // Y-up

// --- Recieve data here ------------------------------------------------------
		unsigned int oldMarkerCount = 0;
		int stateChanges = 0;			

		printf("Waiting for players to join...\n");
		while(1){
			AnounClip1->Update();
			AnounClip2->Update();

			while( MyClient.GetFrame().Result != Result::Success ){
				// Sleep a little so that we don't lumber the CPU with a busy poll
				sleep(1);

				cout << ".";
			}
			int subjects = MyClient.GetSubjectCount().SubjectCount;

			for(int si = subjects; si--;){
				string SubjectName = MyClient.GetSubjectName(si).SubjectName;		

				switch(GameState){
					case STATE_JOINING:
						// if we can see a player then record it, if they
						// disappear in this stage, indicate that too
						for(int i = 4; i--;){
							if(isVisible(names[i])){
								if(!PlayersVisible[i]){
									printf("Player %d is visible\n", i);
									AnounClip1->SetSound(&Anoun_player[i]);
									AnounClip2->SetSound(&Anoun_Joins);

									AnounClip1->Play(); AnounClip2->Play();
								}

								PlayersVisible[i] = true;

							}//else
							//	PlayersVisible[i] = false;
						}

						if(kbhit()){
							// We are ready to move on to the next state

							// save the indices of the players who will be playing
							for(int i = 4; i--;){
								if(PlayersVisible[i]) ActivePlayers.push_back(i);
							}

							// play the anouncer counting down
							AnounClip1->SetSound(&Anoun_count);
							AnounClip1->Play();

							GameState = STATE_GAMESTARTING;
							printf("Game is starting...\n");
						}

						break;
					case STATE_GAMESTARTING:
							for(int i = ActivePlayers.size(); i--;){
								int pi = ActivePlayers[i];
								Update(&Players[pi], pi, 0.0f);
							}

							if(AnounClip1->GetState() == Stopped){
								GameState = STATE_PLAYING;

								// eliminate any built up speed before
								// starting
								for(int i = ActivePlayers.size(); i--;){
									int pi = ActivePlayers[i];
									Players[pi].CombineSpeed = 0;
									Players[pi].VelocityTolerance = MaxVelocity;
								}

								printf("Game is in progress.\n");
							}
						break;
					case STATE_PLAYING:
							for(int i = ActivePlayers.size(); i--;){
								int pi = ActivePlayers[i];
								if(Update(&Players[pi], pi, 0.0f)){
									// player has been eliminated
									ActivePlayers.erase(ActivePlayers.begin() + i);
									AnounClip1->SetSound(&Anoun_player[pi]);
									AnounClip2->SetSound(&Anoun_Eliminated);

									AnounClip1->Play(); AnounClip2->Play();
								}
							}

							printf("Player speeds %.2f  %.2f  %.2f  %.2f\n",
								Players[0].CombineSpeed, Players[1].CombineSpeed,
								Players[2].CombineSpeed, Players[3].CombineSpeed
							);

							if(ActivePlayers.size() <= 1 && AnounClip1->GetState() == Stopped){								
								// anounce who won	
								if(ActivePlayers.size() == 1 && 
								   AnounClip1->GetState() == Stopped &&
								   AnounClip2->GetState() == Stopped){
									int pi = ActivePlayers[0]; // the winner
									AnounClip1->SetSound(&Anoun_player[pi]);
									AnounClip2->SetSound(&Anoun_wins);
									AnounClip1->Play(); AnounClip2->Play();
									printf("Player %d Wins! press any key to replay\n", pi);

									// there can only be one... and the anouncer needs
									// to be finished
									GameState = STATE_GAMEOVER;
								}
							}	
						break;
					case STATE_GAMEOVER:
						//if(AnounClip1->GetState() == Stopped){
							if(kbhit()){
								printf("Restarting the game\n");
								GameState = STATE_JOINING;
								ActivePlayers.clear();
								for(int i = 4; i--; PlayersVisible[i]=false);
							}
						//}
						break;
				}
			}
		}
	}
}
