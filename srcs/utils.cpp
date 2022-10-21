#include "utils.hpp"

std::string numberToString(unsigned long nb)
{
    std::stringstream result;

    result << nb;
    return (result.str());
}

std::string numberToString(long int nb)
{
    std::stringstream result;

    result << nb;
    return (result.str());
}

std::string addStringNumbersVector(std::vector<std::string> vct)
{
    bool            carry = false;
    long int        size;
    long int        current_size;
    std::string     result;

    if (vct.size() == 0)
        return (std::string("0"));
    result = vct[0];
    for (std::vector<std::string>::iterator first = (vct.begin() + 1); first != vct.end(); first++)
    {
        size = static_cast<long int>(first->size());
        current_size = static_cast<long int>(result.size());
        if (size < current_size)
        {
            while (size < current_size)
            {
                first->insert(0, 1, '0');
                size++;
            }
        }
        else
        {
            while (current_size < size)
            {
                result.insert(0, 1, '0');
                current_size++;
            }
        }
        for (long int i = 0; i < size; i++)
        {
            if (carry)
            {
                (*first)[size - 1 - i] += 1;
                if ((*first)[size - 1 - i] == 58)
                {
                    carry = true;
                    (*first)[size - 1 - i] = '0';
                }
                else
                    carry = false;
            }
            result[size - 1 - i] += ((*first)[size - 1 - i] - '0');
            if (result[size - 1 - i] > '9')
            {
                carry = true;
                result[size - 1 - i] = result[size - 1 - i] - 58 + '0';
            }
        }
        if (carry)
        {
            result.insert(0, 1, '1');
        }
    }
    return (result);
}

std::string addStringNumbers(std::string lhs, std::string rhs)
{
    bool            carry = false;
    long int        sizeLhs;
    long int        sizeRhs;

    sizeLhs = static_cast<long int>(lhs.size());
    sizeRhs = static_cast<long int>(rhs.size());
    if (sizeLhs == 0)
        return (rhs);
    if (sizeRhs == 0)
        return (lhs);
    if (sizeLhs < sizeRhs)
    {
        while (sizeLhs < sizeRhs)
        {
            lhs.insert(0, 1, '0');
            sizeLhs++;
        }
    }
    else
    {
        while (sizeRhs < sizeLhs)
        {
            rhs.insert(0, 1, '0');
            sizeRhs++;
        }
    }
    for (long int i = 0; i < sizeRhs; i++)
    {
        if (carry)
        {
            lhs[sizeLhs - 1 - i] += 1;
            if (lhs[sizeLhs - 1 - i] == 58)
            {
                carry = true;
                lhs[sizeLhs - 1 - i] = '0';
            }
            else
                carry = false;
        }
        lhs[sizeLhs - 1 - i] += (rhs[sizeLhs - 1 - i] - '0');
        if (lhs[sizeLhs - 1 - i] > '9')
        {
            carry = true;
            lhs[sizeLhs - 1 - i] = lhs[sizeLhs - 1 - i] - 58 + '0';
        }
    }
    if (carry)
    {
        lhs.insert(0, 1, '1');
    }
    if (lhs[0] == '0')
    {
        if (lhs.find_first_not_of('0') == std::string::npos)
        {
            return (std::string("0"));
        }
        lhs = lhs.erase(0, lhs.find_first_not_of('0'));
    }
    return (lhs);
}

std::string multiplyStringNumbers(std::string lhs, std::string rhs)
{
    int                         carry;
    int                         current_digit;
    int                         current_result;
    std::string                 current;
    std::vector<std::string>    result;

    result.reserve(rhs.size());
    for (unsigned int i = 0; i < rhs.size(); i++)
    {
        result.push_back(std::string(""));
        current_digit = rhs[rhs.size() - 1 - i] - '0';
        carry = 0;
        for (unsigned int j = 0; j < lhs.size(); j++)
        {
            current_result = (lhs[lhs.size() - j - 1] - '0') * current_digit + carry;
            carry = current_result / 10;
            current_result %= 10;
            result[i].insert(0, 1, (current_result + '0'));
        }
        if (carry)
        {
            result[i].insert(0, 1, (carry + '0'));
        }
        for (unsigned int j = 0; j < i; j++)
            result[i].push_back('0');
    }
    for (std::vector<std::string>::iterator a = result.begin(); a != result.end(); a++)
    {
        std::cerr << *a << std::endl;
    }
    return (addStringNumbersVector(result));
}


bool isSuperiorStringNumbers(std::string lhs, std::string rhs)
{
    std::cerr << "Numbers to compare: " << lhs << ">" << rhs << "?" << std::endl;
    if (rhs.size() == 0 && lhs.size())
        return (true);
    if ((lhs.size() == 0 && rhs.size()) || (rhs.size() == 0 && lhs.size() == 0))
        return (false);
    if (rhs[0] == '0')
    {
        if (rhs.find_first_not_of('0') == std::string::npos)
            rhs = "";
        else
            rhs.erase(0, rhs.find_first_not_of('0'));
    }
    if (lhs[0] == '0')
    {
        if (lhs.find_first_not_of('0') == std::string::npos)
            lhs = "";
        else
            lhs.erase(0, lhs.find_first_not_of('0'));
    }
    if (lhs.size() > rhs.size())
        return (true);
    if ((rhs.size() > lhs.size()) || (rhs.size() == 0 && lhs.size() == 0))
        return (false);
    for (std::string::size_type i = 0; i < lhs.size(); i++)
    {
        if (lhs[i] > rhs[i])
            return (true);
        if (rhs[i] > lhs[i])
            return (false);
    }
    return (false);
}

std::vector<std::string>    splitWithEmpty(std::string src, std::string tokens)
{
    std::string                 tmp;
    std::string::size_type      current;
    std::vector<std::string>    result;

    if (src.size() == 0)
        return(result);
    if (tokens.size() == 0)
    {
        result.push_back(src);
        return (result);
    }
    while ((current = src.find_first_of(tokens)) != std::string::npos)
    {
        tmp = src.substr(0, current);
        result.push_back(tmp);
        src.erase(0, (current + 1));
    }
    result.push_back(src);
    return (result);
}

std::vector<std::string>    splitNoEmpty(std::string src, std::string tokens)
{
    std::string                 tmp;
    std::string::size_type      current;
    std::vector<std::string>    result;

    if (src.size() == 0)
        return(result);
    if (tokens.size() == 0)
    {
        result.push_back(src);
        return (result);
    }
    while ((current = src.find_first_of(tokens)) != std::string::npos)
    {
        tmp = src.substr(0, current);
        if (tmp.size())
            result.push_back(tmp);
        src.erase(0, (current + 1));
    }
    if (src.size())
        result.push_back(src);
    return (result);
}