#include "Request.hpp"

void Request::parseAccept() {
    
	if (_headers.find("accept") != _headers.end()) {

        


    }







		// std::string tmp = _headers["range"].front();
		// if (tolower(tmp) != "bytes" && tolower(tmp.substr(0, tmp.find("="))) == "bytes" && tmp.substr(tmp.find("=") + 1).size()) {
		// 	_headers["range"].pop_front();
		// 	_headers["range"].push_front(tmp.substr(tmp.find("=") + 1));
		// 	for (std::list<std::string>::iterator val = _headers["range"].begin(); val != _headers["range"].end(); val++) {
		// 		if ((*val).find("-") == NPOS || *val == "-")
		// 			return (416);
		// 		std::string lhs = (*val).substr(0, (*val).find("-"));
		// 		std::string rhs = (*val).substr((*val).find("-") + 1);
		// 		if (lhs.find_first_not_of("0123456789") != NPOS || rhs.find_first_not_of("0123456789") != NPOS)
		// 			return (416);
		// 		if (lhs.size() && rhs.size()) {
		// 			try {
		// 				if (ft_atoi(lhs) > ft_atoi(rhs))
		// 					return (416);
		// 			} catch (std::exception & e) {
		// 				return (416);
		// 			}
		// 		}
		// 	}
		// } else {
		// 	_headers.erase("range");
		// }
}