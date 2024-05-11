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
  $target = "upload/";
  $target = $target . $usr . "-" . basename( $_FILES['uploaded']['name']) ;

  $uploaded_size = $_FILES['uploaded']['size'] ;
  $uploaded_type = $_FILES['uploaded']['type'] ;
  $ok=1;

  //This is our size condition
  if ($uploaded_size > 350000) {
      echo "Dosyanız çok büyük.<br>";
      $ok=0;
      }

  //This is our limit file type condition
  if ($uploaded_type =="text/php") {
      echo "PHP dosyalari olamaz<br>";
      $ok=0;
      }

  //Here we check that $ok was not set to 0 by an error
  if ($ok==0) {
      echo "Üzgünüz dosyanız yüklenemedi";
      echo("<form>");
      echo("  <input type=\"button\" onclick=\"javascript:parent.clear13('sec5')\" value=\"Kapat\">");
      echo("</form>");
  } else {
      //If everything is ok we try to upload it
      if(move_uploaded_file($_FILES['uploaded']['tmp_name'], $target)) {
          echo "$target yüklendi<br />";
          $uygmsg="$usr $target dosyasını yükledi.";
          my_note_add($usr,$uygmsg);
      } else {
          echo "Üzgünüz, dosyanızı yüklerken bir hata oluştu.";
      }
      echo("<form>");
      echo("  <input type=\"button\" onclick=\"javascript:parent.clear13('sec5')\" value=\"Kapat\">");
      echo("</form>");
  }
?> 
