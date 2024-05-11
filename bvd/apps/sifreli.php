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
   if(isset($_REQUEST['user']))
       $usr = $_REQUEST['user'];
   if(isset($_REQUEST['passwd']))
       $pwd = $_REQUEST['passwd'];
   if(isset($_REQUEST['passwd2']))
        $pwd2 = $_REQUEST['passwd2'];
   else $pwd2 = '';
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
           $aa = "Location: ".$webhost."bvd/";
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
<body topmargin=0 leftmargin=0 marginheight=0 marginwidth=0>
<?php
       $baglan = mysql_connect($dbhost, $dbuser, $dbpasswd)
                 or die("Bağlantı kurulamadı" . mysql_error());
       mysql_select_db($dbname, $baglan);

      /* burada kullanicinin kodu ile kendisine ilişkin bilgiler alinir *
       *   user tablosundan usr ile usrNo, fullName, email           */

       $sorgu = "SELECT email, fullName from user where usrNo = '$usr'";
       list($email, $fullname) = read_row($sorgu,$usr);
?>
<div id="headertop1">
   <div id="headertop">
<?php
    include("inc/baslik.inc");
?>
    <table cellpadding="0" cellspacing="0" border="0">
    <tr><td valign="top" width="140">
          <div id="soltaraf">
<!--
            <div id="masterdiv">
                <div class="menutitle"><a href="javascript:history.back()" class="vnav">Önceki Sayfa</a></div>
            </div>
 -->
    <div class="yazi">
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
            echo("<font color=\"#8e9ccb\"><h3>Uygulamalar: ($fsayi)</h3></font>");

            $sorgu = "SELECT uygKodu,uygAdi from useruyg,uygbaslik where usrNo='$usr' and uygKodu.useruyg=uygKodu.uygbaslik";
            $result = mysql_query($sorgu);
            if($result) {
                echo "<table class=\"yazi\" border=\"0\">\n";
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
                echo("<br />");
            }
        }
?>   
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
            echo("<font color=\"#8e9ccb\"><h3>Gruptakiler: ($fsayi)</h3></font>");
        } else {
            $mstat = mysql_error();
            $fsayi = 0;
            echo("<font color=\"#8e9ccb\"><h3>Gruptakiler: (hata: $mstat)</h3></font>");
        }
        if($fsayi > 0) {
            $sorgu = "SELECT friendNo from friends where usrNo='$usr'";
            $result = mysql_query($sorgu);
            echo "<table class=\"yazi\" border=\"0\">\n";
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
                if(!file_exists($fresim)) {
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
    echo '<br />Online Üye Sayısı: ' . getOnlineUsers() . '<br />';
?>
          </div>

         </td><td valign="top" width="800">
              <div id="header">
                <div class="ul">
                    <table cellpadding="2" cellspacing="0" class="yyazi">
                        <tr>
                            <td><div class="li"><a href="javascript:sayfa('1');" class="gyazi">&nbsp;Üye Sayfası&nbsp;</a></div></td>
                            <td><div class="li"><a href="javascript:sayfa('2');" class="gyazi">&nbsp;Grup Üyeleri&nbsp;</a></div></td>
                            <td><div class="li"><a href="javascript:sayfa('3');" class="gyazi">&nbsp;Kişisel Bilgiler&nbsp;</a></div></td>
                            <td><div class="li"><a href="javascript:sayfa('4');" class="gyazi">&nbsp;Düzenleme&nbsp;</a></td>
                            <td><div class="li"><a href="javascript:sayfa('5');" class="gyazi">&nbsp;Mesajlar&nbsp;</a></div>
                            <td><div class="li"><a href="javascript:logout();" class="gyazi">&nbsp;Çıkış&nbsp;</a></div>
                        </td></tr>
                    </table>
                </div>
              </div>
              <div id="header">
<?php
       require("include/kisi1.inc");
       require("include/kisi2.inc");
       require("include/kisi3.inc");
       require("include/kisi4.inc");
       require("include/kisi5.inc");
?>
                 <div id="baslik">
                   <table width="100%">
                   <tr><td valign="top">
                     <!-- reklamlar varsa buraya alinir -->
<?php
                         /* rekramlara bak
                          * bugun >= basTar and bugun <= bitTar ise reklami yazdir
                          * ANA = ana sayfa reklamı
                          */
                         $bugun=date("Ymd", time());
                         $sorgu="SELECT reklamNo,dosyaAdi from reklam where reklamNo like '%ANA%' and basTar >= $bugun and $bugun <= bitTar";
                         my_reklamyaz($sorgu,$reklamdir);
?>
                   </td><td valign="top" width="280">
                     <!-- ---- sag taraf ------------- -->
                     <div class="borderright">
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
                                           @mysql_close($baglan);
?>
                       </div>
                   </td></tr></table>
                     </div>
                     </div>
<!-- ------------sag taraf bitti ---------------- -->
                </div>
            </div>
    </div>
</div>
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
