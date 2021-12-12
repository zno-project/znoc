#include "memory_location.hpp"
#include "variable.hpp"
#include <map>
#include <deque>
#include <string>
#include <memory>
#include <llvm/IR/IRBuilder.h>

std::deque<std::map<std::string, std::shared_ptr<AST::MemoryLoc>>> stack_allocations;

void push_new_scope() {
	stack_allocations.push_front(std::map<std::string, std::shared_ptr<AST::MemoryLoc>>());
}

std::unique_ptr<ScopeEnd> pop_scope() {
	auto popped_scope = stack_allocations.front();
	stack_allocations.pop_front();

	std::vector<std::shared_ptr<AST::Variable>> out_of_scope_vars;

	for (auto& popped_var: popped_scope) {
		if (auto popped_var_casted = std::dynamic_pointer_cast<AST::Variable>(popped_var.second)) {
			out_of_scope_vars.push_back(std::move(popped_var_casted));
		}
	}

	return std::make_unique<ScopeEnd>(std::move(out_of_scope_vars));
}

llvm::Value* ScopeEnd::codegen(__attribute__((unused)) llvm::IRBuilder<> *builder) {
	/*for (auto& var: out_of_scope_vars) {
		builder->CreateLifetimeEnd(var->codegen(builder));
	}*/
	return nullptr;
}
