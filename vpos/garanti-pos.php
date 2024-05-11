Arkadaslar,
Sanal Pos kodlarını paylaşmaya karar verdim ancak 3DSecure için entegrasyon isteyen arkadaşlar olursa domain başına lisanslı ve zend ile şifrelenmiş olarak taksitli (opsiyonlu) olarak 85 USD+KDV karşılığı alabilirler. 3DSecure için lütfen sadece PM atınız.

Garanti Sanal Pos Kodları aşağıdadır. Kodları kopyalayıp garantisanalpos.php olarak kaydederek WHMCS nin kurulu olduğu dizinde modules/gateway klasörü içine ftp ile gönderiniz. Daha sonra ise whmcs nin admin bölümüne girerek ödeme şeklini ekleyip, Mağaza No, api id ve sifrenizi yazıp kullanmaya başlayabilirsiniz. Unutmayın YTL icin lütfen 840 kullanınız, bankanız dolar kuru icin onay vermez.

Görüş ve önerileriniz için lütfen PM atınız.

PHP Kodu:
<?php
 
# GARANTIBANK Credit Card Payment Gateway Module
 
$GATEWAYMODULE["garantisanalposname"]="garantisanalpos";
$GATEWAYMODULE["garantisanalposvisiblename"]="Garanti Sanal Pos";
$GATEWAYMODULE["garantisanalpostype"]="CC";
 
function garantisanalpos_activate() {
    defineGatewayField("garantisanalpos","text","merchantid","","Merchant ID","20","");
    defineGatewayField("garantisanalpos","text","merchantpw","","Merchant Password","20","");
    defineGatewayField("garantisanalpos","text","merchantnumber","","Merchant Number","20","");
    defineGatewayField("garantisanalpos","text","isokod","","Isokod 949 YTL - 840 USD","10","");
    defineGatewayField("garantisanalpos","yesno","testmode","","Test Mode","","");
}
 
function garantisanalpos_link($params) {
$code='
<form method="post" action="'.$params["systemurl"].'/creditcard.php" name="paymentfrm">
<input type="hidden" name="invoiceid" value="'.$params['invoiceid'].'">
<input type="submit" value="'.$params["langpaynow"].'">
</form>
';
return $code;
}
 
function garantisanalpos_capture($params) {
    if ($params["testmode"]=="on") {
        $gateway_url = "

DOSYALARA ULAŞMAK İÇİN LÜTFEN ÜYE OLUNUZ YADA ÜYE İSENİZ GİRİŞ YAPINIZ";
    } else {
        $gateway_url = "

DOSYALARA ULAŞMAK İÇİN LÜTFEN ÜYE OLUNUZ YADA ÜYE İSENİZ GİRİŞ YAPINIZ";
    }
    $name=$params['merchantid']; //GarantiSanalPos Merchant ID
    $password=$params['merchantpw']; //GarantiSanalPos Merchant Password
    $clientid=$params['merchantnumber']; //GarantiSanalPos Merchant Number
    $isokod=$params['isokod']; // GarantiSanalPos isokod 949 YTL 840 USD
    $ip=GetHostByName($REMOTE_ADDR); //Client IP address
    $type="Auth"; //Auth - PreAuth
    $email=$params["clientdetails"]["email"]; //Client Email
    $oid=$params['invoiceid']; //Order ID ,     
    $ccno=$params['cardnum']; //Client Credit Card Number
    $ccay=substr($params['cardexp'],0,2); //Credit Card Exp. Month
    $ccyil=substr($params['cardexp'],2,2); //Credit Card Exp. Year
    $tutar=$params['amount']; //Total Amount Exp. 11.99
    $cv2=$params['cccvv']; //Credit Card CVV2 number 
 
 
    //Client Details
    $fname=$params["clientdetails"]["firstname"];
    $lname=$params["clientdetails"]["lastname"];
    $firma=$params["clientdetails"]["companyname"];
    $adres1=$params["clientdetails"]["address1"];
    $adres2=$params["clientdetails"]["address2"];
    $ilce=$params["clientdetails"]["city"];
    $sehir=$params["clientdetails"]["state"];
    $postkod=$params["clientdetails"]["postcode"];
    $ulke=$params["clientdetails"]["country"];
    $telno=$params["clientdetails"]["phonenumber"];
 
 
// XML request template 
$request= "DATA=<?xml version=\"1.0\" encoding=\"ISO-8859-9\"?>
<CC5Request>
<Name>{NAME}</Name>
<Password>{PASSWORD}</Password>
<ClientId>{CLIENTID}</ClientId>
<Currency>{ISOKOD}</Currency>
<Mode>P</Mode>
<IPAddress>{IP}</IPAddress>
<Email>{EMAIL}</Email>
<OrderId>{OID}</OrderId>
<GroupId></GroupId>
<TransId></TransId>
<UserId></UserId>
<Type>{TYPE}</Type>
<Number>{CCNO}</Number>
<Expires>{CCAY}{CCYIL}</Expires>
<Cvv2Val>{CV2}</Cvv2Val>
<Total>{TUTAR}</Total>
<BillTo>
    <Name>{FNAME} {LNAME}</Name>
    <Street1>{ADRES1}</Street1>
    <Street2>{ADRES2}</Street2>
    <Street3>{IP}</Street3>
    <City>{ILCE}</City>
    <StateProv>{SEHIR}</StateProv>
    <PostalCode>{POSTKOD}</PostalCode>
    <Country>{ULKE}</Country>
    <Company>{FIRMA}</Company>
    <TelVoice>{TELNO}</TelVoice>
</BillTo>
<ShipTo>
    <Name>{FNAME} {LNAME}</Name>
    <Street1>{ADRES1}</Street1>
    <Street2>{ADRES2}</Street2>
    <Street3></Street3>
    <City>{ILCE}</City>
    <StateProv>{SEHIR}</StateProv>
    <PostalCode>{POSTKOD}</PostalCode>
    <Country>{ULKE}</Country>
</ShipTo>
<Extra></Extra>
</CC5Request>
";
 
//parameters writing XML 
 
$request=str_replace("{NAME}",$name,$request);
$request=str_replace("{PASSWORD}",$password,$request);
$request=str_replace("{CLIENTID}",$clientid,$request);
$request=str_replace("{ISOKOD}",$isokod,$request);
$request=str_replace("{TYPE}",$type,$request);
$request=str_replace("{IP}",$ip,$request);
$request=str_replace("{OID}",$oid,$request);
$request=str_replace("{EMAIL}",$email,$request);
$request=str_replace("{CCNO}",$ccno,$request);
$request=str_replace("{CCAY}",$ccay,$request);
$request=str_replace("{CCYIL}",$ccyil,$request);
$request=str_replace("{CV2}",$cv2,$request);
$request=str_replace("{TUTAR}",$tutar,$request);
 
    //parameters writing XML BILLTO and SHIPTO
$request=str_replace("{FNAME}",$fname,$request);
$request=str_replace("{LNAME}",$lname,$request);
$request=str_replace("{ADRES1}",$adres1,$request);
$request=str_replace("{ADRES2}",$adres2,$request);
$request=str_replace("{ILCE}",$ilce,$request);
$request=str_replace("{SEHIR}",$sehir,$request);
$request=str_replace("{POSTKOD}",$postkod,$request);
$request=str_replace("{ULKE}",$ulke,$request);
$request=str_replace("{TELNO}",$telno,$request);
$request=str_replace("{FIRMA}",$firma,$request);
 
$ch = curl_init(); // initialize curl handle
curl_setopt($ch, CURLOPT_URL,$gateway_url); // set url to post to
curl_setopt($ch, CURLOPT_SSL_VERIFYHOST,1);
curl_setopt($ch, CURLOPT_SSL_VERIFYPEER,0);
curl_setopt($ch, CURLOPT_RETURNTRANSFER,1); // return into a variable
curl_setopt($ch, CURLOPT_TIMEOUT, 90); // times out after 90s
curl_setopt($ch, CURLOPT_POSTFIELDS, $request); // add POST fields
$result = curl_exec($ch); // run the whole process
 
if (curl_errno($ch)) {
    $error = curl_error($ch);
    logTransaction("Garanti Sanal Pos","Error => $error","Error");
    sendMessage("Credit Card Payment Failed",$params['invoiceid']);
    $result="error";
    return $result;
} else {
    curl_close($ch);
    $Response ="";
    $OrderId ="";
    $AuthCode ="";
    $ProcReturnCode ="";
    $ErrMsg ="";
    $HOSTMSG ="";
    $response_tag="Response";
    $posf = strpos ( $result, ("<" . $response_tag . ">") );
    $posl = strpos ( $result, ("</" . $response_tag . ">") ) ;
    $posf = $posf+ strlen($response_tag) +2 ;
    $Response = substr ( $result, $posf, $posl - $posf) ;
    $response_tag="OrderId";
    $posf = strpos ( $result, ("<" . $response_tag . ">") );
    $posl = strpos ( $result, ("</" . $response_tag . ">") ) ;
    $posf = $posf+ strlen($response_tag) +2 ;
    $OrderId = substr ( $result, $posf , $posl - $posf ) ;
    $response_tag="AuthCode";
    $posf = strpos ( $result, "<" . $response_tag . ">" );
    $posl = strpos ( $result, "</" . $response_tag . ">" ) ;
    $posf = $posf+ strlen($response_tag) +2 ;
    $AuthCode = substr ( $result, $posf , $posl - $posf ) ;
    $response_tag="ProcReturnCode";
    $posf = strpos ( $result, "<" . $response_tag . ">" );
    $posl = strpos ( $result, "</" . $response_tag . ">" ) ;
    $posf = $posf+ strlen($response_tag) +2 ;
    $ProcReturnCode = substr ( $result, $posf , $posl - $posf ) ;
    $response_tag="ErrMsg";
    $posf = strpos ( $result, "<" . $response_tag . ">" );
    $posl = strpos ( $result, "</" . $response_tag . ">" ) ;
    $posf = $posf+ strlen($response_tag) +2 ;
    $ErrMsg = substr ( $result, $posf , $posl - $posf ) ;
    $debugdata = "Action => Auth\nClient => ".$params['clientdetails']['firstname']." ".$params['clientdetails']['lastname']."\nResponse => $Response\nOrderId => $OrderId\nAuthCode => $AuthCode\nProcReturnCode => $ProcReturnCode\nErrMsg => $ErrMsg";
 
    if ( $Response === "Approved") {
        addInvoicePayment($params['invoiceid'],$transid,"","","garantisanalpos","on");
        logTransaction("Garanti Sanal Pos",$debugdata,"Successful");
        sendMessage("Credit Card Payment Confirmation",$params['invoiceid']);
        $result="success";
        return $result;
    } else {
        logTransaction("Garanti Sanal Pos",$debugdata,"Declined");
        sendMessage("Credit Card Payment Failed",$params['invoiceid']);
        $result="declined";
        return $result;
        }
 
    }
 
}
 
?>
