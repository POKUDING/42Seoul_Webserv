#!/usr/bin/php
<!DOCTYPE html>
<html>
<head>
	<link rel="shortcut icon" href="data:image/x-icon;," type="image/x-icon">
	<title>Welcome to SpiderMen</title>
    <meta charset="utf-8">
</head>
 
<body style="background-color:rgb(162, 238, 154)">
  <h1><a href="http://0.0.0.0:4243/index.php">this is PHP Page</a></h1>
  <ol>
    <li><a href="http://0.0.0.0:4243/index.py">to Python Page</a></li>
    <li><a href="http://0.0.0.0:4243/">to HTML Page</a></li>
  </ol>

  <h1>여긴 뭐게요</h1>
  <ol>
    <?php
      echo file_get_contents("./www2/source/php2.txt");
    ?>
  </ol>

</body>
</html>