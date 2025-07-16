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
        
        // 맨 앞의 번호 패턴 제거 (예: "1. ", "10. " 등)
        int start = 0;
        
        // 공백 건너뛰기
        while (start < line.length() && line[start] == ' ') {
            start++;
        }
        
        // 숫자 찾기
        int numStart = start;
        while (start < line.length() && isdigit(line[start])) {
            start++;
        }
        
        // 점과 공백이 있는지 확인
        if (start > numStart && start < line.length() && line[start] == '.') {
            start++; // 점 건너뛰기
            // 점 다음 공백들 건너뛰기
            while (start < line.length() && line[start] == ' ') {
                start++;
            }
            // 여기서 start가 실제 수식의 시작점
        } else {
            // 번호 패턴이 아니므로 처음부터 시작
            start = 0;
        }
        
        return line.substr(start);
    }
    
    string preprocessExpression(string expr) {
        expr = extractExpression(expr);
        expr = removeSpaces(expr);
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
            case '/': return a / b;
            case '^': {
                double result = 1;
                for (int i = 0; i < (int)b; i++) {
                    result *= a;
                }
                return result;
            }
            default: return 0;
        }
    }
    
    double parseNumber(string str, double x) {
        if (str == "x") return x;
        if (str == "-x") return -x;
        
        // x가 포함된 경우 (예: 2x, -3x)
        if (str.find('x') != string::npos) {
            string coeff = "";
            for (char c : str) {
                if (c != 'x') coeff += c;
            }
            if (coeff == "" || coeff == "+") return x;
            if (coeff == "-") return -x;
            
            double coefficient = 0;
            bool negative = false;
            int i = 0;
            
            if (coeff[0] == '-') {
                negative = true;
                i = 1;
            } else if (coeff[0] == '+') {
                i = 1;
            }
            
            for (; i < coeff.length(); i++) {
                coefficient = coefficient * 10 + (coeff[i] - '0');
            }
            
            return negative ? -coefficient * x : coefficient * x;
        }
        
        // 순수 숫자
        double result = 0;
        bool negative = false;
        int i = 0;
        
        if (str[0] == '-') {
            negative = true;
            i = 1;
        } else if (str[0] == '+') {
            i = 1;
        }
        
        for (; i < str.length(); i++) {
            if (str[i] == '.') continue; // 소수점은 무시 (간단한 구현)
            result = result * 10 + (str[i] - '0');
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
            
            if (isOperator(c) || isOpenBracket(c) || isCloseBracket(c)) {
                if (!currentToken.empty()) {
                    postfix += currentToken + " ";
                    currentToken = "";
                }
                
                if (isOperator(c)) {
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
    
    double evaluatePostfix(string postfix, double x = 1.0) {
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
                        valueStack.push(parseNumber(token, x));
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
        
        // 연산 결과 (x=1로 가정)
        double result = calculator.evaluatePostfix(postfix, 1.0);
        cout << "연산 결과 : " << result << endl;
        
        if (i < n) cout << endl;
    }
    
    inputFile.close();
    return 0;
}