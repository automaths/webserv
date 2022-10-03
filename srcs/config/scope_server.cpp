#include "library.hpp"
#include "scope_server.hpp"

void ServerScope::extract_listen(std::string directive){
    directive.erase(0, directive.find("listen") + 6);
    if (directive.find_first_of(':') != std::string::npos)
    {
        _address = directive.substr(0, directive.find_first_of(':'));
        directive.erase(0, directive.find_first_of(':') + 1);
        _port = directive;
    }
    else
    {
        if (directive.find_first_not_of("\t\v\n\r\f 0123456789;") == std::string::npos)
        {
            _port = directive;
            _address = "*";
        }
        else
        {
            _port = "80";
            _address = directive;
        }
    }
}

void ServerScope::extract_server_name(std::string directive) {
    directive.erase(0, directive.find("server_name") + 11);
    while(directive.find_first_of(" \t\v\n\r\f") == 0)
        directive.erase(0, 1);
    while (directive.size() != 0)
    {  
        if (directive.find_first_of(" \t\v\n\r\f") != std::string::npos)
        {
            _server_names.push_back(directive.substr(0, directive.find_first_of(" \t\v\n\r\f")));
            directive.erase(0, directive.find_first_of(" \t\v\n\r\f"));
        }
        else
        {
            _server_names.push_back(directive.substr(0, directive.size()));
            directive.erase(0, directive.size());
        }
        while(directive.find_first_of(" \t\v\n\r\f") == 0)
            directive.erase(0, 1);
    }
}

void ServerScope::extract_client_body_buffer_size(std::string directive) {
    directive.erase(0, directive.find("client_body_buffer_size") + 23);
    _client_body_buffer_size = directive.substr(directive.find_first_not_of("\t\v\n\r\f "), directive.find_first_of("\t\v\n\r\f ", directive.find_first_not_of("\t\v\n\r\f ")));
}

void ServerScope::extract_root(std::string directive) {
    directive.erase(0, directive.find_first_of("root") + 4);
    _root = directive.substr(directive.find_first_not_of("\t\v\n\r\f "), directive.find_first_of("\t\v\n\r\f ", directive.find_first_not_of("\t\v\n\r\f ")));
}

void ServerScope::extract_allow_method(std::string directive) {
    directive.erase(0, directive.find("allow_method") + 12);
    while (directive.find_first_of(" \t\v\n\r\f") == 0)
        directive.erase(0, 1);
    while (directive.size() != 0)
    {  
        if (directive.find_first_of(" \t\v\n\r\f") != std::string::npos)
        {
            _allow_method.push_back(directive.substr(0, directive.find_first_of(" \t\v\n\r\f")));
            directive.erase(0, directive.find_first_of(" \t\v\n\r\f"));
        }
        else
        {
            _allow_method.push_back(directive.substr(0, directive.size()));
            directive.erase(0, directive.size());
        }
        while(directive.find_first_of(" \t\v\n\r\f") == 0)
            directive.erase(0, 1);
    }
}

void ServerScope::extract_cgi(std::string cgi_dir) {
    std::list<std::string> content;
    cgi_dir.erase(0, cgi_dir.find_first_of("cgi") + 3);
    while (cgi_dir.find_first_of(" \t\v\n\r\f") == 0)
        cgi_dir.erase(0, 1);
    while (cgi_dir.size() != 0)
    {  
        if (cgi_dir.find_first_of(" \t\v\n\r\f") != std::string::npos)
        {
            content.push_back(cgi_dir.substr(0, cgi_dir.find_first_of(" \t\v\n\r\f")));
            cgi_dir.erase(0, cgi_dir.find_first_of(" \t\v\n\r\f"));
        }
        else
        {
            content.push_back(cgi_dir.substr(0, cgi_dir.size()));
            cgi_dir.erase(0, cgi_dir.size());
        }
        while(cgi_dir.find_first_of(" \t\v\n\r\f") == 0)
            cgi_dir.erase(0, 1);
    }
    for (std::list<std::string>::iterator it = content.begin(); it != content.end(); ++it)
    {
        if (++it != content.end())
            _cgi.insert(std::make_pair(*(--it), *(++it)));
        else
            --it;
    }
}

void ServerScope::extract_index(std::string index_dir) {
    index_dir.erase(0, index_dir.find("index") + 5);
    while (index_dir.find_first_of(" \t\v\n\r\f") == 0)
        index_dir.erase(0, 1);
    while (index_dir.size() != 0)
    {  
        if (index_dir.find_first_of(" \t\v\n\r\f") != std::string::npos)
        {
            _index.push_back(index_dir.substr(0, index_dir.find_first_of(" \t\v\n\r\f")));
            index_dir.erase(0, index_dir.find_first_of(" \t\v\n\r\f"));
        }
        else
        {
            _index.push_back(index_dir.substr(0, index_dir.size()));
            index_dir.erase(0, index_dir.size());
        }
        while(index_dir.find_first_of(" \t\v\n\r\f") == 0)
            index_dir.erase(0, 1);
    }
}

void ServerScope::extract_default_error_pages(std::string error_page_dir) {
    error_page_dir.erase(0, error_page_dir.find("error_page") + 10);
    while (error_page_dir.find_first_of(" \t\v\n\r\f") == 0)
        error_page_dir.erase(0, 1);
    std::list<std::string> number;
    std::string path;
    while (error_page_dir.size() != 0)
    {  
        if (error_page_dir.find_first_of(" \t\v\n\r\f") != std::string::npos)
        {
            number.push_back(error_page_dir.substr(0, error_page_dir.find_first_of(" \t\v\n\r\f")));
            error_page_dir.erase(0, error_page_dir.find_first_of(" \t\v\n\r\f"));
        }
        else
        {
            number.push_back(error_page_dir.substr(0, error_page_dir.size()));
            error_page_dir.erase(0, error_page_dir.size());
        }
        while(error_page_dir.find_first_of(" \t\v\n\r\f") == 0)
            error_page_dir.erase(0, 1);
    }
    path = number.back();
    number.pop_back();
    for (std::list<std::string>::iterator it = number.begin(); it != number.end(); ++it)
        _default_error_pages.insert(std::make_pair(*it, path));
}

void ServerScope::extract_autoindex(std::string autoindex_dir) {
    autoindex_dir.erase(0, autoindex_dir.find("autoindex ") + 9);
    _autoindex = autoindex_dir.substr(autoindex_dir.find_first_not_of("\t\v\n\r\f ", 0), autoindex_dir.find_first_of("\t\v\n\r\f ", autoindex_dir.find_first_not_of("\t\v\n\r\f ", 0)));
    if (_autoindex.compare("on") != 0)
        _autoindex = "off";
}

void ServerScope::extract_try_files(std::string try_files_dir) {
    try_files_dir.erase(0, try_files_dir.find("try_files") + 9);
    while (try_files_dir.find_first_of(" \t\v\n\r\f") == 0)
        try_files_dir.erase(0, 1);
    while (try_files_dir.size() != 0)
    {  
        if (try_files_dir.find_first_of(" \t\v\n\r\f") != std::string::npos)
        {
            _try_files.push_back(try_files_dir.substr(0, try_files_dir.find_first_of(" \t\v\n\r\f")));
            try_files_dir.erase(0, try_files_dir.find_first_of(" \t\v\n\r\f"));
        }
        else
        {
            _try_files.push_back(try_files_dir.substr(0, try_files_dir.size()));
            try_files_dir.erase(0, try_files_dir.size());
        }
        while(try_files_dir.find_first_of(" \t\v\n\r\f") == 0)
            try_files_dir.erase(0, 1);
    }
}

void ServerScope::extract_location_blocks() {
    std::string copy = _chunk;
    while (copy.find(';', 0) != std::string::npos || copy.find('}') != std::string::npos)
    {
        if (copy.find_first_not_of("\t\v\n\r\f ") == copy.find("location") && copy.find("location") != std::string::npos)
        {
            std::string::iterator it = copy.begin();
            for (unsigned int i = 0; i < copy.find("location", 0) + 1; ++i)
                ++it;
            std::string::iterator tmp = it;
            while (it != copy.end() && *it != '{')
                ++it;
            ++it;
            int n = 1;
            while (it != copy.end() && n > 0)
            {
                if (*it == '{')
                    ++n;
                if (*it == '}')
                    --n;
                ++it;
            }
            n = 0;
            while (tmp != it)
            {
                ++n;
                ++tmp;
            }
            _location_blocks.push_back(copy.substr(copy.find("location", 0), n + 1));
            copy.erase(copy.find("location", 0), n);
        }
        if (copy.find('}', 0) == std::string::npos)
        {
            while (copy.find(';') > 0 && copy.find(';') != std::string::npos)
                copy.erase(0, 1);
            if (copy.size() > 0)
                copy.erase(0, 1);
        }
        else if (copy.find(';', 0) == std::string::npos)
        {
            while (copy.find('}') > 0 && copy.find('}') != std::string::npos)
                copy.erase(0, 1);
            if (copy.size() > 0)
                copy.erase(0, 1);
        }
        else 
        {
            if (copy.find(';', 0) < copy.find('}', 0))
            {
                while (copy.find(';') > 0 && copy.find(';') != std::string::npos)
                    copy.erase(0, 1);
                if (copy.size() > 0)
                    copy.erase(0, 1);
            }
            else
            {
                while (copy.find('}') > 0 && copy.find('}') != std::string::npos)
                    copy.erase(0, 1);   
                if (copy.size() > 0)
                    copy.erase(0, 1);
            }
        }
    }
    for (std::list<std::string>::iterator it = _location_blocks.begin(); it != _location_blocks.end(); ++it)
        _chunk.erase(_chunk.find(*it), it->size());
}

void ServerScope::extract_lines() {
    while (_chunk.find(';', 0) != std::string::npos)
    {
        _directives.push_back(_chunk.substr(0, _chunk.find(';')));
        _chunk.erase(0, _chunk.find(';', 0) + 1);
    }
}

void ServerScope::extract_rules(std::string rule)
{
    for (unsigned int i = 0; i < 10; ++i)
    {
        if (rule.find(_directive_types[i]) == rule.find_first_not_of("\t\v\n\r\f "))
        {
            if (rule.find_first_of("\t\v\n\r\f ", rule.find(_directive_types[i])) == (rule.find(_directive_types[i]) + _directive_types[i].size()))
                (this->*exec[i])(rule);
        }

    }
}

void ServerScope::clean_comments_header()
{
    _chunk.erase(0, _chunk.find_first_of('{', 0) + 1);
    _chunk.erase(_chunk.find_last_of('}'), 1);
    while (_chunk.find('#', 0) != std::string::npos)
        _chunk.erase(_chunk.find('#', 0), _chunk.find_first_of('\n', _chunk.find('#', 0)) - _chunk.find('#', 0));
}

void ServerScope::extract_directives() {
    while (_directives.size() != 0)
    {
        extract_rules(_directives.back());
        _directives.pop_back();
    }
}

void ServerScope::apply_default() {
    if (_address.size() == 0)
    {
        _address = "*";
        _port = "80";           
    }
}

void ServerScope::print_result() {
    std::ofstream ofs;
    ofs.open("./configurations/parsed.txt", std::ios_base::app);

    ofs << " \n------------------------------\n" << std::endl;
    ofs << "SCOPE: SERVER\n" << std::endl;
    ofs << "the address is: " << _address << std::endl;
    ofs << "the port is: " << _port << std::endl;
    for (std::list<std::string>::iterator it = _server_names.begin(); it != _server_names.end(); ++it)
        ofs << "server_name: " << *it << std::endl;
    for (std::list<std::string>::iterator it = _index.begin(); it != _index.end(); ++it)
        ofs << "index: " << *it << std::endl;
    for (std::map<std::string, std::string>::iterator it = _default_error_pages.begin(); it != _default_error_pages.end(); ++it)
        ofs << "error page " << it->first << " associated path " << it->second << std::endl;
    ofs << "body max: " << _client_body_buffer_size << std::endl;
    ofs << "root: " << _root << std::endl;
    ofs << "allowed method: ";
    for (std::list<std::string>::iterator it = _allow_method.begin(); it != _allow_method.end(); ++it)
        ofs << *it << ", ";
    ofs << std::endl;
    for (std::map<std::string, std::string>::iterator it = _cgi.begin(); it != _cgi.end(); ++it)
        ofs << "cgi: " << it->first << " associated to path " << it->second << std::endl;
    ofs << "autoindex: " << _autoindex << std::endl;
    for (std::list<std::string>::iterator it = _try_files.begin(); it != _try_files.end(); ++it)
    {
        if (++it != _try_files.end())
            ofs << "try_files: " << *(--it) << std::endl;
        else
            ofs << "try_files (fallback): " << *(--it) << std::endl;
    }
    // for (std::list<std::string>::iterator it = _location_blocks.begin(); it != _location_blocks.end(); ++it)
    //     ofs << "location block: " << *it << std::endl;
}
