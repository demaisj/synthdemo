#include <stdexcept>
#include <iostream>
#include <vector>
#include <functional>
#include <RtMidi.h>

namespace Synth {

class Midi {
public:
  using frame_processor_type = std::function<void (const std::vector<unsigned char>&)>;

  Midi()
  {
    try {
      _instance = new RtMidiIn();
    }
    catch (RtMidiError& error) {
      throw std::runtime_error("RtMidiError: " + error.getMessage());
    }

    select_device();
    open_device();
  }

  ~Midi()
  {
    close_device();
    delete _instance;
  }

  void register_processor(frame_processor_type fcn)
  {
    _frame_processor = fcn;
  }

private:
  void select_device()
  {
    // probe
    unsigned int count = _instance->getPortCount();
    if (count == 0)
      throw std::runtime_error("No midi device found");
    std::cout << "=> " << count << " midi devices found:" << std::endl;

    for (unsigned int i = 0; i < count; i++) {
      std::string name;

      try {
        name = _instance->getPortName(i);
      }
      catch (RtMidiError& error) {
        throw std::runtime_error("RtMidiError: " + error.getMessage());
      }

      std::cout << "[" << i + 1 << "] " << name << std::endl;
    }

    // select
    while (_dev < 0 || _dev >= count) {
      std::cout << "Select a midi device: ";
      std::cin >> _dev;
      _dev -= 1;
    }

    std::cout << std::endl;
  }

  void open_device()
  {
    try {
      _instance->openPort(_dev);
      _instance->setCallback(input_callback, this);
    }
    catch (RtMidiError& error) {
      throw std::runtime_error("RtMidiError: " + error.getMessage());
    }
  }

  void close_device()
  {
    try {
      _instance->closePort();
    }
    catch (RtMidiError& error) {
      throw std::runtime_error("RtMidiError: " + error.getMessage());
    }
  }

  void on_input(double time, std::vector<unsigned char>* msg)
  {
    _frame_processor(*msg);
  }

  static void input_callback(double time, std::vector<unsigned char>* msg, void* data)
  {
    static_cast<Midi*>(data)->on_input(time, msg);
  }

  static void error_callback(RtMidiError::Type type, const std::string& error)
  {
    std::cerr << "Midi: Error: " << error << std::endl;
  }

  RtMidiIn* _instance = nullptr;
  unsigned int _dev = -1;
  frame_processor_type _frame_processor;
};

}