
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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

char *opq_output_buf;

int remaining;
int opaquelex(void);
int opq_EncodeOpaque(char *in, int inSize, char **out, int *outSize);
int opq_DecodeOpaque(char *in, int inSize, char **out, int *outSize);

%}
Digit			[0-9]
Alpha			[a-zA-Z]
Hex				[a-fA-F]
hexdigit		[0-9a-fA-F]
escchars		[fF]

%option noyywrap
%%

"\\"({escchars}{2})({hexdigit}{2}) {
						if (remaining >= 1)
						{
							*opq_output_buf = (char)strtoul(&opaquetext[3], NULL, 16);
							opq_output_buf++;
							remaining--;
						}
						else
							return(-1);
					}

.	 				{
							return(-1);
					}

%%

int opq_EncodeOpaque(char *in, int inSize, char **out, int *outSize)
{

	int ccode = -1;
	assert(in != NULL);
	assert(inSize);
	assert(out != NULL);
	assert(*out == NULL);
	assert(outSize != NULL);

	*outSize = 1 + (inSize * 5);  /* encoding expands buf by 5x */
	remaining = *outSize - 1;
	if (NULL != (*out = (char *)calloc(*outSize, sizeof(char))))
	{
		/* leave room for the size word to be stuffed into the buffer */
		opq_output_buf = (*out) + 2;
		while (inSize && *in != 0x00)
		{
			sprintf(opq_output_buf, "\\ff%.2x", *(unsigned char *)in++); 
			opq_output_buf += strlen(opq_output_buf);
			inSize--;
		}
	}
	return(ccode);
}	

int opq_DecodeOpaque(char *in, int inSize, char **out, int *outSize)
{
	int ccode = -1;
        YY_BUFFER_STATE lexer = 0;
	assert(in != NULL);
	assert(inSize);
	assert(out != NULL);
	assert(*out == NULL);
	assert(outSize != NULL);

	*outSize = 2 + (inSize / 5);  /* decoding compresses by a factor of 5 */
	remaining = *outSize - 1;
	if (NULL != (*out = (char *)calloc(*outSize, sizeof(char))))
	{

		char *temp = (char *)calloc(inSize + 2, sizeof(char));
		if (temp != NULL)
		{
			memcpy(temp, in, inSize);  /* guarantee a null-terminated in buf */
                        if(0 != (lexer = opaque_scan_buffer(temp, inSize + 2))) {
                          opq_output_buf = *out;
			  ccode = opaquelex();
			  if (ccode < 0)
			  {
				free(*out);
				*out = NULL;
			  }
			  free(temp);
                          opaque_delete_buffer(lexer);
                        }
		}
	}
	return(ccode);
}	
/*
int main(int argc, char *argv[])
{
	int outS = 0;
	char *out = NULL;
	char in1[] = 
	{
		'\\', 'f', 'f', 'a', 'b', '\\', 'f', 'f', '0', 'd', 0x00
	};

	char in2[] = 
	{
		0x01, 0x45, 0xfe, 0xc1, 0x45, 0x00
	};

	opq_DecodeOpaque(&in1[0], sizeof(in1), &out, &outS);
	if (out != NULL)
	{
		free(out);
		out = NULL;
	}
	opq_EncodeOpaque(&in2[0], sizeof(in2), &out, &outS);

	return(0);
}	

*/

