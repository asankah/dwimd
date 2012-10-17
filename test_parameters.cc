#include "test_parameters.h"

#include <string>

TestParameters::TestParameters() {
}

TestParameters::TestParameters(int argc, wchar_t** argv) {
  for (int i=0; i < argc; ++i) {
    if (wcsncmp(argv[i], L"-p=", 3) == 0) {
    } else if (wcsncmp(argv[i], L"-u=", 3) == 0) {
    }
  }
}
