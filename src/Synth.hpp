#include <vector>
#include <cmath>

#include "Midi.hpp"
#include "Audio.hpp"

namespace Synth {

class Synth {
public:
  Synth()
  {
    _midi.register_processor([this](const std::vector<unsigned char>& msg){
      midi_processor(msg);
    });
    _audio.register_processor([this](unsigned int rate, unsigned int chan, double* data){
      audio_processor(rate, chan, data);
    });
  }

private:
  void midi_processor(const std::vector<unsigned char>& msg)
  {
    // Midi logic here!
  }

  void audio_processor(unsigned int rate, unsigned int chan, double* data)
  {
    // Synth logic here!
  }

  Midi _midi;
  Audio _audio;
};

}
