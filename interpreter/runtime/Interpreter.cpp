#include "Interpreter.h"
#include "../ast/AST.h"

void Interpreter::execute(Statement& stmt) {
    stmt.execute(*this);
}
