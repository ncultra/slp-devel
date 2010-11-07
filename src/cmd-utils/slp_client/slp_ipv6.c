#include "../lslp-common-defs.h"
#include "../lslp.h"
#include "slp_client.h"

extern int32 LSLP_MTU;
#ifndef LSLP_MAX_PATH
#define LSLP_MAX_PATH 255
#endif 
#if defined (_WIN32)
#include <memory.h>

// For compatibility with Windows 2000 - The GetAdaptersAddresses call does not exist
typedef ULONG (WINAPI *FPGETADAPTERSADDRESSESW)(ULONG Family, ULONG Flags, PVOID Reserved, PIP_ADAPTER_ADDRESSES AdapterAddresses, PULONG SizePointer);

DLL_EXPORT const char *inet_ntop(int af, const void *src, char *dst, socklen_t cnt)
{
  
  int err;
  DWORD len;
  
  len = (DWORD)(cnt & 0x0000ffff);
  
  if (af == AF_INET) {
    struct sockaddr_in sin;
    const struct in_addr *src_addr;
    src_addr = (const struct in_addr *)src;
    
    sin.sin_family = AF_INET;
    sin.sin_port = 0;
    sin.sin_addr= *src_addr;
    
    err = WSAAddressToStringA((LPSOCKADDR)&sin, 
			     sizeof(sin),
			     NULL,
			     dst,
			     &len);
    if (err)
      goto err_out;
  }

  if (af == AF_INET6) {
    struct sockaddr_in6 sin6;
    struct in6_addr *src_addr;
    src_addr = (struct in6_addr *)src;
    
    sin6.sin6_family = AF_INET6;
    sin6.sin6_port = 0;
    sin6.sin6_addr = *src_addr;
    
    err = WSAAddressToStringA((LPSOCKADDR)&sin6, 
			     sizeof(sin6),
			     NULL,
			     dst,
			     &len);
    if (err)
      goto err_out;
  }
  return dst;
 err_out:
  SLP_TRACE("Error in inet_ntop %d\n",WSAGetLastError());
  return NULL;
}

DLL_EXPORT int inet_pton(int af, const char *src, void *dst)
{
  int err=0, len=0;
  struct sockaddr_storage saddr;
  
  memset(&saddr, 0x00, sizeof(saddr));
  
    if (af == AF_INET) {

      struct sockaddr_in *sin;
      sin = (struct sockaddr_in *)&saddr;
      len = sizeof(struct sockaddr_in);

      err = WSAStringToAddress((LPSTR)src, af, NULL, (LPSOCKADDR)&saddr, &len);
      if (err < 0)
          goto err_out;
      
      sin->sin_family = AF_INET;
      memcpy(dst, &sin->sin_addr, sizeof(struct in_addr));

    }
    if (af == AF_INET6) {

      struct sockaddr_in6 *sin6;
      sin6 = (struct sockaddr_in6 *)&saddr;
      len = sizeof(struct sockaddr_in6);

      err = WSAStringToAddress((LPSTR)src, af, NULL, (LPSOCKADDR)&saddr, &len);
      if (err < 0)
          goto err_out;

      sin6->sin6_family = AF_INET6;
      memcpy(dst, &sin6->sin6_addr, sizeof(struct in6_addr));
    }
  return dst;
 err_out:
  SLP_TRACE("Error in inet_pton %d\n",WSAGetLastError());
  return NULL;
}

static struct sockaddr_list * win_get_addr_list(struct sockaddr_list *head, int family)
{
    // Load the library iphlpapi.dll and check if GetAdaptersAddresses function is supported. This will ensure that
    // this function is called only on supported version of Windows. (Windows 2000 do not support GetAdaptersAddresses() )
    HMODULE library = LoadLibrary("iphlpapi.dll");
    if (library != 0) {
        FPGETADAPTERSADDRESSESW GetAdaptersAddressesW = (FPGETADAPTERSADDRESSESW) GetProcAddress(library,"GetAdaptersAddresses");

        if (GetAdaptersAddressesW != 0) {
            PIP_ADAPTER_ADDRESSES AdapterAddresses = NULL;
            ULONG OutBufferLength = 0;
            ULONG RetVal = 0, i;    
            struct sockaddr_list *tmp = NULL;
            struct sockaddr_in *win_sin = NULL;
            struct sockaddr_in6 *win_sin6 = NULL;
	

            // The size of the buffer can be different 
            // between consecutive API calls.
            // In most cases, i < 2 is sufficient;
            // One call to get the size and one call to get the actual parameters.
            // But if one more interface is added or addresses are added, 
            // the call again fails with BUFFER_OVERFLOW. 
            // So the number is picked slightly greater than 2. 
            // We use i <5 in the example

            for (i = 0; i < 5; i++) {
                if (family == AF_INET) 
	            RetVal =  GetAdaptersAddressesW(AF_INET, 0, NULL, 
				       AdapterAddresses,&OutBufferLength);
                else
	                  RetVal =  GetAdaptersAddressesW(AF_INET6, 0, NULL, 
				       AdapterAddresses, &OutBufferLength);
        
                if (RetVal != ERROR_BUFFER_OVERFLOW) {
                    break;
                }

                if (AdapterAddresses != NULL) {
                    FREE(AdapterAddresses);
                }
        
                AdapterAddresses = (PIP_ADAPTER_ADDRESSES) 
                    MALLOC(OutBufferLength);
                if (AdapterAddresses == NULL) {
                    RetVal = GetLastError();
                    break;
                }
            }
    
            if (RetVal == NO_ERROR) {
                PIP_ADAPTER_ADDRESSES AdapterList = AdapterAddresses;
                SLP_TRACE("found adaptor %ws \n", AdapterList->Description);
                while (AdapterList) {
                    //struct sockaddr_list *tmp = (struct sockaddr_list *)calloc(1, sizeof(struct sockaddr_list));
	                PIP_ADAPTER_UNICAST_ADDRESS addr;
	                addr = AdapterList->FirstUnicastAddress;
	                while ((addr) && (addr->Address.lpSockaddr != NULL)) {
	                    win_sin = (struct sockaddr_in *)addr->Address.lpSockaddr;

                        tmp = (struct sockaddr_list *)MALLOC(sizeof(struct sockaddr_list));
	                    if (tmp)
                            memset(tmp, 0x00, sizeof(struct sockaddr_list));
                        else
                            break;
                        tmp->next = tmp->prev = tmp;

                        if (family == AF_INET && win_sin->sin_family == AF_INET) {

                            tmp->addr.sin.sin_family = AF_INET;

                            tmp->addr.sin.sin_port = win_sin->sin_port;

                            tmp->addr.sin.sin_addr.s_addr = win_sin->sin_addr.s_addr;

                            strncpy(tmp->addr6, inet_ntoa(tmp->addr.sin.sin_addr), INET6_ADDRSTRLEN);
		  
		                } else if (family == AF_INET6 && win_sin->sin_family == AF_INET6) {
		                    win_sin6 = (struct sockaddr_in6 *)addr->Address.lpSockaddr;
                            tmp->addr.sin6.sin6_family = AF_INET6;
		                    tmp->addr.sin6.sin6_port = win_sin6->sin6_port;
                            memcpy(&tmp->addr.sin6.sin6_addr, &win_sin6->sin6_addr, sizeof(tmp->addr.sin6.sin6_addr));
                            tmp->addr.sin6.sin6_scope_id = win_sin6->sin6_scope_id;
                            tmp->if_index = AdapterList->Ipv6IfIndex;
                            inet_ntop(AF_INET6, &tmp->addr.sin6.sin6_addr, tmp->addr6, INET6_ADDRSTRLEN);
		                } else {
                            free(tmp);
                            AdapterList = AdapterList->Next;
                            continue;
		                }

                        strncpy(tmp->name, AdapterList->AdapterName, LSLP_PATH_MAX);

                        tmp->mtu = AdapterList->Mtu;

                        _LSLP_INSERT(tmp, head);

                        addr = addr->Next;


	                }

	                AdapterList = AdapterList->Next;

	            }
            } 
            else {
                SLP_TRACE("error retrieving Windows interface list %i\n", RetVal);
            }  

            if (AdapterAddresses != NULL) {
                FREE(AdapterAddresses);
            }
        }
        FreeLibrary(library);

    }

    return head;
}

void ip6_trace_socket(int sock)  
{
  union slp_sockaddr buf;
  
  int err; 
  char addr6[INET6_ADDRSTRLEN];
  
  socklen_t size = sizeof(buf);

  err = getsockname(sock, (struct sockaddr *)&buf.saddr, &size);
  if (err < 0) {
    SLP_TRACE("trace_socket: error getting name %d\n", err);
    return;
  }
    
  if (buf.saddr.sa_family == AF_INET) {
    inet_ntop(AF_INET, &buf.sin.sin_addr.s_addr, addr6, INET6_ADDRSTRLEN);
    SLP_TRACE("socket %d: AF_INET, %s:%d\n", sock, addr6, ntohs(buf.sin.sin_port));
    
  } if (buf.saddr.sa_family == AF_INET6) {
    inet_ntop(AF_INET6, &buf.sin6.sin6_addr, addr6, INET6_ADDRSTRLEN);
    SLP_TRACE("socket %d: AF_INET6, %s:%d\n", sock, addr6, ntohs(buf.sin6.sin6_port));
  } 
}
#else 

void ip6_trace_socket(int sock)  
{
  union slp_sockaddr buf;
  
  int err; 
  char addr6[INET6_ADDRSTRLEN];
  
  socklen_t size = sizeof(buf);
  
  if (sock < 0)
    return;
  
  err = getsockname(sock, (struct sockaddr *)&buf.saddr, &size);
  if (err < 0) {
    SLP_TRACE("trace_socket: error getting name %d\n", err);
    return;
  }
    
  if (buf.saddr.sa_family == AF_INET) {
    inet_ntop(AF_INET, &buf.sin.sin_addr.s_addr, addr6, INET6_ADDRSTRLEN);
    SLP_TRACE("socket %d: AF_INET, %s:%d\n", sock, addr6, ntohs(buf.sin.sin_port));
    
  } else if (buf.saddr.sa_family == AF_INET6) {
    inet_ntop(AF_INET6, &buf.sin6.sin6_addr, addr6, INET6_ADDRSTRLEN);
    SLP_TRACE("socket %d: AF_INET6, %s:%d\n", sock, addr6, ntohs(buf.sin6.sin6_port));
  } else
    SLP_TRACE("not an internet socket: %d\n", sock);
}

#endif

#ifdef __linux__
#define _PATH_PROCNET_IFINET6 "/proc/net/if_inet6"

struct sockaddr_list *ip6_get_addr_list(struct sockaddr_list *head)
{
  FILE *f;
  char addr6p[8][5];
  char addr6[INET6_ADDRSTRLEN], devname[20];
  int scope, dummy;
  struct sockaddr_list *tmp;
  
  f = fopen(_PATH_PROCNET_IFINET6, "r");
  if (f == NULL)
    return head;

  while (fscanf(f, "%4s%4s%4s%4s%4s%4s%4s%4s %02x %02x %02x %02x %20s\n",
		addr6p[0], addr6p[1], addr6p[2], addr6p[3],
		addr6p[4], addr6p[5], addr6p[6], addr6p[7],
		&dummy, &dummy, &scope, &dummy, devname) != EOF) {

    sprintf(addr6, "%s:%s:%s:%s:%s:%s:%s:%s",
	    addr6p[0], addr6p[1], addr6p[2], addr6p[3],
	    addr6p[4], addr6p[5], addr6p[6], addr6p[7]);
    
    tmp = (struct sockaddr_list *)calloc(1, sizeof(struct sockaddr_list));
    inet_pton(AF_INET6, addr6, &tmp->addr.sin6.sin6_addr);
    inet_ntop(AF_INET6, &tmp->addr.sin6.sin6_addr, tmp->addr6, INET6_ADDRSTRLEN);
    tmp->addr.saddr.sa_family = AF_INET6;
    tmp->addr.sin6.sin6_scope_id = scope;
    strncpy(tmp->name, devname, 19);
    tmp->if_index = if_nametoindex(tmp->name);
    _LSLP_INSERT(tmp, head);    
    SLP_TRACE("found ipv6 %s on dev %s\n", addr6, tmp->name);
  }
  fclose(f);
  
  return head;
}
#else

#if defined (_WIN32)
// windows support
struct sockaddr_list *ip6_get_addr_list(struct sockaddr_list *head)
{
  return win_get_addr_list(head, AF_INET6);
}
#else /* _WIN32 */
inline struct sockaddr_list *ip6_get_addr_list(struct sockaddr_list *head)
{
  return head;
}
#endif /* windows or not windows */
#endif /* linux */


#if defined(_WIN32)
struct sockaddr_list *ip_get_addr_list(struct sockaddr_list *head)
{
  return win_get_addr_list(head, AF_INET);
}

#else 

struct sockaddr_list *ip_get_addr_list(struct sockaddr_list *head)
{
  struct sockaddr_list *tmp;
  int sock, len, lastlen = 0;
  char *ptr, *buf;
  struct ifconf ifc;
  struct ifreq *ifr, ifreq;
  
  sock = socket(AF_INET, SOCK_DGRAM, 0);

  if (sock < 0)
    return head;
  
  len = 100 * sizeof(struct ifreq);
  for ( ; ; ) {
    buf = (char *)malloc(len);
    ifc.ifc_len = len;
    ifc.ifc_buf = buf;
    if (ioctl(sock,  SIOCGIFCONF, &ifc) < 0) {
      if (errno != EINVAL || lastlen != 0) {
	SLP_TRACE("ioctl_error\n");
	free(buf);
	_LSLP_CLOSESOCKET(sock);
	
	return head;
      }
    } else {
      if (ifc.ifc_len == lastlen)
	break;
      lastlen = ifc.ifc_len;
    }
    len += 10 * sizeof(struct ifreq);
  }
  
  for (ptr = buf; ptr < buf + ifc.ifc_len; ) {
    /* ip address */
    tmp = (struct sockaddr_list *)calloc(1, sizeof(struct sockaddr_list));
    ifr = (struct ifreq *)ptr;
    ifreq = *ifr;
    
    memcpy(&tmp->addr.sin, &ifr->ifr_addr, sizeof(tmp->addr.sin));
    strncpy(tmp->name, ifr->ifr_name, 19);
    inet_ntop(AF_INET, &tmp->addr.sin.sin_addr.s_addr, tmp->addr6, INET6_ADDRSTRLEN);
    tmp->if_index = if_nametoindex(tmp->name);
    /* broadcast address */

    if (ioctl(sock, SIOCGIFBRDADDR, &ifreq) < 0) {
      memset(&tmp->bcast_addr, 0, sizeof(union slp_sockaddr));
      SLP_TRACE("unable to find bcast addr for %s\n", tmp->name);
    }
    else {
      char tmp_addr[INET6_ADDRSTRLEN];
      memcpy(&tmp->bcast_addr.sin, &ifreq.ifr_broadaddr, sizeof(struct sockaddr_in));
      inet_ntop(AF_INET, &tmp->bcast_addr.sin.sin_addr.s_addr, tmp_addr, INET6_ADDRSTRLEN);
      SLP_TRACE("bcast addr for %s is %s\n", tmp->name, tmp_addr);
    }
    
    /* hwaddr */

    if (ioctl(sock, SIOCGIFHWADDR, &ifreq) < 0)
      memset(&tmp->hw_addr, 0, sizeof(union slp_sockaddr));
    else {
      memcpy(&tmp->hw_addr.saddr.sa_data, ifreq.ifr_hwaddr.sa_data, 8);
      tmp->hw_addr.saddr.sa_family  = ifreq.ifr_hwaddr.sa_family;
    }
    
    /* net mask */
    if (ioctl(sock, SIOCGIFNETMASK, &ifreq) < 0)
      memset(&tmp->netmask.sin, 0, sizeof(struct sockaddr_in));
    else
      memcpy(&tmp->netmask.sin,  &ifreq.ifr_netmask, sizeof(struct sockaddr_in));

    /* flags */
    if (ioctl(sock, SIOCGIFFLAGS, &ifreq) <0)
      tmp->flags = 0;
    else
      tmp->flags = ifreq.ifr_flags;    

    /* mtu */
    if (ioctl(sock, SIOCGIFMTU, &ifreq) < 0)
	tmp->mtu = 0;
    else
	tmp->mtu = ifreq.ifr_mtu;

    _LSLP_INSERT(tmp, head);
      SLP_TRACE("found ipv4 %s on dev %s\n", tmp->addr6, tmp->name);
    ptr += sizeof(struct ifreq);
  }
  free(buf);
  _LSLP_CLOSESOCKET(sock);
  
  return head;
}

#endif 


void ip6_set_client_addrs(struct slp_client *client, 
			  const char *target, 
			  const char *local,
			  unsigned short int port)
{

  if( client == NULL ) abort();
  SLP_TRACE("ip6_set_client_addrs\n");
  SLP_TRACE("target: %s; local: %s; version: %i\n", 
	    target, local, client->ip_version);

  client->target_addr.saddr.sa_family = AF_INET6;
  if (target == NULL )
    inet_pton(AF_INET6, "::1", &client->target_addr.sin6.sin6_addr);
  else 
    inet_pton(AF_INET6, target, &client->target_addr.sin6.sin6_addr);
  if (port == 0)
    client->target_addr.sin6.sin6_port = htons(427);
  else
    client->target_addr.sin6.sin6_port = htons(port);
  
  client->local_addr.saddr.sa_family = AF_INET6;
  if (local == NULL )
    inet_pton(AF_INET6, "::", &client->local_addr.sin6.sin6_addr);
  else 
    inet_pton(AF_INET6, local, &client->local_addr.sin6.sin6_addr);
  if (port == 0)
    client->local_addr.sin6.sin6_port = htons(427);
  else
    client->local_addr.sin6.sin6_port = htons(port);

}

void ip_set_client_addrs(struct slp_client *client, 
			  const char *target, 
			  const char *local,
			  unsigned short int port)
{
  struct sockaddr_in *sin;
  SLP_TRACE("ip_set_client_addrs\n");

  if (client == NULL) abort();
  sin = (struct sockaddr_in *)&client->target_addr;
  sin->sin_family = AF_INET;
  SLP_TRACE("client port (3) %d\n", client->_target_port);
  
  if (target == NULL )
    inet_pton(AF_INET, "127.0.0.1", &sin->sin_addr.s_addr);
  else 
    inet_pton(AF_INET, target, &sin->sin_addr);
  if (port == 0)
    sin->sin_port = htons(427);
  else
    sin->sin_port = htons(port);
  SLP_TRACE("client port (4) %d\n", client->_target_port);
  sin = (struct sockaddr_in *)&client->local_addr;
  sin->sin_family = AF_INET;
  if (local == NULL )
    inet_pton(AF_INET, "255.255.255.255", &sin->sin_addr.s_addr);
  else 
    inet_pton(AF_INET, local, &sin->sin_addr);
  if (port == 0)
    sin->sin_port = htons(427);
  else
    sin->sin_port = htons(port);
  SLP_TRACE("client port (5) %d\n", client->_target_port);
}


//#define IPV6_ADDR_LOOPBACK	0x0010U
//#define IPV6_ADDR_LINKLOCAL	0x0020U
//#define IPV6_ADDR_SITELOCAL	0x0040U

//ipv6 scope prefix:
//  link-local: 0xfe80
//  site-local: 0xfec0/10
//  global: 0x0001
  
//2.3 Site-Local Scope

//  FF05:0:0:0:0:0:0:2       All Routers Address              [ADDARCH]

//   FF05:0:0:0:0:0:1:1000    Service Location                 [RFC2165]
//    -FF05:0:0:0:0:0:1:13FF

//http://www.faqs.org/rfcs/rfc2375.html
//http://www.faqs.org/rfcs/rfc3111.html  


//#define SVRLOC_GRP    "ff05:0:0:0:0:0:0:116"
//#define SVRLOC_DA_GRP "ff05:0:0:0:0:0:0:123"
//#define SLP_BASE      "ff05:0:0:0:0:0:1:1000"

//group-id to join = slp_hash("service:service-agent") + base address
//                    = 0x01d8 + FF0X:0:0:0:0:0:1:1000
//                    = FF0X:0:0:0:0:0:1:11d8

/* from rfc3111 */

int ip4_join_leave_group_all(const char *group, struct slp_client *client, int join)
{
  struct slp_net *listener = client->listeners.next;
  

  while(!_LSLP_IS_HEAD(listener)) {
    if (listener->addr.saddr.sa_family == AF_INET && 
	listener->l.type == SOCK_DGRAM &&
	strcmp(listener->interface, "IF_ANY"))
      ip4_join_leave_group(client, group, listener, join);
    listener = listener->next;
  }
  return 0;
}

int ip4_join_leave_group(struct slp_client *client, 
			 const char *group, 
			 struct slp_net *listen,
			 int join)
{
  struct ip_mreq mreq;
  int err = 1, option;

  if (listen->sock < 0) {
    SLP_TRACE("error opening socket: %i\n", errno);
    return listen->sock;
  }
  
  mreq.imr_interface.s_addr = listen->addr.sin.sin_addr.s_addr;
  inet_pton(AF_INET, group, &mreq.imr_multiaddr.s_addr);

  if (join)
    option = IP_ADD_MEMBERSHIP;
  else
    option = IP_DROP_MEMBERSHIP;

  err = setsockopt(listen->sock, IPPROTO_IP, option, (const char *)&mreq, sizeof(mreq));

  if (err < 0) {
    char addr6[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &mreq.imr_interface.s_addr, addr6, INET_ADDRSTRLEN);
    SLP_TRACE("join multicast: interface %s\n", addr6);
    inet_ntop(AF_INET, &mreq.imr_multiaddr.s_addr, addr6, INET_ADDRSTRLEN);
    SLP_TRACE("join multicast: group %s\n", addr6);
    SLP_TRACE("error %s on %s errno %d sock %i group %s\n", 
	      join ? "join" : "leave", listen->interface, errno, listen->sock, group);
  }
  else 
    SLP_TRACE("%s on interface %s\n", join ? "joined" : "left", listen->interface);
  return err;
}

unsigned long ipv6_hash(const char *pc, unsigned int len) 
{
  unsigned long h = 0;
  while (len-- != 0) {
    h *= 33;
    h += *pc++;
  }
  return (0x3ff & h); /* round to a range of 0-1023 */
}

/* scope is an integer 1-5 */
char *ip6_create_group_addr(const char *service, int scope)
{
  const char *pattern = "ff%02u:0:0:0:0:0:%u:%u";
  
  char *buf;
  unsigned int base_index = 0, group_index=0;
  struct sockaddr_in6 dummy;
  
  if (!strcmp(service, SVRLOC_GRP))
    group_index = 116;
  else if (!strcmp(service, SVRLOC_DA_GRP))
    group_index = 123;
  else {
    base_index = 1;
    group_index = ipv6_hash(service, strlen(service));
  }
  buf = calloc(1, INET6_ADDRSTRLEN + 1);
  snprintf(buf, INET6_ADDRSTRLEN, pattern, scope, base_index, group_index);
  dummy.sin6_scope_id = 0x20;
  inet_pton(AF_INET6, buf, &dummy.sin6_addr);
  inet_ntop(AF_INET6, &dummy.sin6_addr, buf, INET6_ADDRSTRLEN);
  return buf;
}



/*

 From RFC 3111: 
   A SA and a DA MUST join all groups to which a SLPv2 Agent may 
   send a message.  This ensures that the SA or DA will be able to 
   receive all multicast messages.

   option IPV6_JOIN_GROUP or IPV6_LEAVE_GROUP 

*/
int ip6_join_leave_group(struct slp_client *client, 
			 const char *service,  
			 struct slp_net *listen,
			 int option)
{
  char *group_addr;
  struct ipv6_mreq mreq;  
  int err = 1, scope;
  char addr6[INET6_ADDRSTRLEN+1];
  
  SLP_TRACE("ip6 join leave listener binding %s; iface %s; socket %i\n", 
	    listen->binding, listen->interface, listen->sock);
  if (listen->sock < 0) {
    SLP_TRACE("bad socket on listener %s\n", listen->interface);
    return -1;
  }
  
  scope = ip6_mcast_scope((const char *)listen->binding);
  if (scope > 5)
    scope = 5;
  
  group_addr = ip6_create_group_addr(service, scope);
  if (group_addr) {
    inet_pton(AF_INET6, group_addr, &mreq.ipv6mr_multiaddr);
    inet_ntop(AF_INET6, &mreq.ipv6mr_multiaddr, addr6, INET6_ADDRSTRLEN);
    if (! strncmp(listen->interface, "IF_ANY", INET6_ADDRSTRLEN))
      mreq.ipv6mr_interface = 0;
    else
      mreq.ipv6mr_interface = listen->if_index;
    err = setsockopt(listen->sock, IPPROTO_IPV6, option, (char *)&mreq, sizeof(mreq));
    if (err)
    {
          SLP_TRACE("error %s %s on %s scope %i errno %d sock %i\n", 
            option == IPV6_JOIN_GROUP ? "join" : "leave", 
            group_addr, listen->interface, scope, errno, listen->sock);
    }
    else
    {
          SLP_TRACE("%s group %s for service %s on binding %s for interface %s using sock %i\n", 
            option == IPV6_JOIN_GROUP ? "joined" : "left",
            group_addr, service, listen->binding, listen->interface,
            listen->sock);
    }
    free(group_addr);
  }
  return err;
}

/* join the group on all ipv6 interfaces */
/* for each listener:
   join groups using this listener's socket, the correct group for this listener's scope
   e.g., each listener only needs to join for it's interface's scope

*/
void ip6_join_leave_group_all(const char *group, struct slp_client *client, int option)
{
  struct slp_net *listener = client->listeners.next;

  while(!_LSLP_IS_HEAD(listener)) {
    if (listener->addr.saddr.sa_family == AF_INET6 && 
	listener->l.type == SOCK_DGRAM &&
	strcmp(listener->interface, "IF_ANY"))
      ip6_join_leave_group(client, group, listener, option);
    listener = listener->next;
  }
}

struct slp_net *ip4_listener(struct slp_client *client, struct sockaddr_list *iface)
{

  struct  slp_net *l;
  int err;
  
  if (client->ip_version == 4 || client->ip_version == 0) {
    l = (struct slp_net *)calloc(1, sizeof(struct slp_net));
    l->l.type = SOCK_DGRAM;
    l->type = NET_TYPE_RCVFROM;
    l->sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (l->sock < 0) {
      SLP_TRACE("error opening socket: %i\n", errno);
      free(l);
      return NULL;
    } else {
      SLP_TRACE("opened socket %i\n", l->sock);
    }
    
    err = 1;
    setsockopt(l->sock, SOL_SOCKET, SO_REUSEADDR, (char *)&err, sizeof(err));
    if (iface == NULL) {
      l->addr.saddr.sa_family = AF_INET;
      l->addr.sin.sin_port = htons(client->_target_port);
      l->addr.sin.sin_addr.s_addr = htonl(INADDR_ANY);
    } else {
      memcpy(&l->addr.sin, &iface->addr.sin, sizeof(iface->addr.sin));
      l->addr.sin.sin_port = htons(client->_target_port);
    }
    
    inet_ntop(AF_INET, &l->addr.sin.sin_addr.s_addr, l->binding, INET_ADDRSTRLEN);
    SLP_TRACE("target port %d\n", ntohs(l->addr.sin.sin_port));
    SLP_TRACE("listening addr %s\n", l->binding);
    
    err = bind(l->sock, (struct sockaddr *)&l->addr.sin, sizeof(l->addr));
    ip6_trace_socket(l->sock);
    
    if (err < 0) {
      SLP_TRACE("error binding socket - ip4_listener() %d\n", errno);
  
#if defined(_WIN32)
      closesocket(l->sock);
#else
      _LSLP_CLOSESOCKET(l->sock);
#endif
      free(l);
      return NULL;
    }
    if (iface) {
      strncpy(l->interface, iface->name, INET6_ADDRSTRLEN);
    } else {
      strcpy(l->interface, "IF_ANY");
    }
    SLP_TRACE("bound ip4 dgram sock %d\n", l->sock);
    
    _LSLP_INSERT(l, client->listeners.next);
    return l;
  } else
    return NULL;
}

/* use this function to join group, make sure to bind socket to mcast address */
/* have the inverse function leave a group and destory a listener when removing a srvreg */
struct slp_net *ip6_listener(struct slp_client *client, struct sockaddr_list *iface)
{
  struct slp_net *l;
  int err = 1;
  
  if (client->ip_version == 6 || client->ip_version == 0) {
    l = (struct slp_net *)calloc(1, sizeof(struct slp_net));
    l->l.type = SOCK_DGRAM;
    l->type = NET_TYPE_RCVFROM;
    
    l->sock = socket(AF_INET6, SOCK_DGRAM, 0);
    
    if (l->sock < 0) {
      SLP_TRACE("error opening ipv6 socket: %i\n", errno);
      free(l);
      return NULL;
    } else {
      SLP_TRACE("opened ipv6 socket %i\n", l->sock);
    }
    
    setsockopt(l->sock, SOL_SOCKET, SO_REUSEADDR, (char *)&err, sizeof(err));
    setsockopt(l->sock, IPPROTO_IPV6, IPV6_V6ONLY, (char *)&err, sizeof(err));
    l->addr.saddr.sa_family = AF_INET6;
    l->addr.sin6.sin6_port = htons(client->_target_port);

    if (!iface) 
      strcpy(l->binding, "::");
    else {
      inet_ntop(AF_INET6, &iface->addr.sin6.sin6_addr, l->binding, INET6_ADDRSTRLEN);
      inet_pton(AF_INET6, l->binding, &l->addr.sin6.sin6_addr);
      if ( IN6_IS_ADDR_LINKLOCAL(&l->addr.sin6.sin6_addr))
          l->addr.sin6.sin6_scope_id = iface->if_index;
    }
    
    err = bind(l->sock, &l->addr.saddr, sizeof(l->addr));
    if (err) {
      SLP_TRACE("error binding socket %i err %i errno %i binding %s\n", 
		l->sock, err, errno, l->binding);

#if defined(_WIN32)
      closesocket(l->sock);
#else
      _LSLP_CLOSESOCKET(l->sock);
#endif
      free(l);
      return NULL;
      
    } else {
      SLP_TRACE("bound socket - ip6_listener() %d binding %s socket %i\n", 
		errno, l->binding, l->sock);
    }

    ip6_trace_socket(l->sock);
    if (iface) {
      strncpy(l->interface, iface->name, INET6_ADDRSTRLEN);
      l->if_index = iface->if_index;
    } else {
      strcpy(l->interface, "IF_ANY");
      l->if_index = 0;
    }
    
    SLP_TRACE("bound ip6 dgram sock %d\n", l->sock);
    _LSLP_INSERT(l, client->listeners.next);
    SLP_TRACE("target port %d\n", ntohs(l->addr.sin6.sin6_port));
    SLP_TRACE("listening addr %s\n", l->binding);
    SLP_TRACE("device %s\n",l->interface);
    return l;
  } else
    return NULL;
}

void ip6_listen_all_interfaces(struct slp_client *client)
{

  struct sockaddr_list *this_iface = client->local_addr_list.next;
  while (!_LSLP_IS_HEAD(this_iface)) {
    if (this_iface->addr.saddr.sa_family == AF_INET)
      ip4_listener(client, this_iface);
    else if (this_iface->addr.saddr.sa_family == AF_INET6)
      ip6_listener(client, this_iface);
    this_iface = this_iface->next;
  }
    ip4_listener(client, NULL);
    ip6_listener(client, NULL);
}


int ip6_service_one_listener(struct slp_client *client,
			     struct slp_net *listener,
			     struct timeval *wait, 
			     int one_only)
  
{
  struct timeval local_wait = {0,400};
  
  int err = 1;
  fd_set fd;
  SLP_TRACE("service one listener, socket %i\n", listener->sock);
  
  
  /* if we are sending a message to loopback or a local interface, 
     we must process that message before we will ever get a response */
  ip6_service_listeners(client, &local_wait);
  
  do {
    FD_ZERO(&fd);
    FD_SET(listener->sock, &fd);
    SLP_TRACE("selecting...\n");
    
    err = select(FD_SETSIZE, &fd, NULL, NULL, wait);
    if (err > 0 && listener->sock > 0 && FD_ISSET(listener->sock, &fd)) {
      if (listener->l.type == SOCK_DGRAM) {
	SLP_TRACE("select on udp listener\n");
	
	udp_handle_listener(client, listener);
      }
      
      else if (listener->l.type == SOCK_STREAM) {
	SLP_TRACE("select on tcp listener\n");
	tcp_handle_listener(client, listener);
      }
      
      if (one_only)
	break;
    }
    SLP_TRACE("select returned %i\n", err);
    
  } while (err > 0 || err == EINTR);
  return err;
}


int ip6_service_listeners(struct slp_client *client, struct timeval *wait)
{
  struct slp_net *net;
  int err = 0;
  fd_set fd;

  FD_ZERO(&fd);

  net = client->listeners.next;
  while (!_LSLP_IS_HEAD(net)) {
    if (net->sock > 0) {
      FD_SET(net->sock, &fd);
      err++;
    }
    net = net->next;
  }

  net = client->connections.next;
  while (!_LSLP_IS_HEAD(net)) {
    if (net->sock > 0) {
      FD_SET(net->sock, &fd);
      err++;
    }
    net = net->next;
  }
  
  err = select(FD_SETSIZE, &fd, NULL, NULL, wait);
  if (err < 0) {
    SLP_TRACE("Socket err on select: %d\n", errno);
    return err;
  }
  
  net = client->listeners.next;
  while (!_LSLP_IS_HEAD(net)) {
    if (net->sock > 0 && FD_ISSET(net->sock, &fd)) {
      if (net->l.type == SOCK_DGRAM)
	err = udp_handle_listener(client, net);
      else if (net->l.type == SOCK_STREAM)
	err = tcp_handle_listener(client, net);
    }
    net = net->next;
  }
  if (err > 0)
    return TRUE;
  return FALSE;
}


/* add an interface parameter, consolidate socket options 
   inside routine (target_mcast). */

int ip6_send_rcv_udp(struct slp_client *client, 
		     struct sockaddr_list *iface, 
		     int retry)
{
  struct slp_net *net;
  int err = 1, mcast;

  net = (struct slp_net *)calloc(1, sizeof(struct slp_net));
  if (!net)
    return 0;
  _LSLP_INIT_HEAD(net);
  
  net->type = NET_TYPE_SENDTO;
  net->l.type = SOCK_DGRAM;  
  mcast = _LSLP_GETFLAGS(client->_msg_buf);
  mcast &= LSLP_FLAGS_MCAST;
  SLP_TRACE("target family should be %i or %i. family is %i\n", AF_INET, AF_INET6,
	    client->target_addr.saddr.sa_family);

  if (client->target_addr.saddr.sa_family == AF_INET && 
      (client->ip_version == 4 || client->ip_version == 0)) {
    net->addr.saddr.sa_family = AF_INET;
    net->sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (net->sock < 0)
      goto sock_failure;
    
    setsockopt(net->sock, SOL_SOCKET, SO_BROADCAST, (char *)&err, sizeof(err));
    err = 1;
    setsockopt(net->sock, SOL_SOCKET, SO_REUSEADDR, (char *)&err, sizeof(err));
    net->addr.sin.sin_port = htons(0);
    if (mcast && iface)
      inet_pton(AF_INET, iface->addr6, &net->addr.sin.sin_addr); 
    else 
      net->addr.sin.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(net->sock, &net->addr.saddr, sizeof(net->addr));
    if (mcast && iface) {
      err = setsockopt(net->sock, IPPROTO_IP, IP_MULTICAST_IF, 
		       (char *)&net->addr.sin.sin_addr.s_addr, sizeof(struct in_addr));
      SLP_TRACE("set ipv multicast interface: %d %d\n", err, errno);
      
    }
    
  } else if (client->target_addr.saddr.sa_family == AF_INET6 &&
	     (client->ip_version == 6 || client->ip_version == 0)) {
    net->addr.saddr.sa_family = AF_INET6;
    net->sock = socket(AF_INET6, SOCK_DGRAM, 0);
    if (net->sock < 0)
      goto sock_failure;

    net->addr.sin6.sin6_port = htons(0);
    if (mcast && iface) {
      inet_pton(AF_INET6, iface->addr6, &net->addr.sin6.sin6_addr);  
      if ( IN6_IS_ADDR_LINKLOCAL(&net->addr.sin6.sin6_addr))
	net->addr.sin6.sin6_scope_id = iface->if_index;
    }
    else
      inet_pton(AF_INET6, "::", &net->addr.sin6.sin6_addr);
    setsockopt(net->sock, SOL_SOCKET, SO_BROADCAST, (char *)&err, sizeof(err));
    err = 1;
    setsockopt(net->sock, SOL_SOCKET, SO_REUSEADDR, (char *)&err, sizeof(err));
    bind(net->sock, &net->addr.saddr, sizeof(net->addr));
    if (mcast && iface) {
      mcast = iface->if_index;
      err = setsockopt(net->sock, IPPROTO_IPV6, IPV6_MULTICAST_IF, (char *)&mcast, sizeof(mcast));
      SLP_TRACE("set ipv6 multicast interface: %d %d\n", err, errno);
    }
  } else {
    SLP_TRACE("Protocol settings don't match target address\n");
    return -1;
  }

  ip6_trace_socket(net->sock);

  err = sendto(net->sock, client->_msg_buf, 
	       _LSLP_GETLENGTH(client->_msg_buf), 
	       0, &client->target_addr.saddr, 
	       sizeof(client->target_addr));
  
  SLP_TRACE("sent datagram %d bytes on sock %d errno %d\n", err, net->sock, errno); 
  net->type = NET_TYPE_RCVFROM;
   
  {
    int one_only = 0;
    struct timeval wait = {1,0};
    if (!mcast)
      one_only = 1;
    err = ip6_service_one_listener(client, net, &wait, one_only);
  }
   
  lslpDestroyConnection(net);
  return err;

 sock_failure:
  free(net);
  return -1;
}


int is_srvtype_registered(struct slp_client *client, const char *srvtype)
{
  struct lslp_srv_reg_instance *reg;
  reg = client->regs.next;
  while (! _LSLP_IS_HEAD(reg)) {
    if (TRUE == lslp_pattern_match(srvtype, reg->srvType, TRUE))
      return 1;
    reg = reg->next;
  }
  return 0;
}


void ip6_insert_reg(struct slp_client *client, struct lslp_srv_reg_instance *reg)
{
  if (!is_srvtype_registered(client, reg->srvType))
    ip6_join_leave_group_all(reg->srvType, client, IPV6_JOIN_GROUP);
  _LSLP_INSERT(reg, client->regs.next);
}

void ip6_remove_reg(struct slp_client *client, struct lslp_srv_reg_instance *reg)
{
  
  _LSLP_UNLINK(reg);
  if (!is_srvtype_registered(client, reg->srvType))
    ip6_join_leave_group_all(reg->srvType, client, IPV6_LEAVE_GROUP);
}


void ip4_bcast_req(struct slp_client *client, int retry, 
		      const char *one, 
		      const char *two, 
		      const char *three,
		      int (*prepare)(struct slp_client *,
				     unsigned short int,
				     const char *, 
				     const char *, 
				     const char *))
{
  struct sockaddr_list *l;
  struct sockaddr *saddr;
  
  l = client->local_addr_list.next;
  while (!_LSLP_IS_HEAD(l)) {
    saddr = (struct sockaddr *)&l->addr;
    if (saddr->sa_family == AF_INET  && (l->flags & IFF_BROADCAST)) {
      SLP_TRACE("Broadcasting on %s to %s\n", l->name, l->addr6);
      client->target_addr.sin.sin_port = htons(client->_target_port);
      client->target_addr.saddr.sa_family = AF_INET;
      client->target_addr.sin.sin_addr.s_addr = l->bcast_addr.sin.sin_addr.s_addr;
      _LSLP_SETFLAGS(client->_msg_buf, LSLP_FLAGS_MCAST);
      ip6_send_rcv_udp(client, l, retry);
      prepare( client, client->_xid, one, two, three);
    }
    l = l->next;
  }
  return;
}

/* 0 == ipv4
 * 1 == node scope
 * 2 == link scope
 * 5 == site scope
 * 0x0e == global scope
 * http://www.faqs.org/rfcs/rfc2375.html
  
 http://www.faqs.org/rfcs/rfc2373.html
   scop is a 4-bit multicast scope value used to limit the scope of
      the multicast group.  The values are:

         0  reserved
         1  node-local scope
         2  link-local scope
         3  (unassigned)
         4  (unassigned)
         5  site-local scope
         6  (unassigned)
         7  (unassigned)
         8  organization-local scope
         9  (unassigned)
         A  (unassigned)
         B  (unassigned)
         C  (unassigned)

         D  (unassigned)
         E  global scope
         F  reserved
	 
	 IN6_IS_ADDR_LINKLOCAL(a) ret 2
	 IN6_IS_ADDR_SITELOCAL(a) ret 5
	 IN6_IS_ADDR_V4MAPPED(a)  ret 0x0e
	 IN6_IS_ADDR_UNSPECIFIED(a) ret 0
	 IN6_IS_ADDR_V4COMPAT     ret 0x0e

 */

/* NOTE: multicasts are limited to site scope */
int ip6_mcast_scope(const char *address)
{
  struct sockaddr_in6 sin6;
  memset(&sin6, 1, sizeof(sin6.sin6_addr));
  inet_pton(AF_INET6, address, &sin6.sin6_addr);
  
  if (IN6_IS_ADDR_LOOPBACK(&sin6.sin6_addr))
    return 1;
  if (IN6_IS_ADDR_LINKLOCAL(&sin6.sin6_addr))
    return 2;
  if (IN6_IS_ADDR_SITELOCAL(&sin6.sin6_addr))
    return 5;
  if (IN6_IS_ADDR_MC_ORGLOCAL(&sin6.sin6_addr))
    return 8;
  if (IN6_IS_ADDR_MC_GLOBAL(&sin6.sin6_addr) || 
      IN6_IS_ADDR_V4MAPPED(&sin6.sin6_addr) || 
      IN6_IS_ADDR_V4COMPAT(&sin6.sin6_addr))
    return 0x0e;
  if (IN6_IS_ADDR_UNSPECIFIED(&sin6.sin6_addr))
    return 0;
  return 0x0e;
}


void ip6_mcast_req(struct slp_client *client, int retry, 
		      const char *one, 
		      const char *two, 
		      const char *three,
		      int (*prepare)(struct slp_client *,
				     unsigned short int,
				     const char *, 
				     const char *, 
				     const char *))
{
  struct sockaddr_list *l;
  struct sockaddr *saddr;
  struct sockaddr_in6 *sin6;
  
  int srvreq, scope;
  char *name_buf = NULL;
  char binding[INET6_ADDRSTRLEN + 1];
  
  if (prepare == prepare_query)
    srvreq = 1;
  else
    srvreq = 0;
  SLP_TRACE("mcast_req parameters: one %s two %s three %s\n", one, two, three);
  
  client->target_addr.sin6.sin6_port = htons(client->_target_port);
  l = client->local_addr_list.next;
  while (!_LSLP_IS_HEAD(l)) {
    SLP_TRACE("convergence round for %s\n", l->name);
    saddr = (struct sockaddr *)&l->addr;
    
    sin6 = (struct sockaddr_in6 *)&l->addr;
    if (saddr->sa_family == AF_INET6 && 
	(client->ip_version == 6 || client->ip_version == 0)) {
      inet_ntop(AF_INET6,
		&sin6->sin6_addr,
		binding,
		INET6_ADDRSTRLEN);
            
      scope = ip6_mcast_scope(binding);
      if (scope > 5)
	scope = 5;
      
      if (srvreq && !ip6_is_wildcard(one))
	name_buf = ip6_create_group_addr(one, scope);
      else
	name_buf = ip6_create_group_addr(SVRLOC_GRP, scope);
      client->target_addr.saddr.sa_family = AF_INET6;
      inet_pton(AF_INET6, name_buf, &client->target_addr.sin6.sin6_addr);
    } else if (saddr->sa_family == AF_INET && 
	       (client->ip_version == 4 || client->ip_version == 0)) {
      client->target_addr.saddr.sa_family = AF_INET;
      inet_pton(AF_INET,"239.255.255.253", &client->target_addr.sin.sin_addr);
    } else
      goto next_iface;
    _LSLP_SETFLAGS(client->_msg_buf, LSLP_FLAGS_MCAST) ;
    ip6_send_rcv_udp(client, l, retry);
    prepare( client, client->_xid, one, two, three);
next_iface:
    l = l->next;
  }
  if (name_buf != NULL)
    free(name_buf);
  return;
}


void ip6_converge_req(struct slp_client *client,
		      const char *one, 
		      const char *two, 
		      const char *three,
		      int (*prepare)(struct slp_client *,
				     unsigned short int,
				     const char *, 
				     const char *, 
				     const char *))
  
{
  int convergence;
  union slp_sockaddr target_save;
  convergence = client->_convergence ? client->_convergence : 3;
  prepare( client, client->_xid + 1, one, two, three);
  
  memcpy(&target_save, &client->target_addr, sizeof(target_save));
  if (convergence) {
    ip6_mcast_req(client, FALSE, one, two, three, prepare);
    if (client->broadcast)
      ip4_bcast_req(client, FALSE, one, two, three, prepare);
  } else
    ip6_send_rcv_udp(client, NULL, TRUE);
  
  while(--convergence > 0) {

    ip6_mcast_req(client, FALSE, one, two, three, prepare);
    if (client->broadcast)
      ip4_bcast_req(client, FALSE, one, two, three, prepare);
  } 
  memcpy(&client->target_addr, &target_save, sizeof(client->target_addr));
  return;
}

void ip6_unicast_req(struct slp_client *client,
		     const char *one, 
		     const char *two, 
		     const char *three,
		     int (*prepare)(struct slp_client *,
				    unsigned short int,
				    const char *, 
				    const char *, 
				    const char *))
{
  int mcast = 0;
  
  int retry = client->_retries;
  if (client->target_addr.saddr.sa_family == AF_INET) {
    if (client->ip_version != 4 && client->ip_version != 0) {
      SLP_TRACE("Client IP protocol settings are inconsistent with target address\n");
      return;
    }
    if (client->target_addr.sin.sin_addr.s_addr  == _LSLP_MCAST || 
	client->target_addr.sin.sin_addr.s_addr  == _LSLP_LOCAL_BCAST)
      mcast = 1;
  } else if (client->target_addr.saddr.sa_family == AF_INET6) {
    if (client->ip_version != 6 && client->ip_version != 0) {
      SLP_TRACE("Client IP protocol settings are inconsistent with target address\n");
      return;
    }
    if (IN6_IS_ADDR_MULTICAST(&client->target_addr.sin6.sin6_addr))
      mcast = 1;
  }
  prepare(client, (retry == TRUE) ? client->_xid : client->_xid + 1, 
	  one, two, three);
  if (mcast) 
    _LSLP_SETFLAGS(client->_msg_buf, LSLP_FLAGS_MCAST);
  ip6_send_rcv_udp(client, NULL, retry);
  while (--retry && _LSLP_IS_EMPTY(&client->replies)) {
    prepare(client, client->_xid, one, two, three);
    ip6_send_rcv_udp(client, NULL, retry);
  }
}


int ip6_reply(struct slp_client *client, union slp_sockaddr *remote, int len)
{
  int sock, bytes = 1;
  union slp_sockaddr saddr;
  memset(&saddr, 0x00, sizeof(saddr));
  
  if (remote->saddr.sa_family == AF_INET) {
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    saddr.saddr.sa_family = AF_INET;
    saddr.sin.sin_port = htons(client->_target_port);
    inet_pton(AF_INET, "0.0.0.0", &saddr.sin.sin_addr.s_addr);
  }
  else {
    sock = socket(AF_INET6, SOCK_DGRAM, 0);
    saddr.saddr.sa_family = AF_INET6;
    saddr.sin.sin_port = htons(client->_target_port);
    inet_pton(AF_INET6, "::", &saddr.sin6.sin6_addr);
  }
  if (sock < 0)
    return -1;
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&bytes, sizeof bytes);
  bind(sock, (struct sockaddr *)&saddr, sizeof(union slp_sockaddr));
  SLP_TRACE("reply length is %d\n", len);
  SLP_TRACE("reply flags are %i\n", _LSLP_GETFLAGS(client->_msg_buf));
  
  bytes = sendto(sock, client->_msg_buf, len, 0, &remote->saddr, sizeof(*remote));
#if defined(_WIN32)
  closesocket(sock);
#else
  _LSLP_CLOSESOCKET(sock);
#endif
  
  SLP_TRACE("sent %d bytes of response data\n", bytes);
  return bytes;
}


/* ipv6 urls bracket ip addresses '[' ']', and the pattern match */
/* uses brackets as control characters, so we need to escape brackets */
char *ip6_escape_url(const char *url)
{
  char *escape,*e;
  const char *u;
  
  int url_len, escape_len, escape_size = 0;
  
  url_len = strlen(url) + 1;
  escape_len = url_len + 100;
  escape = (char *)calloc(escape_len, sizeof(char));
  if (!escape)
    return NULL;
  
  u = url;
  e = escape;
  
  while(escape_size < escape_len - 2) {
    if (*u == '[' || *u == ']') {
      *e++ = '\\';
      escape_size++;
    }
    
    *e++ = *u++;
    escape_size++;
    
    if (*(u - 1) == '\0')
      break;
    if (u == url + url_len) {
      *e = '\0';
      break;
    }

    if (escape_size >=  escape_len - 2) {
      escape_len += 100;
      escape = (char *)realloc(escape, escape_len);
      if (!escape)
	return NULL;
      e = escape + escape_size;
    }
  }
  return escape;
}

/* strip the brackets and isolate an ipv6 address from a url */
/* "//mdday@[FEDC:BA98:7654:3210:FEDC:BA98:7654::3210]:427" */
char *ip6_strip_url_addr(const char *addr)
{
  const char *start, *end;
  char *stripped;
  
  start = addr + strspn(addr, "/[");
  end = strrchr(start, ']');
  if (!end)
    end = strchr(addr, '\0');
  stripped = (char *)calloc((end - start) + 1, sizeof(char));
  if (stripped)
    memcpy(stripped, start, end - start);
  SLP_TRACE("addr: %s; stripped %s\n", addr, stripped);
  
  return stripped;
}

int ip6_escaped_pattern_match(const char *a, const char *b, int caseless)
{
  int err = -1;
  
  char *tmp_a, *tmp_b;
  tmp_a = ip6_escape_url(a);
  tmp_b = ip6_escape_url(b);

 
  if (tmp_a && tmp_b) {
    err = lslp_pattern_match(tmp_a, tmp_b, caseless);
  }
  
  if (tmp_a)
    free(tmp_a);
  if (tmp_b)
    free(tmp_b);
  return err;
}



int ip6_is_wildcard(const char *buf)
{
  int pos = strcspn(buf, "?*[]");

  SLP_TRACE("searching for wildcards in %s\n", buf);
  
  if (pos == strlen(buf)) {
    SLP_TRACE("no wildcards found in %s\n", buf);
    return 0;
  }
  SLP_TRACE("found wildcard %c in %s\n", buf[pos], buf);
  
  return 1;
}

DLL_EXPORT int ip6_addr_type(const char *addr)
{
  if (!addr)
    return 0;
  int len = strlen(addr);
  if (len > INET_ADDRSTRLEN || len < 7)
    return 6;
  if (strchr(addr, ':'))
    return 6;
  return 4;
}

DLL_EXPORT int ip6_is_loopback(const char *addr)
{
  struct sockaddr_in6 sin6;
  
  if (ip6_addr_type(addr) == 4) {
    if (inet_addr(addr) == INADDR_LOOPBACK)
      return 1;
    else
      return 0;
  }
     
  if (ip6_addr_type(addr) == 6) {
    inet_pton(AF_INET6, addr, &sin6.sin6_addr);
    if (IN6_IS_ADDR_LOOPBACK(&sin6.sin6_addr))
      return 1;
    return 0;
  }
  return 0;
}

/* "::", "*" ->  "ff05:0:0:0:0:0:0:116" 
   "localhost" -> "::1"
   error -> NULL
*/


DLL_EXPORT char *ip6_normalize_addr(const char *addr, int ip)
{
  if (!strcmp(addr, "::"))
    return(strdup("ff05:0:0:0:0:0:0:116"));
  if (ip == 6 || ip == 0) {
    if (!strcmp(addr, "*")) 
      return(strdup("ff05:0:0:0:0:0:0:116"));
    if (!strcmp(addr, "localhost") || !strcmp(addr, "loopback"))
      return(strdup("::1"));
  }
  
  if (ip == 4) {
    if (!strcmp(addr, "*"))
      return(strdup("239.255.255.253"));
    if (!strcmp(addr, "localhost") || !strcmp(addr, "loopback"))
      return(strdup("127.0.0.1"));
  }
  return NULL;  
}


/*
 * Local variables:
 *  c-file-style: "gnu"
 * End:
 */
    
