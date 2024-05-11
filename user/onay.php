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
?>
<html>
<head>
<title>Onay Sayfasi</title>
<link rel="stylesheet" type="text/css" href="main.css" media="screen" />
<script language="JavaScript" src="js/browser.js"> </script>
</head>
<body>
<table cellpadding="0" cellspacing="0" border="1" class="yyazi">
<!-- img src="img/ribi.jpg" -->
<tr><td valign="top">
<div id="ysoltaraf">
</div>
</td><td valign="top">
<div id="ortataraf">
<?php
    $uyeno=""; $ukod="";
    if (isset($_REQUEST['uyeno'])) {
       $uyeno=strtoupper($_REQUEST['uyeno']);
    }
    if (isset($_POST['ukod'])) {
       $ukod=$_POST['ukod'];
    }
    if(($uyeno != "") && ($ukod != "")) {
       $mystatus="";
       $sorgu="SELECT fullName from user where usrNo='$uyeno' and usrkod='0$ukod'";
       $adlar = read_row($sorgu, $uyeno);

       if($mystatus == "") {
           $sorgu="UPDATE user set usrkod='1' where usrNo='$uyeno'";
           update_row($sorgu, $uyeno);
           echo("<h3>UYE ve EPOSTA ONAY ISLEMI</h3>");
           echo("<p>");
           echo("<p>");
           echo("<p>$adlar[0] kullanıcısı bilgileri onaylandı...");
               echo("sistemi kullanabilirsiniz...");
       } else {
           echo("<h3>UYE ve EPOSTA ONAY HATASI</h3>");
           echo($mystatus);
       }
       @mysql_close($baglan);
?>
       <span class="rpic">
       <form>
       <input type="button" value="Yeni Onay" class="gyazi" onclick="location.href='onay.php';">
       </form>
       </span>
<?php
   } else {
?>
       <center>
       <h3>UYE ve EPOSTA ONAY ISLEMI</h3>
       <h3>BİLGİ GİRİŞİ</h3>
       </center>
       <form method="post" action="onay.php">
       <table class="yyazi">
       <tr><td>Uye No</td>
           <td><input type="text" name="uyeno" size="10"></td></tr>
       <tr><td>Uye Kodu</td>
           <td><input type="text" name="ukod" size="10">
               <input type="submit" value="Git" class="gyazi">
       </td></tr>
       </table>
       </form>
<?php
   }
?>
</div>
</td><td>
<div id="ortataraf">
</div>
</td></tr>
</table>
</body>
</html>
