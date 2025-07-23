#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
using namespace std;

// 트리 노드 구조체
struct TreeNode {
    string name;
    vector<TreeNode*> children;
    TreeNode* parent;
    
    TreeNode(string n) : name(n), parent(nullptr) {}
};

class JoseonDynastyTree {
private:
    TreeNode* root;
    map<string, TreeNode*> nodeMap; // 빠른 검색을 위한 맵
    vector<string> kingOrder; // 왕의 순서를 저장
    
public:
    JoseonDynastyTree() : root(nullptr) {}
    
    // 파일에서 트리 구성
    void buildTreeFromFile(const string& filename) {
        ifstream file(filename);
        string line;
        
        if (!file.is_open()) {
            cout << "파일을 열 수 없습니다: " << filename << endl;
            return;
        }
        
        // 첫 번째 줄에서 루트 노드 읽기
        if (getline(file, line)) {
            root = new TreeNode(line);
            nodeMap[line] = root;
            kingOrder.push_back(line);
        }
        
        // 나머지 줄에서 자식-부모 관계 읽기
        while (getline(file, line)) {
            if (line.empty()) continue;
            
            istringstream iss(line);
            string child, parent;
            
            if (iss >> child >> parent) {
                // 노드가 없으면 생성
                if (nodeMap.find(child) == nodeMap.end()) {
                    nodeMap[child] = new TreeNode(child);
                }
                if (nodeMap.find(parent) == nodeMap.end()) {
                    nodeMap[parent] = new TreeNode(parent);
                }
                
                // 부모-자식 관계 설정
                nodeMap[parent]->children.push_back(nodeMap[child]);
                nodeMap[child]->parent = nodeMap[parent];
                
                kingOrder.push_back(child);
            }
        }
        
        // 왕위 계승 순서에 따른 추가 시각화 정보 출력
        cout << "\n=== 왕위 계승 순서와 부모-자식 관계 ===" << endl;
        for (int i = 0; i < kingOrder.size(); i++) {
            string currentKing = kingOrder[i];
            TreeNode* currentNode = nodeMap[currentKing];
            
            cout << (i + 1) << ". " << currentKing;
            
            if (currentNode->parent) {
                cout << " ← " << currentNode->parent->name;
                
                // 이전 왕과 부모가 다른 경우 명시
                if (i > 0 && kingOrder[i-1] != currentNode->parent->name) {
                    cout << " (선조로부터 계승)";
                }
            } else {
                cout << " (시조)";
            }
            cout << endl;
        }
        
        file.close();
    }
    
    // 트리 시각화 (탑다운, 버텍스 포함)
    void printTree() {
        if (root) {
            cout << "\n=== 조선왕조 트리 (탑다운) ===" << endl;
            printTreeHelper(root, 0, "", true);
        }
    }
    
private:
    void printTreeHelper(TreeNode* node, int depth, string prefix, bool isLast) {
        if (!node) return;
        
        // 현재 노드 출력 (왕 번호 포함)
        cout << prefix;
        if (depth > 0) {
            cout << (isLast ? "└── " : "├── ");
        }
        
        // 왕 번호 찾기
        int kingNumber = -1;
        for (int i = 0; i < kingOrder.size(); i++) {
            if (kingOrder[i] == node->name) {
                kingNumber = i + 1;
                break;
            }
        }
        
        if (kingNumber != -1) {
            cout << kingNumber << ". " << node->name << endl;
        } else {
            cout << node->name << endl;
        }
        
        // 자식 노드들 출력
        for (int i = 0; i < node->children.size(); i++) {
            bool isLastChild = (i == node->children.size() - 1);
            string newPrefix = prefix;
            if (depth > 0) {
                newPrefix += (isLast ? "    " : "│   ");
            }
            printTreeHelper(node->children[i], depth + 1, newPrefix, isLastChild);
        }
    }
    
public:
    // 1. 조선의 왕을 순서대로 출력
    void printKingsInOrder() {
        cout << "\n1. 조선의 왕을 순서대로 출력:" << endl;
        for (const string& king : kingOrder) {
            cout << king << " ";
        }
        cout << endl;
    }
    
    // 2. 조선의 왕을 역순으로 출력
    void printKingsInReverse() {
        cout << "\n2. 조선의 왕을 역순으로 출력:" << endl;
        for (int i = kingOrder.size() - 1; i >= 0; i--) {
            cout << kingOrder[i] << " ";
        }
        cout << endl;
    }
    
    // 3. 조선의 왕은 모두 몇 명인가?
    void countKings() {
        cout << "\n3. 조선의 왕은 모두 몇 명인가?" << endl;
        cout << "총 " << kingOrder.size() << "명" << endl;
    }
    
    // 4. 조선의 왕 중에서 인조의 후손은 누구누구인가?
    void findDescendantsOfInjo() {
        cout << "\n4. 조선의 왕 중에서 인조의 후손은 누구누구인가?" << endl;
        if (nodeMap.find("인조") != nodeMap.end()) {
            vector<string> descendants;
            findDescendantsHelper(nodeMap["인조"], descendants);
            if (descendants.empty()) {
                cout << "인조의 후손 왕이 없습니다." << endl;
            } else {
                for (const string& desc : descendants) {
                    cout << desc << " ";
                }
                cout << endl;
            }
        } else {
            cout << "인조를 찾을 수 없습니다." << endl;
        }
    }
    
private:
    void findDescendantsHelper(TreeNode* node, vector<string>& descendants) {
        for (TreeNode* child : node->children) {
            descendants.push_back(child->name);
            findDescendantsHelper(child, descendants);
        }
    }
    
public:
    // 5. 직계 후손이 왕이 되지 못한 왕은 누구누구인가?
    void findKingsWithoutDirectDescendants() {
        cout << "\n5. 직계 후손이 왕이 되지 못한 왕은 누구누구인가?" << endl;
        for (const auto& pair : nodeMap) {
            if (pair.second->children.empty()) {
                cout << pair.first << " ";
            }
        }
        cout << endl;
    }
    
    // 6. 직계 후손이 왕이 된 수가 가장 많은 왕은 누구인가?
    void findKingWithMostDescendants() {
        cout << "\n6. 직계 후손이 왕이 된 수가 가장 많은 왕은 누구인가?" << endl;
        string maxKing;
        int maxCount = 0;
        
        for (const auto& pair : nodeMap) {
            vector<string> descendants;
            findDescendantsHelper(pair.second, descendants);
            if (descendants.size() > maxCount) {
                maxCount = descendants.size();
                maxKing = pair.first;
            }
        }
        
        cout << maxKing << " (" << maxCount << "명의 후손)" << endl;
    }
    
    // 7. 정종의 형제로 조선의 왕이 된 사람은 누구인가?
    void findJeongJongSiblings() {
        cout << "\n7. 정종의 형제로 조선의 왕이 된 사람은 누구인가?" << endl;
        if (nodeMap.find("정종") != nodeMap.end() && nodeMap["정종"]->parent) {
            TreeNode* parent = nodeMap["정종"]->parent;
            cout << "정종의 형제들: ";
            for (TreeNode* sibling : parent->children) {
                if (sibling->name != "정종") {
                    cout << sibling->name << " ";
                }
            }
            cout << endl;
        } else {
            cout << "정종을 찾을 수 없거나 부모가 없습니다." << endl;
        }
    }
    
    // 8. 순종의 직계 선조를 모두 출력하시오.
    void findSunjongAncestors() {
        cout << "\n8. 순종의 직계 선조를 모두 출력하시오:" << endl;
        if (nodeMap.find("순종") != nodeMap.end()) {
            vector<string> ancestors;
            TreeNode* current = nodeMap["순종"]->parent;
            while (current) {
                ancestors.push_back(current->name);
                current = current->parent;
            }
            
            // 역순으로 출력 (먼 조상부터)
            for (int i = ancestors.size() - 1; i >= 0; i--) {
                cout << ancestors[i] << " -> ";
            }
            cout << "순종" << endl;
        } else {
            cout << "순종을 찾을 수 없습니다." << endl;
        }
    }
    
    // 9. 직계 후손이 2명 이상 왕이 된 왕은 몇 명인가?
    void countKingsWithMultipleDescendants() {
        cout << "\n9. 직계 후손이 2명 이상 왕이 된 왕은 몇 명인가?" << endl;
        int count = 0;
        for (const auto& pair : nodeMap) {
            vector<string> descendants;
            findDescendantsHelper(pair.second, descendants);
            if (descendants.size() >= 2) {
                count++;
                cout << pair.first << "(" << descendants.size() << "명) ";
            }
        }
        cout << "\n총 " << count << "명" << endl;
    }
    
    // 10. 예종은 태종의 몇 대 후손인가?
    void findGenerationsBetween() {
        cout << "\n10. 예종은 태종의 몇 대 후손인가?" << endl;
        if (nodeMap.find("예종") != nodeMap.end() && nodeMap.find("태종") != nodeMap.end()) {
            int generations = 0;
            TreeNode* current = nodeMap["예종"];
            
            while (current && current->name != "태종") {
                current = current->parent;
                generations++;
            }
            
            if (current && current->name == "태종") {
                cout << "예종은 태종의 " << generations << "대 후손입니다." << endl;
            } else {
                cout << "예종과 태종 사이의 직계 관계를 찾을 수 없습니다." << endl;
            }
        } else {
            cout << "예종 또는 태종을 찾을 수 없습니다." << endl;
        }
    }
};

int main() {
    JoseonDynastyTree tree;
    
    // 파일에서 트리 구성
    tree.buildTreeFromFile("조선왕조.txt");
    
    // 트리 시각화
    tree.printTree();
    
    // 각 질문에 대한 답 출력
    tree.printKingsInOrder();
    tree.printKingsInReverse();
    tree.countKings();
    tree.findDescendantsOfInjo();
    tree.findKingsWithoutDirectDescendants();
    tree.findKingWithMostDescendants();
    tree.findJeongJongSiblings();
    tree.findSunjongAncestors();
    tree.countKingsWithMultipleDescendants();
    tree.findGenerationsBetween();
    
    return 0;
}