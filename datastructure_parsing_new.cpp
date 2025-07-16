#include <iostream>
#include <fstream>
#include <stack>
#include <string>
#include <cctype>

using namespace std;

// 파싱 클래스 - 문자열 처리 담당
class Parser {
public:
    string removeSpaces(string str) {
        string result = "";
        for (char c : str) {
            if (c != ' ') result += c;
        }
        return result;
    }
    
    string replaceDoubleStar(string str) {
        string result = "";
        for (int i = 0; i < str.length(); i++) {
            if (i < str.length() - 1 && str[i] == '*' && str[i + 1] == '*') {
                result += '^';
                i++; // 다음 * 건너뛰기
            } else {
                result += str[i];
            }
        }
        return result;
    }
    
    string extractExpression(string line) {
        // "답:" 부분 제거
        int pos = line.find("답:");
        if (pos != string::npos) {
            line = line.substr(0, pos);
        }
        
        // 앞뒤 공백 제거
        int start = 0, end = line.length() - 1;
        while (start < line.length() && line[start] == ' ') start++;
        while (end >= 0 && line[end] == ' ') end--;
        
        if (start <= end) {
            return line.substr(start, end - start + 1);
        }
        return "";
    }
    
    string preprocessExpression(string expr) {
        expr = extractExpression(expr);
        expr = replaceDoubleStar(expr);
        return expr;
    }
};

// 계산 클래스 - 스택 기반 연산 담당
class Calculator {
private:
    int getPrecedence(char op) {
        switch (op) {
            case '+':
            case '-': return 1;
            case '*':
            case '/': return 2;
            case '^': return 3;
            default: return 0;
        }
    }
    
    bool isOperator(char c) {
        return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
    }
    
    bool isOpenBracket(char c) {
        return c == '(' || c == '[' || c == '{';
    }
    
    bool isCloseBracket(char c) {
        return c == ')' || c == ']' || c == '}';
    }
    
    double calculate(double a, double b, char op) {
        switch (op) {
            case '+': return a + b;
            case '-': return a - b;
            case '*': return a * b;
            case '/': return b != 0 ? a / b : 0;
            case '^': {
                double result = 1;
                int exp = (int)b;
                if (exp >= 0) {
                    for (int i = 0; i < exp; i++) {
                        result *= a;
                    }
                } else {
                    for (int i = 0; i < -exp; i++) {
                        result /= a;
                    }
                }
                return result;
            }
            default: return 0;
        }
    }
    
    double parseNumber(string str) {
        if (str.empty()) return 0;
        
        double result = 0;
        double decimal = 0.1;
        bool negative = false;
        bool afterDecimal = false;
        int i = 0;
        
        if (str[0] == '-') {
            negative = true;
            i = 1;
        } else if (str[0] == '+') {
            i = 1;
        }
        
        for (; i < str.length(); i++) {
            if (str[i] == '.') {
                afterDecimal = true;
            } else if (isdigit(str[i])) {
                if (afterDecimal) {
                    result += (str[i] - '0') * decimal;
                    decimal *= 0.1;
                } else {
                    result = result * 10 + (str[i] - '0');
                }
            }
        }
        
        return negative ? -result : result;
    }

public:
    string infixToPostfix(string expr) {
        stack<char> opStack;
        string postfix = "";
        string currentToken = "";
        
        for (int i = 0; i < expr.length(); i++) {
            char c = expr[i];
            
            if (c == ' ') continue; // 공백 무시
            
            if (isOperator(c) || isOpenBracket(c) || isCloseBracket(c)) {
                if (!currentToken.empty()) {
                    postfix += currentToken + " ";
                    currentToken = "";
                }
                
                if (isOperator(c)) {
                    // 단항 연산자 처리 (맨 앞이나 연산자/여는괄호 다음의 +/-)
                    if ((c == '+' || c == '-') && 
                        (i == 0 || isOperator(expr[i-1]) || isOpenBracket(expr[i-1]))) {
                        currentToken += c; // 단항 연산자는 숫자에 포함
                        continue;
                    }
                    
                    while (!opStack.empty() && 
                           isOperator(opStack.top()) &&
                           getPrecedence(opStack.top()) >= getPrecedence(c)) {
                        postfix += opStack.top();
                        postfix += " ";
                        opStack.pop();
                    }
                    opStack.push(c);
                }
                else if (isOpenBracket(c)) {
                    opStack.push(c);
                }
                else if (isCloseBracket(c)) {
                    while (!opStack.empty() && !isOpenBracket(opStack.top())) {
                        postfix += opStack.top();
                        postfix += " ";
                        opStack.pop();
                    }
                    if (!opStack.empty()) opStack.pop(); // 여는 괄호 제거
                }
            }
            else {
                currentToken += c;
            }
        }
        
        if (!currentToken.empty()) {
            postfix += currentToken + " ";
        }
        
        while (!opStack.empty()) {
            postfix += opStack.top();
            postfix += " ";
            opStack.pop();
        }
        
        return postfix;
    }
    
    double evaluatePostfix(string postfix) {
        stack<double> valueStack;
        string token = "";
        
        for (int i = 0; i < postfix.length(); i++) {
            char c = postfix[i];
            
            if (c == ' ') {
                if (!token.empty()) {
                    if (token.length() == 1 && isOperator(token[0])) {
                        if (valueStack.size() >= 2) {
                            double b = valueStack.top(); valueStack.pop();
                            double a = valueStack.top(); valueStack.pop();
                            valueStack.push(calculate(a, b, token[0]));
                        }
                    } else {
                        valueStack.push(parseNumber(token));
                    }
                    token = "";
                }
            } else {
                token += c;
            }
        }
        
        return valueStack.empty() ? 0 : valueStack.top();
    }
};

int main() {
    ifstream inputFile("input.txt");
    if (!inputFile.is_open()) {
        cout << "파일을 열 수 없습니다." << endl;
        return 1;
    }
    
    Parser parser;
    Calculator calculator;
    
    int n;
    inputFile >> n;
    inputFile.ignore();
    
    for (int i = 1; i <= n; i++) {
        string line;
        getline(inputFile, line);
        
        // 파싱
        string expression = parser.preprocessExpression(line);
        
        cout << "== " << i << "번째 다항식 ==" << endl;
        cout << "중위 표기 수식 : " << expression << endl;
        
        // 중위 → 후위 변환
        string postfix = calculator.infixToPostfix(expression);
        cout << "후위 표기 수식 : " << postfix << endl;
        
        // 연산 결과
        double result = calculator.evaluatePostfix(postfix);
        cout << "연산 결과 : " << result << endl;
        
        if (i < n) cout << endl;
    }
    
    inputFile.close();
    return 0;
}