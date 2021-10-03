declare i32 @printf(i8*, i32)
declare void @thing(void(i32)*)

@str.0 = global [3 x i8] c"a\0a\00"
@str.1 = global [4 x i8] c"b%\0a\00"
@str.2 = global [3 x i8] c"c\0a\00"
@str.3 = global [3 x i8] c"d\0a\00"

%struct.thing = type { i32, void(i32)* }
@globalStruct = global %struct.thing { i32 7, void(i32)* null }

define void @main() {
	%str.0 = bitcast [3 x i8]* @str.0 to i8*
	%str.3 = bitcast [3 x i8]* @str.3 to i8*
	call i32 @printf(i8* %str.0, i32 0)
	call void @inlinefn(i32 5)
	call i32 @printf(i8* %str.3, i32 0)

	;%structFnptrPtr = getelementptr %struct.thing, %struct.thing* @globalStruct, i32 0, i32 1
	;store void(i32)* @inlinefn, void(i32)** %structFnptrPtr

	ret void
}

define internal void @inlinefn(i32 %x) alwaysinline {
	%str.1 = bitcast [4 x i8]* @str.1 to i8*
	%str.2 = bitcast [3 x i8]* @str.2 to i8*
	call i32 @printf(i8* %str.1, i32 %x)
	call i32 @printf(i8* %str.2, i32 0)
	ret void
}