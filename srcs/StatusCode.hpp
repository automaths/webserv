#pragma once

# include <map>
# include <string>


class StatusCode {

    public:

    StatusCode(){
        _statuscode["100"] = "Continue";
        _statuscode["101"] = "Switching Protocols";
        _statuscode["102"] = "Processing";
        _statuscode["200"] = "OK";
        _statuscode["201"] = "Created";
        _statuscode["202"] = "Accepted";
        _statuscode["203"] = "Non-Authoritative Information";
        _statuscode["204"] = "No Content";
        _statuscode["206"] = "Partial Content";
        _statuscode["207"] = "Multi-Status";
        _statuscode["300"] = "Multiple Choices";
        _statuscode["301"] = "Moved Permanently";
        _statuscode["302"] = "Found";
        _statuscode["303"] = "See Other";
        _statuscode["304"] = "Not Modified";
        _statuscode["305"] = "Use Proxy";
        _statuscode["307"] = "Temporary Redirect";
        _statuscode["308"] = "Permanent Redirect";
        _statuscode["400"] = "Bad Request";
        _statuscode["401"] = "Unauthorized";
        _statuscode["402"] = "Payment Required";
        _statuscode["403"] = "Forbidden";
        _statuscode["404"] = "Not Found";
        _statuscode["405"] = "Method Not Allowed";
        _statuscode["406"] = "Not Acceptable";
        _statuscode["407"] = "Proxy Authentication Required";
        _statuscode["408"] = "Request Timeout";
        _statuscode["409"] = "Conflict";
        _statuscode["410"] = "Gone";
        _statuscode["411"] = "Length Required";
        _statuscode["412"] = "Precondition Failed";
        _statuscode["413"] = "Payload Too Large";
        _statuscode["414"] = "Request-URI Too Long";
        _statuscode["415"] = "Unsupported Media Type";
        _statuscode["416"] = "Request Range Not Satisfiable";
        _statuscode["417"] = "Expectation Failed";
        _statuscode["418"] = "Im a teapot";
        _statuscode["420"] = "Enhance Your Calm";
        _statuscode["421"] = "Misdirected Request";
        _statuscode["422"] = "Unprocessable Entity";
        _statuscode["423"] = "Locked";
        _statuscode["424"] = "Failed Dependency";
        _statuscode["425"] = "Too Early";
        _statuscode["426"] = "Upgrade Required";
        _statuscode["429"] = "Too Many Requests";
        _statuscode["431"] = "Request Header Fields Too Large";
        _statuscode["444"] = "No Response";
        _statuscode["450"] = "Blocked by Windows Parental Controls";
        _statuscode["451"] = "Unavailable For Legal Reasons";
        _statuscode["497"] = "HTTP Request Sent to HTTPS Port";
        _statuscode["498"] = "Token expired/invalid";
        _statuscode["499"] = "Client Closed Request";
        _statuscode["500"] = "Internal Server Error";
        _statuscode["501"] = "Not Implemented";
        _statuscode["502"] = "Bad Gateway";
        _statuscode["503"] = "Service Unavailable";
        _statuscode["504"] = "Gateway Timeout";
        _statuscode["506"] = "Variant Also Negotiates";
        _statuscode["507"] = "Insufficient Storage";
        _statuscode["508"] = "Loop Detected";
        _statuscode["509"] = "Bandwidth Limit Exceeded";
        _statuscode["510"] = "Not Extended";
        _statuscode["511"] = "Network Authentication Required";
        _statuscode["521"] = "Web Server Is Down";
        _statuscode["522"] = "Connection Timed Out";
        _statuscode["523"] = "Origin Is Unreachable";
        _statuscode["525"] = "SSL Handshake Failed";
        _statuscode["599"] = "Network Connect Timeout Error";
    }
    ~StatusCode(){}
    std::string convert(std::string extension) {
        if (_statuscode.find(extension) != _statuscode.end())
            return _statuscode[extension];
        return "OK";
    }
    private:
    std::map<std::string, std::string>  _statuscode;
};
