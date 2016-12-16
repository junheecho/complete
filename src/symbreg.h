#pragma once

#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>

using namespace std;

static double EPS = 1e-10;

static double negation(double x) { return -x; }
static double add(double x, double y) { return x + y; }
static double subtract(double x, double y) { return x - y; }
static double multiply(double x, double y) { return x * y; }
static double divide(double x, double y) { return x / (y ? y : EPS); }

enum NODE_TYPE {
  TERMINAL,
  UNARY_OP,
  BINARY_OP
};
static vector<string> VARIABLES;
typedef double (*UNARY)(double);
static vector<string> UNARY_OPS = {"~", "abs", "sin", "cos", "tan", "asin", "acos", "atan", "sinh", "cosh", "tanh", "exp", "sqrt", "log"};
static map<string, UNARY> UNARY_FUN = {{"~", negation}, {"abs", abs}, {"sin", sin}, {"cos", cos}, {"tan", tan}, {"asin", asin}, {"acos", acos}, {"atan", atan}, {"sinh", sinh}, {"cosh", cosh}, {"tanh", tanh}, {"exp", exp}, {"sqrt", sqrt}, {"log", log}};
typedef double (*BINARY)(double, double);
static vector<string> BINARY_OPS = {"+", "-", "*", "/", "^"};
static map<string, BINARY> BINARY_FUN = {{"+", add}, {"-", subtract}, {"*", multiply}, {"/", divide}, {"^", pow}};

class Node {
private:
  static vector<string> split(string str, char delim) {
    vector<string> tokens;
    string token;
    stringstream ss;
    ss.str(str);
    while (getline(ss, token, delim))
      if (token.size())
        tokens.push_back(token);
    return tokens;
  }

public:
  NODE_TYPE node_type;
  string value;
  Node *left;
  Node *right;

  Node() : node_type(TERMINAL), value("0"), left(NULL), right(NULL) {}
  Node(NODE_TYPE _node_type, string _value, Node *_left = NULL, Node *_right = NULL) :
    node_type(_node_type),
    value(_value),
    left(_left),
    right(_right) {}
  Node(const Node &node) {
    *this = node;
  }
  ~Node() {
    if (this->left) delete this->left;
    if (this->right) delete this->right;
  }

  string rpn() {
    string s = "";
    if (this->left) s += this->left->rpn();
    if (this->right) s += this->right->rpn();
    s += this->value;
    return s;
  }

  double eval(map<string, double> &mapping) const {
    double left = (this->left ? this->left->eval(mapping) : 0);
    double right = (this->right ? this->right->eval(mapping) : 0);
    if (this->node_type == TERMINAL) {
      if (this->value[0] == 'x') {
        return mapping[this->value];
      } else {
        return stod(this->value);
      }
    } else if (this->node_type == UNARY_OP) {
      return UNARY_FUN[this->value](left);
    } else if (this->node_type == BINARY_OP) {
      return BINARY_FUN[this->value](left, right);
    }
    return 0;
  }

  // Parse Reverse Polish Notation (RPN)
  static Node *parse(const vector<string> &rpn, int &p) {
    int q = p--;
    if (std::find(BINARY_OPS.begin(), BINARY_OPS.end(), rpn[q]) != BINARY_OPS.end()) {
      Node *right = parse(rpn, p);
      Node *left = parse(rpn, p);
      return new Node(BINARY_OP, rpn[q], left, right);
    } else if (std::find(UNARY_OPS.begin(), UNARY_OPS.end(), rpn[q]) != UNARY_OPS.end()) {
      Node *left = parse(rpn, p);
      return new Node(UNARY_OP, rpn[q], left);
    } else if (std::find(VARIABLES.begin(), VARIABLES.end(), rpn[q]) != VARIABLES.end()) {
      return new Node(TERMINAL, rpn[q]);
    } else {
      return new Node(TERMINAL, rpn[q]);
    }
  }

  static Node *parse(string rpn) {
    vector<string> tokens = split(rpn, ' ');
    int n = tokens.size() - 1;
    return parse(tokens, n);
  }
};
