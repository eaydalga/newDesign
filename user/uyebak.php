<?php
  require("include/conf.inc");
  if($_SERVER['HTTP_REFERER'] != $webhost.$userphp) {
       $aa = "Location: ".$webhost;
       header($aa);
       exit;
  }
  // db baglantısı gerekli
  $baglan = mysql_connect($dbhost, $dbuser, $dbpasswd)
            or die("Bağlantı kurulamadı" . mysql_error());
  mysql_select_db($dbname, $baglan);
  require("include/maint.inc");
  require("include/okuma.inc");

  function arkadasmi($uyeno,$fuye)
  {
      $sorgu="SELECT usrNo from friends where usrNo='$uyeno' and friendNo='$fuye'";
      $a = 0;
      $result=mysql_query($sorgu);
      if($result) {
           list($no) = mysql_fetch_row($result);
           if($no) $a = 1;
           }
      @mysql_freeresult($result);
      return($a);
  }

  function my_uyeler($sorgu,$val,$ff,$uyeno,$arkadas,$uad)
  {
      global $mystatus;
      $mystatus = "";
      $isay = 0;
      $result = mysql_query($sorgu);
      if($result) {
         echo("$val<br /><br />");
         echo("<div id=\"uyewin\">");
         echo "<table class=\"yazi\">\n";
         while ($line = mysql_fetch_array($result, MYSQL_ASSOC)) {
              $i=0;
              foreach ($line as $col_value) {
                  if($i != 0)
                      echo "\t\t<td>$col_value<br></td>\n";
                  else {
                      $a = arkadasmi($uyeno,$col_value);
/* echo($arkadas." ".$a . " ". $uyeno . " " . $col_value . "<br />"); */
                      if($arkadas == '0' && $a == '1') break;
                      if($arkadas == '1' && $a == '0') break;
                      $fresim=resimal($col_value);
                      $sz = getimagesize($fresim);
                      $w = 30 / $sz[1] * $sz[0];
                      echo "\t\t<tr><td valign=\"bottom\"><a href=\"javascript:uyeekle('$ff','$col_value','$uad');\" class=\"nav\"><img src=\"$fresim\" width=\"$w\" height=\"30\" border=\"0\"/><br />$col_value</a></td>\n";
                      }
                  $i++;
              }
              if($i) echo "\t</tr>\n";
              $isay++;
         }
         echo "</table>\n";
         @mysql_freeresult($result);
      } else {
         $mystatus = mysql_error();
      }
      return($isay);
  }
  $uyeno=""; $ukod=""; $ff="";
  // print_r($_REQUEST);
  if (isset($_REQUEST['s'])) {
     $ff=$_REQUEST['s'];
  }
  if (isset($_REQUEST['usrno'])) {
     $uyeno=$_REQUEST['usrno'];
  }
  if (isset($_REQUEST['kod'])) {
     $ukod=$_REQUEST['kod'];
     $uad='kod';
     $giden='sec3';
  } elseif (isset($_REQUEST['tono'])) {
     $ukod=$_REQUEST['tono'];
     $uad='tono';
     $giden='sec4';
  } elseif (isset($_REQUEST['tocc'])) {
     $ukod=$_REQUEST['tocc'];
     $uad='tocc';
     $giden='sec4';
  }
  if (isset($_REQUEST['arkadas'])) {
     $arkadas=$_REQUEST['arkadas'];
  } else {
     $arkadas = '';
  }
  if (isset($_REQUEST['basla'])) {
     $basla=$_REQUEST['basla'];
  } else {
     $basla = "0";
  }
  if (isset($_REQUEST['sayisi'])) {
     $sayisi=$_REQUEST['sayisi'];
  } else {
     $sayisi = 10;
  }
  if(($uyeno != "") && ($ff != "")) {
     $mystatus="";
     $sorgu="SELECT usrNo, fullName from user where usrNo != '$uyeno' and usrkod='1' order by usrNo limit $basla,$sayisi";
     // echo($basla . "<br />"); echo($sorgu);
     $say = my_uyeler($sorgu,"Uyelerin Listesi",$ff,$uyeno,$arkadas,$uad);
     if($say >= 10) {
         echo("<form name=\"form11\">");
         echo("<input type=\"hidden\" name=\"usrno\" value=\"$uyeno\">");
         echo("<input type=\"hidden\" name=\"$uad\" value=\"$ukod\">");
         echo("<input type=\"hidden\" name=\"arkadas\" value=\"$arkadas\">");
         echo("<input type=\"hidden\" name=\"basla\" value=\"$basla\">");
         echo("<input type=\"hidden\" name=\"sayisi\" value=\"$sayisi\">");
         if($giden == "kod")
              echo("<p><span class=\"rpic\"><input type=\"button\" onclick=\"javascript:uyeler('form11','$giden','$giden')\" class=\"nav\" value=\"DEVAM\"></span></form>");
         else echo("<p><span class=\"rpic\"><input type=\"button\" onclick=\"javascript:uyeler2('form11','$giden','$giden','$uad')\" class=\"nav\" value=\"DEVAM\"></span></form>");
     } else {
         echo("<p><form><span class=\"rpic\">Üyeler bitti...&nbsp;<input type=\"button\" onclick=\"javascript:clear13('$giden')\" class=\"nav\" value=\"KAPAT\"></span></form>");
     }
     echo "</div>\n";

     if($mystatus != "") {
         echo("<h3>UYE BILGISI HATASI</h3>");
         echo($mystatus);
     }
  } else {
     echo("olmadi... $uyeno -- $ukod");
  }
  @mysql_close($baglan);
?>
