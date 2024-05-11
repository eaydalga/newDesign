<?php
  session_start();
  require("../include/conf.inc");
   // db baglantısı gerekli
   $baglan = mysql_connect($dbhost, $dbuser, $dbpasswd)
             or die("Bağlantı kurulamadı" . mysql_error());
   mysql_select_db($dbname, $baglan);
  require("../include/maint.inc");
  require("../include/okuma.inc");
?>
<html>
<title>reklam onayi</title>
<head>
<link rel="stylesheet" type="text/css" href="../main.css" media="screen" />
<script language="JavaScript" src="../js/browser.js"> </script>
</head>
<body>
<table cellpadding="0" cellspacing="0" border="1" class="yyazi">
<!-- img src="img/ribi.jpg" -->
<tr><td valign="top">
<div id="ysoltaraf">
</div>
</td><td valign="top">
<div id="ortataraf">
<?php
    $_SESSION['reklamhata']="reklam no hatalı";
    echo("<b>REKLAM ONAYI</b><br />");
    echo("<b>İşlem sonucu</b><br />");
    $i = 0;
    foreach($_REQUEST as $key => $value) {
        if(strstr($key,"kod") && $value == '1') {
            $ss = substr($key,3);
        }
        if($key == "no$ss") {
	    echo '-->no' . $ss . ": " . $value . "<br/>";
            $sorgu="UPDATE reklam set rkod='1' where reklamNo='$value'";
            $bhata = update_row($sorgu);
            if(strstr($bhata,"güncel")) {
                $omessage = "<br />$value nolu reklam onaylandı<br />\n";
                echo($omessage);
                $_SESSION['reklamhata']="";
            } else {
                $_SESSION['reklamhata']="VT hatası: $bhata";
                break;
            }
        }
        $i++;
    } 
?>
<br />
</div>
</td><td valign="bottom">
<div id="ortataraf">
    <div class="yyazi">
       <form>
         <input type="button" value="Ana Sayfaya" class="gyazi" onclick="location.href='index.php';">
       </form>
    </div>
</div>
</td></tr>
</table>
</body>
</html>
