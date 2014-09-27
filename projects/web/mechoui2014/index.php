<html>
<head>
	<!DOCTYPE html>
	<?php
		session_start();
		$uploadId = uniqid();
	?>

	<style type="text/css">
	    body {scrolling:no;
		}
	    iframe {position:absolute;
	    z-index:1;
	    top:0px;
	    left:0px;
		}
	</style>

	<script src="http://ajax.googleapis.com/ajax/libs/jquery/1.4.0/jquery.js" type="text/javascript"></script>
	<script>
		$(document).ready(function() 
		{ 
			$('#upload_frame').hide();
			//montre le transfert quand on pese sur envoyer
		    $("#formulaireDEnvoi").submit(function()
		    {				    	
				function set ()
				{
					$("#formulaireDEnvoi").hide();
					$('#upload_frame').show();
					setInterval(function()
			        {
			        	var fileElement = document.getElementById('fileElementId');
			        	var numFiles = fileElement.files.length;
			        	var args = '?uploadId=<?php echo $uploadId ?>&numFiles=' + numFiles;
			        	for( var i = 0; i < numFiles; ++i )
			        	{
			        		args += "&fileName" + i + "=" + encodeURIComponent(fileElement.files.item(i).name);
			        		args += "&fileSize" + i + "=" + fileElement.files.item(i).size;
			        	}
			            //$("#uploadProgress").load("uploadProgress.php" + args);
			            //$("#uploadProgress").html(  "arguments: " +  args );			            
						$('#upload_frame').attr('src','uploadProgress.php' + args + '&randomValue=' + Math.random());
					}, 500);
				}
		    setTimeout(set, 1000);

			});
		});
	</script>
</head>

<body>
	<?php
	if ( isset($_GET['success']) )
	{ ?>
		<p>Les fichiers ont &eacutet&eacute envoy&eacute avec succ&egraves.</p>
		<form name="envoyerDautresPhotos" id="envoyerDautresPhotos" action="index.php" method="POST">
			<input name="submit" type="submit" value="Envoyer d'autres fichiers" />
		</form>
	<?php } 
	else if ( isset($_GET['fail']) )
	{ ?>
		<p>Les fichiers n'ont pas &eacutet&eacute envoy&eacute.</p>
    <?php } 
	else
	{ ?>
		<form name="formulaireDEnvoi" id="formulaireDEnvoi" enctype="multipart/form-data" action="moveUploadedFiles.php" method="POST">
		<input type="hidden" name="<?php echo ini_get("session.upload_progress.name"); ?>" value="<?php echo $uploadId; ?>"  /> 
		<!-- <input type="hidden" name="APC_UPLOAD_PROGRESS" id="progress_key" value="<?php echo $uploadId; ?>"/> -->
		Nom du photographe: <input type="text" name="photographName"><br>
		S&eacutelectioner des images: <input type="file" id="fileElementId" name="file[]" multiple="1"><br>
		<input name="submit" type="submit" value="Envoyer les images" />
		</form>

<!--Include the iframe-->
    <p >
    <iframe id="upload_frame" name="upload_frame" frameborder="0" width="100%" height="100%" style="visibility:hidden;" 
      onload="this.style.visibility='visible';"> </iframe>
    </p>
<!---->
	<?php } ?>

</body>
</html>