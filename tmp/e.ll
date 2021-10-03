declare void @__zno_personality_fn()
declare void @__zno_raise(i64)
declare void @_Unwind_Resume(%unwindStruct*)
declare i32 @printf(i8*, ...)

%catchTypeInfo = type {i64}
@catchType1 = global %catchTypeInfo {i64 98}
@catchType2 = global %catchTypeInfo {i64 99}
@catchType3 = global %catchTypeInfo {i64 452}

%unwindStruct = type opaque ;{i64, void()*, i64, i64, %catchTypeInfo, i8*, i8*}
;@unwindThing = global %unwindStruct {i64 67, void()* null, i64 0, i64 0, %catchTypeInfo {i64 98}, i8* null, i8* null}

@s1 = global [45 x i8] c"Caught exception in main with error code %i\0A\00"
@s2 = global [28 x i8] c"Throwing exception in main\0A\00"
@s3 = global [15 x i8] c"Did not throw\0A\00"
@s4 = global [20 x i8] c"test() returned %i\0A\00"
@s5 = global [17 x i8] c"wrapper cleanup\0A\00"

define i32 @main() personality void ()* @__zno_personality_fn {
	%s2 = bitcast [28 x i8]* @s2 to i8*
	call i32 (i8*, ...) @printf(i8* %s2)
	%retval = invoke i32 @test_wrapper_with_cleanup() to label %Continue unwind label %Catch
Catch:
	%2 = landingpad i32 catch %catchTypeInfo* @catchType2 catch %catchTypeInfo* @catchType1
	%s1 = bitcast [45 x i8]* @s1 to i8*
	call i32 (i8*, ...) @printf(i8* %s1, i32 %2)
	ret i32 1
Continue:
	%s3 = bitcast [15 x i8]* @s3 to i8*
	call i32 (i8*, ...) @printf(i8* %s3)
	%s4 = bitcast [20 x i8]* @s4 to i8*
	call i32 (i8*, ...) @printf(i8* %s4, i32 %retval)
	ret i32 0
}

define i32 @test_wrapper_with_cleanup() personality void ()* @__zno_personality_fn {
	%1 = invoke i32 @test() to label %Continue unwind label %Catch
Catch:
	%2 = landingpad %unwindStruct* cleanup
	%s5 = bitcast [17 x i8]* @s5 to i8*
	call i32 (i8*, ...) @printf(i8* %s5)
	call void @_Unwind_Resume(%unwindStruct* %2)
Continue:
	ret i32 0
}

define i32 @test() {
	call void @__zno_raise(i64 98)
	ret i32 6
}