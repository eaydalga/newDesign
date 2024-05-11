<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
</head>
<body>
<?php
unset($s);
if(isset($_REQUEST['s'])) {
    $s=$_REQUEST['s'];
    $t=$_REQUEST['t'];
    $say=1;
} elseif(isset($_REQUEST['sayfa'])) {
    $say = $_REQUEST['sayfa'];
    $s = $_REQUEST['wf_s'];
    $t = $_REQUEST['wf_t'];
} else {
    $say = 0;
    echo("<script>kapat();</script>");
    exit;
}
$parite = 1;
?>
    <form id="id12333">
<?php
    echo("<input type=\"hidden\" id=\"wf_s\" name=\"wf_s\" value=\"$s\">");
    echo("<input type=\"hidden\" id=\"wf_t\" name=\"wf_t\" value=\"$t\">");
if($say == "1") {
   $whesap = 40 * $parite;
   if($s == 2)
        $whesap = $whesap + 150 * $parite;
   if($s > 2)
        $whesap = $whesap + 460 * $parite;
   $whesap = intval($whesap / 10) * 10;
   echo("<p class=\"yazi\">Bilgi: Seçilen web tasarımı için fiyatımız $whesap $'dan başlamaktadır.</p>");
?>
  <fieldset id="wf_f1">
  <legend>Ad/Ünvan</legend>
  <span class="yazi">
    <label for="Adi" class="preField">Adınız/Ünvanınız</label>
  </span>
  <input type="text" size="30" id="Adi" name="Adi" class="validate-alphanum required" title="Kendi adınızı ya da firmanın ünvanını buraya yazın" />
  <span class="reqMark">*</span><br/>
  </fieldset>

  <fieldset id="wf_f3"> 
  <legend>Adresler</legend>
  <table>
  <tr><td valign="top">
      <span class="yazi">
        <label for="Adres" class="preField">Posta Adresiniz</label>
      </span>
  </td><td valign="top">
        <textarea cols="45" rows="4" id="Adres" name="Adres" class="required" title="CD\'lerinizi size ulaştırmamız için posta adresinizi doğru olarak girmelisiniz"></textarea>
  </td><td valign="top">
      <span class="reqMark">*</span><br/>
  </td></tr>
  <tr><td valign="top">
      <span class="yazi">
        <label for="Eposta" class="preField">e-Posta Adresiniz</label>
      </span>
  </td><td valign="top">
        <input type="text" size="20" id="Eposta" name="Eposta" class="validate-email required" value="" title="e-posta adresinizin doğru olması size dönebilmemiz ve kendi e-posta adresimizi size ulaştırmamız için önemlidir"/>
  </td><td valign="top">
      <span class="reqMark">*</span><br/>
  </td></tr>
  </table>
  </fieldset>

  <fieldset id="wf_f31"> 
  <legend>Telefon ve Fax</legend>
  <span class="yazi">
  Birden çok telefon ya da faks  numarası varsa, aralarına boşluk bırakın.
İsteyenler telefon numaralarını alan koduyla birlikte xxxx.xxxx.xx biçiminde de
yazabilirler. Bu durumda araya noktalı virgül konulması düzgün sayfa görüntüsü
için uygundur.
  <table>
  <tr><td valign="top">
      <span class="yazi">
        <label for="Tel" class="preField">Tel Numaraları</label>
      </span>
  </td><td valign="top">
       <input type="text" size="15" id="Tel" name="Tel" class="required" value="" title="Tel numaranız web sitesinde yayımlanacaktır. Birden çok numara yazacak olursanız noktalı virgülle ayırmanız sayfada görüntülenme biçimi açısından yararlıdır"/>
  </td><td valign="top">
       <span class="reqMark">*</span><br/>
       </span>
  </td></tr>
  <tr><td valign="top">
       <span class="yazi">
         <label for="Faks" class="preField">Faks Numaraları</label>
       </span>
  </td><td valign="top">
       <input type="text" size="15" id="Faks" name="Faks" class="" value="" title="Birden çok faks numarası yazarsanız noktalı virgülle ayırmanız, sayfada görüntülenmesi açısından önemlidir"/>
  </td><td valign="top">
  </td></tr>
  </table>
  </fieldset>
<?php
  if($s == 20) {
     // orta boy web tasarimi icin
?>
  <fieldset>
     <legend>ÜCRETLİ SEÇENEKLER</legend>
     <span class="yazi">
       <label for="wf_ghaber" class="preField">Ana Sayfanızda "Güncel haber" köşesi ister misiniz? </label>
     </span>
       <input type="checkbox" id="wf_ghaber" name="wf_ghaber" class="" value="E" title="Güncel haber köşesi için bu alanı seçmek yeterlidir."/>
     <span class="yazi">
       <label for="wf_ozel" class="preField">İletişim sayfasında özel ileti başlığı kullanmak ister misiniz? </label>
     </span>
       <input type="checkbox" id="wf_ozel" name="wf_ozel" class="" value="E" title="Özel ileti başlıkları gelen iletileri ayırmak için kullanılabilir. Hatta bazı iletilerin değişik birimlere yönlendirilerek yanıtlanması sağlanabilir"/>
  </fieldset>
<?php
  }
  if($s == 30) {
     // gelismis web tasarimi icin
?>
  <fieldset>
     <legend>ÜCRETE DAHİL HIZMETLER</legend>
     <span class="yazi">
       <label for="wf_ghaber" class="preField">Ana Sayfanızda "Güncel haber" köşesi ister misiniz? </label>
     </span>
       <input type="checkbox" id="wf_ghaber" name="wf_ghaber" class="" value="E" title="Güncel haber köşesi istenirse bu alan doldurulmalıdır"/>
     <span class="yazi">
       <label for="wf_ozel" class="preField">İletişim sayfasında özel ileti başlığı kullanmak ister misiniz? </label>
     </span>
       <input type="checkbox" id="wf_ozel" name="wf_ozel" class="" value="E" title="Özel ileti başlıkları gelen iletileri değişik sınıflarca yanıtlama olanağı sağlar"/>
  </fieldset>

  <fieldset>
     <legend>ÜCRETLİ SEÇENEKLER</legend>
     <span class="yazi">
       <label for="wf_icerik" class="preField">Web sayfası içeriği güncelleme ister misiniz? </label>
     </span>
       <input type="checkbox" id="wf_icerik" name="wf_icerik" class="" value="E" title="Web sayfası içeriğini kendi başınıza güncellemek isterseniz bu alanı doldurun"/>
     <span class="yazi">
       <label for="wf_arama" class="preField">Güncel haber arşivi ve arama ister misiniz? </label>
     </span>
       <input type="checkbox" id="wf_arama" name="wf_arama" class="" value="E" title="Güncel haber arşivi ve arama yapmak için bu alanı doldurun"/>
     <span class="yazi">
       <label for="wf_siteden" class="preField">Siteden arama ister misiniz? </label>
     </span>
       <input type="checkbox" id="wf_siteden" name="wf_siteden" class="" value="E" title="Site içi arama işlemini kullanmak için bu alanı doldurun"/>
     <span class="yazi">
       <label for="wf_smap" class="preField">Otomatik Site haritası ister misiniz? </label>
     </span>
       <input type="checkbox" id="wf_smap" name="wf_smap" class="" value="E" title="Otomatik site haritasının sitenizde bulunması için bu alanı doldurmanız gerekmektedir"/>
     <span class="yazi">
       <label for="wf_sepet" class="preField">Alış/Veriş sepeti kullanmak ister misiniz? </label>
     </span>
       <input type="checkbox" id="wf_sepet" name="wf_sepet" class="" value="E" title="alış veriş sepeti kullanmak isterseniz bu alanı doldurmanız gerekecektir" />
     <span class="yazi">
       <label for="wf_fiyat" class="preField">Hizmet/Ürün fiyat giriş programı ister misiniz? </label>
     </span>
       <input type="checkbox" id="wf_fiyat" name="wf_fiyat" class="" value="E" title="Hizmet/Ürün fiyatlarının sayfalarınızda görüntülenmesi için bu alanı doldurmanız gerekecektir"/>
     <span class="yazi">
       <label for="wf_kur" class="preField">Kur/Katsayı giriş programı ister misiniz? </label>
     </span>
       <input type="checkbox" id="wf_kur" name="wf_kur" class="" value="E" title="Kur/Katsayı kullanımı hizmet/ürün fiyatlarını değiştirmek yerine kur/katsayı değişikliği yapmanızı sağlar. Bu alanı doldurursanız ilgili programlar sitenizin altyapısında kullanılacaktır."/>
  </fieldset>
<?php
  }
?>
  <table>
<!--
  <tr><td valign="top" colspan="2">
      <span class="yazi">Burada görüntülenen resimdeki rakamları formdaki Güvenlik Kodu alanına yazmayı unutmayın. Böylece iletiniz diğer spam iletilerden ayrılmış olacaktır.</span>
    </td><td valign="bottom">
      <img src="cc.php" />
    </td></tr>
  <tr><td valign="top">
      <label for="security_code" class="yazi">Güvenlik Kodu</label>
    </td><td valign="top">
         <input id="security_code" name="security_code" type="text" class="required" />
         <span class="reqMark">*</span>
    </td><td valign="top">
    </td></tr>
-->
  <tr><td valign="top">
  <span class="yazi">
    <label for="submit_" class="preField">Bilgileri göndermek için Devam tuşuna basın.</label>
  </span>
  </td><td valign="top">
<?php
    $say=$say+1;
    echo("<input type=\"button\" class=\"primaryAction\" name=\"submit\" id=\"submit-\" value=\"Devam\" onclick=\"javascript:ksecim2($say,8,9)\"/>");
    echo("</td><td valign=\"top\"> </td></tr> </table>");
} elseif($say == "2") {
   // print_r($_REQUEST);
   // echo("sayfa: ".$_REQUEST['sayfa']."<br />");
   // echo("wf_s: ".$_REQUEST['wf_s']."<br />");
   $sec = "";
   $onlu = intval($t / 10);
   $birli = $t % 10;
   $sec = "";
   switch($onlu) {
      case "1": 
           $sec = "On";
           break;
      case "2":
           $sec = "Yirmi";
           break;
      case "3":
           $sec = "Otuz";
           break;
      case "4":
           $sec = "Kırk";
           break;
      case "5":
           $sec = "Elli";
           break;
      case "6":
           $sec = "Altmış";
           break;
      case "7":
           $sec = "Yetmiş";
           break;
      case "8":
           $sec = "Seksen";
           break;
      case "9":
           $sec = "Doksan";
           break;
   }
   switch($birli) {
      case "1": 
           $sec .= "Birinci";
           break;
      case "2":
           $sec .= "İkinci";
           break;
      case "3":
           $sec .= "Üçüncü";
           break;
      case "4":
           $sec .= "Dördüncü";
           break;
      case "5":
           $sec .= "Beşinci";
           break;
      case "6":
           $sec .= "Altıncı";
           break;
      case "7":
           $sec .= "Yedinci";
           break;
      case "8":
           $sec .= "Sekizinci";
           break;
      case "9":
           $sec .= "Dokuzuncu";
           break;
   }
   //  echo("wf_t: ".$_REQUEST['wf_t']."<br />");
   echo("<fieldset>");
   echo("<legend>TASARIM</legend><span class=\"yazi\">");
   if($sec == "") {
       echo("Secilen tasarım YOK...<br />");
   } else {
       echo("$sec tasarım secilmiş...<br />");
   }
   echo("</span></fieldset>");
   echo("<fieldset>");
   echo("<legend>ADI/ÜNVANI</legend><span class=\"yazi\">");
   $wh=$_REQUEST['Adi'];
   echo($wh."<br />");
   echo("<input type=\"hidden\" name=\"Adi\" value=\"$wh\"/>");
   echo("</span>");
   echo("</fieldset>");
   echo("<fieldset>");
   echo("<legend>ADRESLER</legend><span class=\"yazi\">");
   echo("Adres: ".$_REQUEST['Adres']."<br />");
   echo("E-posta: ".$_REQUEST['Eposta']."<br />");
   echo("Telefonlar: ".$_REQUEST['Tel']."<br />");
   echo("Faks: ".$_REQUEST['Faks']."<br />");
   $wa=$_REQUEST['Adres'];
   $we=$_REQUEST['Eposta'];
   $wt=$_REQUEST['Tel'];
   $wf=$_REQUEST['Faks'];
   echo("<input type=\"hidden\" name=\"Adres\" value=\"$wa\"/>");
   echo("<input type=\"hidden\" name=\"Eposta\" value=\"$we\"/>");
   echo("<input type=\"hidden\" name=\"Tel\" value=\"$wt\"/>");
   echo("<input type=\"hidden\" name=\"Faks\" value=\"$wf\"/>");
   echo("</span>");
   echo("</fieldset>");
   $hesap2=0;
   if($s == 20) {
       echo("<fieldset>");
       echo("<legend>ÜCRETLİ SEÇENEKLER</legend><span class=\"yazi\">");
       if(isset($_REQUEST['wf_ghaber'])) {
          $wgh=$_REQUEST['wf_ghaber'];
          echo("<input type=\"hidden\" name=\"wf_ghaber\" value=\"$wgh\"/>");
          echo("Güncel haber: ".$_REQUEST['wf_ghaber']." (Bedeli 30 $)<br />");
          $hesap2 += 30;
       }
       if(isset($_REQUEST['wf_ozel'])) {
          $wozl=$_REQUEST['wf_ozel'];
          echo("<input type=\"hidden\" name=\"wf_ozel\" value=\"$wozl\"/>");
          echo("Özel başlıklı ileti: ".$_REQUEST['wf_ozel']." (Bedeli 20 $)<br />");
          $hesap2 += 20;
       }
       echo("</fieldset>");
   }
   $hesap3 = 0;
   if($s == 30) {
       echo("<fieldset>");
       echo("<legend>ÜCRETE DAHİL SEÇENEKLER</legend><span class=\"yazi\">");
       if(isset($_REQUEST['wf_ghaber'])) {
          $wgh=$_REQUEST['wf_ghaber'];
          echo("<input type=\"hidden\" name=\"wf_ghaber\" value=\"$wgh\"/>");
          echo("Güncel haber: ".$_REQUEST['wf_ghaber']."<br />");
       }
       if(isset($_REQUEST['wf_ozel'])) {
          $wozl=$_REQUEST['wf_ozel'];
          echo("<input type=\"hidden\" name=\"wf_ozel\" value=\"$wozl\"/>");
          echo("Özel başlıklı ileti: ".$_REQUEST['wf_ozel']."<br />");
       }
       echo("</fieldset>");
       echo("<fieldset>");
       echo("<legend>ÜCRETLİ SEÇENEKLER</legend><span class=\"yazi\">");
       if(isset($_REQUEST['wf_icerik'])) {
          $wku=$_REQUEST['wf_icerik'];
          echo("<input type=\"hidden\" name=\"wf_icerik\" value=\"$wku\"/>");
          echo("Arşiv ve Arama: ".$_REQUEST['wf_icerik']." (Bedeli 150 $)<br />");
          $hesap3 += 150;
       }
       if(isset($_REQUEST['wf_arama'])) {
          $wku=$_REQUEST['wf_arama'];
          echo("<input type=\"hidden\" name=\"wf_arama\" value=\"$wku\"/>");
          echo("Arşiv ve Arama: ".$_REQUEST['wf_arama']." (Bedeli 70 $)<br />");
          $hesap3 += 70;
       }
       if(isset($_REQUEST['wf_siteden'])) {
          $wku=$_REQUEST['wf_siteden'];
          echo("<input type=\"hidden\" name=\"wf_siteden\" value=\"$wku\"/>");
          echo("Siteden Arama: ".$_REQUEST['wf_siteden']." (Bedeli 80 $)<br />");
          $hesap3 += 80;
       }
       if(isset($_REQUEST['wf_smap'])) {
          $wmp=$_REQUEST['wf_smap'];
          echo("<input type=\"hidden\" name=\"wf_smap\" value=\"$wmp\"/>");
          echo("Otomatik site haritası: ".$_REQUEST['wf_smap']." (Bedeli 50 $)<br />");
          $hesap3 += 50;
       }
       if(isset($_REQUEST['wf_sepet'])) {
          $wsp=$_REQUEST['wf_sepet'];
          echo("<input type=\"hidden\" name=\"wf_sepet\" value=\"$wsp\"/>");
          echo("Alış/Veriş sepeti: ".$_REQUEST['wf_sepet']." (Bedeli 50 $)<br />");
          $hesap3 += 50;
       }
       if(isset($_REQUEST['wf_fiyat'])) {
          $wfy=$_REQUEST['wf_fiyat'];
          echo("<input type=\"hidden\" name=\"wf_fiyat\" value=\"$wfy\"/>");
          echo("Fiyat güncelleme: ".$_REQUEST['wf_fiyat']." (Bedeli 40 $)<br />");
          $hesap3 += 40;
       }
       if(isset($_REQUEST['wf_kur'])) {
          $wku=$_REQUEST['wf_kur'];
          echo("<input type=\"hidden\" name=\"wf_kur\" value=\"$wku\"/>");
          echo("Kur/Katsayı girişi: ".$_REQUEST['wf_kur']." (Bedeli 20 $)<br />");
          $hesap3 += 20;
       }
   }
?>
  <fieldset>
  <legend>HESAPLAMA</legend>
  <table border="1">
<?php
   /* s = 1 icin 100
   *     tur == 2
   *        fazla hizmet icin 30
   *  s = 2 icin 150
   *     tur == 4
   *        fazla hizmet icin 30
   *        her e-posta 10
   *  s = 3 icin 200
   *        her fazla hizmet 30
   *        her e-posta 10
   */
   $whesap = 40 * $parite;
   if($s == 2)
        $whesap = $whesap + 150 * $parite;
   if($s > 2)
        $whesap = $whesap + 460 * $parite;
   $whesap = intval($whesap / 10) * 10;
   echo("<span class=\"yazi\"><tr><td>");
   echo("<label class=\"preField\">Secilen WEB sayfası tasarım işlemleri için</label></td><td align=\"right\">");
   echo("<label class=\"posField\"> $whesap $</label></td></tr></span>");
   $hesap = $whesap;
   if($hesap2 > 0) {
       echo("<span class=\"yazi\"><tr><td>");
       echo("<label class=\"preField\">Orta boy tasarıma eklenen seçenekler</label></td><td align=\"right\">");
       echo("<label class=\"postField\"> $hesap2 $</label></td></tr></span>");
       $hesap += $hesap2;
   }
   if($hesap3 > 0) {
       echo("<span class=\"yazi\"><tr><td>");
       echo("<label class=\"preField\">Gelişmiş tasarıma eklenen seçenekler</label></td><td align=\"right\">");
       echo("<label class=\"postField\"> $hesap3 $</label></td></tr></span>");
       $hesap += $hesap3;
   }
   echo("<span class=\"yazi\"><tr><td>");
   echo("<label class=\"preField\">T O P L A M</label></td><td align=\"right\">");
   echo("<label class=\"postField\"> $hesap $</label></td></tr></span>");
   echo("<input type=\"hidden\" name=\"whesap\" value=\"$whesap\"/>");
   echo("<input type=\"hidden\" name=\"hesap\" value=\"$hesap\"/>");
?>
  </table>
  </fieldset>
  <span class="yazi">
    <label for="submit_" class="preField">Bilgileri göndermek için ONAY tuşuna basın.</label>
<?php
    $say=$say+1;
    echo("<input type=\"button\" class=\"primaryAction\" id=\"submit-\" value=\"ONAY\" onclick=\"javascript:ksecim2($say,9,10)\"/>");
    echo("<input type=\"button\" class=\"\" id=\"sil-\" value=\"İptal\" onclick=\"javascript:bitsin(9)\"/>");
?>
  </span>
<?php
} else {
   // print_r($_REQUEST);
   //echo("<br />");
   $message  = "sayfa= ".$_REQUEST['sayfa']."\n";
   $message .= "wf_s= ".$_REQUEST['wf_s']."\n";
   $message .= "wf_t= ".$_REQUEST['wf_t']."\n";
   if(!empty($_REQUEST['Adi'])) {
       $message .= "Adi= ".$_REQUEST['Adi']."\n";
   }
   if(!empty($_REQUEST['Adres'])) {
       $message .= "Adres= ".$_REQUEST['Adres']."\n";
   }
   if(!empty($_REQUEST['Eposta'])) {
       $message .= "Eposta= ".$_REQUEST['Eposta']."\n";
   }
   if(!empty($_REQUEST['Tel'])) {
       $message .= "Tel= ".$_REQUEST['Tel']."\n";
   }
   if(!empty($_REQUEST['Faks'])) {
       $message .= "Faks= ".$_REQUEST['Faks']."\n";
   }
   if(!empty($_REQUEST['whesap'])) {
       $message .= "whesap=".$_REQUEST['whesap']."\n";
   } else {
       $whesap = 40 * $parite;
       if($s == 2)
            $whesap = $whesap + 150 * $parite;
       if($s > 2)
            $whesap = $whesap + 460 * $parite;
       $whesap = intval($whesap / 10) * 10;
       $message .= "whesap= ".$whesap."\n";
   }
   if(!empty($_REQUEST['hesap'])) {
       $message .= "hesap= ".$_REQUEST['hesap']."\n";
   } else {
       $hesap = $whesap;
       $message .= "hesap= ".$hesap."\n";
   }
   // $bize="eaydalga@gmail.com";
   $bize="aybimbilgx@aybim.com.tr";
   $konu="TasarimWEB";
   $baslik="X-Mailer: PHP". phpversion();
   $message = wordwrap($message);
   // echo($message);
   mail($bize,$konu,$message,$baslik);
?>
   <br>
   <span class="yazi">
     <label for="submit_" class="preField">Bilgileriniz bize gönderildi. Yanıtımızı bekliyiniz.</label>
   </span>
<?php
     echo("<input type=\"button\" class=\"\" id=\"tamam-\" value=\"Bitti\" onclick=\"bitsin(10)\"/>");
   }
?>
</form>
</body>
</html>
