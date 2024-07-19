//////////////////////////////////////////////////////////////////////////
// Module:CmnHdr.h
// Notice:Copyright (c)2013 jialu
// Purpose:Common header file containing handy macros and definitions
//		   used throughout all the applications.
//////////////////////////////////////////////////////////////////////////



#ifndef _CMNHDR_H_
#define _CMNHDR_H_

/////////////////////////// Unicode Build Option //////////////////////////

// Always compiler using Unicode.
#ifndef	UNICODE
#define UNICODE
#endif

// When using Unicode Windows functions, using Unicode C-Runtime function too.
#ifdef UNICODE
#ifndef _UNICODE
#define _UNICODE
#endif
#endif

///////////////////////// Include Windows Definitions /////////////////////

#pragma warning(push,3)
//#include <Windows.h>
#pragma warning(pop)
#pragma warning(push,4)

//#include <CommCtrl.h>
//#include <process.h>	// For _beginthreadex

#include <limits.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
//#include <crtdbg.h>

//#include <afxmt.h>
//#include <afxwin.h>

//////////// Verify that proper header files are being used ///////////////

//#ifndef FILE_SKIP_COMPLETION_PORT_ON_SUCCESS
//#pragma message("You are not using the latest Platform SDK header/library.")
//#pragma message("files. This may prevent the project from building correctly.")
//#endif


/////////// Allow code to compile cleanly at warning level 4 //////////////

// nonstandard ectension 'single line comment' was used
#pragma warning(disable:4001)

// unreferenced formal parameter
#pragma warning(disable:4100)

// Notice: Creating precompiled header
#pragma warning(disable:4699)

// function not inlined
#pragma warning(disable:4710)

//conversion from 'LONGLONG' to 'ULONGLONG',signed/usigned mismatch
#pragma warning(disable:4245)

// 'type cast': conversion from 'LONG' to 'HINSTANCE' of greater size
#pragma warning(disable:4312)


///////////////////////// Pragma message helper macro /////////////////////

// #pragma jiaMSG(Fix this later)
//#define jiaSTR2(x)	 #x
//#define jiaSTR(x)	 jiaSTR2(x)
//#define jiaMSG(desc) message(__FILE__ "(" jiaSTR(__LINE__) "):" #desc)
//

/////////////////// DebugBreak Improvement for X86 platform ///////////////

//#ifdef _X86_
//#define DebugBreak()	_asm{ int 3 }
//#endif


//////////////////////////////////////////////////////////////////////////

#define	MAX(a,b)	a > b ? a:b
#define	MIN(a,b)	a < b ? a:b


//////////////////////// Quick MessageBox Macro ///////////////////////////

//inline void QuickMsgBox(PCSTR szMsg)
//{
//	char szTitle[MAX_PATH];
//	GetModuleFileNameA(NULL,szTitle,_countof(szTitle));
//	MessageBoxA(GetActiveWindow(),szMsg,szTitle,MB_OK);
//}


/////////////////////////// Assert Macro //////////////////////////////////

// Put up an assertion failure message box
//inline void AssertFall(LPCSTR file, int line, PCSTR expr)
//{
//	char sz[2*MAX_PATH];
//	wsprintfA(sz,"File %s,lines %d: %s", file, line, expr);
//	QuickMsgBox(sz);
//	DebugBreak();
//}

// Put up a message box if an assertion fails in a debug build
//#ifdef _DEBUG
//#define jiaAssert(x) if (!(x)) AssertFall(__FILE__,__LINE__,#x)
//#else
//#define jiaAssert(x)
//#endif //_DEBUG


///////////////////////////// Trace Macro /////////////////////////////////

// Using the trace macros only in the Debug version
//#ifdef _DEBUG
//#define	OutputTraceString(x)	OutputDebugString(x)
//#else
//#define	OutputTraceString(x)	((void)0)
//#endif	//_DEBUG



#endif //#ifndef _CMNHDR_H_

//////////////////////////// End of File /////////////////////////////////
