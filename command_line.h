// See LICENSE file

#pragma once

#include "test_parameters.h"

class CommandLine {
 public:
  CommandLine(int argc, wchar_t** argv, TestParameters* parameters);
  bool Parse();

 private:
  bool ParseArgument();
  const wchar_t* GetValue();
  bool IsArgument(const wchar_t short_argument,
                  const wchar_t* long_argument);
  void ShowHelp();

  int argc_;
  int current_;
  const wchar_t* value_ptr_;
  const wchar_t* short_argument_;
  const wchar_t* long_argument_;
  wchar_t** argv_;
  bool need_help_;
  TestParameters* parameters_;
};
    
