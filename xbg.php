<?php

function FloodFill($im, $x, $y) {
    $rgb = imagecolorat($im, $x, $y);
    $r = ($rgb >> 16) & 0xFF;
    $g = ($rgb >> 8) & 0xFF;
    $b = $rgb & 0xFF;

    $counter=0;
    $counter2=0;
    if($r >= 245){ $counter++;}
    if($g >= 245){ $counter++;}
    if($b >= 245){ $counter++;}
    if($r >= 240){ $counter2++;}
    if($g >= 240){ $counter2++;}
    if($b >= 240){ $counter2++;}

    if($counter >= 1 && $counter2 == 3){
        $background = imagecolorallocate($im, 180, 0, 255);
        imagesetpixel($im, $x, $y, $background);

        FloodFill ($im, $x, $y+1);
        FloodFill ($im, $x+1, $y);
        FloodFill ($im, $x, $y-1);
    }
}

/*
// Draw border

// Draw a border
function drawBorder($im, $color, $thickness = 1)
{
    $x1 = 0;
    $y1 = 0;
    $x2 = ImageSX($im) - 1;
    $y2 = ImageSY($im) - 1;

    for($i = 0; $i < $thickness; $i++)
    {
        ImageRectangle($im, $x1++, $y1++, $x2--, $y2--, $color);
    }
}
*/

//$src = $_GET["src"];
//if (!strlen($src))
  // $src = "/var/www/sss.apperceptions.net/public/sites/default/files/sss-logo-1.jpg";
  $src = "http://studentsuccessskills.com/sites/default/files/sss-logo-1.jpg";
$im = imagecreatefromjpeg($src);

//$border = imagecolorallocate($im, 180, 0, 255);
//drawBorder($im, $border, 1);

$rgb = imagecolorat($im, 0, 0);
FloodFill($im, 0, 0);
$color = imagecolorallocate($im, 180, 0, 255);
imagecolortransparent($im, $color);

header("Content-type: image/png");
imagepng($im);
imagedestroy($im);
?>