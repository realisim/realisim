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
  if($x <= 50 && $y <50)
      mysql_query("delete from points;");
  else
    mysql_query("insert into points values(".$x.",".$y.");");
  ?>
     
</body>
</html>