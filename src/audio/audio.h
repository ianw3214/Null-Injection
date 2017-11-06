#pragma once

#include <string>

#include "QcEngine.h"
#include "SDL_mixer.h"

namespace Audio {
	void playTrack(std::string file, int channel, bool repeat);
	void playTrack(std::string file, int channel, bool repeat, int ms);
	void setVolume(int channel, int volume);
	void mute();
	void unmute();

	static bool globalMute = false;
}