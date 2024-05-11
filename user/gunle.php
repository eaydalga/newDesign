<?php
  require("include/conf.inc");
   // db baglantısı gerekli
   $baglan = mysql_connect($dbhost, $dbuser, $dbpasswd)
             or die("Bağlantı kurulamadı" . mysql_error());
   mysql_select_db($dbname, $baglan);
  require("include/maint.inc");
  require("include/okuma.inc");
?>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
</head>
<body>
<?php
   if (isset($_REQUEST['kullanici'])) {
       $usrno=$_REQUEST['kullanici'];
   } else {
       $usrno="";
   }
   if (isset($_REQUEST['islem'])) {
       $islem=$_REQUEST['islem'];
   } else {
       $islem="";
   }
   if($islem == "em") {
       if (isset($_REQUEST['email'])) {
           $email=$_REQUEST['email'];
       } else {
           $email="";
       }
       $win="sec9";
   }
   if($islem == "ad") {
       if (isset($_REQUEST['fullname'])) {
           $fullname=$_REQUEST['fullname'];
       } else {
           $fullname="";
       }
       $win="sec10";
   }
   if($islem == "sf") {
       if (isset($_REQUEST['sifre'])) {
           $sifre=$_REQUEST['sifre'];
       } else {
           $sifre="";
       }
       if (isset($_REQUEST['sifre2'])) {
           $sifre2=$_REQUEST['sifre2'];
       } else {
           $sifre2="";
       }
       $win="sec8";
   }
   if(!empty($usrno)) {
       if (!empty($islem)) {
           $sorgu = "UPDATE user set ";
           if(!empty($fullname)) {
               $sorgu .= " fullname='$fullname'";
               $uygmsg="$usrno için $fullname degisti...";
           }
           if(!empty($email)) {
               $sorgu .= " email='$email'";
               $uygmsg="$usrno için $email degisti...";
           }
           if(!empty($sifre)) {
               if($sifre == $sifre2) {
                   $a = crypt($sifre,"aA");
                   $sorgu .= "passwd ='$a'";
                   $uygmsg="$usrno için sifre degisti...";
               }
           }
           $sorgu .= " where usrNo='$usrno'";
           // $kod icin istek gonder
           my_note_add($usrno,$uygmsg);
           $sonuc = update_row($sorgu);
           if(strstr($sonuc,"güncel")) {
?>
<br>
<br>
<form>
   <span class="yyazi">
     <label for="submit_" class="yyazi">Güncelleme tamamlandı...</label>
<?php
     echo($win);
     echo("<input type=\"button\" class=\"gyazi\" value=\"Kapat\" onclick=\"parent.clear13('$win');\"/>");
?>
   </span>
</form>
<?php
           } else {
               echo("<h3>VT HATASI</h3>");
               echo($sonuc);
           }
       }
     } else {
        if($mystatus != "") {
            echo("<h3>UYE BILGISI HATASI</h3>");
            echo($mystatus);
        }
     }
?>
</body>
</html>
