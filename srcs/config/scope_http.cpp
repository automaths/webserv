#include "library.hpp"
#include "scope_server.hpp"
#include "scope_location.hpp"
#include "scope_http.hpp"

void HttpScope::extract_client_body_buffer_size(std::string directive) {
    directive.erase(0, directive.find("client_body_buffer_size") + 23);
    _client_body_buffer_size = directive.substr(directive.find_first_not_of("\t\v\n\r\f "), directive.find_first_of("\t\v\n\r\f ", directive.find_first_not_of("\t\v\n\r\f ")));
}

void HttpScope::extract_root(std::string directive) {
    directive.erase(0, directive.find_first_of("root") + 4);
    _root = directive.substr(directive.find_first_not_of("\t\v\n\r\f "), directive.find_first_of("\t\v\n\r\f ", directive.find_first_not_of("\t\v\n\r\f ")));
}

void HttpScope::extract_cgi(std::string cgi_dir) {
    std::vector<std::string> content;
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
    for (std::vector<std::string>::iterator it = content.begin(); it != content.end(); ++it)
    {
        if (++it != content.end())
            _cgi.insert(std::make_pair(*(--it), *(++it)));
        else
            --it;
    }
}

void HttpScope::extract_index(std::string index_dir) {
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

void HttpScope::extract_default_error_pages(std::string error_page_dir) {
    error_page_dir.erase(0, error_page_dir.find("error_page") + 10);
    while (error_page_dir.find_first_of(" \t\v\n\r\f") == 0)
        error_page_dir.erase(0, 1);
    std::vector<std::string> number;
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
    for (std::vector<std::string>::iterator it = number.begin(); it != number.end(); ++it)
        _default_error_pages.insert(std::make_pair(*it, path));
}
void HttpScope::extract_autoindex(std::string autoindex_dir) {
    autoindex_dir.erase(0, autoindex_dir.find("autoindex ") + 9);
    _autoindex = autoindex_dir.substr(autoindex_dir.find_first_not_of("\t\v\n\r\f ", 0), autoindex_dir.find_first_of("\t\v\n\r\f ", autoindex_dir.find_first_not_of("\t\v\n\r\f ", 0)));
    if (_autoindex.compare("on") != 0)
        _autoindex = "off";
}
void HttpScope::extract_server_blocks() {
    while (_block.size() > 0 && _block.find("{") > 0)
    {
        if (_block.find("http") == 0)
        {
            if (_block.find_first_not_of("\t\v\n\r\f ", 4) == _block.find("{", 4))
                _block.erase(0, _block.find("{") + 1);
            break;
        }
        if (_block.find("server") == 0)
        {
            if (_block.find_first_not_of("\t\v\n\r\f ", 6) == _block.find("{", 6))
                break;
        }
        _block.erase(0, 1);
    }
    std::string copy = _block;
    while (copy.find(';', 0) != std::string::npos || copy.find('}') != std::string::npos)
    {
        if (copy.find_first_not_of("\t\v\n\r\f ") == copy.find("server") && copy.find("server") != std::string::npos)
        {
            std::string::iterator it = copy.begin();
            for (unsigned int i = 0; i < copy.find("server", 0) + 1; ++i)
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
            _server_blocks.push_back(copy.substr(copy.find("server", 0), n + 1));
            copy.erase(copy.find("server", 0), n);
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
    for (std::vector<std::string>::iterator it = _server_blocks.begin(); it != _server_blocks.end(); ++it)
        _block.erase(_block.find(*it), it->size());
}
void HttpScope::extract_lines() {
    while (_block.find(';', 0) != std::string::npos)
    {
        _directives.push_back(_block.substr(0, _block.find(';')));
        _block.erase(0, _block.find(';', 0) + 1);
    }
}
void HttpScope::clean_http_header()
{
    if (_block.find("http") == _block.find_first_not_of("\t\v\n\r\f "))
    {
        _block.erase(0, _block.find_first_of('{', 0) + 1);
        _block.erase(_block.find_last_of('}'), 1);
    }
    while (_block.find('#', 0) != std::string::npos)
        _block.erase(_block.find('#', 0), _block.find_first_of('\n', _block.find('#', 0)) - _block.find('#', 0));
}
void HttpScope::extract_directives() {
    while (_directives.size() != 0)
    {
        extract_rules(_directives.back());
        _directives.pop_back();
    }
}
void HttpScope::extract_rules(std::string rule)
{
    for (unsigned int i = 0; i < 7; ++i)
    {
        if (rule.find(_directive_types[i]) == rule.find_first_not_of("\t\v\n\r\f "))
        {
            if (rule.find_first_of("\t\v\n\r\f ", rule.find(_directive_types[i])) == (rule.find(_directive_types[i]) + _directive_types[i].size()))
                (this->*exec[i])(rule);
        }

    }
}
void HttpScope::apply_default() {
    // if (_root.size() == 0)
    //     _root = "html";
    if(_autoindex.size() == 0)
        _autoindex = "off";
}
// void HttpScope::print_result() {
//     std::ofstream ofs;
//     ofs.open("./configurations/parsed.txt", std::ios_base::app);
//     ofs << " \n|||||||||||||||||||||||||||||||\n" << std::endl;
//     ofs << "SCOPE: HTTP\n" << std::endl;
//     for (std::vector<std::string>::iterator it = _index.begin(); it != _index.end(); ++it)
//         ofs << "index: " << *it << std::endl;
//     for (std::map<std::string, std::string>::iterator it = _default_error_pages.begin(); it != _default_error_pages.end(); ++it)
//         ofs << "error page " << it->first << " associated path " << it->second << std::endl;
//     ofs << "body max: " << _client_body_buffer_size << std::endl;
//     ofs << "root: " << _root << std::endl;
//     ofs << "allowed method: ";
//     for (std::map<std::string, std::string>::iterator it = _cgi.begin(); it != _cgi.end(); ++it)
//         ofs << "cgi: " << it->first << " associated to path " << it->second << std::endl;
//     ofs << std::endl;
//     ofs << "autoindex: " << _autoindex << std::endl;
//     // for (std::vector<std::string>::iterator it = _server_blocks.begin(); it != _server_blocks.end(); ++it)
//     //     ofs << "server block: " << *it << std::endl;
// }
