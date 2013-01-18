#include "core/operation.h"

kore::Operation::Operation(void)
  : _executed(false),
    _executeOnce(false) {
}

kore::Operation::~Operation(void) {
}

bool kore::Operation::getExecuted(void) {
  return _executed;
}

void kore::Operation::setExecuted(bool flag) {
  _executed = flag;
}