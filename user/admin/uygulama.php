<?php
  session_start();
  require("include/conf.inc");
   // db baglantısı gerekli
   $baglan = mysql_connect($dbhost, $dbuser, $dbpasswd)
             or die("Bağlantı kurulamadı" . mysql_error());
   mysql_select_db($dbname, $baglan);
  require("include/maint.inc");
  require("include/okuma.inc");

function uyg_satoku($i)
{
    $a = 'uygsecno'.$i;
    if(!empty($_REQUEST[$a])) {
        $$a=$_REQUEST[$a];
    }
    $a = 'uygsayi'.$i;
    if(!empty($_REQUEST[$a])) {
        $$a=$_REQUEST[$a];
    }
    $a = 'uyghtml'.$i;
    if(!empty($_REQUEST[$a])) {
        $$a=$_REQUEST[$a];
    }
    $a = 'uyginfield'.$i;
    if(!empty($_REQUEST[$a])) {
        $$a=$_REQUEST[$a];
    }
    $a = 'uygoutfield'.$i;
    if(!empty($_REQUEST[$a])) {
        $$a=$_REQUEST[$a];
    }
    $a = 'uygters'.$i;
    $$a = "";
    if(!empty($_REQUEST[$a])) {
        $$a=$_REQUEST[$a];
    }
    $a = 'uygdevam'.$i;
    $$a = "";
    if(!empty($_REQUEST[$a])) {
        $$a=$_REQUEST[$a];
    }
    $a = 'uygiptal'.$i;
    $$a = "";
    if(!empty($_REQUEST[$a])) {
        $$a=$_REQUEST[$a];
    }
    $a = 'uygatla'.$i;
    $$a = "";
    if(!empty($_REQUEST[$a])) {
        $$a=$_REQUEST[$a];
    }
}

function uyg_satyaz($ii)
{
   echo("<tr><td>Uygulama Bölüm No</td>");
   $a = "uygsecno" . $ii;
   echo("<td> <input type=\"text\" name=\"$a\" class=\"required\" size=\"8\" value=\"$$a\"> </td></tr>");
   echo("<tr><td>Uygulama Sayısı</td>");
   $a = "uygsayi" . $ii;
   echo("<td> <input type=\"text\" name=\"$a\" class=\"required\" size=\"6\" value=\"$$a\"> </td></tr>");
   echo("<tr><td>Uygulama Html</td>");
   $a = "uyghtml" . $ii;
   echo("<td> <input type=\"text\" name=\"$a\" class=\"required\" size=\"20\" value=\"$$a\"> </td></tr>");
   echo("<tr><td>Uygulama Giriş Alanları</td>");
   $a = "uyginfield" . $ii;
   echo("<td> <input type=\"text\" name=\"$a\" class=\"required\" size=\"20\" value=\"$$a\"> </td></tr>");
   echo("<tr><td>Uygulama Çıkış Alanları</td>");
   $a = "uygoutfield" . $ii;
   echo("<td> <input type=\"text\" name=\"$a\" class=\"required\" size=\"20\" value=\"$$a\"> </td></tr>");
   echo("<tr><td></td>");
   echo("<td> <table class=\"yyazi\">");
   echo("<tr><td class=\"yyazi\">Ters Git</td> <td class=\"yyazi\">Devam Et</td> <td class=\"yyazi\">Iptal Et</td> <td class=\"yyazi\">Atla</td> </tr>");
   $a = "uygters" . $ii;
   if($$a == 1)
   echo("<tr><td><center><input type=\"checkbox\" name=\"uygters"+ii+"\" value=\"1\" checked></center>");
   else
   echo("<tr><td><center><input type=\"checkbox\" name=\"uygters"+ii+"\" value=\"1\"></center>");
   $a = "uygdevam" . $ii;
   if($$a == 1)
   echo("</td> <td><center> <input type=\"checkbox\" name=\"uygdevam"+ii+"\" value=\"1\"></center> </td>");
   else
   echo("</td> <td><center> <input type=\"checkbox\" name=\"uygdevam"+ii+"\" value=\"1\" checked></center> </td>");
   $a = "uygdevam" . $ii;
   if($$a == 1)
   echo("<td><center> <input type=\"checkbox\" name=\"uygiptal"+ii+"\" value=\"1\" checked></center>");
   else
   echo("<td><center> <input type=\"checkbox\" name=\"uygiptal"+ii+"\" value=\"1\"></center>");
   $a = "uygdevam" . $ii;
   if($$a == 1)
   echo("</td> <td><center> <input type=\"checkbox\" name=\"uygatla"+ii+"\" value=\"1\" checked></center> </td>");
   else
   echo("</td> <td><center> <input type=\"checkbox\" name=\"uygatla"+ii+"\" value=\"1\"></center> </td>");
}
?>
<html>
<title>uygulama girisi</title>
<head>
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
    $uygkodu=""; $uygsecnob=""; $uygadi="";
    $satirlar="";
    if(!empty($_REQUEST['uygkodu'])) {
        $uygkodu=$_REQUEST['uygkodu'];
    }
    if(!empty($_REQUEST['uygsecnob'])) {
        $uygsecnob=$_REQUEST['uygsecnob'];
    }
    if(!empty($_REQUEST['uygadi'])) {
        $uygadi=$_REQUEST['uygadi'];
    }
    if(!empty($_REQUEST['satirlar'])) {
        $satirlar=$_REQUEST['satirlar'];
    }
    for($i=0;$i<$satirlar;$i++) {
        uyg_satoku($i);
    }
    $_SESSION['uygulamahata']="uygulama no hatalı";
    echo("<b>UYGULAMA GİRİŞİ</b><br />");
    echo("<b>İşlem sonucu</b><br />");
    $haber=$haberno1.$haberno2;
    $sorgu="INSERT into uygbaslik (uygKodu,uygSecNo,uygAdi) values('$uygkodu','$uygsecnob','$uygadi')";
    $bhata = insert_row($sorgu);
    if(strstr($bhata,"eklendi")) {
        for($ii=0;$ii<$sayilar;$ii++) {
            $a = 'uygsecno' . $ii;
            $b = 'uygsayi' . $ii;
            $c = 'uyghtml' . $ii;
            $d = 'uyginfield' . $ii;
            $e = 'uygoutfield' . $ii;
            $f = 'uygters' . $ii;
            $g = 'uygdevam' . $ii;
            $h = 'uygiptal' . $ii;
            $hi = 'uygatla' . $ii;
            $sorgu="INSERT into uygsatir (uygSecNo,uygSayi,uygHtml,uygInField,uygOutField,uygTers,uygDevam,uygIptal,uygAtla) values('$$a','$$b','$$c','$$d','$$e','$$f','$$g','$$h','$$hi')";
            $chata[$ii] = insert_row($sorgu);
        }
        $omessage = "<br />Yukarıdaki ad ve eposta adresiyle yaptığınız başvuruyu onaylamak için<br />\n";
        $omessage .= "<a href=\"$webhost/onay.php?uye=$uyeno&ukod=$ukod\">Burayı</a> tıklayınız<br />\n";
        $omessage .= "Doğrudan bağlantı kurmak için<br />\n$webhost/onay.php web safasindan<br />\n$uyeno uye numaranizi ve $ukod değerini girip onay işlemini tamamlayın<br />\n";
        echo($omessage);
        $_SESSION['uygulamahata']="";
    } else {
        $_SESSION['uygulamahata']="VT hatası: $bhata";
    }
?>
</div>
</td><td valign="bottom">
<div id="ortataraf">
    <fieldset>
    <div class="yyazi">
<?php
   if(isset($_SESSION['uygulamahata']) && $_SESSION['uygulamahata'] != "") {
       echo("<h3>UYGLAMA GİRİŞİ</h3>");
       echo("<form method=\"post\" action=\"uygulamagir.php\">");
       echo("<table class=\"yyazi\">");
       echo("<tr><td>uygulama Kodu</td>");
       echo("    <td>");
       echo("    <input type=\"text\" name=\"uygkodu\" class=\"required\" size=\"8\" value=\"$uygkodu\">");
       echo("    </td>");
       echo("    <td>Uygulama Bolum No</td>");
       echo("    <td>");
       echo("    <input type=\"text\" name=\"uygsecno\" class=\"required\" size=\"8\" value=\"$uygsecnob\">");
       echo("    </td>");
       echo("    <td>Uygulama Adi</td>");
       echo("    <td>");
       echo("    <input type=\"text\" name=\"uygadi\" class=\"required\" size=\"25\" value=\"$uygadi\">");
       echo("    </td>");
       echo("</tr>");
       echo("<tr>");
       echo("    <td>Satır Sayısı</td>");
       echo("    <td>");
       echo("    $sayirlar ");
       echo("    </td>");
       echo("</tr>");
       echo("</table>");

       echo("<table class=\"yyazi\">");
       for($i=0;$i<$sayilar;$i++) {
           uyg_satyaz($i);
       }
       echo("</tr> </table> </td></tr> </tr> </table>");
       echo("</form>");

       if(isset($_SESSION['uygulamahata'])) {
          // uygulamahata icin hata mesaji
          echo($_SESSION['uygulamahata']);
          session_unset();
       }
   } else {
       session_unset();
?>
       <form>
         <input type="button" value="Ana Sayfaya" class="gyazi" onclick="location.href='index.php';">
       </form>
<?php
   }
   @mysql_close($baglan);
?>
    </div>
    </fieldset>
</div>
</td></tr>
</table>
</body>
</html>
