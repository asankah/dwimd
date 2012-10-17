#pragma once

#include "test.h"

class ScannerTest: public Test {
 public:
  ScannerTest()
      : Test("scan", "AV Scanners") {}

  bool Run(const TestParameters& parameters) override;
};
