#ifndef UTILS_HPP
# define UTILS_HPP
# include <string>
# include <vector>
# include <iostream>
# include <sstream>

std::string numberToString(unsigned long nb);
std::string numberToString(long int nb);
std::string addStringNumbersVector(std::vector<std::string> vct);
std::string addStringNumbers(std::string lhs, std::string rhs);
std::string multiplyStringNumbers(std::string lhs, std::string rhs);
bool        isSuperiorStringNumbers(std::string lhs, std::string rhs);

#endif