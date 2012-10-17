#include "dwimd.h"

#include "command_line.h"
#include "test.h"

#include <iomanip>
#include <stdarg.h>
#include <strsafe.h>
#include <sstream>

namespace {

LogSeverity g_log_level = INFO;

const int kLogIndent = 2;
int g_log_indent = 0;

std::stringstream g_summary_stream;
std::stringstream g_discard_stream;
}

ScopedLog::ScopedLog() {
  g_log_indent += kLogIndent;
}

ScopedLog::~ScopedLog() {
  g_log_indent -= kLogIndent;
}

ScopedTimerLog::ScopedTimerLog()
    : ticks_at_start_(GetTickCount()) {
}

ScopedTimerLog::~ScopedTimerLog() {
  DWORD ticks_now = GetTickCount();
  LOG(INFO) << "Time elapsed: " << std::dec << (ticks_now - ticks_at_start_)
            << "ms" << std::endl;
}

std::ostream& StreamForSeverity(LogSeverity severity) {
  switch (severity) {
    case SUMMARY:
      return g_summary_stream;

    case ERR:
      return std::cerr;

    case INFO:
      return (g_log_level < INFO) ? g_discard_stream : std::cout;

    case VERBOSE:
      return (g_log_level < VERBOSE) ? g_discard_stream : std::clog;
  }
  return std::clog;
}

std::ostream& LogStream(LogSeverity severity) {
  std::ostream& stream = StreamForSeverity(severity);
  if (g_log_indent > 0) {
    for (int i = 0; i < g_log_indent; i++)
      stream << ' ';
  }
  return stream;
}

std::ostream& operator<< (std::ostream& out, const wchar_t* string) {
  char converted[1024];
  if (WideCharToMultiByte(CP_UTF8, 0, string, -1,
                          converted, sizeof(converted), NULL, NULL)) {
    out << converted;
  }
  return out;
}

std::ostream& operator<< (std::ostream& out, const std::wstring& string) {
  out << string.c_str();
  return out;
}

std::ostream& operator<< (std::ostream& out, const CLSID& clsid) {
  wchar_t* string = NULL;
  if (SUCCEEDED(StringFromCLSID(clsid, &string))) {
    out << string;
    CoTaskMemFree(string);
  }
  return out;
}

const char* ScanResultToString(HRESULT hr) {
  switch(hr) {
    case S_OK:
      return "S_OK. The scan was successful.";
    case S_FALSE:
      return "S_FALSE. The file was found to be infected, but was cleaned.";
    case E_FAIL:
      return "E_FAIL. The file is infected.";
    case E_ACCESSDENIED:
      return "E_ACCESSDENIED. Access was denied.";
    case __HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND):
      return "ERROR_FILE_NOT_FOUND. File not found.";
    case INET_E_SECURITY_PROBLEM:
      return "INET_E_SECURITY_PROBLEM. The download was blocked.";
    default:
      return "The scan failed.";
  }
}

const char* PolicyCheckResultToString(HRESULT hr) {
  switch(hr) {
    case S_OK:
      return "S_OK. Success.";
    case S_FALSE:
      return "S_FALSE. Prompt";
    default:
      return "Attachment disabled.";
  }
}

void Log(LogSeverity severity, const wchar_t* format, ...) {
  wchar_t log_string[1024];

  if (severity > g_log_level)
    return;

  va_list ap;
  va_start(ap, format);
  StringCbVPrintf(log_string, sizeof(log_string), format, ap);
  va_end(ap);

  SYSTEMTIME systime;
  wchar_t log_entry[1200];
  size_t cb = 0;
  DWORD bytes_written = 0;
  GetLocalTime(&systime);
  StringCbPrintf(log_entry, sizeof(log_entry),
                 L"%02d:%02d:%02d.%03d %s",
                 systime.wHour,
                 systime.wMinute,
                 systime.wSecond,
                 systime.wMilliseconds,
                 log_string);
  StringCbLength(log_entry, sizeof(log_entry), &cb);
  std::cout << log_entry << std::endl;
}

int wmain(int argc, wchar_t** argv) {
  TestParameters parameters;
  CommandLine command_line(argc, argv, &parameters);
  if (!command_line.Parse())
    return false;

  if (parameters.verbose)
    g_log_level = VERBOSE;
  if (parameters.quiet)
    g_log_level = ERR;

  HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
  if (FAILED(hr)) {
    std::cerr << "Can't initialize COM" << std::endl;
    return 1;
  }

  Test::RunTests(&parameters);

  CoUninitialize();

  std::string summary(g_summary_stream.str());
  if (summary.empty()) {
    LOG(SUMMARY) << "No failures detected." << std::endl;
  }

  std::cout << std::endl
            << "Summary:" << std::endl
            << g_summary_stream.str() << std::endl;

  return 0;
}
