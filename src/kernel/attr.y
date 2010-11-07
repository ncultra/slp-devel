
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




%{
#include "lslp-common-defs.h"
#include "attr.h"

#ifndef TEST_HARNESS

#else
#undef vs_free
#define vs_free free
#endif /* test harness */

/* prototypes and globals go here */

int32 attrparse(void);
uint32 heapHandle;
BOOL bt = TRUE, bf = FALSE;
void attr_close_lexer(uint32 handle);
uint32 attr_init_lexer(int8 *s);

lslpAttrList attrHead = 
{
	&attrHead, &attrHead, TRUE, 0, NULL, NULL, head

};

lslpAttrList inProcessAttr = 
{
	&inProcessAttr, &inProcessAttr, TRUE, 0, NULL, NULL, head
};

lslpAttrList inProcessTag = 
{
	&inProcessTag, &inProcessTag, TRUE, 0, NULL, NULL, head
};


%}

/* definitions for ytab.h */
%name-prefix="attr"

%union {
	int32 _i;
	int8 *_s;
	lslpAttrList *_atl;
}

%token<_i> _TRUE _FALSE _MULTIVAL _INT 
%token<_s> _ESCAPED _TAG _STRING 

/* typecast the non-terminals */

/* %type <_i> */
%type <_atl> attr_list attr attr_val_list attr_val

%%

attr_list: attr {
			while (! _LSLP_IS_HEAD(inProcessAttr.next))
			{
				$$ = inProcessAttr.next;
				_LSLP_UNLINK($$);
				_LSLP_INSERT_BEFORE($$, &attrHead);
			}
		/* all we really want to do here is link each attribute */
		/* to the global list head. */
		}
	| attr_list ',' attr {
		/* both of these non-terminals are really lists */
		/* ignore the first non-terminal */
			while (! _LSLP_IS_HEAD(inProcessAttr.next))
			{
				$$ = inProcessAttr.next;
				_LSLP_UNLINK($$);
				_LSLP_INSERT_BEFORE($$, &attrHead);
			}
		}
	;

attr: _TAG 	{
			$$ =  lslpAllocAttr($1, tag, NULL, 0);
			if (NULL != $$)
			{
				_LSLP_INSERT_BEFORE($$, &inProcessAttr);
			}
		}
	| '(' _TAG ')' 	{
			$$ =  lslpAllocAttr($2, tag, NULL, 0);
			if (NULL != $$)
			{
				_LSLP_INSERT_BEFORE($$, &inProcessAttr);
			}
		}
        | '(' _TAG '=' ')' {
  			$$ =  lslpAllocAttr($2, tag, NULL, 0);
			if (NULL != $$)
			{
				_LSLP_INSERT_BEFORE($$, &inProcessAttr);
			}
               }

	| '(' _TAG '=' attr_val_list ')' {
			$$ = inProcessTag.next;
			while (! _LSLP_IS_HEAD($$))
			{
				$$->name = strdup($2); 
				_LSLP_UNLINK($$);
				_LSLP_INSERT_BEFORE($$, &inProcessAttr);
				$$ = inProcessTag.next;
			}
		}
	;

attr_val_list: attr_val {

			if(NULL != $1)
			{
				_LSLP_INSERT($1, &inProcessTag);
			}
		}
	| attr_val_list _MULTIVAL attr_val {
			if (NULL != $3)
			{
				_LSLP_INSERT_BEFORE($3, &inProcessTag);
			}
		}
	;
attr_val: _TRUE {
			$$ = lslpAllocAttr(NULL, bool_type,  &bt, sizeof(BOOL));
		}
	|     _FALSE {
			$$ = lslpAllocAttr(NULL, bool_type,  &bf, sizeof(BOOL));
		}
	|     _ESCAPED {
			$$ = lslpAllocAttr(NULL, opaque, $1, (int16)(strlen($1) + 1));
		}
	|	  _STRING {
	                     if(strlen($1) > 5 ) {
				if( *($1) == '\\' && ((*($1 + 1) == 'f') || (*($1 + 1) == 'F')) &&  ((*($1 + 2) == 'f') || (*($1 + 2) == 'F'))) {
				       $$ = lslpAllocAttr(NULL, opaque, $1, (int16)(strlen($1) + 1));
                                     } else {
				       $$ = lslpAllocAttr(NULL, string, $1, (int16)(strlen($1) + 1));
				     }
                                  }
			     else {
			       
			       $$ = lslpAllocAttr(NULL, string, $1, (int16)(strlen($1) + 1));
			     }
               } 

	|     _INT {
			$$ = lslpAllocAttr(NULL, integer, &($1), sizeof(int32));
		}
	;
	
%%

void _lslpInitInternalAttrList(void)
{
	attrHead.next = attrHead.prev = &attrHead;
	attrHead.isHead = TRUE;
	inProcessAttr.next =  inProcessAttr.prev = &inProcessAttr;
	inProcessAttr.isHead = TRUE;
	inProcessTag.next =  inProcessTag.prev = &inProcessTag;
	inProcessTag.isHead = TRUE;
	return;
}	

lslpAttrList *_lslpDecodeAttrString(int8 *s)
{
#ifndef TEST_HARNESS
  extern LSLP_SEM_T attrParseSem;
#endif
  uint32 lexer = 0, ccode = LSLP_WAIT_OK;
  lslpAttrList *temp = NULL;
  assert(s != NULL);
#ifndef TEST_HARNESS
  _LSLP_WAIT_SEM(attrParseSem, 1000, &ccode);
#endif
  assert(ccode == LSLP_WAIT_OK);
  if(ccode == LSLP_WAIT_OK) {
    _lslpInitInternalAttrList();
    if (s != NULL) {
      if(NULL != (temp = lslpAllocAttrList()))  {
	if ((0 != (lexer = attr_init_lexer( s))) &&  attrparse()) {
	  lslpFreeAttrList(temp, TRUE);
	  while (! _LSLP_IS_HEAD(inProcessTag.next))  {
	    temp = inProcessTag.next;
	    _LSLP_UNLINK(temp);
	    lslpFreeAttr(temp);
	  }
	  while (! _LSLP_IS_HEAD(inProcessAttr.next))  {
	    temp = inProcessAttr.next;
	    _LSLP_UNLINK(temp);
	    lslpFreeAttr(temp);
	  }
	  while (! _LSLP_IS_HEAD(attrHead.next))  {
	    temp = attrHead.next;
	    _LSLP_UNLINK(temp);
	    lslpFreeAttr(temp);
	  }
	  attr_close_lexer(lexer);
#ifndef TEST_HARNESS
	  _LSLP_SIGNAL_SEM(attrParseSem);
#endif
	  return(NULL);
	}
	if (! _LSLP_IS_EMPTY(&attrHead)) {
	  _LSLP_LINK_HEAD(temp, &attrHead);
	}
	if(lexer != 0) 
	  attr_close_lexer(lexer);
      }
    }
#ifndef TEST_HARNESS
    _LSLP_SIGNAL_SEM(attrParseSem);
#endif
  }
  return(temp);
}

#ifdef TEST_HARNESS
#define NUM_STRINGS 2
int8 *attrStrings[] = 
{
  " management-module, (enclosure-serial-number=23ad8957febaa), (web-url=http://chassis\\5fone/index.html)",
	"load-balanced, (current-load = 450), (recent-loads = 35, 480, 9785, 28)",
	"(enclosed-tag)",
	"(apache = true), (iis = faLsE), (Bill Gates = \\e4\\98\\fe, monopolist)",
	"baetis, caddis, pmd, salmonfly, (fishing = 10)",
	"error, (iis = ), (apache = , , , , ) 45"
};



lslpAttrList *lslpAllocAttr(int8 *name, int8 type, void *val, int16 len)
{	
  lslpAttrList *attr;
  if (NULL != (attr = (lslpAttrList *)calloc(1, sizeof(lslpAttrList))))
    {
      if (name != NULL)
	{
	  if (NULL == (attr->name = strdup(name)))
	    {
	      free(attr);
	      return(NULL);
	    }
	}
      attr->type = type;
      if (type == head)	/* listhead */
	return(attr);
      if (val != NULL)
	{
	  switch (type)	    {
	    case string:
	      if ( NULL != (attr->val.stringVal = strdup((uint8 *)val)))
		return(attr);
	      break;
	    case integer:
	      attr->val.intVal = *(uint32 *)val;
	      break;
	    case bool:
	      attr->val.boolVal = *(BOOL *)val;
	      break;
#ifndef TEST_HARNESS
	    case opaque:
	      if (len > 0)
		{
		  int encLen;
		  opq_EncodeOpaque(val, len, (int8 **)&(attr->val.opaqueVal), &encLen);
		  if (NULL != attr->val.opaqueVal)
		    {
		      /* first two bytes contain length of attribute */
		      _LSLP_SETSHORT(((int8 *)attr->val.opaqueVal), encLen, 0 );
		    }
		}
	      break;
#endif
	    default:
	      break;
	    }
	}
    }
  return(attr);
}	

lslpAttrList *lslpAllocAttrList(void)
{
  lslpAttrList *temp;
  if (NULL != (temp = lslpAllocAttr(NULL, head, NULL, 0)))
    {
      temp->next = temp->prev = temp;
      temp->isHead = TRUE;	
    }
  return(temp);
}	

/* attr MUST be unlinked from its list ! */
void lslpFreeAttr(lslpAttrList *attr)
{
  assert(attr != NULL);
  if (attr->name != NULL)
    free(attr->name);
  if (attr->type == string && attr->val.stringVal != NULL)
    free(attr->val.stringVal);
  else if (attr->type == opaque && attr->val.opaqueVal != NULL)
    free(attr->val.opaqueVal);
  free(attr);
}	


/* attr MUST be unlinked from its list ! */
void lslpFreeAttr(lslpAttrList *attr)
{
  assert(attr != NULL);
  if (attr->name != NULL)
    free(attr->name);
  if (attr->type == string && attr->val.stringVal != NULL)
    free(attr->val.stringVal);
  else if (attr->type == opaque && attr->val.opaqueVal != NULL)
    free(attr->val.opaqueVal);
  free(attr);
}	


int main(int argc, char *argv[])
{
	int8 i;
	lslpAttrList *head, *temp;

	for (i = 0; i < NUM_STRINGS; i++)
	{
		printf("input: %s\n", attrStrings[i]);
		head = _lslpDecodeAttrString(attrStrings[i]);
		if (NULL != head)
		{
			temp = head->next;
			while (! _LSLP_IS_HEAD(temp))
			{
				printf("output: %s", temp->name);
				if (temp->type == integer)
					printf(" = %u", temp->val.intVal);
				else  if (temp->type == bool)
  						printf(" = %s", temp->val.intVal ? "true" : "false");
				else  if(temp->type == string || temp->type == opaque)
						printf(" = %s", temp->val.stringVal);
				printf(", ");
				temp = temp->next;
			}
			lslpFreeAttrList(head, TRUE);
			printf("\n");
		}
	}
	return(0);
}	

#endif

