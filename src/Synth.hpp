#include <vector>
#include <cmath>

#include "Midi.hpp"
#include "Audio.hpp"
#include "Oscillator.hpp"

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

    for (int i = 0; i < 16; ++i) {
      _osc[i].detune = (i - 8) * 0.1;
    }
    _lfo.frequency = 1;
  }

private:
  void midi_processor(const std::vector<unsigned char>& msg)
  {
    // Midi logic here!
    switch (msg[0]) {
      case 0x80: // NOTE_OFF
        _on -= 1;
        break;
      case 0x90: // NOTE_ON
        _on += 1;
        for (int i = 0; i < 16; ++i)
          _osc[i].note(msg[1]);
        break;
    }
  }

  void audio_processor(unsigned int rate, unsigned int chan, double* data)
  {
    // Synth logic here!
    if (!_on)
      return;

    /*for (int i = 0; i < chan; ++i) {
      double val;
      _lfo.process(rate, 1, &val);
      for (int j = 0; j < 16; ++j)
        _osc[j].detune = val * 100;
    }*/

    for (int i = 0; i < 16; ++i) {
      double sub[chan];
      _osc[i].process(rate, chan, sub);
      for (int j = 0; j < chan; ++j) {
        data[j] += sub[j] / 16;
        data[j] *= 1.33;
      }
    }
  }

  Midi _midi;
  Audio _audio;

  unsigned int _on = 0;
  Oscillator _osc[16];
  Oscillator _lfo;
};

}
