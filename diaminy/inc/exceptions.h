#ifndef DIAMINY_EXCEPTIONS_H
#define DIAMINY_EXCEPTIONS_H

class no_solution_exception : public std::exception {

};

class assertion_error : public std::exception {
private:
    const char *message;

public:
    inline assertion_error(const char *message) : message(message) {}

    const char *what() const noexcept override;
};

const char *assertion_error::what() const noexcept {
    return message;
}

#endif
