#pragma once

#include "test.h"

class PolicyTest: public Test {
 public:
  PolicyTest()
      : Test("policy", "Attachment Policy") {}

  bool Run(const TestParameters& parameters) override;
};
