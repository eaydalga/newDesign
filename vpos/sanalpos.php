<?php
    class SanalPos {
        
        function SanalPos() {
            $this->RET='';
            $this->USERNAME=''; 
            $this->PASSWORD=''; 
            $this->CLIENTID='';
            $this->CCNUMBER=''; 
            $this->CCEXP   =''; AY/YIL Duzeninde.. (06/05)
            $this->CCCVC2  =''; 
            $this->TOTAL   =''; // Cekilecek Tutar...
            $this->INSTALLMENT = ''; // Taksit sayisi..
            $this->B_NAME ='';
            $this->B_ADDR1 ='';
            $this->B_ADDR2 ='';
            $this->B_ADDR3 ='';
            $this->B_CITY ='';
            $this->B_STATE ='';
            $this->B_POSTALCODE ='';
            $this->B_COUNTRY =''; 
            $this->B_COMPANY ='';
            $this->B_TEL ='';
            $this->S_NAME ='';
            $this->S_ADDR1 ='';
            $this->S_ADDR2 ='';
            $this->S_ADDR3 ='';
            $this->S_CITY ='';
            $this->S_STATE ='';
            $this->S_POSTALCODE ='';
            $this->S_COUNTRY =''; 


            $this->xml=file_get_contents("ccpos.xml");
        }
    
        function setINSTALLMENT($x) { $this->INSTALLMENT = $x; } //Taksit Sayisi
        function setCCNUMBER($x) { $this->CCNUMBER = $x; }
        function setCCEXP($x) {    $this->CCEXP = $x; }
        function setCCCVC2($x) { $this->CCCVC2 = $x; }
        function setTOTAL($x) {    $this->TOTAL = $x; }
        function setB_NAME($x) { $this->B_NAME = $x; }
        function setB_ADDR1($x) { $this->B_ADDR1 = $x; }
        function setB_ADDR2($x) { $this->B_ADDR2 = $x; }
        function setB_ADDR3($x) { $this->B_ADDR3 = $x; }
        function setB_STATE($x) { $this->B_STATE = $x; }
        function setB_POSTALCODE($x) { $this->B_POSTALCODE = $x; }
        function setB_COMPANY($x) { $this->B_COMPANY = $x; }
        function setB_TEL($x) { $this->B_TEL = $x; }
        function setS_NAME($x) { $this->S_NAME = $x; }
        function setS_ADDR1($x) { $this->S_ADDR1 = $x; }
        function setS_ADDR2($x) { $this->S_ADDR2 = $x; }
        function setS_ADDR3($x) { $this->S_ADDR3 = $x; }
        function setS_STATE($x) { $this->S_STATE = $x; }
        function setS_POSTALCODE($x) { $this->S_POSTALCODE = $x; }
        function setS_COMPANY($x) { $this->S_COMPANY = $x; }
        function setS_TEL($x) { $this->S_TEL = $x; }

        
        function makeXMLTree ($data) {
           $output = array();
           
           $parser = xml_parser_create();

           xml_parser_set_option($parser, XML_OPTION_CASE_FOLDING, 0);
           xml_parser_set_option($parser, XML_OPTION_SKIP_WHITE, 1);
           xml_parse_into_struct($parser, $data, $values, $tags);
           xml_parser_free($parser);
           
           $hash_stack = array();
           
           foreach ($values as $key => $val)  {
               switch ($val['type']) {
                   case 'open':
                       array_push($hash_stack, $val['tag']);
                       break;
               
                   case 'close':
                       array_pop($hash_stack);
                       break;
               
                   case 'complete':
                       array_push($hash_stack, $val['tag']);
                       eval("\$output['" . implode($hash_stack, "']['") . "'] = \"{$val['value']}\";");
                       array_pop($hash_stack);
                       break;
               }
           }
           return $output;
        }

        function ccpos() {
            $this->xml = preg_replace("/\[(\w+)\]/e", "\$this->\\1", $this->xml);
            //echo $this->xml;
            $this->xml = urlencode($this->xml);
            $len = strlen("$this->xml") + 5;
            //echo "LEN====".$len;
            $headers =  "POST /servlet/cc5ApiServer HTTP/1.1\n".
                        "Host: ccpos.garanti.com.tr\n".
                        "Content-Type: application/x-www-form-urlencoded\n".
                        "Content-Length: $len\n\n".
                        "DATA=$this->xml";
            //$headers =  "POST /sanalpos/post.php HTTP/1.1\n"."Host: 10.0.0.250\n"."Content-Type: application/x-www-form-urlencoded\n"."Content-Length: $len\n\n"."DATA=$this->xml\n";
            //echo $headers;
            $fp = fsockopen("ssl://ccpos.garanti.com.tr", 443, $errno, $errstr);
            if (!$fp) {
                //echo 'socket acilamadi';
                return -2;
            }
            fputs($fp, $headers);
            $ret = '';
            $ret = fread($fp, 8192);
            //print($ret);
            $pure_xml = substr($ret, strpos($ret,'<CC5Response>'));
            $xml_array = $this->makeXMLTree($pure_xml);
            //print_r($xml_array);
            switch ($xml_array[CC5Response][Response]) {
                case 'Approved' : 
                    //echo "OK Cekildi";
                    $this->RET = 1;
                    break;

                case 'Declined' :
                    //echo "RED !!!";
                    $this->RET = 0;
                    break;

                case 'Error' :
                    //echo 'Error: '. $xml_array[CC5Response][ErrMsg];
                    $this->RET = -1;
                    break;
            }
            flush();
            fclose($fp);
            return $this->RET;
        }

    }
        
?>

