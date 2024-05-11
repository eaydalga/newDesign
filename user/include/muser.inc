<?php
require("../include/conf.inc");
if(!isset($login_successful))
    $login_successful = false;

// user id and passwd
if (isset($_SERVER['PHP_AUTH_USER']) && isset($_SERVER['PHP_AUTH_PW'])){

    $usr = $_SERVER['PHP_AUTH_USER'];
    $pwd = $_SERVER['PHP_AUTH_PW'];

    if ($usr == $webuser && $webpwd == crypt($pwd,"aA")){
        $login_successful = true;
    }
}

// login ok
if (!$login_successful) {
    // 401 headers;
    header('WWW-Authenticate: Basic realm="Sifre girisi"');
    header('HTTP/1.0 401 Unauthorized');
    print "Login failed!\n";
    exit;
}
?>
