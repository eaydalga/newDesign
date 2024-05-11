<?
if($TaksitSayisi == 1) $TaksitSayisi = "0";
$client = new SoapClient("https://www.e-tahsildar.com.tr/NetProvWS/NetProvWS.asmx?WSDL", array('trace' => 1)); 
 
$info = array(
  "pOrgNo" => "-----------",
  "pFirmNo" => "-----------", 
  "pTermNo" => "-----------", 
  "pCardNo" => "-----------",
  "pCvv2No" => "-----------",
  "pExpiry" => "-----------", // 200901 gibi
  "pAmount" => "1", //tutar virgullu ise virgulu . yapmamiz lazim = str_replace(".", ",", $ToplamMiktar), 
  "pTaksit" => "0"
  );
 
try 
{
 $client->__soapCall("GetAuthorizationInstTrn", array($info)) ;
} catch (SoapFault $exception) {
 echo $exception;
}
$res = $client->__getLastResponse();

// Result degerini parse edelim
$searchres = preg_match("@<GetAuthorizationInstTrnResult>(.*)</GetAuthorizationInstTrnResult>@ism", $res, $matches);
if ($searchres && strlen($matches[1]))
 $Result = $matches[1];
else 
 $Result= "";

$response = substr($Result, 0, 2);
$ErrMsg = substr($ErrMsg, 2, strlen($ErrMsg));
  
if($response == "00") { // hatali degil
 $error__ = 0;
 $IslemSonucu = "Onayland&yacute;";
 $HataKodu = "";
} else {
 $error__ = 1;
 $IslemSonucu = "HATA!";
 $HataKodu = "Hata Kodu: ". $response . " " .$ErrMsg;
}
?> 
