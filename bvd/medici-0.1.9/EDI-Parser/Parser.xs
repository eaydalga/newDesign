#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "medici.h"
#include "internal.h"

#ifndef HAS_SVPV_NOLEN
#define SvPV_nolen(s) SvPV(s,PL_na)
#endif

#ifndef HAS_CALL_METHOD
#define call_method(m, f) perl_call_pv("EDI::Parser::MEDICI::" m, f)
#endif

/*
  these read_* functions are defined in the code in the examples
  directory. should probably re-implement them in perl to avoid having
  to use this
*/

edi_directory_t *read_xmltsg(char *);
edi_directory_t *read_txttsg(char *);
edi_directory_t *read_xmltsg_fd(int fd);

static void hashparameters(HV* hash, edi_parameters_t *parameters)
{
  char *key, *value;
  edi_parameter_t parameter;
  
  if(!parameters)	return;
  for(parameter = MinParameter; parameter < MaxParameter; parameter++)
    if((value = edi_parameters_get (parameters, parameter)))
      {
	key = edi_parameters_get_string (parameter);
	hv_store(hash, key, strlen(key), newSVpv(value, 0), 0);
      }
}

static void starthandler
(void *user_data, edi_event_t event, edi_parameters_t *parameters)
{
  HV* hash = newHV();
  dSP ;
  PUSHMARK(SP) ;
  XPUSHs(newRV_noinc((SV*) user_data));
  XPUSHs(newSVpv(edi_event_string(event), 0));
  XPUSHs(newRV_noinc((SV*) hash));
  PUTBACK ;
  
  hashparameters(hash, parameters);
  
  call_method("startHandler", G_VOID | G_DISCARD);
}

static void endhandler
(void *user_data, edi_event_t event, edi_parameters_t *parameters)
{
  char buffer[1024];
  HV* hash = newHV();
  dSP ;
  PUSHMARK(SP) ;
  XPUSHs(newRV_noinc((SV*) user_data));
  XPUSHs(newSVpv(edi_event_string(event), 0));
  XPUSHs(newRV_noinc((SV*) hash));
  PUTBACK ;
  
  hashparameters(hash, parameters);
  
  call_method("endHandler", G_VOID | G_DISCARD);
}


static void segmenthandler
(void *user_data, edi_parameters_t *parameters,
 edi_segment_t *segment, edi_directory_t *directory)
{
  char *key, *value;
  edi_parameter_t parameter;
  HV* hash = newHV();
  
  dSP ;
  PUSHMARK(SP) ;
  XPUSHs(newRV_noinc((SV*) user_data));
  XPUSHs(newSViv((IV) segment));
  XPUSHs(newSViv((IV) directory));
  XPUSHs(newRV_noinc((SV*) hash));
  PUTBACK ;
  
  hashparameters(hash, parameters);

  call_method("segmentHandler", G_VOID|G_DISCARD);
}

static edi_directory_t *directoryhandler
(void *user_data, edi_parameters_t *parameters)
{
  FILE *stream = stdin;
  char *file = NULL;
  SV* scalar;
  HV* hash = newHV();
  int count;
  
  dSP ;
  PUSHMARK(SP) ;
  XPUSHs(newRV_noinc((SV*) user_data));
  XPUSHs(newRV_noinc((SV*) hash));
  PUTBACK ;
  
  hashparameters(hash, parameters);
  
  count = call_method("directoryHandler", G_SCALAR);
  
  SPAGAIN;
  
  if(count != 1)
    return NULL;
  
  scalar = POPs;
  PUTBACK;
  
  return (edi_directory_t *) SvIV(scalar);
}

static void tokenhandler
(void *user_data, edi_token_t *token)
{
  dSP ;
  PUSHMARK(SP) ;
  XPUSHs(newRV_noinc((SV*) user_data));
  XPUSHs(newSViv((IV) token));
  PUTBACK ;
  
  call_method("tokenHandler", G_VOID|G_DISCARD);
}


static void errorhandler
(void *user_data, edi_error_t code)
{
  dSP ;
  PUSHMARK(SP) ;
  XPUSHs(newRV_noinc((SV*) user_data));
  XPUSHs(newSViv(code));
  XPUSHs(newSVpv(edi_event_string(code), 0));
  PUTBACK ;
  
  call_method("errorHandler", G_VOID|G_DISCARD);
}

static void warninghandler
(void *user_data, edi_error_t code)
{
  dSP ;
  PUSHMARK(SP) ;
  XPUSHs(newRV_noinc((SV*) user_data));
  XPUSHs(newSViv(code));
  XPUSHs(newSVpv(edi_event_string(code), 0));
  PUTBACK ;
  
  call_method("warningHandler", G_VOID|G_DISCARD);
}


MODULE = EDI::Parser		PACKAGE = EDI::Parser

IV
EDIFACT()
    CODE:
	RETVAL = EDI_EDIFACT;
    OUTPUT:
	RETVAL

IV
UNGTDI()
    CODE:
	RETVAL = EDI_UNGTDI;
    OUTPUT:
	RETVAL

IV
X12()
    CODE:
	RETVAL = EDI_X12;
    OUTPUT:
	RETVAL


MODULE = EDI::Parser		PACKAGE = EDI::Parser::MEDICI		

######################################################################
# Parser methods
######################################################################
IV
parser_create()
    CODE:
	edi_parser_t *parser;
	parser = edi_parser_create (EDI_UNKNOWN);

	edi_parser_set_segment_handler (parser, segmenthandler);
	edi_parser_set_start_handler (parser, starthandler);
	edi_parser_set_end_handler (parser, endhandler);
	edi_parser_set_directory_handler (parser, directoryhandler);
	edi_parser_set_token_handler (parser, tokenhandler);
	edi_parser_set_error_handler (parser, errorhandler);
	edi_parser_set_warning_handler (parser, warninghandler);

	RETVAL = (IV) parser;
    OUTPUT:
	RETVAL

void
parser_free(parser)
	IV	parser
    CODE:
	edi_parser_free((edi_parser_t *) parser);


void
parser_user_data(parser, ref)
	IV	parser
	SV*     ref
    CODE:
	SV* user_data;
	user_data = SvRV(ref);
	edi_parser_set_user_data((edi_parser_t *) parser, (void *) user_data);


long
parser_get_error_code(parser)
	IV	parser
    CODE:
	RETVAL = edi_parser_get_error_code ((edi_parser_t *) parser);
    OUTPUT:
	RETVAL

long
parser_parse(parser, string, done)
	IV	parser
	SV*	string
	SV*	done
    CODE:
	char	*buffer;
	STRLEN 	length;
	buffer = SvPV(string, length);
	RETVAL = edi_parser_parse((edi_parser_t *) parser,
				 buffer, length, SvTRUE(done));
    OUTPUT:
	RETVAL


IV
parser_xmltsg(parser, string)
	IV	parser
	SV*	string
    CODE:
	char *filename;

	if(!(filename = SvPV_nolen(string)))
          RETVAL = 0L;
	else
	  RETVAL = (IV) read_xmltsg (filename);

    OUTPUT:
	RETVAL

IV
parser_xmltsg_fd(parser, fd)
	IV	parser
	IV	fd
    CODE:
	RETVAL = (IV) read_xmltsg_fd (fd);

    OUTPUT:
	RETVAL


IV
parser_txttsg(parser, string)
	IV	parser
	SV*	string
    CODE:
	char *filename;

	if(!(filename = SvPV_nolen(string)))
          RETVAL = 0L;
	else
	  RETVAL = (IV) read_txttsg (filename);

    OUTPUT:
	RETVAL


IV
parser_interchange_type(parser)
	IV	parser
    CODE:
	RETVAL = (IV) edi_parser_interchange_type((edi_parser_t *) parser);

    OUTPUT:
	RETVAL

IV
parser_abort(parser)
        IV      parser
    CODE:
        edi_parser_raise_error((edi_parser_t *) parser, EDI_EAPP);



######################################################################
# Miscellaneous
######################################################################

SV*
get_error_string(code)
	int	code
    CODE:
	RETVAL = newSVpv(edi_error_string((edi_error_t) code), 0);
    OUTPUT:
	RETVAL










######################################################################
# Segment methods
######################################################################
MODULE = EDI::Parser            PACKAGE = EDI::Parser::Segment

SV*
segment_get_code(segment)
	IV	segment
    CODE:
	char *code;
	code = edi_segment_get_code((edi_segment_t *) segment);
	RETVAL = newSVpv(code, 0);
    OUTPUT:
	RETVAL 

IV
segment_element_count(segment)
	IV	segment
    CODE:
	RETVAL = edi_segment_get_element_count((edi_segment_t *) segment);
    OUTPUT:
	RETVAL

IV
segment_subelement_count(segment, element)
	IV	segment
	IV	element
    CODE:
	RETVAL = edi_segment_get_subelement_count((edi_segment_t *) segment, element);
    OUTPUT:
	RETVAL

SV*
segment_get_element(segment, element, subelem)
        IV      segment
        IV      element
	IV	subelem
    CODE:
	char *value;
	value = edi_segment_get_element((edi_segment_t *) segment, element, subelem);
	RETVAL = value ? newSVpv(value, 0) : &PL_sv_undef;
    OUTPUT:
        RETVAL

SV*
segment_get_element_by_name(segment, directory, element, subelement)
        IV      segment
	IV      directory
	SV*	element
	SV*	subelement
    PREINIT:
	char	*value = NULL;
	char	*elementp, *subelementp;
	STRLEN	length;
    CODE:
	RETVAL = &PL_sv_undef;

	if(!directory || !segment) return;

	elementp = SvPV(element, length);
	subelementp = SvPV(subelement, length);

	value = edi_get_element_by_name ((edi_directory_t *) directory,
				      	  (edi_segment_t *) segment,
					   elementp, subelementp);

	RETVAL = value ? newSVpv(value, 0) : &PL_sv_undef;
    OUTPUT:
        RETVAL


SV*
directory_get_segment_name(directory, segment_code)
	IV	directory
	SV*	segment_code
    PREINIT:
        char    *value = NULL, *buffer = NULL;
        STRLEN  length;
    CODE:
	RETVAL = &PL_sv_undef;
	value = SvPV(segment_code, length);
	if((buffer = malloc(length + 1))) {
	  strncpy(buffer, value, length);
	  buffer[length] = '\0';
	  if((value = EDI_SegmentName ((edi_directory_t *) directory, buffer)))
	    RETVAL = newSVpv(value, 0);
	}
    OUTPUT:
	RETVAL
	


######################################################################
# Token methods
######################################################################

MODULE = EDI::Parser            PACKAGE = EDI::Parser::Token

SV*
token_get_rdata(t)
	IV	t
    CODE:
	edi_token_t *token = (edi_token_t *) t;
	RETVAL = token->rsize ?
	newSVpv(token->rdata, token->rsize) : newSVpv("", 0);
    OUTPUT:
	RETVAL


SV*
token_get_cdata(t)
	IV	t
    CODE:
	edi_token_t *token = (edi_token_t *) t;
	RETVAL = token->csize ?
	newSVpv(token->cdata, token->csize) : newSVpv("", 0);
    OUTPUT:
	RETVAL


IV
token_get_type(t)
	IV	t
    CODE:
	edi_token_t *token = (edi_token_t *) t;
	RETVAL = token->type;
    OUTPUT:
	RETVAL

IV
token_is_first(t)
	IV	t
    CODE:
	edi_token_t *token = (edi_token_t *) t;
	RETVAL = token->first;
    OUTPUT:
	RETVAL

IV
token_is_last(t)
	IV	t
    CODE:
	edi_token_t *token = (edi_token_t *) t;
	RETVAL = token->last;
    OUTPUT:
	RETVAL


