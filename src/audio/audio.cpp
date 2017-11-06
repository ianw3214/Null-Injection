#include "audio.h"

namespace Audio {

	void playTrack(std::string file, int channel, bool repeat) {
		Mix_Chunk * temp = Mix_LoadWAV(file.c_str());
		if (Mix_PlayChannel(channel, temp, repeat ? -1 : 0) == -1) {
			ERR("Couldn't play track: " << file);
			LOG(Mix_GetError());
		}
	}

	void playTrack(std::string file, int channel, bool repeat, int ms) {
		Mix_Chunk * temp = Mix_LoadWAV(file.c_str());
		if (Mix_FadeInChannel(channel, temp, repeat ? -1 : 0, ms) == -1) {
			ERR("Couldn't play track: " << file);
			LOG(Mix_GetError());
		}
	}

	void setVolume(int channel, int volume) {
		if (globalMute) return;
		Mix_Volume(channel, volume);
	}

	void mute() {
		globalMute = true;
		Mix_Volume(-1, 0);
	}

	void unmute() {
		globalMute = false;
		Mix_Volume(-1, MIX_MAX_VOLUME);
	}

}