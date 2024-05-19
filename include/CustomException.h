#ifndef CUSTOMEXCEPTION_H
#define CUSTOMEXCEPTION_H

#include <exception>
#include <string>


class CustomException : public std::exception{
private:
    std::string message;
public:
    explicit CustomException(const std::string &msg);
    virtual const char* what() const noexcept override;
};


#endif
