#include <iostream>
#include <stdexcept>
#include <unistd.h>

#include "Synth.hpp"

int main(int argc, char const *argv[])
{
  try {
    Synth::Synth synth;
    ::pause(); // dirty! subsequent threads will do the work
  }
  catch (const std::exception& e) {
    std::cerr << "Uncaught exception: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}