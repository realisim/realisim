<html>
<body>

<?php

$numberOfFiles = count( $_FILES['file']['name'] );
$photographName = 'sansNom';
if( !empty( $_POST['photographName'] ) )
{ $photographName = $_POST['photographName']; }
$imageFolder = "C:/wamp/www/mechoui2014/images/";
$destination = $imageFolder.$photographName."/";

/*Cretion du repertoire sil nexiste pas*/
if( !file_exists($imageFolder) )
{ mkdir( $imageFolder ); }
if( !file_exists($destination) )
{ mkdir( $destination ); }

echo "photographe: ".$photographName."<br>";

$success = true;
$unique = uniqid();
if( !empty($_FILES['file']['name'][0] ) )
{
	var_dump($_FILES['file']);
	echo $numberOfFiles." fichier(s) &agrave t&eacutel&eacuteverser<br>";
	$count = 0;
	foreach ($_FILES['file']['name'] as $file)
	{
		$filename = pathinfo( $file, PATHINFO_FILENAME) . "_" . $unique . "_" . $count;
		$extension = pathinfo( $file, PATHINFO_EXTENSION);
		$tmpFile = $_FILES['file']['tmp_name'][$count];
	    echo $tmpFile." -> ".$destination.$filename.'<br>';
	    $hasFileMoved = move_uploaded_file($tmpFile, $destination.$filename.".".$extension);
	    if( !$hasFileMoved )
	    {
	    	$success = false;
	    	echo "impossible de d&eacuteplacer le fichier ".$tmpFile."<br.";
	    }
	    $count++;
	}	
}
else
{
  echo "aucun fichier &agrave envoyer.";
}

//specify redirect URL
if( $success )
{ $redirect = "index.php?success"; }
else
{ { $redirect = "index.php?fail"; } }
//redirect user
header('Location: '.$redirect); die;

?>

</body>
</html>