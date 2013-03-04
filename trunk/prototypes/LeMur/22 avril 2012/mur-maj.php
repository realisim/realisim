<!DOCTYPE HTML>
<html>
<head>
</head>


<body>

  <?php 
  mysql_connect("localhost", "", "") or die (mysql_error());
  mysql_select_db("test") or die(mysql_error());

	$lastUpdateFromClient = $_GET['lastUpdate'];

	$query = "SELECT * FROM points WHERE timestamp > '".$lastUpdateFromClient."'";
//echo $query."<br>";
  $queryResult = mysql_query( $query ) or die(mysql_error());  
  $nrows = mysql_num_rows( $queryResult );

//	while( $nrows == 0 )
//  {
//    sleep(1);
//    $queryResult = mysql_query( $query ) or die(mysql_error());  
//    $nrows = mysql_num_rows( $queryResult );
//		echo "nouveau points<br> ";    
//  }

	if($nrows > 0)
  {
    $lastUpdate = "";
    while($row = mysql_fetch_array( $queryResult ))
    {
      echo ",".$row['x'].",".$row['y'];
      $lastUpdate = $row['timestamp'];
    }	
    echo ",".$lastUpdate.",";
  }
  
  ?>
     
</body>
</html>