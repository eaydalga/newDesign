<?php
  session_start();
  require("include/conf.inc");
  require("include/okuma.inc");
  if(isset($_SESSION['sifrehata'])) {
      if(empty($_SESSION['sifrehata'])) {
           $aa = "Location: ".$webhost.$userphp;
           header($aa);
           exit;
      }
  }
  require("include/dbconnect.inc");
  @mysql_select_db($dbname, $baglan);
?>
<html>
<head>
<title>Ana Sayfa</title>
<link rel="stylesheet" type="text/css" href="main.css" media="screen" />
<link rel="stylesheet" type="text/css" href="default.css" media="screen" />
<script language="JavaScript" src="js/browser.js"> </script>
<script language="JavaScript" src="js/sifreli.js"> </script>
</head>
<body>
<div id="usttaraf">
</div>
<table cellpadding="0" cellspacing="0" class="yyazi" width=100%">
<tr><td valign="top">
<div id="soltaraf">
    <form action="sifreli.php" method="post">
    <fieldset class="bdy">
    <h3> ÜYE GİRİŞİ</h3>
    <table cellpadding="1" cellspacing="3" class="yyazi">
    <tr><td>Üye Kodu </td><td width="70%"><input type="text" size="10" name="kullanici"></td></tr>
    <tr><td>Şifre </td><td width="70%"><nobr><input type="password" name="sifre" size="10">
    <input type="submit" value="Git" class="gyazi"></nobr>
    </td></tr>
    </table>
    <div class="yyazi">
<?php
  if(isset($_SESSION['sifrehata'])) {
     // sifre icin hata mesaji
     echo($_SESSION['sifrehata']);
     session_unset();
  }
?>
    </div>
    </fieldset>
    </form>
<?php
    $bugun=date("Ymd", time());
    $sorgu="SELECT reklamNo,dosyaAdi from reklam where reklamNo like 'LFT%' and bitTar >= $bugun and rkod='1'";
    my_reklamyaz($sorgu,$reklamdir);
?>
</div>
</td><td valign="top">
<div id="yortataraf">
    <div id="header">
    <div id="acikmavi">
<!-- banner olmali bu alanda -->
<?php
    /* haberlere bak
     * bugun >= baslangic and bugun <= bitis ise haberi yazdir
     * GNC = guncel haberler
     */
    /* 440 x 230 ile resim goruntuleme ve haberleri olusturma */
    $bugun=date("Ymd", time());
    $sorgu="SELECT haberno,dosyaAdi from haber where haberNo like 'HOM%' and baslangic <= '$bugun' and hkod = '1' order by baslangic desc";
    $say = my_haberyaz($sorgu,$haberdir);
    /* yazilan birseyler yoksa standart ana sayfa yazisini goruntule */
    if($say < 1) {
        include "haberler/$standart_haber";
        $bugun=date("Ymd", time());
        $sorgu="SELECT reklamNo,dosyaAdi from reklam where reklamNo like 'MID%' and bitTar >= $bugun and rkod='1'";
        my_reklamyaz($sorgu,$reklamdir);
    }
    echo("<script>timedMsg()</script>");
?>
    </div>
    </div>
</div>
</td><td valign="bottom">
<br />
<br />
<!-- reklamlar varsa buraya alinir -->
<?php
    /* rekramlara bak
     * bugun >= basTar and bugun <= bitTar ise reklami yazdir
     * ANA = ana sayfa reklamı
     */
    $bugun=date("Ymd", time());
    $sorgu="SELECT reklamNo,dosyaAdi from reklam where reklamNo like 'HOM%' and bitTar >= $bugun and rkod='1'";
    my_reklamyaz($sorgu,$reklamdir);
?>
<div id="sagtaraf">
<br />
    <fieldset class="bdy">
    <div class="yyazi">
       <h3>ÜYE BAŞVURUSU</h3>
       <form method="post" action="basvuru.php">
          <table class="yyazi">
             <tr><td>Üye Numarası </td>
                 <td> <input type="text" size="10" name="uyeno">
             </td></tr>
             <tr><td>Şifre </td>
                 <td> <input type="password" size="10" name="sifre">
             </td></tr>
             <tr><td>Şifre Tekrarı </td>
                 <td> <input type="password" size="10" name="sifre2">
             </td></tr>
             <tr><td>e-Posta Adresi </td>
                 <td> <input type="text" size="13" name="eposta">
             </td></tr>
             <tr><td>Adı Soyadı </td>
                 <td> <input type="text" size="13" name="fullname">
             </td></tr>
             <tr><td>Güvenlik Kodu</td><td><img src="cc.php"></td></tr>
             <tr><td>Güvenlik Kodu</td>
                 <td> <input type="text" size="10" name="security">
                      <input type="submit" value="Git" class="gyazi">
             </td></tr>
          </table>
       </form>
<?php
  if(isset($_SESSION['basvuruhata'])) {
     // basvuru icin hata mesaji
     echo($_SESSION['basvuruhata']);
     session_unset();
  }
?>
    </div>
    </fieldset>
</div>
<br />
</td></tr>
</table>
</body>
</html>
