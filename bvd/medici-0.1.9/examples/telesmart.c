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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <medici.h>
#include <frncsc.h>

/**********************************************************************
 * Produces a human readable version of MAILACK, POSTACK and SESSACK
 * messages.
 **********************************************************************/

#define DEBUG 0
#define NPS "%14s\n"
#define FRD "%2s\n"
#define NCP "%-14s %14s %-14s %-8s %1s %-6s\n"
#define NAC "%-14s %14s %-14s %-8s %9s %-6.6s %-4.4s %4s %s%s\n"
#define NCP_ /*CMD*/ "INTERCHANGE#", "APPREF", "TYPE", "S", "DATE"
#define NAC_ /*ACK*/ "INTERCHANGE#", "APPREF", "TYPE", "FILE SIZE", "DATE",\
                     "TIME", "CODE", "STATUS", ""

void *EDIFACT_293_ts (void);

typedef struct
{
  EDI_Parser parser;
  char buffer[64];
  int offset, message;
  int length, body, bgm;
  FILE *stream;
}
MyData;

void
chkerr (EDI_Parser p)
{
  int code = EDI_GetErrorCode (p);
  long byte = EDI_GetCurrentByteIndex (p);

  if (code)
    {
      fprintf (stderr, "error:%ld: %s\n", byte, EDI_GetErrorString (code));
      exit (-1);
    }
}


void
segmenth (void *u, EDI_Parameters p, EDI_Segment s, EDI_Directory d)
{
  char *valu, *clst, *code = EDI_GetCode (s);
  MyData *mydata = u;
  int doc, nfields;
  char *fields[12];

  for (nfields = 0; nfields < (sizeof (fields) / sizeof (char *)); nfields++)
    fields[nfields] = NULL;
  nfields = 0;

  if (mydata->bgm)
    {
      if (!strcmp (code, "NAC"))
	{
	  fields[0] = NAC;
	  fields[1] = EDI_GetElementByName (d, s, "9504");
	  fields[2] = EDI_GetElementByName (d, s, "0020");
	  fields[3] = EDI_GetElementByName (d, s, "0026");
	  fields[4] = EDI_GetElementByName (d, s, "9506");
	  fields[5] = EDI_GetElementByName (d, s, "9505");
	  fields[6] = EDI_GetElementByName (d, s, "S004/0017");
	  fields[7] = EDI_GetElementByName (d, s, "S004/0019");
	  fields[8] = "";
	  fields[9] = "";
	  fields[10] = "";

	  if ((valu = EDI_GetElementByName (d, s, "C999/9502")))
	    {
	      fields[8] = valu;
	      if ((clst = EDI_GetCodelistValue (d, "9502", valu)))
		fields[9] = clst;
	    }

	  if ((valu = EDI_GetElementByName (d, s, "C999/9503")))
	    {
	      fields[10] = valu;
	      if ((clst = EDI_GetCodelistValue (d, "9503", valu)))
		fields[10] = clst;
	    }

	}
      else if (!strcmp (code, "NCP"))
	{
	  fields[0] = NCP;
	  fields[1] = EDI_GetElementByName (d, s, "C998/0004");
	  fields[2] = EDI_GetElementByName (d, s, "C998/0020");
	  fields[3] = EDI_GetElementByName (d, s, "0026");
	  fields[4] = EDI_GetElementByName (d, s, "9506");
	  fields[5] = EDI_GetElementByName (d, s, "9500");
	  fields[6] = EDI_GetElementByName (d, s, "0017");
	}
      else if (!strcmp (code, "NPS"))
	{
	  fields[0] = NPS;
	  fields[1] = EDI_GetElementByName (d, s, "0022");
	}
      else if (!strcmp (code, "FRD"))
	{
	  fields[0] = FRD;
	  fields[1] = EDI_GetElementByName (d, s, "9501");
	}
      else
	{
	  return;
	}

      if (fields[0])
	printf (fields[0],
		fields[1] ? fields[1] : "*",
		fields[2] ? fields[2] : "*",
		fields[3] ? fields[3] : "*",
		fields[4] ? fields[4] : "*",
		fields[5] ? fields[5] : "*",
		fields[6] ? fields[6] : "*",
		fields[7] ? fields[7] : "*",
		fields[8] ? fields[8] : "*",
		fields[9] ? fields[9] : "*", fields[10] ? fields[10] : "*");
    }
  else
    {
      if (strcmp (code, "BGM"))
	return;
      mydata->bgm = 1;
      doc = atoi (EDI_GetElementByName (d, s, "C002/1001"));
      if (!((doc >= 250 && doc <= 255) || (doc >= 260 && doc <= 262)))
	return;

      switch (doc)
	{
	case 250:
	  printf ("MAILBOX LISTING COMMAND\n");
	  break;
	case 251:
	  printf ("POSTBOX LISTING COMMAND\n");
	  break;
	case 252:
	  printf (NCP, "RECV CMD/FROM", NCP_);
	  break;
	case 253:
	  printf (NCP, "DEL CMD/FROM", NCP_);
	  break;
	case 254:
	  printf (FRD, "SET AUTODEL COMMAND");
	  break;
	case 255:
	  printf (NPS, "CHANGE PASSWORD COMMAND");
	  break;
	case 260:
	  printf (NAC, "SESSACK [FROM]", NAC_);
	  break;
	case 261:
	  printf (NAC, "MAILACK [FROM]", NAC_);
	  break;
	case 262:
	  printf (NAC, "POSTACK [UNTO]", NAC_);
	  break;
	}
    }
}


EDI_Directory
dhandler (void *u, EDI_Parameters p)
{
  char *type = EDI_GetParameter (p, MessageType);
  char *ver = EDI_GetParameter (p, MessageVersionNumber);
  char *rel = EDI_GetParameter (p, MessageReleaseNumber);

  if ((!strcmp (type, "NETCMD") || !strcmp (type, "NETACK")) &&
      !strcmp (ver, "2") && !strcmp (rel, "93"))
    return EDIFACT_293_ts ();
  fprintf (stderr, "Not a known TeleSmart API message type!\n");
  exit (-1);
  return NULL;
}



int
main (int argc, char **argv)
{
  EDI_Parser parser;
  FILE *stream = stdin;
  MyData mydata;
  char buffer[4096];
  int length, done;

  if (argc > 1)
    if (!(stream = fopen (argv[1], "r")))
      {
	perror ("Couldn't open file");
	return 1;
      }

  if (!(parser = EDI_ParserCreate ()))
    {
      perror ("Couldn't create parser");
      return 1;
    }

  /* init mydata */
  mydata.parser = parser;
  mydata.stream = stdout;
  mydata.bgm = 0;
  strcpy (mydata.buffer, "");

  /* set callbacks */
  EDI_SetDirectoryHandler (parser, dhandler);
  EDI_SetSegmentHandler (parser, segmenth);
  EDI_SetUserData (parser, &mydata);

  do
    {
      length = fread (buffer, 1, sizeof (buffer), stream);
      done = length < sizeof (buffer);

      EDI_Parse (parser, buffer, length, done);

      if (EDI_GetErrorCode (parser))
	{
	  fprintf (stderr,
		   "%s at segment %ld\n",
		   EDI_GetErrorString (EDI_GetErrorCode (parser)),
		   EDI_GetCurrentSegmentIndex (parser));
	  return 1;
	}
    }
  while (!done);

  EDI_ParserFree (parser);
  return 0;
}









/*

  The follwing data is derived from:

  Value Added Network - Edifact Commands And Acknowledgements
  By Telesmart API Ltd and TeleOrdering Ltd
  
  Whitaker (owners of TeleOrdering) is now part of VNU. The
  specification document (docs/telesmart.txt) is attributed to
  them. The specification document and derived works are included in
  this project with the kind permission of the IT Director, VNU
  Entertainment Media Ltd.

  This data and the rest of the code in this project has no other
  connection with Telesmart, TeleOrdering, Whitaker or VNU.

*/

static edi_francesco_trnsctn_rule_t transactionsetrule[] = {
  {NULL}
};

static edi_francesco_element_info_t elementinfo[] = {
  {"0017", EDI_ISO2382X, 6, 6, "Date", "YYMMDD"},
  {"0019", EDI_ISO2382N, 4, 4, "Time", "HHMM"},
  {"0020", EDI_ISO2382X, 1, 14, "INTERCHANGE CONTROL REFERENCE",
   "Unique reference assigned by sender"},
  {"0026", EDI_ISO2382X, 1, 14, "APPLICATION REFERENCE",
   "Optionally message identification if the interchange contains only one type of message"},
  {"0062", EDI_ISO2382X, 1, 14, "MESSAGE REFERENCE NUMBER",
   "A sender's unique message reference"},
  {"1000", EDI_ISO2382X, 1, 35, "Document/message name",
   "Plain language identifier specifying the function of a document/message."},
  {"1001", EDI_ISO2382X, 1, 3, "Document/message name coded",
   "Document/message identifier expressed in code."},
  {"1004", EDI_ISO2382X, 1, 35, "DOCUMENT/MESSAGE NUMBER",
   "Reference number assigned to the document/message by the issuer."},
  {"1131", EDI_ISO2382X, 1, 3, "Code list qualifier",
   "Identification of a code list."},
  {"1225", EDI_ISO2382X, 1, 3, "MESSAGE FUNCTION, CODED",
   "Code indicating the function of the message."},
  {"3055", EDI_ISO2382X, 1, 3, "Code list responsible agency, coded",
   "Code identifying the agency responsible for a code list."},
  {"4343", EDI_ISO2382X, 1, 3, "RESPONSE TYPE, CODED",
   "Code specifying the type of acknowledgment required."},
  {"9500", EDI_ISO2382A, 1, 1, "File Status", NULL},
  {"9501", EDI_ISO2382N, 1, 12, "File Retention Days", NULL},
  {"9502", EDI_ISO2382N, 1, 3, "Interchange Level Status", NULL},
  {"9503", EDI_ISO2382N, 1, 3, "Status Diagnostic Information", NULL},
  {"9504", EDI_ISO2382X, 1, 35, "Sender/Receiver Identification", NULL},
  {"9505", EDI_ISO2382N, 1, 14, "File Size", NULL},
  {"9506", EDI_ISO2382X, 1, 8, "Interchange Type", NULL},
  {NULL}
};

static edi_francesco_cmpsite_info_t compositeinfo[] = {
  {"S004", "DATE/TIME OF PREPARATION", NULL},
  {"C002", "DOCUMENT/MESSAGE NAME",
   "Identification of a type of document/message by code or name. Code preferred."},
  {"C999", "Status Information", NULL},
  {NULL}
};

static edi_francesco_cmpsite_list_t compositecontents[] = {
  {"S004", "0017", 'M'},
  {"S004", "0019", 'M'},
  {"C002", "1001", 'C'},
  {"C002", "1131", 'C'},
  {"C002", "3055", 'C'},
  {"C002", "1000", 'C'},
  {"C998", "0004", 'M'},
  {"C998", "0020", 'C'},
  /* FIXME   ^^^^ VAN doc says 0200 -surely shome mishtake */
  {"C999", "9502", 'M'},
  {"C999", "9503", 'C'},
  {"C999", "0062", 'C'},
  {NULL}
};


static edi_francesco_segment_info_t segmentinfo[] = {
  {"BGM", "BEGINNING OF MESSAGE",
   "To indicate the type and function of a message and to transmit the identifying number."},
  {"NAC", "Network Acknowledgment", "The New NAC Segment"},
  {"NCP", "Network Command Parameter", "The New NCP Segment"},
  {"FRD", "File Retention Days", "The New FRD segment"},
  {"NPS", "New Password", "The New NPS Segment"},
  {NULL}
};


static edi_francesco_segment_list_t segmentcontents[] = {
  {"BGM", "C002", 'C', EDI_COMPOSITE},
  {"BGM", "1004", 'C', EDI_ELEMENT},
  {"BGM", "1225", 'C', EDI_ELEMENT},
  {"BGM", "4343", 'C', EDI_ELEMENT},
  {"NAC", "9504", 'M', EDI_ELEMENT},
  {"NAC", "0020", 'M', EDI_ELEMENT},
  {"NAC", "0026", 'C', EDI_ELEMENT},
  {"NAC", "9506", 'M', EDI_ELEMENT},
  {"NAC", "9505", 'M', EDI_ELEMENT},
  {"NAC", "S004", 'M', EDI_COMPOSITE},
  {"NAC", "C999", 'M', EDI_COMPOSITE},
  {"NCP", "9500", 'M', EDI_ELEMENT},
  {"NCP", "0026", 'C', EDI_ELEMENT},
  {"NCP", "C998", 'C', EDI_COMPOSITE},
  {"NCP", "9506", 'C', EDI_ELEMENT},
  {"NCP", "0017", 'C', EDI_ELEMENT},
  {"FRD", "9501", 'M', EDI_ELEMENT},
  {"NPS", "0022", 'M', EDI_ELEMENT},
  {NULL}
};

static edi_francesco_codelst_info_t codelistinfo[] = {
  {"1001", "250", "NLMAIL", "Network List Mailbox Command"},
  {"1001", "251", "NLPOST", "Network List Postbox Command"},
  {"1001", "252", "NRECEIVE", "Network Receive Files (s) Command"},
  {"1001", "253", "NDELETE", "Network Delete File(s) Command"},
  {"1001", "254", "NAUTODEL", "Network Auto-delete Files Command"},
  {"1001", "255", "NCPASS", "Network Change Password Command"},

  {"1001", "260", "SESSACK", "Network session acknowledgement"},
  {"1001", "261", "MAILACK", "Mailbox listing generated by NLMAIL command"},
  {"1001", "262", "POSTACK", "Postbox listing generated by NLPOST command"},

  {"9502", "1", "File accepted by network", NULL},
  {"9502", "2", "Invalid Interchange Header (see diagnostic) ", NULL},
  {"9502", "3", "Mismatching Interchange Trailer Control Ref.", NULL},
  {"9502", "4", "Mismatching Functional Group Trailer Ref", NULL},
  {"9502", "5", "Functional Group Trailer Message Count wrong", NULL},
  {"9502", "6", "Mismatching Message Trailer Msg Ref", NULL},
  {"9502", "7", "Message Trailer Message Count wrong", NULL},
  {"9502", "8", "Interchange Trailer Control Count wrong", NULL},
  {"9502", "9", "Duplicate Interchange Reference", NULL},
  {"9502", "10", "Start envelope with no end envelope", NULL},
  {"9502", "11", "No interchange start found", NULL},
  {"9502", "12", "Service unavailable", NULL},
  {"9502", "13", "Invalid command (see diagnostic) ", NULL},
  {"9502", "14", "Bad Segment Syntax", NULL},
  {"9502", "15", "Unexpected Segment (sequence error)", NULL},

  {"9502", "100", "File awaiting extraction", NULL},
  {"9502", "101", "File received", NULL},
  {"9502", "200", "File delivered to recipients mailbox", NULL},
  {"9502", "201", "File extracted by recipient", NULL},
  {"9502", "202", "File deleted by recipient before extraction", NULL},
  {"9502", "203", "Unknown recipient", NULL},
  {"9502", "204", "Accepted by Network, but not yet delivered", NULL},
  {"9502", "205", "Purged by Network before recipient extraction", NULL},
  {"9502", "206", "Duplicate File discarded by Network", NULL},

  {"9503", "1", "Invalid Date", NULL},
  {"9503", "2", "Recipient does not exist", NULL},
  {"9503", "3", "Invalid Password", NULL},
  {"9503", "4", "Invalid Sender", NULL},
  {"9503", "5", "Invalid Priority", NULL},
  {"9503", "6", "Invalid Application Ref", NULL},
  {"9503", "7", "Invalid Syntax Id", NULL},
  {"9503", "8", "Invalid Interchange Ref", NULL},
  {"9503", "9", "Invalid Binary File Size", NULL},

  {"9503", "100", "Command Not Recognised", NULL},
  {"9503", "101", "Invalid Post Box List Command", NULL},
  {"9503", "102", "Invalid Mail Box List Command", NULL},
  {"9503", "103", "Invalid Receive Command", NULL},
  {"9503", "104", "Invalid Delete Command", NULL},
  {"9503", "105", "Invalid Auto-delete Command", NULL},
  {"9503", "106", "Invalid Change Password Command", NULL},

  {"9506", "X12", "ANSI ASC X12", NULL},
  {"9506", "UNTDI", "UN(G) TDI", NULL},
  {"9506", "EDIFACT", "EDIFACT", NULL},
  {NULL}
};

void *
EDIFACT_293_ts (void)
{
  return edi_francesco_create (elementinfo,
			       compositeinfo,
			       segmentinfo,
			       compositecontents,
			       segmentcontents,
			       codelistinfo, transactionsetrule);
}
