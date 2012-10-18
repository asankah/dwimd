// See LICENSE file

#pragma once

#include "test_parameters.h"

class Test {
 public:
  Test(const char* name, const char* description)
      : name_(name),
        description_(description) {}
  virtual ~Test() {}

  const char* GetName() {
    return name_.c_str();
  }

  const char* GetDescription() {
    return description_.c_str();
  }

  virtual bool Run(const TestParameters& parameters) = 0;

  static bool RunTests(TestParameters* parameters);

 private:
  std::string name_;
  std::string description_;
};
