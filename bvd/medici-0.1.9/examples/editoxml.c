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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <medici.h>

#include "xmlout.h"

/*
  
  You can embed an XML TSG into this program by #include-ing a file
  filled with the character values of each character in the XML TSG
  file - this can be done with the perl line below, eg.:
  
  perl -pe 's/(.)/"0x".unpack("H2",$1).","/egs; $_.="\n"' < d96a.xml > test.h
  
  This will take a while to compile, but the executable doesn't have
  to find the TSG at run-time - you can, however, override the
  embedded TSG at run-time by using the -x switch.
  
*/

char xmlbuff[] = {
  /* #include your C-ified XML file here */
  /* Avoid "ANSI C forbids empty initializer braces" warnings with a newline */
  0x0a
};


EDI_Directory read_xmltsg_buffer (char *, unsigned int);
EDI_Directory read_xmltsg (char *);


int command_line_options (int, char **);
void indent (int);

void warninghandler (void *, int);
void starthandler (void *, edi_event_t, EDI_Parameters);
void endhandler (void *, edi_event_t, EDI_Parameters);
void segmenthandler (void *, EDI_Parameters, EDI_Segment, EDI_Directory);
EDI_Directory directoryhandler (void *, EDI_Parameters);



/* Globals for command-line processing, etc. */

EDI_Directory directory = NULL;
char *xmlfile = NULL;
int terse = 0;
int outline = 0;

int
main (int argc, char **argv)
{
  EDI_Parser parser;
  FILE *stream = stdin;
  char buffer[4096];
  int arg, done, userdata = 0;
  size_t length;

  arg = command_line_options (argc, argv);

  /* If we were given a filename argument use that instead of stdin */

  if (arg && arg < argc)
    if (!(stream = fopen (argv[arg], "r")))
      {
	sprintf (buffer, "Couldn't open file '%s'", argv[arg]);
	perror (buffer);
	exit (-1);
      }

#ifdef HAVE_LIBEXPAT

  /* Parse the XML TSG if embedded or given on the command line */

  if (xmlfile)
    directory = read_xmltsg (xmlfile);
  else if (sizeof (xmlbuff) > 1)
    directory = read_xmltsg_buffer (xmlbuff, sizeof (xmlbuff));

#else /* HAVE_LIBEXPAT */

  if (xmlfile)
    {
      fprintf (stderr, "Expat library not available - XML cannot be read!\n");
      exit (-1);
    }

#endif /* HAVE_LIBEXPAT */



  /* Create the parser  */

  if (!(parser = EDI_ParserCreate ()))
    {
      /* If this failed it's almost certainly because a malloc() failed */

      perror ("Couldn't create parser");
      exit (-1);
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
  EDI_SetDirectoryHandler (parser, directoryhandler);

  /* print the xml header */
  vxmlprocessinginstruction ("xml",
			     "version", "1.0",
			     "encoding", "ISO-8859-1",
			     "standalone", "yes", NULL);
  xmlprint ("\n");

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


/* Only one directory to use - read in at the start */

EDI_Directory directoryhandler (void *v, EDI_Parameters p)
{
  return directory;
}

/* If we encounter a warning alert the user */

void
warninghandler (void *u, int error)
{
  fprintf (stderr, "WARNING: %s\n", EDI_GetErrorString (error));
}


/* At the start of a section print an XML element and increase indentation */

void
starthandler (void *userdata, edi_event_t event, EDI_Parameters parameters)
{
  int *indentation = (int *) userdata;

  indent (*indentation);


#define GP(v) EDI_GetParameter (parameters, v)

  vxmlstartelement (edi_event_string (event),
		    "test", GP (TestIndicator),
		    "standard", GP (Standard),
		    "sender", GP (SendersId),
		    "recipient", GP (RecipientsId),
		    "code", GP (Code),
		    "name", terse ? NULL : GP (Name),
		    "desc", terse ? NULL : GP (Description),
		    "note", terse ? NULL : GP (Note),
		    "date", GP (Date),
		    "time", GP (Time),
		    "transactioncode", GP (TransactionCode),
		    "transactiontype", GP (TransactionType),
		    "code", GP (MessageType),
		    "ref", GP (MessageReferenceNumber), NULL);
#undef GP

  xmlprint ("\n");

  (*indentation)++;
}


/* At the end of a section end the XML element and decrease indentation */

void
endhandler (void *userdata, edi_event_t event, EDI_Parameters parameters)
{
  int *indentation = (int *) userdata;

  (*indentation)--;

  indent (*indentation);

  xmlendelement (edi_event_string (event));
  xmlprint ("\n");
}


/* Each time a segment is encountered we want to print the contents */

void segmenthandler
  (void *userdata,
   EDI_Parameters parameters, EDI_Segment segment, EDI_Directory tsg)
{
  int n, m, *indentation = (int *) userdata;
  char *tag, indexn[8], indexm[8], *code, *name, *desc, *data, *list;
  edi_item_t element, subelement;

  /* Print some spaces to indent the output */

  indent (*indentation);

  tag = EDI_GetCode (segment);


  if (outline)
    {
      /* outline-only mode, so just print a void XML element */

      vxmlvoidelement ("segment",
		       "code", tag,
		       "name", terse ? NULL : EDI_SegmentName (tsg, tag),
		       "desc", terse ? NULL : EDI_SegmentDesc (tsg, tag),
		       NULL);
      xmlprint ("\n");
    }
  else
    {
      vxmlstartelement ("segment",
			"code", tag,
			"name", terse ? NULL : EDI_SegmentName (tsg, tag),
			"desc", terse ? NULL : EDI_SegmentDesc (tsg, tag),
			NULL);
      xmlprint ("\n");

      /* For every element in the segment ... */

      for (n = 0; n < EDI_GetElementCount (segment); n++, m = 0)
	{
	  sprintf (indexn, "%d", n);

	  element = EDI_SegmentItem (tsg, tag, n);

	  /* Is the element simple or composite? */

	  if (element.type || (EDI_GetSubelementCount (segment, n) > 1))
	    {
	      /* Composite element */

	      code = element.code;

	      name = terse ? NULL : EDI_ElementName (tsg, code);
	      desc = terse ? NULL : EDI_ElementDesc (tsg, code);

	      indent (*indentation + 1);
	      vxmlstartelement ("composite",
				"code", code,
				"name", name,
				"desc", desc, "index", indexn, NULL);
	      xmlprint ("\n");

	      /* Print every sub-element */

	      for (m = 0; m < EDI_GetSubelementCount (segment, n); m++)
		{
		  sprintf (indexm, "%d", m);

		  subelement = EDI_CompositeItem (tsg, element.code, m);
		  code = subelement.code;

		  data = EDI_GetElement (segment, n, m);
		  name = terse ? NULL : EDI_ElementName (tsg, code);
		  desc = terse ? NULL : EDI_ElementDesc (tsg, code);
		  list =
		    terse ? NULL : EDI_GetCodelistValue (tsg, code, data);

		  indent (*indentation + 2);
		  vxmlelement ("element", data,
			       "code", code,
			       "name", name,
			       "desc", desc,
			       "list", list, "index", indexm, NULL);
		  xmlprint ("\n");
		}

	      indent (*indentation + 1);
	      xmlendelement ("composite");
	      xmlprint ("\n");

	    }
	  else
	    {
	      /* Simple element */

	      code = element.code;

	      data = EDI_GetElement (segment, n, 0);
	      name = terse ? NULL : EDI_ElementName (tsg, code);
	      desc = terse ? NULL : EDI_ElementDesc (tsg, code);
	      list = terse ? NULL : EDI_GetCodelistValue (tsg, code, data);

	      indent (*indentation + 1);
	      vxmlelement ("element", data,
			   "code", code,
			   "name", name,
			   "desc", desc, "list", list, "index", indexn, NULL);
	      xmlprint ("\n");
	    }
	}

      /* End the segment and print a newline */

      indent (*indentation);
      xmlendelement ("segment");
      xmlprint ("\n");
    }
}


void
indent (int i)
{
  int n;
  for (n = 0; n < i; n++)
    xmlprint ("  ");
}


int
usage (int n)
{
  printf
    ("\n"
     "Usage: editoxml [-h] [-t] [-o] [-x <xmlfile>] [<edifile>]\n"
     "       -h help (this text)\n"
     "       -t terse output\n"
     "       -o outline only (no data elements, just structure)\n"
     "       -x read directory definition from <xmlfile>\n" "\n");
  return n;
}

/* very simple command line options - can't be bothered with getopt(3) */

int
command_line_options (int argc, char **argv)
{
  int optind;

  for (optind = 1; argv[optind] && argv[optind][0] == '-'; optind++)
    {

      /* switches must be a minus followed by exactly one letter */

      if (argv[optind][1] == '\0' || argv[optind][2] != '\0')
	exit (usage (-1));

      switch (argv[optind][1])
	{

	case 'x':
	  if (!(xmlfile = argv[++optind]))
	    exit (usage (-1));
	  break;

	case 't':
	  terse = 1;
	  break;

	case 'o':
	  outline = 1;
	  break;

	case 'h':
	  exit (usage (0));

	default:
	  exit (usage (-1));
	}
    }

  return optind;
}
























































/* old stuff from here down */



EDI_Directory xdirectoryhandler (void *, EDI_Parameters);
void xstarthandler (void *, edi_event_t event, EDI_Parameters);
void xendhandler (void *, edi_event_t event, EDI_Parameters);
void xtokenhandler (void *u, EDI_Token token);
void xsegmenthandler (void *, EDI_Parameters, EDI_Segment, EDI_Directory);

void xmlprolog (void);




typedef struct
{
  EDI_Directory directory;
  EDI_Parser parser;
}
MyData;

long
EDI_Escape (EDI_Parser p, char *dst, char *src)
{
  if (!dst)
    return 0;

  if (!src)
    {
      dst[0] = '\0';
      return 0;
    }
  else
    strcpy (dst, src);

  return strlen (dst);
}












EDI_Directory
xdirectoryhandler (void *u, EDI_Parameters p)
{
  MyData *mydata = (MyData *) u;
  char *type = EDI_GetParameter (p, MessageType);
  char *vers = EDI_GetParameter (p, MessageVersionNumber);
  char *rlse = EDI_GetParameter (p, MessageReleaseNumber);
  char *agcy = EDI_GetParameter (p, ControllingAgency);
  char *code = EDI_GetParameter (p, AssociationAssignedCode);

  vxmlprocessinginstruction ("directory",
			     "type", type,
			     "version", vers,
			     "release", rlse,
			     "agency", agcy, "code", code, NULL);

  return mydata->directory;
}


void
parameters2attributes (EDI_Parameters parameters, char **attributes)
{
  EDI_Parameter n;
  char *value;
  int m;

  for (m = 0, n = MinParameter; n < MaxParameter; n++)
    if ((value = EDI_GetParameter (parameters, n)) && strlen (value))
      {
	attributes[m++] = EDI_GetParameterString (n);
	attributes[m++] = value;
      }
  attributes[m] = NULL;
}


void
xstarthandler (void *u, edi_event_t event, EDI_Parameters p)
{
  char *attributes[256];

  parameters2attributes (p, attributes);
  xmlstartelement (edi_event_string (event), attributes);
}


void
xendhandler (void *u, edi_event_t event, EDI_Parameters p)
{
  xmlendelement (edi_event_string (event));
}







/* experimental */
void
tokenhandler (void *u, EDI_Token token)
{
  /*  char *name = "unknown", *attributes[256];
     char rdata[256], cdata[256];
     int n = 0;
     static int tag = 0;

     switch(token->type) {
     case EDI_TX: name = "element"; break;
     case EDI_TG: name = "segment"; break;
     case EDI_SA: name = "advice"; break;
     case EDI_TS: name = "ts"; break;
     case EDI_ES: name = "es"; break;
     case EDI_SS: name = "ss"; break;
     case EDI_ST: name = "st"; tag = 0; break;
     }

     sprintf(rdata, "%.*s", (int) token->rsize, token->rdata);
     sprintf(cdata, "%.*s", (int) token->csize, token->cdata);  


     if(token->type == EDI_TX) {
     attributes[n++] = "text";
     attributes[n++] = cdata;
     }

     if(token->type == EDI_TG) {
     attributes[n++] = "tag";
     attributes[n++] = cdata;
     }

     attributes[n++] = NULL;

     if(token->first) xmlstartelement(name, attributes);
     xmlnprint(token->rsize, token->rdata);
     if(token->type != EDI_TG && token->last) xmlendelement(name);

     if(token->type == EDI_ST) xmlendelement("segment"); */
}













void
voidstructhandler (void *u, EDI_Parameters p)
{
  char *attributes[256];
  parameters2attributes (p, attributes);
  xmlvoidelement (EDI_GetParameter (p, Event), NULL);
}


void
startcompositehandler (edi_item_t item)
{
  vxmlstartelement ("composite",
		    "code", item.code,
		    "name", item.name,
		    "description", item.desc,
		    "status", item.reqr ? "mandatory" : "conditional", NULL);
}


void xelementhandler
  (EDI_Parser parser, EDI_Directory directory, char *valu, edi_item_t item)
{
  char *attributes[32];
  char representation[16], buffer[8192], *repr;
  char tmp[64];
  double a_double;
  int n = 0, precision = 0;


  switch (item.repr)
    {
    case EDI_ISO2382A:
      repr = "a";
      break;
    case EDI_ISO2382N:
      repr = "n";
      break;
    case EDI_ISO2382X:
      repr = "an";
      break;
    case EDI_TRADACOMS1:
      precision = 1;
      repr = "d1";
      break;
    case EDI_TRADACOMS2:
      precision = 2;
      repr = "d2";
      break;
    case EDI_TRADACOMS3:
      precision = 3;
      repr = "d3";
      break;
    case EDI_TRADACOMS4:
      precision = 4;
      repr = "d4";
      break;
    default:
      repr = "??";
      break;
    }

  sprintf (representation, "%s%s%d", repr,
	   (item.min == item.max) ? "  " : "..", item.max);

  attributes[n++] = "code";
  attributes[n++] = item.code;
  attributes[n++] = "status";
  attributes[n++] = item.reqr ? "mandatory" : "conditional";
  attributes[n++] = "representation";
  attributes[n++] = representation;
  attributes[n++] = "name";
  attributes[n++] = item.name;
  attributes[n++] = "description";
  attributes[n++] = item.desc;
  attributes[n++] = "codelist";
  attributes[n++] = EDI_GetCodelistValue (directory, item.code, valu);
  attributes[n++] = "value";
  attributes[n++] = valu;

  if (precision && valu)
    {
      EDI_EvaluateString (item.repr, valu, EDI_DOUBLE, &a_double);
      sprintf (tmp, "%.*f", precision, a_double);
      attributes[n++] = "eval";
      attributes[n++] = tmp;
    }

  attributes[n++] = NULL;

  if (EDI_Escape (parser, buffer, valu))
    xmlelement ("element", attributes, buffer);
  else
    xmlvoidelement ("element", attributes);
}


void xsegmenthandler
  (void *u, EDI_Parameters parameters, EDI_Segment segment,
   EDI_Directory directory)
{
  MyData *mydata = (MyData *) u;
  EDI_Parser parser = mydata->parser;
  EDI_Parameters info;
  int n, m;
  char buff[8096];
  char *code, *element;
  char *ts = NULL, *ss = NULL, *es = NULL, *ri = NULL, *st = NULL;
  edi_item_t item;

  if ((info = EDI_GetParserInfo (parser)))
    {
      ts = EDI_GetParameter (info, TagSeparator);
      ss = EDI_GetParameter (info, SubelementSeparator);
      es = EDI_GetParameter (info, ElementSeparator);
      ri = EDI_GetParameter (info, ReleaseIndicator);
      st = EDI_GetParameter (info, SegmentTerminator);
    }

  code = EDI_GetCode (segment);

  vxmlstartelement ("segment",
		    "code", code,
		    "name", EDI_SegmentName (directory, code),
		    "description", EDI_SegmentDesc (directory, code),
		    "context", EDI_GetParameter (parameters, Context), NULL);

  EDI_Escape (mydata->parser, buff, code);
  if (buff)
    xmlprint (buff);

  xmlelement ("ts", NULL, ts ? ts : "+");

  for (n = 0; n < EDI_GetElementCount (segment); n++)
    {
      item = EDI_SegmentItem (directory, code, n);
      element = item.code;

      if (n)
	xmlelement ("es", NULL, es ? es : "+");

      if (item.type || (EDI_GetSubelementCount (segment, n) > 1))
	{
	  startcompositehandler (item);

	  for (m = 0; m < EDI_GetSubelementCount (segment, n); m++)
	    {
	      item = EDI_CompositeItem (directory, element, m);

	      if (m)
		xmlelement ("ss", NULL, ss ? ss : ":");

	      xelementhandler (mydata->parser, directory,
			       EDI_GetElement (segment, n, m), item);
	    }

	  xmlendelement ("composite");
	}
      else
	{
	  xelementhandler (mydata->parser, directory,
			   EDI_GetElement (segment, n, 0), item);
	}
    }

  xmlelement ("st", NULL, st ? st : "'");
  xmlendelement ("segment");

  if (info)
    free (info);
}






void
xmlprolog (void)
{
  vxmlprocessinginstruction ("xml", "version", "1.0", "encoding",
			     "ISO-8859-1", "standalone", "yes", NULL);
  xmlprint ("\n\n");

  vxmlcomment ("Generated with ", EDI_MEDICIVersion (), ".", NULL);

  xmlprint ("\n");

  vxmlcomment ("This is an example only!", NULL);

  xmlprint ("\n\n");

  vxmlcomment ("You can convert this file back to raw EDI ",
	       "by stripping out all XML markup to leave the text. ", NULL);

  xmlprint ("\n");

  vxmlcomment ("A Perl one-liner such as the following will work:", NULL);

  xmlprint ("\n");

  vxmlcomment ("perl -mXML::Parser -e '(new XML::Parser( Handlers => ",
	       "{ Char => sub {print $_[1]} }))->parse(\\*STDIN)'", NULL);
  xmlprint ("\n\n");

}
