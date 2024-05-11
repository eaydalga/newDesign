<?php
   function read_row($sorgu, $usr) {
       global $mystatus;
       $bsonuc = mysql_query($sorgu);
       if($bsonuc) {
            $arr = mysql_fetch_row($bsonuc);
            if(empty($arr[0]))
                      $mystatus = $usr . ' için kayıt yok';
            @mysql_freeresult($bsonuc);
       } else {
            $arr="";
            $mystatus = mysql_error();
       }
       return($arr);
   }
   function update_row($sorgu) {
       $bsonuc = mysql_query($sorgu);
       if($bsonuc) {
           $mystat = $bsonuc . " kayit güncellendi";
           if(isset($pp) && $pp == "bos") {
               $mystat .= " şifre değeri boştur";
           }
       } else {
           $mystat = mysql_error();
       }
       @mysql_freeresult($bsonuc);
       return($mystat);
   }
   function insert_row($sorgu) {
       $bsonuc = mysql_query($sorgu);
       if($bsonuc) {
           $mystat = $bsonuc . " kayit eklendi";
           if(isset($pp) && $pp == "bos") {
               $mystat .= " şifre değeri boştur";
           }
       } else {
           $mystat = mysql_error();
       }
       @mysql_freeresult($bsonuc);
       return $mystat;
   }
   function multi_row($islem, $sorgu) {
       global $mystatus;
       $bsonuc = mysql_query($sorgu);
       if($bsonuc) {
         $arr = mysql_fetch_row($bsonuc);
         if(!$arr[0])
            if($islem == "N")
                 $mystatus = $userno1 . 'daha sonrası yok';
            else $mystatus = $userno1 . 'daha öncesi yok';
         else
            @mysql_freeresult($bsonuc);
       } else {
          $mystatus = mysql_error();
       }
       return($arr);
   }
?>
