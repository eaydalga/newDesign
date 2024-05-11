<?php
   require("../include/conf.inc");
   require("../include/muser.inc");
?>
<html>
<head>
<title>Ana Sayfa</title>
<link rel="stylesheet" type="text/css" href="../main.css" media="screen" />
<link rel="stylesheet" type="text/css" href="default.css" media="screen" />
<script language="JavaScript" src="../js/browser.js"> </script>
<script language="JavaScript" src="../js/email.js"> </script>
<script language="JavaScript" src="../js/date.js"> </script>
<script language="JavaScript" src="../js/float.js"> </script>
<script language="JavaScript" src="../js/baslik.js"> </script>
<script language="JavaScript" src="../js/XHConn.js"> </script>
<script>
var wsay;

function secim(m)
{
    if(document.getElementById) {
       clearbase();
       target=document.getElementById(m);
       if(target) {
             target.style.display = "inline";
       }
    }
}
function ikontrol(ff)
{
    var ik_errstr = "";
    var err;
    // tt = document.forms[ff];
    for(i=0; i < document.forms[ff].elements.length; i++) {
        var aa = document.forms[ff].elements[i].name;
        if ( aa != "undefined" ) {
            wff=document.forms[ff].elements[i].value;
            err = mycheck(wff,document.forms[ff].elements[i].className);
            if (err.length > 0) {
                ik_errstr += document.forms[ff].elements[i].name+ ":" + err + "\n";
            }
        }
    }
    if(ik_errstr.length > 0) {
        alert(ik_errstr);
        return false;
    }
    document.forms[ff].submit();
}

function gonder(ff)
{
    document.forms[ff].submit();
}

function yazdir(ff,m)
{
    // tt = document.forms[ff];
    if(tt) {
        wsay=document.forms[ff].elements['satirlar'].value;
        yazsat('satyaz');
    } else {
        wsay=0;
    }
return true;
}
function yazsat(mm)
{
    if(wsay) {
       el = document.getElementById(mm);
       el.innerHTML="";
       aa = "";
       for(ii=0;ii<wsay;ii++) {
           aa += "<table class=\"yyazi\">";
           aa += "<tr><td>Uygulama Bölüm No</td>";
           aa += "<td> <input type=\"text\" name=\"uygsecno"+ii+"\" class=\"required\" size=\"8\"> </td></tr>";
           aa += "<tr><td>Uygulama Sayısı</td>";
           aa += "<td> <input type=\"text\" name=\"uygsayi"+ii+"\" class=\"required\" size=\"6\"> </td></tr>";
           aa += "<tr><td>Uygulama Html</td>";
           aa += "<td> <input type=\"text\" name=\"uyghtml"+ii+"\" class=\"required\" size=\"20\"> </td></tr>";
           aa += "<tr><td>Uygulama Giriş Alanları</td>";
           aa += "<td> <input type=\"text\" name=\"uyginfield"+ii+"\" class=\"required\" size=\"20\"> </td></tr>";
           aa += "<tr><td>Uygulama Çıkış Alanları</td>";
           aa += "<td> <input type=\"text\" name=\"uygoutfield"+ii+"\" class=\"required\" size=\"20\"> </td></tr>";
           aa += "<tr><td></td>";
           aa += "<td> <table class=\"yyazi\">";
           aa += "<tr><td class=\"yyazi\">Ters Git</td> <td class=\"yyazi\">Devam Et</td> <td class=\"yyazi\">Iptal Et</td> <td class=\"yyazi\">Atla</td> </tr>";
           aa += "<tr><td><center><input type=\"checkbox\" name=\"uygters"+ii+"\" value=\"1\"></center> </td> <td><center> <input type=\"checkbox\" name=\"uygdevam"+ii+"\" value=\"1\"></center> </td>";
           aa += "<td><center> <input type=\"checkbox\" name=\"uygiptal"+ii+"\" value=\"1\"></center> </td> <td><center> <input type=\"checkbox\" name=\"uygatla"+ii+"\" value=\"1\"></center> </td>";
           aa += "</tr> </table> </td></tr> </tr> </table>";
       }
       el.innerHTML = aa;
    }
}

function dosyaekle(fname)
{
   if(fname.indexOf("reklam") > 0) {
     ffc = "form3";
   }
   if(fname.indexOf("haber") > 0) {
     ffc = "form2";
   }
   document.forms[ffc].elements['dosyaadi'].value = fname;
   clear13();
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
<span class="yyazi"><center><h1>Yönetim İşlemleri</h1></center></span>
<table class="yyazi">
<tr><td valign="top" width="200">
<div id="soltaraf">
    <div id="ymenublock">
         <font color="#8e9ccb"><h3>Bakım Programları</h3></font>
         <a href="maint.php" class="nav">Bakım</a>
         <font color="#8e9ccb"><h3>Giriş Programları</h3></font>
         <a href="javascript:secim('user1')" class="nav">Üye Giriş</a>
         <a href="javascript:secim('user2')" class="nav">Haber Giriş</a>
         <a href="javascript:secim('user3')" class="nav">Reklam Giriş</a>
         <a href="javascript:secim('user4')" class="nav">Uygulama Giriş</a>
         <font color="#8e9ccb"><h3>Onay Programları</h3></font>
         <a href="javascript:secim('user5')" class="nav">Haber Onayı</a>
         <a href="javascript:secim('user6')" class="nav">Reklam Onayı</a>
    </div>
</div>
</td><td valign="top">
<div id="ortataraf">
    <div id="user1" style="display: none;">
        <h3>Üye Girişi</h3>
        <fieldset>
        <form id="form1" action="uyegir.php" method="post">
        <table class="yyazi">
        <tr><td>userNo</td><td>
        <input type="text" name="userno1" class="required" size="17">
        </td></tr>
        <tr><td>email</td><td>
        <input type="text" name="email" class="validate-email" size="17">
        </td></tr>
        <tr><td>fullName</td><td>
        <input type="text" name="fullname" class="validate-alphanum" size="17">
        <input type="hidden" name="usrkod" value="1">
        </td></tr>
        <tr><td>Güvenlik Kodu</td><td>
        <img src="../cc.php">
        </td></tr>
        <tr><td>Güvenlik Kodu</td><td>
        <input type="text" name="security" class="required" size="10">
        <input type="button" value="git" class="gyazi" onclick="javascript:ikontrol('form1');">
        </td></tr>
        </table>
        </form>
        </fieldset>
    </div>
    <div id="user2" style="display: none;">
        <h3>Haber Girişi</h3>
        <fieldset>
        <form id="form2" action="habergir.php" method="post">
        <table class="yyazi">
        <tr><td>haberNo</td><td>
            <select name="haberno1">
              <option value="HOM">Ana</option>
              <option value="USR">Uye</option>
            </select>
            <input type="text" name="habeno2" class="validate-numeric" size="6">
            </td></tr>
        <tr><td>Baslangıç</td><td>
            <input type="text" name="baslangic" class="required" size="10">
            <input type="button" value="TarihAl" class="gyazi" onclick="javascript:tarihbak('form2','baslangic','user13','');">
            </td></tr>
        <tr><td>Bitiş</td><td>
            <input type="text" name="bitis" class="required" size="10">
            <input type="button" value="TarihAl" class="gyazi" onclick="javascript:tarihbak('form2','bitis','user13','');">
            </td></tr>
        <tr><td>DosyaAdi</td><td>
            <input type="text" name="dosyaadi" class="required" size="17">
            <input type="button" value="Gözat" class="gyazi" onclick="javascript:veribak('haberler','user13','user13');">
            </td></tr>
        <tr><td>hkod</td><td>
            <input type="hidden" name="hkod" value="0">0
            <input type="button" value="git" class="gyazi" onclick="javascript:ikontrol('form2');">
            </td></tr>
        </table>
        </form>
        </fieldset>
    </div>
    <div id="user3" style="display: none;">
        <h3>Reklam Girişi</h3>
        <fieldset>
        <form id="form3" action="reklamgir.php" method="post">
            <input type="hidden" name="rkod" value="0">
        <table class="yyazi">
        <tr><td>reklamNo</td><td>
            <select name="reklamno1">
              <option value="HOM">Ana</option>
              <option value="LFT">Sol</option>
              <option value="MID">Orta</option>
              <option value="USR">Uye</option>
            </select>
            <input type="text" name="reklamno2" class="validate-numeric" size="6">
            </td></tr>
        <tr><td>Baslangıç</td><td>
            <input type="text" name="baslangic" class="required" size="10">
            <input type="button" value="TarihAl" class="gyazi" onclick="javascript:tarihbak('form3','baslangic','user13','');">
            </td></tr>
        <tr><td>Bitiş</td><td>
            <input type="text" name="bitis" class="required" size="10">
            <input type="button" value="TarihAl" class="gyazi" onclick="javascript:tarihbak('form3','bitis','user13','');">
            </td></tr>
        <tr><td>DosyaAdi</td><td>
            <input type="text" name="dosyaadi" class="required" size="17">
            <input type="button" value="Gözat" class="gyazi" onclick="javascript:veribak('reklamlar','user13','user13');">
        <tr><td>rkod</td><td>
            0<input type="button" value="git" class="gyazi" onclick="javascript:ikontrol('form3');">
            </td></tr>
        </table>
        </form>
        </fieldset>
    </div>
    <div id="user4" style="display: none;">
        <h3>UYGULAMA GİRİŞİ</h3>
        <fieldset>
        <form id="form4" action="uygulamagir.php" method="post">
        <h3>Uygulama Başlığı</h3>
        <table class="yyazi">
        <tr><td>uygulama Kodu</td>
            <td>
            <input type="text" name="uygkodu" class="required" size="8">
            </td>
            <td>Uygulama Bolum No</td>
            <td>
            <input type="text" name="uygsecno" class="required" size="8">
            </td>
            <td>Uygulama Adi</td>
            <td>
            <input type="text" name="uygadi" class="required" size="25">
            </td>
        </tr>
        <tr>
            <td>Satır Sayısı</td>
            <td>
            <input type="text" name="satirlar" class="required" size="6">
            <input type="button" value="Aç" onclick="javascript:yazdir('form4');">
            </td>
        </tr>
        </table>
        <br /><br />
        <h3>Uygulama Satırları</h3>
        <div id="satyaz">
        </div>
        <span class="rpic">
        <input type="button" value="git" class="gyazi" onclick="javascript:ikontrol('form4');">
        </span>
        </form>
        </fieldset>
    </div>
    <div id="user5" style="display: none;">
        <h3>HABER ONAYI</h3>
<?php
        $baglan = mysql_connect($dbhost, $dbuser, $dbpasswd)
                  or die("Bağlantı kurulamadı" . mysql_error());
        mysql_select_db($dbname, $baglan);
        $sorgu = "SELECT * from haber where hkod like '0%'";
        $result = mysql_query($sorgu);


        echo "<span class=\"yyazi\"><h3>Haber listesi</h3></span>";
        if($result) {
            echo("<form name=\"form5\" method=\"post\" action=\"haberonay.php\">");
            echo "<table class=\"yyazi\" border=\"1\">\n";
            $i = 0;
            while ($line = mysql_fetch_array($result, MYSQL_NUM)) {
                $onaykod='kod' . $i;
                $onayno= 'no' . $i;
                echo "\t<tr>\n";
                echo("\t\t<td><input type=\"checkbox\" name=\"$onaykod\" value=\"1\">");
                echo("\t\t<input type=\"hidden\" name=\"$onayno\" value=\"$line[0]\"");
                echo("\t\t</td>");
                foreach ($line as $col_value) {
                    echo "\t\t<td>$col_value</td>\n";
                }
                echo "\t</tr>\n";
                $i++;
            }
            echo "</table>\n";
            if($i > 0) {
                echo("<span class=\"rpic\">");
                echo("<input type=\"button\" value=\"git\" class=\"gyazi\" onclick=\"gonder('form5');\">");
                echo("</span>");

                // Free resultset
                mysql_free_result($result);
            } else {
                echo("onaylanacak haber yok...<br />");
            }
            echo("</form>");
        }
        mysql_close($baglan);
?>
    </div>
    <div id="user6" style="display: none;">
        <h3>REKLAM ONAYI</h3>
<?php
        $baglan = mysql_connect($dbhost, $dbuser, $dbpasswd)
                  or die("Bağlantı kurulamadı" . mysql_error());
        mysql_select_db($dbname, $baglan);
        $sorgu = "SELECT * from reklam where rkod like '0%'";
        $result = mysql_query($sorgu);


        echo "<span class=\"yyazi\"><h3>Reklam listesi</h3></span>";
        if($result) {
            echo("<form name=\"form6\" method=\"post\" action=\"reklamonay.php\">");
            $i = 0;
            echo "<table class=\"yyazi\" border=\"1\">\n";
            while ($line = mysql_fetch_array($result, MYSQL_NUM)) {
                $onaykod='kod' . $i;
                $onayno= 'no' . $i;
                echo "\t<tr>\n";
                echo("\t\t<td><input type=\"checkbox\" name=\"$onaykod\" value=\"1\">");
                echo("\t\t<input type=\"hidden\" name=\"$onayno\" value=\"$line[0]\"");
                echo("\t\t</td>");
                foreach ($line as $col_value) {
                    echo "\t\t<td>$col_value</td>\n";
                }
                echo "\t</tr>\n";
                $i++;
            }
            echo "</table>\n";
            if($i > 0) {
                echo("<span class=\"rpic\">");
                echo("<input type=\"button\" value=\"git\" class=\"gyazi\" onclick=\"gonder('form6');\">");
                echo("</span>");
                // Free resultset
                mysql_free_result($result);
            } else {
                echo("onaylanacak reklam yok...<br />");
            }
            echo("</form>");

        }
        mysql_close($baglan);
?>
    </div>
</div>
</td><td valign="top">
<div id="ortataraf">
    <div id="user13" style="display: none;">
    </div>
</div>
</td></tr></table>
<script>secim('user1');</script>
</body>
</html>
