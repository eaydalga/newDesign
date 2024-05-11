/*  
    
  The MEDICI Electronic Data Interchange Library
  Copyright (C) 2002  David Coles
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
  
*/

/*

  This program will display an EDI message in the following format:
  
  > <interchange>
  >     [UNB]=[UNOA][1]+[7349734757][12]+[5033075000007][14]+[990...
  >     <functionalgroup>
  >         [UNG]=[ORDERS]+[7349734757][12]+[5033075000007][14]+[99062...
  >         <transaction>
  >             [UNH]=[00000000035773]+[ORDERS][D][93A][UN]
  >             [BGM]=[220]
  >             [DTM]=[4][990621][101]
                ...
  >             [CNT]=[2][45]
  >             [UNT]=[35]+[00000000035799]
  >         [UNE]=[27]+[00000000000627]
  >     [UNZ]=[1]+[00000000000627]

  Where indentation denotes structure.
  
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <medici.h>


void starthandler (void *, edi_event_t, EDI_Parameters);
void endhandler (void *, edi_event_t, EDI_Parameters);
void segmenthandler (void *, EDI_Parameters, EDI_Segment, EDI_Directory);
void warninghandler (void *, int);

int
main (int argc, char **argv)
{
  EDI_Parser parser;
  FILE *stream = stdin;
  char buffer[4096];
  int done, userdata = 0;
  size_t length;

  /* If we were given a filename argument use that instead of stdin */

  if (argc > 1)
    if (!(stream = fopen (argv[1], "r")))
      {
	perror ("Couldn't open file");
	return 1;
      }

  /* First, create the parser  */

  if (!(parser = EDI_ParserCreate ()))
    {
      /* If this failed it's almost certainly because a malloc() failed */

      perror ("Couldn't create parser");
      return 1;
    }


  /* Set the userdata argument which will be passed to handlers */
  /* each time they are called. We will use this to indent output */

  EDI_SetUserData (parser, &userdata);

  /* Tell the parser to relax the rules on characters which are not */
  /* supposed to be allowed in the message - sometimes service advice */
  /* strings contain verboten characters such as newline */

  /* FIXME - make these activated by command line switches */
  /* EDI_SetPragma (parser, EDI_PCHARSET); */

  /* We want to be alerted if a error is handled pragmatically */
  /* so that we can display a warning message on stderr */

  EDI_SetWarningHandler (parser, warninghandler);

  /* Set up handlers which will indent output */

  EDI_SetStartHandler (parser, starthandler);
  EDI_SetEndHandler (parser, endhandler);

  /* When we encounter a full segment we want to be able to print out */
  /* the contents, so we set a handler for system and message segments */

  EDI_SetSegmentHandler (parser, segmenthandler);


  /* Tell the parser how to find out about transaction set directories */
  /* This allows it to understand the structure of the transaction */

  /* FIXME - temporarily removed due to changes in directory handling */
  /* EDI_SetDirectoryHandler (parser, EDI_DirectoryHandler); */

  /* Now we can start feeding the stream to MEDICI */

  do
    {
      /* Read in the stream chunk by chunk. If we don't get a whole */
      /* buffer full of characters the stream is finished */

      length = fread (buffer, 1, sizeof (buffer), stream);
      done = length < sizeof (buffer);

      /* Tell MEDICI to parse this chunk. If it is the last chunk */
      /* the "done" argument will be 1 to indicate this to MEDICI */

      EDI_Parse (parser, buffer, length, done);

      /* The error code will be non-zero on a fatal error */

      if (EDI_GetErrorCode (parser))
	{
	  fprintf (stderr, "%s at segment %ld\n",
		   EDI_GetErrorString (EDI_GetErrorCode (parser)),
		   EDI_GetCurrentSegmentIndex (parser));
	  return 1;
	}
    }
  while (!done);

  /* Tell MEDICI to free all resources associated with this parse(r) */

  EDI_ParserFree (parser);

  return 0;
}


/* At the start of a enclosing envelope increase indentation */

void
starthandler (void *userdata, edi_event_t event, EDI_Parameters parameters)
{
  int n, *indentation = (int *) userdata;
  char *code;

  for (n = 0; n < *indentation; n++)
    printf ("    ");

  if (parameters && (code = EDI_GetParameter (parameters, Code)))
    printf ("<%s %s>\n", edi_event_string (event), code);
  else
    printf ("<%s>\n", edi_event_string (event));

  (*indentation)++;
}


/* At the end of a enclosing envelope decrease indentation */

void
endhandler (void *userdata, edi_event_t event, EDI_Parameters parameters)
{
  int *indentation = (int *) userdata;

  (*indentation)--;
}


/* If we encounter a non-fatal error alert the user */

void
warninghandler (void *u, int error)
{
  fprintf (stderr, "WARNING: %s\n", EDI_GetErrorString (error));
}


/* Each time a segment is encountered we want to print the contents */

void segmenthandler
  (void *userdata, EDI_Parameters parameters, EDI_Segment segment,
   EDI_Directory d)
{
  int n, m, *indentation;
  char *s;
  indentation = (int *) userdata;

  /* Print some spaces to indent the output */

  for (n = 0; n < *indentation; n++)
    printf ("    ");

  /* Print the code in the segment tag */

  printf ("[%s]=", EDI_GetCode (segment));

  /* For every element in the segment ... */

  for (n = 0; n < EDI_GetElementCount (segment); n++)
    {
      /* The first element (element[0]) is precededed by an "=" */
      /* insert an element separator character for all others */

      if (n)
	printf ("+");

      /* Print every sub-element */

      for (m = 0; m < EDI_GetSubelementCount (segment, n); m++)
	printf ("[%s]", (s = EDI_GetElement (segment, n, m)) ? s : "");

    }

  /* End the segment with a newline */
  printf ("\n");
}
