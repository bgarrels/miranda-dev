#include "Common.h"

INT bpStatus;
//=======================================================
//GetCaps
//=======================================================

INT_PTR GetCaps(WPARAM wParam,LPARAM lParam)
{
	if(wParam==PFLAGNUM_1)
		return 0;
	if(wParam==PFLAGNUM_2)
		return PF2_ONLINE|PF2_SHORTAWAY|PF2_HEAVYDND|PF2_INVISIBLE; // add the possible statuses here.
	if(wParam==PFLAGNUM_3)
		return 0;
	return 0;
}

//=======================================================
//GetName (tray icon)
//=======================================================
INT_PTR GetName(WPARAM wParam,LPARAM lParam)
{
	_tcsncpy((LPWSTR)lParam,_T("Common"),wParam);
	return 0;
}

//=======================================================
//TMLoadIcon
//=======================================================
INT_PTR TMLoadIcon(WPARAM wParam,LPARAM lParam)
{
	UINT id;

	switch(wParam & 0xFFFF)
	{
		case PLI_PROTOCOL: id=IDI_ON; break; // IDI_ON is the main icon for the protocol
		default: return 0;	
	}
	return (int)LoadImage(hinstance, MAKEINTRESOURCE(id), IMAGE_ICON, GetSystemMetrics(wParam&PLIF_SMALL?SM_CXSMICON:SM_CXICON), GetSystemMetrics(wParam&PLIF_SMALL?SM_CYSMICON:SM_CYICON), 0);
}

//=======================================================
//SetStatus
//=======================================================
INT_PTR SetStatus(WPARAM wParam,LPARAM lParam)
{
	INT oldStatus;
	oldStatus = bpStatus;

	if (wParam == ID_STATUS_AWAY)
	{
		// the status has been changed to away (maybe run some more code)
	}
	else if (wParam == ID_STATUS_DND)
	{
		// the status has been changed to dnd (maybe run some more code)
	}
	else if (wParam == ID_STATUS_NA)
	{
		// the status has been changed to na (maybe run some more code)
	}
	else if (wParam == ID_STATUS_OCCUPIED)
	{
		// the status has been changed to occupied (maybe run some more code)
	}
	else if (wParam == ID_STATUS_ONTHEPHONE)
	{
		// the status has been changed to onthephone (maybe run some more code)
	}
	else if (wParam == ID_STATUS_OUTTOLUNCH)
	{
		// the status has been changed to outtolunch (maybe run some more code)
	}
	//broadcast the message
	bpStatus = wParam;
	ProtoBroadcastAck(protocolname,NULL,ACKTYPE_STATUS,ACKRESULT_SUCCESS,(HANDLE)oldStatus,wParam);
	
	return 0;
}

//=======================================================
//GetStatus
//=======================================================
INT_PTR GetStatus(WPARAM wParam,LPARAM lParam)
{
	if (bpStatus == ID_STATUS_AWAY)
		return ID_STATUS_AWAY;
	else if (bpStatus == ID_STATUS_DND)
		return ID_STATUS_DND;
	else if (bpStatus == ID_STATUS_NA)
		return ID_STATUS_NA;
	else if (bpStatus == ID_STATUS_OCCUPIED)
		return ID_STATUS_OCCUPIED;
	else if (bpStatus == ID_STATUS_ONTHEPHONE)
		return ID_STATUS_ONTHEPHONE;
	else if (bpStatus == ID_STATUS_OUTTOLUNCH)
		return ID_STATUS_OUTTOLUNCH;
	else
		return ID_STATUS_OFFLINE;
}