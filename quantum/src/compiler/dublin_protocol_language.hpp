#ifndef DUBLIN_PROTOCOL_LANGUAGE_HPP
#define DUBLIN_PROTOCOL_LANGUAGE_HPP

#include "../core/dublin_protocol_integration.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include <sstream>
#include <regex>
#include <stack>

namespace dublin {

// === DUBLIN PROTOCOL LANGUAGE (DPL) ===
// A domain-specific language for programming consciousness and cognitive patterns
// Compiles high-level cognitive specifications into executable quantum cognitive programs

// === LEXICAL ANALYSIS ===
enum class TokenType {
    // Keywords
    CONSCIOUSNESS, COGNITION, EMERGENCE, QUANTUM, NEURAL, HYBRID,
    AGENT, COLLECTIVE, STATE, TRANSITION, PATTERN, MEMORY,
    ATTENTION, REASONING, LEARNING, EVOLUTION,

    // Operators
    ARROW, COLON, SEMICOLON, COMMA, DOT, EQUALS,
    PLUS, MINUS, MULTIPLY, DIVIDE, LPAREN, RPAREN,
    LBRACE, RBRACE, LBRACKET, RBRACKET,

    // Literals
    IDENTIFIER, NUMBER, STRING, BOOLEAN,

    // Special
    EOF_TOKEN, UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
    size_t line;
    size_t column;

    Token(TokenType t, const std::string& v, size_t l = 0, size_t c = 0)
        : type(t), value(v), line(l), column(c) {}
};

class Lexer {
private:
    std::string source;
    size_t position;
    size_t line;
    size_t column;
    std::map<std::string, TokenType> keywords;

public:
    Lexer(const std::string& src) : source(src), position(0), line(1), column(1) {
        initialize_keywords();
    }

    std::vector<Token> tokenize() {
        std::vector<Token> tokens;

        while (position < source.length()) {
            char current = source[position];

            if (isspace(current)) {
                advance();
                continue;
            }

            if (isdigit(current)) {
                tokens.push_back(tokenize_number());
                continue;
            }

            if (isalpha(current) || current == '_') {
                tokens.push_back(tokenize_identifier());
                continue;
            }

            if (current == '"') {
                tokens.push_back(tokenize_string());
                continue;
            }

            // Single character tokens
            Token token = tokenize_single_char(current);
            if (token.type != TokenType::UNKNOWN) {
                tokens.push_back(token);
                continue;
            }

            // Unknown character
            tokens.push_back(Token(TokenType::UNKNOWN, std::string(1, current), line, column));
            advance();
        }

        tokens.push_back(Token(TokenType::EOF_TOKEN, "", line, column));
        return tokens;
    }

private:
    void initialize_keywords() {
        keywords["consciousness"] = TokenType::CONSCIOUSNESS;
        keywords["cognition"] = TokenType::COGNITION;
        keywords["emergence"] = TokenType::EMERGENCE;
        keywords["quantum"] = TokenType::QUANTUM;
        keywords["neural"] = TokenType::NEURAL;
        keywords["hybrid"] = TokenType::HYBRID;
        keywords["agent"] = TokenType::AGENT;
        keywords["collective"] = TokenType::COLLECTIVE;
        keywords["state"] = TokenType::STATE;
        keywords["transition"] = TokenType::TRANSITION;
        keywords["pattern"] = TokenType::PATTERN;
        keywords["memory"] = TokenType::MEMORY;
        keywords["attention"] = TokenType::ATTENTION;
        keywords["reasoning"] = TokenType::REASONING;
        keywords["learning"] = TokenType::LEARNING;
        keywords["evolution"] = TokenType::EVOLUTION;
        keywords["true"] = TokenType::BOOLEAN;
        keywords["false"] = TokenType::BOOLEAN;
    }

    void advance() {
        position++;
        column++;
        if (source[position] == '\n') {
            line++;
            column = 1;
        }
    }

    Token tokenize_number() {
        size_t start = position;
        while (position < source.length() && (isdigit(source[position]) || source[position] == '.')) {
            advance();
        }
        return Token(TokenType::NUMBER, source.substr(start, position - start), line, column);
    }

    Token tokenize_identifier() {
        size_t start = position;
        while (position < source.length() && (isalnum(source[position]) || source[position] == '_')) {
            advance();
        }
        std::string value = source.substr(start, position - start);

        // Check if it's a keyword
        if (keywords.find(value) != keywords.end()) {
            return Token(keywords[value], value, line, column);
        }

        return Token(TokenType::IDENTIFIER, value, line, column);
    }

    Token tokenize_string() {
        advance(); // Skip opening quote
        size_t start = position;
        while (position < source.length() && source[position] != '"') {
            advance();
        }
        std::string value = source.substr(start, position - start);
        if (position < source.length()) advance(); // Skip closing quote
        return Token(TokenType::STRING, value, line, column);
    }

    Token tokenize_single_char(char c) {
        switch (c) {
            case '-': {
                advance();
                if (position < source.length() && source[position] == '>') {
                    advance();
                    return Token(TokenType::ARROW, "->", line, column);
                }
                return Token(TokenType::MINUS, "-", line, column);
            }
            case ':': advance(); return Token(TokenType::COLON, ":", line, column);
            case ';': advance(); return Token(TokenType::SEMICOLON, ";", line, column);
            case ',': advance(); return Token(TokenType::COMMA, ",", line, column);
            case '.': advance(); return Token(TokenType::DOT, ".", line, column);
            case '=': advance(); return Token(TokenType::EQUALS, "=", line, column);
            case '+': advance(); return Token(TokenType::PLUS, "+", line, column);
            case '*': advance(); return Token(TokenType::MULTIPLY, "*", line, column);
            case '/': advance(); return Token(TokenType::DIVIDE, "/", line, column);
            case '(': advance(); return Token(TokenType::LPAREN, "(", line, column);
            case ')': advance(); return Token(TokenType::RPAREN, ")", line, column);
            case '{': advance(); return Token(TokenType::LBRACE, "{", line, column);
            case '}': advance(); return Token(TokenType::RBRACE, "}", line, column);
            case '[': advance(); return Token(TokenType::LBRACKET, "[", line, column);
            case ']': advance(); return Token(TokenType::RBRACKET, "]", line, column);
            default: return Token(TokenType::UNKNOWN, std::string(1, c), line, column);
        }
    }
};

// === ABSTRACT SYNTAX TREE (AST) ===
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual std::string to_string() const = 0;
};

class ConsciousnessDeclaration : public ASTNode {
public:
    std::string name;
    std::vector<std::shared_ptr<ASTNode>> states;
    std::vector<std::shared_ptr<ASTNode>> transitions;

    ConsciousnessDeclaration(const std::string& n) : name(n) {}

    std::string to_string() const override {
        std::stringstream ss;
        ss << "consciousness " << name << " {\n";
        for (const auto& state : states) {
            ss << "  " << state->to_string() << "\n";
        }
        for (const auto& transition : transitions) {
            ss << "  " << transition->to_string() << "\n";
        }
        ss << "}";
        return ss.str();
    }
};

class StateDeclaration : public ASTNode {
public:
    std::string name;
    std::map<std::string, double> features;

    StateDeclaration(const std::string& n) : name(n) {}

    std::string to_string() const override {
        std::stringstream ss;
        ss << "state " << name << " {";
        for (const auto& feature : features) {
            ss << " " << feature.first << ": " << feature.second;
        }
        ss << " }";
        return ss.str();
    }
};

class TransitionDeclaration : public ASTNode {
public:
    std::string from_state;
    std::string to_state;
    double probability;
    std::string condition;

    TransitionDeclaration(const std::string& from, const std::string& to,
                         double prob, const std::string& cond = "")
        : from_state(from), to_state(to), probability(prob), condition(cond) {}

    std::string to_string() const override {
        std::stringstream ss;
        ss << "transition " << from_state << " -> " << to_state
           << " : " << probability;
        if (!condition.empty()) {
            ss << " when " << condition;
        }
        return ss.str();
    }
};

class AgentDeclaration : public ASTNode {
public:
    std::string name;
    std::string type; // quantum, neural, hybrid
    std::map<std::string, double> parameters;

    AgentDeclaration(const std::string& n, const std::string& t) : name(n), type(t) {}

    std::string to_string() const override {
        std::stringstream ss;
        ss << "agent " << name << " : " << type << " {";
        for (const auto& param : parameters) {
            ss << " " << param.first << ": " << param.second;
        }
        ss << " }";
        return ss.str();
    }
};

// === PARSER ===
class Parser {
private:
    std::vector<Token> tokens;
    size_t current;

public:
    Parser(const std::vector<Token>& t) : tokens(t), current(0) {}

    std::shared_ptr<ASTNode> parse() {
        if (match(TokenType::CONSCIOUSNESS)) {
            return parse_consciousness();
        } else if (match(TokenType::AGENT)) {
            return parse_agent();
        }

        return nullptr;
    }

private:
    bool match(TokenType type) {
        if (check(type)) {
            advance();
            return true;
        }
        return false;
    }

    bool check(TokenType type) {
        if (is_at_end()) return false;
        return peek().type == type;
    }

    Token advance() {
        if (!is_at_end()) current++;
        return previous();
    }

    bool is_at_end() {
        return current >= tokens.size() || peek().type == TokenType::EOF_TOKEN;
    }

    Token peek() {
        return tokens[current];
    }

    Token previous() {
        return tokens[current - 1];
    }

    std::shared_ptr<ConsciousnessDeclaration> parse_consciousness() {
        Token name_token = consume(TokenType::IDENTIFIER, "Expected consciousness name");
        auto consciousness = std::make_shared<ConsciousnessDeclaration>(name_token.value);

        consume(TokenType::LBRACE, "Expected '{' after consciousness name");

        while (!check(TokenType::RBRACE) && !is_at_end()) {
            if (match(TokenType::STATE)) {
                consciousness->states.push_back(parse_state());
            } else if (match(TokenType::TRANSITION)) {
                consciousness->transitions.push_back(parse_transition());
            } else {
                advance(); // Skip unknown tokens
            }
        }

        consume(TokenType::RBRACE, "Expected '}' after consciousness body");
        return consciousness;
    }

    std::shared_ptr<StateDeclaration> parse_state() {
        Token name_token = consume(TokenType::IDENTIFIER, "Expected state name");
        auto state = std::make_shared<StateDeclaration>(name_token.value);

        consume(TokenType::LBRACE, "Expected '{' after state name");

        while (!check(TokenType::RBRACE) && !is_at_end()) {
            Token feature_name = consume(TokenType::IDENTIFIER, "Expected feature name");
            consume(TokenType::COLON, "Expected ':' after feature name");

            if (check(TokenType::NUMBER)) {
                double value = std::stod(advance().value);
                state->features[feature_name.value] = value;
            }

            if (!check(TokenType::RBRACE)) {
                consume(TokenType::COMMA, "Expected ',' or '}'");
            }
        }

        consume(TokenType::RBRACE, "Expected '}' after state body");
        return state;
    }

    std::shared_ptr<TransitionDeclaration> parse_transition() {
        Token from_token = consume(TokenType::IDENTIFIER, "Expected from state");
        consume(TokenType::ARROW, "Expected '->'");
        Token to_token = consume(TokenType::IDENTIFIER, "Expected to state");
        consume(TokenType::COLON, "Expected ':'");
        double probability = std::stod(consume(TokenType::NUMBER, "Expected probability").value);

        std::string condition;
        if (match(TokenType::IDENTIFIER) && previous().value == "when") {
            condition = consume(TokenType::STRING, "Expected condition").value;
        }

        return std::make_shared<TransitionDeclaration>(
            from_token.value, to_token.value, probability, condition);
    }

    std::shared_ptr<AgentDeclaration> parse_agent() {
        Token name_token = consume(TokenType::IDENTIFIER, "Expected agent name");
        consume(TokenType::COLON, "Expected ':'");
        Token type_token = consume(TokenType::IDENTIFIER, "Expected agent type");

        auto agent = std::make_shared<AgentDeclaration>(name_token.value, type_token.value);

        if (match(TokenType::LBRACE)) {
            while (!check(TokenType::RBRACE) && !is_at_end()) {
                Token param_name = consume(TokenType::IDENTIFIER, "Expected parameter name");
                consume(TokenType::COLON, "Expected ':' after parameter name");

                if (check(TokenType::NUMBER)) {
                    double value = std::stod(advance().value);
                    agent->parameters[param_name.value] = value;
                }

                if (!check(TokenType::RBRACE)) {
                    consume(TokenType::COMMA, "Expected ',' or '}'");
                }
            }
            consume(TokenType::RBRACE, "Expected '}' after agent body");
        }

        return agent;
    }

    Token consume(TokenType type, const std::string& message) {
        if (check(type)) return advance();

        throw std::runtime_error("Parse error at line " +
                               std::to_string(peek().line) + ": " + message);
    }
};

// === CODE GENERATOR ===
class CodeGenerator {
private:
    std::stringstream output;
    int indent_level;

public:
    CodeGenerator() : indent_level(0) {}

    std::string generate(const std::shared_ptr<ASTNode>& ast) {
        output.str("");
        output.clear();
        indent_level = 0;

        generate_node(ast);
        return output.str();
    }

private:
    void generate_node(const std::shared_ptr<ASTNode>& node) {
        if (auto consciousness = std::dynamic_pointer_cast<ConsciousnessDeclaration>(node)) {
            generate_consciousness(consciousness);
        } else if (auto agent = std::dynamic_pointer_cast<AgentDeclaration>(node)) {
            generate_agent(agent);
        }
    }

    void generate_consciousness(const std::shared_ptr<ConsciousnessDeclaration>& consciousness) {
        indent() << "// Generated consciousness: " << consciousness->name << "\n";
        indent() << "dublin::ConsciousnessEvolutionSimulator " << consciousness->name << "_simulator;\n\n";

        // Generate state definitions
        indent() << "// State definitions\n";
        for (const auto& state_node : consciousness->states) {
            if (auto state = std::dynamic_pointer_cast<StateDeclaration>(state_node)) {
                indent() << "// State: " << state->name << "\n";
                indent() << "std::vector<double> " << state->name << "_features = {";
                for (auto it = state->features.begin(); it != state->features.end(); ++it) {
                    if (it != state->features.begin()) output << ", ";
                    output << it->second;
                }
                output << "};\n";
            }
        }
        output << "\n";

        // Generate transition definitions
        indent() << "// Transition definitions\n";
        for (const auto& transition_node : consciousness->transitions) {
            if (auto transition = std::dynamic_pointer_cast<TransitionDeclaration>(transition_node)) {
                indent() << "// Transition: " << transition->from_state << " -> " << transition->to_state << "\n";
                indent() << consciousness->name << "_simulator.add_transition(\"" << transition->from_state
                        << "\", \"" << transition->to_state << "\", " << transition->probability << ");\n";
            }
        }
        output << "\n";

        // Generate simulation code
        indent() << "// Run simulation\n";
        indent() << "std::cout << \"🧠 Running consciousness simulation: " << consciousness->name << "\\n\";\n";
        indent() << consciousness->name << "_simulator.simulate_evolution(100, 0.6);\n";
        indent() << "std::cout << " << consciousness->name << "_simulator.analyze_evolution() << \"\\n\";\n";
    }

    void generate_agent(const std::shared_ptr<AgentDeclaration>& agent) {
        indent() << "// Generated agent: " << agent->name << "\n";

        if (agent->type == "quantum") {
            indent() << "dublin::QuantumCognitiveAgent " << agent->name << "(\"" << agent->name << "\");\n";
        } else if (agent->type == "neural") {
            indent() << "qi::NeuralCarryAgent " << agent->name << ";\n";
        } else if (agent->type == "hybrid") {
            indent() << "dublin::QuantumNeuralHybridAgent " << agent->name << "(\"" << agent->name << "\");\n";
        }

        // Generate parameter settings
        for (const auto& param : agent->parameters) {
            indent() << "// Parameter: " << param.first << " = " << param.second << "\n";
        }

        indent() << "std::cout << \"🤖 Agent " << agent->name << " initialized\\n\";\n\n";
    }

    std::stringstream& indent() {
        for (int i = 0; i < indent_level; ++i) {
            output << "  ";
        }
        return output;
    }
};

// === DUBLIN PROTOCOL LANGUAGE COMPILER ===
class DublinProtocolCompiler {
private:
    Lexer lexer;
    Parser parser;
    CodeGenerator generator;

public:
    DublinProtocolCompiler() = default;

    std::string compile(const std::string& source_code) {
        try {
            // Lexical analysis
            lexer = Lexer(source_code);
            auto tokens = lexer.tokenize();

            // Parsing
            parser = Parser(tokens);
            auto ast = parser.parse();

            if (!ast) {
                return "ERROR: Failed to parse source code";
            }

            // Code generation
            generator = CodeGenerator();
            std::string generated_code = generator.generate(ast);

            // Wrap in complete program
            std::stringstream ss;
            ss << "// DUBLIN PROTOCOL LANGUAGE COMPILER OUTPUT\n";
            ss << "// Generated from DPL source code\n\n";
            ss << "#include \"dublin_protocol_integration.hpp\"\n\n";
            ss << "int main() {\n";
            ss << "    std::cout << \"🎭 DUBLIN PROTOCOL LANGUAGE EXECUTION\\n\";\n";
            ss << "    std::cout << \"====================================\\n\\n\";\n\n";
            ss << generated_code;
            ss << "\n    std::cout << \"\\n🎯 DPL EXECUTION COMPLETE\\n\";\n";
            ss << "    return 0;\n";
            ss << "}\n";

            return ss.str();

        } catch (const std::exception& e) {
            return "COMPILATION ERROR: " + std::string(e.what());
        }
    }

    // Execute compiled code (conceptual - would need actual compilation)
    std::string execute_compiled_code(const std::string& compiled_code) {
        std::stringstream ss;
        ss << "🎯 DUBLIN PROTOCOL LANGUAGE EXECUTOR\n";
        ss << "===================================\n\n";
        ss << "Generated C++ Code:\n";
        ss << "-------------------\n";
        ss << compiled_code << "\n\n";
        ss << "📝 To execute:\n";
        ss << "1. Save the above code to a .cpp file\n";
        ss << "2. Compile with: g++ -std=c++17 -I path/to/dublin file.cpp -o dpl_program\n";
        ss << "3. Run with: ./dpl_program\n\n";
        ss << "This will create and run the consciousness/cognitive system defined in DPL!\n";

        return ss.str();
    }
};

// === DPL DEMONSTRATION ===
class DPLDemonstration {
public:
    static void demonstrate_dpl_compiler() {
        std::cout << "🎭 DUBLIN PROTOCOL LANGUAGE (DPL) COMPILER DEMO\n";
        std::cout << "=============================================\n\n";

        DublinProtocolCompiler compiler;

        // Example DPL program
        std::string dpl_program = R"(
consciousness awake_mind {
    state awake {
        attention: 0.9,
        memory: 0.8,
        emotion: 0.6,
        intention: 0.7,
        perception: 0.85,
        reasoning: 0.75
    }

    state focused {
        attention: 0.95,
        memory: 0.9,
        emotion: 0.4,
        intention: 0.8,
        perception: 0.9,
        reasoning: 0.85
    }

    state distracted {
        attention: 0.3,
        memory: 0.4,
        emotion: 0.8,
        intention: 0.2,
        perception: 0.3,
        reasoning: 0.4
    }

    transition awake -> focused : 0.6
    transition awake -> distracted : 0.4
    transition focused -> awake : 0.7
    transition distracted -> awake : 0.5
}

agent quantum_mind : quantum {
    dimensions: 32,
    learning_rate: 0.1,
    coherence_threshold: 0.8
}
)";

        std::cout << "📝 DPL Source Code:\n";
        std::cout << "------------------\n";
        std::cout << dpl_program << "\n\n";

        std::cout << "🔨 Compiling DPL to C++...\n\n";

        std::string compiled_code = compiler.compile(dpl_program);

        if (compiled_code.find("ERROR") == 0) {
            std::cout << "❌ Compilation failed:\n" << compiled_code << "\n";
        } else {
            std::cout << "✅ Compilation successful!\n\n";
            std::cout << compiler.execute_compiled_code(compiled_code);
        }

        std::cout << "\n=== DPL COMPILER DEMONSTRATION COMPLETE ===\n";
        std::cout << "DPL enables programming consciousness with simple, readable syntax!\n";
    }
};

} // namespace dublin

#endif // DUBLIN_PROTOCOL_LANGUAGE_HPP
