// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit
#define _AFX_NO_MFC_CONTROLS_IN_DIALOGS         // remove support for MFC controls in dialogs

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "afxdb.h"

#include <atlimage.h>
#include <iostream>

#include <cctype>
#include <iomanip>
#include <sstream>
#include <string>

#include <iostream>
#include <conio.h>
#include <map>
#include <unordered_map>
#include <vector>
#include <set>
#include <queue>
#include <mutex>
#include <numeric>
#include <initializer_list>
#include <random>
#include <algorithm>
#include <fstream>
#include <regex>
using namespace std;
#include <chrono>
using namespace std::chrono;
#include "../MGen/dirent.h"
#include <windows.h>
#include <tlhelp32.h>


// TODO: reference additional headers your program requires here
