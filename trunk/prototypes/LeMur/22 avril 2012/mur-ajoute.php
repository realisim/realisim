<!DOCTYPE HTML>
<html>

<head>
</head>


<body>

  <?php 
  mysql_connect("localhost", "", "") or die (mysql_error());
  mysql_select_db("test") or die(mysql_error());

  $x = $_GET['x'];
  $y = $_GET['y'];
//  if($x <= 20 && $y <= 20)
//      mysql_query("delete from points;");
//  else
    mysql_query("insert into points (x,y) values(".$x.",".$y.");");
  ?>
     
</body>
</html>