<!DOCTYPE HTML>
<html>
<head>
</head>


<body>

  <?php 
  mysql_connect("localhost", "", "") or die (mysql_error());
  mysql_select_db("test") or die(mysql_error());

  // Retrieve all the data from the "example" table
  $result = mysql_query("SELECT * FROM points") or die(mysql_error());  

  // store the record of the "example" table into $row
  $nombreDePoints = 0;
  while($row = mysql_fetch_array( $result ))
  {
  	echo ",".$row['x'].",".$row['y'];
  	//$nombreDePoints++;  X
  }	
  echo ",";
  //echo "nombre de points: ".$nombreDePoints."<br>";
  ?>
     
</body>
</html>