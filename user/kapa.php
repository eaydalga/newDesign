<?php
   session_destroy();
   require("include/conf.inc");
   $aa = "Location: ".$webhost;
   header($aa);
?>
