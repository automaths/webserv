# Webserv - 42 school project

The goal of this project is to implement a webserver using C++. 

## Usage

```
make && ./webserv [+ configuration file path]
```

#### Implemented configuration file directives (follows the nginx standard): 
  - listen: address:port <br/>
      -> ex: " 127.0.0.1:8080; "
  - server_name <br/>
      -> ex: " server_name default; "
  - index <br/>
      -> ex: " index path.html path_two.html path_three.html "
  - autoindex <br/>
      -> ex: " autoindex on; "
  - root <br/>
      -> ex: " root ./www/default; "
  - client_body_buffer_size <br/>
      -> ex: "client_body_buffer_size 200M;"
  - rewrite (redirection for temporary or permanent for permanent redirection) <br/>
      -> ex: " rewrite /path_to_rewrite redirection; " or " rewrite /path_to_rewrite permanent; "
  - upload_pass <br/>
      -> ex: " upload_pass /tmp/path_to_upload; "
  - location <br/>
      -> ex: " location /form {<br/>
			  root ./www/form;<br/>
			  index index.php;<br/>
			  cgi .php /usr/lib/cgi-bin/php;<br/>
		    } "
 
#### Other implemented directives: 
  - error_page: set a page returned on given error response from server<br/>
      -> ex: " error_page 404 ./404_template.html 403 ./403_template.html; "
  - allow_method: allow only the stated method for http requests<br/>
      -> ex: "allow_method GET POST;"
  - cgi [cgi + extension + path]: set path to cgi depending on chosen extensions<br/>
      -> ex: " cgi .php /usr/lib/cgi-bin/php .py /usr/lib/cgi-bin/flask; "
  - limit_upload: limit uploads on server<br/>
      -> ex: " limit_upload; "
      
 #### Inheritance:
  All directives in the configuration files use inheritance, meaning that the directive will also apply to the sublocations of the configuration file
 
 **An example can be found in default_config**

## Theory

- From the select, poll and epoll functions allowed, epoll has been implemented as it uses a kernel queue for the sockets, making it more performant
- The documentation used for the project: **rfc 9110** (update of rfc 7230, 7231, 7232, 7233, 7234 and 7235), **nginx configuration guidelines**
- Telnet, curl, siege and vegeta were used for testing the server
 

