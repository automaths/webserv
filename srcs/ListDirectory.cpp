#include "ListDirectory.hpp"

    ListDirectory::ListDirectory(std::string directory, std::string uri)
    {
        DIR *d;
        struct dirent *dir;
        std::string bullet;
        if (uri[uri.size() - 1] != '/')
                uri += "/";
        d = opendir(directory.c_str());
        if (d)
        {
            while ((dir = readdir(d)) != NULL)
            {
                if (strcmp(dir->d_name, "..") && strcmp(dir->d_name, "."))
                {
                    bullet.clear();
                    bullet = "<li><a href=\'" + uri + dir->d_name + "\'>" + dir->d_name + "</a></li>\n";
                    _listing.push_back(bullet);
                }
            }
            closedir(d);
        }
        else
            throw OpenDirException();
    }

    std::string ListDirectory::listing() {
        std::string body = "<!DOCTYPE html>\n<html lang='en'>\n<head>\n<meta charset='UTF-8'>\n<meta http-equiv='X-UA-Compatible' content='IE=edge'>\n<meta name='viewport' content='width=device-width, initial-scale=1.0'>\n<title>Index $uri</title>\n</head>\n<body>\n<h1>Listing directory</h1>\n<ul>\n";
        for (std::vector<std::string>::iterator it = _listing.begin(); it != _listing.end(); ++it)
            body += *it;
        body += "</ul>\n</body>\n</html>";
        std::cout << body << std::endl;
        return (body);
    }

    //TEMPLATE
    // List files in directory
    // $uri = req.getFile();

    // <!DOCTYPE html>
    // <html lang="en">
    // <head>
    // <meta charset="UTF-8">
    // <meta http-equiv="X-UA-Compatible" content="IE=edge">
    // <meta name="viewport" content="width=device-width, initial-scale=1.0">
    // <title>Index $uri</title>
    // </head>
    // <body>
    // 	<h1>$uri</h1>
    // 	<ul>
    // 		<li><a href="/$uri/file1">file1</a></li>
    // 		<li><a href="/$uri/file2">file2</a></li>
    // 	</ul>
    // </body>
    // </html>
