<html>
<body>

<?php
session_start(); 

$progressKey = ini_get("session.upload_progress.prefix").$_GET['uploadId'];
$numFiles = $_GET['numFiles'];
$fileNames = [];
$fileSizes = [];
$bytesProcessed = [];
$randomValue = $_GET['randomValue'];
//echo $numFiles."<br>";
//echo "randomValue".$randomValue."<br>";
/*comptabilisation des donnees*/
for( $i = 0; $i < $numFiles; $i++ )
{
	$fileNames[$i] = $_GET['fileName'.$i];
	$fileSizes[$i] = $_GET['fileSize'.$i];

	 if( isset($_SESSION[$progressKey]) && $i < count($_SESSION[$progressKey]['files']) )
	 { $bytesProcessed[$i] = $_SESSION[$progressKey]['files'][$i]['bytes_processed']; }
	 else
	{ $bytesProcessed[$i] = 0; }

	$p = $bytesProcessed[$i] / $fileSizes[$i] * 100;
	//echo $fileNames[$i]." ".number_format($p, 0)."%<br>";
}

//affichage des donnees
if( !isset($_SESSION[$progressKey]) )
{ 
	echo "Le t&eacutel&eacuteversement commencera sous peu...<br>";
}
else
{
	//echo "progressKey = " . $progressKey;
	echo "<table border='1' style='width:50%;height:100%'>";
	echo "<tr><th>fichier</th><th>% d'envoi</th></tr>";
	for( $i = 0; $i < $numFiles; $i++ )
	{
		$p = $bytesProcessed[$i] / $fileSizes[$i] * 100;
		echo "<tr>";
		echo "<td>".$fileNames[$i]."</td>";
		echo "<td>".number_format($p, 1)."</td>";
		echo "</tr>";
	}
	echo "</table>";	
}


// echo "------------------------------<br>";
// var_dump($_SESSION[$progressKey]);
// echo "_POST: <br>"; var_dump($_POST);
// echo "_FILES: <br>";  var_dump($_FILES);
// echo "------------------------------<br>";
?>

</body>
</html>
