#include "frost.h"
#include <algorithm>
#include <cctype>
#include <vector>
#include <io.h>

#ifdef __linux__

#include <unistd.h>

#endif // __linux__

namespace Frost {

    ///------------------------------------------------------------------------
    /// endsWidth - Checks if a string ends with the specified string
    /// @param string to check
    /// @param value to check for
    /// @return true if the string ends with the value given, false otherwise
    ///------------------------------------------------------------------------

    bool endsWith(const std::string& str, const std::string& val) {

        if(val.length() > str.length()) {
            // String we are searching for is too big to be found.
            return false;
        }
        const size_t pos = str.length() - val.length();

        return !str.compare(pos, val.length(), val);

    }

    ///------------------------------------------------------------------------
    /// startsWith - Checks if a string ends with the specified string
    /// @param string to check
    /// @param value to check for
    /// @return true if the string starts with the value given, false otherwise
    ///------------------------------------------------------------------------

    bool startsWith(const std::string& str, const std::string& val) {

        if(val.length() > str.length()) {
            // String we are searching for is too big to be found.
            return false;
        }

        return !str.compare(0, val.length(), val);

    }

    ///------------------------------------------------------------------------
    /// split - Split the string into a collection of strings where it finds
    /// the given delimiter.
    /// @param string to check
    /// @param value to split on
    /// @param limit on how many splits to do.
    /// @return a vector containing the tokenized strings
    ///------------------------------------------------------------------------

    std::vector<std::string> split(const std::string& str, const char& delimiter,
                                   const size_t& limit) {

        size_t startPos         = 0;
        size_t tokensParsed     = 0;
        std::vector<std::string> out;
        std::string token;

        if(limit == 0) {
            return out;  // Nothing to do here.
        }

        if(limit != -1) {
            out.reserve(limit);
        }

        while(true) {

            if(tokensParsed == limit) {
                // Limit hit.
                return out;
            }

            // Find next delimiter
            const size_t pos = str.find(delimiter, startPos);

            if(pos != std::string::npos) {
                
                // Extract the token, add it to the vector, and indicate that
                // we parsed a token.
                token = str.substr(startPos, pos - startPos);
                out.push_back(token);
                startPos = pos + 1;
                tokensParsed++;

            }
            else {
                break; // No more delimiters found
            }
        }

        // Copy the rest of the string as a token, that is, if there is
        // anything to copy.
        if(startPos < str.length()) {
            token = str.substr(startPos, std::string::npos);
            out.push_back(token);
        }

        return out;
    }

    ///------------------------------------------------------------------------
    /// toLower - Lowercases a string.
    /// @param string to lower case
    /// @return the string in all lower case.
    ///------------------------------------------------------------------------

    std::string toLower(const std::string& str) {
        std::string out;
        out.resize(str.size());
        std::transform(str.begin(), str.end(), out.begin(), tolower);
        return out;
    }

    ///------------------------------------------------------------------------
    /// toUpper - Uppercases a string.
    /// @param string to Upper case
    /// @return the string in all upper case.
    ///------------------------------------------------------------------------

    std::string toUpper(const std::string& str) {
        std::string out;
        out.resize(str.size());
        std::transform(str.begin(), str.end(), out.begin(), toupper);
        return out;
    }

    ///------------------------------------------------------------------------
    /// ltrim - Removes the string given from the start of the string given
    /// @param string to trim
    /// @param string to remove
    /// @return the string trimmed of the string
    ///------------------------------------------------------------------------

    std::string ltrim(const std::string& str, const std::string& needle) {
        
        if(str.length() == 0 || needle.length() == 0) {
            return str;
        }
        
        const size_t firstPos = str.find_first_not_of(needle);

        if(firstPos == std::string::npos) {
            // String contains only the characters in the needle, so it's
            // effectively empty.
            return "";
        }

        return str.substr(firstPos, str.length() - firstPos);

    }

    ///------------------------------------------------------------------------
    /// rtrim - Removes the string given from the end of the string given
    /// @param string to trim
    /// @param string to remove
    /// @return the string trimmed of the string
    ///------------------------------------------------------------------------

    std::string rtrim(const std::string& str, const std::string& needle) {
        
        if(str.length() == 0 || needle.length() == 0) {
            return str;
        }
        
        const size_t lastPos = str.find_last_not_of(needle);

        if(lastPos == std::string::npos) {
            // String contains only the characters in the needle, so it's
            // effectively empty.
            return "";
        }

        return str.substr(0, lastPos+1);

    }

    ///------------------------------------------------------------------------
    /// readVBString - Reads a VB String.
    /// @param Reference to an input stream
    /// @param string to be read into
    /// @throws IOException Error if a string could not be read
    ///------------------------------------------------------------------------

    std::string Frost::readVBString(std::istream& is) {

        // Visual Basic Escapes Double Quotes, with a second Double Quote.
        // So as such, we need to locate the true end quote: <"><CR><LF>
        
        std::string outStr;
        std::string curLine;
        bool firstLine = true;


        do {

            // So the first thing to do is get the next line.

            std::getline(is, curLine);

            if (!is.fail()) {

                if (!is.eof()) {
                    // We need to ensure that the new line is present in the string read.
                    curLine.push_back('\n');
                }
                else {
                    throw std::runtime_error("Reached end of file without finding the end of the string.");
                }

                // If this is the first line read, we need to do some checks
                if (firstLine) {
                    
                    // For instance, it must start with a double quote
                    if (curLine[0] != '\"') {
                        throw std::runtime_error("Value read was not a valid Visual Basic String.");
                    }
                    else if (curLine.size() == 4) {
                        // And we know if it's exactly 4 characters long that
                        // this is an empty string, and that double quotes here
                        // is not an escape.
                        if (!curLine.compare("\"\"\r\n")) {
                            return ""; // Empty String
                        }
                    }
                    
                    // Valid string, so remove the starting quote.
                    outStr = curLine.substr(1, std::string::npos);
                    curLine = outStr;
                    firstLine = false;

                }
                else {
                    outStr += curLine;
                }

                bool endString = false;

                // Now we need to begin our search for the end quote,
                // we will search each new line for a quote that has no
                // second quote directly left of it.

                if (curLine.size() > 2) {
                    const size_t numChars = curLine.size() - 2;
                    for (int i = numChars; i != 0; --i) {
                        
                        // If we find a quote once, it is possibly the end
                        // but if we find another one space to the left,
                        // the we can assume that it's a escaped double quote
                        if (curLine[i-1] == '"') {
                            
                            endString = !endString;
                        }
                        else if (endString) {
                            // Found!
                            break;
                        }
                    }
                }

                // Since we found the end of the string, we can chop off the
                // last three characters.
                if (endString) {
                    outStr = outStr.substr(0, outStr.size() - 3);
                    break;
                }

            }
            else {
                throw std::runtime_error("Failed to read from input stream.");
            }

        } while (!is.eof() && !is.fail());

        return outStr;
    }

    ///------------------------------------------------------------------------
    /// writeVBInteger - Writes an integer to a stream the same way Visual
    /// Basic would.
    /// @param Output stream to write to
    /// @param integer to write
    ///------------------------------------------------------------------------

    void writeVBInteger(std::ostream& os, const int32_t& intVal) {

        if (intVal > -1) {
            os.write(" ", 1);
        }

        os << intVal;
        os.write(" \r\n", 3);
    }

    ///------------------------------------------------------------------------
    /// writeVBLine - Writes a single line string to a file just like Visual
    /// Basic's "Print" command would.
    /// @param Output stream to write to
    /// @param single line string to write
    ///------------------------------------------------------------------------

    void writeVBLine(std::ostream& os, const std::string& line) {
        os.write(&line[0], line.length());
        os.write("\r\n", 2);
    }

    ///------------------------------------------------------------------------
    /// writeVBString - Writes a string to the file just like Visual Basic's
    /// "Write" command would.
    /// @param Output stream to write to
    /// @param String to write.
    ///------------------------------------------------------------------------

    void writeVBString(std::ostream& os, const std::string& str) {
        
        // TODO: Properly escape Quotes
        os.write("\"", 1);
        os.write(&str[0], str.length());
        os.write("\"\r\n", 3);
    }

    ///------------------------------------------------------------------------
    /// DoesFileExist - Checks if a file exists
    /// @return true if the file exists, false if it does not
    ///------------------------------------------------------------------------
    
#ifdef _WIN32
    
    bool doesFileExist(const std::string& fullPath) {
        return _access_s(fullPath.c_str(), 0) ? false : true;
    }
    
#elif __linux__

    bool doesFileExist(const std::string& filePath) {
        return !access(fullPath.c_str(), F_OK);
    }
    
#endif  // __WIN32__

}
