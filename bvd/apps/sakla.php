<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<?php
  session_start();
  require("include/conf.inc");
  require("include/okuma.inc");
  require("include/aes.inc");
  require("include/sdxf.inc");
  // db baglantısı gerekli
  $baglan = mysql_connect($dbhost, $dbuser, $dbpasswd)
            or die("Bağlantı kurulamadı" . mysql_error());
  mysql_select_db($dbname, $baglan);
  if($_SERVER['HTTP_REFERER'] != $webhost."bvd/apps/crtsdxf.php") {
       $aa = "Location: ".$webhost."bvd/apps/index.php";
       header($aa);
       exit;
  }
  $usr = $_SESSION['user'];

function dosyayaz($dd,$veri)
{
  if(!file_exists($dd)) {
    $fp = fopen($dd,"w");
    fwrite($fp,$veri,strlen($veri));
    fclose($fp);
    echo($dd." dosyasına<br>".strlen($veri). " bayt saklandı<br>");
    }
}
?>
<html>
<head>
<meta content="text/html; charset=utf-8" http-equiv="content-type">
<title>SDXF Dosya Saklama</title>
<link rel="stylesheet" type="text/css" href="main.css" media="screen" />
<script language="JavaScript" src="js/browser.js"> </script>
<script language="JavaScript" src="js/goster.js"> </script>
<script language="JavaScript" src="js/sifreli.js"> </script>
</head>
<body topmargin="0" leftmargin="0" marginheight="0" marginwidth="0">
<?php
   if(isset($_REQUEST['dosya'])) {
       $dosya = $_REQUEST['dosya'];
       $b = strpos($dosya,'download/');
       if(!($b >= 0))
           $dosya= "download/".$dosya;
       if(!isset($_SESSION['chunk'])) {
           $_SESSION['chunk'] = '';
       }
       $chunk = $_SESSION['chunk'];
       dosyayaz($dosya,$chunk);
   } else {
       echo("dosya adi girilmemiş");
   }
?> 
</body>
</html>
