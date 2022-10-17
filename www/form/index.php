<?php
session_start();
if (isset($_SESSION["PAGES"]))
    $_SESSION["PAGES"] += 1;
else
    $_SESSION["PAGES"] = 1;
if (isset($_POST["password"]))
{
    // echo "We are getting somewhere\n";
    // var_dump($_GET);
    // var_dump($_COOKIE);
    if ($_POST["password"] == "impossible")
    {
        setcookie("Logged", "true", time() + (86400 * 30));
        echo nl2br ("Welcome to webserv\n Number of pages viewed: " . $_SESSION["PAGES"]);

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
        echo nl2br ("Welcome to webserv\n Number of pages viewed: " . $_SESSION["PAGES"]);
    }
    else
    {
        include "log.php";
    }
}