<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<?php
  session_start();
  require("include/conf.inc");
  if(isset($_SESSION['sifrehata'])) {
      if(empty($_SESSION['sifrehata'])) {
           $aa = "Location: ".$webhost.$userphp;
           header($aa);
           exit;
      }
  }
  // db baglantısı gerekli
  $baglan = mysql_connect($dbhost, $dbuser, $dbpasswd)
            or die("Bağlantı kurulamadı" . mysql_error());
  mysql_select_db($dbname, $baglan);
  require("include/maint.inc");
  require("include/okuma.inc");
?>
<html>
<head>
<title>Basvuru Sayfasi</title>
<link rel="stylesheet" type="text/css" href="main.css" media="screen" />
<script language="JavaScript" src="js/browser.js"> </script>
</head>
<body topmargin=0 leftmargin=0 marginheight=0 marginwidth=0>
<div id="headertop1">
   <div id="headertop">
<?php
    include("inc/baslik.inc");
?>
    <table cellpadding="0" cellspacing="0" border="0">
    <tr><td valign="top" width="140">
          <div id="soltaraf">
            <div id="masterdiv">
                <div class="menutitle"><a href="javascript:history.back()" class="vnav">Önceki Sayfa</a></div>
            </div>
          </div>

         </td><td valign="top" width="800">
              <div id="header">
                 <div id="baslik">
<table width="100%">
<tr><td valign="top">
<?php
    if (isset($_SESSION['security_code'])) {
       $ass=strtoupper($_SESSION['security_code']);
    } else {
       $ass="yok";
    }
    if (isset($_POST['security'])) {
       $pss=strtoupper($_POST['security']);
    } else {
       $pss="";
    }
    $uyeno=""; $sifre=""; $sifre2="";
    $eposta=""; $fullname="";
    if(!empty($_REQUEST['uyeno'])) {
        $uyeno=$_REQUEST['uyeno'];
    }
    if(!empty($_REQUEST['sifre'])) {
        $sifre=$_REQUEST['sifre'];
    }
    if(!empty($_REQUEST['sifre2'])) {
        $sifre2=$_REQUEST['sifre2'];
    }
    if(!empty($_REQUEST['eposta'])) {
        $eposta=$_REQUEST['eposta'];
    }
    if(!empty($_REQUEST['fullname'])) {
        $fullname=$_REQUEST['fullname'];
    }
    $_SESSION['basvuruhata']="güvenlik kodu hatalı";
    echo("<b>ÜYE BAŞVURUSU İşlem sonuçları</b><br />");
    if(($ass == $pss) && (!empty($_SESSION['security_code'])) ) {
      $sorgu = "SELECT usrNo from user where usrNo = '$uyeno'";
      $bsonuc = mysql_query($sorgu);
      if($bsonuc) {
        $_SESSION['basvuruhata']="üye kodu daha önce var";
      } else {
        if(strlen($sifre) < 4) $sifre="";
        if(strlen($sifre2) < 4) $sifre="";
        if(($sifre == $sifre2) && (strlen($sifre) >= 4)) {
            $ss = crypt($sifre);
            $ukod="0".rand(0,999999);
            $sorgu="INSERT into user (usrNo,email,passwd,fullName,usrkod) values('$uyeno','$eposta','$ss','$fullname','$ukod')";
            $bhata = insert_row($sorgu);
            if(strstr($bhata,"eklendi")) {
                 /* mail gonder */
                 $message="";
                 $omessage="";
                 if(!empty($_REQUEST['fullname'])) {
                     $message .= "Ad Unvan= ".$fullname."\n";
                     $omessage .= "Ad Unvan= ".$fullname."<br />\n";
                 }
                 if(!empty($_REQUEST['eposta'])) {
                     $message .= "E-posta= ".$eposta."\n";
                     $omessage .= "E-posta= ".$eposta."<br />\n";
                 }
                 if(strlen($message) > 10) {
                     echo($omessage);
                     $konu="Basvuru Giris Onayi";
                     $baslik="X-Mailer: PHP". phpversion();
                     $message .= "Yukarıdaki ad ve eposta adresiyle yaptığınız başvuruyu onaylamak için\n";
                     $omessage = "<br />Yukarıdaki ad ve eposta adresiyle yaptığınız başvuruyu onaylamak için<br />\n";
                     $message .= "<a href=\"$webhost/onay.php?uye=$uyeno&ukod=$ukod\">Burayı</a> tıklayınız\n";
                     $omessage .= "<a href=\"$webhost/onay.php?uye=$uyeno&ukod=$ukod\">Burayı</a> tıklayınız<br />\n";
                     $message .= "Doğrudan bağlantı kurmak için\n$webhost/onay.php web safasindan\n$uyeno uye numaranizi ve $ukod değerini girip onay işlemini tamamlayın\n";
                     $omessage .= "Doğrudan bağlantı kurmak için<br />\n$webhost/onay.php web safasindan<br />\n$uyeno uye numaranizi ve $ukod değerini girip onay işlemini tamamlayın<br />\n";
                     $message = wordwrap($message);
                     echo($omessage);
                     // echo("<hr>$message<br>");
                     // mail($eposta,$konu,$message,$baslik);
                     // onay icin mesaj ePosta adresine gonderildi
                     echo("<br />onay mesajı ePosta adresinize gönderildi<br />");
                     $_SESSION['basvuruhata']="";
                 } else {
                     echo("message length:" . strlen($message));
                     $_SESSION['basvuruhata']="mesaj boş";
                 }
            } else {
                 $_SESSION['basvuruhata']="VT hatası: $bhata";
            }
        } else {
            $_SESSION['basvuruhata']="şifre ve tekrarı uyumsuz";
        }
      }
    }
?>
                     <!-- reklamlar varsa buraya alinir -->
<?php
                         /* rekramlara bak
                          * bugun >= basTar and bugun <= bitTar ise reklami yazdir
                          * ANA = ana sayfa reklamı
                          */
                         $bugun=date("Ymd", time());
                         $sorgu="SELECT reklamNo,dosyaAdi from reklam where reklamNo like '%ANA%' and basTar >= $bugun and $bugun <= bitTar";
                         my_reklamyaz($sorgu,$reklamdir);
?>
</td><td valign="top" width="280">
                     <!-- ---- sag taraf ------------- -->
                     <div class="borderright">
                     <!-- basvuru giris ekrani ---- -->
                     <div class="yazi">
<?php
                        if(isset($_SESSION['basvuruhata']) && $_SESSION['basvuruhata'] != "") {
                            echo("<h3>ÜYE BAŞVURUSU</h3>");
                            echo("<form method=\"post\" action=\"basvuru.php\">");
                               echo("<table class=\"yazi\">");
                                  echo("<tr><td>Üye Numarası </td>");
                                      echo("<td> <input type=\"text\" size=\"17\" name=\"uyeno\" value=\"$uyeno\">");
                                  echo("</td></tr>");
                                  echo("<tr><td>Şifre </td>");
                                      echo("<td> <input type=\"password\" size=\"10\" name=\"sifre\" value=\"$sifre\">");
                                  echo("</td></tr>");
                                  echo("<tr><td>Şifre Tekrarı </td>");
                                      echo("<td> <input type=\"password\" size=\"10\" name=\"sifre2\" value=\"$sifre2\">");
                                  echo("</td></tr>");
                                  echo("<tr><td>e-Posta Adresi </td>");
                                      echo("<td> <input type=\"text\" size=\"17\" name=\"eposta\" value=\"$eposta\">");
                                  echo("</td></tr>");
                                  echo("<tr><td>Adı Soyadı </td>");
                                      echo("<td> <input type=\"text\" size=\"17\" name=\"fullname\" value=\"$fullname\">");
                                  echo("</td></tr>");
                                  echo("<tr><td>Güvenlik Kodu</td><td><img src=\"../../cc.php\"></td></tr>");
                                  echo("<tr><td>Güvenlik Kodu</td>");
                                      echo("<td> <input type=\"text\" size=\"10\" name=\"security\">");
                                           echo("<input type=\"submit\" value=\"Git\" class=\"gyazi\">");
                                  echo("</td></tr>");
                               echo("</table>");
                            echo("</form>");
                            if(isset($_SESSION['basvuruhata'])) {
                               // basvuru icin hata mesaji
                               echo("<b>HATA:</b> ".$_SESSION['basvuruhata']);
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
</td></tr></table>
                     </div>
                     </div>
<!-- ------------sag taraf bitti ---------------- -->
                </div>
            </div>
    </div>
</div>
</td></tr>
</table>
</body>
</html>
