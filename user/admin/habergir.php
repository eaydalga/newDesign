<?php
  session_start();
  require("../include/conf.inc");
   // db baglantısı gerekli
   $baglan = mysql_connect($dbhost, $dbuser, $dbpasswd)
             or die("Bağlantı kurulamadı" . mysql_error());
   mysql_select_db($dbname, $baglan);
  require("../include/maint.inc");
  require("../include/okuma.inc");
?>
<html>
<title>Haber Giris</title>
<head>
<link rel="stylesheet" type="text/css" href="../main.css" media="screen" />
<link rel="stylesheet" type="text/css" href="default.css" media="screen" />
<script language="JavaScript" src="../js/browser.js"> </script>
<script language="JavaScript" src="../js/email.js"> </script>
<script language="JavaScript" src="../js/date.js"> </script>
<script language="JavaScript" src="../js/float.js"> </script>
<script language="JavaScript" src="../js/baslik.js"> </script>
<script language="JavaScript" src="../js/XHConn.js"> </script>

<script>

function dosyaekle(fname)
{
   ffc = "form2";
   document.forms[ffc].elements['dosyaadi'].value = fname;
   clearall();
}

function veribak(s,t,w)
{
var myConn2 = new XHConn();
var body="s="+s+"&";

var fnBitti = function (oXML) {
   if(document.getElementById) {
      target1=document.getElementById(t);
      if(target1) {
            target1.style.display = "none";
      }
      target=document.getElementById(w);
      if(target) {
            target.innerHTML=oXML.responseText;
            target.style.display = "inline";
      }
   }
 };
myConn2.connect("reklamhaber.php", "POST", body, fnBitti);
}

function tarihbak(f,n,w,dts)
{
var myConn2 = new XHConn();
var body="f="+f+"&n="+n+"&ww="+w+"&dts="+dts+"&";

var fnBitti = function (oXML) {
   if(document.getElementById) {
      target1=document.getElementById(w);
      if(target1) {
            target1.style.display = "none";
      }
      target=document.getElementById(w);
      if(target) {
            target.innerHTML=oXML.responseText;
            target.style.display = "inline";
      }
   }
 };
myConn2.connect("calendar.php", "POST", body, fnBitti);
}

function tarihal(fform,fname,ymd)
{
   document.forms[fform].elements[fname].value = ymd;
   clear13();
}
</script>
</head>
<body>
<table cellpadding="0" cellspacing="0" border="1" class="yyazi">
<!-- img src="img/ribi.jpg" -->
<tr><td valign="top">
<div id="ysoltaraf">
</div>
</td><td valign="top" width="500">
<div id="ortataraf">
<?php
    $haberno1=""; $haberno2=""; $dosyaadi="";
    $baslangic=""; $bitis="";
    if(!empty($_REQUEST['haberno1'])) {
        $haberno1=$_REQUEST['haberno1'];
    }
    if(!empty($_REQUEST['haberno2'])) {
        $haberno2=$_REQUEST['haberno2'];
    }
    if(!empty($_REQUEST['baslangic'])) {
        $baslangic=$_REQUEST['baslangic'];
    }
    if(!empty($_REQUEST['bitis'])) {
        $bitis=$_REQUEST['bitis'];
    }
    if(!empty($_REQUEST['dosyaadi'])) {
        $dosyaadi=$_REQUEST['dosyaadi'];
    }
    if(isset($_REQUEST['hkod'])) {
        $hkod=$_REQUEST['hkod'];
    } else {
        @mysql_close($baglan);
        exit;
    }
    $_SESSION['haberhata']="haber no hatalı";
    echo("<b>HABER GİRİŞİ</b><br />");
    echo("<b>İşlem sonucu</b><br /><br />");
    $haber=$haberno1.$haberno2;
    $sorgu="INSERT into haber (haberNo,baslangic,bitis,dosyaAdi,hkod) values('$haber','$baslangic','$bitis','$dosyaadi','$hkod')";
    $bhata = insert_row($sorgu);
    if(strstr($bhata,"eklendi")) {
        $omessage = "<br />haber tablosuna eklendi<br />\n";
        $omessage .= "haber no: $haber<br />\n";
        $omessage .= "basla bitis: $baslangic - $bitis<br />\n";
        $omessage .= "dosya adi: $dosyaadi<br /><br />\n";
        echo($omessage);
        $_SESSION['haberhata']="";
    } else {
        $_SESSION['haberhata']="VT hatası: $bhata";
        echo("<fieldset>");
        echo("<h3>HABER GİRİŞİ</h3>");
        echo("<form method=\"post\" name=\"form2\" action=\"habergir.php\">");
        echo("<table class=\"yyazi\">");
        echo("<tr><td>haberNo</td><td>");
        echo("    <select name=\"haberno1\">");
        if($haberno1 == "HOM") {
            echo("      <option value=\"HOM\" selected>Ana</option>");
            echo("      <option value=\"USR\">Uye</option>");
        } else {
            echo("      <option value=\"USR\" selected>Uye</option>");
            echo("      <option value=\"HOM\">Ana</option>");
        }
        echo("    </select>");
        echo("    <input type=\"text\" name=\"habeno2\" class=\"validate-numeric\" size=\"6\" value=\"$haberno2\">");
        echo("    </td></tr>");
        echo("<tr><td>Baslangıç</td><td>");
        echo("    <input type=\"text\" name=\"baslangic\" class=\"required\" size=\"10\" value=\"$baslangic\">");
        echo("<input type=\"button\" value=\"TarihAl\" class=\"gyazi\" onclick=\"javascript:tarihbak('form2','baslangic','user13','');\">");
        echo("    </td></tr>");
        echo("<tr><td>Bitiş</td><td>");
        echo("    <input type=\"text\" name=\"bitis\" class=\"required\" size=\"10\" value=\"$bitis\">");
        echo("<input type=\"button\" value=\"TarihAl\" class=\"gyazi\" onclick=\"javascript:tarihbak('form2','bitis','user13','');\">");
        echo("    </td></tr>");
        echo("<tr><td>DosyaAdi</td><td>");
        echo("    <input type=\"text\" name=\"dosyaadi\" class=\"required\" size=\"17\" value=\"$dosyaadi\">");
        echo("<input type=\"button\" value=\"Gözat\" class=\"gyazi\" onclick=\"javascript:veribak('haberler','user13','user13');\">");
        echo("    </td></tr>");
        echo("<tr><td>hkod</td><td>");
        echo("    <input type=\"hidden\" name=\"hkod\" value=\"0\">0");
        echo("    <input type=\"button\" value=\"git\" class=\"gyazi\" onclick=\"javascript:ikontrol('form2');\">");
        echo("    </td></tr>");
        echo("</table>");
        echo("</form>");

        echo($_SESSION['haberhata']);
        echo("</fieldset>");
    }
?>
</div>
</td><td valign="bottom">
<div id="ortataraf">
    <div id="user13" style="display: none">
    </div>
<?php
   if(!(isset($_SESSION['haberhata']) && $_SESSION['haberhata'] != "")) {
       session_unset();
   }
?>
     <form>
     <input type="button" value="Ana Sayfaya" class="gyazi" onclick="location.href='index.php';">
     </form>
<?php
   @mysql_close($baglan);
?>
</div>
</td></tr>
</table>
</body>
</html>
