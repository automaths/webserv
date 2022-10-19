<?php
session_start();
if (isset($_SESSION["PAGES"]))
    $_SESSION["PAGES"] += 1;
else
    $_SESSION["PAGES"] = 1;
if (isset($_POST["password"]))
{
    if ($_POST["password"] == "impossible")
    {
        if (isset($_COOKIE["Logged"]))
           $_COOKIE["Logged"] = "true"; 
        else
            setcookie("Logged", "true", time() + (86400 * 30));
        $_SESSION["NEW"] = "true";
        include "upload.php";
    }
    else
    {
        include "log.php";
    }
}
else if (isset($_POST["disconnect"]))
{
    if (isset($_COOKIE["Logged"]))
        $_COOKIE["Logged"] = "false";
    else
        setcookie("Logged", "false", time() + (86400 * 30));
    include "log.php";
}
else if (isset($_FILES["loggedfile"]))
{
    if ($_FILES["loggedfile"]["error"] != 0)
        echo "Error while trying to upload file";
    else
        move_uploaded_file($_FILES["loggedfile"]["tmp_name"], "./Downloads/" . $_FILES["loggedfile"]["name"]);
    if (isset($_COOKIE["Logged"]) && $_COOKIE["Logged"] == "true")
    {
        include "upload.php";
    }
    else
    {
        include "log.php";
    }
}
else
{
    if (isset($_COOKIE["Logged"]) && $_COOKIE["Logged"] == "true")
    {
        include "upload.php";
    }
    else
    {
        include "log.php";
    }
}


var_dump($_POST);
var_dump($_FILES);
phpinfo();
exit(0);