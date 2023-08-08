#!/usr/local/bin/python3  
print("Content-Type: text/html\n")
print('''<!DOCTYPE html>
<html>

<head>
  <link rel="shortcut icon" href="data:image/x-icon;," type="image/x-icon"> 
  <title>Welcome to SpiderMen</title>
  <meta charset="utf-8">
</head>

<body style="background-color:rgb(228, 198, 114)">
  <h1><a href="http://0.0.0.0:4243/index.py">this is Python Page</a></h1>
  <ol>
    <li><a href="http://0.0.0.0:4243/">to Html Page</a></li>
    <li><a href="http://0.0.0.0:4243/index.php">to PHP Page</a></li>
  </ol>

</body>
</html>''')