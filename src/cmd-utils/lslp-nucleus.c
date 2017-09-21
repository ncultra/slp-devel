
#include "lslp-nucleus.h"

unsigned int errno;


NU_HOSTENT * nucleus_gethostbyname(const char *name)         //jeb
{
  static NU_HOSTENT name_host_entry;
  
  if (NU_SUCCESS == NU_Get_Host_By_Name((char*)name, &name_host_entry))   //jeb
    return &name_host_entry;
  return NULL;
}

NU_HOSTENT * nucleus_gethostbyaddr(const void *addr, int len, int type)   //jeb
{
  static NU_HOSTENT addr_host_entry;

  if ( type != NU_FAMILY_IP || len != 4 )
    return NULL;
  if(NU_SUCCESS == NU_Get_Host_By_Addr((char *)addr, len, type, &addr_host_entry))
    return &addr_host_entry;
  return NULL;
}
            

/************************************************************************
*                                                                       
*   FUNCTION                                                              
*
*       inet_ntoa                                      
*
*   DESCRIPTION
*
*       This function transforms a UINT32 IP address into a string of
*       the form xxx.xxx.xxx.xxx.
*                                                                       
*   INPUTS                                                                
*
*       in      The data structure containing the UINT32 IP address.
*                                                                       
*   OUTPUTS                                                               
*
*       A pointer to the new string.
*                                                                       
*************************************************************************/
char *inet_ntoa(struct in_addr in)
{
    static      char    b[18];
    register    unsigned char   *p;
    CHAR        temp[3];
    INT         i, j;
    
    memset(b, 0, 18);
    p = (UINT8 *)&in;

    for (i = 0; i <= 3; i++)
    {
        if (p[i] == 0)
        {
            for (j = 0; j < 3; j++)
                strcat(b, (CHAR*)(NU_ITOA((INT)(p[j]), temp, 10)));
        }
        else    
            strcat(b, (CHAR*)(NU_ITOA((INT)(p[i]), temp, 10)));

        if (i != 3)
            strcat(b, ".");
    }

    return (b);
}

/************************************************************************
*                                                                       
*   FUNCTION                                                              
*
*       assert                                      
*                                                                       
*************************************************************************/
//#ifdef SLPDBG
//void assert(int expr)
//{
//  if (!expr)
//       _system_info(TRUE,"ASSERT:%s/%d", __FILE__, __LINE__);
////_system_info(TRUE,"ASSERT:%s/%d\n", __FILE__, __LINE__); HALT;}
//}
//#endif
