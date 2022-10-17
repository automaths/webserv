<?php
if (!isset($_SESSION["NEW"]) && (!isset($_COOKIE["Logged"]) || $_COOKIE["Logged"] != "true"))
{
    echo "True";
    header("Location: /form");
    exit;
}
if (isset($_SESSION["NEW"]))
{
    unset($_SESSION["NEW"]);
} 
?>
<!DOCTYPE html>
<html lang="en">
    <head>
        <title></title>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <!-- <link href="css/style.css" rel="stylesheet"> -->
    </head>
    <body>
    <h1>Time to upload</h1>
    <form method="post" enctype="multipart/form-data">
        <input type="file" name="loggedfile" id="loggedfile">
        <input type="submit" value="Upload File">
    </form>
    <form method="post">
        <input type="checkbox" name="disconnect" id="disconnect" checked="true" hidden>
        <input type="submit" value="Disconnect">
    </form>
    <footer><p>Number of pages viewed: <?php echo $_SESSION["PAGES"] ?></p></footer>
    </body>
</html>