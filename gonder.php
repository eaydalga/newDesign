<?php
   session_start();
?>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
</head>
<body>
<?php
   if (isset($_SESSION['security_code'])) {
       $ass=strtoupper($_SESSION['security_code']);
   } else {
       $ass="yok";
   }
   if (isset($_POST['security_code'])) {
       $pss=strtoupper($_POST['security_code']);
   } else {
       $pss="";
   }
   // echo ("(".$ass . ") (" . $pss. ")<br>");
   if(($ass == $pss) && (!empty($_SESSION['security_code'])) ) {
       // Insert you code for processing the form here
       $message="";
       if(!empty($_REQUEST['fADSOYAD'])) {
           $message .= "AdSoyad= ".$_REQUEST['fADSOYAD']."\n";
       }
       if(!empty($_REQUEST['fADRES'])) {
           $message .= "Adres= ".$_REQUEST['fADRES']."\n";
       }
       if(!empty($_REQUEST['fILCE'])) {
           $message .= "Ilce= ".$_REQUEST['fILCE']."\n";
       }
       if(!empty($_REQUEST['fBAG'])) {
           $message .= "Baglanti Tel= ".$_REQUEST['fBAG']."\n";
       }
       if(!empty($_REQUEST['fTELEFON'])) {
           $message .= "Irtibat= ".$_REQUEST['fTELEFON']."\n";
       }
       if(strlen($message) > 10) {
           $bize="aybimbilgx@aybim.com.tr";
           $konu="ttnet-ABONE";
           $baslik="X-Mailer: PHP". phpversion();
           $message = wordwrap($message);
           // echo($message);
           mail($bize,$konu,$message,$baslik);
       } else {
          echo("message length:" . strlen($message));
       }
?>
<br>
<br>
<form>
   <span class="oneField">
     <label for="submit_" class="yazi">Mesajınız bize iletildi. Lütfen aramamızı bekleyiniz.</label>
     <input type="button" class="" id="tamam-" value="Ended" onclick="kapat();"/>
   </span>
</form>
<?php
   } else {
      // Insert your code for showing an error message here
      exit;
   }
?>
</body>
</html>
