#include <iostream>
#include <string>
#include <vector>

// Check if a single character matches a pattern element
bool match_char(char c, const std::string& pattern_element)
{
    if (pattern_element == "\\d")
    {
        return c >= '0' && c <= '9';
    }
    
    if (pattern_element == "\\w")
    {
        return (c >= 'a' && c <= 'z') || 
               (c >= 'A' && c <= 'Z') || 
               (c >= '0' && c <= '9') || 
               c == '_';
    }
    
    // Negative character group [^abc]
    if (pattern_element.length() >= 4 && 
        pattern_element[0] == '[' && 
        pattern_element[1] == '^' && 
        pattern_element[pattern_element.length() - 1] == ']')
    {
        std::string chars = pattern_element.substr(2, pattern_element.length() - 3);
        return chars.find(c) == std::string::npos;
    }
    
    // Positive character group [abc]
    if (pattern_element.length() >= 3 && 
        pattern_element[0] == '[' && 
        pattern_element[pattern_element.length() - 1] == ']')
    {
        std::string chars = pattern_element.substr(1, pattern_element.length() - 2);
        return chars.find(c) != std::string::npos;
    }
    
    // Literal character
    return pattern_element.length() == 1 && c == pattern_element[0];
}

// Parse pattern into elements (handles \d, \w, [abc], [^abc], and literals)
std::vector<std::string> parse_pattern(const std::string& pattern)
{
    std::vector<std::string> elements;
    size_t i = 0;
    
    while (i < pattern.length())
    {
        if (pattern[i] == '\\' && i + 1 < pattern.length())
        {
            // Escape sequence like \d or \w
            elements.push_back(pattern.substr(i, 2));
            i += 2;
        }
        else if (pattern[i] == '[')
        {
            // Character group [abc] or [^abc]
            size_t end = pattern.find(']', i);
            if (end != std::string::npos)
            {
                elements.push_back(pattern.substr(i, end - i + 1));
                i = end + 1;
            }
            else
            {
                elements.push_back(pattern.substr(i, 1));
                i++;
            }
        }
        else
        {
            // Literal character
            elements.push_back(pattern.substr(i, 1));
            i++;
        }
    }
    
    return elements;
}

// Try to match pattern elements starting at a specific position in input
bool match_at_position(const std::string& input, size_t start, const std::vector<std::string>& elements)
{
    size_t input_pos = start;
    
    for (const auto& element : elements)
    {
        if (input_pos >= input.length())
        {
            return false;
        }
        
        if (!match_char(input[input_pos], element))
        {
            return false;
        }
        
        input_pos++;
    }
    
    return true;
}

bool match_pattern(const std::string& input_line, const std::string& pattern) 
{
    std::string actual_pattern = pattern;
    bool anchor_start = false;
    bool anchor_end = false;
    
    // Check for start anchor ^
    if (!actual_pattern.empty() && actual_pattern[0] == '^')
    {
        anchor_start = true;
        actual_pattern = actual_pattern.substr(1);
    }
    
    // Check for end anchor $
    if (!actual_pattern.empty() && actual_pattern[actual_pattern.length() - 1] == '$')
    {
        anchor_end = true;
        actual_pattern = actual_pattern.substr(0, actual_pattern.length() - 1);
    }
    
    std::vector<std::string> elements = parse_pattern(actual_pattern);
    
    if (elements.empty())
    {
        return true;
    }
    
    // If anchored to start, only try matching at position 0
    if (anchor_start)
    {
        if (anchor_end)
        {
            // Both anchors: pattern must match entire string
            return elements.size() == input_line.length() && 
                   match_at_position(input_line, 0, elements);
        }
        return match_at_position(input_line, 0, elements);
    }
    
    // If anchored to end, match must end at the last character
    if (anchor_end)
    {
        // The match must end exactly at input_line.length()
        // So we need to start at position (input_line.length() - elements.size())
        if (input_line.length() < elements.size())
        {
            return false;
        }
        size_t start_pos = input_line.length() - elements.size();
        return match_at_position(input_line, start_pos, elements);
    }
    
    // Try matching at each position in the input
    for (size_t i = 0; i <= input_line.length(); i++)
    {
        if (match_at_position(input_line, i, elements))
        {
            return true;
        }
    }
    
    return false;
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
