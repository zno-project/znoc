@a_stack = global i8 0
%ctors = type { i32, void ()*, i8* }
@llvm.global_ctors = appending global [1 x %ctors] [%ctors { i32 65535, void ()* @ctor, i8* @a_stack }]

declare void @printI(i32)

define void @ctor() {
	store i8 5, i8* @a_stack
	ret void
}

define i32 @main() {
	%1 = load i8, i8* @a_stack
	%2 = sext i8 %1 to i32
	call void @printI(i32 %2)
	ret i32 0
}