; ModuleID = 'fptr_inline.ll'
source_filename = "fptr_inline.ll"

%struct.thing = type { i32, void (i32)* }

@str.0 = global [3 x i8] c"a\0A\00"
@str.1 = global [4 x i8] c"b%\0A\00"
@str.2 = global [3 x i8] c"c\0A\00"
@str.3 = global [3 x i8] c"d\0A\00"
@globalStruct = global %struct.thing { i32 7, void (i32)* null }

declare i32 @printf(i8*, i32)

declare void @thing(void (i32)*)

define void @main() {
  %str.0 = bitcast [3 x i8]* @str.0 to i8*
  %str.3 = bitcast [3 x i8]* @str.3 to i8*
  %1 = call i32 @printf(i8* %str.0, i32 0)
  %2 = call i32 @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @str.1, i32 0, i32 0), i32 5)
  %3 = call i32 @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @str.2, i32 0, i32 0), i32 0)
  %4 = call i32 @printf(i8* %str.3, i32 0)
  ret void
}
