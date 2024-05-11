<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>Enum example</title>
</head>
<body>
<?php
include('SimpleEnum.class.php');
$array = array('spring', 'summer', 'autumn', 'winter');

#Create Enum
$eSeason = new SimpleEnum($array);

#Using printValues(): Print in <pre> format for debug purpose
$eSeason->printValues();	

#Using values(): This is the same as $eSeason->printValues();
print '<pre>';	print_r($eSeason->values()); print '</pre>';		

#Using length() to get the number of items in enum
print $eSeason->length().'<br/>';	//Print 4

#Using is() to get the enum
$result = $eSeason->is('spring');

#Example of using Enum
if($result == $eSeason->is('spring')){
	print 'Result is spring'.'<br/>';	//Print
}

if($result == $eSeason->is('summer')){
	print 'Result is summer'.'<br/>';	//Not print
}

?>
</body>
</html>