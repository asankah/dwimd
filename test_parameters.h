#pragma once

#include <string>

struct TestParameters {
  TestParameters()
      : verbose(false),
        quiet(false),
        create_test_file(true) {}

  std::wstring path;
  std::wstring url;
  std::wstring filename;
  bool verbose;
  bool quiet;
  bool create_test_file;
};
