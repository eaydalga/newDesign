<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
</head>
<body>
<?php
if(isset($_REQUEST['bweb'])) {
    $istek=$_REQUEST['bweb'];
} else {
    $istek=0;
}
?>
<br>
<br>
<form method="post" id="wf_Enquiry">
<?php
if($istek) {
  if ($istek == 1) {
     $subject = "1- BVDO nasil kullanabiliriz";
     $baslik = "Bağımsız Veri Değişim Ortamı'nı (BVDO) Nasıl Kullanabiliriz";
  }
  if ($istek == 2) {
     $subject = "2- OFISICI istedigim sudur";
     $baslik = "Ofisiçi yazılım (OFISICI) yazılımından istenenler";
  }
  if ($istek == 3) {
     $subject = "3- SWY Benim gereksinimim";
     $baslik = "Sürekli WEB Yayımı (SWY) yazılımı gereksinimi";
  }
  if ($istek == 4) {
     $subject = "4- PG benim ihtiyacim";
     $baslik = "Panorama ihtiyacı";
  }
  if ($istek == 5) {
     $subject = "5- Internet TV";
     $baslik = "Internet TV Kurulumu";
  }
  echo("<h4>$baslik</h4>");
?>
  <table>
  <tr><td valign="top">
  <span class="yazi">
    <label for="wf_Myname" class="preField">Ad/Ünvan</label>
  </span>
  </td><td>
  <input type="text" id="wf_Myname" name="wf_Myname" value="" size="" class="validate-alphanum required" title="Adınızı ya da firmanızın ünvanını bu alana yazın."/>
  </td><td valign="top">
  <span class="reqMark">*</span>
  </td></tr>
  <tr><td valign="top">
  <span class="yazi">
    <label for="wf_Email" class="preField">E-posta</label>
  </span>
  </td><td valign="top">
  <input type="text" id="wf_Email" name="wf_Email" value="" size="" class="validate-email required" title="Size ulaşabilmemiz için e-posta adresinizi doğru girin."/>
<?php
  echo("<input type=\"hidden\" id=\"wf_subject\" name=\"wf_subject\" value=\"". $subject."\" size=\"\" class=\"\"/>");
?>
  </td><td valign="top">
  <span class="reqMark">*</span>
  </td></tr>
  </table>
  <fieldset id="wf_Aciklama" class="">
    <legend>Açıklama</legend>
  <table>
  <tr><td valign="top">
    <span class="yazi">
      <label for="wf_aciklama" class="preField">Dileğiniz</label>
    </span>
  </td><td valign="top">
    <textarea cols="45" rows="5" id="wf_aciklama" name="wf_aciklama" class="required" title="Varsa bize iletmek istediğiniz diğer isteklerinizi bu alana yazabilirsiniz."></textarea>
  </td><td valign="top">
  </td></tr>
  </table>
  </fieldset>
  <div class="actions">
  <table>
  <tr><td valign="top">
  </td><td valign="top">
    <input type="button" class="primaryAction" id="submit-" value="Gönder" onclick="javascript:isteme2(0)"/>
  </td><td valign="top">
    <input type="button" class="" value="Iptal" onclick="javasript:bitsin(11)"/>
  </td></tr>
  </table>
  </div>
<?php
} else {
    if((isset($_REQUEST['wf_Myname'])) &&
       (isset($_REQUEST['wf_Email'])) &&
       (isset($_REQUEST['wf_subject'])) &&
       (isset($_REQUEST['wf_aciklama']))) {
       $message = "";
       $message .= "Adi= ".$_REQUEST['wf_Myname']."\n";
       $message .= "E-posta= ".$_REQUEST['wf_Email']."\n";
       $message .= "Konu= ".$_REQUEST['wf_subject']."\n";
       $message .= "Aciklama= ".$_REQUEST['wf_aciklama']."\n";
       // $bize="eaydalga@gmail.com";
       $bize="aybimbilgx@aybim.com.tr";
       $konu="IStekler";
       $baslik="X-Mailer: PHP". phpversion();
       $message = wordwrap($message);
       // echo($message);
       mail($bize,$konu,$message,$baslik);
    }
?>
   <br />
   <br />
   <span class="yazi">
     <label for="submit_" class="preField">Bilgileriniz bize gönderildi. Yanıtımızı bekliyiniz.</label>
   </span>
<?php
    echo("<input type=\"button\" class=\"\" value=\"Bitti\" onclick=\"javasript:bitsin(12)\"/>");
}
?>
</form>
</body>
</html>
