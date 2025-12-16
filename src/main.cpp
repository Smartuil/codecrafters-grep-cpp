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

// Structure to hold a pattern element and its quantifier
struct PatternElement
{
    std::string pattern;
    char quantifier; // '\0' for none, '+' for one or more
};

// Parse pattern into elements (handles \d, \w, [abc], [^abc], literals, and + quantifier)
std::vector<PatternElement> parse_pattern(const std::string& pattern)
{
    std::vector<PatternElement> elements;
    size_t i = 0;
    
    while (i < pattern.length())
    {
        PatternElement elem;
        elem.quantifier = '\0';
        
        if (pattern[i] == '\\' && i + 1 < pattern.length())
        {
            // Escape sequence like \d or \w
            elem.pattern = pattern.substr(i, 2);
            i += 2;
        }
        else if (pattern[i] == '[')
        {
            // Character group [abc] or [^abc]
            size_t end = pattern.find(']', i);
            if (end != std::string::npos)
            {
                elem.pattern = pattern.substr(i, end - i + 1);
                i = end + 1;
            }
            else
            {
                elem.pattern = pattern.substr(i, 1);
                i++;
            }
        }
        else
        {
            // Literal character
            elem.pattern = pattern.substr(i, 1);
            i++;
        }
        
        // Check for quantifier
        if (i < pattern.length() && pattern[i] == '+')
        {
            elem.quantifier = '+';
            i++;
        }
        
        elements.push_back(elem);
    }
    
    return elements;
}

// Try to match pattern elements starting at a specific position in input
// Returns the position after the match, or -1 if no match
int match_at_position(const std::string& input, size_t start, const std::vector<PatternElement>& elements)
{
    size_t input_pos = start;
    
    for (const auto& element : elements)
    {
        if (input_pos >= input.length())
        {
            return -1;
        }
        
        if (element.quantifier == '+')
        {
            // One or more: must match at least once
            if (!match_char(input[input_pos], element.pattern))
            {
                return -1;
            }
            input_pos++;
            
            // Greedy: match as many as possible
            while (input_pos < input.length() && match_char(input[input_pos], element.pattern))
            {
                input_pos++;
            }
        }
        else
        {
            // No quantifier: match exactly once
            if (!match_char(input[input_pos], element.pattern))
            {
                return -1;
            }
            input_pos++;
        }
    }
    
    return static_cast<int>(input_pos);
}

// Check if match succeeds at position (returns true/false)
bool match_at_position_bool(const std::string& input, size_t start, const std::vector<PatternElement>& elements)
{
    return match_at_position(input, start, elements) != -1;
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
    
    std::vector<PatternElement> elements = parse_pattern(actual_pattern);
    
    if (elements.empty())
    {
        return true;
    }
    
    // If anchored to start, only try matching at position 0
    if (anchor_start)
    {
        int end_pos = match_at_position(input_line, 0, elements);
        if (end_pos == -1)
        {
            return false;
        }
        if (anchor_end)
        {
            // Both anchors: match must consume entire string
            return static_cast<size_t>(end_pos) == input_line.length();
        }
        return true;
    }
    
    // If anchored to end, match must end at the last character
    if (anchor_end)
    {
        // Try matching from each position and check if it ends at the string end
        for (size_t i = 0; i <= input_line.length(); i++)
        {
            int end_pos = match_at_position(input_line, i, elements);
            if (end_pos != -1 && static_cast<size_t>(end_pos) == input_line.length())
            {
                return true;
            }
        }
        return false;
    }
    
    // Try matching at each position in the input
    for (size_t i = 0; i <= input_line.length(); i++)
    {
        if (match_at_position_bool(input_line, i, elements))
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
