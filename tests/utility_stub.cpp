#include <string>

class Utility {
public:
    static const std::string Stack();
    static std::string l2string(long l);
};

const std::string Utility::Stack() { return std::string(); }
std::string Utility::l2string(long l) { return std::to_string(l); }
