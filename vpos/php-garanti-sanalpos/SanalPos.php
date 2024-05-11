<?php
	class SanalPos{
		const Url			= 'https://ccpos.garanti.com.tr/servlet/cc5ApiServer';
		const ClientID		= '';
		const Username		= '';
		const Password		= '';
		
		private $Rop		= array('Rop', 'ConfigFile', 'PostData', 'error_code');
		private $ConfigFile	= 'garanti-pos.xml';
		private $PostData	= NULL;

		private $CreditCard;			# Kredi Kartı Numarası
		private $Expires;				# Son Kullanma Tarihi (05/08)
		private $CVV2;					# Güvenlik Numarası
		private $Total;					# Toplam Tutar
		private $Installment;			# Taksit Sayısı
		private $CurrencyCode	= 949;	# Para Birimi (YTL)
		private $Live			= 'T';	# Demo Mode
		private $Name;					# Kullanıcı İsim/Soyisim

		private $error_code	= array(
			'00'	=> 'İşlem başarılı.',
			'01'	=> 'Bankanızdan provizyon alınız.',
			'02'	=> 'Bankanızdan provizyon alınız. (visa)',
			'05'	=> 'İşlem onaylanamadı.',
			'06'	=> 'İşleminiz kabul edilmedi.',
			'12'	=> 'Geçersiz işlem.',
			'13'	=> 'Geçersiz tutar bilgisi.',
			'14'	=> 'Geçersiz kart numarası.',
			'16'	=> 'Yetersiz bakiye.',
			'17'	=> 'İşlem iptal edildi.',
			'38'	=> 'Şifre giriş limiti aşıldı.',
			'54'	=> 'Vadesi dolmuş kart.',
			'61'	=> 'Limit aşımı! İşlem gerçekleştirilemiyor.',
			'63'	=> 'Bu işlemi yapmaya yetkili değilsiniz.',
			'65'	=> 'Günlük işlem limiti dolmuş.',
			'76'	=> 'Şifre doğrulanamıyor.',
			'82'	=> 'Hatalı CVV kodu.',
			'83'	=> 'Şifre doğrulanamıyor.',
			'86'	=> 'Şifre doğrulanamıyor.',
			'96'	=> 'Sistem hatası.'
		);
		
		private function __get($key){
			if(array_key_exists($key, get_class_vars(__CLASS__))){
				return $this->{$key};
			}
			
			trigger_error('<strong>'. $key .'</strong> property cannot be found in <strong>'. __CLASS__ .'</strong> class', E_USER_ERROR);
		}
		
		private function __set($key, $value){
			if($this->is_readonly($key) === FALSE){
				if(array_key_exists($key, get_class_vars(__CLASS__))){
					$this->{$key} = $value;
				}
				else{
					trigger_error('<strong>'. $key .'</strong> property cannot be found in '. __CLASS__ .' class', E_USER_ERROR);
				}
			}
			else{
				trigger_error('<strong>'. $key .'</strong> is readyonly!', E_USER_ERROR);
			}
		}

		private function is_readonly($key){
			return in_array($key, $this->Rop) ? TRUE : FALSE;
		}

		public function __construct(){
			if(file_exists($this->ConfigFile)){
				$this->PostData = file_get_contents($this->ConfigFile);

			}
			else{
				trigger_error('<strong>'. $this->ConfigFile .'</strong> is not found!', E_USER_ERROR);
			}
		}
		
		private function this_callback($matches){
			eval('$var = $this->'. $matches[1] .';');
				return $var;
		}

		private function self_callback($matches){
			eval('$var = self::'. $matches[1] .';');
				return $var;
		}

		private function &xml_parser($data){
			if(!class_exists('XmlToArray')){
				require_once('XmlToArray.class.php');
			}

			$xml = new XmlToArray($data);
				return $xml->createArray();
		}

		public function getError($code){
			if(array_key_exists($code, $this->error_code)){
				return $this->error_code[$code];
			}

			return 'Bilinmeyen hata oluştu. Hata kodu : <strong>'. $code .'</strong><br />';
		}
		
		public function send(){
			if(!empty($this->PostData)){
				$this->PostData = preg_replace_callback("/\{:([a-zA-Z0-9]+):\}/",  array($this, 'this_callback'), $this->PostData);
				$this->PostData = preg_replace_callback('/\[:([a-zA-Z0-9_]+):\]/', array($this, 'self_callback'), $this->PostData);
				
				
				$ch =	curl_init();
						curl_setopt($ch, CURLOPT_URL, self::Url);
						curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, 0);
						curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, 0);
						curl_setopt($ch, CURLOPT_HTTPHEADER, array('Content-Type' => 'text/xml'));
						curl_setopt($ch, CURLOPT_FOLLOWLOCATION, TRUE);
						curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);
						curl_setopt($ch, CURLOPT_POST, TRUE);
						curl_setopt($ch, CURLOPT_POSTFIELDS, 'DATA='. urlencode($this->PostData));

				$result = curl_exec($ch);

				if(curl_errno($ch)){
					return FALSE;
				}
				else{
					$data =& $this->xml_parser($result);
						return $data;
				//	print '<pre>'. print_r($data, TRUE) .'</pre>';
				}

				curl_close($ch);
			}

			return FALSE;
		}
	}
?>