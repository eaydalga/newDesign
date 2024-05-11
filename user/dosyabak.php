<?php
  require("include/conf.inc");
  require("include/file_tree.inc");
  /* referer aranacak */
  if($_SERVER['HTTP_REFERER'] != $webhost.$userphp) {
       $aa = "Location: ".$webhost;
       header($aa);
       exit; 
  }

?>
<div class=\"yyazi\">
<?php
$dir = "./";
$ret=php_file_tree($dir,'','');
echo($ret);
?>
</div>
