<?php
function EDI_GAP_SCAN($parser, $pointer) {
   if((strlen($pointeri) - strlen($parser->bufReadPtr)) > 0) {
       $junk = 0;
       $prefix = strlen($pointer) - strlen($parser->bufReadPtr);
       $search;
       for($search = strlen($parser->bufReadPtr); $search<$pointer; $search++){
           if(!isspace(substr($pointer,$search,1))){
               $junk++;
           }
       }
       if($junk){
           $garbage = edi_strndup($parser->bufReadPtr, $prefix, $parser->memsuite);
           if(!$garbage){
               $parser->errorCode = EDI_ERROR_NO_MEM;
               return $parser->error;
           }
           $parser->nonEDIDataHandler($parser->userData, $garbage);
           $parser->$bufReadPtr = $pointer;
       }
   }
}


function EDI_strnlen($str)
{
    return strlen($str);
}

function EDI_strndup($s, $memsuit)
{
   $memsuit[] = $s;
}   

function strin_eq($str1, $str2)
{
   $rc = 0;
   if($str1 < $str2) $rc = -1;
   if($str1 > $str2) $rc = 1;
   return $rc;
}

function EDI_strdup($s)
{
   $new = $s;
   return $new;
}
?>
