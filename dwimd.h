// See LICENSE file

#pragma once
#include <windows.h>

#include <iostream>

#include <atlbase.h>
#include <atlcom.h>

enum LogSeverity {
  SUMMARY,
  ERR,
  INFO,
  VERBOSE
};

#define LOG(S) LogStream(S)
#define VLOG(L) LogStream(VERBOSE)

class ScopedLog {
 public:
  ScopedLog();
  ~ScopedLog();
 private:
  int i;
};

class ScopedTimerLog : public ScopedLog {
 public:
  ScopedTimerLog();
  ~ScopedTimerLog();
 private:
  DWORD ticks_at_start_;
};

void Log(LogSeverity severity, const wchar_t* format, ...);
std::ostream& LogStream(LogSeverity severity);
std::ostream& operator<< (std::ostream& out, const CLSID& clsid);
std::ostream& operator<< (std::ostream& out, const wchar_t* string);
std::ostream& operator<< (std::ostream& out, const std::wstring& string);

const char* ScanResultToString(HRESULT hr);
const char* PolicyCheckResultToString(HRESULT hr);

#define ARRAYSIZE_UNSAFE(x) (sizeof(x)/sizeof(x[0]))
