#ifndef _ZNO_TYPE_H
#define _ZNO_TYPE_H

#include <map>
#include <string>
#include <vector>
#include <llvm/IR/DerivedTypes.h>
#include <variant>
#include <optional>
#include <fmt/format.h>
#include "../parsing.hpp"

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

	struct FieldInfoField {
		AST::TypeInstance& type;
		size_t index;
	};

	struct FieldInfoConst {
		AST::TypeInstance& type;
		std::string linkage_name;
	};

	typedef std::variant<FieldInfoField, FieldInfoConst> FieldInfo;

	struct Interface {};

	class Expression;

	struct TypeInstance {
		std::shared_ptr<AST::TypeBase> base_type;
		std::optional<size_t> template_instance_id;
		std::vector<size_t> array_lengths;

		private:
		size_t get_template_id();

		public:
		std::shared_ptr<AST::Function> get_function_by_name(std::string name);
		FieldInfo get_field_info_by_name(std::string name);
		FieldInfo get_field_info_by_index(size_t idx);

		AST::TypeInstance get_pointer_to();
		AST::TypeInstance get_pointed_to();

		AST::TypeInstance get_function_returning(std::vector<AST::TypeInstance> args, bool variadic);
		AST::TypeInstance get_return_of_function();
		std::vector<AST::TypeInstance> get_args_of_function();

		AST::TypeInstance get_array_elem_of();

		llvm::Type* codegen();
		bool is_templateable();
		bool is_array();
		bool operator==(const TypeInstance& cmp) const;
		void get_or_create_template(std::vector<AST::TypeInstance> template_types);
	};

	struct Constant {
		std::unique_ptr<AST::Expression> val;
		AST::TypeInstance type;
	};

	AST::TypeInstance get_fundamental_type(std::string name);
}

namespace Parser {
	AST::TypeInstance parse_type(zno_ifile& f);
	void parse_extension(zno_ifile& f);
	AST::TypeInstance parse_aggregate_type_definition(zno_ifile& f);
}

#endif
