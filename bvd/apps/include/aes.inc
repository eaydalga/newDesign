<?php

/* ***************************************************************************
** This PHP code is PHP's AES encryption.
**
** MCRYPT_RIJNDAEL_128 -> 16 byte
** MCRYPT_RIJNDAEL_192 -> 24 byte
** MCRYPT_RIJNDAEL_256 -> 32 byte
**
** If you want to be AES compliant,always choose MCRYPT_RIJNDAEL_128.
**
** So the first step is to create the cipher object: *********************** */

function aes_encrypt($cleartext,$key128)
  {
  $cipher = mcrypt_module_open(MCRYPT_RIJNDAEL_128, '', MCRYPT_MODE_CBC, '');

  // We're using CBC mode(cipher-block chaining).Block cipher modes are detailed
  // here: http://en.wikipedia.org/wiki/Block_cipher_modes_of_operation

  // PHP code to verify our Initialization Vector (IV) size:
  $iv_size = mcrypt_enc_get_iv_size($cipher);

  $iv = mcrypt_create_iv($iv_size, MCRYPT_RAND);

  if (mcrypt_generic_init($cipher, $key128, $iv) != -1)
    {
	$cipherText = mcrypt_generic($cipher,$cleartext );
	mcrypt_generic_deinit($cipher);
    }
  else $chipherText = -1;
  mcrypt_module_close($td);
  return $chipherText;
  }

function aes_decrypt($chipherText,$key128)
  {
  $cipher = mcrypt_module_open(MCRYPT_RIJNDAEL_128, '', MCRYPT_MODE_CBC, '');
  $iv_size = mcrypt_enc_get_iv_size($cipher);
  $iv = mcrypt_create_iv($iv_size, MCRYPT_RAND);
  if (mcrypt_generic_init($cipher, $key128, $iv) != -1)
    {
	$cipherText = mdecrypt_generic($cipher,$chiperText);
	mcrypt_generic_deinit($cipher);
	printf("128-bit encrypted result:\n%s\n\n",bin2hex($cipherText));
    }
  else $chipherText = -1;
  mcrypt_module_close($td);
  return $clearText;
  }
?>
