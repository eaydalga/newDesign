<?php
   session_start();
   require("include/conf.inc");
   require("include/user.inc");
   require("include/maint.inc");
   require("include/bilgi.inc");
   require("include/okuma.inc");
   require("include/online.inc");
   function usr_adi($fusr)
   {
      $sorgu = "SELECT fullName from user where usrNo = '$fusr'";
      list($fname) = read_row($sorgu,$fusr);
      return $fname;
   }
   if(isset($usr) && isset($pwd)) {
     $ret = false;
     $mystat="Uye/Şifre Hatalı";
     if(strlen($usr) && strlen($pwd)) {
       if($pwd == $pwd2) {
           $ret = sifreupd($usr, $pwd);
       } else {
           $ret = sifrebak($usr, $pwd);
           if($pwd == "")
              $pwd = "bos";
       }
     }
     if($ret == false) {
           session_start();
           // Şifre/Kullanıcı Hatalı
           $_SESSION['sifrehata']=$mystat;
           $aa = "Location: ".$webhost;
           header($aa);
           exit;
     } else {
           $_SESSION['sifrehata']="";
?>
<html>
<head>
<title>Uye Sayfasi</title>
<link rel="stylesheet" type="text/css" href="main.css" media="screen" />
<link rel="stylesheet" type="text/css" href="default.css" media="screen" />
<script language="JavaScript" src="js/browser.js"> </script>
<script language="JavaScript" src="js/email.js"> </script>
<script language="JavaScript" src="js/date.js"> </script>
<script language="JavaScript" src="js/float.js"> </script>
<script language="JavaScript" src="js/baslik.js"> </script>
<script language="JavaScript" src="js/XHConn.js"> </script>
<script language="JavaScript" src="js/sifreli.js"> </script>
<script>
//<![CDATA[
  function init() {
     document.getElementById('fek').onsubmit=function() {
         //'sec5' is the name of the iframe
         document.getElementById('sec5').style.display="inline";
         document.getElementById('fek').target = 'sec5'; 
     }
     document.getElementById('fresim').onsubmit=function() {
         //'sec6' is the name of the iframe
         document.getElementById('sec6').style.display="inline";
         document.getElementById('fresim').target = 'sec6'; 
     }
     document.getElementById('fkisi').onsubmit=function() {
         //'sec7' is the name of the iframe
         document.getElementById('sec7').style.display="inline";
         document.getElementById('fkisi').target = 'sec7'; 
     }
     document.getElementById('gir8').onsubmit=function() {
         document.getElementById('sec9').style.display="inline";
         document.getElementById('gir8').target = 'sec9'; 
     }
     document.getElementById('gir9').onsubmit=function() {
         document.getElementById('sec9').style.display="inline";
         document.getElementById('gir9').target = 'sec9'; 
     }
     document.getElementById('gir10').onsubmit=function() {
         document.getElementById('sec10').style.display="inline";
         document.getElementById('gir10').target = 'sec10'; 
     }
  }
  window.onload=init;
//]]>
</script>
</head>
<body>
<?php
       $baglan = mysql_connect($dbhost, $dbuser, $dbpasswd)
                 or die("Bağlantı kurulamadı" . mysql_error());
       mysql_select_db($dbname, $baglan);

      /* burada kullanicinin kodu ile kendisine ilişkin bilgiler alinir *
       *   user tablosundan usr ile usrNo, fullName, email           */

       $sorgu = "SELECT email, fullName from user where usrNo = '$usr'";
       list($email, $fullname) = read_row($sorgu,$usr);
?>
<div id="usttaraf">
</div>
<div id="mybanner">
   <div class="ul">
       <table cellpadding="0" cellspacing="0" class="yyazi">
           <tr>
               <td><div class="li"><a href="javascript:sayfa('1');">Üye Sayfası</a></div></td>
               <td><div class="li"><a href="javascript:sayfa('2');">Yakın Arkadaş</a></div></td>
               <td><div class="li"><a href="javascript:sayfa('3');">Kişisel Bilgiler</a></div></td>
               <td><div class="li"><a href="javascript:sayfa('4');">Düzenleme</a></td>
               <td><div class="li"><a href="javascript:sayfa('5');">Mesajlar</a></div>
               <td><div class="li"><a href="javascript:logout();">Çıkış</a></div>
           </td></tr>
       </table>
   </div>
</div>
<table cellpadding="0" cellspacing="0" class="yyazi" width="970">
<tr><td valign="top">
<?php
     $fresim=resimal($usr);
     $size = getimagesize($fresim);
     $w = 130 / $size[1] * $size[0];
     echo "<center><img src=\"$fresim\" width=\"$w\" height=\"130\" alt=\"$fullname\"><br />";
     echo("$usr: $fullname</center>");
    /* ************************************************************* *
     *     burada menu olacak                                        *
     * ***************************************************************
     * Uygulamalar *                                   * son durum   *
     *             * log bilgisi                       * istekler    *
     *             * haberler                          *             *
     * Arkadaslar  *                                   *             *
     *             *                                   *             *
     * ***************************************************************/
?>
    <div id="ysoltaraf">
    <div class="yyazi">
<?php
       /* kullanıcı uygulamaları : usrNo uygKodu uygSecno */
       /* uygulama başlıkları: uygKodu uygSecno uygAdi */
        $sorgu = "SELECT count(usrNo) from useruyg where usrNo='$usr'";
        $result = mysql_query($sorgu);
        if($result) {
            list($fsayi) = mysql_fetch_row($result);
            mysql_free_result($result);
        } else {
            $mstat = mysql_error();
            $fsayi = 0;
            echo("<font color=\"#8e9ccb\"><h3>Uygulamalar: (hata: $mstat)</h3></font>");
        }
        if($fsayi > 0) {
            echo("<font color=\"#8e9ccb\"><h3>Uygulamalar: ($fsayi tane)</h3></font>");

            $sorgu = "SELECT uygKodu,uygAdi from useruyg,uygbaslik where usrNo='$usr' and uygKodu.useruyg=uygKodu.uygbaslik";
            $result = mysql_query($sorgu);
            if($result) {
                echo "<table class=\"yyazi\" border=\"0\">\n";
                $i = 0;
                while ($frow = mysql_fetch_row($result)) {
                    $fno = $frow[0];
                    $fadi = $frow[1];
                    echo "\t<tr>\n";
                    /* burada uygulama resmini kullanilir */
                    echo "\t\t<td>$fadi</td>\n";
                    /* echo "\t\t<td>$ad</td>\n"; */
                    echo "\t</tr>\n";
                    $uygulama_no[$i] = $fno;
                    $uygulama_adi[$i] = $fadi;
                    $i++;
                }
                echo "</table>\n";
            }
        }
?>   
       <br />
       <!-- ARKADAS ARAMA BURADAN YAPILMALI -->
<?php
       /* yakin dostlar icin friends tablosundan bu kullaniciya iliskin
        * arkadaslar alinir.
        *   friends tablosundan alinanlar
        *     firendNo bu kisilerin adlarini ve resimlerini de al        */
        $sorgu = "SELECT count(friendNo) from friends where usrNo='$usr'";
        $result = mysql_query($sorgu);
        if($result) {
            list($fsayi) = mysql_fetch_row($result);
            mysql_free_result($result);
            echo("<font color=\"#8e9ccb\"><h3>Arkadaşlar: ($fsayi tane)</h3></font>");
        } else {
            $mstat = mysql_error();
            $fsayi = 0;
            echo("<font color=\"#8e9ccb\"><h3>Arkadaşlar: (hata: $mstat)</h3></font>");
        }
        if($fsayi > 0) {
            $sorgu = "SELECT friendNo from friends where usrNo='$usr'";
            $result = mysql_query($sorgu);
            echo "<table class=\"yyazi\" border=\"0\">\n";
            $i = 0;
            while (list($fno) = mysql_fetch_row($result)) {
                echo "\t<tr>\n";
                $sor = "SELECT fullName from user where usrNo='$fno'";
                list($fadi) = read_row($sor,$fno);
                $sorgu = "SELECT bilgiAlani from personel where usrNo='$fno' and bilgiKodu='$resimno'";
                list($fresim) = read_row($sorgu,$fno);
                /* burada arkadasin resmi kullanilir */
                if(empty($fresim)) {
                   $fresim = $defaultpic;
                }
                $sz = getimagesize($fresim);
                $w = 30 / $sz[1] * $sz[0];
                echo "\t\t<td><img src=\"$fresim\" width=\"$w\" height=\"30\" alt=\"$fadi\"></td>\n";
                echo "\t\t<td>$fadi</td>\n";
                echo "\t</tr>\n";
                $arkadas_no[$i] = $fno;
                $arkadas_adi[$i] = $fadi;
                $i++;
                if($i > 10) {
                    echo("...");
                    break;
                }
            }
            echo "</table>\n";
        }

    /* - onlise users - */
    echo '<br />Online Kişi Sayısı: ' . getOnlineUsers() . '<br />';
?>
    </div>
    </div>
    </td>
    <td valign="top">
    <div id="ortataraf">
       <!-- menu basliklari burada olmali -->
       <div id="header">
<?php
       require("include/kisi1.inc");
       require("include/kisi2.inc");
       require("include/kisi3.inc");
       require("include/kisi4.inc");
       require("include/kisi5.inc");
?>
       </div>
    </div>
   </td>
   <td valign="top" id="sagtaraf">
   <br />
<?php
    /* ************************************************************
     * istekler toNo istekNo fromNo uygKodu uygMsg
     *    fromNo -- > user --> fullName
     *    uygKodu -- > uygbaslik --> uygAdi
     *    uygMsg                                                  
     * ************************************************************ */
    $sorgu="SELECT istekNo,fromNo,uygKodu,uygMsg from istekler where toNo='$usr' and ikod='1'";
    $result = mysql_query($sorgu);
    if($result) {
        $fullname = usr_adi($usr);
        while ($line = mysql_fetch_array($result, MYSQL_NUM)) {
            $istekno = $line[0];
            $fromno = $line[1];
            $fromname = usr_adi($fromno);
            $uygkodu = $line[2];
            $uygmsg = $line[3];
            /* fullname fromname uygmsg uygekler */
            istek_yaz($fullname, $fromname, $istekno, $uygmsg);
        }
        @mysql_free_result($result);
    }
    /* rekramlara bak
     * bugun >= basTar and bugun <= bitTar ise reklami yazdir
     * ANA = ana sayfa reklamı
     */
    $bugun=date("Ymd", time());
    $sorgu="SELECT reklamNo,dosyaAdi from reklam where reklamNo like 'USR%' and bitTar >= '$bugun' and rkod = '1'";
    my_reklamyaz($sorgu,$reklamdir);
?>
    </td></tr>
</table>

<div id="kisi9" style="display: none">
</div>
<script>sayfa('1');</script>
</body>
</html>
<?php
       }
   } else {
       // echo("nasıl geldi buraya?");
       $aa = "Location: ".$webhost;
       header($aa);
   }
?>
