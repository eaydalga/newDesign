<?php
session_start();
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
<meta content="text/html; charset=utf-8" http-equiv="content-type">
<title>Odeme Adimi</title>
<link rel="stylesheet" href="main.css" type="text/css">
<script type="text/javascript" src="js/browser.js"></script>
<script type="text/javascript" src="js/baslik.js"></script>
<script type="text/javascript" src="js/goster.js"></script>
<script>
function my_void()
{
}
</script>
<body onLoad="ayarla();">
<div id="taban">
  <table cellpadding="0" cellspacing="0" border="0" width="960">
  <tr>
  <td valign="top">
  <div id="ysoltaraf">
  <a href="index.html"><img src="img/aybim.png" border="0"></a><br>
  <span class="byazi">Aybim Bilgisayar Tic. Ltd.</span><br>
  </div>
  </td>
  <td valign="top" width="800">
  <div id="yortataraf">
<br /><br />
<?php
if(!empty($_SESSION['urun'])) {
   $dirfile = "urunler/" . $_SESSION['urun'];
   echo("<br>Indireceğiniz ürün için burayı <a href=\"$dirfile\">TIKLAYIN</a>");
} else {
   echo("<br>Yaptığınız bağış için TEŞEKKÜR ederiz");
}
session_unset();
?>
    <br />
  </div>
  </td>
  </tr></table>

</div>
<br>
</body></html>
