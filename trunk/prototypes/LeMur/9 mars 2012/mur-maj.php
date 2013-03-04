<!DOCTYPE HTML>
<html>
<head>
</head>


<body>

  <?php 
  mysql_connect("localhost", "", "") or die (mysql_error());
  mysql_select_db("test") or die(mysql_error());

	$lastUpdateFromClient = $_GET['lastUpdate'];

  // Retrieve all the data from the "example" table
  $result = mysql_query("SELECT * FROM points") or die(mysql_error());  

  $lastUpdate = "";
  while($row = mysql_fetch_array( $result ))
  {
  	echo ",".$row['x'].",".$row['y'];
    $lastUpdate = $row['timestamp'];
  }	
  echo ",".$lastUpdate;
  ?>
     
</body>
</html>