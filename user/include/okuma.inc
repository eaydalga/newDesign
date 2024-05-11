<?php
   function resimal($usr)
   {
      global $defaultpic;
      global $resimno;
      global $mystatus;
      $sorgu = "SELECT bilgiAlani from personel where usrNo='$usr' and bilgiKodu='$resimno'";
      list($fresim) = read_row($sorgu,$usr);
      /* burada uyenin resmi kullanilir */
      if(strlen($fresim)) {
          if(!file_exists($fresim)) {
              $fresim = $defaultpic;
          }
      } else {
          $fresim = $defaultpic;
          $mystatus = "";
      }
   return $fresim;
   }
   function my_tabloyaz($sorgu,$keyval,$header)
   {
       $result = mysql_query($sorgu);
       if($result) {
          echo($keyval."<br>");
          echo "<table class=\"yyazi\">\n";
          echo $header;
          while ($line = mysql_fetch_array($result, MYSQL_ASSOC)) {
               echo "\t<tr>\n";
               foreach ($line as $col_value) {
                   echo "\t\t<td valign=\"top\">$col_value</td>\n";
               }
               echo "\t</tr>\n";
          }
          echo "</table>\n";
          @mysql_freeresult($result);
       }
   }
   function my_haberyaz($sorgu,$fdir)
   {
       $result = mysql_query($sorgu);
       if($result) {
          $i = 0;
          while (list($haberno, $dosyaadi ) = mysql_fetch_row($result)) {
            /* read file and print here */
            $file = $fdir."/".$dosyaadi;
            if(file_exists($file)) {
                include($file);
                // $text=file_get_contents($file);
                // echo iconv("ISO-8859-9","UTF-8",$text);
                }
                $i++;
            }
          @mysql_freeresult($result);
       }
       return $i;
   }
   function my_reklamyaz($sorgu,$fdir)
   {
       $result = mysql_query($sorgu);
       if($result) {
          while (list($reklamno, $dosyaadi ) = mysql_fetch_row($result)) {
            /* read file and print here */
            $file = $fdir."/".$dosyaadi;
            if(file_exists($file)) {
                include($file);
                // $text=file_get_contents($file);
                // echo iconv("ISO-8859-9","UTF-8",$text);
                }
            }
          @mysql_freeresult($result);
       }
   }

  function my_request_add($usr,$kod,$uygkodu,$msg)
  {
     $istekno=mk_uniqueid();
     $ikod="1";
     $sorgu = "INSERT INTO istek (toNo,istekNo,fromNo,uygKodu,uygMsg,ikod) values('$kod','$istekno','$usr','$uygkodu','$msg','$ikod')";
     $result=mysql_query($sorgu);
  }

  function my_note_add($usr,$msg)
  {
     $logno = mk_uniqueid();
     $tar = date("Ymd");
     $saat = date("h:j:s");
     $sorgu = "INSERT INTO ulog (userNo, logNo, logTar, logSaat, logKod, LogMsg) values('$usr','$logno','$tar','$saat','1','$msg')";
     $result = mysql_query($sorgu);
  }

  function mk_uniqueid()
  {
     $prefix = 'W'; // a universal prefix prefix 
     $my_random_id = $prefix;
     $my_random_id .= chr(rand(65,90));
     $my_random_id .= time();
     $my_random_id .= uniqid($prefix);
     return $my_random_id;
  }
?>
