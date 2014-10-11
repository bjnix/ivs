#pragma once
#ifdef WINDOWS
#include "../include/al.h"
#include "../include/alc.h"
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif
#include "OPaudio.h"

#define CHUNKS 10
#define BUFFERS 5

enum OPsoundEmitterState{
	Playing = 0,
	Stopped = 1,
	Paused = 2
};

class OPSoundEmitter{
	public:
		bool Looping;
		bool Processed;

		OPSoundEmitter(OPsound* sound, int bytesPerBuffer){
			_sound = sound;
			Processed = true;

			_intermediateBuffer = new unsigned char[_bufferSize = bytesPerBuffer];
			for(int i = bytesPerBuffer; i--; _intermediateBuffer[i] = 0);

			alGenBuffers(BUFFERS, _buffers); // backbuffer and forebuffer
			alGenSources(1, &_alSrc);
			alSourcei(_alSrc, AL_LOOPING, AL_FALSE);

			// fill all buffers with silence
			for(int i = BUFFERS; i--;)
			alBufferData(
				_buffers[i],
				_sound->Format,
				_intermediateBuffer,
				bytesPerBuffer,
				_sound->SampleRate
			);

			Looping = false;
			_activeBuffer = 0; // start with the forebuffer
			_oldBuffsPlayed = _queued = 0;
			_bytesPlayed = _oldBytesPlayed = 0; // bytes played
			_freeBuffers = BUFFERS;

			_chunkSize = bytesPerBuffer / CHUNKS;
			_chunksProcessed = 0;
			_bytesInBuffer = 0;
		}

		OPSoundEmitter(OPsound* sound){
			_sound = sound;

			alGenBuffers(1, &_buffers[0]); // backbuffer and forebuffer
			alGenSources(1, &_alSrc);
			alSourcei(_alSrc, AL_LOOPING, AL_FALSE);

			Looping = Processed = false;

			alBufferData(
				_buffers[0],
				_sound->Format,
				_sound->Data,
				_sound->DataSize,
				_sound->SampleRate
			);

			alSourcei(_alSrc, AL_BUFFER, _buffers[0]);
		}
		 
		~OPSoundEmitter(){
			alDeleteBuffers(BUFFERS, _buffers);
			alDeleteSources(1, &_alSrc);
		}

		long Length(){ return _sound->DataSize; }

		void Play(){
			while(Processed && _freeBuffers && process());

			//alSourcei(_alSrc, AL_BUFFER, _buffers[_activeBuffer++]);
			alSourcePlay(_alSrc);
			_state = Playing;
		}

		void Pause(){
			alSourcePause(_alSrc);
			_state = Paused;
		}

		void Stop(){
			alSourceStop(_alSrc);
			_state = Stopped;
		}

		void Update(){
			if(!Processed) return;
			if(_state == Playing){
				ALint buffsPlayed = 0, buffsQueued = 0, state = 0, playPos = 0;

				alGetSourcei(_alSrc, AL_BUFFERS_PROCESSED, &buffsPlayed);
				alGetSourcei(_alSrc, AL_BUFFERS_QUEUED, &buffsQueued);
				alGetSourcei(_alSrc, AL_BYTE_OFFSET, &playPos);

				if(buffsPlayed != _oldBuffsPlayed){

					if(buffsQueued){
						ALuint playedBuff = 0;
						alSourceUnqueueBuffers(_alSrc, buffsPlayed, &playedBuff);
						_bytesPlayed += _bufferSize;
						_freeBuffers += buffsPlayed;
					}
					_oldBuffsPlayed = buffsPlayed;
				}

				if(_queued + playPos >= _sound->DataSize){
					printf("Played: %d\n", _bytesPlayed);
					if(Looping){
						_queued = _bytesPlayed = _bytesInBuffer = _chunksProcessed = 0;
						_activeBuffer = 0;
						_freeBuffers = BUFFERS;
					}
					else{
						if(_sound->FillCallback){
							if(_sound->FillCallback(_sound, 0, 0) > 0){
								_queued = _bytesPlayed = _bytesInBuffer = _chunksProcessed = 0;
								_activeBuffer = 0;
							}
							else
								Stop();

						}
						else
							Stop();
					}
				}

				// if there are avalible buffers, fill them with data
				if(_freeBuffers) process();

				// can we continue playing after we've run out of data?
				alGetSourcei(_alSrc, AL_SOURCE_STATE, &state);
				if(state == AL_STOPPED && _freeBuffers != BUFFERS){
					alSourcePlay(_alSrc);
				}
			}
		}

#pragma region Setters
		void SetSound(OPsound* sound){
			_sound = sound;

			if(Processed){
				for(int i = BUFFERS; i--;)
				alBufferData(
					_buffers[i],
					_sound->Format,
					_intermediateBuffer,
					_bufferSize,
					_sound->SampleRate
				);
			}
			else{
				alDeleteBuffers(1, &_buffers[0]);
				alGenBuffers(1, &_buffers[0]);
				alBufferData(
					_buffers[0],
					_sound->Format,
					sound->Data,
					_sound->DataSize,
					_sound->SampleRate
				);
				alSourcei(_alSrc, AL_BUFFER, _buffers[0]);
			}
		}

		void SetPosition(ALfloat position[]){
			alSourcefv(_alSrc, AL_POSITION, position);
		}

		void SetVelocity(ALfloat velocity[]){
			alSourcefv(_alSrc, AL_VELOCITY, velocity);
		}

		void SetGain(float gain){
			alSourcef(_alSrc, AL_GAIN, gain);
		}

		void SetPitch(float pitch){
			alSourcef(_alSrc, AL_PITCH, pitch);
		}

		OPsoundEmitterState GetState(){
			if(!Processed){
				ALint state;
				alGetSourcei(_alSrc, AL_SOURCE_STATE, &state);

				switch(state){
					case AL_STOPPED:
						return Stopped;
					case AL_PLAYING:
						return Playing;
					default:
						return Stopped;
				}
			}

			return _state; 
		}
#pragma endregion

	private:
		OPsound* _sound;
		ALuint _buffers[BUFFERS], _alSrc;
		unsigned char* _intermediateBuffer;

		int _activeBuffer, _freeBuffers;
		int _queued, _chunksProcessed, _oldBuffsPlayed;
		int _bufferSize, _chunkSize;
		int _bytesInBuffer, _bytesPlayed, _oldBytesPlayed;
		OPsoundEmitterState _state;

		int process(){
				int toProcess = _sound->DataSize - (_queued + _bytesInBuffer); // # of bytes that need to be queued
				int offset = _chunksProcessed++ * _chunkSize;                  // offset in bytes for current chunk
				toProcess = toProcess > _chunkSize ? _chunkSize : toProcess;   // don't process more than a chunk's worth

				if(!toProcess) return 0; // no more data! we are done

				// this is where processing would happen, for now just simply copy
				for(int i = toProcess; i--;){
					_intermediateBuffer[offset + i] = (_sound->Data + _queued)[offset + i];
				}


				_bytesInBuffer += toProcess;

				// have all the chunks of this buffer been processed?
				if(_chunksProcessed == CHUNKS){
					// push processed data to the buffer
					alBufferData(
						_buffers[_activeBuffer],
						_sound->Format,
						_intermediateBuffer,
						_bytesInBuffer,
						_sound->SampleRate
					);

					
					alSourceQueueBuffers(_alSrc, 1, &_buffers[_activeBuffer]); // queue this buffer to be played
					++_activeBuffer %= BUFFERS;                                // swap to the next buffer


					_queued += _bytesInBuffer;             // increment the total number of bytes that have been queued
					_bytesInBuffer = _chunksProcessed = 0; // clear for next buffer
					_freeBuffers--;                        // indicate that one less buffer is free
				}

				return toProcess;
		}

};
