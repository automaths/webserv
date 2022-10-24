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
else if (isset($_FILES["loggedfile"]))
{
    if (isset($_COOKIE["Logged"]) && $_COOKIE["Logged"] == "true")
    {
        if ($_FILES["loggedfile"]["error"] != 0 || !move_uploaded_file($_FILES["loggedfile"]["tmp_name"], "./Downloads/" . $_FILES["loggedfile"]["name"]))
            echo "Error while trying to upload file";
        else
        {
            echo "File " . $_FILES["loggedfile"]["name"] . " successfully uploaded";
            http_response_code(201);
        }
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