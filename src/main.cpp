#include <iostream>
#include <string>

bool match_pattern(const std::string& input_line, const std::string& pattern) 
{
    // Handle \d character class - matches any digit
    if (pattern == "\\d") 
    {
        for (char c : input_line) 
        {
            if (c >= '0' && c <= '9')
            {
                return true;
            }
        }
        return false;
    }
    
    // Handle \w character class - matches any word character (a-z, A-Z, 0-9, _)
    if (pattern == "\\w") 
    {
        for (char c : input_line) 
        {
            if ((c >= 'a' && c <= 'z') || 
                (c >= 'A' && c <= 'Z') || 
                (c >= '0' && c <= '9') || 
                c == '_')
            {
                return true;
            }
        }
        
        return false;
    }
    
    // Handle positive character groups [abc]
    if (pattern.length() >= 3 && pattern[0] == '[' && pattern[pattern.length() - 1] == ']') 
    {
        std::string chars = pattern.substr(1, pattern.length() - 2);
        for (char c : input_line) 
        {
            if (chars.find(c) != std::string::npos)
            {
                return true;
            }
        }
        return false;
    }
    
    if (pattern.length() == 1) 
    {
        return input_line.find(pattern) != std::string::npos;
    }
    else 
    {
        throw std::runtime_error("Unhandled pattern " + pattern);
    }
}

int main(int argc, char* argv[]) 
{
    // Flush after every std::cout / std::cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    // You can use print statements as follows for debugging, they'll be visible when running tests.
    std::cerr << "Logs from your program will appear here" << std::endl;

    if (argc != 3) 
    {
        std::cerr << "Expected two arguments" << std::endl;
        return 1;
    }

    std::string flag = argv[1];
    std::string pattern = argv[2];

    if (flag != "-E") 
    {
        std::cerr << "Expected first argument to be '-E'" << std::endl;
        return 1;
    }

    // TODO: Uncomment the code below to pass the first stage
    
    std::string input_line;
    std::getline(std::cin, input_line);
    
    try 
    {
        if (match_pattern(input_line, pattern)) 
        {
            return 0;
        } 
        else 
        {
            return 1;
        }
    } 
    catch (const std::runtime_error& e) 
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
