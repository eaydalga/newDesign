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
       if(!empty($_REQUEST['wf_Name1'])) {
           $message .= "Ad Unvan= ".$_REQUEST['wf_Name1']."\n";
       }
       if(!empty($_REQUEST['wf_Email'])) {
           $message .= "E-posta= ".$_REQUEST['wf_Email']."\n";
       }
       if(!empty($_REQUEST['wf_Telephone'])) {
           $message .= "Telefon= ".$_REQUEST['wf_Telephone']."\n";
       }
       if(!empty($_REQUEST['wf_YourQuery3'])) {
           $message .= "Istek= ".$_REQUEST['wf_YourQuery3']."\n";
       }
       if(!empty($_REQUEST['wf_How'])) {
           $message .= "Nasil Eriselim= ".$_REQUEST['wf_How']."\n";
       }
       if(strlen($message) > 10) {
           $bize="aybimbilgx@aybim.com.tr";
           $konu="IletisimWEB";
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
     <label for="submit_" class="yazi">Mesajınız bize iletildi. Lütfen yanıtımızı bekleyiniz.</label>
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
