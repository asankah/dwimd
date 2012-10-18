// See LICENSE file

#include "dwimd.h"
#include "scanner_test.h"

#include <sstream>
#include <vector>

#define AVVENDOR
#include <InitGuid.h>
#include <msoav.h>
#include <ComCat.h>

namespace {

bool GetScannerCLSIDs(std::vector<CLSID>* clsids) {
  HRESULT hr;

  CComPtr<ICatInformation> category_info;
  hr = ::CoCreateInstance(CLSID_StdComponentCategoriesMgr, NULL,
                          CLSCTX_INPROC_SERVER,
                          __uuidof(ICatInformation), (void**)&category_info);
  if (FAILED(hr)) {
    LOG(ERR) << "Can't instantiate Component Categories Manager. HR="
             << std::hex << hr << std::endl;
    return false;
  }

  CComPtr<IEnumCLSID> enumerator;
  hr = category_info->EnumClassesOfCategories(1, &CATID_MSOfficeAntiVirus,
                                              -1, NULL, &enumerator);
  if (FAILED(hr)) {
    LOG(ERR) << "EnumClassesOfCategories failed. HR="
             << std::hex << hr << std::endl;
    return false;
  }

  ULONG count = 0;
  CLSID clsid;
  ZeroMemory(&clsid, sizeof(clsid));
  clsids->clear();

  do {
    count = 0;
    hr = enumerator->Next(1, &clsid, &count);
    if (SUCCEEDED(hr) && count > 0) {
      VLOG(1) << "Class " << clsid << " implements MSOfficeAntiVirus"
              << std::endl;
      clsids->push_back(clsid);
    }
  } while (SUCCEEDED(hr) && count > 0);

  if (clsids->empty()) {
    LOG(INFO)
        << "No classes found that implements MSOfficeAntiVirus category"
        << std::endl;
    return false;
  }
  return true;
}

void VectorFromString(std::vector<wchar_t>* vector,
                      const std::wstring& string) {
  size_t length = string.length() + 1;
  vector->resize(length);
  memcpy(&vector->front(), string.c_str(), length * sizeof(wchar_t));
}

void DescribeClass(LogSeverity severity, const CLSID& clsid) {
  ScopedLog scope;
  CRegKey regkey;
  std::wstringstream reg_path;
  wchar_t buffer[1024];
  ULONG nchars;
  wchar_t* clsid_string = NULL;

  if (FAILED(StringFromCLSID(clsid, &clsid_string))) {
    LOG(ERR) << "Failed to convert CLSID" << std::endl;
    return;
  }

  reg_path << L"CLSID\\" << clsid_string;
  CoTaskMemFree(clsid_string);
  clsid_string = NULL;

  if (regkey.Open(HKEY_CLASSES_ROOT, reg_path.str().c_str(), KEY_READ)
      != ERROR_SUCCESS) {
    LOG(ERR) << "Failed to open registry path " << reg_path.str()
             << std::endl;
    return;
  }

  nchars = ARRAYSIZE_UNSAFE(buffer);
  if (regkey.QueryStringValue(L"", buffer, &nchars) != ERROR_SUCCESS ||
      nchars >= ARRAYSIZE_UNSAFE(buffer)) {
    LOG(ERR) << "Failed to query registry value at " << reg_path.str()
             << std::endl;
    return;
  }
  buffer[nchars] = L'\0';

  LOG(severity) << "Name: " << buffer << std::endl;

  reg_path << L"\\InProcServer32";
  if (regkey.Open(HKEY_CLASSES_ROOT, reg_path.str().c_str(), KEY_READ)
      != ERROR_SUCCESS) {
    LOG(ERR) << "Failed to open registry path " << reg_path.str()
             << std::endl;
    return;
  }

  nchars = ARRAYSIZE_UNSAFE(buffer);
  if (regkey.QueryStringValue(L"", buffer, &nchars) != ERROR_SUCCESS ||
      nchars >= ARRAYSIZE_UNSAFE(buffer)) {
    LOG(ERR) << "Failed to query registry value at " << reg_path.str()
             << std::endl;
    return;
  }
  buffer[nchars] = L'\0';
  LOG(severity) << "Path: " << buffer << std::endl;
}

void RunTestWithScanner(const CLSID& clsid, const TestParameters& parameters) {
  LOG(INFO) << std::endl;
  LOG(INFO) << "Running test with " << clsid << std::endl;
  ScopedTimerLog scope;

  HRESULT hr;
  CComPtr<IOfficeAntiVirus> anti_virus;

  hr = CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER,
                        IID_IOfficeAntiVirus, (void**) &anti_virus);
  if (FAILED(hr)) {
    LOG(ERR) << "Couldn't instantiate " << clsid << " for IOfficeAntiVirus"
             << std::endl;
    return;
  }

  DescribeClass(INFO, clsid);

  MSOAVINFO av_info;
  ZeroMemory(&av_info, sizeof(av_info));

  std::vector<wchar_t> path_copy;
  VectorFromString(&path_copy, parameters.path);

  std::vector<wchar_t> url_copy;
  VectorFromString(&url_copy, parameters.url);

  std::vector<wchar_t> host_copy;
  VectorFromString(&host_copy, std::wstring(L"TestScanner"));

  av_info.cbsize = sizeof(av_info);
  av_info.fPath = 1;
  av_info.fHttpDownload = 1;
  av_info.hwnd = NULL;
  av_info.u.pwzFullPath = &path_copy.front();
  av_info.pwzHostName = &host_copy.front();
  av_info.pwzOrigURL = &url_copy.front();

  hr = anti_virus->Scan(&av_info);
  LOG(INFO) << "Scanner returns " << std::hex << hr
            << " (" << ScanResultToString(hr) << ")" << std::endl;

  if (FAILED(hr)) {
    LOG(SUMMARY) << "Anti-Virus scanner " << clsid 
                 << " returned an error." << std::endl;
    DescribeClass(SUMMARY, clsid);
    LOG(SUMMARY) << "The error code is " << std::hex << hr
                 << " (" << ScanResultToString(hr) << ")"
                 << std::endl << std::endl;
  }
}

} // namespace

// Run through the list of AV scanners and invoke each one on the file.
bool ScannerTest::Run(const TestParameters& parameters) {
  HRESULT hr;
  std::vector<CLSID> clsids;

  hr = GetScannerCLSIDs(&clsids);
  if (FAILED(hr))
    return false;

  for (size_t i = 0; i < clsids.size(); i++) {
    RunTestWithScanner(clsids[i], parameters);
  }

  return true;
}
