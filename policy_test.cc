// See LICENSE file

#include "dwimd.h"
#include "policy_test.h"

#include <shlobj.h>
#include <shobjidl.h>

namespace {

static const GUID kClientID  = { /* 8d9d2384-c58d-4b7c-a88d-aa54d498086a */
  0x8d9d2384,
  0xc58d,
  0x4b7c,
  {0xa8, 0x8d, 0xaa, 0x54, 0xd4, 0x98, 0x08, 0x6a}
};

} // namespace

bool PolicyTest::Run(const TestParameters& parameters) {
  HRESULT hr;
  ScopedTimerLog scope;

  LOG(INFO) << "Invoking Attachment Services" << std::endl;

  CComPtr<IAttachmentExecute> attachment_services;
  hr = CoCreateInstance(CLSID_AttachmentServices, NULL,
                        CLSCTX_INPROC_SERVER, IID_IAttachmentExecute,
                        (void**) &attachment_services);
  if (FAILED(hr)) {
    LOG(ERR) << "Can't instantiate Attachment Services class. HR="
             << std::hex << std::endl;
    return false;
  }

  hr = attachment_services->SetClientGuid(kClientID);
  if (FAILED(hr)) {
    LOG(ERR) << "Can't set client GUID. HR=" << std::hex << hr << std::endl;
    return false;
  }

  hr = attachment_services->SetLocalPath(parameters.path.c_str());
  if (FAILED(hr)) {
    LOG(ERR) << "Can't set local path. HR=" << std::hex << hr << std::endl;
    return false;
  }

  hr = attachment_services->SetSource(parameters.url.c_str());
  if (FAILED(hr)) {
    LOG(ERR) << "Can't set source URL. HR=" << std::hex << hr << std::endl;
    return false;
  }

  hr = attachment_services->CheckPolicy();
  LOG(INFO) << "IAttachmentExecute::CheckPolicy() returned code "
            << std::hex << hr << " (" << PolicyCheckResultToString(hr) << ")"
            << std::endl;
  if (FAILED(hr)) {
    LOG(SUMMARY) << "IAttachmentExecute::CheckPolicy() returned code "
                 << std::hex << hr
                 << " (" << PolicyCheckResultToString(hr) << ")" << std::endl;
    if (hr == INET_E_SECURITY_PROBLEM) {
      LOG(SUMMARY) << std::endl <<
          "This error is likely because your security settings are configured "
          "to block downloads from this URL (" << parameters.url << "). "
          "Check your security zone settings."
                   << std::endl << std::endl;
    }
  }

  HRESULT save_hr = attachment_services->Save();
  LOG(INFO) << "IAttachmentExecute::Save() returned code "
            << std::hex << save_hr << " (" << ScanResultToString(save_hr) << ")"
            << std::endl;
  if (FAILED(save_hr)) {
    LOG(SUMMARY) << "IAttachmentExecute::Save() returned code "
                 << std::hex << save_hr
                 << " (" << ScanResultToString(save_hr) << ")"
                 << std::endl;
    if (hr == save_hr && hr == INET_E_SECURITY_PROBLEM) {
      LOG(SUMMARY) << "Since the IAttachmentExecute::CheckPolicy() test also "
                   << "failed, this error is likely due to that failure."
                   << std::endl << std::endl;
    } else if (save_hr == __HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
      LOG(SUMMARY) << "The file at " << parameters.path << " may not exist or may not be accessible." << std::endl;
    } else {
      LOG(SUMMARY) << "Check for errors in the 'AV Scanners' test above."
                   << std::endl << std::endl;
    }
  }

  return SUCCEEDED(hr) && SUCCEEDED(save_hr);
}
