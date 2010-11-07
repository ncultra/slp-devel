
/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/* IBM CONFIDENTIAL                                                       */
/*                                                                        */
/* OBJECT CODE ONLY SOURCE MATERIALS                                      */
/*                                                                        */ 
/* SLP for Server Management                                              */
/*                                                                        */
/*                                                                        */
/* (C) COPYRIGHT International Business Machines Corp. 2000,2005          */
/* All Rights Reserved                                                    */
/*                                                                        */
/* The source code for this program is not published or otherwise         */
/* divested of its trade secrets, irrespective of what has been           */
/* deposited with the U.S. Copyright Office.                              */
/*                                                                        */
/* IBM_PROLOG_END_TAG                                                     */





#include "lslp-common-defs.h"

extern uint32 dieNow ;
extern BOOL is_nt_service ;
extern int8 lslp_logPath[];
extern LSLP_CONFIG_ADV lslpConfigTransport; /* transport configuration */
extern LSLP_CONFIG_ADV lslpConfigListen;
extern LSLP_CONFIG_ADV lslpConfigDontListen;


void WindowsStartNetwork(void)
{
	WSADATA wsData;
	WSAStartup (  0x0002,   &wsData );
}	

/* int gethostbyname_r(const char *name,  */
/* 		    struct hostent *resultbuf,  */
/* 		    char *buf,  */
/* 		    size_t bufsize,  */
/* 		    struct hostent **result,  */
/* 		    int *errnop)  */
/* { */
/*   name = name; */
/*   resultbuf = resultbuf; */
/*   buf = buf; */
/*   bufsize = bufsize; */
  
/*   if(NULL == (*result = gethostbyname(name))) { */
/*     *errnop = WSAGetLastError(); */
/*     return(-1); */
/*   }  */
/*   return(0); */
/* } */




void WinTestMacros(void)
{
	uint32 h;
	int32 ccode;
	_LSLP_CREATE_SEM((void *)h, 0);
	_LSLP_WAIT_SEM(h, 10, &ccode);	   /* should timeout - returning 0x102 */
	_LSLP_SIGNAL_SEM(h);
	_LSLP_CLOSE_SEM(h);
	_LSLP_SLEEP(10);
	_LSLP_INIT_NETWORK();
	_LSLP_DEINIT_NETWORK();
}	


/*****************************************************************
 *  Windows Event Log Code
*****************************************************************/

BOOL windows_report_event(uint32 code, void *parm, uint32 parmIndex, va_list va)
{
  /* use the mask in lslp_Msgs to get the number and type of parameters */
  /* extract each paramter and build an array of string ptrs */
  /* pass that, plus the data parameter, to ReportEvent */

  int32 int_param, inserts;
  uint8 *str_param;
  uint8 **str_array;

  inserts = 0;
  if(NULL != (str_array = (uint8 **) malloc((lslp_Msgs[LSLP_MSG_CODE(code)].inserts + 1 * sizeof(uint8 *))))) {
    while(inserts < (int32)lslp_Msgs[LSLP_MSG_CODE(code)].inserts) {
      if(_LSLP_IS_BIT_SET(inserts, &(lslp_Msgs[LSLP_MSG_CODE(code)].mask) )) {
	/* parm is an integer */
	int_param = va_arg(va, int);
	if(NULL != (str_array[inserts] =  (uint8 *)calloc(20, sizeof(uint8)))) {
	  sprintf(str_array[inserts], "%d", int_param);
	} else { break ;} 
      } else {
	str_param = va_arg(va, char *) ;
	if(NULL == (str_array[inserts] = strdup(str_param))) {
	  break;
	}
      }
      inserts++;
    }
    if(inserts == (int32)lslp_Msgs[LSLP_MSG_CODE(code)].inserts) {
      if(lslp_syslog_handle != ((LSLP_SYSLOG_T)0)) {

	ReportEvent(lslp_syslog_handle, 
		    (WORD)LSLP_MSG_SEVERITY(code), 
		    (WORD)0, 
		    (DWORD)code, 
		    (PSID)NULL, 
		    (WORD)inserts,
		    (DWORD)LSLP_PARM_SIZE(parmIndex), 
		    str_array, 
		    (LPVOID)parm);
      }
    }
    while(inserts--) {
      if(str_array[inserts] != NULL) {
	free(str_array[inserts]);
      }
    }
    free(str_array);
  } /* if we allocated the string ptr array */
  return(TRUE);
}


/*#################################################################
 Windows NT Service Control Code 
 #################################################################*/



static SERVICE_STATUS slp_status;
static SERVICE_STATUS_HANDLE slp_status_handle;


/******************************************************************
 this routine is a dummy main() routine tailored specifically to 
 windows nt services .
*****************************************************************/
VOID WINAPI  _lslp_windows_main(void) 
{
  SERVICE_TABLE_ENTRY dispatch_table[] = 
  {
    {"slp", _lslp_service_start},
    {NULL, NULL}
  };
  /* let everyone know we are running (or trying to run) as an NT service */
  is_nt_service = TRUE ;
  strcpy(lslp_logPath, LSLP_CONFIG_FILE_NAME);
  if(! StartServiceCtrlDispatcher(dispatch_table)) {
    printf(lslp_Msgs[68].msg, GetLastError());
  }
  return;
}

/*****************************************************************
called by the NT service control manager to start the SLP service
*****************************************************************/
extern uint32 dieNow;
extern uint32 waitInterval;
extern int8 lslp_default_config[] ;
VOID WINAPI _lslp_service_start(DWORD argc, LPTSTR *argv) 
{

  DWORD status;
  DWORD specificError;
  
  strcpy(lslp_default_config, "slp.conf");
  slp_status.dwServiceType = SERVICE_WIN32;
  slp_status.dwCurrentState = SERVICE_START_PENDING;
  slp_status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN ;
  slp_status.dwWin32ExitCode = 0;
  slp_status.dwServiceSpecificExitCode = 0;
  slp_status.dwCheckPoint = 0;
  slp_status.dwWaitHint = 0;

  slp_status_handle = RegisterServiceCtrlHandler("slp", _lslp_service_ctrl_handler);
  if( slp_status_handle == (SERVICE_STATUS_HANDLE)0) {
    printf(lslp_Msgs[69].msg, GetLastError());
    return;
  }

  status = _lslp_service_initialization(argc, argv, &specificError);
  if(status != LSLP_OK) {
    slp_status.dwCurrentState = SERVICE_STOPPED;
    slp_status.dwCheckPoint = 0;
    slp_status.dwWaitHint = 0;
    slp_status.dwWin32ExitCode = status;
    slp_status.dwServiceSpecificExitCode = specificError;
    SetServiceStatus(slp_status_handle, &slp_status);
    printf(lslp_Msgs[70].msg, specificError);
    return;
  }

  slp_status.dwCurrentState = SERVICE_RUNNING;
  slp_status.dwCheckPoint = 0;
  slp_status.dwWaitHint = 0;

  if(!SetServiceStatus(slp_status_handle, &slp_status)) {
    printf(lslp_Msgs[71].msg, specificError);
    dieNow = 1;
  }
         
  /* loop and wait until it is time to die */                                       
  while(dieNow == 0) {
    _LSLP_SLEEP(waitInterval);
  }
  /* die */
  _LSLP_SLEEP(waitInterval * 5);
  _stopKernel(); /* shut everything down */
  return;
}

VOID WINAPI _lslp_service_ctrl_handler(DWORD opcode) 
{

  switch(opcode) {
  case SERVICE_CONTROL_STOP:
  case SERVICE_CONTROL_SHUTDOWN:
    dieNow = 1;
    slp_status.dwCurrentState = SERVICE_STOPPED;
    slp_status.dwCheckPoint = 0;
    slp_status.dwWaitHint = 0;
    slp_status.dwWin32ExitCode = 0;
    SetServiceStatus(slp_status_handle, &slp_status);
    return;
    break;
  default:
    break;
  }
  SetServiceStatus(slp_status_handle, &slp_status);
  return;
}

DWORD _lslp_service_initialization(DWORD argc, LPTSTR *argv, DWORD *specificError) 
{
  return(( *specificError = _startKernel()));
}
 

uint32 _lslp_install_nt_service( void ) 
{
  SC_HANDLE service_handle, sc_manager;
  int ccode = 0;
  LPCSTR path_name = "%SystemRoot%\\system32\\lslpkernel.exe";
  if(NULL != (sc_manager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS))) {
    if(NULL != (service_handle = CreateService(sc_manager, 
					       "slp",
					       "SLP v2.0 DA",
					       SERVICE_ALL_ACCESS,
					       SERVICE_WIN32_OWN_PROCESS,
					       SERVICE_DEMAND_START,
					       SERVICE_ERROR_NORMAL, 
					       path_name,
					       NULL, NULL, NULL, NULL, NULL))) {
      printf(lslp_Msgs[73].msg);
      CloseServiceHandle(service_handle);
    } else {   /* if we created the service */
      ccode = 1;
      printf(lslp_Msgs[74].msg, "CreateService", GetLastError());
    }
    CloseServiceHandle(sc_manager);
  } else {  /* if we opened the sc manager database */
    ccode = 1;
    printf(lslp_Msgs[74].msg, "OpenSCManager", GetLastError());
  }
  return(ccode);
}

uint32 _lslp_remove_nt_service(void) 
{

  SC_HANDLE service_handle, sc_manager;
  int ccode = 0;
  if(NULL != (sc_manager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS))) {
    if(NULL != (service_handle = OpenService(sc_manager, "slp", DELETE))) {
      if(FALSE == DeleteService(service_handle)) {
	ccode = 1;
	printf(lslp_Msgs[74].msg, "Delete Service", GetLastError());
      } else {
	printf(lslp_Msgs[75].msg);
      }
      CloseServiceHandle(service_handle);
    } else { /* if we opened the slp service */
      ccode = 1;
      printf(lslp_Msgs[74].msg, "OpenService", GetLastError());
    }
    CloseServiceHandle(sc_manager);
  } else {  /* if we opened the sc manager database */
    ccode = 1;
    printf(lslp_Msgs[74].msg, "OpenSCManager", GetLastError());
  }
  return(ccode);
}

/* from the MSDN Winsock 2 developer's information */
/* int WSAIoctl( SOCKET s,   */
/* 	      DWORD dwIoControlCode,   */
/* 	      LPVOID lpvInBuffer,   */
/* 	      DWORD cbInBuffer,   */
/* 	      LPVOID lpvOutBuffer,   */
/* 	      DWORD cbOutBuffer,   */
/* 	      LPDWORD lpcbBytesReturned,   */
/* 	      LPWSAOVERLAPPED lpOverlapped,   */
/* 	      LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine ); */

/* SIO_MULTICAST_SCOPE (opcode setting: I, T==1) */
/* Specifies the scope over which multicast transmissions will occur.  */
/* Scope is defined as the number of routed network segments to be covered.  */
/* A scope of zero would indicate that the multicast transmission would not be  */
/* placed on the wire but could be disseminated across sockets within the local host.  */
/* A scope value of one (the default) indicates that the transmission will be  */
/* placed on the wire, but will not cross any routers. Higher scope values  */
/* determine the number of routers that can be crossed. Note that this corresponds  */
/* to the time-to-live (TTL) parameter in IP multicasting. By default, scope is 1. */


int WindowsSetTTL(SOCKET sock, int ttl )
{
  DWORD bytes;
  return(WSAIoctl(sock, SIO_MULTICAST_SCOPE, &ttl, sizeof(ttl), 
		  NULL, 0, &bytes, NULL, NULL));
}


/* if the slp service is running on an NT machine that has dynamic addresses */
/* for example, the best thing to do is restart the SLP */
/* service when addresses change. For example, when failing over a clustered server, */
/* one of the tasks on the failover server should be to restart the SLP service. This */
/* will trigger a transport configuration query (below), and the restarted service */
/* will listen on all the new interfaces. */

/* If restarting the service is not an option,the service should listen on INADDR_ANY */ 
/* instead of on each interface. */

/* my problem here is that I was unable to get WSAIoctl to work the same way */
/* ioctl works on linux_get_transport_configuration. */
/* I need someone to write a routine that will return a list of all IP addresses */
/* on the NT box, plus whether or not each interface is broadcast and multicast */
/* capable, plus what the netmask is for each interface. */
/* Until I have that routine, I will essentially fake this information by */
/* using what is in the configuration file. */

void windows_get_transport_configuration(void)
{
  LSLP_CONFIG_ADV *this_config, *t;


  /* take the entire listen list and move it to the transport list. */
  if(! _LSLP_IS_HEAD(lslpConfigListen.next)) {
    _LSLP_LINK_HEAD(&lslpConfigTransport, &lslpConfigListen);
    lslpConfigListen.next = lslpConfigListen.prev = &lslpConfigListen;
  }
  t = lslpConfigTransport.next; 
  while(! _LSLP_IS_HEAD(t)) { 
    /* if this interface is on the exclude list or is the local host, flag it */
    if(t->iface == inet_addr("127.0.0.1")) { 
      t->mask = 0xffffffff; 
      t = t->next; 
      continue; 
    } 

    /* if this interface is on the exclude list, flag it */
    this_config = lslpConfigDontListen.next; 
    while(! _LSLP_IS_HEAD(this_config)) { 
      if(t->iface == this_config->iface ) { 
 	t->mask = 0xffffffff;  
 	break; 
      } 
      this_config = this_config->next; 
    }
    /* see if this is the desired multicast interface */
    if((lslp_mcast_int == INADDR_ANY)  || (t->iface ==  lslp_mcast_int ))
      t->flag = IFF_MULTICAST;
    t->flag |= IFF_UP;

    t = t->next;
  } 
}


/* lifted from http://msdn.microsoft.com, then modified */
int ReturnNTVersion()
{
   OSVERSIONINFOEX osvi;
   BOOL bOsVersionInfoEx;

   // Try calling GetVersionEx using the OSVERSIONINFOEX structure.
   //
   // If that fails, try using the OSVERSIONINFO structure.

   ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
   osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

   if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) ) {
      // If OSVERSIONINFOEX doesn't work, try OSVERSIONINFO.
     
     osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
     if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
       return 0;
   }

   switch (osvi.dwPlatformId)   {
      case VER_PLATFORM_WIN32_NT:
	return(osvi.dwMajorVersion);
   default:
     break;
   }
   return(0);
}

