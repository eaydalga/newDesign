<?php
  session_start();
  require("include/conf.inc");
  /* referer aranacak */
  if($_SERVER['HTTP_REFERER'] != $webhost.$userphp) {
       $aa = "Location: ".$webhost;
       header($aa);
       exit; 
  }

?>
<div id="uyewin">
<table class="yyazi">
<?php
$dir = $resimdir;
$usr = $_SESSION['user'];
if ($handle = opendir($dir)) {

    /* This is the correct way to loop over the directory. */
    while (false !== ($file = readdir($handle))) {
        if(strstr($file,$usr)) {
           $xx = $dir . $file;
           echo("<tr><td>><a href=\"javascript:resimekle('fkisi','$xx','ad9');\" class=\"nav\"><img src=\"$dir$file\" width=\"30\" height=\"30\"></a>");
           echo("</td><td>$file</td></tr>");
        }
    }
    closedir($handle);
    echo("</table>");
    echo("<p><form><span class=\"rpic\">Resimler bitti...&nbsp;<input type=\"button\" onclick=\"javascript:clear13('sec31')\" class=\"nav\" value=\"KAPAT\"></span></form>");

}
?>
</div>
