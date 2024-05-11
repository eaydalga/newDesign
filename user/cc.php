<?php
session_start();

/*
* File: CaptchaSecurityImages.php
* Author: Simon Jarvis
* Copyright: 2006 Simon Jarvis
* Date: 03/08/06
* Link: http://www.white-hat-web-design.co.uk/articles/php-captcha.php
* 
* This program is free software; you can redistribute it and/or 
* modify it under the terms of the GNU General Public License 
* as published by the Free Software Foundation; either version 2 
* of the License, or (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful, 
* but WITHOUT ANY WARRANTY; without even the implied warranty of 
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
* GNU General Public License for more details: 
* http://www.gnu.org/licenses/gpl.html
*
*/

class CaptchaSecurityImages {

	function generateCode($characters) {
		// $possible = '23456709bcdfg0jkmnp3rstv67yz'; 
		$possible = '2345670923456090765356746734'; 
		$code = '';
		$i = 0;
		while ($i < $characters) { 
			$code .= substr($possible, mt_rand(0, strlen($possible)-1), 1);
			$i++;
		}
		return $code;
	}

	function CaptchaSecurityImages($width='120',$height='40',$characters='6') {
		$code = $this->generateCode($characters);
		/* font size will be 75% of the image height */
		$font_size = $height * 0.75;
		$image = ImageCreate($width, $height);
		/* set the colours */
		$background_color = imagecolorallocate($image, 255, 255, 255);
		$text_color = imagecolorallocate($image, 20, 40, 100);
		$noise_color = imagecolorallocate($image, 100, 120, 180);

                $white = ImageColorAllocate($image, 255, 255, 255);
                $black = ImageColorAllocate($image, 0, 0, 0);
                $grey = ImageColorAllocate($image, 204, 204, 204);

                ImageFill($image, 0, 0, $grey); 

		/* generate random lines in background */
                ImageRectangle($image,0,0,$width-1,$height-1,$white);
		/* generate random dots in background */
		for( $i=0; $i<($width*$height)/10; $i++ ) {
	 		imagefilledellipse($image, mt_rand(0,$width), mt_rand(0,$height), 1, 1, $noise_color);
	 	}

                $font = imageloadfont("etc/chowfun.gdf");
                ImageString($image, $font, 5, 2, $code, $text_color);
		/* output captcha image to browser */
		imagejpeg($image);
		imagedestroy($image);
		$_SESSION['security_code'] = $code;
	}

}

$width = '100';
$height = '30';
$characters = '5';

header('Content-Type: image/jpeg');
$captcha = new CaptchaSecurityImages($width,$height,$characters);
?>
