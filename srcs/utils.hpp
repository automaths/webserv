#ifndef UTILS_HPP
# define UTILS_HPP
# include <string>
# include <vector>
# include <iostream>
# include <sstream>

// Operation on String Numbers, extremely handy to avoid overflows
std::string numberToString(unsigned long nb);
std::string numberToString(long int nb);
std::string addStringNumbersVector(std::vector<std::string> vct);
std::string addStringNumbers(std::string lhs, std::string rhs);
std::string multiplyStringNumbers(std::string lhs, std::string rhs);
bool        isSuperiorStringNumbers(std::string lhs, std::string rhs);

// Splits
std::vector<std::string>    splitWithEmpty(std::string src, std::string tokens);
std::vector<std::string>    splitNoEmpty(std::string src, std::string tokens);

#endif