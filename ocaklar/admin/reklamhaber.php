<?php
require("../include/php_file_tree.inc");
if(isset($_REQUEST['s'])) {
    $dir = "../" . $_REQUEST['s'];
    $ret=php_file_tree($dir,'','');
    echo($ret);
}
?>
