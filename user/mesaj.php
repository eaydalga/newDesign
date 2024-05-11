<?php
  require("include/conf.inc");
  // db baglantısı gerekli
  $baglan = mysql_connect($dbhost, $dbuser, $dbpasswd)
            or die("Bağlantı kurulamadı" . mysql_error());
  mysql_select_db($dbname, $baglan);
  require("include/maint.inc");
  require("include/okuma.inc");
  function mail_gonder($email,$konu,$message)
  {
      if(strlen($message) > 10) {
          $konu="Mesaj Gönderme";
          $baslik="X-Mailer: PHP". phpversion();
          $message = wordwrap($message);
          my_note_add($usrno,$message);
          // mail($email,$konu,$message,$baslik);
      }
  }
  function increase($no)
  {
    global $mystatus;
    $sorgu = "SELECT sirano from sabitler where sira='$no'";
    list($sno) = read_row($sorgu,$no);
    $sno = $sno + 1;
    $sorgu = "UPDATE sabitler set sirano='$sno' where sira='$no'";
    $mystatus = update_row($sorgu);
    return($sno);
  }
?>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
</head>
<body>
<?php
   if (isset($_REQUEST['usrno'])) {
       $usrno=$_REQUEST['usrno'];
   } else {
       $usrno="";
   }
   $win="sec5";
   $mystatus = "girişte hata var";
   if(!empty($usrno)) {
       if (isset($_REQUEST['tono'])) {
           $tono=$_REQUEST['tono'];
           $tocc=$_REQUEST['tocc'];
           $attach=$_REQUEST['attach'];
           $subject=$_REQUEST['subject'];
           $body=$_REQUEST['body'];
           if(strlen($body) > 200) {
              $body = substr($body,0,200);
           }
           $uygkodu = $mesajno;
           /* gmsgno arttirilacak */
           $gmsgno = increase('gmsgno');
       
           /* mesaj eklerini yaz */
           if(!empty($attach)) {
              $cc = split($attach,",");
              $ekno = 1;
              foreach $cc as $ekfile {
                  $sorgu = "INSERT into mesajek (msgNo, ekNo, ekBilgi) values('$gmsgno', '$ekno', '$ekfile')";
                  $mystatus = insert_row($sorgu);
                  $ekno = $ekno + 1;
              }
           }

           $aa = split($tono," ");
           foreach ($aa as $to) {
              // $to icin istek gonder
              $uygmsg="Sayin $to $usrno sizine mesaj gönderdi.\n";
              my_note_add($to,$uygmsg);
              $uygmsg .= $body;
              my_request_add($usrno,$to,$uygkodu,$uygmsg);
              /* once mesaji yaz */
              $sorgu = "INSERT into mesaj (toNo,msgNo,fromNo,uygMsg,uygEkler,rmsgkod) values ('$to','$gmsgno', '$usrno', '$body', '$attach', '1')";
              $mystatus = insert_row($sorgu);
           }
           if(!empty($tocc)) {
               $bb = split($tocc," ");
               foreach ($bb as $tcc) {
                   $uygmsg="Sayin $tcc $usrno bilginiz için mesaj size de gönderdi.\n";
                   my_note_add($tcc,$uygmsg);
                   $uygmsg .= $body;
                   my_request_add($usrno,$tcc,$uygkodu,$uygmsg);
                   /* mesaji yaz */
                   $sorgu = "INSERT into mesaj (toNo,msgNo,fromNo,uygMsg,uygEkler,rmsgkod) values ('$to','$gmsgno', '$usrno', '$body', '$attach', '1')";
                   $mystatus = insert_row($sorgu);
               }
           }
           $uygmsg="$tono için mesaj gönderdiniz.";
           my_note_add($usrno,$uygmsg);
           if(!empty($tocc)) {
               $uygmsg="$tocc için mesaj gönderdiniz.";
               my_note_add($usrno,$uygmsg);
           }

           $sor = "SELECT fullname from user where usrNo = '$usrno'";
           $mystatus="";
           list($fullname) = read_row($sor,$usrno);
           foreach ($aa as $to) {
              // $tono icin email adresi al
              $sor = "SELECT email,fullname from user where usrNo = '$to'";
              $mystatus="";
              list($email,$fname) = read_row($sor,$to);
              if($mystatus != "") $email = "";
              if($email != "") {
                   $message="Sayin $fname, $fullname sizi bir mesaj gönderdi.";
                   mail_gonder($email,$subject,$message);
              }
              $uygmsg="Sayin $to $usrno sizine eposta gönderdi.";
              my_note_add($to,$uygmsg);
           }
       }
?>
<br>
<br>
<form>
   <span class="yyazi">
     <label for="submit_" class="yyazi">Mesajınız iletildi...</label>
<?php
     echo("<input type=\"button\" class=\"gyazi\" value=\"Kapat\" onclick=\"clear13('$win');\"/>");
?>
   </span>
</form>
<?php
       } else {
           if($mystatus != "") {
               echo("<h3>UYE BILGISI HATASI</h3>");
               echo($mystatus);
           }
       }
   } else {
      /* Insert your code for showing an error message here */
      echo("yok...");
      exit;
   }
?>
</body>
</html>
