#ifndef GPG_WRAPPER_H
#define GPG_WRAPPER_H
typedef enum {
	pxSuccess,
	pxSuccessExitCodeInvalid,
	pxCreatePipeFailed,
	pxDuplicateHandleFailed,
	pxCloseHandleFailed,
	pxCreateProcessFailed,
	pxThreadWaitFailed,
	pxReadFileFailed,
	pxBufferOverflow,
	pxNotFound,
	pxNotConfigured
}
pxResult;

pxResult pxExecute(wstring *acommandline, char *ainput, string *aoutput, LPDWORD aexitcode, pxResult *result);

struct gpg_execution_params
{
	wstring *cmd;
	char *useless;
	string *out;
	LPDWORD code;
	pxResult *result;
	HANDLE hProcess;
	PROCESS_INFORMATION *proc;
};

void pxEexcute_thread(void *param);

#endif