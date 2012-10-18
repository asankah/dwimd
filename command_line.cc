// See LICENSE file

#include "dwimd.h"
#include "command_line.h"

#include <string.h>
#include <string>

CommandLine::CommandLine(int argc, wchar_t** argv, TestParameters* parameters)
    : argc_(argc),
      current_(0),
      value_ptr_(NULL),
      short_argument_(NULL),
      long_argument_(NULL),
      argv_(argv),
      need_help_(false),
      parameters_(parameters) {}

bool CommandLine::ParseArgument() {
  if (current_ >= argc_)
    return false;

  const wchar_t* arg = argv_[current_];
  value_ptr_ = NULL;
  short_argument_ = NULL;
  long_argument_ = NULL;

  if (arg[0] == L'-') {
    if (arg[1] == L'-') {
      long_argument_ = &arg[2];
      value_ptr_ = wcschr(long_argument_, L'=');
    } else if (arg[1] != L'\0') {
      short_argument_ = &arg[1];
      value_ptr_ = &arg[2];
    } else {
      LOG(ERR) << "Invalid argument: " << arg << std::endl;
      need_help_ = true;
    }
  } else {
    LOG(ERR) << "Invalid argument: " << arg << std::endl;
    need_help_ = true;
  }

  ++current_;
  return !need_help_;
}

const wchar_t* CommandLine::GetValue() {
  if (value_ptr_ == NULL || *value_ptr_ == L'\0') {
    if (current_ >= argc_) {
      LOG(ERR) << "No argument specified for option" << std::endl;
      need_help_ = true;
      return L"";
    }
    return argv_[current_++];
  }

  if (*value_ptr_ == L'=')
    return ++value_ptr_;
  
  LOG(ERR) << "Invalid argument" << std::endl;
  need_help_ = true;
  return L"";
}

bool CommandLine::IsArgument(const wchar_t short_argument,
                             const wchar_t* long_argument) {
  return (short_argument_ != NULL && *short_argument_ == short_argument) ||
      (long_argument_ != NULL && wcsncmp(long_argument_, long_argument,
                                         wcslen(long_argument)) == 0 &&
       (long_argument_[wcslen(long_argument)] == L'\0' ||
        long_argument_[wcslen(long_argument)] == L'='));
}

void CommandLine::ShowHelp() {
  LOG(INFO)
      << "Usage dwimd [-p <path>|-f <filename>] [-u <url>] [-n] [-v]"
      << std::endl
      << "  -p <path>: Specify the full path to the test file" << std::endl
      << "  -f <filename>: Specify the filename for the test file " << std::endl
      << "                (ignored if -p is given)." << std::endl
      << "  -u <url> : Source URL for the test file." << std::endl
      << "  -n       : Don't create a temporary test file." << std::endl
      << "  -v       : Enable verbose logging." << std::endl
      << std::endl
      << "This utility simulates what would happen when you download a file"
      << std::endl
      << "from the internet and submit it to Attachment Execution Services."
      << std::endl
      << "A test file will be created (either at a default location in your"
      << std::endl
      << "downloads folder, or at the location specified by the [-p] option."
      << std::endl
      << "The name of the file can be specified via the [-f] option."
      << std::endl
      << std::endl
      << "The test file is then run through a series of tests assuming it was"
      << std::endl
      << "downloaded from the URL specified by the [-u] option. If a URL isn't"
      << std::endl
      << "specified, a test URL will be used." << std::endl;
}

bool CommandLine::Parse() {
  current_ = 1;
  while (ParseArgument()) {
    if (IsArgument(L'p', L"path"))
      parameters_->path = GetValue();
    else if (IsArgument(L'u', L"url"))
      parameters_->url = GetValue();
    else if (IsArgument(L'v', L"verbose"))
      parameters_->verbose = true;
    else if (IsArgument(L'n', L"no-create"))
      parameters_->create_test_file = false;
    else if (IsArgument(L'q', L"quiet"))
      parameters_->quiet = true;
    else if (IsArgument(L'f', L"filename"))
      parameters_->filename = GetValue();
    else {
      LOG(ERR) << "Invalid argument: " <<
          (short_argument_ ? short_argument_ : long_argument_) << std::endl;
      need_help_ = true;
    }
  }

  if (need_help_) {
    ShowHelp();
  }

  return !need_help_;
}
