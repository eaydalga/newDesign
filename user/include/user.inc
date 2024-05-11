<?php
// user id and passwd
if (isset($_REQUEST['kullanici']) && isset($_REQUEST['sifre'])){
    $usr = $_REQUEST['kullanici'];
    $pwd = $_REQUEST['sifre'];
    if (isset($_REQUEST['sifre2'])) {
        $pwd2 = $_REQUEST['sifre2'];
    } else {
        $pwd2 = "";
    }
}

function dbbaglan()
{
    global $dbhost, $dbname, $dbuser, $dbpasswd;
    // db baglantısı gerekli
    $baglan = mysql_connect($dbhost, $dbuser, $dbpasswd) or
                    die("Bağlantı kurulamadı" . mysql_error());
    @mysql_select_db($dbname, $baglan);
    return $baglan;
}

function sifrebak($usr, $pwd) {
    global $mystat;
    $login_successful = false;

    // db baglantısı gerekli
    $baglan = dbbaglan();
    $sorgu = "SELECT email,passwd FROM user where usrNo='$usr' and usrkod='1'";
    $bsonuc = mysql_query($sorgu);
    if($bsonuc) {
         list($email,$passwd) = mysql_fetch_row($bsonuc);
         $mystat = "Üye/Şifre yanlış";
         if ($passwd != "bos") {
             if ($passwd == crypt($pwd,"aA")){
                 $mystat = "";
                 $login_successful = true;
             }
         } else {
             $mystat = "";
             $login_successful = true;
         }
         @mysql_freeresult($bsonuc);
    } else {
         $mystat = mysql_error();
    }
    @mysql_close($baglan);
    return $login_successful;
}

function sifreupd($usr, $pwd) {
    global $mystat;
    $login_successful = false;

    $baglan = dbbaglan();
    $passwd = crypt($pwd,"aA");
    $sorgu = "UPDATE user set passwd='$passwd' where usrNo='$usr'";
    $bsonuc = mysql_query($sorgu);
    if($bsonuc) {
         $mystat = "Şifre Değişti";
         $login_successful = true;
         @mysql_freeresult($bsonuc);
    } else {
         $mystat = mysql_error();
    }
    @mysql_close($baglan);
    return $login_successful;
}
?>
