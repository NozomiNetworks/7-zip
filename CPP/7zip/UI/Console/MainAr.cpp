// MainAr.cpp

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>

#include "StdAfx.h"

#ifdef _WIN32
#include "../../../../C/DllSecur.h"
#endif

#include "../../../Common/MyException.h"
#include "../../../Common/StdOutStream.h"
#include "../../../Common/StdInStream.h"

#include "../../../Windows/ErrorMsg.h"
#include "../../../Windows/NtCheck.h"

#include "../Common/ArchiveCommandLine.h"
#include "../Common/ExitCode.h"

#include "ConsoleClose.h"

using namespace NWindows;

extern
CStdOutStream *g_StdStream;
CStdOutStream *g_StdStream = NULL;
extern
CStdInStream *g_StdInStream;
CStdInStream *g_StdInStream = NULL;
extern
CStdOutStream *g_ErrStream;
CStdOutStream *g_ErrStream = NULL;

extern int Main2(
  #ifndef _WIN32
  int numArgs, char *args[]
  #endif
);

static const char * const kException_CmdLine_Error_Message = "Command Line Error:";
static const char * const kExceptionErrorMessage = "ERROR:";
static const char * const kUserBreakMessage  = "Break signaled";
static const char * const kMemoryExceptionMessage = "ERROR: Can't allocate required memory!";
static const char * const kUnknownExceptionMessage = "Unknown Error";
static const char * const kInternalExceptionMessage = "\n\nInternal Error #";

static void FlushStreams()
{
  if (g_StdStream)
    g_StdStream->Flush();
}

static void PrintError(const char *message)
{
  FlushStreams();
  if (g_ErrStream)
    *g_ErrStream << "\n\n" << message << endl;
}

#if defined(_WIN32) && defined(_UNICODE) && !defined(_WIN64) && !defined(UNDER_CE)
#define NT_CHECK_FAIL_ACTION *g_StdStream << "Unsupported Windows version"; return NExitCode::kFatalError;
#endif

int MainWrapper(int numArgs, char *args[])
{
  int res = 0;

  try
  {
    #ifdef _WIN32
    My_SetDefaultDllDirectories();
    #endif

    res = Main2(
    #ifndef _WIN32
    numArgs, args
    #endif
    );
  }
  catch(const CNewException &)
  {
    PrintError(kMemoryExceptionMessage);
    return (NExitCode::kMemoryError);
  }
  catch(const NConsoleClose::CCtrlBreakException &)
  {
    PrintError(kUserBreakMessage);
    return (NExitCode::kUserBreak);
  }
  catch(const CMessagePathException &e)
  {
    PrintError(kException_CmdLine_Error_Message);
    if (g_ErrStream)
      *g_ErrStream << e << endl;
    return (NExitCode::kUserError);
  }
  catch(const CSystemException &systemError)
  {
    if (systemError.ErrorCode == E_OUTOFMEMORY)
    {
      PrintError(kMemoryExceptionMessage);
      return (NExitCode::kMemoryError);
    }
    if (systemError.ErrorCode == E_ABORT)
    {
      PrintError(kUserBreakMessage);
      return (NExitCode::kUserBreak);
    }
    if (g_ErrStream)
    {
      PrintError("System ERROR:");
      *g_ErrStream << NError::MyFormatMessage(systemError.ErrorCode) << endl;
    }
    return (NExitCode::kFatalError);
  }
  catch(NExitCode::EEnum &exitCode)
  {
    FlushStreams();
    if (g_ErrStream)
      *g_ErrStream << kInternalExceptionMessage << exitCode << endl;
    return (exitCode);
  }
  catch(const UString &s)
  {
    if (g_ErrStream)
    {
      PrintError(kExceptionErrorMessage);
      *g_ErrStream << s << endl;
    }
    return (NExitCode::kFatalError);
  }
  catch(const AString &s)
  {
    if (g_ErrStream)
    {
      PrintError(kExceptionErrorMessage);
      *g_ErrStream << s << endl;
    }
    return (NExitCode::kFatalError);
  }
  catch(const char *s)
  {
    if (g_ErrStream)
    {
      PrintError(kExceptionErrorMessage);
      *g_ErrStream << s << endl;
    }
    return (NExitCode::kFatalError);
  }
  catch(const wchar_t *s)
  {
    if (g_ErrStream)
    {
      PrintError(kExceptionErrorMessage);
      *g_ErrStream << s << endl;
    }
    return (NExitCode::kFatalError);
  }
  catch(int t)
  {
    if (g_ErrStream)
    {
      FlushStreams();
      *g_ErrStream << kInternalExceptionMessage << t << endl;
      return (NExitCode::kFatalError);
    }
  }
  catch(...)
  {
    PrintError(kUnknownExceptionMessage);
    return (NExitCode::kFatalError);
  }

  return res;
}

int MainLoop(int numArgs, char *args[])
{
  if (numArgs == 2)
  {
    if (std::string(args[1]) == "iv")
    {
      std::cout << "0.0.1" << std::endl;
      return 0;
    }
  }
  else if (numArgs == 3)
  {
    if (std::string(args[1]) == "in")
    {
      const std::string lockFilePath = args[2];
      std::string buffer;
      while (std::getline(std::cin, buffer))
      {
        if (buffer == "exit")
        {
          return 0;
        }
        else
        {
          size_t pos = 0;
          std::string arg;
          std::string s(buffer);
          const std::string delimiter = " ";
          std::vector<std::string> parts;
          while ((pos = s.find(delimiter)) != std::string::npos)
          {
              arg = s.substr(0, pos);
              parts.push_back(arg);
              s.erase(0, pos + delimiter.length());
          }
          parts.push_back(s.substr(0, s.size()));
          std::vector<char*> vecArgs;
          char dummy1stArg[] = "7zz";
          vecArgs.push_back(dummy1stArg);
          for (unsigned long ii = 0; ii < parts.size(); ++ii)
          {
            char * partData = const_cast<char*>(parts[ii].data());
            vecArgs.push_back(partData);
          }
          MainWrapper(
            static_cast<int>(parts.size() + 1),
            &vecArgs[0]
          );
          std::ofstream outLock(lockFilePath);
          outLock << "";
          outLock.close();
        }
      }
    }
  }
  return -1;
}

int MY_CDECL main
(
  #ifndef _WIN32
  int numArgs, char *args[]
  #endif
)
{
  g_ErrStream = &g_StdErr;
  g_StdStream = &g_StdOut;
  g_StdInStream = &g_StdIn;

  NT_CHECK

  NConsoleClose::CCtrlHandlerSetter ctrlHandlerSetter;
  
  const int mainLoopRetVal = MainLoop(numArgs, args);
  if (mainLoopRetVal == -1)
  {
    return MainWrapper(numArgs, args);
  }

  return mainLoopRetVal;
}
