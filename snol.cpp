#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <cstdlib>

using namespace std;

string int_to_string(int num) {
    char buf[1024];
    sprintf(buf, "%d", num);
    string s(buf);
    return s;
}

string float_to_string(double num) {
    char buf[1024];
    sprintf(buf, "%f", num);
    string s(buf);
    return s;
}

bool is_digit(const char c) {
    return (c >= '0' && c <= '9');
}

bool is_letter(const char c) {
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'z'));
}

struct SNOL_Var {
    // 0 if int, 1 if float (double)
    // if type == 0, then operations use 'data';
    // else use 'fdata' for operations.
    int type;
    int data;
    double fdata;

    SNOL_Var() {}

    SNOL_Var(int num) {
        type = 0;
        data = num;
    }

    SNOL_Var(double num) {
        type = 1;
        fdata = num;
    }

    SNOL_Var(const SNOL_Var& var) {
        type = var.type;
        data = var.data;
        fdata = var.fdata;
    }
};

class SNOL {
private:
    // storing SNOL variables
    map<string, SNOL_Var> variables;
    bool is_running;
    string err_reg;

    void snol_output(const string& s) {
        cout << "SNOL> " << s << endl;
    }

    string snol_input(const string& s = "") {
        string input_str;

        cout << s;
        getline(cin, input_str);

        return input_str;
    }

    bool is_int(const string& num) {
        int size = num.length();

        if(size == 0) return false;

        for(int i = 0; i < size; ++i) {
            //if not a digit and if first character not '-' (negative sign)
            if(!is_digit(num[i]) && (i != 0 || num[i] != '-')) {
                return false;
            }
        }

        return true;
    }

    bool is_float(const string& num) {
        int size = num.length();

        if(size == 0) return false;

        // -1 meaning no decimal point found, otherwise it represents
        // index of where the decimal point is found
        int decimal_idx = -1;

        for(int i = 0; i < size; ++i) {
            // if not a digit
            if(!is_digit(num[i])) {
                // if no decimal point has been PREVIOUSLY found
                if(num[i] == '.' && decimal_idx == -1) {
                    decimal_idx = i;
                } else if(i != 0 || num[i] != '-') {
                    // if no negative sign on first character return false
                    return false;   
                }
            }
        }

        // not valid if no decimal point
        if(decimal_idx == -1) return false;

        return true;
    }

    bool is_keyword(const string& name) {
        if(name == "BEG" || name == "PRINT" || name == "EXIT!") {
            return true;
        }

        return false;
    }

    bool is_valid_name(const string& name) {
        // if the variable is a keyword
        if(is_keyword(name))
            return false;

        int size = name.length();

        if(size == 0) return false;

        for(int i = 0; i < size; ++i) {
            // if non-letter
            if(!is_letter(name[i])) {
                // if non digit or a digit as a first character
                if(i == 0 || !is_digit(name[i])) {
                    return false;
                }
            } 
        }

        return true;
    }

    bool is_arithmetic_operator(char op) {
        if(op == '+' || op == '-' || op == '*' || op == '/' || op == '%') {
            return true;
        }

        return false;
    }

    bool is_arithmetic_operator(string s) {
        if(s.size() != 1) {
            return false;
        }

        return is_arithmetic_operator(s[0]);
    }

    bool is_arithmetic_operation(const string& name1, const string& name2, char op) {
        if(
            (is_valid_name(name1) || is_int(name1) || is_float(name1)) && 
            (is_arithmetic_operator(op)) &&
            (is_valid_name(name2) || is_int(name2) || is_float(name2))
        ) {
            return true;
        }

        return false;
    }

public:
    SNOL() {
        is_running = false;
    }

    // execute operation given two SNOL variables
    SNOL_Var execute_op(const SNOL_Var& var1, const SNOL_Var& var2, char op) {
        if(var1.type != var2.type) {
            throw "ArithmeticTypeError";
        }

        SNOL_Var result;

        switch (op)
        {
            case '+':
                if(var1.type == 0) {
                    result = SNOL_Var(var1.data + var2.data);
                } else if(var1.type == 1){
                    result = SNOL_Var(var1.fdata + var2.fdata);
                } else {
                    throw "ArithmeticOpError";
                }
                break;
            case '-':
                if(var1.type == 0) {
                    result = SNOL_Var(var1.data - var2.data);
                } else if(var1.type == 1){
                    result = SNOL_Var(var1.fdata - var2.fdata);
                } else {
                    throw "ArithmeticOpError";
                }
                break;
            case '*':
                if(var1.type == 0) {
                    result = SNOL_Var(var1.data * var2.data);
                } else if(var1.type == 1){
                    result = SNOL_Var(var1.fdata * var2.fdata);
                } else {
                    throw "ArithmeticOpError";
                }
                break;
            case '/':
                if(var1.type == 0) {
                    result = SNOL_Var(var1.data / var2.data);
                } else if(var1.type == 1){
                    result = SNOL_Var(var1.fdata / var2.fdata);
                } else {
                    throw "ArithmeticOpError";
                }
                break;
            case '%':
                if(var1.type == 0) {
                    result = SNOL_Var(var1.data % var2.data);
                } else {
                    throw "ArithmeticModuloError";
                }
                break;
            default:
                throw "UnknownCommand";
        }
        
        return result;
    }

    SNOL_Var create_int(int num) {
        return SNOL_Var(num);
    }

    SNOL_Var create_float(double num) {
        return SNOL_Var(num);
    }

    void assign_int_var(const string& name, int num) {
        variables[name] = create_int(num);
    }

    void assign_float_var(const string& name, double num) {
        variables[name] = create_float(num);
    }

    void assign_var(const string& name, const SNOL_Var& var) {
        variables[name] = var;
    }

    SNOL_Var load_var(const string& name) {
        try {
            return variables.at(name);
        } catch (exception e) {
            err_reg = name;
            throw "UnknownVariable";
        }
    }

    void beg_input(const string& name) {
        string s = "Please enter value for [" + name + "]";
        snol_output(s);
        string input_str = snol_input("Input: ");

        if(is_int(input_str)) {
            assign_int_var(name, atoi(input_str.c_str()));
        } else if(is_float(input_str)) {
            assign_float_var(name, atof(input_str.c_str()));
        } else {
            throw "InvalidNumber";
        }
    }

    void print_var(const string& name) {
        SNOL_Var result = load_var(name);

        if(result.type == 0) {
            string s = '[' + name + "] = " + int_to_string(result.data);
            snol_output(s);
        } else if(result.type == 1) {
            string s = '[' + name + "] = " + float_to_string(result.fdata);
            snol_output(s);
        } else {
            err_reg = name;
            throw "UnknownVariable";
        }
    }

    void print_var(const SNOL_Var& var) {
        if(var.type == 0) {
            snol_output(int_to_string(var.data));
        } else if(var.type == 1) {
            snol_output(float_to_string(var.fdata));
        } else {
            throw "TypeError";
        }
    }

    vector<string> tokenize_input(const string& input_str) {
        // holds info about the string processed so far
        string curr_token;
        string last_token;

        // holds the tokens processed
        vector<string> tokens;

        int size = input_str.length();

        for(int i = 0; i < size; ++i) {
            if((is_arithmetic_operator(last_token) || last_token == "=") && input_str[i] == '-') {
                // do nothing
                // added for negative numbers
            } else if(curr_token.length() != 0 && curr_token[0] == '-' && is_digit(input_str[i])) {
                // do nothing
                // also added for negative numbers
            } else if(
                (is_keyword(curr_token) || input_str[i] == ' ' ||
                is_arithmetic_operator(input_str[i]) || input_str[i] == '=' ||
                is_arithmetic_operator(curr_token) || curr_token == "=") &&
                curr_token.length() != 0
            ) {
                tokens.push_back(curr_token);
                last_token = curr_token;
                curr_token.clear();
            }

            if(input_str[i] == ' ') {
                continue;
            }

            curr_token.push_back(input_str[i]);
        }

        if(curr_token.length() != 0) {
            tokens.push_back(curr_token);
        }

        return tokens;
    }

    void process_tokens(const vector<string>& tokens) {
        if(is_keyword(tokens[0])) {
            string cmd = tokens[0];

            if(cmd == "BEG") {
                if(tokens.size() != 2) {
                    throw "UnknownCommand";
                }

                if(is_valid_name(tokens[1])) {
                    beg_input(tokens[1]);
                } else {
                    throw "UnknownCommand";
                }
            } else if(cmd == "PRINT") {
                if(tokens.size() != 2) {
                    throw "UnknownCommand";
                }

                if(is_valid_name(tokens[1])) {
                    print_var(tokens[1]);
                } else if(is_int(tokens[1])) {
                    SNOL_Var var = create_int(atoi(tokens[1].c_str()));
                    print_var(var);
                } else if(is_float(tokens[1])) {
                    SNOL_Var var = create_float(atof(tokens[1].c_str()));
                    print_var(var);
                } else {
                    throw "UnknownCommand";
                }
            } else if(cmd == "EXIT!") {
                if(tokens.size() != 1) {
                    throw "UnknownCommand";
                }

                is_running = false;
            } else {
                throw "UnknownCommand";
            }
        } else if(is_int(tokens[0]) || is_float(tokens[0]) || is_valid_name(tokens[0])) {
            if(tokens.size() == 1) {
                if(is_valid_name(tokens[0])) {
                    load_var(tokens[0]);
                }
            } else if(tokens.size() == 3) {
                char op = tokens[1][0];

                if(is_arithmetic_operation(tokens[0], tokens[2], op)) {
                    SNOL_Var var1;
                    SNOL_Var var2;

                    if(is_valid_name(tokens[0])) {
                        var1 = load_var(tokens[0]);
                    } else if(is_int(tokens[0])) {
                        var1 = create_int(atoi(tokens[0].c_str()));
                    } else if(is_float(tokens[0])) {
                        var1 = create_float(atof(tokens[0].c_str()));
                    } else {
                        throw "UnknownCommand";
                    }

                    if(is_valid_name(tokens[2])) {
                        var2 = load_var(tokens[2]);
                    } else if(is_int(tokens[2])) {
                        var2 = create_int(atoi(tokens[2].c_str()));
                    } else if(is_float(tokens[2])) {
                        var2 = create_float(atof(tokens[2].c_str()));
                    } else {
                        throw "UnknownCommand";
                    }

                    execute_op(var1, var2, op);
                } else if(op == '=') {
                    if(is_valid_name(tokens[0])) {
                        if(is_valid_name(tokens[2])) {
                            SNOL_Var var = load_var(tokens[2]);
                            SNOL_Var new_var(var);
                            assign_var(tokens[0], new_var);
                        } else if(is_int(tokens[2])) {
                            assign_int_var(tokens[0], atoi(tokens[2].c_str()));
                        } else if(is_float(tokens[2])) {
                            assign_float_var(tokens[0], atof(tokens[2].c_str()));
                        } else {
                            throw "UnknownCommand";
                        }
                    } else {
                        throw "UnknownCommand";
                    }
                }
            } else if(tokens.size() == 5) {
                // if first token is a valid var name and second token is a '='
                if(is_valid_name(tokens[0]) && tokens[1].length() == 1 && tokens[1][0] == '=') {
                    char op = tokens[3][0];

                    if(tokens[3].size() == 1 && is_arithmetic_operation(tokens[2], tokens[4], op)) {
                        SNOL_Var var1;
                        SNOL_Var var2;

                        if(is_valid_name(tokens[2])) {
                            var1 = load_var(tokens[2]);
                        } else if(is_int(tokens[2])) {
                            var1 = create_int(atoi(tokens[2].c_str()));
                        } else if(is_float(tokens[2])) {
                            var1 = create_float(atof(tokens[2].c_str()));
                        } else {
                            throw "UnknownCommand";
                        }

                        if(is_valid_name(tokens[4])) {
                            var2 = load_var(tokens[4]);
                        } else if(is_int(tokens[4])) {
                            var2 = create_int(atoi(tokens[4].c_str()));
                        } else if(is_float(tokens[4])) {
                            var2 = create_float(atof(tokens[4].c_str()));
                        } else {
                            throw "UnknownCommand";
                        }

                        SNOL_Var result = execute_op(var1, var2, op);
                        assign_var(tokens[0], result);
                    } else {
                        throw "UnknownCommand";
                    }
                } else {
                    throw "UnknownCommand";
                }
            } else {
                throw "UnknownCommand";
            }
        } else if(is_arithmetic_operator(tokens[0]) || tokens[0] == "=") {
            throw "UnknownCommand";
        } else {
            err_reg = tokens[0];
            throw "UnknownWord";
        }
    }

    void print_intro() {
        string intro_msg = "BigPeePoo Here is an intro";
        cout << intro_msg << endl;
    }

    void print_outro() {
        string outro_msg = "Interpreter is now terminated...";
        cout << outro_msg << endl;
    }

    void run() {
        cout << endl;
        print_intro();
        cout << endl;

        is_running = true;
        
        while(is_running) {
            try {
                string input_str = snol_input("Command: ");
                vector<string> tokens = tokenize_input(input_str);
                process_tokens(tokens);
            } catch (const char* err) {
                string err_msg;

                if(err == "UnknownCommand") {
                    err_msg = "Unknown command! Does not match any valid command of the language.";
                } else if(err == "ArithmeticTypeError") {
                    err_msg = "Error! Operands must be of the same type in an arithmetic operation!";
                } else if(err == "InvalidNumber") {
                    err_msg = "Error! Please enter a valid number.";
                } else if(err == "UnknownVariable") {
                    err_msg = "Error! [" + err_reg + "] is not defined!";
                } else if(err == "ArithmeticOpError") {
                    err_msg = "Error! Only integers and floating-point numbers can be used in an arithmetic operation!";
                } else if(err == "ArithmeticModuloError") {
                    err_msg = "Error! Only integers can be used in a modulo operation!";
                } else if(err == "UnknownWord") {
                    err_msg = "Unknown word! [" + err_reg + "]";
                } else {
                    err_msg = "Unknown error!";
                }

                snol_output(err_msg);
            }

            cout << endl;
        }

        print_outro();
        cout << endl;
    }
};

int main() {
    SNOL snol;
    snol.run();
}