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
   if (isset($_REQUEST['usrno'])) {
       $usrno=$_REQUEST['usrno'];
   } else {
       $usrno="";
   }
   $win="sec3";
   $mystatus = "girişte hata var";
   if(!empty($usrno)) {
       if (isset($_REQUEST['kod'])) {
           $kod=$_REQUEST['kod'];
           $win=$_REQUEST['win'];
           $uygkodu=$yakin_arkadas;
           // $kod icin istek gonder
           $uygmsg="Sayin $kod $usrno sizinle YAKIN ARKADAS olmak istedi.";
           my_request_add($usrno,$kod,$uygkodu,$uygmsg);
           my_note_add($kod,$uygmsg);
           $uygmsg="Sayin $kod ile YAKIN ARKADAS olmak istediniz.";
           my_note_add($usrno,$uygmsg);
           // $kod icin email adresi al
           $sor = "SELECT fullname from user where usrNo = '$usrno'";
           $mystatus="";
           list($fullname) = read_row($sor,$usrno);
           $sor = "SELECT email,fullname from user where usrNo = '$kod'";
           $mystatus="";
           list($email,$fname) = read_row($sor,$kod);
           if($mystatus != "") $email = "";
       } else {
           $email = "";
       }
       if (isset($_REQUEST['email'])) {
           $email=$_REQUEST['email'];
       }
       if($email != "") {
           // Insert you code for processing the form here
           $message="Sayin $fname, $fullname sizi YAKIN ARKADAS olarak $site_adina davet ediyor.";
           if(strlen($message) > 10) {
               $konu="Yakin Arkadas";
               $baslik="X-Mailer: PHP". phpversion();
               $message = wordwrap($message);
               my_note_add($usrno,$message);
               // mail($email,$konu,$message,$baslik);
           } else {
              echo("Mesaj boyu hatali:" . strlen($message));
           }
?>
<br>
<br>
<form>
   <span class="yyazi">
     <label for="submit_" class="yyazi">Isteginiz iletildi...</label>
<?php
     echo("<input type=\"button\" class=\"gyazi\" value=\"Kapat\" onclick=\"clear13('$win');\"/>");
?>
   </span>
</form>
<?php
       } else {
           if($mystatus != "") {
               echo("<h3>UYE BILGISI HATASI</h3>");
               echo($mystatus);
           }
       }
   } else {
      /* Insert your code for showing an error message here */
      echo("yok...");
      exit;
   }
?>
</body>
</html>
