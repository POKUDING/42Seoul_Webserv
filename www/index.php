#!/usr/bin/php
<!DOCTYPE html>
<html>
<head>
	<link rel="shortcut icon" href="data:image/x-icon;," type="image/x-icon">
	<title>Welcome to SpiderMen</title>
    <meta charset="utf-8">
</head>
 
<body>
  <h1><a href="http://0.0.0.0:4242/index.php">this is PHP Page</a></h1>
  <ol>
    <li><a href="http://0.0.0.0:4242/index.py">to Python Page</a></li>
    <li><a href="http://0.0.0.0:4242/">to HTML Page</a></li>
  </ol>

  <h1>여긴 뭐게요</h1>
  <ol>
    <?php
      echo file_get_contents("./www/source/php.txt");
    ?>
  </ol>

</body>
</html>