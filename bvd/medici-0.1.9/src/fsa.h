#ifndef FSA
#define FSA
typedef struct
{
  int trans;
  int (*ccode) (void *, char, int);
}
FSARule;
typedef struct
{
  int state, states, events;
  FSARule *rules;
}
FSAutomaton;

void FSAInit (FSAutomaton *, FSARule *, int, int, int);
int FSAProcess (FSAutomaton *, void *, char, int);
#endif

/*                                                                        */
/* The MEDICI Electronic Data Interchange Library                         */
/* Copyright (C) 2002  David Coles                                        */
/*                                                                        */
/* This library is free software; you can redistribute it and/or          */
/* modify it under the terms of the GNU Lesser General Public             */
/* License as published by the Free Software Foundation; either           */
/* version 2.1 of the License, or (at your option) any later version.     */
/*                                                                        */
/* This library is distributed in the hope that it will be useful,        */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of         */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      */
/* Lesser General Public License for more details.                        */
/*                                                                        */
/* You should have received a copy of the GNU Lesser General Public       */
/* License along with this library; if not, write to the Free Software    */
/* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */
/*                                                                        */
/* This code is absolute gibberish. But it does seem to work.             */
/* Mostly it's just horrid nested #defines - one day I'll document it ;)  */
/* STATE  EVENT   TRANS   ACTION                                          */
/* STATE  EVENT   TRANS   ACTION                                          */
/* STATE  EVENT   TRANS   ACTION                                          */
/* STATE	EVENT	TRANS	ACTION                                               */
/* FIXME - These next two lines are a hack.                               */
/* I have no idea what the X12 spec says about how cr/lf chars are handled. */
/* At least this allows them to be transparently ignored.                 */
#define SYNTAX_DEFAULT  0
#define SYNTAX_A        1
#define SYNTAX_B        2
#define SYNTAX_CR       3
#define SYNTAX_ES       4
#define SYNTAX_I        5
#define SYNTAX_LF       6
#define SYNTAX_N        7
#define SYNTAX_RI       8
#define SYNTAX_S        9
#define SYNTAX_SS       10
#define SYNTAX_ST       11
#define SYNTAX_T        12
#define SYNTAX_TS       13
#define SYNTAX_U        14
#define SYNTAX_X        15
#define SYNTAX_DATA     1
#define SYNTAX_ISA10    2
#define SYNTAX_ISA100   3
#define SYNTAX_ISA101   4
#define SYNTAX_ISA102   5
#define SYNTAX_ISA103   6
#define SYNTAX_ISA104   7
#define SYNTAX_ISA105   8
#define SYNTAX_ISA106   9
#define SYNTAX_ISA11    10
#define SYNTAX_ISA12    11
#define SYNTAX_ISA13    12
#define SYNTAX_ISA14    13
#define SYNTAX_ISA15    14
#define SYNTAX_ISA16    15
#define SYNTAX_ISA17    16
#define SYNTAX_ISA18    17
#define SYNTAX_ISA19    18
#define SYNTAX_ISA2     19
#define SYNTAX_ISA20    20
#define SYNTAX_ISA21    21
#define SYNTAX_ISA22    22
#define SYNTAX_ISA23    23
#define SYNTAX_ISA24    24
#define SYNTAX_ISA25    25
#define SYNTAX_ISA26    26
#define SYNTAX_ISA27    27
#define SYNTAX_ISA28    28
#define SYNTAX_ISA29    29
#define SYNTAX_ISA3     30
#define SYNTAX_ISA30    31
#define SYNTAX_ISA31    32
#define SYNTAX_ISA32    33
#define SYNTAX_ISA33    34
#define SYNTAX_ISA34    35
#define SYNTAX_ISA35    36
#define SYNTAX_ISA36    37
#define SYNTAX_ISA37    38
#define SYNTAX_ISA38    39
#define SYNTAX_ISA39    40
#define SYNTAX_ISA4     41
#define SYNTAX_ISA40    42
#define SYNTAX_ISA41    43
#define SYNTAX_ISA42    44
#define SYNTAX_ISA43    45
#define SYNTAX_ISA44    46
#define SYNTAX_ISA45    47
#define SYNTAX_ISA46    48
#define SYNTAX_ISA47    49
#define SYNTAX_ISA48    50
#define SYNTAX_ISA49    51
#define SYNTAX_ISA5     52
#define SYNTAX_ISA50    53
#define SYNTAX_ISA51    54
#define SYNTAX_ISA52    55
#define SYNTAX_ISA53    56
#define SYNTAX_ISA54    57
#define SYNTAX_ISA55    58
#define SYNTAX_ISA56    59
#define SYNTAX_ISA57    60
#define SYNTAX_ISA58    61
#define SYNTAX_ISA59    62
#define SYNTAX_ISA6     63
#define SYNTAX_ISA60    64
#define SYNTAX_ISA61    65
#define SYNTAX_ISA62    66
#define SYNTAX_ISA63    67
#define SYNTAX_ISA64    68
#define SYNTAX_ISA65    69
#define SYNTAX_ISA66    70
#define SYNTAX_ISA67    71
#define SYNTAX_ISA68    72
#define SYNTAX_ISA69    73
#define SYNTAX_ISA7     74
#define SYNTAX_ISA70    75
#define SYNTAX_ISA71    76
#define SYNTAX_ISA72    77
#define SYNTAX_ISA73    78
#define SYNTAX_ISA74    79
#define SYNTAX_ISA75    80
#define SYNTAX_ISA76    81
#define SYNTAX_ISA77    82
#define SYNTAX_ISA78    83
#define SYNTAX_ISA79    84
#define SYNTAX_ISA8     85
#define SYNTAX_ISA80    86
#define SYNTAX_ISA81    87
#define SYNTAX_ISA82    88
#define SYNTAX_ISA83    89
#define SYNTAX_ISA84    90
#define SYNTAX_ISA85    91
#define SYNTAX_ISA86    92
#define SYNTAX_ISA87    93
#define SYNTAX_ISA88    94
#define SYNTAX_ISA89    95
#define SYNTAX_ISA9     96
#define SYNTAX_ISA90    97
#define SYNTAX_ISA91    98
#define SYNTAX_ISA92    99
#define SYNTAX_ISA93    100
#define SYNTAX_ISA94    101
#define SYNTAX_ISA95    102
#define SYNTAX_ISA96    103
#define SYNTAX_ISA97    104
#define SYNTAX_ISA98    105
#define SYNTAX_ISA99    106
#define SYNTAX_RELEASE  107
#define SYNTAX_SSA1     108
#define SYNTAX_SSA2     109
#define SYNTAX_SSA3     110
#define SYNTAX_SSA4     111
#define SYNTAX_SSA5     112
#define SYNTAX_SSA6     113
#define SYNTAX_START    114
#define SYNTAX_STX1     115
#define SYNTAX_STX2     116
#define SYNTAX_STX3     117
#define SYNTAX_TAG      118
#define SYNTAX_UDAT     119
#define SYNTAX_UN1      120
#define SYNTAX_UN2      121
#define SYNTAX_UNB1     122
#define SYNTAX_UNB2     123
#define SYNTAX_UNB3     124
#define SYNTAX_UNB4     125
#define SYNTAX_UREL     126
#define SYNTAX_UTAG     127
#define SYNTAX_XDAT     128
#define SYNTAX_XTAG     129

/* # Determine if an EDIFACT numeric value meets the spec                 */
/* # FIXME - _READ_ the spec, don't just guess ;)                         */
#define EDIFACTNumeric_DEFAULT  0
#define EDIFACTNumeric_DECMARK  1
#define EDIFACTNumeric_MINUS    2
#define EDIFACTNumeric_NUMBER   3
#define EDIFACTNumeric_DFAULT   1
#define EDIFACTNumeric_FPART    2
#define EDIFACTNumeric_IPART    3
#define EDIFACTNumeric_SIGNED   4
#define EDIFACTNumeric_START    5

/* OLD STUFF ...                                                          */
/* STATE  EVENT   TRANS   ACTION                                          */
#define EDIFACT_DEFAULT  0
#define EDIFACT_A        1
#define EDIFACT_B        2
#define EDIFACT_ES       3
#define EDIFACT_N        4
#define EDIFACT_RI       5
#define EDIFACT_SS       6
#define EDIFACT_ST       7
#define EDIFACT_U        8
#define EDIFACT_DATA     1
#define EDIFACT_RELEASE  2
#define EDIFACT_SSA1     3
#define EDIFACT_SSA2     4
#define EDIFACT_SSA3     5
#define EDIFACT_SSA4     6
#define EDIFACT_SSA5     7
#define EDIFACT_SSA6     8
#define EDIFACT_START    9
#define EDIFACT_TAG      10
#define EDIFACT_UN1      11
#define EDIFACT_UN2      12
#define EDIFACT_UNB1     13
#define EDIFACT_UNB2     14
#define EDIFACT_UNB3     15
#define EDIFACT_UNB4     16

/* STATE  EVENT   TRANS   ACTION                                          */
#define UNGTDI_DEFAULT  0
#define UNGTDI_DES      1
#define UNGTDI_RI       2
#define UNGTDI_S        3
#define UNGTDI_SES      4
#define UNGTDI_ST       5
#define UNGTDI_T        6
#define UNGTDI_TS       7
#define UNGTDI_X        8
#define UNGTDI_DATA     1
#define UNGTDI_RELEASE  2
#define UNGTDI_START    3
#define UNGTDI_STX1     4
#define UNGTDI_STX2     5
#define UNGTDI_STX3     6
#define UNGTDI_TAG      7

/* STATE	EVENT	TRANS	ACTION                                               */
#define X12_DEFAULT  0
#define X12_A        1
#define X12_ES       2
#define X12_I        3
#define X12_S        4
#define X12_SS       5
#define X12_ST       6
#define X12_DATA     1
#define X12_ISA10    2
#define X12_ISA100   3
#define X12_ISA101   4
#define X12_ISA102   5
#define X12_ISA103   6
#define X12_ISA104   7
#define X12_ISA105   8
#define X12_ISA106   9
#define X12_ISA11    10
#define X12_ISA12    11
#define X12_ISA13    12
#define X12_ISA14    13
#define X12_ISA15    14
#define X12_ISA16    15
#define X12_ISA17    16
#define X12_ISA18    17
#define X12_ISA19    18
#define X12_ISA2     19
#define X12_ISA20    20
#define X12_ISA21    21
#define X12_ISA22    22
#define X12_ISA23    23
#define X12_ISA24    24
#define X12_ISA25    25
#define X12_ISA26    26
#define X12_ISA27    27
#define X12_ISA28    28
#define X12_ISA29    29
#define X12_ISA3     30
#define X12_ISA30    31
#define X12_ISA31    32
#define X12_ISA32    33
#define X12_ISA33    34
#define X12_ISA34    35
#define X12_ISA35    36
#define X12_ISA36    37
#define X12_ISA37    38
#define X12_ISA38    39
#define X12_ISA39    40
#define X12_ISA4     41
#define X12_ISA40    42
#define X12_ISA41    43
#define X12_ISA42    44
#define X12_ISA43    45
#define X12_ISA44    46
#define X12_ISA45    47
#define X12_ISA46    48
#define X12_ISA47    49
#define X12_ISA48    50
#define X12_ISA49    51
#define X12_ISA5     52
#define X12_ISA50    53
#define X12_ISA51    54
#define X12_ISA52    55
#define X12_ISA53    56
#define X12_ISA54    57
#define X12_ISA55    58
#define X12_ISA56    59
#define X12_ISA57    60
#define X12_ISA58    61
#define X12_ISA59    62
#define X12_ISA6     63
#define X12_ISA60    64
#define X12_ISA61    65
#define X12_ISA62    66
#define X12_ISA63    67
#define X12_ISA64    68
#define X12_ISA65    69
#define X12_ISA66    70
#define X12_ISA67    71
#define X12_ISA68    72
#define X12_ISA69    73
#define X12_ISA7     74
#define X12_ISA70    75
#define X12_ISA71    76
#define X12_ISA72    77
#define X12_ISA73    78
#define X12_ISA74    79
#define X12_ISA75    80
#define X12_ISA76    81
#define X12_ISA77    82
#define X12_ISA78    83
#define X12_ISA79    84
#define X12_ISA8     85
#define X12_ISA80    86
#define X12_ISA81    87
#define X12_ISA82    88
#define X12_ISA83    89
#define X12_ISA84    90
#define X12_ISA85    91
#define X12_ISA86    92
#define X12_ISA87    93
#define X12_ISA88    94
#define X12_ISA89    95
#define X12_ISA9     96
#define X12_ISA90    97
#define X12_ISA91    98
#define X12_ISA92    99
#define X12_ISA93    100
#define X12_ISA94    101
#define X12_ISA95    102
#define X12_ISA96    103
#define X12_ISA97    104
#define X12_ISA98    105
#define X12_ISA99    106
#define X12_START    107
#define X12_TAG      108

#define ISO9735Decimal_DEFAULT  0
#define ISO9735Decimal_DECMARK  1
#define ISO9735Decimal_MINUS    2
#define ISO9735Decimal_NUMBER   3
#define ISO9735Decimal_FPART    1
#define ISO9735Decimal_IPART    2
#define ISO9735Decimal_START    3
