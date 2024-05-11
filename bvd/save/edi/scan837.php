<?php
//gcc scan837.c -std=c99 -I../include -L../lib -lm -ledival -o scan837

define("BUFF_SIZE", "16777216");

$group_start = 0;
$counter = 0;
$depth = 0;
$prefix[50] = Array();
$curr_seg[4] = Array();
$curr_e = 0;
$curr_c = 0;
$is_comp = 0;
$type_ok = 0;

include('defs.inc');
include('enum.inc');
include('class.inc');
include('common.inc');
include('complextype.inc');
include('edifactparser.inc');
include('ediparse.inc');
include('hashtable.inc');
include('schema.inc');
include('simpletype.inc');
include('statemachine.inc');
include('x12parser.inc');

function handleSegmentError($myData, $tag, $err)
{
    echo( "*~*~* Segment error $err on $tag\n");
}

function handleElementError($myData, $element, $component, $err)
{
    if($component){
        echo("\n\n *** Element error $err on element $curr_seg$element-$component ***\n\n");
    } else {
        echo("\n\n *** Element error $err on element $curr_seg$element ***\n\n");
    }
}

function handleLoopStart($myData, $loopID)
{
    echo("$prefix<$loopID>\n");
    $depth++;
    $prefix[0] = '\0';
    for($i = 0; $i < $depth; $i++){
        $prefix .= "   ";
    }
    if("group" == $loopID){
        $group_start = 1;
        $counter = 0;
        $type_ok = 0;
    }    
    return;
}

function handleLoopEnd($myData, $loopID)
{
    $depth--;
    $prefix[0] = '\0';
    for($i = 0; $i < $depth; $i++){
        $prefix .= "   ";
    }
    echo("$prefix</$loopID>\n");
    return;
}

function load_basic_standards($p)
{

    $s = new EDI_Schema;
    $parent = new EDI_SchemaNode;
    $child = new EDI_SchemaNode;

    $s = EDI_GetSchema($p);
    if(s && EDI_GetSchemaId($s) != "Generic ANSI X12 Schema"){
        $s = EDI_RemoveSchema($p);
        EDI_SchemaFree($s);
    }
    if(!$s){
        $s = EDI_SchemaCreate(EDI_ANSI_X12);
        EDI_SetSchemaId($s, "Generic ANSI X12 Schema");
        EDI_SetSchema($p, $s);

        $hold = EDI_CreateElementType(s, EDI_DATA_STRING, "I01", 2, 2);
        EDI_AddElementValue($hold, "00");
        EDI_AddElementValue($hold, "01");
        EDI_AddElementValue($hold, "02");
        EDI_AddElementValue($hold, "03");
        EDI_AddElementValue($hold, "04");
        EDI_AddElementValue($hold, "05");
        EDI_AddElementValue($hold, "06");
        
        EDI_CreateElementType($s, EDI_DATA_STRING, "I02", 10, 10);

        $hold = EDI_CreateElementType(s, EDI_DATA_STRING, "I03", 2, 2);
        EDI_AddElementValue($hold, "00");
        EDI_AddElementValue($hold, "01");

        EDI_CreateElementType($s, EDI_DATA_STRING, "I04", 10, 10);

        $hold = EDI_CreateElementType(s, EDI_DATA_STRING, "I05", 2, 2);
        EDI_AddElementValue($hold, "01");
        EDI_AddElementValue($hold, "02");
        EDI_AddElementValue($hold, "03");
        EDI_AddElementValue($hold, "04");
        EDI_AddElementValue($hold, "07");
        EDI_AddElementValue($hold, "08");
        EDI_AddElementValue($hold, "09");
        EDI_AddElementValue($hold, "10");
        EDI_AddElementValue($hold, "11");
        EDI_AddElementValue($hold, "12");
        EDI_AddElementValue($hold, "13");
        EDI_AddElementValue($hold, "14");
        EDI_AddElementValue($hold, "15");
        EDI_AddElementValue($hold, "16");
        EDI_AddElementValue($hold, "17");
        EDI_AddElementValue($hold, "18");
        EDI_AddElementValue($hold, "19");
        EDI_AddElementValue($hold, "20");
        EDI_AddElementValue($hold, "21");
        EDI_AddElementValue($hold, "22");
        EDI_AddElementValue($hold, "23");
        EDI_AddElementValue($hold, "24");
        EDI_AddElementValue($hold, "25");
        EDI_AddElementValue($hold, "26");
        EDI_AddElementValue($hold, "27");
        EDI_AddElementValue($hold, "28");
        EDI_AddElementValue($hold, "29");
        EDI_AddElementValue($hold, "30");
        EDI_AddElementValue($hold, "31");
        EDI_AddElementValue($hold, "32");
        EDI_AddElementValue($hold, "33");
        EDI_AddElementValue($hold, "34");
        EDI_AddElementValue($hold, "35");
        EDI_AddElementValue($hold, "36");
        EDI_AddElementValue($hold, "37");
        EDI_AddElementValue($hold, "38");
        EDI_AddElementValue($hold, "AM");
        EDI_AddElementValue($hold, "NR");
        EDI_AddElementValue($hold, "SA");
        EDI_AddElementValue($hold, "SN");
        EDI_AddElementValue($hold, "ZZ");

        EDI_CreateElementType($s, EDI_DATA_STRING, "I06", 15, 15);
        EDI_CreateElementType($s, EDI_DATA_STRING, "I07", 15, 15);
        EDI_CreateElementType($s, EDI_DATA_DATE, "I08", 6, 6);
        EDI_CreateElementType($s, EDI_DATA_TIME, "I09", 4, 4);
        EDI_CreateElementType($s, EDI_DATA_STRING, "I65", 1, 1);

        $hold = EDI_CreateElementType(s, EDI_DATA_STRING, "I11", 5, 5);
        EDI_AddElementValue($hold, "00200");
        EDI_AddElementValue($hold, "00201");
        EDI_AddElementValue($hold, "00204");
        EDI_AddElementValue($hold, "00300");
        EDI_AddElementValue($hold, "00301");
        EDI_AddElementValue($hold, "00302");
        EDI_AddElementValue($hold, "00303");
        EDI_AddElementValue($hold, "00304");
        EDI_AddElementValue($hold, "00305");
        EDI_AddElementValue($hold, "00306");
        EDI_AddElementValue($hold, "00307");
        EDI_AddElementValue($hold, "00400");
        EDI_AddElementValue($hold, "00401");
        EDI_AddElementValue($hold, "00402");
        EDI_AddElementValue($hold, "00403");
        EDI_AddElementValue($hold, "00404");
        EDI_AddElementValue($hold, "00405");
        EDI_AddElementValue($hold, "00406");
        EDI_AddElementValue($hold, "00500");
        EDI_AddElementValue($hold, "00501");
        EDI_AddElementValue($hold, "00502");
        EDI_AddElementValue($hold, "00503");

        EDI_CreateElementType($s, EDI_DATA_INTEGER, "I12", 9, 9);

        $hold = EDI_CreateElementType(s, EDI_DATA_STRING, "I13", 1, 1);
        EDI_AddElementValue($hold, "0");
        EDI_AddElementValue($hold, "1");

        $hold = EDI_CreateElementType(s, EDI_DATA_STRING, "I14", 1, 1);
        EDI_AddElementValue($hold, "I");
        EDI_AddElementValue($hold, "P");
        EDI_AddElementValue($hold, "T");

        EDI_CreateElementType($s, EDI_DATA_STRING, "I15", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_INTEGER, "I16", 1, 5);
        EDI_CreateElementType($s, EDI_DATA_INTEGER, "28", 1, 9);
        EDI_CreateElementType($s, EDI_DATA_INTEGER, "97", 1, 6);
        EDI_CreateElementType($s, EDI_DATA_STRING, "124", 2, 15);
        EDI_CreateElementType($s, EDI_DATA_STRING, "142", 2, 15);
        EDI_CreateElementType($s, EDI_DATA_TIME, "337", 4, 8);
        EDI_CreateElementType($s, EDI_DATA_DATE, "373", 8, 8);
        EDI_CreateElementType($s, EDI_DATA_STRING, "455", 1, 2);

        $hold = EDI_CreateElementType(s, EDI_DATA_STRING, "479", 2, 2);
        EDI_AddElementValue($hold, "AA");
        EDI_AddElementValue($hold, "AB");
        EDI_AddElementValue($hold, "AC");
        EDI_AddElementValue($hold, "AD");
        EDI_AddElementValue($hold, "AE");
        EDI_AddElementValue($hold, "AF");
        EDI_AddElementValue($hold, "AG");
        EDI_AddElementValue($hold, "AH");
        EDI_AddElementValue($hold, "AI");
        EDI_AddElementValue($hold, "AK");
        EDI_AddElementValue($hold, "AL");
        EDI_AddElementValue($hold, "AM");
        EDI_AddElementValue($hold, "AN");
        EDI_AddElementValue($hold, "AO");
        EDI_AddElementValue($hold, "AP");
        EDI_AddElementValue($hold, "AQ");
        EDI_AddElementValue($hold, "AR");
        EDI_AddElementValue($hold, "AS");
        EDI_AddElementValue($hold, "AT");
        EDI_AddElementValue($hold, "AU");
        EDI_AddElementValue($hold, "AV");
        EDI_AddElementValue($hold, "AW");
        EDI_AddElementValue($hold, "AX");
        EDI_AddElementValue($hold, "AY");
        EDI_AddElementValue($hold, "AZ");
        EDI_AddElementValue($hold, "BA");
        EDI_AddElementValue($hold, "BB");
        EDI_AddElementValue($hold, "BC");
        EDI_AddElementValue($hold, "BD");
        EDI_AddElementValue($hold, "BE");
        EDI_AddElementValue($hold, "BF");
        EDI_AddElementValue($hold, "BL");
        EDI_AddElementValue($hold, "BS");
        EDI_AddElementValue($hold, "CA");
        EDI_AddElementValue($hold, "CB");
        EDI_AddElementValue($hold, "CC");
        EDI_AddElementValue($hold, "CD");
        EDI_AddElementValue($hold, "CE");
        EDI_AddElementValue($hold, "CF");
        EDI_AddElementValue($hold, "CH");
        EDI_AddElementValue($hold, "CI");
        EDI_AddElementValue($hold, "CJ");
        EDI_AddElementValue($hold, "CK");
        EDI_AddElementValue($hold, "CL");
        EDI_AddElementValue($hold, "CM");
        EDI_AddElementValue($hold, "CN");
        EDI_AddElementValue($hold, "CO");
        EDI_AddElementValue($hold, "CP");
        EDI_AddElementValue($hold, "CQ");
        EDI_AddElementValue($hold, "CR");
        EDI_AddElementValue($hold, "CS");
        EDI_AddElementValue($hold, "CT");
        EDI_AddElementValue($hold, "CU");
        EDI_AddElementValue($hold, "CV");
        EDI_AddElementValue($hold, "CW");
        EDI_AddElementValue($hold, "DA");
        EDI_AddElementValue($hold, "DD");
        EDI_AddElementValue($hold, "DF");
        EDI_AddElementValue($hold, "DI");
        EDI_AddElementValue($hold, "DM");
        EDI_AddElementValue($hold, "DS");
        EDI_AddElementValue($hold, "DX");
        EDI_AddElementValue($hold, "D3");
        EDI_AddElementValue($hold, "D4");
        EDI_AddElementValue($hold, "D5");
        EDI_AddElementValue($hold, "EC");
        EDI_AddElementValue($hold, "ED");
        EDI_AddElementValue($hold, "EI");
        EDI_AddElementValue($hold, "EN");
        EDI_AddElementValue($hold, "EP");
        EDI_AddElementValue($hold, "ER");
        EDI_AddElementValue($hold, "ES");
        EDI_AddElementValue($hold, "EV");
        EDI_AddElementValue($hold, "EX");
        EDI_AddElementValue($hold, "FA");
        EDI_AddElementValue($hold, "FB");
        EDI_AddElementValue($hold, "FC");
        EDI_AddElementValue($hold, "FG");
        EDI_AddElementValue($hold, "FR");
        EDI_AddElementValue($hold, "FT");
        EDI_AddElementValue($hold, "GC");
        EDI_AddElementValue($hold, "GE");
        EDI_AddElementValue($hold, "GF");
        EDI_AddElementValue($hold, "GL");
        EDI_AddElementValue($hold, "GP");
        EDI_AddElementValue($hold, "GR");
        EDI_AddElementValue($hold, "GT");
        EDI_AddElementValue($hold, "HB");
        EDI_AddElementValue($hold, "HC");
        EDI_AddElementValue($hold, "HI");
        EDI_AddElementValue($hold, "HN");
        EDI_AddElementValue($hold, "HP");
        EDI_AddElementValue($hold, "HR");
        EDI_AddElementValue($hold, "HS");
        EDI_AddElementValue($hold, "HU");
        EDI_AddElementValue($hold, "HV");
        EDI_AddElementValue($hold, "IA");
        EDI_AddElementValue($hold, "IB");
        EDI_AddElementValue($hold, "IC");
        EDI_AddElementValue($hold, "ID");
        EDI_AddElementValue($hold, "IE");
        EDI_AddElementValue($hold, "IF");
        EDI_AddElementValue($hold, "IG");
        EDI_AddElementValue($hold, "IH");
        EDI_AddElementValue($hold, "IJ");
        EDI_AddElementValue($hold, "IM");
        EDI_AddElementValue($hold, "IN");
        EDI_AddElementValue($hold, "IO");
        EDI_AddElementValue($hold, "IR");
        EDI_AddElementValue($hold, "IS");
        EDI_AddElementValue($hold, "JB");
        EDI_AddElementValue($hold, "KM");
        EDI_AddElementValue($hold, "LA");
        EDI_AddElementValue($hold, "LB");
        EDI_AddElementValue($hold, "LI");
        EDI_AddElementValue($hold, "LN");
        EDI_AddElementValue($hold, "LR");
        EDI_AddElementValue($hold, "LS");
        EDI_AddElementValue($hold, "LT");
        EDI_AddElementValue($hold, "MA");
        EDI_AddElementValue($hold, "MC");
        EDI_AddElementValue($hold, "MD");
        EDI_AddElementValue($hold, "ME");
        EDI_AddElementValue($hold, "MF");
        EDI_AddElementValue($hold, "MG");
        EDI_AddElementValue($hold, "MH");
        EDI_AddElementValue($hold, "MI");
        EDI_AddElementValue($hold, "MJ");
        EDI_AddElementValue($hold, "MK");
        EDI_AddElementValue($hold, "MM");
        EDI_AddElementValue($hold, "MN");
        EDI_AddElementValue($hold, "MO");
        EDI_AddElementValue($hold, "MP");
        EDI_AddElementValue($hold, "MQ");
        EDI_AddElementValue($hold, "MR");
        EDI_AddElementValue($hold, "MS");
        EDI_AddElementValue($hold, "MT");
        EDI_AddElementValue($hold, "MV");
        EDI_AddElementValue($hold, "MW");
        EDI_AddElementValue($hold, "MX");
        EDI_AddElementValue($hold, "MY");
        EDI_AddElementValue($hold, "MZ");
        EDI_AddElementValue($hold, "NC");
        EDI_AddElementValue($hold, "NL");
        EDI_AddElementValue($hold, "NP");
        EDI_AddElementValue($hold, "NR");
        EDI_AddElementValue($hold, "NT");
        EDI_AddElementValue($hold, "OC");
        EDI_AddElementValue($hold, "OG");
        EDI_AddElementValue($hold, "OR");
        EDI_AddElementValue($hold, "OW");
        EDI_AddElementValue($hold, "PA");
        EDI_AddElementValue($hold, "PB");
        EDI_AddElementValue($hold, "PC");
        EDI_AddElementValue($hold, "PD");
        EDI_AddElementValue($hold, "PE");
        EDI_AddElementValue($hold, "PF");
        EDI_AddElementValue($hold, "PG");
        EDI_AddElementValue($hold, "PH");
        EDI_AddElementValue($hold, "PI");
        EDI_AddElementValue($hold, "PJ");
        EDI_AddElementValue($hold, "PK");
        EDI_AddElementValue($hold, "PL");
        EDI_AddElementValue($hold, "PN");
        EDI_AddElementValue($hold, "PO");
        EDI_AddElementValue($hold, "PQ");
        EDI_AddElementValue($hold, "PR");
        EDI_AddElementValue($hold, "PS");
        EDI_AddElementValue($hold, "PT");
        EDI_AddElementValue($hold, "PU");
        EDI_AddElementValue($hold, "PV");
        EDI_AddElementValue($hold, "PW");
        EDI_AddElementValue($hold, "PY");
        EDI_AddElementValue($hold, "QG");
        EDI_AddElementValue($hold, "QM");
        EDI_AddElementValue($hold, "QO");
        EDI_AddElementValue($hold, "RA");
        EDI_AddElementValue($hold, "RB");
        EDI_AddElementValue($hold, "RC");
        EDI_AddElementValue($hold, "RD");
        EDI_AddElementValue($hold, "RE");
        EDI_AddElementValue($hold, "RF");
        EDI_AddElementValue($hold, "RG");
        EDI_AddElementValue($hold, "RH");
        EDI_AddElementValue($hold, "RI");
        EDI_AddElementValue($hold, "RJ");
        EDI_AddElementValue($hold, "RK");
        EDI_AddElementValue($hold, "RL");
        EDI_AddElementValue($hold, "RM");
        EDI_AddElementValue($hold, "RN");
        EDI_AddElementValue($hold, "RO");
        EDI_AddElementValue($hold, "RP");
        EDI_AddElementValue($hold, "RQ");
        EDI_AddElementValue($hold, "RR");
        EDI_AddElementValue($hold, "RS");
        EDI_AddElementValue($hold, "RT");
        EDI_AddElementValue($hold, "RU");
        EDI_AddElementValue($hold, "RV");
        EDI_AddElementValue($hold, "RW");
        EDI_AddElementValue($hold, "RX");
        EDI_AddElementValue($hold, "RY");
        EDI_AddElementValue($hold, "RZ");
        EDI_AddElementValue($hold, "SA");
        EDI_AddElementValue($hold, "SB");
        EDI_AddElementValue($hold, "SC");
        EDI_AddElementValue($hold, "SD");
        EDI_AddElementValue($hold, "SE");
        EDI_AddElementValue($hold, "SH");
        EDI_AddElementValue($hold, "SI");
        EDI_AddElementValue($hold, "SJ");
        EDI_AddElementValue($hold, "SL");
        EDI_AddElementValue($hold, "SM");
        EDI_AddElementValue($hold, "SN");
        EDI_AddElementValue($hold, "SO");
        EDI_AddElementValue($hold, "SP");
        EDI_AddElementValue($hold, "SQ");
        EDI_AddElementValue($hold, "SR");
        EDI_AddElementValue($hold, "SS");
        EDI_AddElementValue($hold, "ST");
        EDI_AddElementValue($hold, "SU");
        EDI_AddElementValue($hold, "SV");
        EDI_AddElementValue($hold, "SW");
        EDI_AddElementValue($hold, "TA");
        EDI_AddElementValue($hold, "TB");
        EDI_AddElementValue($hold, "TD");
        EDI_AddElementValue($hold, "TE");
        EDI_AddElementValue($hold, "TF");
        EDI_AddElementValue($hold, "TI");
        EDI_AddElementValue($hold, "TJ");
        EDI_AddElementValue($hold, "TM");
        EDI_AddElementValue($hold, "TN");
        EDI_AddElementValue($hold, "TO");
        EDI_AddElementValue($hold, "TP");
        EDI_AddElementValue($hold, "TR");
        EDI_AddElementValue($hold, "TS");
        EDI_AddElementValue($hold, "TT");
        EDI_AddElementValue($hold, "TU");
        EDI_AddElementValue($hold, "TX");
        EDI_AddElementValue($hold, "UA");
        EDI_AddElementValue($hold, "UB");
        EDI_AddElementValue($hold, "UC");
        EDI_AddElementValue($hold, "UD");
        EDI_AddElementValue($hold, "UI");
        EDI_AddElementValue($hold, "UP");
        EDI_AddElementValue($hold, "UW");
        EDI_AddElementValue($hold, "VA");
        EDI_AddElementValue($hold, "VB");
        EDI_AddElementValue($hold, "VC");
        EDI_AddElementValue($hold, "VD");
        EDI_AddElementValue($hold, "VE");
        EDI_AddElementValue($hold, "VH");
        EDI_AddElementValue($hold, "VI");
        EDI_AddElementValue($hold, "VS");
        EDI_AddElementValue($hold, "WA");
        EDI_AddElementValue($hold, "WB");
        EDI_AddElementValue($hold, "WG");
        EDI_AddElementValue($hold, "WI");
        EDI_AddElementValue($hold, "WL");
        EDI_AddElementValue($hold, "WR");
        EDI_AddElementValue($hold, "WT");
        EDI_CreateElementType($s, EDI_DATA_STRING, "480", 1, 12);

        $hold = EDI_CreateComplexType(s, EDITYPE_SEGMENT, "ISA");
        EDI_AppendType($hold, EDI_GetElementByID(s, "I01"), 1, 1);
        EDI_AppendType($hold, EDI_GetElementByID(s, "I02"), 1, 1);
        EDI_AppendType($hold, EDI_GetElementByID(s, "I03"), 1, 1);
        EDI_AppendType($hold, EDI_GetElementByID(s, "I04"), 1, 1);
        EDI_AppendType($hold, EDI_GetElementByID(s, "I05"), 1, 1);
        EDI_AppendType($hold, EDI_GetElementByID(s, "I06"), 1, 1);
        EDI_AppendType($hold, EDI_GetElementByID(s, "I05"), 1, 1);
        EDI_AppendType($hold, EDI_GetElementByID(s, "I07"), 1, 1);
        EDI_AppendType($hold, EDI_GetElementByID(s, "I08"), 1, 1);
        EDI_AppendType($hold, EDI_GetElementByID(s, "I09"), 1, 1);
        EDI_AppendType($hold, EDI_GetElementByID(s, "I65"), 1, 1);
        EDI_AppendType($hold, EDI_GetElementByID(s, "I11"), 1, 1);
        EDI_AppendType($hold, EDI_GetElementByID(s, "I12"), 1, 1);
        EDI_AppendType($hold, EDI_GetElementByID(s, "I13"), 1, 1);
        EDI_AppendType($hold, EDI_GetElementByID(s, "I14"), 1, 1);
        EDI_AppendType($hold, EDI_GetElementByID(s, "I15"), 1, 1);
        EDI_StoreComplexNode($s, hold);
        
        $hold = EDI_CreateComplexType(s, EDITYPE_SEGMENT, "IEA");
        EDI_AppendType($hold, EDI_GetElementByID(s, "I16"), 1, 1);
        EDI_AppendType($hold, EDI_GetElementByID(s, "I12"), 1, 1);
        EDI_StoreComplexNode($s, hold);

        $hold = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "GS");
        EDI_AppendType($hold, EDI_GetElementByID($s, "479"), 1, 1);
        EDI_AppendType($hold, EDI_GetElementByID($s, "142"), 1, 1);
        EDI_AppendType($hold, EDI_GetElementByID($s, "124"), 1, 1);
        EDI_AppendType($hold, EDI_GetElementByID($s, "373"), 1, 1);
        EDI_AppendType($hold, EDI_GetElementByID($s, "337"), 1, 1);
        EDI_AppendType($hold, EDI_GetElementByID($s, "28"), 1, 1);
        EDI_AppendType($hold, EDI_GetElementByID($s, "455"), 1, 1);
        EDI_AppendType($hold, EDI_GetElementByID($s, "480"), 1, 1);
        EDI_StoreComplexNode($s, $hold);

        $hold = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "GE");
        EDI_AppendType($hold, EDI_GetElementByID($s, "28"), 1, 1);
        EDI_AppendType($hold, EDI_GetElementByID($s, "97"), 1, 1);
        EDI_StoreComplexNode($s, $hold);

        $hold = EDI_CreateComplexType($s, EDITYPE_DOCUMENT, "interchange");
        EDI_AppendType($hold, EDI_GetComplexNodeByID($s, "ISA"), 1, 1);

        $group = EDI_CreateComplexType($s, EDITYPE_LOOP, "group");
        EDI_AppendType($group, EDI_GetComplexNodeByID($s, "GS"), 1, 1);
        EDI_AppendType($group, EDI_GetComplexNodeByID(s, "GE"), 1, 1);
        EDI_StoreComplexNode($s, $group);

        EDI_AppendType($hold, $group, 0, 999999999);
        EDI_AppendType($hold, EDI_GetComplexNodeByID($s, "IEA"), 1, 1);
        EDI_StoreComplexNode($s, $hold);
        
        EDI_SetDocumentRoot($s, EDI_GetComplexNodeByID($s, "interchange"));
        EDI_SetSegmentErrorHandler($s, $handleSegmentError);
        EDI_SetElementErrorHandler($s, $handleElementError);
        EDI_SetLoopStartHandler($s, $handleLoopStart);
        EDI_SetLoopEndHandler($s, $handleLoopEnd);
    }
}

function load_standard($p)
{
    $s = new EDI_Schema;
    $parent = new EDI_SchemaNode;
    $child = new EDI_SchemaNode;
    
    $s = EDI_GetSchema($p);
    if(!$s || ($s &&
               (EDI_GetSchemaId($s) == "Health Care Claim: HC/837") != 0)){
        load_basic_standards($p);
        $s = EDI_GetSchema($p);
        EDI_SetSchemaId($s, "Health Care Claim: HC/837");

        EDI_CreateElementType($s, EDI_DATA_STRING, "143", 3, 3);
        EDI_CreateElementType($s, EDI_DATA_INTEGER, "329", 4, 9);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1005", 4, 4);
        EDI_CreateElementType($s, EDI_DATA_STRING, "353", 2, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "127", 1, 30);
        EDI_CreateElementType($s, EDI_DATA_DATE, "373", 6, 8);
        EDI_CreateElementType($s, EDI_DATA_TIME, "337", 4, 8);
        EDI_CreateElementType($s, EDI_DATA_STRING, "640", 2, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "128", 2, 3);
        EDI_CreateElementType($s, EDI_DATA_STRING, "352", 1, 80);
        EDI_CreateElementType($s, EDI_DATA_STRING, "98", 2, 3);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1065", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1035", 1, 35);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1036", 1, 25);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1037", 1, 25);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1038", 1, 10);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1039", 1, 10);
        EDI_CreateElementType($s, EDI_DATA_STRING, "66", 1, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "67", 2, 80);
        EDI_CreateElementType($s, EDI_DATA_STRING, "706", 2, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "93", 1, 60);
        EDI_CreateElementType($s, EDI_DATA_STRING, "166", 1, 55);
        EDI_CreateElementType($s, EDI_DATA_STRING, "19", 2, 30);
        EDI_CreateElementType($s, EDI_DATA_STRING, "156", 2, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "116", 3, 15);
        EDI_CreateElementType($s, EDI_DATA_STRING, "26", 2, 3);
        EDI_CreateElementType($s, EDI_DATA_STRING, "309", 1, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "310", 1, 30);
        EDI_CreateElementType($s, EDI_DATA_STRING, "366", 2, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "365", 2, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "364", 1, 80);
        EDI_CreateElementType($s, EDI_DATA_STRING, "443", 1, 20);
        EDI_CreateElementType($s, EDI_DATA_STRING, "628", 1, 12);
        EDI_CreateElementType($s, EDI_DATA_STRING, "734", 1, 12);
        EDI_CreateElementType($s, EDI_DATA_STRING, "735", 1, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "736", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1221", 1, 3);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1222", 1, 3);
        EDI_CreateElementType($s, EDI_DATA_STRING, "559", 2, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1073", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1223", 3, 3);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1138", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1069", 2, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1336", 1, 3);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1143", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_STRING, "584", 2, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1032", 1, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1384", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1220", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1250", 2, 3);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1251", 1, 35);
        EDI_CreateElementType($s, EDI_DATA_STRING, "355", 2, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "81", 1, 10);
        EDI_CreateElementType($s, EDI_DATA_STRING, "374", 3, 3);
        EDI_CreateElementType($s, EDI_DATA_STRING, "100", 3, 3);
        EDI_CreateElementType($s, EDI_DATA_STRING, "280", 4, 10);
        EDI_CreateElementType($s, EDI_DATA_STRING, "669", 3, 3);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1068", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1067", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1109", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1066", 1, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "659", 1, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "380", 1, 15);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1028", 1, 38);
        EDI_CreateElementType($s, EDI_DATA_DECIMAL, "782", 1, 18);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1343", 1, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1331", 1, 2);
        EDI_CreateElementType($s, EDI_DATA_DECIMAL, "1332", 1, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1325", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1359", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1363", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1351", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1362", 2, 3);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1366", 2, 3);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1338", 1, 3);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1360", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1029", 1, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1383", 2, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1514", 1, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1315", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1314", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1352", 1, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1345", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1368", 1, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "755", 2, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "756", 1, 2);
        EDI_CreateElementType($s, EDI_DATA_INTEGER, "757", 1, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "704", 1, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1525", 1, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1166", 2, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "332", 1, 6);
        EDI_CreateElementType($s, EDI_DATA_STRING, "338", 1, 6);
        EDI_CreateElementType($s, EDI_DATA_STRING, "799", 1, 30);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1146", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1149", 4, 6);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1154", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1161", 1, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "235", 2, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1137", 1, 15);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1318", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_STRING, "522", 1, 3);
        EDI_CreateElementType($s, EDI_DATA_STRING, "478", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_STRING, "449", 1, 80);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1333", 1, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1018", 1, 15);
        EDI_CreateElementType($s, EDI_DATA_STRING, "649", 1, 10);
        EDI_CreateElementType($s, EDI_DATA_STRING, "363", 3, 3);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1316", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1317", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_INTEGER, "609", 1, 9);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1367", 2, 3);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1342", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1322", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1335", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1344", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_STRING, "65", 1, 8);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1346", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1347", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_STRING, "954", 1, 10);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1348", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1349", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1350", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1382", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_STRING, "923", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1403", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1404", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1136", 2, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1321", 2, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1270", 1, 3);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1271", 1, 30);
        EDI_CreateElementType($s, EDI_DATA_STRING, "673", 2, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "61", 1, 30);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1473", 2, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "118", 1, 9);
        EDI_CreateElementType($s, EDI_DATA_STRING, "234", 1, 48);
        EDI_CreateElementType($s, EDI_DATA_STRING, "901", 2, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1526", 1, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1527", 1, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "921", 2, 2);
        EDI_CreateElementType($s, EDI_DATA_INTEGER, "1470", 1, 9);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1167", 1, 6);
        EDI_CreateElementType($s, EDI_DATA_STRING, "615", 1, 2);
        EDI_CreateElementType($s, EDI_DATA_INTEGER, "616", 1, 3);
        EDI_CreateElementType($s, EDI_DATA_STRING, "678", 1, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "679", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1033", 1, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1034", 1, 5);
        EDI_CreateElementType($s, EDI_DATA_INTEGER, "554", 1, 6);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1339", 2, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1365", 1, 2);
        EDI_CreateElementType($s, EDI_DATA_INTEGER, "1328", 1, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1340", 1, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1364", 1, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1341", 1, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1327", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1334", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1337", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1371", 1, 10);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1361", 1, 3);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1358", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1369", 1, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1329", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1356", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1370", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1319", 1, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1320", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1330", 2, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1357", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_STRING, "594", 1, 1);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1355", 2, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1207", 3, 3);
        EDI_CreateElementType($s, EDI_DATA_STRING, "750", 2, 3);
        EDI_CreateElementType($s, EDI_DATA_STRING, "737", 2, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "738", 1, 3);
        EDI_CreateElementType($s, EDI_DATA_STRING, "739", 1, 20);
        EDI_CreateElementType($s, EDI_DATA_STRING, "740", 1, 20);
        EDI_CreateElementType($s, EDI_DATA_STRING, "741", 1, 20);
        EDI_CreateElementType($s, EDI_DATA_STRING, "935", 2, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "936", 2, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "752", 2, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1373", 2, 4);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1253", 3, 6);
        EDI_CreateElementType($s, EDI_DATA_STRING, "1254", 1, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "350", 1, 20);
        EDI_CreateElementType($s, EDI_DATA_STRING, "687", 2, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "236", 3, 3);
        EDI_CreateElementType($s, EDI_DATA_STRING, "212", 1, 17);
        EDI_CreateElementType($s, EDI_DATA_STRING, "648", 3, 3);
        EDI_CreateElementType($s, EDI_DATA_STRING, "639", 2, 2);
        EDI_CreateElementType($s, EDI_DATA_STRING, "499", 1, 10);
        EDI_CreateElementType($s, EDI_DATA_INTEGER, "289", 1, 2);
        EDI_CreateElementType($s, EDI_DATA_INTEGER, "96", 1, 10);

        $parent = EDI_CreateComplexType($s, EDITYPE_COMPOSITE, "C001");
        EDI_AppendType($parent, EDI_GetElementByID($s, "355"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1018"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "649"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "355"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1018"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "649"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "355"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1018"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "649"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "355"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1018"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "649"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "355"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1018"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "649"), 0, 1);
        EDI_StoreComplexNode($s, $parent);

        $parent = EDI_CreateComplexType($s, EDITYPE_COMPOSITE, "C002");
        EDI_AppendType($parent, EDI_GetElementByID($s, "704"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "704"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "704"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "704"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "704"), 0, 1);
        EDI_StoreComplexNode($s, $parent);

        $parent = EDI_CreateComplexType($s, EDITYPE_COMPOSITE, "C003");
        EDI_AppendType($parent, EDI_GetElementByID($s, "235"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "234"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1339"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1339"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1339"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1339"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "352"), 0, 1);
        EDI_StoreComplexNode(s, $parent);

        $parent = EDI_CreateComplexType($s, EDITYPE_COMPOSITE, "C004");
        EDI_AppendType($parent, EDI_GetElementByID($s, "1328"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1328"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1328"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1328"), 0, 1);
        EDI_StoreComplexNode(s, $parent);

        $parent = EDI_CreateComplexType($s, EDITYPE_COMPOSITE, "C005");
        EDI_AppendType($parent, EDI_GetElementByID($s, "1369"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1369"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1369"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1369"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1369"), 0, 1);
        EDI_StoreComplexNode($s, $parent);

        $parent = EDI_CreateComplexType($s, EDITYPE_COMPOSITE, "C006");
        EDI_AppendType($parent, EDI_GetElementByID($s, "1361"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1361"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1361"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1361"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1361"), 0, 1);
        EDI_StoreComplexNode($s, $parent);

        $parent = EDI_CreateComplexType($s, EDITYPE_COMPOSITE, "C022");
        EDI_AppendType($parent, EDI_GetElementByID($s, "1270"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1271"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1250"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1251"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "782"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "799"), 0, 1);
        EDI_StoreComplexNode($s, $parent);

        $parent = EDI_CreateComplexType($s, EDITYPE_COMPOSITE, "C023");
        EDI_AppendType($parent, EDI_GetElementByID($s, "1331"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1332"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1325"), 0, 1);
        EDI_StoreComplexNode(s, $parent);

        $parent = EDI_CreateComplexType($s, EDITYPE_COMPOSITE, "C024");
        EDI_AppendType($parent, EDI_GetElementByID($s, "1362"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1362"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1362"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "156"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "26"), 0, 1);
        EDI_StoreComplexNode($s, $parent);

        $parent = EDI_CreateComplexType(s, EDITYPE_COMPOSITE, "C035");
        EDI_AppendType($parent, EDI_GetElementByID(s, "1222"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID(s, "559"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID(s, "1073"), 0, 1);
        EDI_StoreComplexNode($s, $parent);

        $parent = EDI_CreateComplexType($s, EDITYPE_COMPOSITE, "C040");
        EDI_AppendType($parent, EDI_GetElementByID($s, "128"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "127"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "128"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "127"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "128"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "127"), 0, 1);
        EDI_StoreComplexNode($s, $parent);

        $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "ST");
        EDI_AppendType($parent, EDI_GetElementByID($s, "143"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "329"), 1, 1);
        EDI_StoreComplexNode(s, $parent);

        $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "BHT");
        EDI_AppendType($parent, EDI_GetElementByID($s, "1005"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "353"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "127"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "373"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "337"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "640"), 0, 1);
        EDI_StoreComplexNode($s, $parent);

        $parent = EDI_CreateComplexType(s, EDITYPE_SEGMENT, "REF");
        EDI_AppendType($parent, EDI_GetElementByID($s, "128"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "127"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "352"), 0, 1);
        EDI_AppendType($parent, EDI_GetComplexNodeByID($s, "C040"), 0, 1);
        $arr[] = Array( 0 => 2, 1 => 3 );
        EDI_AddSyntaxNote($parent, EDI_SYNTAX_REQUIRED, 2, $arr);
        EDI_StoreComplexNode($s, $parent);

        $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "NM1");
        EDI_AppendType($parent, EDI_GetElementByID($s, "98"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1065"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1035"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1036"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1037"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1038"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1039"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "66"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "67"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "706"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "98"), 0, 1);
        EDI_StoreComplexNode($s, $parent);

        $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "N2");
        EDI_AppendType($parent, EDI_GetElementByID($s, "93"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "93"), 0, 1);
        EDI_StoreComplexNode($s, $parent);

        $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "N3");
        EDI_AppendType($parent, EDI_GetElementByID($s, "166"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "166"), 0, 1);
        EDI_StoreComplexNode($s, $parent);

        $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "N4");
        EDI_AppendType($parent, EDI_GetElementByID($s, "19"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "156"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "116"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "26"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "309"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "310"), 0, 1);
        EDI_StoreComplexNode($s, $parent);

        $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "PER");
        EDI_AppendType($parent, EDI_GetElementByID($s, "366"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "93"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "365"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "364"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "365"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "364"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "365"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "364"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "443"), 0, 1);
        EDI_StoreComplexNode($s, $parent);

        $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "HL");
        EDI_AppendType($parent, EDI_GetElementByID($s, "628"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "734"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "735"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "736"), 0, 1);
        EDI_StoreComplexNode($s, $parent);

        $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "PRV");
        EDI_AppendType($parent, EDI_GetElementByID($s, "1221"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "128"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "127"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "156"), 0, 1);
        EDI_AppendType($parent, EDI_GetComplexNodeByID($s, "C035"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1223"), 0, 1);
        EDI_StoreComplexNode($s, $parent);

        $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "SBR");
        EDI_AppendType($parent, EDI_GetElementByID($s, "1138"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1069"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "127"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "93"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1336"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1143"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1073"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "584"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1032"), 0, 1);
        EDI_StoreComplexNode($s, $parent);

        $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "PAT");
        EDI_AppendType($parent, EDI_GetElementByID($s, "1069"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1384"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "584"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1220"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1250"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1251"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "355"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "81"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1073"), 0, 1);
        EDI_StoreComplexNode($s, $parent);

        $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "DTP");
        EDI_AppendType($parent, EDI_GetElementByID($s, "374"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1250"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1251"), 1, 1);
        EDI_StoreComplexNode($s, $parent);

        $parent = EDI_CreateComplexType(s, EDITYPE_SEGMENT, "CUR");
        EDI_AppendType($parent, EDI_GetElementByID(s, "98"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID(s, "100"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID(s, "280"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID(s, "98"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID(s, "100"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID(s, "669"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID(s, "374"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID(s, "373"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID(s, "337"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID(s, "374"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID(s, "373"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID(s, "337"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID(s, "374"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID(s, "373"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID(s, "337"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID(s, "374"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID(s, "373"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID(s, "337"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID(s, "374"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID(s, "373"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID(s, "337"), 0, 1);
        EDI_StoreComplexNode($s, $parent);

        $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "DMG");
        EDI_AppendType($parent, EDI_GetElementByID($s, "1250"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1251"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1068"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1067"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1109"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1066"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "26"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "659"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_StoreComplexNode($s, $parent);

        $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "CLM");
        EDI_AppendType($parent, EDI_GetElementByID($s, "1028"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "782"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1032"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1343"), 0, 1);
        EDI_AppendType($parent, EDI_GetComplexNodeByID($s, "C023"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1073"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1359"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1073"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1363"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1351"), 0, 1);
        EDI_AppendType($parent, EDI_GetComplexNodeByID($s, "C024"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1366"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1073"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1338"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1073"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1360"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1029"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1073"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1383"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1514"), 0, 1);
        EDI_StoreComplexNode($s, $parent);

        $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "CL1");
        EDI_AppendType($parent, EDI_GetElementByID($s, "1315"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1314"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1352"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1345"), 0, 1);
        EDI_StoreComplexNode($s, $parent);

        $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "DN1");
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1073"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "352"), 0, 1);
        EDI_StoreComplexNode($s, $parent);

        $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "DN2");
        EDI_AppendType($parent, EDI_GetElementByID($s, "127"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1368"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1250"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1251"), 0, 1);
        EDI_StoreComplexNode($s, $parent);

        $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "PWK");
        EDI_AppendType($parent, EDI_GetElementByID($s, "755"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "756"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "757"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "98"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "66"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "67"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "352"), 0, 1);
        EDI_AppendType($parent, EDI_GetComplexNodeByID($s, "C002"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1525"), 0, 1);
        EDI_StoreComplexNode($s, $parent);

        $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "CN1");
        EDI_AppendType($parent, EDI_GetElementByID($s, "1166"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "782"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "332"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "127"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "338"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "799"), 0, 1);
        EDI_StoreComplexNode($s, $parent);

        $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "DSB");
        EDI_AppendType($parent, EDI_GetElementByID($s, "1146"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1149"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1154"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1161"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "782"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "235"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1137"), 0, 1);
        EDI_StoreComplexNode($s, $parent);

        $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "UR");
        EDI_AppendType($parent, EDI_GetElementByID($s, "1318"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_StoreComplexNode($s, $parent);

        $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "AMT");
        EDI_AppendType($parent, EDI_GetElementByID($s, "522"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "782"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "478"), 0, 1);
        EDI_StoreComplexNode($$s, $parent);

       $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "K3");
        EDI_AppendType($parent, EDI_GetElementByID($s, "449"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1333"), 0, 1);
        EDI_AppendType($parent, EDI_GetComplexNodeByID($s, "C001"), 0, 1);
        EDI_StoreComplexNode($s,$parent);

       $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "NTE");
        EDI_AppendType($parent, EDI_GetElementByID($s, "363"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "352"), 1, 1);
        EDI_StoreComplexNode($s,$parent);

       $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "CR1");
        EDI_AppendType($parent, EDI_GetElementByID($s, "355"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "81"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1316"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1317"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "355"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "166"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "166"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "352"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "352"), 0, 1);
        EDI_StoreComplexNode($s,$parent);

       $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "CR2");
        EDI_AppendType($parent, EDI_GetElementByID($s, "609"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1367"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1367"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "355"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1342"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1073"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "352"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "352"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1073"), 0, 1);
        EDI_StoreComplexNode($s,$parent);

       $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "CR3");
        EDI_AppendType($parent, EDI_GetElementByID($s, "1322"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "355"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1335"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "352"), 0, 1);
        EDI_StoreComplexNode($s,$parent);

       $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "CR4");
        EDI_AppendType($parent, EDI_GetElementByID($s, "1073"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1322"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "355"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "355"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1344"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "355"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "355"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "65"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "355"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "81"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "352"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1346"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1347"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "352"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "954"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "954"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "954"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "352"), 0, 1);
        EDI_StoreComplexNode($s,$parent);

       $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "CR5");
        EDI_AppendType($parent, EDI_GetElementByID($s, "1322"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1348"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1348"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "352"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "352"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1349"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1350"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1350"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1350"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1382"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1348"), 0, 1);
        EDI_StoreComplexNode($s,$parent);

       $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "CR6");
        EDI_AppendType($parent, EDI_GetElementByID($s, "923"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "373"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1250"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1251"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "373"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1073"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1073"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1322"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "373"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "235"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1137"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "373"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "373"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "373"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1250"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1251"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1384"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "373"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "373"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "373"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "373"), 0, 1);
        EDI_StoreComplexNode($s,$parent);

       $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "CR8");
        EDI_AppendType($parent, EDI_GetElementByID($s, "1403"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1404"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "373"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "373"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "127"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "127"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "127"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1073"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1073"), 1, 1);
        EDI_StoreComplexNode($s,$parent);

       $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "CRC");
        EDI_AppendType($parent, EDI_GetElementByID($s, "1136"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1073"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1321"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1321"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1321"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1321"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1321"), 0, 1);
        EDI_StoreComplexNode($s,$parent);

       $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "HI");
        EDI_AppendType($parent, EDI_GetComplexNodeByID($s, "C022"), 1, 1);
        EDI_AppendType($parent, EDI_GetComplexNodeByID($s, "C022"), 0, 1);
        EDI_AppendType($parent, EDI_GetComplexNodeByID($s, "C022"), 0, 1);
        EDI_AppendType($parent, EDI_GetComplexNodeByID($s, "C022"), 0, 1);
        EDI_AppendType($parent, EDI_GetComplexNodeByID($s, "C022"), 0, 1);
        EDI_AppendType($parent, EDI_GetComplexNodeByID($s, "C022"), 0, 1);
        EDI_AppendType($parent, EDI_GetComplexNodeByID($s, "C022"), 0, 1);
        EDI_AppendType($parent, EDI_GetComplexNodeByID($s, "C022"), 0, 1);
        EDI_AppendType($parent, EDI_GetComplexNodeByID($s, "C022"), 0, 1);
        EDI_AppendType($parent, EDI_GetComplexNodeByID($s, "C022"), 0, 1);
        EDI_AppendType($parent, EDI_GetComplexNodeByID($s, "C022"), 0, 1);
        EDI_AppendType($parent, EDI_GetComplexNodeByID($s, "C022"), 0, 1);
        EDI_StoreComplexNode($s,$parent);

       $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "QTY");
        EDI_AppendType($parent, EDI_GetElementByID($s, "673"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_AppendType($parent, EDI_GetComplexNodeByID($s, "C001"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "61"), 0, 1);
        EDI_StoreComplexNode($s,$parent);

       $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "HCP");
        EDI_AppendType($parent, EDI_GetElementByID($s, "1473"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "782"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "782"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "127"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "118"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "127"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "782"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "234"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "235"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "234"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "355"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "901"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1526"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1527"), 0, 1);
        EDI_StoreComplexNode($s,$parent);

       $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "CR7");
        EDI_AppendType($parent, EDI_GetElementByID($s, "921"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1470"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1470"), 1, 1);
        EDI_StoreComplexNode($s,$parent);

       $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "HSD");
        EDI_AppendType($parent, EDI_GetElementByID($s, "673"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "355"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1167"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "615"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "616"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "678"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "679"), 0, 1);
        EDI_StoreComplexNode($s,$parent);

       $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "CAS");
        EDI_AppendType($parent, EDI_GetElementByID($s, "1033"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1034"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "782"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1034"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "782"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1034"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "782"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1034"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "782"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1034"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "782"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1034"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "782"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_StoreComplexNode($s,$parent);

       $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "OI");
        EDI_AppendType($parent, EDI_GetElementByID($s, "1032"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1383"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1073"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1351"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1360"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1363"), 0, 1);
        EDI_StoreComplexNode($s,$parent);

       $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "MIA");
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "782"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "127"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "782"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "782"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "782"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "782"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "782"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "782"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "782"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "782"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "782"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "782"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "782"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "782"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "782"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "127"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "127"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "127"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "127"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "782"), 0, 1);
        EDI_StoreComplexNode($s,$parent);

       $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "MOA");
        EDI_AppendType($parent, EDI_GetElementByID($s, "954"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "782"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "127"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "127"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "127"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "127"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "127"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "782"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "782"), 0, 1);
        EDI_StoreComplexNode($s,$parent);

       $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "LX");
        EDI_AppendType($parent, EDI_GetElementByID($s, "554"), 1, 1);
        EDI_StoreComplexNode($s,$parent);

       $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "SV1");
        EDI_AppendType($parent, EDI_GetComplexNodeByID($s, "C003"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "782"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "355"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1331"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1365"), 0, 1);
        EDI_AppendType($parent, EDI_GetComplexNodeByID($s, "C004"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "782"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1073"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1340"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1073"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1073"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1364"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1341"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1327"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1334"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "127"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "116"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "782"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1337"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1360"), 0, 1);
        $arr[] = Array( 0 => 3, 1 => 4 );
        EDI_AddSyntaxNote($parent, EDI_SYNTAX_PAIRED, 2, $arr);
        EDI_StoreComplexNode($s,$parent);

       $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "SV2");
        EDI_AppendType($parent, EDI_GetElementByID($s, "234"), 0, 1);
        EDI_AppendType($parent, EDI_GetComplexNodeByID($s, "C003"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "782"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "355"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1371"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "782"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1073"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1345"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1337"), 0, 1);
        EDI_StoreComplexNode($s,$parent);

       $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "SV3");
        EDI_AppendType($parent, EDI_GetComplexNodeByID($s, "C003"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "782"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1331"), 0, 1);
        EDI_AppendType($parent, EDI_GetComplexNodeByID($s, "C006"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1358"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "352"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1327"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1360"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1073"), 0, 1);
        EDI_AppendType($parent, EDI_GetComplexNodeByID($s, "C004"), 0, 1);
        EDI_StoreComplexNode($s,$parent);

       $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "TOO");
        EDI_AppendType($parent, EDI_GetElementByID($s, "1270"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1271"), 0, 1);
        EDI_AppendType($parent, EDI_GetComplexNodeByID($s, "C005"), 0, 1);
        EDI_StoreComplexNode($s,$parent);

       $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "SV4");
        EDI_AppendType($parent, EDI_GetElementByID($s, "127"), 1, 1);
        EDI_AppendType($parent, EDI_GetComplexNodeByID($s, "C003"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "127"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1073"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1329"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1338"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1356"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "352"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1073"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1073"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1370"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1319"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1320"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1330"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1327"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1384"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1337"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1357"), 0, 1);
        EDI_StoreComplexNode($s,$parent);

       $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "SV5");
        EDI_AppendType($parent, EDI_GetComplexNodeByID($s, "C003"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "355"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "782"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "782"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "594"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "923"), 0, 1);
        EDI_StoreComplexNode($s,$parent);

       $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "SV6");
        EDI_AppendType($parent, EDI_GetComplexNodeByID($s, "C003"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1332"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1331"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "782"), 0, 1);
        EDI_AppendType($parent, EDI_GetComplexNodeByID($s, "C004"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1073"), 0, 1);
        EDI_StoreComplexNode($s,$parent);

       $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "SV7");
        EDI_AppendType($parent, EDI_GetElementByID($s, "127"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "127"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1355"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1207"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "750"), 1, 1);
        EDI_AppendType($$parent, EDI_GetElementByID($s, "1073"), 0, 1);
        EDI_StoreComplexNode($s,$parent);

       $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "MEA");
        EDI_AppendType($parent, EDI_GetElementByID($s, "737"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "738"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "739"), 0, 1);
        EDI_AppendType($parent, EDI_GetComplexNodeByID($s, "C001"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "740"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "741"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "935"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "936"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "752"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1373"), 0, 1);
        EDI_StoreComplexNode($s,$parent);

       $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "PS1");
        EDI_AppendType($parent, EDI_GetElementByID($s, "127"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "782"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "156"), 0, 1);
        EDI_StoreComplexNode($s,$parent);

       $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "IMM");
        EDI_AppendType($parent, EDI_GetElementByID($s, "1253"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1250"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1251"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1254"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "755"), 0, 1);
        EDI_StoreComplexNode($s,$parent);

       $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "LIN");
        EDI_AppendType($parent, EDI_GetElementByID($s, "350"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "235"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "234"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "235"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "234"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "235"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "234"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "235"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "234"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "235"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "234"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "235"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "234"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "235"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "234"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "235"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "234"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "235"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "234"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "235"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "234"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "235"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "234"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "235"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "234"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "235"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "234"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "235"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "234"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "235"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "234"), 0, 1);
        EDI_StoreComplexNode($s,$parent);

       $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "CTP");
        EDI_AppendType($parent, EDI_GetElementByID($s, "687"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "236"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "212"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_AppendType($parent, EDI_GetComplexNodeByID($s, "C001"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "648"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "649"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "782"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "639"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "499"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "289"), 0, 1);
        EDI_StoreComplexNode($s,$parent);

       $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "SVD");
        EDI_AppendType($parent, EDI_GetElementByID($s, "67"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "782"), 1, 1);
        EDI_AppendType($parent, EDI_GetComplexNodeByID($s, "C003"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "234"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "380"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "554"), 0, 1);
        EDI_StoreComplexNode($s,$parent);

       $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "LQ");
        EDI_AppendType($parent, EDI_GetElementByID($s, "1270"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1271"), 0, 1);
        EDI_StoreComplexNode($s,$parent);

       $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "FRM");
        EDI_AppendType($parent, EDI_GetElementByID($s, "350"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "1073"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "127"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "373"), 0, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "332"), 0, 1);
        EDI_StoreComplexNode($s,$parent);

       $parent = EDI_CreateComplexType($s, EDITYPE_SEGMENT, "SE");
        EDI_AppendType($parent, EDI_GetElementByID($s, "96"), 1, 1);
        EDI_AppendType($parent, EDI_GetElementByID($s, "329"), 1, 1);
        EDI_StoreComplexNode($s,$parent);

        $loop1 = EDI_CreateComplexType($s, EDITYPE_LOOP, "transaction");
        EDI_AppendType($loop1, EDI_GetComplexNodeByID($s, "ST"), 1, 1);
        EDI_AppendType($loop1, EDI_GetComplexNodeByID($s, "BHT"), 1, 1);
        EDI_AppendType($loop1, EDI_GetComplexNodeByID($s, "REF"), 0, 3);
        $loop1000 = EDI_CreateComplexType($s, EDITYPE_LOOP, "Loop1000");
        EDI_AppendType($loop1000, EDI_GetComplexNodeByID($s, "NM1"), 1, 1);
        EDI_AppendType($loop1000, EDI_GetComplexNodeByID($s, "N2"), 0, 2);
        EDI_AppendType($loop1000, EDI_GetComplexNodeByID($s, "N3"), 0, 2);
        EDI_AppendType($loop1000, EDI_GetComplexNodeByID($s, "N4"), 0, 1);
        EDI_AppendType($loop1000, EDI_GetComplexNodeByID($s, "REF"), 0, 2);
        EDI_AppendType($loop1000, EDI_GetComplexNodeByID($s, "PER"), 0, 2);
        EDI_StoreComplexNode($s, $loop1000);
        EDI_AppendType($loop1, $loop1000, 0, 10);
        $loop2000 = EDI_CreateComplexType($s, EDITYPE_LOOP, "Loop2000");
        EDI_AppendType($loop2000, EDI_GetComplexNodeByID($s, "HL"), 1, 1);
        EDI_AppendType($loop2000, EDI_GetComplexNodeByID($s, "PRV"), 0, 1);
        EDI_AppendType($loop2000, EDI_GetComplexNodeByID($s, "SBR"), 0, 1);
        EDI_AppendType($loop2000, EDI_GetComplexNodeByID($s, "PAT"), 0, 1);
        EDI_AppendType($loop2000, EDI_GetComplexNodeByID($s, "DTP"), 0, 5);
        EDI_AppendType($loop2000, EDI_GetComplexNodeByID($s, "CUR"), 0, 1);
        $loop2010 = EDI_CreateComplexType($s, EDITYPE_LOOP, "Loop2010");
        EDI_AppendType($loop2010, EDI_GetComplexNodeByID($s, "NM1"), 1, 1);
        EDI_AppendType($loop2010, EDI_GetComplexNodeByID($s, "N2"), 0, 2);
        EDI_AppendType($loop2010, EDI_GetComplexNodeByID($s, "N3"), 0, 2);
        EDI_AppendType($loop2010, EDI_GetComplexNodeByID($s, "N4"), 0, 1);
        EDI_AppendType($loop2010, EDI_GetComplexNodeByID($s, "DMG"), 0, 1);
        EDI_AppendType($loop2010, EDI_GetComplexNodeByID($s, "REF"), 0, 20);
        EDI_AppendType($loop2010, EDI_GetComplexNodeByID($s, "PER"), 0, 2);
        EDI_StoreComplexNode($s, $loop2010);
        EDI_AppendType($loop2000, $loop2010, 0, 10);
        $loop2300 = EDI_CreateComplexType($s, EDITYPE_LOOP, "Loop2300");
        EDI_AppendType($loop2300, EDI_GetComplexNodeByID($s, "CLM"), 1, 1);
        EDI_AppendType($loop2300, EDI_GetComplexNodeByID($s, "DTP"), 0, 150);
        EDI_AppendType($loop2300, EDI_GetComplexNodeByID($s, "CL1"), 0, 1);
        EDI_AppendType($loop2300, EDI_GetComplexNodeByID($s, "DN1"), 0, 1);
        EDI_AppendType($loop2300, EDI_GetComplexNodeByID($s, "DN2"), 0, 35);
        EDI_AppendType($loop2300, EDI_GetComplexNodeByID($s, "PWK"), 0, 10);
        EDI_AppendType($loop2300, EDI_GetComplexNodeByID($s, "CN1"), 0, 1);
        EDI_AppendType($loop2300, EDI_GetComplexNodeByID($s, "DSB"), 0, 1);
        EDI_AppendType($loop2300, EDI_GetComplexNodeByID($s, "UR"), 0, 1);
        EDI_AppendType($loop2300, EDI_GetComplexNodeByID($s, "AMT"), 0, 40);
        EDI_AppendType($loop2300, EDI_GetComplexNodeByID($s, "REF"), 0, 30);
        EDI_AppendType($loop2300, EDI_GetComplexNodeByID($s, "K3"), 0, 10);
        EDI_AppendType($loop2300, EDI_GetComplexNodeByID($s, "NTE"), 0, 20);
        EDI_AppendType($loop2300, EDI_GetComplexNodeByID($s, "CR1"), 0, 1);
        EDI_AppendType($loop2300, EDI_GetComplexNodeByID($s, "CR2"), 0, 1);
        EDI_AppendType($loop2300, EDI_GetComplexNodeByID($s, "CR3"), 0, 1);
        EDI_AppendType($loop2300, EDI_GetComplexNodeByID($s, "CR4"), 0, 3);
        EDI_AppendType($loop2300, EDI_GetComplexNodeByID($s, "CR5"), 0, 1);
        EDI_AppendType($loop2300, EDI_GetComplexNodeByID($s, "CR6"), 0, 1);
        EDI_AppendType($loop2300, EDI_GetComplexNodeByID($s, "CR8"), 0, 1);
        EDI_AppendType($loop2300, EDI_GetComplexNodeByID($s, "CRC"), 0, 100);
        EDI_AppendType($loop2300, EDI_GetComplexNodeByID($s, "HI"), 0, 25);
        EDI_AppendType($loop2300, EDI_GetComplexNodeByID($s, "QTY"), 0, 10);
        EDI_AppendType($loop2300, EDI_GetComplexNodeByID($s, "HCP"), 0, 1);
        $loop2305 = EDI_CreateComplexType($s, EDITYPE_LOOP, "Loop2305");
        EDI_AppendType($loop2305, EDI_GetComplexNodeByID($s, "CR7"), 1, 1);
        EDI_AppendType($loop2305, EDI_GetComplexNodeByID($s, "HSD"), 0, 12);
        EDI_StoreComplexNode($s, $loop2305);
        EDI_AppendType($loop2300, $loop2305, 0, 6);
        $loop2310 = EDI_CreateComplexType($s, EDITYPE_LOOP, "Loop2310");
        EDI_AppendType($loop2310, EDI_GetComplexNodeByID($s, "NM1"), 1, 1);
        EDI_AppendType($loop2310, EDI_GetComplexNodeByID($s, "PRV"), 0, 1);
        EDI_AppendType($loop2310, EDI_GetComplexNodeByID($s, "N2"), 0, 2);
        EDI_AppendType($loop2310, EDI_GetComplexNodeByID($s, "N3"), 0, 2);
        EDI_AppendType($loop2310, EDI_GetComplexNodeByID($s, "N4"), 0, 1);
        EDI_AppendType($loop2310, EDI_GetComplexNodeByID($s, "REF"), 0, 20);
        EDI_AppendType($loop2310, EDI_GetComplexNodeByID($s, "PER"), 0, 2);
        EDI_StoreComplexNode($s, $loop2310);
        EDI_AppendType($loop2300, $loop2310, 0, 9);
        $loop2320 = EDI_CreateComplexType($s, EDITYPE_LOOP, "Loop2320");
        EDI_AppendType($loop2320, EDI_GetComplexNodeByID($s, "SBR"), 1, 1);
        EDI_AppendType($loop2320, EDI_GetComplexNodeByID($s, "CAS"), 0, 99);
        EDI_AppendType($loop2320, EDI_GetComplexNodeByID($s, "AMT"), 0, 15);
        EDI_AppendType($loop2320, EDI_GetComplexNodeByID($s, "DMG"), 0, 1);
        EDI_AppendType($loop2320, EDI_GetComplexNodeByID($s, "OI"), 0, 1);
        EDI_AppendType($loop2320, EDI_GetComplexNodeByID($s, "MIA"), 0, 1);
        EDI_AppendType($loop2320, EDI_GetComplexNodeByID($s, "MOA"), 0, 1);
        $loop2330 = EDI_CreateComplexType($s, EDITYPE_LOOP, "Loop2330");
        EDI_AppendType($loop2330, EDI_GetComplexNodeByID($s, "NM1"), 1, 1);
        EDI_AppendType($loop2330, EDI_GetComplexNodeByID($s, "N2"), 0, 2);
        EDI_AppendType($loop2330, EDI_GetComplexNodeByID($s, "N3"), 0, 2);
        EDI_AppendType($loop2330, EDI_GetComplexNodeByID($s, "N4"), 0, 1);
        EDI_AppendType($loop2330, EDI_GetComplexNodeByID($s, "PER"), 0, 2);
        EDI_AppendType($loop2330, EDI_GetComplexNodeByID($s, "DTP"), 0, 9);
        EDI_AppendType($loop2330, EDI_GetComplexNodeByID($s, "REF"), 0, 3);
        EDI_StoreComplexNode($s, $loop2330);
        EDI_AppendType($loop2320, $loop2330, 0, 10);
        EDI_StoreComplexNode($s, $loop2320);
        EDI_AppendType($loop2300, $loop2320, 0, 10);
        $loop2400 = EDI_CreateComplexType($s, EDITYPE_LOOP, "Loop2400");
        EDI_AppendType($loop2400, EDI_GetComplexNodeByID($s, "LX"), 1, 1);
        EDI_AppendType($loop2400, EDI_GetComplexNodeByID($s, "SV1"), 0, 1);
        EDI_AppendType($loop2400, EDI_GetComplexNodeByID($s, "SV2"), 0, 1);
        EDI_AppendType($loop2400, EDI_GetComplexNodeByID($s, "SV3"), 0, 1);
        EDI_AppendType($loop2400, EDI_GetComplexNodeByID($s, "TOO"), 0, 32);
        EDI_AppendType($loop2400, EDI_GetComplexNodeByID($s, "SV4"), 0, 1);
        EDI_AppendType($loop2400, EDI_GetComplexNodeByID($s, "SV5"), 0, 1);
        EDI_AppendType($loop2400, EDI_GetComplexNodeByID($s, "SV6"), 0, 1);
        EDI_AppendType($loop2400, EDI_GetComplexNodeByID($s, "SV7"), 0, 1);
        EDI_AppendType($loop2400, EDI_GetComplexNodeByID($s, "HI"), 0, 25);
        EDI_AppendType($loop2400, EDI_GetComplexNodeByID($s, "PWK"), 0, 10);
        EDI_AppendType($loop2400, EDI_GetComplexNodeByID($s, "CR1"), 0, 1);
        EDI_AppendType($loop2400, EDI_GetComplexNodeByID($s, "CR2"), 0, 5);
        EDI_AppendType($loop2400, EDI_GetComplexNodeByID($s, "CR3"), 0, 1);
        EDI_AppendType($loop2400, EDI_GetComplexNodeByID($s, "CR4"), 0, 3);
        EDI_AppendType($loop2400, EDI_GetComplexNodeByID($s, "CR5"), 0, 1);
        EDI_AppendType($loop2400, EDI_GetComplexNodeByID($s, "CRC"), 0, 3);
        EDI_AppendType($loop2400, EDI_GetComplexNodeByID($s, "DTP"), 0, 15);
        EDI_AppendType($loop2400, EDI_GetComplexNodeByID($s, "QTY"), 0, 5);
        EDI_AppendType($loop2400, EDI_GetComplexNodeByID($s, "MEA"), 0, 20);
        EDI_AppendType($loop2400, EDI_GetComplexNodeByID($s, "CN1"), 0, 1);
        EDI_AppendType($loop2400, EDI_GetComplexNodeByID($s, "REF"), 0, 30);
        EDI_AppendType($loop2400, EDI_GetComplexNodeByID($s, "AMT"), 0, 15);
        EDI_AppendType($loop2400, EDI_GetComplexNodeByID($s, "K3"), 0, 10);
        EDI_AppendType($loop2400, EDI_GetComplexNodeByID($s, "NTE"), 0, 10);
        EDI_AppendType($loop2400, EDI_GetComplexNodeByID($s, "PS1"), 0, 1);
        EDI_AppendType($loop2400, EDI_GetComplexNodeByID($s, "IMM"), 0, 999999);
        EDI_AppendType($loop2400, EDI_GetComplexNodeByID($s, "HSD"), 0, 1);
        EDI_AppendType($loop2400, EDI_GetComplexNodeByID($s, "HCP"), 0, 1);
        $loop2410 = EDI_CreateComplexType($s, EDITYPE_LOOP, "Loop2410");
        EDI_AppendType($loop2410, EDI_GetComplexNodeByID($s, "LIN"), 1, 1);
        EDI_AppendType($loop2410, EDI_GetComplexNodeByID($s, "CTP"), 0, 1);
        EDI_AppendType($loop2410, EDI_GetComplexNodeByID($s, "REF"), 0, 1);
        EDI_StoreComplexNode($s, $loop2410);
        EDI_AppendType($loop2400, $loop2410, 0, 999999);
        $loop2420 = EDI_CreateComplexType($s, EDITYPE_LOOP, "Loop2420");
        EDI_AppendType($loop2420, EDI_GetComplexNodeByID($s, "NM1"), 1, 1);
        EDI_AppendType($loop2420, EDI_GetComplexNodeByID($s, "PRV"), 0, 1);
        EDI_AppendType($loop2420, EDI_GetComplexNodeByID($s, "N2"), 0, 2);
        EDI_AppendType($loop2420, EDI_GetComplexNodeByID($s, "N3"), 0, 2);
        EDI_AppendType($loop2420, EDI_GetComplexNodeByID($s, "N4"), 0, 1);
        EDI_AppendType($loop2420, EDI_GetComplexNodeByID($s, "REF"), 0, 20);
        EDI_AppendType($loop2420, EDI_GetComplexNodeByID($s, "PER"), 0, 2);
        EDI_StoreComplexNode($s, $loop2420);
        EDI_AppendType($loop2400, $loop2420, 0, 10);

        $loop2430 = EDI_CreateComplexType($s, EDITYPE_LOOP, "Loop2430");
        EDI_AppendType($loop2430, EDI_GetComplexNodeByID($s, "SVD"), 1, 1);
        EDI_AppendType($loop2430, EDI_GetComplexNodeByID($s, "CAS"), 0, 99);
        EDI_AppendType($loop2430, EDI_GetComplexNodeByID($s, "DTP"), 0, 9);
        EDI_StoreComplexNode($s, $loop2430);
        EDI_AppendType($loop2400, $loop2430, 0, 999999);
        
        $loop2440 = EDI_CreateComplexType($s, EDITYPE_LOOP, "Loop2440");
        EDI_AppendType($loop2440, EDI_GetComplexNodeByID($s, "LQ"), 1, 1);
        EDI_AppendType($loop2440, EDI_GetComplexNodeByID($s, "FRM"), 0, 99);
        EDI_StoreComplexNode($s, $loop2440);
        EDI_AppendType($loop2400, $loop2440, 0, 999999);
        
        
        EDI_StoreComplexNode($s, $loop2400);
        EDI_AppendType($loop2300, $loop2400, 0, 999999);
        EDI_StoreComplexNode($s, $loop2300);
        EDI_AppendType($loop2000, $loop2300, 0, 100);
        EDI_StoreComplexNode($s, $loop2000);
        EDI_AppendType($loop1, $loop2000, 1, 999999);
        EDI_AppendType($loop1, EDI_GetComplexNodeByID($s, "SE"), 1, 1);
        
        EDI_InsertType(EDI_GetComplexNodeByID($s, "group"), $loop1, 2, 0, 999999999);        
    }
    return;
}

function handleDocumentStart($myData, $type)
{
    echo( "$prefix<document>\n");
    $depth++;
    $prefix[0] = '\0';
    for($i = 0; $i < $depth; $i++){
        $prefix .= "   ";
    }
    return;
}

function handleDocumentEnd($myData)
{
    $depth--;
    $prefix[0] = '\0';
    for($i = 0; $i < $depth; $i++){
        $prefix .= "   ";
    }
    echo( "$prefix</document>\n");
    return;
}

function handleSegmentStart($myData, $tag)
{
    echo($prefix."Seg: $tag ->");
    $curr_seg=$tag;
    $curr_e = 0;
    return;
}

function handleSegmentEnd($myData, $tag)
{
    echo(" <- End $tag\n");
    return;
}

function handleCompositeStart($myData)
{
    $curr_e++;
    $curr_c = 0;
    $is_comp = 1;
    echo("{");
    return;
}

function handleCompositeEnd($myData)
{
    echo( "}");
    $is_comp = 0;
    return;
}

function handleElement($myData, $element)
{
    $string = '';

    $counter++;
    switch($element->type){
        case EDI_DATA_INTEGER:
        case EDI_DATA_BINARY_SIZE:
            echo( "[$element->data.integer]");
            break;
        case EDI_DATA_DECIMAL:
            echo( "[$element->data.decimal]");
            break;
        default:
            echo( "[$element->data.string]");
            if($is_comp){
                $curr_c++;
            } else {
                $curr_e++;
            }
            if($group_start){
                if($counter == 1){
                    if(strcmp("HC", $element->data.string) == 0){
                        $type_ok = 1;
                    } else {
                        echo("*** Type not 'FA': No transaction-level validation will be performed on this functional group.\n");
                    }
                } else if($counter == 8){
                    if($type_ok){
                        if("004010" == substr($element->data.string,0,6)){
                            load_standard($myData);
                        } else {
                            echo( "*** Version not '004010': No transaction-level validation will be performed on this functional group.\n");
                        }
                    }
                    $group_start = 0;
                }
            }
            break;
    }
    return;
}

function handleJunk($myData, $val)
{
    echo("Junk: >>{$val}<<\n");
    return;

}

function main($argc, $argv)
{
    $bytes_read=0;
    $file_stat_size=0;
    $file_size = 0;
    $speed = 0;
    $input ='';
    $i = 0;
    $length = 0;
    $buff = '';
    $j = 0;
    $size = 0;
    $start_time = 0;
    $end_time = 0;
    $run_time = 0;
    $p = new EDI_Parser;
    $fin =0;

    if($argc < 2){
        echo("Not enough arguments, exiting.\n");
        exit(1);
    }
    
    $start_time = time();

    if(!($input = fopen($argv[1], "r"))){
        echo("Couldn't open\n");
        exit(1);
    }
        
    $p = EDI_ParserCreate();
    
    EDI_SetDocumentStartHandler($p, $handleDocumentStart);
    EDI_SetDocumentEndHandler($p, $handleDocumentEnd);
    EDI_SetSegmentStartHandler($p, $handleSegmentStart);
    EDI_SetSegmentEndHandler($p, $handleSegmentEnd);
    EDI_SetCompositeStartHandler($p, $handleCompositeStart);
    EDI_SetCompositeEndHandler($p, $handleCompositeEnd);
    EDI_SetElementHandler($p, $handleElement);
    EDI_SetNonEDIDataHandler($p, $handleJunk);
    EDI_SetUserData($p, $p);
    load_basic_standards($p);

    $buff = EDI_GetBuffer($p, BUFF_SIZE);
    $buff = fread($input,BUFF_SIZE);
    while(strlen($buff) > 0){
        $final = ($length < BUFF_SIZE) ? EDI_TRUE : EDI_FALSE;
        $status = EDI_ParseBuffer($p, $length, $final);
        if($status != EDI_STATUS_OK){
            echo("Error: EDI_GetErrorCode(p)\n");
        }
        $buff = EDI_GetBuffer($p, BUFF_SIZE);
        $buff = fread($input,BUFF_SIZE);
    }
    $s = EDI_GetSchema($p);    
    if($s){
        EDI_SchemaFree($s);
    }
    EDI_ParserFree($p);
    fclose($input);

    $end_time = time();
    $file_size = statbuf.st_size;
    echo("Processed $file_size Bytes of data\n");
    $run_time = $end_time - $start_time;
    echo("Runtime: $run_time seconds\n");
    $speed = ($file_size/1048576)/$run_time;
    echo("Average processing speed was $spedd MB/sec\n");
    return 0;
}
$myarr[] = Array(0 => '', 1 => 'ibmdata.txt');
main(2, $myarr);
?>
