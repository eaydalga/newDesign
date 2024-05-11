<?php
session_start();
?>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
</head>
<body>
<?php
$say = 0;
unset($s);
if(isset($_REQUEST['s'])) {
    $s=$_REQUEST['s'];
    $say += 1;
}
if(isset($_REQUEST['t'])) {
    $t = $_REQUEST['t'];
    $say += 1;
}
if($say < 2) {
    echo("<script>kapat();</script>");
    exit;
}
$parite = 1;
$tarball = "urun-".$s."-".$t.".zip";
$_SESSION['urun'] = "urunler/" . $tarball;
?>
<p>&nbsp;</p>
<p>&nbsp;</p>
<b>Seçtiğiniz ürün</b> :
<?php
if($s == 1) {
   echo("Temel");
   $prod = "temel";
   $tut = "10.00";
   }
if($s == 2) {
   echo("Ortanca");
   $prod = "ortanca";
   $tut = "12.00";
   }
if($s == 3) {
   echo("Gelişkin");
   $prod = "geliskin";
   $tut = "15.00";
   }
$prod = $prod . "-" . $t;
echo(" grubun " . $t. " nolu ürünüdür. Bu ürün grubunun tutarı $tut ABD Dolarıdır. Seçiminiz doğruysa <b>BUY KNOWN</b> tuşuna basarak satın alma işlemine devam edin. Yoksa <b>KAPAT</b> tuşunu kullanarak bu ekrandan çıkın");
?>
<p>&nbsp;</p>
<table width="80%">
<tr><td valign="top" width="200"> &nbsp;
</td><td valign="top">
<form action="https://www.paypal.com/cgi-bin/webscr" method="post">
<input type="hidden" name="cmd" value="_xclick">
<input type="hidden" name="business" value="ergunx@aybim.com.tr">
<?php
echo("<input type=\"hidden\" name=\"item_name\" value=\"$prod\">");
echo("<input type=\"hidden\" name=\"amount\" value=\"$tut\">");
?>
<input type="hidden" name="page_style" value="Primary">
<input type="hidden" name="no_shipping" value="1">
<input type="hidden" name="return" value="http://www.aybim.com.tr/odendi.php">
<input type="hidden" name="cancel_return" value="http://www.aybim.com.tr/iptal.php">
<input type="hidden" name="no_note" value="1">
<input type="hidden" name="currency_code" value="USD">
<input type="hidden" name="lc" value="TR">
<input type="hidden" name="bn" value="PP-BuyNowBF">
<input type="image" src="https://www.paypal.com/en_US/i/btn/btn_buynow_LG.gif" border="0" name="submit" alt="PayPal - The safer, easier way to pay online!">
<img alt="" border="0" src="https://www.paypal.com/en_US/i/scr/pixel.gif" width="1" height="1">
</form>

</td><td valign="top">
<form><span class="rpic"><input type="button" onclick="javascript:kapat()" class="nav" value="KAPAT"></span></form>
</td></tr></table>
<p>&nbsp;</p>
<b>PAYPAL</b> Ödeme sayfasının üst bölümünde toplam alış veriş tutarı bulunur. Tutar ABD Doları türündendir.
<p>&nbsp;</p>
Paypal ödemesini tamamlamak için bir sonraki sayfada ya "paypal" hesabınıza girin ya da hesabınız yoksa 
<b>Sign up for a PayPal account and continue checkout</b>. yazısındaki bağı tıklayın (bağ <b>continue checkout</b> biçimindedir). Görüntülenen ekranda kart bilgilerinizi girebilirsiniz...
<p>&nbsp;</p>
Eğer işleme devam etmeyecekseniz o zaman 
<b>Return to Aybim Bilgisayar Tic Ltd.</b> satırındaki bağı tıklayın ve Aybim sayfalarına geri dönüş yapın...
</body>
</html>
