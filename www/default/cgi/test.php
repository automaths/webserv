<?php 
if (!isset($_COOKIE["testcookie"]))
{
    echo $_COOKIE["testcookie"];
}
else
{
    echo "setting";
    setcookie( "testcookie", "coucou", time() + (86400 * 30));
}
?>
<!DOCTYPE html>
<html>
<body>
<h1>My first PHP page</h1>
<?php
echo "Hello World!";
?>
</body>
</html>
