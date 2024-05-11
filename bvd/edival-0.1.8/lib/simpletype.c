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

#include "simpletype.h"
/******************************************************************************/
EDI_SchemaNode EDI_CreateElementType(EDI_Schema                  schema,
                                     enum EDI_PrimitiveDataType  type  ,
                                     const char                 *id    ,
                                     unsigned int                minlen,
                                     unsigned int                maxlen)
{
	EDI_SimpleType *node = NULL;
	
	node = MALLOC(schema, sizeof(EDI_SimpleType));
	if(!node){
		return NULL;
	}
	node->header.type = EDITYPE_ELEMENT;
	node->header.nodeID = EDI_strndup(id, strlen(id), schema->memsuite);
	node->header.refCount = 0;
	node->header.schema = schema;
	node->type = type;
	if(type == EDI_DATA_DATE){
		node->min = (minlen < 6) ? 6 : minlen;
		node->max = (maxlen > 8) ? 8 : maxlen;
	} else if(type == EDI_DATA_TIME){
		node->min = (minlen < 4) ? 4 : minlen;
		node->max = (maxlen > 8) ? 8 : maxlen;
	} else {	
		node->min = minlen;
		node->max = maxlen;
	}
	node->values = NULL;
	if(hashtable_insert(schema->elements, (void *)EDI_strdup(id), (void *)node)){
		return (EDI_SchemaNode)node;
	}
	return NULL;
}
/******************************************************************************/
EDI_SchemaNode EDI_GetElementByID(EDI_Schema  schema,
                                  const char *nodeID)
{
	EDI_SchemaNode    node  = NULL;
	struct hashtable *table = NULL;
	
	table = schema->elements;
	if((node = (EDI_SchemaNode)hashtable_search(table, (void *)nodeID))){
		return node;
	} else {
		return NULL;
	}
}

/******************************************************************************/
enum EDI_ElementValidationError EDI_AddElementValue(EDI_SchemaNode  node ,
                                                    const char     *value)
{
	EDI_SimpleType *element = (EDI_SimpleType *)node;
	enum EDI_ElementValidationError error   = VAL_UNKNOWN_ELEMENT;
	
	if(node->type != EDITYPE_ELEMENT){
		error = VAL_UNKNOWN_ELEMENT;
	} else if(element){
		error = EDI_CheckElementConstraints(element, value, strlen(value), NULL);
		if(!error || error == VAL_CODE_ERROR){
			if(!element->values){
				element->values = create_hashtable(20);
			}
			if(hashtable_insert(element->values, (void *)EDI_strdup(value), (void *)EDI_strdup(value))){
				error = VAL_VALID_ELEMENT;
			}
		}
	}
	return error;
}
/******************************************************************************/
enum EDI_ElementValidationError EDI_CheckElementConstraints(EDI_SimpleType *element,
                                                            const char     *value  ,
                                                            int             length ,
                                                            EDI_DataElement results)
{
	const char       *check    = NULL;
	char             *invalid  = NULL;
	long long         llvalue  = 0;
	long double       ldvalue  = 0.0;
	long              i        = 0;
	
	if(element){
		switch(element->type){
			case EDI_DATA_STRING:
				if(length > element->max){
					return VAL_RANGE_HIGH;
				} else if(length < element->min){
					return VAL_RANGE_LOW;
				}
				if(element->values){
					if(! hashtable_search(element->values, (void *)value)){
						return VAL_CODE_ERROR;
					}
				} else {
					for(i = 0; i < llvalue; i++){
						if(!isprint(value[i])){
							return VAL_CHAR_ERROR;
						}
					}
				}
				break;
			case EDI_DATA_INTEGER:
				if(value[0] == '+' || value[0] == '-'){
					length--;
				}
				if(length > element->max){
					return VAL_RANGE_HIGH;
				} else if(length < element->min){
					return VAL_RANGE_LOW;
				}
				llvalue = strtoll(value, &invalid, 10);
				if(value[0] == '\0' || *invalid != '\0'){
					return VAL_CHAR_ERROR;
				}
				if(results){
					results->type = EDI_DATA_INTEGER;
					results->data.integer = llvalue;
				}
				break;
			case EDI_DATA_DECIMAL:
				if(value[0] == '+' || value[0] == '-'){
					length--;
					check = value + 1;				
				} else {
					check = value;
				}
				for(i = 0;*check;check++){
					if(!isdigit(*check) && *check != '.'){
						return VAL_CHAR_ERROR;
					} else if(*check == '.' && ++i > 1){
						return VAL_CHAR_ERROR;
					}
				}
				length -= i;
				if(length > element->max){
					return VAL_RANGE_HIGH;
				} else if(length < element->min){
					return VAL_RANGE_LOW;
				}
				ldvalue = strtold(value, &invalid);
				if(value == invalid || *invalid != '\0'){
					return VAL_CHAR_ERROR;
				}
				if(results){
					results->type = EDI_DATA_DECIMAL;
					results->data.decimal = ldvalue;
				}
				break;
			case EDI_DATA_DATE:
				if(length > element->max){
					return VAL_RANGE_HIGH;
				} else if(length < element->min){
					return VAL_RANGE_LOW;
				} else if(length % 2){
					return VAL_DATE_ERROR;
				}
				llvalue = strtoll(value, &invalid, 10);
				if(value[0] == '\0' || *invalid != '\0'){
					return VAL_DATE_ERROR;
				}
				long date[] = {0, 0, 0};
				date[2] = llvalue % 100;
				llvalue /= 100;
				date[1] = llvalue % 100;
				llvalue /= 100;			
				date[0] = llvalue % 100;
				llvalue /= 100;
				if(length == 6){
					time_t curtime = time(NULL);
					struct tm *loctime = localtime(&curtime);
					date[0] += ((int)(loctime->tm_year + 1900) / 1000)*10000000;
				} else {
					date[0] += llvalue * 100;	
				}
				if(!DATE_IS_VALID(date[0], date[1], date[2])){
					return VAL_DATE_ERROR;
				}
				break;
			case EDI_DATA_TIME:
				if(length > element->max){
					return VAL_RANGE_HIGH;
				} else if(length < element->min){
					return VAL_RANGE_LOW;
				} else if(length % 2){
					return VAL_TIME_ERROR;
				}
				llvalue = strtoll(value, &invalid, 10);
				if(value[0] == '\0' || *invalid != '\0'){
					return VAL_TIME_ERROR;
				}
				long time[] = {0, 0, 0, 0};
				for(i = length; i > 0; i-=2){
					time[(i/2)-1] = llvalue % 100;
					llvalue /= 100;
				}				
				if( (time[0] < 0) || (time[0] > 23) || 
				    (time[1] < 0) || (time[1] > 59) || 
				    (time[2] < 0) || (time[2] > 59) || (time[3] < 0)){
					return VAL_TIME_ERROR;
				}				
				break;
			case EDI_DATA_BINARY_SIZE: /* Same code as integer, only save to the parser. */
				if(value[0] == '+' || value[0] == '-'){
					length--;
				}
				if(length > element->max){
					return VAL_RANGE_HIGH;
				} else if(length < element->min){
					return VAL_RANGE_LOW;
				}
				llvalue = strtoll(value, &invalid, 10);
				if(value[0] == '\0' || *invalid != '\0'){
					return VAL_CHAR_ERROR;
				}
				element->header.schema->parser->binaryElementSize = llvalue;
				if(results){
					results->type = EDI_DATA_INTEGER;
					results->data.integer = llvalue;
				}
				break;
			case EDI_DATA_BINARY:
				break;
			default:
				return VAL_UNKNOWN_ELEMENT;
				break;
		}
		return VAL_VALID_ELEMENT;
	} else {
		return VAL_UNKNOWN_ELEMENT;
	}
}
/******************************************************************************/
void EDI_DisposeSimpleType(EDI_SchemaNode node)
{
	EDI_SimpleType   *element = NULL;
	struct hashtable *table   = NULL;

	if(node && node->refCount == 0){
		/*table = schema->elements;*/
		table = node->schema->elements;
		if(table){
			element = (EDI_SimpleType*)hashtable_search(table, node->nodeID);
			if(element && element->values){
				hashtable_destroy(element->values, EDI_TRUE);
			}
		}
		/*FREE(schema, node->nodeID);
		FREE(schema, node);*/
		FREE(node->schema, node->nodeID);
		FREE(node->schema, node);
	}
	return;
}
