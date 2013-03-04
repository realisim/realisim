<!DOCTYPE HTML>
<html>
<head>
</head>


<body>

  <?php 
  mysql_connect("localhost", "", "") or die (mysql_error());
  mysql_select_db("test") or die(mysql_error());
  //echo "Connected to Database test:<br>";

  // Retrieve all the data from the "example" table
  $result = mysql_query("SELECT * FROM points") or die(mysql_error());  

  // store the record of the "example" table into $row
  $nombreDePoints = 0;
  while($row = mysql_fetch_array( $result ))
  {
  	echo ",".$row['x'].",".$row['y'];
    //echo "x: ".$row['x']." y: ".$row['y']."<br>";
  	//$nombreDePoints++;  
  }	
  echo ",";
  //echo "nombre de points: ".$nombreDePoints."<br>";
  ?>
     
</body>
</html>