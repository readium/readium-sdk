// Copyright 2007, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "_config.h"

#include <ctime>
#include <iomanip>
#include <cstring>
#if EPUB_OS(ANDROID)
# include <utf8/utf8.h>
# include <sstream>
# include <fstream>
#include REGEX_INCLUDE
#else
# include <codecvt>
#endif
#if _WIN32 || _WIN64
# include <windows.h>
# include <tchar.h>
# define NEWLINE "\r\n"
#else
# include <stdlib.h>
# include <unistd.h>
# include <sys/time.h>
# if __MACH__
#  include <TargetConditionals.h>
#  if TARGET_OS_IPHONE
extern "C" const char* _IOSGetProgname(void);
#  else
#   include <crt_externs.h>
#  endif
#  include <mach/mach_time.h>
# endif
typedef pthread_rwlock_t CRITICAL_SECTION;
# define MAX_PATH PATH_MAX
# define NEWLINE "\n"
#endif
#include <algorithm>
#include "logging.h"

namespace logging {

const char* const log_severity_names[LOG_NUM_SEVERITIES] = {
  "INFO", "WARNING", "ERROR", "FATAL" };

int min_log_level = 0;
LogLockingState lock_log_file = LOCK_LOG_FILE;
LoggingDestination logging_destination = LOG_ONLY_TO_FILE;

const int kMaxFilteredLogLevel = LOG_WARNING;
char* log_filter_prefix = NULL;

// which log file to use? This is initialized by InitLogging or
// will be lazily initialized to the default value when it is
// first needed.
TCHAR log_file_name[MAX_PATH] = { 0 };

// this file is lazily opened and the handle may be NULL
HANDLE log_file = NULL;

// what should be prepended to each message?
bool log_process_id = false;
bool log_thread_id = false;
bool log_timestamp = true;
bool log_tickcount = false;

// An assert handler override specified by the client to be called instead of
// the debug message dialog.
LogAssertHandlerFunction log_assert_handler = NULL;

#if _WIN32 || _WIN64
// The critical section is used if log file locking is false. It helps us
// avoid problems with multiple threads writing to the log file at the same
// time.
bool initialized_critical_section = false;
CRITICAL_SECTION log_critical_section;

// When we don't use a critical section, we are using a global mutex. We
// need to do this because LockFileEx is not thread safe
HANDLE log_mutex = NULL;
#else
pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif
    
#if EPUB_OS(ANDROID)
std::string GetProcessName()
{
    static std::string __procname;
    if ( __procname.empty() )
    {
        std::stringstream ss;
        ss << "/proc/" << getpid() << "/stat";
        std::ifstream reader(ss.str());
        
        std::string contents;
        reader >> contents;
        
        REGEX_NS::regex re(R"X(\((.*?)\))X");
        REGEX_NS::smatch found;
        if ( REGEX_NS::regex_search(contents, found, re) )
        {
            __procname = found[1].str();
        }
        else
        {
            std::stringstream pss;
            pss << "unknown_process_" << getpid();
            __procname = pss.str();
        }
    }
    
    return __procname;
}
#endif

// Called by logging functions to ensure that debug_file is initialized
// and can be used for writing. Returns false if the file could not be
// initialized. debug_file will be NULL in this case.
bool InitializeLogFileHandle() {
  if (log_file)
    return true;
#if _WIN32 || _WIN64
  if (!log_file_name[0]) {
    // nobody has called InitLogging to specify a debug log file, so here we
    // initialize the log file name to the default
    GetModuleFileName(NULL, log_file_name, MAX_PATH);
    TCHAR* last_backslash = _tcsrchr(log_file_name, '\\');
    if (last_backslash)
      last_backslash[1] = 0; // name now ends with the backslash
    _tcscat_s(log_file_name, _T("debug.log"));
  }

  log_file = CreateFile(log_file_name, GENERIC_WRITE,
                        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (log_file == INVALID_HANDLE_VALUE || log_file == NULL) {
    // try the current directory
    log_file = CreateFile(_T(".\\debug.log"), GENERIC_WRITE,
                          FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                          OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (log_file == INVALID_HANDLE_VALUE || log_file == NULL) {
      log_file = NULL;
      return false;
    }
  }
  SetFilePointer(log_file, 0, 0, FILE_END);
#else
    if ( log_file_name[0] == '\0' )
    {
#if __MACH__
# if TARGET_OS_IPHONE
        strlcpy(log_file_name, _IOSGetProgname(), MAX_PATH);
# else
        strlcpy(log_file_name, *_NSGetProgname(), MAX_PATH);
# endif
#elif EPUB_OS(ANDROID)
        strlcpy(log_file_name, GetProcessName().c_str(), MAX_PATH);
#else
        strlcpy(log_file_name, __progname, MAX_PATH);
#endif
        char* last_slash = strrchr(log_file_name, '/');
        if ( last_slash != nullptr )
            last_slash[1] = '\0';
        strlcat(log_file_name, "debug.log", MAX_PATH);
    }
    
    log_file = fopen(log_file_name, "a+");
    if ( log_file == nullptr )
        return false;
#endif
  return true;
}

void InitLogMutex() {
#if _WIN32 || _WIN64
  if (!log_mutex) {
    // \ is not a legal character in mutex names so we replace \ with /
    std::wstring safe_name(log_file_name);
    std::replace(safe_name.begin(), safe_name.end(), '\\', '/');
    std::wstring t(L"Global\\");
    t.append(safe_name);
    log_mutex = ::CreateMutex(NULL, FALSE, t.c_str());
  }
#endif
}

void InitLogging(const TCHAR* new_log_file, LoggingDestination logging_dest,
                 LogLockingState lock_log, OldFileDeletionState delete_old) {
  if (log_file) {
    // calling InitLogging twice or after some log call has already opened the
    // default log file will re-initialize to the new options
#if _WIN32 || _WIN64
    CloseHandle(log_file);
#else
      fclose(log_file);
#endif
    log_file = NULL;
  }

  lock_log_file = lock_log;
  logging_destination = logging_dest;

  // ignore file options if logging is only to system
  if (logging_destination == LOG_ONLY_TO_SYSTEM_DEBUG_LOG)
    return;
#if _WIN32 || _WIN64
  _tcscpy_s(log_file_name, MAX_PATH, new_log_file);
  if (delete_old == DELETE_OLD_LOG_FILE)
    DeleteFile(log_file_name);
#else
    strlcpy(log_file_name, new_log_file, MAX_PATH);
    if ( delete_old == DELETE_OLD_LOG_FILE )
        remove(log_file_name);
#endif

  if (lock_log_file == LOCK_LOG_FILE) {
    InitLogMutex();
  }
#if _WIN32 || _WIN64
  else if (!initialized_critical_section) {
    // initialize the critical section
    InitializeCriticalSection(&log_critical_section);
    initialized_critical_section = true;
  }
#endif

  InitializeLogFileHandle();
}

void SetMinLogLevel(int level) {
  min_log_level = level;
}

void SetLogFilterPrefix(char* filter)  {
  if (log_filter_prefix) {
    delete[] log_filter_prefix;
    log_filter_prefix = NULL;
  }

  if (filter) {
    size_t size = strlen(filter)+1;
    log_filter_prefix = new char[size];
#if _WIN32 || _WIN64
    strcpy_s(log_filter_prefix, size, filter);
#else
      strlcpy(log_filter_prefix, filter, size);
#endif
  }
}

void SetLogItems(bool enable_process_id, bool enable_thread_id,
                 bool enable_timestamp, bool enable_tickcount) {
  log_process_id = enable_process_id;
  log_thread_id = enable_thread_id;
  log_timestamp = enable_timestamp;
  log_tickcount = enable_tickcount;
}

void SetLogAssertHandler(LogAssertHandlerFunction handler) {
  log_assert_handler = handler;
}

// Displays a message box to the user with the error message in it. For
// Windows programs, it's possible that the message loop is messed up on
// a fatal error, and creating a MessageBox will cause that message loop
// to be run. Instead, we try to spawn another process that displays its
// command line. We look for "Debug Message.exe" in the same directory as
// the application. If it exists, we use it, otherwise, we use a regular
// message box.
void DisplayDebugMessage(const std::string& str) {
#if _WIN32 || _WIN64
  if (str.empty())
    return;

  // look for the debug dialog program next to our application
  wchar_t prog_name[MAX_PATH];
  GetModuleFileNameW(NULL, prog_name, MAX_PATH);
  wchar_t* backslash = wcsrchr(prog_name, '\\');
  if (backslash)
    backslash[1] = 0;
  wcscat_s(prog_name, MAX_PATH, L"debug_message.exe");

  // stupid CreateProcess requires a non-const command line and may modify it.
  // We also want to use the wide string
  int charcount = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
  if (!charcount)
    return;
  scoped_array<wchar_t> cmdline(new wchar_t[charcount]);
  if (!MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, cmdline.get(), charcount))
    return;

  STARTUPINFO startup_info;
  memset(&startup_info, 0, sizeof(startup_info));
  startup_info.cb = sizeof(startup_info);

  PROCESS_INFORMATION process_info;
  if (CreateProcessW(prog_name, cmdline.get(), NULL, NULL, false, 0, NULL,
                     NULL, &startup_info, &process_info)) {
    WaitForSingleObject(process_info.hProcess, INFINITE);
    CloseHandle(process_info.hThread);
    CloseHandle(process_info.hProcess);
  } else {
    // debug process broken, let's just do a message box
    MessageBoxW(NULL, cmdline.get(), L"Fatal error", MB_OK | MB_ICONHAND);
  }
#endif
}

LogMessage::LogMessage(const char* file, int line, LogSeverity severity,
                       int ctr)
    : severity_(severity) {
  Init(file, line);
}

LogMessage::LogMessage(const char* file, int line, const CheckOpString& result)
    : severity_(LOG_FATAL) {
  Init(file, line);
  stream_ << "Check failed: " << (*result.str_);
}

LogMessage::LogMessage(const char* file, int line)
     : severity_(LOG_INFO) {
  Init(file, line);
}

LogMessage::LogMessage(const char* file, int line, LogSeverity severity)
    : severity_(severity) {
  Init(file, line);
}
    
#if !defined(_WIN32) && !defined(_WIN64)
# define GetCurrentProcessId() getpid()
# if EPUB_OS(LINUX)
#  define GetCurrentThreadId() static_cast<uintptr_t>(pthread_self())
# else
#  define GetCurrentThreadId() reinterpret_cast<uintptr_t>(pthread_self())
# endif
# if __MACH__
#  define GetTickCount() mach_absolute_time()
# else
inline uint64_t GetTickCount() {
    struct timeval t;
    gettimeofday(&t, nullptr);
    return t.tv_sec*1000000 + t.tv_usec;
}
# endif
#endif

// writes the common header info to the stream
void LogMessage::Init(const char* file, int line) {
  // log only the filename
  const char* last_slash = strrchr(file, '\\');
  if (last_slash)
    file = last_slash + 1;

  stream_ <<  '[';
  if (log_process_id)
    stream_ << GetCurrentProcessId() << ':';
  if (log_thread_id)
    stream_ << GetCurrentThreadId() << ':';
  if (log_timestamp) {
    time_t t = time(NULL);
    struct tm tm_time;
#if _WIN32 || _WIN64
    localtime_s(&tm_time, &t);
#else
      localtime_r(&t, &tm_time);
#endif
    stream_ << std::setfill('0')
            << std::setw(2) << 1 + tm_time.tm_mon
            << std::setw(2) << tm_time.tm_mday
            << '/'
            << std::setw(2) << tm_time.tm_hour
            << std::setw(2) << tm_time.tm_min
            << std::setw(2) << tm_time.tm_sec
            << ':';
  }
  if (log_tickcount)
    stream_ << GetTickCount() << ':';
  stream_ << log_severity_names[severity_] << ":" << file << "(" << line << ")] ";
  
#if _WIN32 || _WIN64
  message_start_ = static_cast<int>(stream_.pcount());
#else
  message_start_ = static_cast<int>(stream_.str().length());
#endif
}

LogMessage::~LogMessage() {
  if (severity_ < min_log_level)
    return;

  std::string str_newline(stream_.str());
  str_newline.append(NEWLINE);

  if (log_filter_prefix && severity_ <= kMaxFilteredLogLevel &&
      str_newline.compare(message_start_, strlen(log_filter_prefix),
                          log_filter_prefix) != 0) {
#if _WIN32 || _WIN64
      goto cleanup;
#else
      return;
#endif
  }

  if (logging_destination != LOG_ONLY_TO_FILE)
  {
#if _WIN32 || _WIN64
    OutputDebugStringA(str_newline.c_str());
#else
      fprintf(stderr, "%s", str_newline.c_str());
#endif
  }

  // write to log file
  if (logging_destination != LOG_ONLY_TO_SYSTEM_DEBUG_LOG &&
      InitializeLogFileHandle()) {
    // we can have multiple threads and/or processes, so try to prevent them from
    // clobbering each other's writes
    if (lock_log_file == LOCK_LOG_FILE) {
      // Ensure that the mutex is initialized in case the client app did not
      // call InitLogging. This is not thread safe. See below
      InitLogMutex();
#if _WIN32 || _WIN64
      DWORD r = ::WaitForSingleObject(log_mutex, INFINITE);
      DCHECK(r != WAIT_ABANDONED);
#else
        pthread_mutex_lock(&log_mutex);
#endif
    }
#if _WIN32 || _WIN64
    else {
      // use the critical section
      if (!initialized_critical_section) {
        // The client app did not call InitLogging, and so the critical section
        // has not been created. We do this on demand, but if two threads try to
        // do this at the same time, there will be a race condition to create
        // the critical section. This is why InitLogging should be called from
        // the main thread at the beginning of execution.
        InitializeCriticalSection(&log_critical_section);
        initialized_critical_section = true;
      }
      EnterCriticalSection(&log_critical_section);
    }
#endif

#if _WIN32 || _WIN64
    SetFilePointer(log_file, 0, 0, SEEK_END);
    DWORD num_written;
    WriteFile(log_file, (void*)str_newline.c_str(), (DWORD)str_newline.length(), &num_written, NULL);

    if (lock_log_file == LOCK_LOG_FILE) {
      ReleaseMutex(log_mutex);
    } else {
      LeaveCriticalSection(&log_critical_section);
    }
#else
      fprintf(log_file, "%s", str_newline.c_str());
#endif
  }

  if (severity_ == LOG_FATAL) {
#if _WIN32 || _WIN64
    // display a message or break into the debugger on a fatal error
    if (::IsDebuggerPresent()) {
      DebugBreak();
    } else {
      if (log_assert_handler) {
        log_assert_handler(std::string(stream_.str(), static_cast<std::string::size_type>(stream_.pcount())));
      } else {
        // don't use the string with the newline, get a fresh version to send to
        // the debug message process
        DisplayDebugMessage(std::string(stream_.str(), static_cast<std::string::size_type>(stream_.pcount())));
        TerminateProcess(GetCurrentProcess(), 1);
      }
    }
#else
# ifndef NDEBUG
      __builtin_trap();
# else
      if ( log_assert_handler != nullptr )
      {
          log_assert_handler(stream_.str());
      }
      else
      {
          abort();
      }
# endif // NDEBUG
#endif
  }

#if _WIN32 || _WIN64
cleanup:
  stream_.freeze(false);
#endif
}

void CloseLogFile() {
  if (!log_file)
    return;

#if _WIN32 || _WIN64
  CloseHandle(log_file);
#else
    fclose(log_file);
#endif
  log_file = NULL;
}

} // namespace logging

std::ostream& operator<<(std::ostream& out, const wchar_t* wstr) {
  if (!wstr || !wstr[0])
    return out;
#if _WIN32 || _WIN64
  // compute the length of the buffer we'll need
  int charcount = WideCharToMultiByte(CP_UTF8, 0, wstr, -1,
                                      NULL, 0, NULL, NULL);
  if (charcount == 0)
    return out;

  // convert
  scoped_array<char> buf(new char[charcount]);
  WideCharToMultiByte(CP_UTF8, 0, wstr, -1, buf.get(), charcount, NULL, NULL);
  return out << buf.get();
#elif EPUB_OS(ANDROID)
    std::string bytes;
    int len = std::char_traits<wchar_t>::length(wstr);
    if ( sizeof(wchar_t) == sizeof(char32_t))
        utf8::utf32to8(wstr, wstr+len, std::back_inserter(bytes));
    else
        utf8::utf16to8(wstr, wstr+len, std::back_inserter(bytes));
    return out << bytes;
#else
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> _cvt;
    return out << _cvt.to_bytes(wstr);
#endif
}
