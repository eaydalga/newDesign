<?php
function IS_LEAP_YEAR($Y) {
   return ( (($Y)>0) && !(($Y)%4) && ( (($Y)%100) || !(($Y)%400) ) );
}

function DATE_IS_VALID($Y, $M, $D) 
{
    return($M < 8 && 
        (
                ($M==2 && ($D<=28 || (IS_LEAP_YEAR($Y) && $D<=29)))
            ||
                (!($M%2) && ($M!=2) && ($D<=30))
            || 
                ( ($M%2) && ($D<=31))
        )
    ) || ($M>7 && 
        (
            (!($M%2) && ($D<=31))
            || 
            ( ($M%2) && ($D<=30))
        )
    );
}
/******************************************************************************/
function EDI_CreateElementType($schema,
                               $type  ,
                               $id    ,
                               $minlen,
                               $maxlen)
{
    $node = EDI_SimpleType;
    
    $node = MALLOC(schema, sizeof(EDI_SimpleType));
    if(!$node){
        return '';
    }
    $node->header->type = EDITYPE_ELEMENT;
    $node->header->nodeID = EDI_strndup($id, strlen($id), $schema->memsuite);
    $node->header->refCount = 0;
    $node->header->schema = $schema;
    $node->type = $type;
    if($type == EDI_DATA_DATE){
        $node->min = ($minlen < 6) ? 6 : $minlen;
        $node->max = ($maxlen > 8) ? 8 : $maxlen;
    } else if(type == EDI_DATA_TIME){
        $node->min = ($minlen < 4) ? 4 : $minlen;
        $node->max = ($maxlen > 8) ? 8 : $maxlen;
    } else {    
        $node->min = $minlen;
        $node->max = $maxlen;
    }
    $node->values = '';
    if(hashtable_insert($schema->elements, EDI_strdup($id), $node)){
        return $node;
    }
    return '';
}
/******************************************************************************/
function EDI_GetElementByID($schema, $nodeID)
{
    $node = EDI_SchemaNode;
    $table = hashtable;
    
    $table = $schema->elements;
    if($node = hashtable_search($table, $nodeID)){
        return $node;
    } else {
        return '';
    }
}

/******************************************************************************/
function EDI_AddElementValue($node ,
                             $value)
{
    $element = $node;
    $error   = VAL_UNKNOWN_ELEMENT;
    
    if($node->type != EDITYPE_ELEMENT){
        $error = VAL_UNKNOWN_ELEMENT;
    } else if($element){
        $error = EDI_CheckElementConstraints($element, $value, strlen($value), '');
        if(!$error || $error == VAL_CODE_ERROR){
            if(!$element->values){
                $element->values = create_hashtable(20);
            }
            if(hashtable_insert($element->values, EDI_strdup($value), EDI_strdup($value))){
                $error = VAL_VALID_ELEMENT;
            }
        }
    }
    return $error;
}
/******************************************************************************/
function EDI_CheckElementConstraints($element,
                                  $value  ,
                                  $length ,
                                  $results)
{
    $check    = '';
    $invalid  = '';
    $llvalue  = 0;
    $ldvalue  = 0.0;
    $i        = 0;
    
    if($element){
        switch($element->type){
            case EDI_DATA_STRING:
                if($length > $element->max){
                    return VAL_RANGE_HIGH;
                } else if($length < $element->min){
                    return VAL_RANGE_LOW;
                }
                if($element->values){
                    if(! hashtable_search($element->values, $value)){
                        return VAL_CODE_ERROR;
                    }
                } else {
                    for($i = 0; $i < $llvalue; $i++){
                        if(!isprint($value[$i])){
                            return VAL_CHAR_ERROR;
                        }
                    }
                }
                break;
            case EDI_DATA_INTEGER:
                if($value[0] == '+' || $value[0] == '-'){
                    $length--;
                }
                if($length > $element->max){
                    return VAL_RANGE_HIGH;
                } else if($length < $element->min){
                    return VAL_RANGE_LOW;
                }
                $llvalue = strtoll($value, $invalid, 10);
                if($value[0] == '' || $invalid != ''){
                    return VAL_CHAR_ERROR;
                }
                if($results){
                    $results->type = EDI_DATA_INTEGER;
                    $results->data->integer = llvalue;
                }
                break;
            case EDI_DATA_DECIMAL:
                if($value[0] == '+' || $value[0] == '-'){
                    $length--;
                    $check = $value + 1;                
                } else {
                    $check = $value;
                }
                for($i = 0;$check;$check++){
                    if(!isdigit($check) && $check != '.'){
                        return VAL_CHAR_ERROR;
                    } else if($check == '.' && ++$i > 1){
                        return VAL_CHAR_ERROR;
                    }
                }
                $length -= $i;
                if($length > $element->max){
                    return VAL_RANGE_HIGH;
                } else if($length < $element->min){
                    return VAL_RANGE_LOW;
                }
                $ldvalue = strtold($value, $invalid);
                if($value == $invalid || $invalid != '\0'){
                    return VAL_CHAR_ERROR;
                }
                if($results){
                    $results->type = EDI_DATA_DECIMAL;
                    $results->data->decimal = $ldvalue;
                }
                break;
            case EDI_DATA_DATE:
                if($length > $element->max){
                    return VAL_RANGE_HIGH;
                } else if($length < $element->min){
                    return VAL_RANGE_LOW;
                } else if($length % 2){
                    return VAL_DATE_ERROR;
                }
                $llvalue = strtoll($value, $invalid, 10);
                if($value[0] == '' || $invalid != ''){
                    return VAL_DATE_ERROR;
                }
                $date[] = Array(0, 0, 0);
                $date[2] = $llvalue % 100;
                $llvalue /= 100;
                $date[1] = $llvalue % 100;
                $llvalue /= 100;            
                $date[0] = $llvalue % 100;
                $llvalue /= 100;
                if($length == 6){
                    $curtime = time('');
                    $loctime = localtime($curtime);
                    $date[0] += (($loctime->tm_year + 1900) / 1000)*10000000;
                } else {
                    $date[0] += $llvalue * 100;    
                }
                if(!DATE_IS_VALID($date[0], $date[1], $date[2])){
                    return VAL_DATE_ERROR;
                }
                break;
            case EDI_DATA_TIME:
                if($length > $element->max){
                    return VAL_RANGE_HIGH;
                } else if($length < $element->min){
                    return VAL_RANGE_LOW;
                } else if($length % 2){
                    return VAL_TIME_ERROR;
                }
                $llvalue = strtoll($value, $invalid, 10);
                if($value[0] == '' || $invalid != ''){
                    return VAL_TIME_ERROR;
                }
                $time[] = Array(0, 0, 0, 0);
                for($i = $length; $i > 0; $i-=2){
                    $time[($i/2)-1] = $llvalue % 100;
                    $llvalue /= 100;
                }                
                if( ($time[0] < 0) || ($time[0] > 23) || 
                    ($time[1] < 0) || ($time[1] > 59) || 
                    ($time[2] < 0) || ($time[2] > 59) || ($time[3] < 0)){
                    return VAL_TIME_ERROR;
                }                
                break;
            case EDI_DATA_BINARY_SIZE: /* Same code as integer, only save to the parser. */
                if($value[0] == '+' || $value[0] == '-'){
                    $length--;
                }
                if($length > $element->max){
                    return VAL_RANGE_HIGH;
                } else if($length < $element->min){
                    return VAL_RANGE_LOW;
                }
                $llvalue = strtoll($value, $invalid, 10);
                if($value[0] == '' || $invalid != ''){
                    return VAL_CHAR_ERROR;
                }
                $element->header->schema->parser->binaryElementSize = $llvalue;
                if($results){
                    $results->type = EDI_DATA_INTEGER;
                    $results->data->integer = $llvalue;
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
function EDI_DisposeSimpleType($node)
{
    $element = EDI_SimpleType;
    $table = hashtable;

    if($node && $node->refCount == 0){
        /*table = schema->elements;*/
        $table = $node->schema->elements;
        if($table){
            $element = hashtable_search($table, $node->nodeID);
            if($element && $element->values){
                hashtable_destroy($element->values, EDI_TRUE);
            }
        }
        /*FREE(schema, node->nodeID);
        FREE(schema, node);*/
        FREE($node->schema, $node->nodeID);
        FREE($node->schema, $node);
    }
    return;
}
?>
