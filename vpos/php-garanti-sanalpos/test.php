<?php
	require_once("SanalPos.php");

	$sp = new SanalPos();
	$sp->CreditCard		= '5454699546548272';
	$sp->Expires		= '06/10';
	$sp->CVV2			= '354';
	$sp->Total			= '5.50';
	
	$cc_result = $sp->send();

	if($cc_result !== FALSE){
		if($cc_result['CC5Response']['ProcReturnCode'] == '00'){
			// İşlem Başarılı
		}
		else{
			print($sp->getError($cc_result['CC5Response']['ProcReturnCode']));
		}
	}
	else{
		print('Sorun oluştu!');
	}
?>