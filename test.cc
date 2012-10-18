// See LICENSE file

#include "test.h"
#include "dwimd.h"
#include "scanner_test.h"
#include "policy_test.h"

#include <vector>
#include <shlobj.h>
#include <knownfolders.h>

namespace {

bool InitDefaultParameters(TestParameters* parameters) {
  if (parameters->url.empty())
    parameters->url = L"http://www.example.com/download";

  if (parameters->path.empty()) {
    wchar_t* downloads_folder = NULL;
    HRESULT hr = SHGetKnownFolderPath(FOLDERID_Downloads,
                                      0,
                                      NULL,
                                      &downloads_folder);
    if (FAILED(hr) || downloads_folder == NULL) {
      LOG(ERR) << "Can't determine downloads folder. Code=" << std::hex
               << hr << std::endl;
      return false;
    }

    LOG(VERBOSE) << "Downloads folder is " << downloads_folder << std::endl;
    std::wstring path(downloads_folder);
    path.append(L"\\");
    if (parameters->filename.empty()) {
      LOG(VERBOSE) << "Using default filename." << std::endl;
      path.append(L"test_download_389238.zip");
    } else {
      LOG(VERBOSE) << "Using filename " << parameters->filename << std::endl;
      path.append(parameters->filename);
    }
    parameters->path = path;
    CoTaskMemFree(downloads_folder);
  }

  return true;
}

bool CreateTestFiles(const TestParameters& parameters) {
  const char kContents[] = "Hello world!";

  if (!parameters.create_test_file)
    return true;

  if (parameters.path.empty())
    return false;

  LOG(VERBOSE) << "Creating test file at " << parameters.path << std::endl;

  HANDLE file = CreateFile(parameters.path.c_str(),
                           GENERIC_WRITE,
                           0,
                           NULL,
                           CREATE_NEW,
                           0,
                           NULL);
  if (file == INVALID_HANDLE_VALUE) {
    LOG(ERR) << "Can't create test file " << parameters.path
             << " Last error " << GetLastError() << std::endl;
    return false;
  }

  DWORD bytes_written = 0;
  if (!WriteFile(file, kContents, sizeof(kContents) - 1,
                 &bytes_written, NULL)) {
    LOG(ERR) << "Can't write to test file. Last error " << GetLastError()
             << std::endl;
    CloseHandle(file);
    return false;
  }

  CloseHandle(file);
  return true;
}

bool CleanUpTestFiles(const TestParameters& parameters) {
  if (!parameters.create_test_file)
    return true;

  if (!DeleteFile(parameters.path.c_str())) {
    LOG(ERR) << "Failed to delete test file at " << parameters.path
             << " Last error " << GetLastError() << std::endl;
    return false;
  }

  return true;
}

} // namespace

// static
bool Test::RunTests(TestParameters* parameters) {

  std::vector<Test*> tests;

  tests.push_back(new ScannerTest());
  tests.push_back(new PolicyTest());

  if (!InitDefaultParameters(parameters)) {
    LOG(SUMMARY) << "Couldn't initialize tests." << std::endl;
    return false;
  }

  if (!CreateTestFiles(*parameters)) {
    LOG(SUMMARY) << "Couldn't create test file." << std::endl;
    return false;
  }

  LOG(INFO) << "Test parameters:" << std::endl
            << "  path = " << parameters->path << std::endl
            << "  url  = " << parameters->url << std::endl;

  bool has_failures = false;
  for (unsigned i = 0; i < tests.size(); i++) {
    Test* test = tests[i];
    LOG(INFO) << std::endl;
    LOG(INFO) <<"Running test: " << test->GetDescription() << std::endl;
    ScopedLog log_scope;
    bool result = test->Run(*parameters);
    LOG(INFO) << "Done" << std::endl;
    if (!result) has_failures = true;
  }

  CleanUpTestFiles(*parameters);

  return has_failures;
}
