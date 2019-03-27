#include <cmath>

namespace Synth {

class Oscillator {
public:
  void process(unsigned int rate, unsigned int chan, double* data)
  {
    for (unsigned int i = 0; i < chan; ++i) {
      data[i] = 1.0 - (2 * _phase / (2.0 * M_PI));
    }

    _phase += (frequency + detune) * 2.0 * M_PI / rate;
    if (_phase > 2.0 * M_PI)
      _phase -= 2.0 * M_PI;
  }

  double detune = 0;
  double frequency = 0;

  void note(unsigned char key) {
    frequency = pow(2.0, (key - 69.0) / 12.0) * 440;
  }

private:
  double _phase = fmod(rand(), 2.0 * M_PI);
};

}