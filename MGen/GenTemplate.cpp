#include "stdafx.h"
#include "GenTemplate.h"


CGenTemplate::CGenTemplate()
{
}


CGenTemplate::~CGenTemplate()
{
	TRACE("CGenTemplate destructor called");
}

void CGenTemplate::Generate()
{
	TRACE("CGenTemplate.Generate called");
}

UINT CGenTemplate::GenThread(LPVOID pParam)
{
	CGenTemplate* pGen = (CGenTemplate*)pParam;

	if (pGen == NULL) return 1;   // if Object is not valid  

	pGen->Generate();
	Sleep(4000);
	CString * message = new CString("Connected");
	//PostMessage(CGenTemplate::m_hWnd, CGenTemplate::WM_GEN_FINISH, 0, (LPARAM)message);

	return 0;   // thread completed successfully 
}
