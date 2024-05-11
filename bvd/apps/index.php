<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
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
<meta content="text/html; charset=utf-8" http-equiv="content-type">
<title>Uye Basvuru Sayfasi</title>
<link rel="stylesheet" type="text/css" href="main.css" media="screen" />
<script language="JavaScript" src="js/browser.js"> </script>
<script language="JavaScript" src="js/sifreli.js"> </script>
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
<?php
    $bugun=date("Ymd", time());
    $sorgu="SELECT reklamNo,dosyaAdi from reklam where reklamNo like 'LFT%' and bitTar >= $bugun and rkod='1'";
    my_reklamyaz($sorgu,$reklamdir);
?>
        </td><td valign="top" width="800">
          <div id="header">
           <div id="baslik">
            <div class="borderright">
               <center><h3>ÜYE BAŞVURUSU</h3></center>
               <form method="post" action="basvuru.php">
                  <table class="yazi">
                     <tr><td>Üye Kodu </td>
                         <td> <input type="text" size="8" name="uyeno">
                     </td></tr>
                     <tr><td>e-Posta Adresi </td>
                         <td> <input type="text" size="13" name="eposta">
                     </td></tr>
                     <tr><td>Şifre </td>
                         <td> <input type="password" size="10" name="sifre">
                     </td></tr>
                     <tr><td>Şifre Tekrarı </td>
                         <td> <input type="password" size="10" name="sifre2">
                     </td></tr>
                     <tr><td>Adı Soyadı </td>
                         <td> <input type="text" size="13" name="fullname">
                     </td></tr>
                     <tr><td>Güvenlik Kodu</td><td><img src="../../cc.php"></td></tr>
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
<!-- sag taraf sonu -->
           <h3>VERİ DEĞİŞİM UYGULAMALARI ve KULLANILIŞI</h3>
Üyelik başvurusu yaptıktan sonra, kendi grubunuzda ya da katıldığınız grupta aşağıdaki yöntemleri kullanarak başkalarıyla veri değişimi yapabilirsiniz.
Burada üyeler kendi ilgi alanlarına ilişkin bilgileri diğer kişilerle paylaşırken, açıklanan yöntemlerden birini kullanabilirler.
<br><br>
Kullanımda birden çok yöntem olması, iletişim ortamında taşınan ya da değiştirilen veriler için uygun yöntemin değişmesinden kaynaklanmaktadır. Örneğin Klinik uygulamalarında CDISC araştırmalar sonucu saptanmış bir yöntemdir. Bilgi alış verişinde bu yöntemin kullanılması, ileride başka ülkelerle yapılacak veri değişiminde çevirim sıkıntısı yaşanmadan iletişim başlatılmasını sağlar.
<br><br>
SDXF yapısal veri değişimi web ortamında php kullanılarak programlandığında, kullanıcılar veri değişiminde, ortamın farklılığından kaynaklanan güçlükleri yenmiş olacaklardır. Örneğin windows kullanan birinin gönderdiği SDXF kalıplı dosya, Mac ortamına indirilirken Mac koşullarına çevrilmiş olur.
<br><br>
EDIFACT Birleşmiş Milletler tarafından geliştirilip değişen bir yöntem olduğundan yönetim ticari ve taşıma uygulamalarında kolaylıkla kullanılabilmektedir.
<br><br>
MIDAS/FITS tümüyle uydu tabanlı görüntülerin taşınması için tasarlanmış bir yöntemdir. Özellikler aynı amaçla ilgili yazılım gruplar tarafından kullanılabilir.
<br><br>
            <dl class="yazi">
               <dt> SDXF Yapısal Veri Değişimi
                  <dd>
SDXF Yapısal Veri Değişimi, değişik verileri bilgisayarlar arası taşınması için kullanılan bir yöntemdir. Internet'te RCF-3072 ile tanımı yapılmıştır. Buradaki uygulamada, değişik kaynaklardan (üyelerden) gönderilen değişik tür verilerin istenilen üyelere gönderilmesi sağlanmıştır.
                  </dd>
               </dt>
               <dt> CDISC Klinik Veri Değişimi
                  <dd>
CDISC Klinik Veri Değişimi, verilerin klinikler aracılığıyla sisteme aktarılması ve ilgilenenlerin kullanması için kurulmuş bir uygulamadır. Klinik verileri CDISC standartlarına göre düzenlenmiştir. İlgilenenler arasında veriler paylaşılmaktadır.
                  </dd>
               </dt>
               <dt> Çevresel Veri Değişimi
                  <dd>
Çevresel Veri Değişimi, aslında çevreyle ilgili haberleri, iş listelerini ve ihaleleri web sayfalarında görüntüler.
                  </dd>
               </dt>
               <dt> EDIFACT Veri Değişimi
                  <dd>
EDIFACT Yönetim, Ticari ve Taşıma Amaçlı Elektronik Veri Değişimidir. EDIFACT Birleşmiş Milletler tarafından geliştirilir ve değiştirilir. ISO 9735 olarak standart haline getirilmiştir.
                  </dd>
               </dt>
               <dt> MIDAS/FITS Veri Değişimi
                  <dd>
MIDAS Veri Değişim sistemi, FITS (Flexible Image Transport System) kalıplama sistemini kullanır. Bu sistem veri değişimi için kullanılmaktadır.
                  </dd>
               </dt>
               <dt> JSON Veri Değişimi
                  <dd>
JSON Veri Değişimi, dil bağımsız veri değişim kalıbıdır. Birçok dil altında tanımlanmıştır. XML ve diğer standartların yükünü hissetmeden diller arasında bilgi değişimini sağlar.
                  </dd>
               </dt>
            </dl>
<?php
           /* haberlere bak
            * bugun >= baslangic and bugun <= bitis ise haberi yazdir
            * GNC = guncel haberler
            */
           /* 440 x 140 ile resim goruntuleme ve haberleri olusturma */
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
           <br>
           </div>
           <div id="altsatir">
               <br />
               <span class="rside">Tasarım: Aybim Bilgisayar Tic Ltd Şti&nbsp;&nbsp;</span>
           </div>
          </div>
        </td></tr>
      </table>
   </div>
</div>
</body>
</html>
