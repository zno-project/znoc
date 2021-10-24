#ifndef _ZNO_TYPE_H
#define _ZNO_TYPE_H

#include <map>
#include <string>
#include <vector>
#include <llvm/IR/DerivedTypes.h>
#include <variant>
#include <optional>

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
	void init_builtin_types();
	class TypeBase;
	struct TypeInstance;
	struct GenericInstance;
	typedef std::variant<AST::GenericInstance, AST::TypeInstance> field_type_t;
	class TypeBase;
}

namespace AST {
	class Function;

	struct GenericInstance {
		size_t generic_type_index;
	};

	struct FieldInfo {
		AST::TypeInstance& type;
		size_t index;
	};

	class TypeBase {
		protected:
		std::string name;
		std::map<size_t, llvm::Type*> generated;
		std::map<std::string, size_t> fields_by_name;
		std::vector<AST::field_type_t> fields_by_index;
		std::vector<std::vector<AST::TypeInstance>> generic_types;
		std::map<std::string, std::shared_ptr<AST::Function>> functions;

		public:
		TypeBase(std::string name): name(std::move(name)), fields_by_name(), fields_by_index(), functions(), generic_types(), generated() {}
		//TypeBase(std::string name, std::map<std::string, std::map<std::string, size_t> fields_by_name): name(std::move(name)), fields(std::move(fields)), functions() {}
		TypeBase(std::string name, std::map<std::string, size_t> fields_by_name, std::vector<AST::field_type_t> fields_by_index, std::map<std::string, std::shared_ptr<AST::Function>> functions): name(std::move(name)), fields_by_name(std::move(fields_by_name)), generic_types(), fields_by_index(std::move(fields_by_index)), functions(std::move(functions)), generated() {}

		virtual llvm::Type* codegen(size_t template_instance);

		virtual std::string get_name() { return name; }
		virtual ~TypeBase() = default;

		std::shared_ptr<AST::Function> get_function_by_name(std::string name, size_t template_instance_id);
		FieldInfo get_field_info_by_name(std::string name, size_t template_instance_id);
		FieldInfo get_field_info_by_index(size_t idx, size_t template_instance_id);
		size_t add_generic_instance(std::vector<AST::TypeInstance> types);
	};

	struct TypeInstance {
		std::shared_ptr<AST::TypeBase> base_type;
		std::optional<size_t> template_instance_id;

		std::shared_ptr<AST::Function> get_function_by_name(std::string name) {
			return base_type->get_function_by_name(name, template_instance_id.value());
		}

		FieldInfo get_field_info_by_name(std::string name) {
			return base_type->get_field_info_by_name(name, template_instance_id.value());
		}

		FieldInfo get_field_info_by_index(size_t idx) {
			return base_type->get_field_info_by_index(idx, template_instance_id.value());
		}

		AST::TypeInstance get_pointer_to();
		AST::TypeInstance get_pointed_to();

		llvm::Type* codegen() {
			return base_type->codegen(template_instance_id.value());
		}

		bool operator==(const TypeInstance&) const = default;
	};
}

/*namespace AST {
	typedef struct TypeUsage type_usage_t;
	typedef struct TemplatedFieldType templated_field_type_t;

	typedef std::variant<AST::templated_field_type_t, AST::type_usage_t> field_type_t;
}*/

//#include "../mangling.hpp"
//#include "../constructions/function.hpp"

/*namespace AST {
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
		std::pair<std::unique_ptr<AST::GEP>, AST::type_usage_t> get_field(std::string field_name, int template_instance);
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
}*/

namespace Parser {
	AST::TypeInstance parse_type(FILE* f);
	AST::TypeInstance parse_aggregate_type_definition(FILE* f);
}

//extern std::map<std::string, std::shared_ptr<AST::Type>> named_types;

#endif