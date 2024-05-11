<?php
class SimpleEnum{
	
	var $aList;
	
	function SimpleEnum($aItems){
		if(is_array($aItems)){
			$this->aList = array_flip($aItems);
		}else{
			die('Only allow array to construct SimpleEnum.');
		}
	}

	//Return the value of specific enum
	function is($item){
		if(array_key_exists($item, $this->aList)){
			return $this->aList[$item];
		}else{
			/*
				Using 'die' because I would like to have programmer's attention
				on referring a wrong item.
			*/			
			die('No such item in SimpleEnum: '.$item);	
		}		
	}
	
	//Return the total length of enum
	function length(){
		return count($this->aList);
	}
	
	//Return all the enum array values
	function values(){
		return $this->aList;
	}
	
	//Print all enum array values
	function printValues(){
		print '<pre>';print_r($this->aList); print '</pre>';
	}
}

?>