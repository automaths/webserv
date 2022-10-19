<?php 
if (!isset($_COOKIE["testcookie"]))
{
    echo "setting";
    setcookie( "testcookie", "coucou", time() + (86400 * 30));
}
else
{
    echo $_COOKIE["testcookie"];
}
?>
<!DOCTYPE html>
<html>
<body>
<h1>My second PHP page</h1>
<?php
echo "Hello World!";
?>
</body>
</html>
