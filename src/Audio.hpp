#include <functional>
#include <RtAudio.h>

namespace Synth {

class Audio {
public:
  using frame_processor_type = std::function<void (unsigned int, unsigned int, double*)>;

  Audio()
  {
    try {
      _instance = new RtAudio();
    }
    catch (RtAudioError& error) {
      throw std::runtime_error("RtAudioError: " + error.getMessage());
    }

    select_device();
    open_device();
  }

  ~Audio()
  {
    close_device();
    delete _instance;
  }

  static constexpr unsigned int NB_CHANNELS = 2;
  static constexpr unsigned int SAMPLE_RATE = 44100;
  static constexpr RtAudioFormat FORMAT = RTAUDIO_FLOAT64;

  void register_processor(frame_processor_type fcn)
  {
    _frame_processor = fcn;
  }

private:
  void select_device()
  {
    // probe
    unsigned int count = _instance->getDeviceCount();
    if (count == 0)
      throw std::runtime_error("No audio devices found");
    std::cout << "=> " << count << " audio devices found:" << std::endl;

    for (unsigned int i = 0; i < count; i++) {
      RtAudio::DeviceInfo info;

      try {
        info = _instance->getDeviceInfo(i);
      }
      catch (RtAudioError& error) {
        throw std::runtime_error("RtAudioError: " + error.getMessage());
      }

      if ( info.probed == true ) {
        std::cout << "[" << i + 1 << "] " << (info.isDefaultOutput ? "[default] " : "") << info.name << std::endl;
        //_dev = i;
      }
    }

    // select
    while (_dev < 0 || _dev >= count) {
      std::cout << "Select an audio device: ";
      std::cin >> _dev;
      _dev -= 1;
    }

    try {
      _devinfo = _instance->getDeviceInfo(_dev);
    }
    catch (RtAudioError& error) {
      throw std::runtime_error("RtAudioError: " + error.getMessage());
    }

    std::cout << std::endl;
  }

  void open_device()
  {
    RtAudio::StreamParameters params;
    params.deviceId = _dev;
    params.nChannels = NB_CHANNELS;
    unsigned int frames = 256;

    try {
      _instance->openStream(
        &params,         //input params
        nullptr,         //output params
        FORMAT,          //format
        SAMPLE_RATE,     //sample rate
        &frames,         //frames
        output_callback, //callback
        this,            //data
        nullptr,         //options
        error_callback   //error callback
      );
      _instance->startStream();
    }
    catch (RtAudioError& error) {
      throw std::runtime_error("RtAudioError: " + error.getMessage());
    }
  }

  void close_device()
  {
    try {
      _instance->stopStream();
    }
    catch (RtAudioError& error) {
      throw std::runtime_error("RtAudioError: " + error.getMessage());
    }
  }

  int on_output(void* output, unsigned int frames,
                double time, RtAudioStreamStatus status)
  {
    if (status == RTAUDIO_OUTPUT_UNDERFLOW)
      std::cerr << "Audio: Warning: Audio output underflow" << std::endl;

    auto buffer = static_cast<double*>(output);
    for (unsigned int i = 0; i < frames; i++) {
      double data[NB_CHANNELS] = { 0 };
      _frame_processor(SAMPLE_RATE, NB_CHANNELS, data);
      for (unsigned int j = 0; j < NB_CHANNELS; j++) {
        *buffer++ = data[j];
      }
    }

    return 0;
  }

  static int output_callback(void* output, void* input, unsigned int frames,
                             double time, RtAudioStreamStatus status, void* data)
  {
    return static_cast<Audio*>(data)->on_output(output, frames, time, status);
  }

  static void error_callback(RtAudioError::Type type, const std::string& error)
  {
    std::cerr << "Audio: Error: " << error << std::endl;
  }

  RtAudio* _instance = nullptr;
  unsigned int _dev = -1;
  RtAudio::DeviceInfo _devinfo;
  frame_processor_type _frame_processor;
};

}