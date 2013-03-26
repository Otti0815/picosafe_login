<html>
<head></head>
<body onload="document.otp.givenOtp.focus();">

<form action="" method="post" name="otp">
<table><tr><td>OTP:</td><td><input type="text" name="givenOtp"></td></tr>
<!--<tr><td>AES-Key:</td><td><input type="text" name="aesKey"></td></tr>-->
<tr><td></td><td><input type="submit" value="Go"></td></tr>
</table>
</form>
</body>

<?PHP


include "class.picosafelogin.php";

$pico = new PicosafeLogin("Europe/Berlin");

if($_POST["givenOtp"]!="")
{
  $givenOtp = $_POST["givenOtp"];
  $aesKey = "soopu9goBoay9vongooth2ooLu8keed1";
  //$datablock = "eeng5jo7th";  

  $result = $pico->ParseOTP($givenOtp,$aesKey);

  echo "Nummer: " . $result['counter'] . "<br>";

  echo "Datenblock: " . $result['datablock']." <br>";

  echo "Timestamp: " . $result['timestamp']." <br>";

  echo "Datetime: " . date("d.m.Y H:i:s",$result['timestamp']) . "<br>";
}

?>
