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

  function varmi($u, $ii)
  {
     $x = 0;
     $ss = "SELECT count(usrNo) from personel where usrNo = '$u' and bilgiKodu = '$ii'";
     list($x) = read_row($ss,$ii);
     return($x);
  }

  if(isset($_REQUEST['usrkod'])) {
      $usr=$_REQUEST['usrkod'];
      echo("$usr için<br />");
      $i=1;
      $r = "kod$i";
      /* usrNo ve bilgiKodu yoksa ekle */
      while(isset($_REQUEST[$r])) {
        if(varmi($usr,$i) > 0) {
          $sorgu = "UPDATE personel set ";
          $a = "kod$i";
          $$a = $_REQUEST[$a];
          if(!empty($$a)) {
              $b = "ad$i";
              $$b = $_REQUEST[$b];
              $sorgu .= "bilgiAlani='" . $$b ."' where usrNo='".$usr."' and ";
              $sorgu .= "bilgiKodu='" . $i . "';";
              $mystat = "";
              update_row($sorgu);
              if(!empty($mystat)) {
                  echo("HATA: " . $mystat . "<br />"); 
                  $hata = 1;
                  break;
              } else {
                  echo(" " . $i ." güncellendi<br />");
              }
          }
          $i++;
          $r = "kod$i";
        } else {
          $sorgu = "INSERT into personel (usrNo,bilgiKodu,bilgiAlani,bkod) ";
          $sorgu .= " values('$usr','$i',";
          $a = "kod$i";
          $$a = $_REQUEST[$a];
          if(!empty($$a)) {
              $b = "ad$i";
              $$b = $_REQUEST[$b];
              $xx = $$b;
              $sorgu .= "'$xx','1');";
              if(!empty($xx)) {
                  $mystat = insert_row($sorgu);
                  if(strstr($mystat,"eklendi")) {
                      echo(" " . $i ." eklendi<br />");
                  } else {
                      echo("HATA: " . $mystat . "<br />"); 
                      $hata = 1;
                      break;
                  }
              }
          }
          $i++;
          $r = "kod$i";
        }
      }
      if(!isset($hata)) {
         $uygmsg="$usr kişisel bilgilerini değiştirdi.";
         my_note_add($usr,$uygmsg);
      }
  } else {
      echo("hata var<br />");
  }
  echo("<form>");
  echo("  <input type=\"button\" onclick=\"javascript:parent.clear13('sec7')\" value=\"Kapat\">");
  echo("</form>");
?>
