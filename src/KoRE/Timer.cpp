#include "KoRE/Timer.h"

kore::Timer::Timer(void) {
}

kore::Timer::~Timer(void) {
}

void kore::Timer::start(void) {
  _start = clock();
  _last = _start;
}

double kore::Timer::timeSinceLastCall(void) {
  // time prototype
  _current = clock();
  _diff = _current - _last;
  _last = _current;
  return static_cast<double>(_diff *0.001);  // in milliseconds
}
