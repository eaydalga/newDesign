<?php
  session_start();
  require("include/conf.inc");
  require("include/okuma.inc");
  // db baglantısı gerekli
  $baglan = mysql_connect($dbhost, $dbuser, $dbpasswd)
            or die("Bağlantı kurulamadı" . mysql_error());
  mysql_select_db($dbname, $baglan);

  if($_SERVER['HTTP_REFERER'] != $webhost.$userphp) {
       $aa = "Location: ".$webhost;
       header($aa);
       exit; 
  }
  $usr = $_SESSION['user'];
  $target = $resimdir;
  $target = $target . $usr . "-" . basename( $_FILES['resim']['name']) ;
  $uploaded_size = $_FILES['resim']['size'] ;
  $uploaded_type = $_FILES['resim']['type'] ;
  $ok=1;

  //This is our size condition
  if ($uploaded_size > 350000) {
      echo "Your file is too large.<br>";
      $ok=0;
      }

  //This is our limit file type condition
  if ($uploaded_type =="text/php") {
      echo "No PHP files<br>";
      $ok=0;
      }

  //Here we check that $ok was not set to 0 by an error
  if ($ok==0) {
      echo "Uzgunuz dosyanız yüklenemedi";
  } else {
      //If everything is ok we try to upload it
      if(move_uploaded_file($_FILES['resim']['tmp_name'], $target)) {
          echo "$target yüklendi<br />";
          $uygmsg="$usr $target resim dosyasini yükledi.";
          my_note_add($usr,$uygmsg);
      } else {
          echo "Uzgünüz, dosyanızı yüklerken bir sorun oldu.";
      }
  }
  echo("<form>");
  echo("  <input type=\"button\" onclick=\"javascript:parent.clear13('sec6')\" value=\"Kapat\">");
  echo("</form>");
?> 
