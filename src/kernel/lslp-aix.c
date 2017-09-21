/* .                             */
/*                                                    */
/* .                             */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/*                                                        */
/*                                                                        */
/*                                       */
/*                                                                        */ 
/*                                               */
/*                                                                        */
/*                                                                        */
/*           */
/*                                                     */
/*                                                                        */
/*          */
/*            */
/* .                              */
/*                                                                        */
/*                                                      */



/*                                                                        */
/*                                                                        */
/*                                                                        */
/*                                                        */
/*                                                                        */
/*                                       */
/*                                                                        */
/*           */
/*                                                     */
/*                                                                        */
/*          */
/*            */
/* .                              */
/*                                                                        */
/*                                                      */

/*******************************************************************
 *  Description:   aix.c
 *
 *****************************************************************************/
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <assert.h>
#include <pthread.h>
#include "lslp-common-defs.h"

/* Thread safe syslog_r calls need syslog_data structure */
struct syslog_data kernel_syslog_data = SYSLOG_DATA_INIT;

/**  h = handle, m = milliseconds, c = pointer to completion code  **/
/** #define _LSLP_WAIT_MUTEX(h, m, c)                              **/
extern void _stopKernel(void);
extern volatile sig_atomic_t dieNow;
extern LSLP_THREAD_T dispatchHandle;
extern LSLP_THREAD_T slpOutHandle;
extern LSLP_THREAD_T slpInHandle;
extern LSLP_THREAD_T apiHandle;
extern LSLP_CONFIG_ADV lslpConfigTransport; /* transport configuration */
extern LSLP_CONFIG_ADV lslpConfigListen;
extern LSLP_CONFIG_ADV lslpConfigDontListen;

void
lslp_linux_wait_mutex(sem_t *h, uint32 m, uint32 *c)
{
  int count = 0;
  assert(h != NULL);
  assert(m != 0);
  assert(c != NULL);

  while(0 != (*c = sem_trywait(h))) {
      usleep(m);
      count++;
      if(count > LSLP_Q_RETRIES)
	abort();
  }
  return;
}


uint32 
lslp_linux_begin_thread(void *(*start)(void *), uint32 ss, void *a)
  {
    pthread_t thread;
    if(0 == pthread_create(&thread, NULL, start, a))
      return((uint32)thread);
    return(-1);
  }


void  num_to_ascii(uint32 val, int8 *buf, int32 radix, BOOL is_neg)
{
  int8 *p;
  int8 *firstdig;
  int8 temp;
  uint32 digval;

  assert(buf != NULL);
  p = buf;
  /* safeguard against a div by zero fault ! */
  if(val == 0){
    *p++ = '0';
    *p = 0x00;
    return;
  }

  /* default to a decimal radix */
  if (radix <= 0)
    radix = 10;

  /* negate two's complement, print a minus sign */
  if (is_neg == TRUE){
    *p++ = '-';
    val = (uint32)(-(int32)val);
  }

  firstdig = p;

  do {
    digval = (uint32) (val % radix);
    val /= radix;
    if(digval > 9)
      *p++ = (int8)(digval - 10 + 'a');
    else
      *p++ = (int8)(digval + '0');
  } while (val > 0);

  /* by getting the mod value before the div value, the digits are */
  /* reversed in the buffer */
  /* terminate the string */
  *p-- = '\0';
  do {
    temp = *p;
    *p = *firstdig;
    *firstdig = temp;
    --p;
    ++firstdig;
  }while(firstdig < p);
  return;  
}


void  hug_num_to_ascii(uint64 val, int8 *buf, int32 radix, BOOL is_neg)
{
  int8 *p;
  int8 *firstdig;
  int8 temp;
  uint64 digval;

  assert(buf != NULL);
  p = buf;
  /* safeguard against a div by zero fault ! */
  if(val == 0){
    *p++ = '0';
    *p = 0x00;
    return;
  }

  /* default to a decimal radix */
  if (radix <= 0)
    radix = 10;

  /* negate two's complement, print a minus sign */
  if (is_neg == TRUE){
    *p++ = '-';
    val = (uint64)(-(int64)val);
  }

  firstdig = p;

  do {
    digval = (uint64)(val % radix);
    val /= radix;
    if(digval > 9)
      *p++ = (int8)(digval - 10 + 'a');
    else
      *p++ = (int8)(digval + '0');
  } while (val > 0);

  /* by getting the mod value before the div value, the digits are */
  /* reversed in the buffer */
  /* terminate the string */
  *p-- = '\0';
  do {
    temp = *p;
    *p = *firstdig;
    *firstdig = temp;
    --p;
    ++firstdig;
  }while(firstdig < p);
  return;  
}


/* signal handler */
void _lslp_term(int sig) 
{
  if(dieNow)
    raise(sig);  /* recurse into oblivion */
  dieNow = 1; 
  fprintf(stderr, "\nSLP kernel: received interrupt, exiting...\n");
  /* kill threads */
  pthread_cancel(dispatchHandle);
  pthread_cancel(slpOutHandle);
  pthread_cancel(slpInHandle);
  _stopKernel(); /* shut everything down */
  signal(sig, SIG_DFL);
  raise(sig);
}
 
int32 _lslp_thread_sig_action(void)
{
  sigset_t sa_mask;
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
  sigemptyset(&sa_mask);
  sigaddset(&sa_mask, SIGHUP);
  sigaddset(&sa_mask, SIGINT);
  sigaddset(&sa_mask, SIGTERM);
  sigaddset(&sa_mask, SIGPIPE);
  /* sigaddset(&sa_mask, SIGQUIT); */
  return(pthread_sigmask(SIG_BLOCK, &sa_mask, NULL));

}

int32 _lslp_sig_action(void) 
{

  sigset_t sa_mask;
  sigemptyset(&sa_mask);
  sigaddset(&sa_mask, SIGHUP);
  sigaddset(&sa_mask, SIGINT);
  sigaddset(&sa_mask, SIGTERM);
  sigaddset(&sa_mask, SIGPIPE);
  if(-1 != sigprocmask(SIG_UNBLOCK, &sa_mask, NULL)) {
    /* explicitly unblock signals */
    if(SIG_ERR != signal(SIGHUP, _lslp_term)) {
      if(SIG_ERR != signal(SIGINT, _lslp_term)) {
	if(SIG_ERR != signal(SIGTERM, _lslp_term))
	  if(SIG_ERR != signal(SIGPIPE, _lslp_term))
	    return(0);
      }
    }
  }
  return(-1);
}

/* since we are running on a linux machine, this function always fails */
/* after printing a message */
uint32 _lslp_install_nt_service(void)
{
  printf(lslp_Msgs[72].msg, "Service Not Installed");
  return(1);
}

uint32 _lslp_remove_nt_service(void) 
{
  printf(lslp_Msgs[72].msg, "Service Not Removed");
  return(1);

}


BOOL linux_report_event(uint32 code, void *parm, uint32 parmIndex, va_list va)
{
  int priority = LSLP_MSG_SEVERITY(code);
  switch(priority) {
  default:
  case LSLP_SEVERITY_SUCCESS:
  case LSLP_SEVERITY_INFO:
    priority = LOG_INFO;
    break;
  case LSLP_SEVERITY_WARN:
    priority = LOG_WARNING;
    break;
  case LSLP_SEVERITY_ERROR:
    priority = LOG_ERR;
    break;
  }
  // vsyslog(priority, lslp_Msgs[LSLP_MSG_CODE(code)].msg, va); 
  // Thread safe version of syslog call
  syslog_r(priority, &kernel_syslog_data, lslp_Msgs[LSLP_MSG_CODE(code)].msg, va);
  return(TRUE);
}


/*****************************************************************
 *
 * http://www.landfield.com/faqs/unix-fsocket/
 *
 * 
 * It seems to be the case in the majority of implementations that a
 *  socket bound to a unicast address will not see incoming packets with
 * broadcast addresses as their destinations.
 *
 *  The approach I've taken is to use SIOCGIFCONF to retrieve the list of
 *  active network interfaces, and SIOCGIFFLAGS and SIOCGIFBRDADDR to
 *  identify broadcastable interfaces and get the broadcast addresses.
 *  Then I bind to each unicast address, each broadcast address, and to
 *  INADDR_ANY as well. That last is necessary to catch packets that are
 *  on the wire with INADDR_BROADCAST in the destination.  (SO_REUSEADDR
 *  is necessary to bind INADDR_ANY as well as the specific addresses.)
 *
 *  This gives me very nearly what I want. The wrinkles are:
 *
 *  o  I don't assume that getting a packet through a particular socket
 *     necessarily means that it actually arrived on that interface.
 *
 *  o  I can't tell anything about which subnet a packet originated on if
 *     its destination was INADDR_BROADCAST.
 *
 *  o  On some stacks, apparently only those with multicast support, I get
 *     duplicate incoming messages on the INADDR_ANY socket.
 *
 *****************************************************************/

void linux_get_transport_configuration(void)
{
  int sock;
  LSLP_CONFIG_ADV *this_config;
  LSLP_CONFIG_ADV *t; 
  BOOL found = FALSE; 

  /* do an ioctl to get the actual transport configuration of this box */
  if(-1 < (sock = socket(AF_INET, SOCK_DGRAM, 0))) {
    LSLP_CONFIG_ADV *iface;
    struct ifconf conf;
    /* allocate room for up to 256 interfaces */
    if(NULL != (conf.ifc_buf = (char *)calloc(256, sizeof(struct ifreq)))) {
      conf.ifc_len = 256 * sizeof(struct ifreq);
      if(-1 < ioctl(sock, SIOCGIFCONF, &conf) ) {
	if(NULL != (iface = (LSLP_CONFIG_ADV *)calloc(1, sizeof(LSLP_CONFIG_ADV)))) {
	  SOCKADDR_IN *addr;
	  struct ifreq *temp;
	  struct ifreq *r = conf.ifc_req;
	  addr = (SOCKADDR_IN *)&(r->ifr_addr);
	  while((iface != NULL) && (addr->sin_addr.s_addr != 0) && (addr->sin_addr.s_addr != inet_addr("127.0.0.0" ))) {
	    iface->iface = addr->sin_addr.s_addr;
	    /* get the flags for this interface */
	    temp = r;
	    if ( -1 < ioctl(sock, SIOCGIFFLAGS , temp)) {
	      iface->flag = (uint32)temp->ifr_flags;
	      temp = r;
	      if (r->ifr_flags & IFF_BROADCAST) {
		temp = r;
		if( -1 < ioctl(sock, SIOCGIFBRDADDR, temp)) {
		  /* get the actual broadcast address for this box */
		  iface->addr = ((SOCKADDR_IN *)(&temp->ifr_broadaddr))->sin_addr.s_addr;
		}
	      }
	      _LSLP_INSERT(iface, &lslpConfigTransport);
	      iface = (LSLP_CONFIG_ADV *)calloc(1, sizeof(LSLP_CONFIG_ADV));
	    }
	    r++;
	    addr = (SOCKADDR_IN *)&(r->ifr_addr);
	  } /* while grabbing interfaces */
	  if(iface != NULL) 
	    free(iface);
	} /* allocated config struct */
      } /* ioctl success */
      free(conf.ifc_buf);
    } /* allocated interface configuration buffer */
    close(sock);
  } /* got socket */


  /* We want to get as many sockets as possible listening. */
  /* The transport list represents the actual configuration of the hardware */
  /* if the listen and exclude lists are empty, we will go with the transport list*/
  /* as it is. */

  /* if the listen list has been initialized, subtract all interfaces that */
  /* are not on the listen list. Then subtract all interfaces that are on the */
  /* dont_listen list. */

  /* if the result is an empty list (which should only result from a configuration error),*/
  /*  we create one listener interface and bind it to INADDR_ANY. */

  /* if the configuration did not specify a particular multicast interface, */
  /* pick one from the transport list */
  
  t = lslpConfigTransport.next; 
  while(! _LSLP_IS_HEAD(t)) { 
    /*     if this interface is on the exclude list or is the local host, flag it */
    if(t->iface == inet_addr("127.0.0.1")) { 
      t->mask = 0xffffffff; 
      t = t->next; 
      continue; 
    } 
    this_config = lslpConfigDontListen.next; 
    while(! _LSLP_IS_HEAD(this_config)) { 
      if(t->iface == this_config->iface ) { 
 	t->mask = 0xffffffff;  
 	break; 
      } 
      this_config = this_config->next; 
    } 
    /* if this interface is not on the include list, flag it */
    if((t->mask != 0xffffffff) && (! _LSLP_IS_EMPTY(&lslpConfigListen ))) { 
      this_config = lslpConfigListen.next; 
      while(! _LSLP_IS_HEAD(this_config)) { 
 	if(t->iface == this_config->iface) { 
 	  found = TRUE;  
 	  break; 
 	} 
 	this_config = this_config->next; 
      } 
      if(found == FALSE) 
 	t->mask = 0xffffffff; 
    } 
    /* if there is a requested multicast address, make sure we mask out others */
    if((lslp_mcast_int != INADDR_ANY) && (t->iface !=  lslp_mcast_int ))
      t->flag &=  ~IFF_MULTICAST;
    t = t->next; 
  } 

  /* make certain we have an interface to listen on. Its probably better */
  /* to listen on INADDR_ANY if the user mis-configures the server such that */
  /* no interfaces are listening. */
  t = lslpConfigTransport.next; 
  found = FALSE; 
  while(! _LSLP_IS_HEAD(t)) { 
    if(t->mask != 0xffffffff) { 
      found = TRUE; 
      break; 
    } 
    t = t->next; 
  } 

  if( FALSE == found ) { 
    /* create a dummy transport because the user caused all transports to be masked out */
    if(NULL != (this_config = (LSLP_CONFIG_ADV *)calloc(1, sizeof(LSLP_CONFIG_ADV)))) { 
      this_config->addr = INADDR_ANY; 
      _LSLP_INSERT(this_config, &lslpConfigTransport); 
    } 
  }  
  return;
}


