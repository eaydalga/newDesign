/*
 *  Copyright (C) 2006, 2007 Michael Edgar
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

//gcc scan997.c -std=c99 -I../include -L../lib -lm -ledival -o scan997

#include <edival.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFF_SIZE 16777216

int group_start = 0;
int counter = 0;
int depth = 0;
char prefix[50];
char curr_seg[4];
int  curr_e = 0;
int  curr_c = 0;
int  is_comp = 0;
int  type_ok = 0;

void handleSegmentError(void *myData, const char *tag, enum EDI_SegmentValidationError err)
{
	fprintf(stderr, "*~*~* Segment error %d on %s\n", err, tag);
}

void handleElementError(void *myData, int element, int component, enum EDI_ElementValidationError err)
{
	if(component){
		fprintf(stderr, "\n\n *** Element error %d on element %s%2.2d-%d ***\n\n", err, curr_seg, element, component);
	} else {
		fprintf(stderr, "\n\n *** Element error %d on element %s%2.2d ***\n\n", err, curr_seg, element);
	}
}

void handleLoopStart(void *myData, const char *loopID)
{
	fprintf(stdout, "%s<%s>\n", prefix, loopID);
	depth++;
	prefix[0] = '\0';
	for(int i = 0; i < depth; i++){
		strcat(prefix, "   ");
	}
	if(strcmp("group", loopID) == 0){
		group_start = 1;
		counter = 0;
		type_ok = 0;
		EDI_Schema s = EDI_GetSchema((EDI_Parser)myData);
	}	
	return;
}

void handleLoopEnd(void *myData, const char *loopID)
{
	depth--;
	prefix[0] = '\0';
	for(int i = 0; i < depth; i++){
		strcat(prefix, "   ");
	}
	fprintf(stdout, "%s</%s>\n", prefix, loopID);
	if(strcmp("group", loopID) == 0){
		EDI_Schema s = EDI_GetSchema((EDI_Parser)myData);
	}
	return;
}

void load_basic_standards(EDI_Parser p)
{

	EDI_Schema s = NULL;
	EDI_SchemaNode parent = NULL;
	EDI_SchemaNode child  = NULL;

	s = EDI_GetSchema(p);
	if(s && strcmp(EDI_GetSchemaId(s), "Generic ANSI X12 Schema") != 0){
		s = EDI_RemoveSchema(p);
		EDI_SchemaFree(s);
	}
	if(!s){
		s = EDI_SchemaCreate(EDI_ANSI_X12);
		EDI_SetSchemaId(s, "Generic ANSI X12 Schema");
		EDI_SetSchema(p, s);

		EDI_SchemaNode hold = EDI_CreateElementType(s, EDI_DATA_STRING, "I01", 2, 2);
		EDI_AddElementValue(hold, "00");
		EDI_AddElementValue(hold, "01");
		EDI_AddElementValue(hold, "02");
		EDI_AddElementValue(hold, "03");
		EDI_AddElementValue(hold, "04");
		EDI_AddElementValue(hold, "05");
		EDI_AddElementValue(hold, "06");
		
		EDI_CreateElementType(s, EDI_DATA_STRING, "I02", 10, 10);

		hold = EDI_CreateElementType(s, EDI_DATA_STRING, "I03", 2, 2);
		EDI_AddElementValue(hold, "00");
		EDI_AddElementValue(hold, "01");

		EDI_CreateElementType(s, EDI_DATA_STRING, "I04", 10, 10);

		hold = EDI_CreateElementType(s, EDI_DATA_STRING, "I05", 2, 2);
		EDI_AddElementValue(hold, "01");
		EDI_AddElementValue(hold, "02");
		EDI_AddElementValue(hold, "03");
		EDI_AddElementValue(hold, "04");
		EDI_AddElementValue(hold, "07");
		EDI_AddElementValue(hold, "08");
		EDI_AddElementValue(hold, "09");
		EDI_AddElementValue(hold, "10");
		EDI_AddElementValue(hold, "11");
		EDI_AddElementValue(hold, "12");
		EDI_AddElementValue(hold, "13");
		EDI_AddElementValue(hold, "14");
		EDI_AddElementValue(hold, "15");
		EDI_AddElementValue(hold, "16");
		EDI_AddElementValue(hold, "17");
		EDI_AddElementValue(hold, "18");
		EDI_AddElementValue(hold, "19");
		EDI_AddElementValue(hold, "20");
		EDI_AddElementValue(hold, "21");
		EDI_AddElementValue(hold, "22");
		EDI_AddElementValue(hold, "23");
		EDI_AddElementValue(hold, "24");
		EDI_AddElementValue(hold, "25");
		EDI_AddElementValue(hold, "26");
		EDI_AddElementValue(hold, "27");
		EDI_AddElementValue(hold, "28");
		EDI_AddElementValue(hold, "29");
		EDI_AddElementValue(hold, "30");
		EDI_AddElementValue(hold, "31");
		EDI_AddElementValue(hold, "32");
		EDI_AddElementValue(hold, "33");
		EDI_AddElementValue(hold, "34");
		EDI_AddElementValue(hold, "35");
		EDI_AddElementValue(hold, "36");
		EDI_AddElementValue(hold, "37");
		EDI_AddElementValue(hold, "38");
		EDI_AddElementValue(hold, "AM");
		EDI_AddElementValue(hold, "NR");
		EDI_AddElementValue(hold, "SA");
		EDI_AddElementValue(hold, "SN");
		EDI_AddElementValue(hold, "ZZ");

		EDI_CreateElementType(s, EDI_DATA_STRING, "I06", 15, 15);
		EDI_CreateElementType(s, EDI_DATA_STRING, "I07", 15, 15);
		EDI_CreateElementType(s, EDI_DATA_DATE, "I08", 6, 6);
		EDI_CreateElementType(s, EDI_DATA_TIME, "I09", 4, 4);
		EDI_CreateElementType(s, EDI_DATA_STRING, "I65", 1, 1);

		hold = EDI_CreateElementType(s, EDI_DATA_STRING, "I11", 5, 5);
		EDI_AddElementValue(hold, "00200");
		EDI_AddElementValue(hold, "00201");
		EDI_AddElementValue(hold, "00204");
		EDI_AddElementValue(hold, "00300");
		EDI_AddElementValue(hold, "00301");
		EDI_AddElementValue(hold, "00302");
		EDI_AddElementValue(hold, "00303");
		EDI_AddElementValue(hold, "00304");
		EDI_AddElementValue(hold, "00305");
		EDI_AddElementValue(hold, "00306");
		EDI_AddElementValue(hold, "00307");
		EDI_AddElementValue(hold, "00400");
		EDI_AddElementValue(hold, "00401");
		EDI_AddElementValue(hold, "00402");
		EDI_AddElementValue(hold, "00403");
		EDI_AddElementValue(hold, "00404");
		EDI_AddElementValue(hold, "00405");
		EDI_AddElementValue(hold, "00406");
		EDI_AddElementValue(hold, "00500");
		EDI_AddElementValue(hold, "00501");
		EDI_AddElementValue(hold, "00502");
		EDI_AddElementValue(hold, "00503");

		EDI_CreateElementType(s, EDI_DATA_INTEGER, "I12", 9, 9);

		hold = EDI_CreateElementType(s, EDI_DATA_STRING, "I13", 1, 1);
		EDI_AddElementValue(hold, "0");
		EDI_AddElementValue(hold, "1");

		hold = EDI_CreateElementType(s, EDI_DATA_STRING, "I14", 1, 1);
		EDI_AddElementValue(hold, "I");
		EDI_AddElementValue(hold, "P");
		EDI_AddElementValue(hold, "T");

		EDI_CreateElementType(s, EDI_DATA_STRING, "I15", 1, 1);
		EDI_CreateElementType(s, EDI_DATA_INTEGER, "I16", 1, 5);
		EDI_CreateElementType(s, EDI_DATA_INTEGER, "28", 1, 9);
		EDI_CreateElementType(s, EDI_DATA_INTEGER, "97", 1, 6);
		EDI_CreateElementType(s, EDI_DATA_STRING, "124", 2, 15);
		EDI_CreateElementType(s, EDI_DATA_STRING, "142", 2, 15);
		EDI_CreateElementType(s, EDI_DATA_TIME, "337", 4, 8);
		EDI_CreateElementType(s, EDI_DATA_DATE, "373", 8, 8);
		EDI_CreateElementType(s, EDI_DATA_STRING, "455", 1, 2);

		hold = EDI_CreateElementType(s, EDI_DATA_STRING, "479", 2, 2);
		/*EDI_AddElementValue(hold, "AA");
		EDI_AddElementValue(hold, "AB");
		EDI_AddElementValue(hold, "AC");
		EDI_AddElementValue(hold, "AD");
		EDI_AddElementValue(hold, "AE");
		EDI_AddElementValue(hold, "AF");
		EDI_AddElementValue(hold, "AG");
		EDI_AddElementValue(hold, "AH");
		EDI_AddElementValue(hold, "AI");
		EDI_AddElementValue(hold, "AK");
		EDI_AddElementValue(hold, "AL");
		EDI_AddElementValue(hold, "AM");
		EDI_AddElementValue(hold, "AN");
		EDI_AddElementValue(hold, "AO");
		EDI_AddElementValue(hold, "AP");
		EDI_AddElementValue(hold, "AQ");
		EDI_AddElementValue(hold, "AR");
		EDI_AddElementValue(hold, "AS");
		EDI_AddElementValue(hold, "AT");
		EDI_AddElementValue(hold, "AU");
		EDI_AddElementValue(hold, "AV");
		EDI_AddElementValue(hold, "AW");
		EDI_AddElementValue(hold, "AX");
		EDI_AddElementValue(hold, "AY");
		EDI_AddElementValue(hold, "AZ");
		EDI_AddElementValue(hold, "BA");
		EDI_AddElementValue(hold, "BB");
		EDI_AddElementValue(hold, "BC");
		EDI_AddElementValue(hold, "BD");
		EDI_AddElementValue(hold, "BE");
		EDI_AddElementValue(hold, "BF");
		EDI_AddElementValue(hold, "BL");
		EDI_AddElementValue(hold, "BS");
		EDI_AddElementValue(hold, "CA");
		EDI_AddElementValue(hold, "CB");
		EDI_AddElementValue(hold, "CC");
		EDI_AddElementValue(hold, "CD");
		EDI_AddElementValue(hold, "CE");
		EDI_AddElementValue(hold, "CF");
		EDI_AddElementValue(hold, "CH");
		EDI_AddElementValue(hold, "CI");
		EDI_AddElementValue(hold, "CJ");
		EDI_AddElementValue(hold, "CK");
		EDI_AddElementValue(hold, "CL");
		EDI_AddElementValue(hold, "CM");
		EDI_AddElementValue(hold, "CN");
		EDI_AddElementValue(hold, "CO");
		EDI_AddElementValue(hold, "CP");
		EDI_AddElementValue(hold, "CQ");
		EDI_AddElementValue(hold, "CR");
		EDI_AddElementValue(hold, "CS");
		EDI_AddElementValue(hold, "CT");
		EDI_AddElementValue(hold, "CU");
		EDI_AddElementValue(hold, "CV");
		EDI_AddElementValue(hold, "CW");
		EDI_AddElementValue(hold, "DA");
		EDI_AddElementValue(hold, "DD");
		EDI_AddElementValue(hold, "DF");
		EDI_AddElementValue(hold, "DI");
		EDI_AddElementValue(hold, "DM");
		EDI_AddElementValue(hold, "DS");
		EDI_AddElementValue(hold, "DX");
		EDI_AddElementValue(hold, "D3");
		EDI_AddElementValue(hold, "D4");
		EDI_AddElementValue(hold, "D5");
		EDI_AddElementValue(hold, "EC");
		EDI_AddElementValue(hold, "ED");
		EDI_AddElementValue(hold, "EI");
		EDI_AddElementValue(hold, "EN");
		EDI_AddElementValue(hold, "EP");
		EDI_AddElementValue(hold, "ER");
		EDI_AddElementValue(hold, "ES");
		EDI_AddElementValue(hold, "EV");
		EDI_AddElementValue(hold, "EX");
		EDI_AddElementValue(hold, "FA");
		EDI_AddElementValue(hold, "FB");
		EDI_AddElementValue(hold, "FC");
		EDI_AddElementValue(hold, "FG");
		EDI_AddElementValue(hold, "FR");
		EDI_AddElementValue(hold, "FT");
		EDI_AddElementValue(hold, "GC");
		EDI_AddElementValue(hold, "GE");
		EDI_AddElementValue(hold, "GF");
		EDI_AddElementValue(hold, "GL");
		EDI_AddElementValue(hold, "GP");
		EDI_AddElementValue(hold, "GR");
		EDI_AddElementValue(hold, "GT");
		EDI_AddElementValue(hold, "HB");
		EDI_AddElementValue(hold, "HC");
		EDI_AddElementValue(hold, "HI");
		EDI_AddElementValue(hold, "HN");
		EDI_AddElementValue(hold, "HP");
		EDI_AddElementValue(hold, "HR");
		EDI_AddElementValue(hold, "HS");
		EDI_AddElementValue(hold, "HU");
		EDI_AddElementValue(hold, "HV");
		EDI_AddElementValue(hold, "IA");
		EDI_AddElementValue(hold, "IB");
		EDI_AddElementValue(hold, "IC");
		EDI_AddElementValue(hold, "ID");
		EDI_AddElementValue(hold, "IE");
		EDI_AddElementValue(hold, "IF");
		EDI_AddElementValue(hold, "IG");
		EDI_AddElementValue(hold, "IH");
		EDI_AddElementValue(hold, "IJ");
		EDI_AddElementValue(hold, "IM");
		EDI_AddElementValue(hold, "IN");
		EDI_AddElementValue(hold, "IO");
		EDI_AddElementValue(hold, "IR");
		EDI_AddElementValue(hold, "IS");
		EDI_AddElementValue(hold, "JB");
		EDI_AddElementValue(hold, "KM");
		EDI_AddElementValue(hold, "LA");
		EDI_AddElementValue(hold, "LB");
		EDI_AddElementValue(hold, "LI");
		EDI_AddElementValue(hold, "LN");
		EDI_AddElementValue(hold, "LR");
		EDI_AddElementValue(hold, "LS");
		EDI_AddElementValue(hold, "LT");
		EDI_AddElementValue(hold, "MA");
		EDI_AddElementValue(hold, "MC");
		EDI_AddElementValue(hold, "MD");
		EDI_AddElementValue(hold, "ME");
		EDI_AddElementValue(hold, "MF");
		EDI_AddElementValue(hold, "MG");
		EDI_AddElementValue(hold, "MH");
		EDI_AddElementValue(hold, "MI");
		EDI_AddElementValue(hold, "MJ");
		EDI_AddElementValue(hold, "MK");
		EDI_AddElementValue(hold, "MM");
		EDI_AddElementValue(hold, "MN");
		EDI_AddElementValue(hold, "MO");
		EDI_AddElementValue(hold, "MP");
		EDI_AddElementValue(hold, "MQ");
		EDI_AddElementValue(hold, "MR");
		EDI_AddElementValue(hold, "MS");
		EDI_AddElementValue(hold, "MT");
		EDI_AddElementValue(hold, "MV");
		EDI_AddElementValue(hold, "MW");
		EDI_AddElementValue(hold, "MX");
		EDI_AddElementValue(hold, "MY");
		EDI_AddElementValue(hold, "MZ");
		EDI_AddElementValue(hold, "NC");
		EDI_AddElementValue(hold, "NL");
		EDI_AddElementValue(hold, "NP");
		EDI_AddElementValue(hold, "NR");
		EDI_AddElementValue(hold, "NT");
		EDI_AddElementValue(hold, "OC");
		EDI_AddElementValue(hold, "OG");
		EDI_AddElementValue(hold, "OR");
		EDI_AddElementValue(hold, "OW");
		EDI_AddElementValue(hold, "PA");
		EDI_AddElementValue(hold, "PB");
		EDI_AddElementValue(hold, "PC");
		EDI_AddElementValue(hold, "PD");
		EDI_AddElementValue(hold, "PE");
		EDI_AddElementValue(hold, "PF");
		EDI_AddElementValue(hold, "PG");
		EDI_AddElementValue(hold, "PH");
		EDI_AddElementValue(hold, "PI");
		EDI_AddElementValue(hold, "PJ");
		EDI_AddElementValue(hold, "PK");
		EDI_AddElementValue(hold, "PL");
		EDI_AddElementValue(hold, "PN");
		EDI_AddElementValue(hold, "PO");
		EDI_AddElementValue(hold, "PQ");
		EDI_AddElementValue(hold, "PR");
		EDI_AddElementValue(hold, "PS");
		EDI_AddElementValue(hold, "PT");
		EDI_AddElementValue(hold, "PU");
		EDI_AddElementValue(hold, "PV");
		EDI_AddElementValue(hold, "PW");
		EDI_AddElementValue(hold, "PY");
		EDI_AddElementValue(hold, "QG");
		EDI_AddElementValue(hold, "QM");
		EDI_AddElementValue(hold, "QO");
		EDI_AddElementValue(hold, "RA");
		EDI_AddElementValue(hold, "RB");
		EDI_AddElementValue(hold, "RC");
		EDI_AddElementValue(hold, "RD");
		EDI_AddElementValue(hold, "RE");
		EDI_AddElementValue(hold, "RF");
		EDI_AddElementValue(hold, "RG");
		EDI_AddElementValue(hold, "RH");
		EDI_AddElementValue(hold, "RI");
		EDI_AddElementValue(hold, "RJ");
		EDI_AddElementValue(hold, "RK");
		EDI_AddElementValue(hold, "RL");
		EDI_AddElementValue(hold, "RM");
		EDI_AddElementValue(hold, "RN");
		EDI_AddElementValue(hold, "RO");
		EDI_AddElementValue(hold, "RP");
		EDI_AddElementValue(hold, "RQ");
		EDI_AddElementValue(hold, "RR");
		EDI_AddElementValue(hold, "RS");
		EDI_AddElementValue(hold, "RT");
		EDI_AddElementValue(hold, "RU");
		EDI_AddElementValue(hold, "RV");
		EDI_AddElementValue(hold, "RW");
		EDI_AddElementValue(hold, "RX");
		EDI_AddElementValue(hold, "RY");
		EDI_AddElementValue(hold, "RZ");
		EDI_AddElementValue(hold, "SA");
		EDI_AddElementValue(hold, "SB");
		EDI_AddElementValue(hold, "SC");
		EDI_AddElementValue(hold, "SD");
		EDI_AddElementValue(hold, "SE");
		EDI_AddElementValue(hold, "SH");
		EDI_AddElementValue(hold, "SI");
		EDI_AddElementValue(hold, "SJ");
		EDI_AddElementValue(hold, "SL");
		EDI_AddElementValue(hold, "SM");
		EDI_AddElementValue(hold, "SN");
		EDI_AddElementValue(hold, "SO");
		EDI_AddElementValue(hold, "SP");
		EDI_AddElementValue(hold, "SQ");
		EDI_AddElementValue(hold, "SR");
		EDI_AddElementValue(hold, "SS");
		EDI_AddElementValue(hold, "ST");
		EDI_AddElementValue(hold, "SU");
		EDI_AddElementValue(hold, "SV");
		EDI_AddElementValue(hold, "SW");
		EDI_AddElementValue(hold, "TA");
		EDI_AddElementValue(hold, "TB");
		EDI_AddElementValue(hold, "TD");
		EDI_AddElementValue(hold, "TE");
		EDI_AddElementValue(hold, "TF");
		EDI_AddElementValue(hold, "TI");
		EDI_AddElementValue(hold, "TJ");
		EDI_AddElementValue(hold, "TM");
		EDI_AddElementValue(hold, "TN");
		EDI_AddElementValue(hold, "TO");
		EDI_AddElementValue(hold, "TP");
		EDI_AddElementValue(hold, "TR");
		EDI_AddElementValue(hold, "TS");
		EDI_AddElementValue(hold, "TT");
		EDI_AddElementValue(hold, "TU");
		EDI_AddElementValue(hold, "TX");
		EDI_AddElementValue(hold, "UA");
		EDI_AddElementValue(hold, "UB");
		EDI_AddElementValue(hold, "UC");
		EDI_AddElementValue(hold, "UD");
		EDI_AddElementValue(hold, "UI");
		EDI_AddElementValue(hold, "UP");
		EDI_AddElementValue(hold, "UW");
		EDI_AddElementValue(hold, "VA");
		EDI_AddElementValue(hold, "VB");
		EDI_AddElementValue(hold, "VC");
		EDI_AddElementValue(hold, "VD");
		EDI_AddElementValue(hold, "VE");
		EDI_AddElementValue(hold, "VH");
		EDI_AddElementValue(hold, "VI");
		EDI_AddElementValue(hold, "VS");
		EDI_AddElementValue(hold, "WA");
		EDI_AddElementValue(hold, "WB");
		EDI_AddElementValue(hold, "WG");
		EDI_AddElementValue(hold, "WI");
		EDI_AddElementValue(hold, "WL");
		EDI_AddElementValue(hold, "WR");
		EDI_AddElementValue(hold, "WT");*/
		EDI_CreateElementType(s, EDI_DATA_STRING, "480", 1, 12);

		hold = EDI_CreateComplexType(s, EDITYPE_SEGMENT, "ISA");
		EDI_AppendType(hold, EDI_GetElementByID(s, "I01"), 1, 1);
		EDI_AppendType(hold, EDI_GetElementByID(s, "I02"), 1, 1);
		EDI_AppendType(hold, EDI_GetElementByID(s, "I03"), 1, 1);
		EDI_AppendType(hold, EDI_GetElementByID(s, "I04"), 1, 1);
		EDI_AppendType(hold, EDI_GetElementByID(s, "I05"), 1, 1);
		EDI_AppendType(hold, EDI_GetElementByID(s, "I06"), 1, 1);
		EDI_AppendType(hold, EDI_GetElementByID(s, "I05"), 1, 1);
		EDI_AppendType(hold, EDI_GetElementByID(s, "I07"), 1, 1);
		EDI_AppendType(hold, EDI_GetElementByID(s, "I08"), 1, 1);
		EDI_AppendType(hold, EDI_GetElementByID(s, "I09"), 1, 1);
		EDI_AppendType(hold, EDI_GetElementByID(s, "I65"), 1, 1);
		EDI_AppendType(hold, EDI_GetElementByID(s, "I11"), 1, 1);
		EDI_AppendType(hold, EDI_GetElementByID(s, "I12"), 1, 1);
		EDI_AppendType(hold, EDI_GetElementByID(s, "I13"), 1, 1);
		EDI_AppendType(hold, EDI_GetElementByID(s, "I14"), 1, 1);
		EDI_AppendType(hold, EDI_GetElementByID(s, "I15"), 1, 1);
		EDI_StoreComplexNode(s, hold);
		
		hold = EDI_CreateComplexType(s, EDITYPE_SEGMENT, "IEA");
		EDI_AppendType(hold, EDI_GetElementByID(s, "I16"), 1, 1);
		EDI_AppendType(hold, EDI_GetElementByID(s, "I12"), 1, 1);
		EDI_StoreComplexNode(s, hold);

		hold = EDI_CreateComplexType(s, EDITYPE_SEGMENT, "GS");
		EDI_AppendType(hold, EDI_GetElementByID(s, "479"), 1, 1);
		EDI_AppendType(hold, EDI_GetElementByID(s, "142"), 1, 1);
		EDI_AppendType(hold, EDI_GetElementByID(s, "124"), 1, 1);
		EDI_AppendType(hold, EDI_GetElementByID(s, "373"), 1, 1);
		EDI_AppendType(hold, EDI_GetElementByID(s, "337"), 1, 1);
		EDI_AppendType(hold, EDI_GetElementByID(s, "28"), 1, 1);
		EDI_AppendType(hold, EDI_GetElementByID(s, "455"), 1, 1);
		EDI_AppendType(hold, EDI_GetElementByID(s, "480"), 1, 1);
		EDI_StoreComplexNode(s, hold);

		hold = EDI_CreateComplexType(s, EDITYPE_SEGMENT, "GE");
		EDI_AppendType(hold, EDI_GetElementByID(s, "28"), 1, 1);
		EDI_AppendType(hold, EDI_GetElementByID(s, "97"), 1, 1);
		EDI_StoreComplexNode(s, hold);

		hold = EDI_CreateComplexType(s, EDITYPE_DOCUMENT, "interchange");
		EDI_AppendType(hold, EDI_GetComplexNodeByID(s, "ISA"), 1, 1);

		EDI_SchemaNode group = EDI_CreateComplexType(s, EDITYPE_LOOP, "group");
		EDI_AppendType(group, EDI_GetComplexNodeByID(s, "GS"), 1, 1);
		EDI_AppendType(group, EDI_GetComplexNodeByID(s, "GE"), 1, 1);
		EDI_StoreComplexNode(s, group);

		EDI_AppendType(hold, group, 0, 999999999);
		EDI_AppendType(hold, EDI_GetComplexNodeByID(s, "IEA"), 1, 1);
		EDI_StoreComplexNode(s, hold);
		
		EDI_SetDocumentRoot(s, EDI_GetComplexNodeByID(s, "interchange"));
		EDI_SetSegmentErrorHandler(s, &handleSegmentError);
		EDI_SetElementErrorHandler(s, &handleElementError);
		EDI_SetLoopStartHandler(s, &handleLoopStart);
		EDI_SetLoopEndHandler(s, &handleLoopEnd);
	}
}

void load_standard(EDI_Parser p)
{
	EDI_Schema s = NULL;
	EDI_SchemaNode parent = NULL;
	EDI_SchemaNode child  = NULL;
	
	s = EDI_GetSchema(p);
	if(!s || (s && strcmp(EDI_GetSchemaId(s), "Functional Acknowledgment: FA/997") != 0)){
		load_basic_standards(p);
		s = EDI_GetSchema(p);
		EDI_SetSchemaId(s, "Functional Acknowledgment: FA/997");

		EDI_CreateElementType(s, EDI_DATA_INTEGER, "2", 1, 6);
		EDI_CreateElementType(s, EDI_DATA_INTEGER, "96", 1, 10);
		EDI_CreateElementType(s, EDI_DATA_INTEGER, "123", 1, 6);
		parent = EDI_CreateElementType(s, EDI_DATA_STRING, "my143", 3, 3);
		EDI_AddElementValue(parent, "997");
		parent = EDI_CreateElementType(s, EDI_DATA_STRING, "143", 3, 3);
		/*EDI_AddElementValue(parent, "100");
		EDI_AddElementValue(parent, "101");
		EDI_AddElementValue(parent, "102");
		EDI_AddElementValue(parent, "103");
		EDI_AddElementValue(parent, "104");
		EDI_AddElementValue(parent, "105");
		EDI_AddElementValue(parent, "106");
		EDI_AddElementValue(parent, "107");
		EDI_AddElementValue(parent, "108");
		EDI_AddElementValue(parent, "109");
		EDI_AddElementValue(parent, "110");
		EDI_AddElementValue(parent, "111");
		EDI_AddElementValue(parent, "112");
		EDI_AddElementValue(parent, "113");
		EDI_AddElementValue(parent, "120");
		EDI_AddElementValue(parent, "121");
		EDI_AddElementValue(parent, "124");
		EDI_AddElementValue(parent, "125");
		EDI_AddElementValue(parent, "126");
		EDI_AddElementValue(parent, "127");
		EDI_AddElementValue(parent, "128");
		EDI_AddElementValue(parent, "129");
		EDI_AddElementValue(parent, "130");
		EDI_AddElementValue(parent, "131");
		EDI_AddElementValue(parent, "132");
		EDI_AddElementValue(parent, "133");
		EDI_AddElementValue(parent, "135");
		EDI_AddElementValue(parent, "138");
		EDI_AddElementValue(parent, "139");
		EDI_AddElementValue(parent, "140");
		EDI_AddElementValue(parent, "141");
		EDI_AddElementValue(parent, "142");
		EDI_AddElementValue(parent, "143");
		EDI_AddElementValue(parent, "144");
		EDI_AddElementValue(parent, "146");
		EDI_AddElementValue(parent, "147");
		EDI_AddElementValue(parent, "148");
		EDI_AddElementValue(parent, "149");
		EDI_AddElementValue(parent, "150");
		EDI_AddElementValue(parent, "151");
		EDI_AddElementValue(parent, "152");
		EDI_AddElementValue(parent, "153");
		EDI_AddElementValue(parent, "154");
		EDI_AddElementValue(parent, "155");
		EDI_AddElementValue(parent, "157");
		EDI_AddElementValue(parent, "158");
		EDI_AddElementValue(parent, "159");
		EDI_AddElementValue(parent, "160");
		EDI_AddElementValue(parent, "161");
		EDI_AddElementValue(parent, "163");
		EDI_AddElementValue(parent, "170");
		EDI_AddElementValue(parent, "175");
		EDI_AddElementValue(parent, "176");
		EDI_AddElementValue(parent, "179");
		EDI_AddElementValue(parent, "180");
		EDI_AddElementValue(parent, "185");
		EDI_AddElementValue(parent, "186");
		EDI_AddElementValue(parent, "187");
		EDI_AddElementValue(parent, "188");
		EDI_AddElementValue(parent, "189");
		EDI_AddElementValue(parent, "190");
		EDI_AddElementValue(parent, "191");
		EDI_AddElementValue(parent, "194");
		EDI_AddElementValue(parent, "195");
		EDI_AddElementValue(parent, "196");
		EDI_AddElementValue(parent, "197");
		EDI_AddElementValue(parent, "198");
		EDI_AddElementValue(parent, "199");
		EDI_AddElementValue(parent, "200");
		EDI_AddElementValue(parent, "201");
		EDI_AddElementValue(parent, "202");
		EDI_AddElementValue(parent, "203");
		EDI_AddElementValue(parent, "204");
		EDI_AddElementValue(parent, "205");
		EDI_AddElementValue(parent, "206");
		EDI_AddElementValue(parent, "210");
		EDI_AddElementValue(parent, "211");
		EDI_AddElementValue(parent, "212");
		EDI_AddElementValue(parent, "213");
		EDI_AddElementValue(parent, "214");
		EDI_AddElementValue(parent, "215");
		EDI_AddElementValue(parent, "216");
		EDI_AddElementValue(parent, "217");
		EDI_AddElementValue(parent, "219");
		EDI_AddElementValue(parent, "220");
		EDI_AddElementValue(parent, "222");
		EDI_AddElementValue(parent, "223");
		EDI_AddElementValue(parent, "224");
		EDI_AddElementValue(parent, "225");
		EDI_AddElementValue(parent, "227");
		EDI_AddElementValue(parent, "228");
		EDI_AddElementValue(parent, "240");
		EDI_AddElementValue(parent, "242");
		EDI_AddElementValue(parent, "244");
		EDI_AddElementValue(parent, "245");
		EDI_AddElementValue(parent, "248");
		EDI_AddElementValue(parent, "249");
		EDI_AddElementValue(parent, "250");
		EDI_AddElementValue(parent, "251");
		EDI_AddElementValue(parent, "252");
		EDI_AddElementValue(parent, "255");
		EDI_AddElementValue(parent, "256");
		EDI_AddElementValue(parent, "259");
		EDI_AddElementValue(parent, "260");
		EDI_AddElementValue(parent, "261");
		EDI_AddElementValue(parent, "262");
		EDI_AddElementValue(parent, "263");
		EDI_AddElementValue(parent, "264");
		EDI_AddElementValue(parent, "265");
		EDI_AddElementValue(parent, "266");
		EDI_AddElementValue(parent, "267");
		EDI_AddElementValue(parent, "268");
		EDI_AddElementValue(parent, "269");
		EDI_AddElementValue(parent, "270");
		EDI_AddElementValue(parent, "271");
		EDI_AddElementValue(parent, "272");
		EDI_AddElementValue(parent, "273");
		EDI_AddElementValue(parent, "274");
		EDI_AddElementValue(parent, "275");
		EDI_AddElementValue(parent, "276");
		EDI_AddElementValue(parent, "277");
		EDI_AddElementValue(parent, "278");
		EDI_AddElementValue(parent, "280");
		EDI_AddElementValue(parent, "283");
		EDI_AddElementValue(parent, "284");
		EDI_AddElementValue(parent, "285");
		EDI_AddElementValue(parent, "286");
		EDI_AddElementValue(parent, "288");
		EDI_AddElementValue(parent, "290");
		EDI_AddElementValue(parent, "300");
		EDI_AddElementValue(parent, "301");
		EDI_AddElementValue(parent, "303");
		EDI_AddElementValue(parent, "304");
		EDI_AddElementValue(parent, "309");
		EDI_AddElementValue(parent, "310");
		EDI_AddElementValue(parent, "311");
		EDI_AddElementValue(parent, "312");
		EDI_AddElementValue(parent, "313");
		EDI_AddElementValue(parent, "315");
		EDI_AddElementValue(parent, "317");
		EDI_AddElementValue(parent, "319");
		EDI_AddElementValue(parent, "322");
		EDI_AddElementValue(parent, "323");
		EDI_AddElementValue(parent, "324");
		EDI_AddElementValue(parent, "325");
		EDI_AddElementValue(parent, "326");
		EDI_AddElementValue(parent, "350");
		EDI_AddElementValue(parent, "352");
		EDI_AddElementValue(parent, "353");
		EDI_AddElementValue(parent, "354");
		EDI_AddElementValue(parent, "355");
		EDI_AddElementValue(parent, "356");
		EDI_AddElementValue(parent, "357");
		EDI_AddElementValue(parent, "358");
		EDI_AddElementValue(parent, "361");
		EDI_AddElementValue(parent, "362");
		EDI_AddElementValue(parent, "404");
		EDI_AddElementValue(parent, "410");
		EDI_AddElementValue(parent, "412");
		EDI_AddElementValue(parent, "414");
		EDI_AddElementValue(parent, "417");
		EDI_AddElementValue(parent, "418");
		EDI_AddElementValue(parent, "419");
		EDI_AddElementValue(parent, "420");
		EDI_AddElementValue(parent, "421");
		EDI_AddElementValue(parent, "422");
		EDI_AddElementValue(parent, "423");
		EDI_AddElementValue(parent, "424");
		EDI_AddElementValue(parent, "425");
		EDI_AddElementValue(parent, "426");
		EDI_AddElementValue(parent, "429");
		EDI_AddElementValue(parent, "431");
		EDI_AddElementValue(parent, "432");
		EDI_AddElementValue(parent, "433");
		EDI_AddElementValue(parent, "434");
		EDI_AddElementValue(parent, "435");
		EDI_AddElementValue(parent, "436");
		EDI_AddElementValue(parent, "437");
		EDI_AddElementValue(parent, "440");
		EDI_AddElementValue(parent, "451");
		EDI_AddElementValue(parent, "452");
		EDI_AddElementValue(parent, "453");
		EDI_AddElementValue(parent, "455");
		EDI_AddElementValue(parent, "456");
		EDI_AddElementValue(parent, "460");
		EDI_AddElementValue(parent, "463");
		EDI_AddElementValue(parent, "466");
		EDI_AddElementValue(parent, "468");
		EDI_AddElementValue(parent, "470");
		EDI_AddElementValue(parent, "475");
		EDI_AddElementValue(parent, "485");
		EDI_AddElementValue(parent, "486");
		EDI_AddElementValue(parent, "490");
		EDI_AddElementValue(parent, "492");
		EDI_AddElementValue(parent, "494");
		EDI_AddElementValue(parent, "500");
		EDI_AddElementValue(parent, "501");
		EDI_AddElementValue(parent, "503");
		EDI_AddElementValue(parent, "504");
		EDI_AddElementValue(parent, "511");
		EDI_AddElementValue(parent, "517");
		EDI_AddElementValue(parent, "521");
		EDI_AddElementValue(parent, "527");
		EDI_AddElementValue(parent, "536");
		EDI_AddElementValue(parent, "540");
		EDI_AddElementValue(parent, "561");
		EDI_AddElementValue(parent, "567");
		EDI_AddElementValue(parent, "568");
		EDI_AddElementValue(parent, "601");
		EDI_AddElementValue(parent, "602");
		EDI_AddElementValue(parent, "620");
		EDI_AddElementValue(parent, "625");
		EDI_AddElementValue(parent, "650");
		EDI_AddElementValue(parent, "715");
		EDI_AddElementValue(parent, "753");
		EDI_AddElementValue(parent, "754");
		EDI_AddElementValue(parent, "805");
		EDI_AddElementValue(parent, "806");
		EDI_AddElementValue(parent, "810");
		EDI_AddElementValue(parent, "811");
		EDI_AddElementValue(parent, "812");
		EDI_AddElementValue(parent, "813");
		EDI_AddElementValue(parent, "814");
		EDI_AddElementValue(parent, "815");
		EDI_AddElementValue(parent, "816");
		EDI_AddElementValue(parent, "818");
		EDI_AddElementValue(parent, "819");
		EDI_AddElementValue(parent, "820");
		EDI_AddElementValue(parent, "821");
		EDI_AddElementValue(parent, "822");
		EDI_AddElementValue(parent, "823");
		EDI_AddElementValue(parent, "824");
		EDI_AddElementValue(parent, "826");
		EDI_AddElementValue(parent, "827");
		EDI_AddElementValue(parent, "828");
		EDI_AddElementValue(parent, "829");
		EDI_AddElementValue(parent, "830");
		EDI_AddElementValue(parent, "831");
		EDI_AddElementValue(parent, "832");
		EDI_AddElementValue(parent, "833");
		EDI_AddElementValue(parent, "834");
		EDI_AddElementValue(parent, "835");
		EDI_AddElementValue(parent, "836");
		EDI_AddElementValue(parent, "837");
		EDI_AddElementValue(parent, "838");
		EDI_AddElementValue(parent, "839");
		EDI_AddElementValue(parent, "840");
		EDI_AddElementValue(parent, "841");
		EDI_AddElementValue(parent, "842");
		EDI_AddElementValue(parent, "843");
		EDI_AddElementValue(parent, "844");
		EDI_AddElementValue(parent, "845");
		EDI_AddElementValue(parent, "846");
		EDI_AddElementValue(parent, "847");
		EDI_AddElementValue(parent, "848");
		EDI_AddElementValue(parent, "849");
		EDI_AddElementValue(parent, "850");
		EDI_AddElementValue(parent, "851");
		EDI_AddElementValue(parent, "852");
		EDI_AddElementValue(parent, "853");
		EDI_AddElementValue(parent, "854");
		EDI_AddElementValue(parent, "855");
		EDI_AddElementValue(parent, "856");
		EDI_AddElementValue(parent, "857");
		EDI_AddElementValue(parent, "858");
		EDI_AddElementValue(parent, "859");
		EDI_AddElementValue(parent, "860");
		EDI_AddElementValue(parent, "861");
		EDI_AddElementValue(parent, "862");
		EDI_AddElementValue(parent, "863");
		EDI_AddElementValue(parent, "864");
		EDI_AddElementValue(parent, "865");
		EDI_AddElementValue(parent, "866");
		EDI_AddElementValue(parent, "867");
		EDI_AddElementValue(parent, "868");
		EDI_AddElementValue(parent, "869");
		EDI_AddElementValue(parent, "870");
		EDI_AddElementValue(parent, "871");
		EDI_AddElementValue(parent, "872");
		EDI_AddElementValue(parent, "873");
		EDI_AddElementValue(parent, "874");
		EDI_AddElementValue(parent, "875");
		EDI_AddElementValue(parent, "876");
		EDI_AddElementValue(parent, "877");
		EDI_AddElementValue(parent, "878");
		EDI_AddElementValue(parent, "879");
		EDI_AddElementValue(parent, "880");
		EDI_AddElementValue(parent, "881");
		EDI_AddElementValue(parent, "882");
		EDI_AddElementValue(parent, "883");
		EDI_AddElementValue(parent, "884");
		EDI_AddElementValue(parent, "885");
		EDI_AddElementValue(parent, "886");
		EDI_AddElementValue(parent, "887");
		EDI_AddElementValue(parent, "888");
		EDI_AddElementValue(parent, "889");
		EDI_AddElementValue(parent, "891");
		EDI_AddElementValue(parent, "893");
		EDI_AddElementValue(parent, "894");
		EDI_AddElementValue(parent, "895");
		EDI_AddElementValue(parent, "896");
		EDI_AddElementValue(parent, "920");
		EDI_AddElementValue(parent, "924");
		EDI_AddElementValue(parent, "925");
		EDI_AddElementValue(parent, "926");
		EDI_AddElementValue(parent, "928");
		EDI_AddElementValue(parent, "940");
		EDI_AddElementValue(parent, "943");
		EDI_AddElementValue(parent, "944");
		EDI_AddElementValue(parent, "945");
		EDI_AddElementValue(parent, "947");
		EDI_AddElementValue(parent, "980");
		EDI_AddElementValue(parent, "990");
		EDI_AddElementValue(parent, "993");
		EDI_AddElementValue(parent, "996");
		EDI_AddElementValue(parent, "997");
		EDI_AddElementValue(parent, "998");
		EDI_AddElementValue(parent, "999");*/
		EDI_CreateElementType(s, EDI_DATA_STRING, "329", 4, 9);
		EDI_CreateElementType(s, EDI_DATA_STRING, "447", 1, 4);
		parent = EDI_CreateElementType(s, EDI_DATA_STRING, "715", 1, 1);
		/*EDI_AddElementValue(parent, "A");
		EDI_AddElementValue(parent, "E");
		EDI_AddElementValue(parent, "M");
		EDI_AddElementValue(parent, "P");
		EDI_AddElementValue(parent, "R");
		EDI_AddElementValue(parent, "W");
		EDI_AddElementValue(parent, "X");*/
		parent = EDI_CreateElementType(s, EDI_DATA_STRING, "716", 1, 3);
		/*EDI_AddElementValue(parent, "1");
		EDI_AddElementValue(parent, "2");
		EDI_AddElementValue(parent, "3");
		EDI_AddElementValue(parent, "4");
		EDI_AddElementValue(parent, "5");
		EDI_AddElementValue(parent, "6");
		EDI_AddElementValue(parent, "10");
		EDI_AddElementValue(parent, "11");
		EDI_AddElementValue(parent, "12");
		EDI_AddElementValue(parent, "13");
		EDI_AddElementValue(parent, "14");
		EDI_AddElementValue(parent, "15");
		EDI_AddElementValue(parent, "16");
		EDI_AddElementValue(parent, "17");
		EDI_AddElementValue(parent, "18");
		EDI_AddElementValue(parent, "19");
		EDI_AddElementValue(parent, "23");
		EDI_AddElementValue(parent, "24");
		EDI_AddElementValue(parent, "25");
		EDI_AddElementValue(parent, "26");*/
		parent = EDI_CreateElementType(s, EDI_DATA_STRING, "717", 1, 1);
		/*EDI_AddElementValue(parent, "A");
		EDI_AddElementValue(parent, "E");
		EDI_AddElementValue(parent, "M");
		EDI_AddElementValue(parent, "R");
		EDI_AddElementValue(parent, "W");
		EDI_AddElementValue(parent, "X");*/
		parent = EDI_CreateElementType(s, EDI_DATA_STRING, "718", 1, 3);
		/*EDI_AddElementValue(parent, "1");
		EDI_AddElementValue(parent, "2");
		EDI_AddElementValue(parent, "3");
		EDI_AddElementValue(parent, "4");
		EDI_AddElementValue(parent, "5");
		EDI_AddElementValue(parent, "6");
		EDI_AddElementValue(parent, "7");
		EDI_AddElementValue(parent, "8");
		EDI_AddElementValue(parent, "9");
		EDI_AddElementValue(parent, "10");
		EDI_AddElementValue(parent, "11");
		EDI_AddElementValue(parent, "12");
		EDI_AddElementValue(parent, "13");
		EDI_AddElementValue(parent, "15");
		EDI_AddElementValue(parent, "16");
		EDI_AddElementValue(parent, "17");
		EDI_AddElementValue(parent, "18");
		EDI_AddElementValue(parent, "19");
		EDI_AddElementValue(parent, "23");
		EDI_AddElementValue(parent, "24");
		EDI_AddElementValue(parent, "25");
		EDI_AddElementValue(parent, "26");
		EDI_AddElementValue(parent, "27");*/
		EDI_CreateElementType(s, EDI_DATA_INTEGER, "719", 1, 10);
		parent = EDI_CreateElementType(s, EDI_DATA_STRING, "720", 1, 3);
		/*EDI_AddElementValue(parent, "1");
		EDI_AddElementValue(parent, "2");
		EDI_AddElementValue(parent, "3");
		EDI_AddElementValue(parent, "4");
		EDI_AddElementValue(parent, "5");
		EDI_AddElementValue(parent, "6");
		EDI_AddElementValue(parent, "7");
		EDI_AddElementValue(parent, "8");*/
		EDI_CreateElementType(s, EDI_DATA_STRING, "721", 2, 3);
		EDI_CreateElementType(s, EDI_DATA_INTEGER, "722", 1, 2);
		parent = EDI_CreateElementType(s, EDI_DATA_STRING, "723", 1, 3);
		/*EDI_AddElementValue(parent, "1");
		EDI_AddElementValue(parent, "2");
		EDI_AddElementValue(parent, "3");
		EDI_AddElementValue(parent, "4");
		EDI_AddElementValue(parent, "5");
		EDI_AddElementValue(parent, "6");
		EDI_AddElementValue(parent, "7");
		EDI_AddElementValue(parent, "8");
		EDI_AddElementValue(parent, "9");
		EDI_AddElementValue(parent, "10");
		EDI_AddElementValue(parent, "12");
		EDI_AddElementValue(parent, "13");*/
		EDI_CreateElementType(s, EDI_DATA_STRING, "724", 1, 99);
		EDI_CreateElementType(s, EDI_DATA_INTEGER, "725", 1, 4);
		EDI_CreateElementType(s, EDI_DATA_INTEGER, "1528", 1, 2);
		EDI_CreateElementType(s, EDI_DATA_INTEGER, "1686", 1, 4);
		EDI_CreateElementType(s, EDI_DATA_STRING, "1705", 1, 35);

		parent = EDI_CreateComplexType(s, EDITYPE_COMPOSITE, "C030");
		EDI_AppendType(parent, EDI_GetElementByID(s, "722"), 1, 1);
		EDI_AppendType(parent, EDI_GetElementByID(s, "1528"), 0, 1);
		EDI_AppendType(parent, EDI_GetElementByID(s, "1686"), 0, 1);
		EDI_StoreComplexNode(s, parent);

		parent = EDI_CreateComplexType(s, EDITYPE_SEGMENT, "ST");
		EDI_AppendType(parent, EDI_GetElementByID(s, "my143"), 1, 1);
		EDI_AppendType(parent, EDI_GetElementByID(s, "329"), 1, 1);
		EDI_StoreComplexNode(s, parent);

		parent = EDI_CreateComplexType(s, EDITYPE_SEGMENT, "AK1");
		EDI_AppendType(parent, EDI_GetElementByID(s, "479"), 1, 1);
		EDI_AppendType(parent, EDI_GetElementByID(s, "28"), 1, 1);
		EDI_AppendType(parent, EDI_GetElementByID(s, "480"), 0, 1);
		EDI_StoreComplexNode(s, parent);

		parent = EDI_CreateComplexType(s, EDITYPE_SEGMENT, "AK2");
		EDI_AppendType(parent, EDI_GetElementByID(s, "143"), 1, 1);
		EDI_AppendType(parent, EDI_GetElementByID(s, "329"), 1, 1);
		EDI_AppendType(parent, EDI_GetElementByID(s, "1705"), 0, 1);
		EDI_StoreComplexNode(s, parent);

		parent = EDI_CreateComplexType(s, EDITYPE_SEGMENT, "AK3");
		EDI_AppendType(parent, EDI_GetElementByID(s, "721"), 1, 1);
		EDI_AppendType(parent, EDI_GetElementByID(s, "719"), 1, 1);
		EDI_AppendType(parent, EDI_GetElementByID(s, "447"), 0, 1);
		EDI_AppendType(parent, EDI_GetElementByID(s, "720"), 0, 1);
		EDI_StoreComplexNode(s, parent);

		parent = EDI_CreateComplexType(s, EDITYPE_SEGMENT, "AK4");
		EDI_AppendType(parent, EDI_GetComplexNodeByID(s, "C030"), 1, 1);
		EDI_AppendType(parent, EDI_GetElementByID(s, "725"), 0, 1);
		EDI_AppendType(parent, EDI_GetElementByID(s, "723"), 1, 1);
		EDI_AppendType(parent, EDI_GetElementByID(s, "724"), 0, 1);
		EDI_StoreComplexNode(s, parent);

		parent = EDI_CreateComplexType(s, EDITYPE_SEGMENT, "AK5");
		EDI_AppendType(parent, EDI_GetElementByID(s, "717"), 1, 1);
		EDI_AppendType(parent, EDI_GetElementByID(s, "718"), 0, 1);
		EDI_AppendType(parent, EDI_GetElementByID(s, "718"), 0, 1);
		EDI_AppendType(parent, EDI_GetElementByID(s, "718"), 0, 1);
		EDI_AppendType(parent, EDI_GetElementByID(s, "718"), 0, 1);
		EDI_AppendType(parent, EDI_GetElementByID(s, "718"), 0, 1);
		EDI_StoreComplexNode(s, parent);

		parent = EDI_CreateComplexType(s, EDITYPE_SEGMENT, "AK9");
		EDI_AppendType(parent, EDI_GetElementByID(s, "715"), 1, 1);
		EDI_AppendType(parent, EDI_GetElementByID(s, "97"), 1, 1);
		EDI_AppendType(parent, EDI_GetElementByID(s, "123"), 1, 1);
		EDI_AppendType(parent, EDI_GetElementByID(s, "2"), 1, 1);
		EDI_AppendType(parent, EDI_GetElementByID(s, "716"), 0, 1);
		EDI_AppendType(parent, EDI_GetElementByID(s, "716"), 0, 1);
		EDI_AppendType(parent, EDI_GetElementByID(s, "716"), 0, 1);
		EDI_AppendType(parent, EDI_GetElementByID(s, "716"), 0, 1);
		EDI_AppendType(parent, EDI_GetElementByID(s, "716"), 0, 1);
		EDI_StoreComplexNode(s, parent);

		parent = EDI_CreateComplexType(s, EDITYPE_SEGMENT, "SE");
		EDI_AppendType(parent, EDI_GetElementByID(s, "96"), 1, 1);
		EDI_AppendType(parent, EDI_GetElementByID(s, "329"), 1, 1);
		EDI_StoreComplexNode(s, parent);

		EDI_SchemaNode root = EDI_CreateComplexType(s, EDITYPE_LOOP, "transaction");
		EDI_AppendType(root, EDI_GetComplexNodeByID(s, "ST"), 1, 1);
		EDI_AppendType(root, EDI_GetComplexNodeByID(s, "AK1"), 1, 1);
		EDI_SchemaNode loop2000 = EDI_CreateComplexType(s, EDITYPE_LOOP, "loop2000");
		EDI_AppendType(loop2000, EDI_GetComplexNodeByID(s, "AK2"), 1, 1);
		EDI_SchemaNode loop2100 = EDI_CreateComplexType(s, EDITYPE_LOOP, "loop2100");
		EDI_AppendType(loop2100, EDI_GetComplexNodeByID(s, "AK3"), 1, 1);
		EDI_AppendType(loop2100, EDI_GetComplexNodeByID(s, "AK4"), 0, 99);
		EDI_StoreComplexNode(s, loop2100);
		EDI_AppendType(loop2000, loop2100, 0, 999999);
		EDI_AppendType(loop2000, EDI_GetComplexNodeByID(s, "AK5"), 1, 1);
		EDI_StoreComplexNode(s, loop2000);
		EDI_AppendType(root, loop2000, 0, 999999);
		EDI_AppendType(root, EDI_GetComplexNodeByID(s, "AK9"), 1, 1);
		EDI_AppendType(root, EDI_GetComplexNodeByID(s, "SE"), 1, 1);
		
		EDI_InsertType(EDI_GetComplexNodeByID(s, "group"), root, 2, 0, 999999999);
	}
	return;
}

void handleDocumentStart(void *myData, enum EDI_DocumentType type)
{
	fprintf(stdout, "%s<document>\n", prefix);
	depth++;
	prefix[0] = '\0';
	for(int i = 0; i < depth; i++){
		strcat(prefix, "   ");
	}
	return;
}

void handleDocumentEnd(void *myData)
{
	depth--;
	prefix[0] = '\0';
	for(int i = 0; i < depth; i++){
		strcat(prefix, "   ");
	}
	fprintf(stdout, "%s</document>\n", prefix);
	return;
}

void handleSegmentStart(void *myData, const char *tag)
{
	fprintf(stdout, "%sSeg: %3s ->", prefix, tag);
	strcpy(curr_seg, tag);
	curr_e = 0;
	return;
}

void handleSegmentEnd(void *myData, const char *tag)
{
    fprintf(stdout, " <- End %s\n", tag);
    return;
}

void handleCompositeStart(void *myData)
{
	curr_e++;
	curr_c = 0;
	is_comp = 1;
	fprintf(stdout, "{");
	return;
}

void handleCompositeEnd(void *myData)
{
    fprintf(stdout, "}");
    is_comp = 0;
    return;
}

void handleElement(void *myData, EDI_DataElement element)
{
	const char *string;

	counter++;
	switch(element->type){
		case EDI_DATA_INTEGER:
		case EDI_DATA_BINARY_SIZE:
			fprintf(stdout, "[%d]", element->data.integer);
			break;
		case EDI_DATA_DECIMAL:
			fprintf(stdout, "[%ld]", element->data.decimal);
			break;
		default:
			fprintf(stdout, "[%s]", element->data.string);
			if(is_comp){
				curr_c++;
			} else {
				curr_e++;
			}
			if(group_start){
				if(counter == 1){
					if(strcmp("FA", element->data.string) == 0){
						type_ok = 1;
					} else {
						fprintf(stderr, "*** Type not 'FA': No transaction-level validation will be performed on this functional group.\n");
					}
				} else if(counter == 8){
					if(type_ok){
						if((strncmp("005010", element->data.string, 6) == 0)){
							load_standard((EDI_Parser)myData);
						} else {
							fprintf(stderr, "*** Version not '005010': No transaction-level validation will be performed on this functional group.\n");
						}
					}
					group_start = 0;
				}
			}
			break;
	}
	return;
}

void handleJunk(void *myData, const char *val)
{
    fprintf(stderr, "Junk: >>{%s}<<\n", val);
    return;

}

int main(int argc, char **argv)
{
	int bytes_read, file_stat_size;
	double file_size = 0;
	double speed = 0;
	int input, i, length;
	void *buff = NULL;
	unsigned long j = 0;
	unsigned int size;
	struct timeval tv;
	double start_time, end_time, run_time;
	struct stat statbuf;
	EDI_Parser p = NULL;
	enum EDI_Status status;
	FILE *fin;

	if(argc < 2){
		fprintf(stderr, "Not enough arguments, exiting.\n");
		exit(1);
	}
	
	gettimeofday(&tv, NULL);
	start_time = tv.tv_sec + (tv.tv_usec * 0.000001);

	if((input = open(argv[1], O_RDONLY)) < 0){
		fprintf(stderr, "Couldn't open\n");
		exit(1);
	}
	if (fstat (input, &statbuf) < 0){
		fprintf(stderr, "Couldn't stat\n");
		exit(1);
	}
        
	p = EDI_ParserCreate();
	
	EDI_SetDocumentStartHandler(p, handleDocumentStart);
	EDI_SetDocumentEndHandler(p, handleDocumentEnd);
	EDI_SetSegmentStartHandler(p, handleSegmentStart);
	EDI_SetSegmentEndHandler(p, handleSegmentEnd);
	EDI_SetCompositeStartHandler(p, handleCompositeStart);
	EDI_SetCompositeEndHandler(p, handleCompositeEnd);
	EDI_SetElementHandler(p, handleElement);
	EDI_SetNonEDIDataHandler(p, handleJunk);
	EDI_SetUserData(p, p);
	load_basic_standards(p);

	buff = EDI_GetBuffer(p, BUFF_SIZE);
	length = read(input, buff, BUFF_SIZE);
	while(length > 0){
		EDI_Bool final = (length < BUFF_SIZE) ? EDI_TRUE : EDI_FALSE;
		status = EDI_ParseBuffer(p, length, final);
		if(status != EDI_STATUS_OK){
			fprintf(stderr, "Error: %d\n", EDI_GetErrorCode(p));
			break;
		}
		buff = EDI_GetBuffer(p, BUFF_SIZE);
		length = read(input, buff, BUFF_SIZE);
	}
	EDI_Schema s = EDI_GetSchema(p);	
	if(s){
		EDI_SchemaFree(s);
	}
	EDI_ParserFree(p);
	close(input);

	gettimeofday(&tv, NULL);
	end_time = tv.tv_sec + (tv.tv_usec * 0.000001);
	file_size = statbuf.st_size;
	fprintf(stderr, "Processed %d Bytes of data\n", (int)file_size);
	run_time = end_time - start_time;
	fprintf(stderr, "Runtime: %f seconds\n", run_time);
	speed = (file_size/1048576)/run_time;
	fprintf(stderr, "Average processing speed was %f MB/sec\n", speed);
	return 0;
}
