<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<?php
/************************************************************************
** 1. dosya upload islemi
**
*********************************************************************** */
  session_start();
  require("include/conf.inc");
  require("include/okuma.inc");
  require("include/aes.inc");
  // db baglantısı gerekli
  $baglan = mysql_connect($dbhost, $dbuser, $dbpasswd)
            or die("Bağlantı kurulamadı" . mysql_error());
  mysql_select_db($dbname, $baglan);
  if($_SERVER['HTTP_REFERER'] != $webhost."bvd/apps/".$userphp) {
       $aa = "Location: ".$webhost."bvd/";
       header($aa);
       exit;
  }
  $usr = $_SESSION['user'];
  function listdir($directory,$pusr,$program)
  {
  /* **** buraya yetki de eklenmeli .... *********** */
      $file = scandir($directory);
      natcasesort($file);
      $cikti = "<ul>";
      foreach($file as $tfile) {
          $ff = strstr($tfile,$pusr);
          if($ff != '') {
              if($tfile != "." && $tfile != "..") {
                  $cikti .= "<li>";
                  $tt = $directory."/".$tfile;
                  if($program != '')
                       $cikti .= "<a href=\"".$tt."\">".$tfile;
                  else $cikti .= "<a href=\"".$program."'$tt'\">".$tfile;
                  $cikti .= "</a><br />";
              }
          }
      }
      $cikti .= "</ul>";
      return $cikti;
  }
  /* ************** create input html ********************************** */
?>
<html>
<head>
<meta content="text/html; charset=utf-8" http-equiv="content-type">
<title>SDXF Dosya Isleri</title>
<link rel="stylesheet" type="text/css" href="main.css" media="screen" />
<script language="JavaScript" src="js/baslik.js"> </script>
<script language="JavaScript" src="js/browser.js"> </script>
<script language="JavaScript" src="js/goster.js"> </script>
<script language="JavaScript" src="js/sifreli.js"> </script>
<script type="text/javascript">
//<![CDATA[
function init() {
  document.getElementById('fform1').onsubmit=function() {
                        //'target' is the name of the iframe
      document.getElementById('fform1').target = 'target'; 
      }
  document.getElementById('fform2').onsubmit=function() {
                        //'target2' is the name of the iframe
      document.getElementById('fform2').target = 'target2'; 
      }
  document.getElementById('fform4').onsubmit=function() {
                        //'target4' is the name of the iframe
      document.getElementById('fform4').target = 'target4'; 
      }
}
window.onload=init;
//]]>
</script>
</head>
<body topmargin="0" leftmargin="0" marginheight="0" marginwidth="0">
<div id="headertop1">
   <div id="headertop">
<?php
    include("inc/baslik.inc");
?>
    <table cellpadding="0" cellspacing="0" border="0">
    <tr><td valign="top" width="140">
          <div id="soltaraf">
            <div id="masterdiv">
                <div class="menutitle"><a href="sifreli.php" class="vnav">Kullanıcı Sayfası</a></div>
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
<?php
             if(isset($_SESSION['basvuruhata'])) {
                // basvuru icin hata mesaji
                echo($_SESSION['basvuruhata']);
                session_unset();
             } else {
?>
    <h3>SDXF Dosyası İşlemleri</h3>
<div class="yazi">
SDXF (Yapısal Veri Değişim Kalıbı), yığınlardan oluşan bir dosyanın değişimi için kullanılan bir yöntemdir. Var olan diğer veri değişim yöntemlerinden ayrı olarak, SDXF her tür dosyanın bir yığın biçiminde paketlenip taşınmasını sağlar. Bu uygulamada sunulan işlemlerin başlıkları aşağıda kullanıma sunulmuştur...
<br><br>
    <ul>
    <li> <b>Dosya Yüklemeleri</b> <a href="javascript:mygoster('sec1')">(Evet)</a>
<br>Çeşitli dosyaların kullanıcının çalışma alanına yüklenmesi işlemidir. Aynı adımda birden çok dosya yklemesi yapılabileceği gibi birden çok yüklemeyle tüm dosyaların çalışma alanına taşınması tamamlanabilir.<br>
         <div id="sec1" style="display: none">
             <div id="sagdan">
             <iframe id="target" name="target" src="" style="width:290px;height:200px;border:0px solid #fff;overflow:auto;"></iframe>
             </div>
             <form id="fform1" method="post" enctype="multipart/form-data" action="upload.php">
             <table>
             <tr><td><input name="uploaded" type="file" /></td><td></td></tr>
             <tr><td><input name="uploaded" type="file" /></td><td></td></tr>
             <tr><td><input name="uploaded" type="file" /></td><td></td></tr>
             <tr><td><input name="uploaded" type="file" /></td><td></td></tr>
             <tr><td><input name="uploaded" type="file" /></td><td></td></tr>
             <tr><td><input name="uploaded" type="file" /></td><td></td></tr>
             <tr><td><input name="uploaded" type="file" /></td>
                 <td><input type="submit" value="Yukle" /></td></tr>
             </table>
             </form>
             <br /> <br /> <br /> <br />
         </div>
    <li> <b>SDXF dosyası Yaratılması</b> <a href="javascript:mygoster('sec2')">(Evet)</a>
<br>Kullanıcının yüklediği çalışma alanında biriken dosyalar yığın tanıtım kodu ile birleştirilerek SDXF kalıbında dosya elde edilir. Dosya yığınların ardaşık ve yapısal dizilmesinden oluşur. Burada dpsyalar adım adım kullanıcı tarafından oluşturulur.
<br>
         <div id="sec2" style="display: none">
            <h3>Yığın Yarat</h3>
             <div id="sagdan">
             <iframe id="target2" name="target2" src="" style="width:290px;height:350px;border:0px solid #fff;overflow:auto;"></iframe>
             </div>
            <form name="fform2" id="fform2" action="crtsdxf.php">
            <table width = "55%">
            <tr><td>Ekle</td>
                <td>
                  <select name="ekleme">
                     <option value="New"> Yeni
                     <option value="After"> Peşine
                  </select>
                </td><td>
                </td></tr>
            <tr><td>Öndeki</td>
                <td colspan="2">
                  <input id="previous" type="text" name="previous">
                  <input type="button" value="Gözat" onclick="javascript:cc('previous')">
                </td></tr>
            <tr><td>Yığın Adı</td>
                <td>
                  <input type="text" id="yiginadi" name="yiginadi" class="required">
                </td></tr>
            <tr><td colspan="2"><b>Durum Kodları</b></td>
                <td>
                </td></tr>
            <tr><td></td><td>Veri Türleri</td>
                <td>
                  <select id="flag" name="flag">
                     <option value="01"> Yapı
                     <option value="02"> İkili Dizi
                     <option value="03"> Sayısal
                     <option value="04"> Karakter
                     <option value="05"> Kayan Noktalı
                     <option value="06"> UTF-8
                  </select>
                </td></tr>
            <tr><td></td><td>Sıkıştırılmış Yığın</td>
                <td>
                  <input id="dkod1" name="dkod1" type="checkbox" value="16">
                </td></tr>
            <tr><td></td><td>Kriptolu Yığın</td>
                <td>
                  <input id="dkod2" name="dkod2" type="checkbox" value="8">
                </td></tr>
            <tr><td></td><td>Kısa Yığın</td>
                <td>
                  <input id="dkod3" name="dkod3" type="checkbox" value="4">
                </td></tr>
            <tr><td></td><td valign="top">Dizi</td>
                <td>
                  <input id="dkod4" name="dkod4" type="checkbox" value="2"><br>
                  Boy <input id="arrcl" name="arrcl" type="input" size="2">
                  Öğe <input id="arrel" name="arrel" type="input" size="2">

                </td></tr>
            <tr><td>Veri Girişi</td>
                <td colspan="2">
                  <input id="vdata" name="vdata" type="text">
                </td></tr>
            <tr><td>Yapılar</td>
                <td colspan="2">
                  <input id="struct" type="text" name="struct">
                  <input type="button" value="Gözat" onclick="javascript:dd('struct')">
                </td></tr>
            <tr><td>Dosyalar</td>
                <td colspan="2">
                  <input id="file" type="text" name="file">
                  <input type="button" value="Gözat" onclick="javascript:cc('file')">
                </td></tr>
            <tr><td colspan="2"><b>Erişim Seçenekleri</b></td>
                <td>
                </td></tr>
            <tr><td></td>
                <td>Kullanıcı</td>
                <td>
                  <input id="access" name="access" type="checkbox" value="700" checked>
                </td></tr>
            <tr><td></td>
                <td>Aynı Grup/Sınıf</td>
                <td>
                  <input id="access" name="access" type="checkbox" value="770">
                </td></tr>
            <tr><td></td>
                <td>Herkese</td>
                <td>
                  <input id="access" name="access" type="checkbox" value="777">
                  <input type="submit" value="Gönder"
                         onclick="return ikontrol('fform2');"/>
                </td></tr>
            </table>
            </form>
         </div>
    <li> <b>SDXF dosyasının İndirilmesi</b> <a href="javascript:mygoster('sec3')">(Evet)</a>
<br>Kullanıcının hazır SDXF dosyaları indirilmek için listelenir. Bu dosyaların nasıl ve kimler tarafından indirilebileceği yine kullanıcı tarafından belirtilir. Burada listelenen dosya adlarından seçilen indirilebilir.
<br>
         <div id="sec3" style="display: none">
<?php
    /* kullanıcının klasorlerinde bulunan dosyalardan
       secim listesi olusturulur */
    $pusr = $usr."-";
    $directory = "download/";
    $program="";
    $cikti = listdir($directory,$pusr,$program);
    echo($cikti);
    if(strlen($cikti) <= 9) {
?>
       <br><br>
       <div class="yazi">
           Erişebileceğiniz hiç bir dosya bulunmadı...
       </div>
       <br><br>
<?php
    }
?>
         </div>
    <li> <b>SDXF Dosyasını Yükleme</b> <a href="javascript:mygoster('sec4')">(Evet)</a>
<br>Daga önce hazırlanmış bir SDXF dosyası sunucuya yüklenir ve kullanıcı tarafından başkalarının kullanımına açılır (Verdiği yetkiler ölçüsünde).
         <div id="sec4" style="display: none">
             <div id="sagdan">
             <iframe id="target4" name="target4" src="" style="width:290px;height:200px;border:0px solid #fff;overflow:auto;"></iframe>
             </div>
             <br><br>
             <div class="yazi">
             Bu dosya, sunucuda kullanıcı adı eklenerek saklanacaktır. Yani dosya adı "&lt;kullanıcı&gt;-&lt;dosya_adi&gt;" biçiminde olacaktır.<br><br>
Kullanıcılar dilerlerse dosyaya erişimi sınırlandırabilirler. Herkesin her dosyaya erişimi eşit olmayabilir. Kullanıcılar yalnız kendi kullanımları için dosyaları sunucuya yükleyebilirler (yedekleme amaçlı). Bu tür dosyaların güvenle saklanabilmesi için şifrelenmiş ve sıkıştırılmış olmasına özen gösterilmelidir.
             </div>
             <br><br>
             <b>Yüklenecek Dosyayı girin:</b>
             <form id="fform4" name="fform4" method="post" enctype="multipart/form-data" action="upload.php">
             <table>
             <tr><td><input name="uploaded" type="file" /></td>
                 <td><input type="submit" value="Yukle"/></td></tr>
             </table>
             </form>
             <br /> <br />
         </div>
    <li> <b>SDXF Dosyasını Listeleme</b> <a href="javascript:mygoster('sec5')">(Evet)</a>
<br>
Var olan bir SDXF dosyası seçilip içeriği (başlıklar biçiminde) görüntülenebilir. Burada kullanıcının sunduğu SDXF dosyaları listelenir. Dosyaların sıkıştırılmış ve şifrelenmiş olmaları durumunda ancak yığınların elverdiği ölçüde açıklayıcı sonuç elde edilebilir.
<br>
         <div id="sec5" style="display: none">
             <div id="sagdan">
             <iframe id="target5" name="target5" src="" style="width:290px;height:200px;border:0px solid #fff;overflow:auto;"></iframe>
             </div>
<?php
    $pusr = $usr."-";
    $directory = "download/";
    $program="sdxfbak.php?id=";
    $cikti = listdir($directory,$pusr,$program);
    echo($cikti);
    if(strlen($cikti) <= 9) {
?>
        <br><br>
        <div class="yazi">
           Listelemeniz için seçebileceğiniz dosya bulunmadı. Önerimiz:
           <ul>
            <li> Bir SDXF dosyası yükleyin ve içeriğini listeleyin.<br>
                 İçerik düzgün listelenemezse ya dosyanız bozuktur ya da dosyada kriptolama yapılmıştır. Bu durumda şifreyi bilmeden içini açamazsınız.
            <li> Çeşitli dosyaları yükleyip bir SDXF dosyası yaratın.<br>
                 Sonra yaratılan dosyayı listeleyin. Böylece yaratılan dosyanın nasıl açılabileceğini de öğrenmiş olursunuz.
            <li> Bir başkasından bir dosyasına erişme olanağı isteyin ve o dosyayı listeleyin.
           </ul>
        </div>
        <br><br>
        <br><br>
<?php
    }
?>
         </div>
     </ul>
</div>
<?php
             }
?>
           </div>
          </div>
        </td></tr>
      </table>
   </div>
</div>
</body>
</html>

