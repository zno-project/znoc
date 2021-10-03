#ifndef _ZNO_TYPE_H
#define _ZNO_TYPE_H

#include <map>
#include <string>
#include <vector>
#include <llvm/IR/DerivedTypes.h>
#include <variant>
#include "../mangling.hpp"
#include "../constructions/function.hpp"
#include "../constructions/expression.hpp"

/**
 * THE TYPE AND TEMPLATING SYSTEM
 * Each type contains a bunch of named fields
 * `fieldTypes` holds an array of the types of each field
 * `fieldIndices` holds a map of which field name maps to each element in the array
 * `get_field_idxs(...)` returns an array of field indices, by recursively descending through nested structs by field name
 * For example, with the following structs
 ```
 struct Inner {
	 a: i32,
	 b: i32
 }
 struct Outer {
	 c: i32,
	 d: Inner
 }
 ```
 * calling `get_field_idxs(['c'], ...)` on `Outer` will return `[0]`, since `c` is the 0th element of `Outer`
 * calling `get_field_idxs(['d'], ...)` on `Outer` will return `[1]`, since `d` is the 1st element of `Outer`
 * calling `get_field_idxs(['d', 'a'], ...)` on `Outer` will return `[1, 0]`, since `d` is the 1st element of `Outer`, and `a` is the 0th element of `Inner`
 * 
 * Types can also be templated
 * In this case `templatedTypes` contains the different variants of the types
 * For example, with the struct `struct Templates<A, B> { ... }`, the first inner element of `templatedTypes` will contain the type that `A` is in that instance, and the second will contain the same for `B`
 * Each variant usage of a type will create a new inner array in `templatedTypes`
 * For example, `let x: Templates<i32, i64>` will add a new array of `[i32, i64]`, as `A` is `i32`, and `B` is `i64`
 * Creating a differenet variant, such as `let y: Templates<f32, f32>`, will append another array, in this containing `[f32, f32]`
 * 
 * When using a type, the `type_usage_t` struct is used
 * This holds a reference to the base type, as well which template instance is being referenced
 * So, using the above example, `x` is `{ Templates, 0 }`, whereas `y` is `{ Templates, 1 }`
 * 
 * This also means that `fieldTypes` does not hold only types
 * For a templated type field, it instead holds the index into the inner arrays of `templatedTypes` where that type comes from
 */

namespace AST {
	typedef struct TypeUsage type_usage_t;
	typedef struct TemplatedFieldType templated_field_type_t;

	typedef std::variant<AST::templated_field_type_t, AST::type_usage_t> field_type_t;

	class Type {
		//std::vector<int> get_field_idxs(std::vector<std::string> fieldNames, int template_instance);
		friend type_usage_t;

		protected:
		std::map<std::string, int> fieldIndices; // Map of names to field indices
		std::vector<AST::field_type_t> fieldTypes; // Array of types of fields

		public:
		std::string name;
		std::vector<std::vector<AST::type_usage_t>> templatedTypes;
		
		Type(std::string name): templatedTypes(), fieldIndices(), fieldTypes(), name(std::move(name)) {}
		Type(std::string name, std::vector<AST::field_type_t> fieldTypes, std::map<std::string, int> fieldIndices, std::vector<std::vector<AST::type_usage_t>> templatedTypes): templatedTypes(std::move(templatedTypes)), fieldIndices(std::move(fieldIndices)), fieldTypes(std::move(fieldTypes)), name(std::move(name)) {}
		virtual llvm::Type* codegen(int template_instance) = 0;
		virtual const std::string& get_name() { return name; }
		virtual ~Type() = default;

		std::shared_ptr<AST::Function> get_function_by_name(std::string name);
		AST::Namespace* get_namespace_by_name(std::string name);
		std::pair<std::unique_ptr<AST::Expression>, AST::type_usage_t> get_field(std::string field_name, int template_instance);
	};

	// An instance of the usage of a type
	struct TypeUsage {
		std::shared_ptr<AST::Type> type; // The type used
		int template_instance; // which templated instance is being referenced

		bool operator !() {
			return !type;
		}

		std::shared_ptr<AST::Type> operator ->() {
			return type;
		}

		llvm::Type* codegen() {
			return type->codegen(template_instance);
		}

		std::pair<std::unique_ptr<AST::Expression>, AST::type_usage_t> get_field(std::string field_name) {
			return type->get_field(field_name, template_instance);
		}
	};

	struct TemplatedFieldType {
		int typeIndex; // Which index in the list of templated args this field is
	};

	void init_builtin_types();
}

namespace Parser {
	AST::type_usage_t parse_type(FILE* f);
}

namespace AST {
	class GEP: public AST::Expression {
		int idx;
		
		public:
		GEP(int idx, AST::type_usage_t t): Expression(t), idx(idx) {}
		virtual llvm::Value* codegen(__attribute__((unused)) llvm::IRBuilder<> *builder, __attribute__((unused)) std::string _name = "");
	};
}

//extern std::map<std::string, std::shared_ptr<AST::Type>> named_types;

#endif