<?php
$gdfonts = array(
    '8x13iso', '9x15iso', 'andale12', 'atommicclock', 'bmreceipt',
    'chowfun', 'courier8', 'dimurph', 'georgia8', 'proggyclean',
    'proggysquare', 'systemex', 'terminal6', 'trisk'
);

//
//
//
if( !isset($image_info) || !isset($image_text) )
{
    exit;
}

//
//
//
$sz = @getimagesize('./images/'.$image_info['image']);
if( !$sz )
{
    exit;
}
$image_w = $sz[0];
$image_h = $sz[1];

//
//
//
@header('Content-type: image/png');

$im = @imagecreatefromgif('./images/'.$image_info['image']);
$rgb = ( isset($image_info['color']) ? $image_info['color'] : array(255, 255, 255) );
$bgColor = imagecolorallocate($im, $rgb[0], $rgb[1], $rgb[2]);

for( $i = 0; $i < count($image_text); $i++ )
{
    if( !is_numeric($image_text[$i]['font']) )
    {
        $font = 1;
    }
    else if( $image_text[$i]['font'] < 0 )
    {
        $font = $image_text[$i]['font'] * -1;
    }
    else
    {
        if( !($font = @imageloadfont('./gd_fonts/'.$gdfonts[$image_text[$i]['font']].'.gdf')) )
        {
            $font = 1;
        }
    }
    $rgb = $image_text[$i]['color'];
    $fgColor = imagecolorallocate($im, $rgb[0], $rgb[1], $rgb[2]);
    imagestring($im, $font, $image_text[$i]['x'], $image_text[$i]['y'], $image_text[$i]['text'], $fgColor);
}

imagepng($im);
imagedestroy($im);
?> 
