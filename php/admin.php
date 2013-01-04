<?
include "class.picosafelogin.php";
session_start();

if(!isset($_GET['bgimage']))$_GET['bgimage']="";
if(!isset($_GET['action']))$_GET['action']="";
if(!isset($_POST['login']))$_POST['login']="";
if(!isset($_POST['create']))$_POST['create']="";
if(!isset($_SESSION["db_database"]))$_SESSION["db_database"]="";
if(!isset($_SESSION["db_user"]))$_SESSION["db_user"]="";
if(!isset($_SESSION["db_password"]))$_SESSION["db_password"]="";


$error="";
// Show image before an echo
if($_GET['bgimage']!='')
	BgImage();


echo "
<script type=\"text/javascript\">
	function GenerateKey() {
		var el = document.getElementById(\"aes\");	
		var chars = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';	
		var key = '';
		for(var i=0;i<32;i++) {
			var cindex = Math.floor(Math.random() * chars.length);
			key += chars.substring(cindex, cindex + 1);
		}
		el.value = key;
	}
	function GenerateDatablock() {
		var el = document.getElementById(\"datablock\");	
		var chars = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';	
		var key = '';
		for(var i=0;i<10;i++) {
			var cindex = Math.floor(Math.random() * chars.length);
			key += chars.substring(cindex, cindex + 1);
		}
		el.value = key;
	}

</script>
<style>
	table.list tr.header {
		font-weight: 600;	
	}

	table.list td {
		border-spacing: 0px;
		border: 1px solid black;
		padding: 3px 6px;
	}

	a.menu {
		-moz-box-shadow:inset 0px 1px 0px 0px #97c4fe;
		-webkit-box-shadow:inset 0px 1px 0px 0px #97c4fe;
		box-shadow:inset 0px 1px 0px 0px #97c4fe;
		background-color:#3d94f6;
		-moz-border-radius:6px;
		-webkit-border-radius:6px;
		border-radius:6px;
		border:1px solid #337fed;
		display:inline-block;
		color:#ffffff;
		font-family:Trebuchet MS;
		font-size:15px;
		font-weight:bold;
		padding:3px 13px;
		text-decoration:none;
		text-shadow:1px 1px 0px #1570cd;
	}

	a {
		text-decoration: none;
		color: #3D94F6;
	}

	a:hover {
		text-decoration: underline;
	}

	#footer {
		display: block;
		position:absolute;
		right: 10px;
		bottom:7px;
	}

</style>";

echo "<div style=\"height: 45px\"><img src=\"admin.php?bgimage=1\"></div>";
echo "<a class=\"menu\" href=\"admin.php\">List</a>&nbsp;";
echo "<a class=\"menu\" href=\"admin.php?action=create\">Create</a>&nbsp;";
echo "<a class=\"menu\" href=\"admin.php?action=logout\">Logout</a>&nbsp;";
echo "<hr>";

/* login and database connection */

try{
		if($_SESSION["db_database"]=="")
			$database ="none"; else $database = $_SESSION["db_database"];
		$server   = 'mysql:dbname='.$database.';host=localhost; port=3333';
		$user     = $_SESSION["db_user"];
		$password = $_SESSION["db_password"];
		$options  = array (PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION);

		$pdo      = new PDO($server, $user, $password, $options);
}
catch(PDOException $ex){
		$pdo = "";

		echo "<form action=\"admin.php\" method=\"post\">";
		echo "<table><tr><td colspan=\"2\"><h3>Please Login</h3></td></tr>";
		echo "<tr><td>Database:</td><td><input type=\"text\" name=\"db_database\"></td></tr>";
		echo "<tr><td>User:</td><td><input type=\"text\" name=\"db_user\"></td></tr>";
		echo "<tr><td>Password:</td><td><input type=\"text\" name=\"db_password\"></td></tr>";
		echo "<tr><td></td><td><input type=\"submit\" value=\"Login\" name=\"login\"></td></tr></table>";
		echo "</form>";
		
		if($_POST["login"]!='') {
			$_SESSION["db_database"]=$_POST["db_database"];
			$_SESSION["db_user"]=$_POST["db_user"];
			$_SESSION["db_password"]=$_POST["db_password"];

			header("Location: admin.php");
		}
    //die(json_encode(array('outcome' => false, 'message' => 'Unable to connect, please add dbconfig to admin.php')));
}




if($pdo)
{

switch($_GET["action"])
{
	case "edit":
		if($_POST['create']!='') {
    	$stmt =  $pdo->prepare('UPDATE picosafelogin SET user=?, aes=?, datablock=?, counter=?, hw=?, locked=? WHERE user=? LIMIT 1');
      $success = $stmt->execute(array($_POST['user'], $_POST['aes'], $_POST['datablock'],$_POST['counter'], $_POST['hw'], $_POST['locked'], $_GET['user']));

      if($success){ 
      	header('Location: admin.php');
				exit;
			}else
        echo '<div style="background-color: #F4E6E6;border: 1px solid #F2C5C5;color: #B70A0F;padding: 5px 0px 5px 10px">Can`t write into database.</div><br>';
    }
	
		$stmt = $pdo->prepare("SELECT * FROM picosafelogin WHERE user=?");
		$stmt->execute(array($_GET['user']));
		$data = $stmt->fetchAll();
		
		echo "<h2>Edit user</h2>";
    if($error!='') echo '<div style="background-color: #F4E6E6;border: 1px solid #F2C5C5;color: #B70A0F;padding: 5px 0px 5px 10px">'.$error.'</div><br>';
    echo "<form action=\"\" method=\"post\">";
    echo "<table>";
    echo "<tr><td>User</td><td><input type=\"text\" name=\"user\" value=\"{$data[0]['user']}\"></td></tr>";
    echo "<tr><td>Key</td><td><input type=\"text\" name=\"aes\" id=\"aes\" value=\"{$data[0]['aes']}\">&nbsp;<input type=\"button\" value=\"Generate\" onclick=\"GenerateKey()\"></td></tr>";
    echo "<tr><td>Datablock</td><td><input type=\"text\" name=\"datablock\" id=\"datablock\" value=\"{$data[0]['datablock']}\">&nbsp;<input type=\"button\" value=\"Generate\" onclick=\"GenerateDatablock()\"></td></tr>";
    echo "<tr><td>HW Version</td><td><input type=\"text\" name=\"hw\" value=\"{$data[0]['hw']}\"></td></tr>";
    echo "<tr><td>Counter</td><td><input type=\"text\" name=\"counter\" value=\"{$data[0]['counter']}\"></td></tr>";
		$checked = (($data[0]['locked']=='1') ? 'checked' : '');
    echo "<tr><td>Locked</td><td><input type=\"checkbox\" name=\"locked\" value=\"1\" $checked></td></tr>";
    echo "<tr><td></td><td><input type=\"submit\" name=\"create\" value=\"Save\"></td></tr>";
    echo "</table>";
    echo "</form>";
	break;



	case "delete":
		if($_GET['user']!='') {
			$stmt = $pdo->prepare("DELETE FROM picosafelogin WHERE user=? LIMIT 1");
			$stmt->execute(array($_GET['user']));
			header('Location: admin.php');
			exit;
		}
	break;



	case "create":
		switch($_GET["step"])
		{
			case "step2":
				if($_POST['create']!='') {
					$_SESSION['company'] = $_POST['company'];
					$_SESSION['owner'] = $_POST['owner'];
					$_SESSION['description'] = $_POST['description'];
					$_SESSION['hwlocked'] = ((is_numeric($_POST['hwlocked'])) ? '1' : '0');
					header('Location: admin.php?action=create&step=step3');
					exit;
				}				

				echo "<h2>Step 2: Public data for Hardware</h2>";
				echo "<form action=\"admin.php?action=create&step=step2\" method=\"post\">";
				echo "<table>";
				echo "<tr><td>Company</td><td><input type=\"text\" name=\"company\"></td></tr>";
				echo "<tr><td>Owner</td><td><input type=\"text\" name=\"owner\"></td></tr>";
				echo "<tr><td>Description</td><td><input type=\"text\" name=\"description\"></td></tr>";
				echo "<tr><td>Locked</td><td><input type=\"checkbox\" name=\"hwlocked\" value=\"1\"></td></tr>";
				echo "<tr><td></td><td><input type=\"submit\" name=\"create\" value=\"next\"></td></tr>";
				echo "</table>";
				echo "</form>";
			break;



			case "step3":
				if($_POST['create']!='') {
					$stmt =  $pdo->prepare('INSERT INTO picosafelogin (user,aes,datablock,counter,hw,locked) VALUES(?,?,?,?,?,?)');
					$success = $stmt->execute(array($_SESSION['user'], $_SESSION['aes'], $_SESSION['datablock'],$_SESSION['counter'], $_SESSION['hw'], $_SESSION['locked']));

					if($success) {
						header('Location: ./admin.php?action=create&step=step4');
						exit;
					} else
						echo '<div style="background-color: #F4E6E6;border: 1px solid #F2C5C5;color: #B70A0F;padding: 5px 0px 5px 10px">Can`t write into database</div><br>';		
				}
				// soopu9goBoay9vongooth2ooLu8keed1eeng5jo7th

				$lock = (($_SESSION['hwlocked']=='1') ? ' -l' : '');
				$cmd1 = "sudo python init.py -c \"{$_SESSION['company']}\" -o \"{$_SESSION['owner']}\" -p {$_SESSION['aes']}{$_SESSION['datablock']} -d \"{$_SESSION['description']}\"";
				$cmd2 = "sudo python init.py{$lock} -c \"{$_SESSION['company']}\" -o \"{$_SESSION['owner']}\" -p {$_SESSION['aes']}{$_SESSION['datablock']} -d \"{$_SESSION['description']}\"";

				echo "<h2>Step 3: Initialize Hardware</h2>";
				echo "<h3>Testing Initialize Command for Token</h3>";

				echo "<pre>";
				echo $cmd1;
				echo "</pre>";

				echo "<h3>Final Initialize Command for Token</h3>";

				echo "<pre>";
				echo $cmd2;
				echo "</pre>";
				echo "<form action=\"admin.php?action=create&step=step3\" method=\"post\">";
				echo "<table>";
				echo "<tr><td></td><td><input type=\"submit\" name=\"create\" value=\"Next\"></td></tr>";
				echo "</table>";
				echo "</form>";
	
			break;

			

			case "step4":
				echo "<h2>Step 4: Check Hardware</h2>";
				echo "<form action=\"admin.php?action=create&step=step4\" method=\"post\">";
				echo "<table>";
				echo "<tr><td>OTP:</td><td><input type=\"text\" name=\"otp\"></td></tr>";
				echo "<tr><td></td><td><input type=\"submit\" name=\"check\" value=\"check\"></td></tr>";
				echo "</table>";
				echo "</form>";

				if($_POST['check']!='') {
          $pico = new PicosafeLogin("Europe/Berlin");
          $result = $pico->ParseOTP($_POST['otp'],$_SESSION['aes'].$_SESSION['datablock']);
          echo "
					<table>
						<tr><td>Counter:</td><td>{$result['counter']}</td></tr>
						<tr><td>Datablock:</td><td>{$result['datablock']}</td></tr>
						<tr><td>Timestamp</td><td>".date("d.m.Y H:i:s",$result['timestamp'])."</td></tr>
					</table>
					";
        }
			break;



			default:
				$error = '';
				if($_POST['create']!='') {
				
					if($_POST['user']=='')
						$error = 'Please enter a value for `user`<br>';
					else {
						$stmt = $pdo->prepare("SELECT '1' FROM picosafelogin WHERE user=? LIMIT 1");
						$stmt->execute(array($_POST['user']));
						$exists = $stmt->fetch();
						if(is_array($exists)) $error .= 'This user already exists.<br>';
					}

					if($_POST['aes']=='' && strlen($_POST['aes'])!=32)
						$error .= 'Please enter a value for `aes`';

					if($_POST['datablock']=='' && strlen($_POST['datablock'])!=10)
						$error .= 'Please enter a value for `datablock`';


					$counter = ((is_numeric($_POST['counter'])) ? $_POST['counter'] : '1'); 
					$locked = ((is_numeric($_POST['locked'])) ? '1' : '0'); 

					if($error=='') {
						$_SESSION['user'] = $_POST['user'];	
						$_SESSION['aes'] = $_POST['aes'];	
						$_SESSION['datablock'] = $_POST['datablock'];	
						$_SESSION['hw'] = ((is_numeric($_POST['hw'])) ? $_POST['hw'] : '0');	
						$_SESSION['counter'] = ((is_numeric($_POST['hw'])) ? $_POST['counter'] : '0');
						$_SESSION['locked'] = ((is_numeric($_POST['locked'])) ? '1' : '0');	
						header('Location: ./admin.php?action=create&step=step2');
						exit;
					}
				}

				echo "<h2>Step 1: New user for database</h2>";
				if($error!='') echo '<div style="background-color: #F4E6E6;border: 1px solid #F2C5C5;color: #B70A0F;padding: 5px 0px 5px 10px">'.$error.'</div><br>';
        echo "<form action=\"admin.php?action=create&step=step1\" method=\"post\">";
        echo "<table>";
        echo "<tr><td>User</td><td><input type=\"text\" name=\"user\"></td></tr>";
        echo "<tr><td>Key</td><td><input type=\"text\" name=\"aes\" id=\"aes\">&nbsp;<input type=\"button\" value=\"Generate\" onclick=\"GenerateKey()\"></td></tr>";
        echo "<tr><td>Datablock</td><td><input type=\"text\" name=\"datablock\" id=\"datablock\">&nbsp;<input type=\"button\" value=\"Generate\" onclick=\"GenerateDatablock()\"></td></tr>";
        echo "<tr><td>HW Version</td><td><input type=\"text\" name=\"hw\"></td></tr>";
        echo "<tr><td>Counter</td><td><input type=\"text\" name=\"counter\"></td></tr>";
        echo "<tr><td>Locked</td><td><input type=\"checkbox\" name=\"locked\" value=\"1\"></td></tr>";
        echo "<tr><td></td><td><input type=\"submit\" name=\"create\" value=\"next\"></td></tr>";
        echo "</table>";
        echo "</form>";
			break;
		}	
	break;



	case "logout":
		session_destroy();
		header("Location: admin.php");
	break;



	default:
		$query = 'SELECT user,aes,datablock,counter,hw,locked FROM picosafelogin';
		echo "<table class=\"list\"><tr class=\"header\"><td>User</td><td>AES</td><td>Datablock</td><td>Counter</td><td>HW</td><td>Locked</td><td>Action</td></tr>";
		foreach ($pdo->query($query) as $row) 
		{
			echo "<tr><td>".$row["user"]."</td><td>".$row["aes"]."</td><td>".$row["datablock"]."</td><td>".$row["counter"]."</td><td>".$row["hw"]."</td><td>".$row["locked"]."</td>
								<td><a href=\"admin.php?action=edit&user={$row['user']}\">Edit</a>&nbsp;
										<a href=\"#\" onclick=\"if(!confirm('Do you really want delete this entry?')) return false;
    																																									else window.location.href='admin.php?action=delete&user={$row['user']}';\">Delete</a></td></tr>";
		}
		echo "</table>";
	}
}

echo "<div id=\"footer\"><a href=\"http://picosafe.embedded-projects.net/\">embedded projects GmbH 2013</a></div>";


function BgImage()
{
	$image = base64_decode('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');

	header('Content-Type: image/jpeg');
	echo $image;
	exit;
}


?>
