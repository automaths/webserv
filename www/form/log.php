<!DOCTYPE html>
<html lang="en">
    <head>
        <title></title>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <!-- <link href="css/style.css" rel="stylesheet"> -->
    </head>
    <body>
    <h1>Find the password and you may enter</h1>
    <form action="index.php" method="get">
        <input type="text" name="password">
        <input type="submit" value="Try">
    </form>
    <footer><p>Number of pages viewed: <?php echo $_SESSION["PAGES"] ?></p></footer>
    </body>
</html>