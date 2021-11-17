#include <iostream>
#include <string>

#include <scheme.h>

void Ltrim(std::string* s) {
    s->erase(s->begin(), std::find_if(s->begin(), s->end(),
                                      [](unsigned char ch) { return !std::isspace(ch); }));
}

void Rtrim(std::string* s) {
    s->erase(
        std::find_if(s->rbegin(), s->rend(), [](unsigned char ch) { return !std::isspace(ch); })
            .base(),
        s->end());
}

void Trim(std::string* s) {
    Ltrim(s);
    Rtrim(s);
}

// (define len (lambda x (if (null? x) 0 (+ 1 (len (cdr x))))))
// (define fib (lambda n (if (< n 3) 1 (+ (fib (- n 1)) (fib (- n 2))))))
int main() {
    auto engine = SchemeEngine();
    engine.Init();
    for (std::string line;;) {
        std::cout << "> ";

        std::getline(std::cin, line);
        if (line.empty()) {
            std::cout << std::endl;
            continue;
        }

        Trim(&line);
        if (line == "quit" || line == "exit") {
            break;
        }
        try {
            std::cout << engine.Interpret(line) << std::endl;
        } catch (const SyntaxError& ex) {
            std::cout << "Syntax error: " << ex.what() << std::endl;
        } catch (const NameError& ex) {
            std::cout << "Name error: " << ex.what() << std::endl;
        } catch (const RuntimeError& ex) {
            std::cout << "Runtime error: " << ex.what() << std::endl;
        }
    }
    return 0;
}
