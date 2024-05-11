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
/*
  if($_SERVER['HTTP_REFERER'] != $webhost."bvd/apps/".$userphp) {
       $aa = "Location: ".$webhost."bvd/";
       header($aa);
       exit;
  }
  $usr = $_SESSION['user'];
*/
  $usr="deneme";

function dosyaoku($file)
{
  $rdata = '';
  if(file_exists($file)) {
    $fp = fopen($file,"r");
    $rdata = fread($fp,filesize($file));
    fclose($fp);
    }
  return $rdata;
}

function sdxf_display($rest)
{
$mchunk = unpack("nID/Nflen",$rest);
while(count($mchunk) > 1) {
    $currChunk = $mchunk['ID'];
    $flag = ($mchunk['flen'] >> 24) & 0xff;
    $len = $mchunk['flen'] & 0xffffff;
    if($len) {
         $mdata = substr($rest,7,$len);
         echo("<br>".$currChunk." ");
         for($i=0;$i<8;$i++) {
            $a = ($flag >> (7 - $i)) & 1;
            echo($a);
            }
         echo("(".$len.") (");
         // icerik yazma islemi icin once compress ve encypt kodlarina bak
         if($len > 20)
              echo(substr($mdata,0,20)."...)");
         else echo($mdata.")");

         $rest = substr($rest,7+$len);
         if($rest != '')
              $mchunk = unpack("nID/Nflen",$rest);
         else break;
         }
    else break;
    }
    echo("<br>");
}
?>
<html>
<head>
<meta content="text/html; charset=utf-8" http-equiv="content-type">
<title>SDXF Dosya Yaratma</title>
<link rel="stylesheet" type="text/css" href="main.css" media="screen" />
<script language="JavaScript" src="js/browser.js"> </script>
<script language="JavaScript" src="js/goster.js"> </script>
<script language="JavaScript" src="js/sifreli.js"> </script>
</head>
<body topmargin="0" leftmargin="0" marginheight="0" marginwidth="0">
<?php
   if(isset($_REQUEST['ekleme'])) {
       $ekleme = $_REQUEST['ekleme'];
   } else {
       $ekleme = 'New';
   }
   if(isset($_REQUEST['yiginadi'])) {
       $yiginadi = $_REQUEST['yiginadi'];
   } else {
       $yiginadi = '';
   }
   if(isset($_REQUEST['flag'])) {
       $flag = $_REQUEST['flag'];
   } else {
       $flag = '';
   }
   if(isset($_REQUEST['dkod1'])) { // compression
       $dkod1 = $_REQUEST['dkod1'];
   } else {
       $dkod1 = '';
   }
   if(isset($_REQUEST['dkod2'])) { // encryption
       $dkod2 = $_REQUEST['dkod2'];
   } else {
       $dkod2 = '';
   }
   if(isset($_REQUEST['dkod3'])) { // short chunk
       $dkod3 = $_REQUEST['dkod3'];
   } else {
       $dkod3 = '';
   }
   if(isset($_REQUEST['dkod4'])) { // dizi
       $dkod4 = $_REQUEST['dkod4'];
   } else {
       $dkod4 = '';
   }
   if(isset($_REQUEST['arrct'])) { // cell length
       $arrct = $_REQUEST['arrct'];
   } else {
       $arrct = '';
   }
   if(isset($_REQUEST['arrel'])) { // elements
       $arrel = $_REQUEST['arrel'];
   } else {
       $arrel = '';
   }
   if(isset($_REQUEST['vdata'])) { // number,text,float or key
       $vdata = $_REQUEST['vdata'];
   } else {
       $vdata = '';
   }
   if(isset($_REQUEST['struct'])) { // structures
       $struct = $_REQUEST['struct'];
   } else {
       $struct = '';
   }
   if(isset($_REQUEST['file'])) { // file as data area
       $file = $_REQUEST['file'];
   } else {
       $file = '';
   }
   if(isset($_REQUEST['access'])) { // access rights to a chunk
       $access = $_REQUEST['access'];
   } else {
       $access = '';
   }
   // hata kontrolu buradan baslar
   $hata = '';
   if($vdata == '' && $struct == '' && $file == '') {
       $hata .= "veri yapı ya da dosya girişiniz yok.<br>";
   }
   if(($flag != "01") && ($vdata == '')) {
       $hata .= "Sayı, Karakter ya da Kayan Noktalı için değer girilmemiş<br>";
   }
   $dkodu = ($dkod1 == '')&&($dkod2 == '')&&($dkod3 == '')&&($dkod4 == '');
   if(($dkod2 != '') && $vdata == '') {
       $hata .= "Kripto için anahtar girilmemiş<br>";
   }
   if($yiginadi == '' || $dkodu) {
       $hata .= "bilgi girişiniz eksik dosyanız yaratılmadı.<br>";
   }
   if(strlen($dkod4) && strlen($dkod3)) {
       $hata .= "Hem dizi hem de kısa tanım olamaz.<br>";
   }
   if(strlen($dkod4) && strlen($struct)) {
       $hata .= "Hem yapı hem de kısa tanım olamaz.<br>";
   }
   if(strlen($dkod4) && ($arrct!='') && ($arrel!='')) {
       $hata .= "Dizinin boyutları girilmemiş.<br>";
   }
   if(($flag=="05") && strlen($dkod4)) {
       $hata .= "Hem kayan noktali hem de kısa tanım olamaz.<br>";
   }
   if(strlen($dkod3) && strlen($struct)) {
       $hata .= "Hem yapı hem de dizi olamaz.<br>";
   }
   if(strlen($hata))
       echo($hata);
   else {
       if(!isset($_SESSION['chunk'])) {
           $_SESSION['chunk'] = '';
       }
       $chunk = $_SESSION['chunk'];
       $params[0]=$yiginadi;    // chunkID
       $params[1]=$dkod1;
       $params[2]=$dkod2;
       $params[3]=$vdata;
       if(($flag == "03") || ($flag == "05")) {
           $datalength = 4;
       }
       if($flag == "02") { // bits 
           $datalength = strlen($vdata);
       }
       if(($flag == "04") || ($flag == "06")) { // character or utf-8
           if(strlen($dkod4))
                $datalength = ($arrct << 16) | ($arrel);
           else $datalength = filesize($file);
       }
       if($flag == "01") { // struct
           $datalength = strlen($struct);
       }
       $params[5]=''; 
       if(strlen($vdata) && strlen($dkod3)) {
            $params[5]=$vdata;   // data short value
            $datalength = strlen($vdata);
            }
       $params[4]=$datalength;
       if(strlen($file)) {
            $rdata = dosyaoku($file);
            $params[5]=$rdata;   // data struct array short value
            }
       if(strlen($struct)) {
            $rdata = dosyaoku($struct);
            if($rdata != '')
                 $params[5]=$rdata; // data struct array short value
            else $params[5]=$struct; // data struct array short value
            }
       $params[6]=($flag << 5) |
                  $dkod1 | $dkod2 | $dkod3 | $dkod4;
       // datatype
       $params[7]=$arrct;
       $params[8]=$arrel;
       $params[9]=$access;
       $params[10]=$chunk;

       // giriste hata yok...
       if($ekleme == "New") {
           $params[11]=2; // SDX_NEW
           if(strlen($dkod4))
                $chunk = sdxf_create_array($params);
           else
               switch($flag) {
                 case "02":
                 case "03":
                 case "05": $chunk = sdxf_create_short($params);
                            break;
                 case "04":
                 case "06":
                            if(strlen($dkod3))
                                 $chunk = sdxf_create_short($params);
                            else $chunk = sdxf_create($params);
                 case "01": $chunk = sdxf_create_struct($params);
               }
       } else {
           // append to the existing chunk in $previous
           $params[11]=1; // SDX_OLD
           if($flag == "01")
                $chunk = sdxf_appendstruct($params);
           if($flag != "01")
                $chunk = sdxf_append($params);
       }
       $_SESSION['chunk'] = $chunk;
       sdxf_display($chunk);
?>
       <br>
       <br>
       <div class="yazi">
       <form method="post" action="sakla.php">
<?php
   $tar=date("mdhjs");
   $dname="download/".$usr."-temp".$tar.".sdx";
   echo("Dosya Adi <input type=\"text\" name=\"dosya\" id=\"dosya\" size=\"10\" value=\"$dname\">");
?>
          <input type="submit" value="Sakla">
       </form>
       </div>
<?php
   }
?> 
</body>
</html>
