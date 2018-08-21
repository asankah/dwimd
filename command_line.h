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

  int argc_ = 0;
  int current_ = 0;
  const wchar_t* value_ptr_ = nullptr;
  const wchar_t* short_argument_ = nullptr;
  const wchar_t* long_argument_ = nullptr;
  wchar_t** argv_ = nullptr;
  bool need_help_ = false;
  TestParameters* parameters_ = nullptr;
};
    
