#include<bits/stdc++.h>
using namespace std;

    bool isValidSudoku(vector<vector<char>> b) {
        for(int i = 0; i < 9; i ++){
                map <char, int> m, m1;
                for (int j = 0; j < 9; j ++){
                        if(m[b[i][j]] == 0){
                                m[b[i][j]] ++;
                        }
                        else 
                                return 0;
                        
                        if(m1[b[j][i]] == 0){
                                m1[b[j][i]] ++;
                        }
                        else 
                                return 0;
                        
                        
                }
        }
            
        for(int i = 0; i < 9; i += 3){
                for (int j = 0; j < 9; j += 3){
                        map<char, int> m;
                        for(int x = i; x < i + 3; x ++){
                                for (int y = j; j < 3; j ++){
                                       if(m[b[x][y]] == 0){
                                               m[b[x][y]] ++;
                                       }
                                        else
                                                return 0;
                                }
                        }
                }
        }
        return true;
    }

int main(){

    //freopen("input.txt", "r", stdin);
    //freopen("output.txt", "w", stdout);

    string v[][] = [["5","3",".",".","7",".",".",".","."],["6",".",".","1","9","5",".",".","."],[".","9","8",".",".",".",".","6","."],["8",".",".",".","6",".",".",".","3"],["4",".",".","8",".","3",".",".","1"],["7",".",".",".","2",".",".",".","6"],[".","6",".",".",".",".","2","8","."],[".",".",".","4","1","9",".",".","5"],[".",".",".",".","8",".",".","7","9"]];
    /*for (int i = 0; i < 9; i ++){
        for(int j = 0; j < 9; j ++){
            char c;
            cin >> c;
            v[i].push_back(c);
        }
    }*/

    cout << isValidSudoku(v);
}    