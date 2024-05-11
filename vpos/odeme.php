
     include 'SanalPos.php';
    $xxx = new SanalPos;


$xxx->setCCNUMBER("".$cc1."".$cc2."".$cc3."".$cc4."");
$xxx->setCCEXP("".$skt_ay."/".$skt_yil."");
$xxx->setCCCVC2("$cv2");
$xxx->setTOTAL("$toplam");
$xxx->setINSTALLMENT("$taksit");
$xxx->setORDERID("$siparisno");
$xxx->setEMAIL("$eposta");


    $return = $xxx->ccpos();

    switch ($return) {
        case 1: 
                echo "OK";
                break;
        case 0:
                echo "NOT OK";
                break;
        case -1:
                echo "Problem var!!!";
                break;
    }
    
