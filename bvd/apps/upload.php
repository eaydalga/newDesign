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
  require("include/sdxf.inc");
  // db baglantısı gerekli
  $baglan = mysql_connect($dbhost, $dbuser, $dbpasswd)
            or die("Bağlantı kurulamadı" . mysql_error());
  mysql_select_db($dbname, $baglan);
/*
  if($_SERVER['HTTP_REFERER'] != $webhost."bvd/apps/".$userphp) {
       $aa = "Location: ".$webhost."bvd/";
       header($aa);
       exit;
  }
  $usr = $_SESSION['user'];
*/
  $usr = "deneme";
  function upload($user)
  {
    global $target;
    $target = "upload/";
    $target = $target . $user . "-" . basename( $_FILES['uploaded']['name']) ;

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
        return false;
    }
    return true;
  }
  /* ************** create input html ********************************** */
?>
<html>
<head>
<meta content="text/html; charset=utf-8" http-equiv="content-type">
<title>SDXF Dosya Yukleme</title>
<link rel="stylesheet" type="text/css" href="main.css" media="screen" />
<script language="JavaScript" src="js/browser.js"> </script>
<script language="JavaScript" src="js/goster.js"> </script>
<script language="JavaScript" src="js/sifreli.js"> </script>
</head>
<body topmargin="0" leftmargin="0" marginheight="0" marginwidth="0">
<?php
 if(isset($_FILES['uploaded'])) {
     if(upload($usr) == true) {
        if(move_uploaded_file($_FILES['uploaded']['tmp_name'], $target)) {
           echo "$target yüklendi<br />";
           $uygmsg="$usr $target dosyasını yükledi.";
           /* **********************************************************
           **  veri tabanina kayit yaz
           ** ******************************************************** */
           my_note_add($usr,$uygmsg);
           /* **********************************************************
           **  ilgililere mesaj gonder
           ** ******************************************************** */
        } else {
           echo "Üzgünüz, dosyanızı yüklerken bir hata oluştu.";
        }
     }
     /* echo("<form>");
     echo("  <input type=\"button\" onclick=\"javascript:parent.usrclear13('sec5')\" value=\"Kapat\">");
     echo("</form>"); */
 }
?> 
</body>
</html>
