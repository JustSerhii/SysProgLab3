#include <iostream>
#include <vector>
#include <regex>
#include <string>
#include <unordered_set>

using namespace std;

enum class TokenType {
    DELIMITER, PREPROCESSOR_DIRECTIVE, KEYWORD, IDENTIFIER, NUMERIC_CONSTANT, HEXADECIMAL_NUMBER,
    DECIMAL_NUMBER, STRING_CONSTANT, OPERATOR, COMMENT, UNKNOWN
};

struct Token {
    string value;
    TokenType type;
};

unordered_set<string> keywords;
unordered_set<string> operators;
unordered_set<string> delimiters;


bool isIdentifier(const string& token) {
    regex identifierRegex("[_a-zA-Z][_a-zA-Z0-9]*");
    return regex_match(token, identifierRegex);
}

bool isKeyword(const string& token) {
    return keywords.find(token) != keywords.end();
}

bool isStringConstant(const string& token) {
    regex stringRegex("\"([^\"\\\\]|\\\\.)*\"");
    return regex_match(token, stringRegex);
}

bool isDecimalNumber(const string& token) {
    regex decimalPattern("^\\d*\\.\\d+|\\d+\\.$");
    return regex_match(token, decimalPattern);
}

bool isHexadecimalNumber(const string& token) {
    regex hexPattern("^0[xX][0-9a-fA-F]+$");
    return regex_match(token, hexPattern);
}

bool isNumericConstant(const string& token) {
    regex numericRegex("([0-9]*\\.[0-9]+|[0-9]+)");
    return regex_match(token, numericRegex);
}

void initializeKeywordSet() {
    const std::string keywordString = "catch ref do struct goto bool extern implicit byte switch in out lock sbyte sealed true string foreach try this throw ulong short volatile default object params namespace uint event base break override false using float for char private continue explicit interface virtual sizeof static new double long unchecked case const finally abstract protected while true operator if class readonly int enum public checked decimal true";
    istringstream ss(keywordString);
    string keyword;
    while (ss >> keyword) {
        keywords.insert(keyword);
    }
}

void initializeOperatorSet() {
    const string operatorString = "+ - % & * / | ^ ! ~ -- ++ << >> == != > < <= >= || && ?? ?: => = += -= *= /= %= &= |= ^= <<= >>= [] () . ->";
    istringstream ss(operatorString);
    string op;
    while (ss >> op) {
        operators.insert(op);
    }
}

void initializeDelimiterSet() {
    const string delimiterStr = "; , { } ( ) [ ] : ?";
    istringstream ss(delimiterStr);
    string delimiter;
    while (ss >> delimiter) {
        delimiters.insert(delimiter);
    }
}

vector<Token> tokenize(const string& code) {
    initializeKeywordSet();
    initializeOperatorSet();
    initializeDelimiterSet();

    vector<Token> tokens;
    string token;
    bool inString = false;
    bool inSingleLineComment = false;
    bool inMultiLineComment = false;

    for (size_t i = 0; i < code.length(); ++i) {
        char c = code[i];

        if (c == '#' && !inString && !inSingleLineComment && !inMultiLineComment) {
            token += c;
            while (i + 1 < code.length() && !isspace(code[i + 1])) {
                token += code[++i];
            }
            tokens.push_back({ token, TokenType::PREPROCESSOR_DIRECTIVE });
            token.clear();
            continue;
        }

        if (c == '/' && !inString && !inMultiLineComment) {
            if (i + 1 < code.length()) {
                if (code[i + 1] == '/') inSingleLineComment = true;
                else if (code[i + 1] == '*') inMultiLineComment = true;
            }
        }

        if (inSingleLineComment && c == '\n') {
            inSingleLineComment = false;
            tokens.push_back({ token, TokenType::COMMENT });
            token.clear();
        }

        if (inMultiLineComment && c == '*' && i + 1 < code.length() && code[i + 1] == '/') {
            inMultiLineComment = false;
            token += "*/";
            tokens.push_back({ token, TokenType::COMMENT });
            token.clear();
            ++i;
            continue;
        }

        if (inSingleLineComment || inMultiLineComment) {
            token += c;
            continue;
        }

        if (c == '\"') {
            if (inString) {
                token += c;
                tokens.push_back({ token, TokenType::STRING_CONSTANT });
                token.clear();
            } else {
                if (!token.empty()) {
                    tokens.push_back({ token, TokenType::UNKNOWN });
                    token.clear();
                }
            }
            inString = !inString;
        } else if (inString) {
            token += c;
        } else if (isspace(c)) {
            if (!token.empty()) {
                tokens.push_back({ token, TokenType::UNKNOWN });
                token.clear();
            }
        } else if (ispunct(c)) {
            if (!token.empty()) {
                tokens.push_back({ token, TokenType::UNKNOWN });
                token.clear();
            }
            tokens.push_back({ string(1, c), TokenType::UNKNOWN });
        } else {
            token += c;
        }
    }


    if (!token.empty()) {
        tokens.push_back({ token, TokenType::UNKNOWN });
    }

    for (Token& t : tokens) {
        if (isKeyword(t.value)) t.type = TokenType::KEYWORD;
        else if (isHexadecimalNumber(t.value)) t.type = TokenType::HEXADECIMAL_NUMBER;
        else if (isIdentifier(t.value)) t.type = TokenType::IDENTIFIER;
        else if (isNumericConstant(t.value)) t.type = TokenType::NUMERIC_CONSTANT;
        else if (isStringConstant(t.value)) t.type = TokenType::STRING_CONSTANT;
        else if (isDecimalNumber(t.value)) t.type = TokenType::DECIMAL_NUMBER;
        else if (delimiters.find(t.value) != delimiters.end()) t.type = TokenType::DELIMITER;
        else if (operators.find(t.value) != operators.end()) t.type = TokenType::OPERATOR;
        else if (t.value.front() == '#') t.type = TokenType::PREPROCESSOR_DIRECTIVE;
    }
    return tokens;
}

void displayTokens(const vector<Token>& tokens) {

    for (const Token& token : tokens) {
        cout << "< " << token.value << " | ";
        switch (token.type) {
            case TokenType::KEYWORD: cout << "Keyword"; break;
            case TokenType::STRING_CONSTANT: cout << "String Constant"; break;
            case TokenType::NUMERIC_CONSTANT: cout << "Numeric Constant"; break;
            case TokenType::IDENTIFIER: cout << "Identifier"; break;
            case TokenType::HEXADECIMAL_NUMBER: cout << "Hexadecimal Number"; break;
            case TokenType::OPERATOR: cout << "Operator"; break;
            case TokenType::DELIMITER: cout << "Delimiter"; break;
            case TokenType::DECIMAL_NUMBER: cout << "Decimal Number"; break;
            case TokenType::COMMENT: cout << "Comment"; break;
            case TokenType::PREPROCESSOR_DIRECTIVE: cout << "Preprocessor Directive"; break;
            case TokenType::UNKNOWN: cout << "Unknown"; break;
        }
        cout << " >" << endl;
    }
}

int main() {
    string code = R"(
    using System;
    #nullable disable

    class Program {
        static void Main(string[] args) {
            Console.WriteLine("Hello, C#");
            int num1 = 42;
            int num2 = 16;
            int result = 0;

            result = num1 + num2; // addition
            /*multi
            line*/
            result = num1 - num2;
            result = num1 * num2;
            result = num1 / num2;

            if (result > 10) {
                Console.WriteLine("Result is greater than 10.");
            } else {
                Console.WriteLine("Result is not greater than 10.");
            }
            int invalidHexNumber = 0xG1;
            int hexNumber = 0x1A;
        }
    }
    )";
    vector<Token> tokens = tokenize(code);


    displayTokens(tokens);
    return 0;
}
