<html>
<head>
</head>
<body>
<?php
$data ="123456789010101010";
$aa = pack("nCN",12345,0x40,18);
print($aa);
$aa = $aa . $data;
$chunk = unpack("nID/Cflag/NLength/",$aa);
echo("<br>".count($chunk)."<br>");
foreach($chunk as $cc)
    echo($cc." ");
print_r($chunk);
echo(substr($aa,7,$chunk['Length']));
?>
</body>
</html>
