#include "commonheaders.h"


// ��������� ��������� szMsg �� ����� ������ iLen, ���������� ������ ���� PARTzPARTzz
LPSTR splitMsg(LPSTR szMsg, int iLen) {

#if defined(_DEBUG) || defined(NETLIB_LOG)
	Sent_NetLog("split: msg: -----\n%s\n-----\n",szMsg);
#endif
	int len = (int)strlen(szMsg);
	LPSTR out = (LPSTR) mir_alloc(len*2);
	LPSTR buf = out;

	WORD msg_id = DBGetContactSettingWord(0, szModuleName, "msgid", 0) + 1;
	DBWriteContactSettingWord(0, szModuleName, "msgid", msg_id);

	int part_all = (len+iLen-1)/iLen;
	for(int part_num=0; part_num<part_all; part_num++) {
		int sz = (len>iLen)?iLen:len;
		mir_snprintf(buf,32,"%s%04X%02X%02X",SIG_SECP,msg_id,part_num,part_all);
		memcpy(buf+LEN_SECP+8,szMsg,sz);
		*(buf+LEN_SECP+8+sz) = '\0';
#if defined(_DEBUG) || defined(NETLIB_LOG)
		Sent_NetLog("split: part: %s",buf);
#endif
		buf += LEN_SECP+8+sz+1;
		szMsg += sz;
		len -= sz;
	}
	*buf = '\0';
	return out;
}


// �������� ��������� �� ������, ����� ������ � ��������� � ��������
LPSTR combineMessage(pUinKey ptr, LPSTR szMsg) {

#if defined(_DEBUG) || defined(NETLIB_LOG)
	Sent_NetLog("combine: part: %s",szMsg);
#endif
	int msg_id,part_num,part_all;
	sscanf(szMsg,"%4X%2X%2X",&msg_id,&part_num,&part_all);
	//
	pPM ppm = NULL, pm = ptr->msgPart;
	if( !ptr->msgPart ) {
		pm = ptr->msgPart = new partitionMessage;
		memset(pm,0,sizeof(partitionMessage));
		pm->id = msg_id;
		pm->message = new LPSTR[part_all];
		memset(pm->message,0,sizeof(LPSTR)*part_all);
	}
	else
	while(pm) {
		if( pm->id == msg_id ) break;
		ppm = pm; pm = pm->nextMessage;
	}
	if(!pm) { // nothing to found
		pm = ppm->nextMessage = new partitionMessage;
		memset(pm,0,sizeof(partitionMessage));
		pm->id = msg_id;
		pm->message = new LPSTR[part_all];
		memset(pm->message,0,sizeof(LPSTR)*part_all);
	}
	pm->message[part_num] = new char[strlen(szMsg)];
	strcpy(pm->message[part_num],szMsg+8);
#if defined(_DEBUG) || defined(NETLIB_LOG)
	Sent_NetLog("combine: save part: %s",pm->message[part_num]);
#endif
	int len=0,i;
	for( i=0; i<part_all; i++ ){
		if(pm->message[i]==NULL) break;
		len+=(int)strlen(pm->message[i]);
	}
	if( i==part_all ) { // combine message
       		SAFE_FREE(ptr->tmp);
		ptr->tmp = (LPSTR) mir_alloc(len+1); *(ptr->tmp)='\0';
		for( i=0; i<part_all; i++ ){
			strcat(ptr->tmp,pm->message[i]);
			delete pm->message[i];
		}
		delete pm->message;
		if(ppm) ppm->nextMessage = pm->nextMessage;
		else 	ptr->msgPart = pm->nextMessage;
		delete pm;
#if defined(_DEBUG) || defined(NETLIB_LOG)
		Sent_NetLog("combine: all parts: -----\n%s\n-----\n", ptr->tmp);
#endif
		// ������� ���� ���������
		return ptr->tmp;
	}
#if defined(_DEBUG) || defined(NETLIB_LOG)
	Sent_NetLog("combine: not all parts");
#endif
	// ��� �� �������
	return NULL;
}


// ���������� ���������, ���� ���� �� ��������� �� �����
int splitMessageSend(pUinKey ptr, LPSTR szMsg) {

	int ret;
	int len = (int)strlen(szMsg);
	int par = (getContactStatus(ptr->hContact)==ID_STATUS_OFFLINE)?ptr->proto->split_off:ptr->proto->split_on;
	if( par && len>par ) {
		LPSTR msg = splitMsg(szMsg,par);
		LPSTR buf = msg;
		while( *buf ) {
			len = (int)strlen(buf);
			LPSTR tmp = mir_strdup(buf);
			ret = CallContactService(ptr->hContact,PSS_MESSAGE,(WPARAM)PREF_METANODB,(LPARAM)tmp);
			mir_free(tmp);
			buf += len+1;
		}
		SAFE_FREE(msg);
	}
	else {
		ret = CallContactService(ptr->hContact,PSS_MESSAGE,(WPARAM)PREF_METANODB,(LPARAM)szMsg);
	}
	return ret;
}


// EOF
