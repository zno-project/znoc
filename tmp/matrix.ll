; ModuleID = 'm.c'
source_filename = "m.c"
target datalayout = "e-m:o-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx12.0.0"

@.str = private unnamed_addr constant [3 x i8] c"[ \00", align 1
@.str.1 = private unnamed_addr constant [4 x i8] c" %f\00", align 1
@.str.2 = private unnamed_addr constant [3 x i8] c"]\0A\00", align 1

; Function Attrs: noinline nounwind optnone ssp uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca [16 x float], align 4
  %3 = alloca [16 x float], align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  %6 = bitcast [16 x float]* %2 to <16 x float>*
  %7 = load <16 x float>, <16 x float>* %6, align 4
  %8 = insertelement <16 x float> %7, float 5.000000e+00, i64 0
  store <16 x float> %8, <16 x float>* %6, align 4
  %9 = bitcast [16 x float]* %2 to <16 x float>*
  %10 = load <16 x float>, <16 x float>* %9, align 4
  %11 = insertelement <16 x float> %10, float 6.000000e+00, i64 4
  store <16 x float> %11, <16 x float>* %9, align 4
  %12 = bitcast [16 x float]* %2 to <16 x float>*
  %13 = load <16 x float>, <16 x float>* %12, align 4
  %14 = insertelement <16 x float> %13, float 7.000000e+00, i64 8
  store <16 x float> %14, <16 x float>* %12, align 4
  %15 = bitcast [16 x float]* %2 to <16 x float>*
  %16 = load <16 x float>, <16 x float>* %15, align 4
  %17 = insertelement <16 x float> %16, float 8.000000e+00, i64 12
  store <16 x float> %17, <16 x float>* %15, align 4
  %18 = bitcast [16 x float]* %2 to <16 x float>*
  %19 = load <16 x float>, <16 x float>* %18, align 4
  %20 = insertelement <16 x float> %19, float 9.000000e+00, i64 1
  store <16 x float> %20, <16 x float>* %18, align 4
  %21 = bitcast [16 x float]* %2 to <16 x float>*
  %22 = load <16 x float>, <16 x float>* %21, align 4
  %23 = insertelement <16 x float> %22, float 1.000000e+00, i64 5
  store <16 x float> %23, <16 x float>* %21, align 4
  %24 = bitcast [16 x float]* %2 to <16 x float>*
  %25 = load <16 x float>, <16 x float>* %24, align 4
  %26 = insertelement <16 x float> %25, float 2.000000e+00, i64 9
  store <16 x float> %26, <16 x float>* %24, align 4
  %27 = bitcast [16 x float]* %2 to <16 x float>*
  %28 = load <16 x float>, <16 x float>* %27, align 4
  %29 = insertelement <16 x float> %28, float 3.000000e+00, i64 13
  store <16 x float> %29, <16 x float>* %27, align 4
  %30 = bitcast [16 x float]* %2 to <16 x float>*
  %31 = load <16 x float>, <16 x float>* %30, align 4
  %32 = insertelement <16 x float> %31, float 4.000000e+00, i64 2
  store <16 x float> %32, <16 x float>* %30, align 4
  %33 = bitcast [16 x float]* %2 to <16 x float>*
  %34 = load <16 x float>, <16 x float>* %33, align 4
  %35 = insertelement <16 x float> %34, float 1.000000e+01, i64 6
  store <16 x float> %35, <16 x float>* %33, align 4
  %36 = bitcast [16 x float]* %2 to <16 x float>*
  %37 = load <16 x float>, <16 x float>* %36, align 4
  %38 = insertelement <16 x float> %37, float 1.100000e+01, i64 10
  store <16 x float> %38, <16 x float>* %36, align 4
  %39 = bitcast [16 x float]* %2 to <16 x float>*
  %40 = load <16 x float>, <16 x float>* %39, align 4
  %41 = insertelement <16 x float> %40, float 1.200000e+01, i64 14
  store <16 x float> %41, <16 x float>* %39, align 4
  %42 = bitcast [16 x float]* %2 to <16 x float>*
  %43 = load <16 x float>, <16 x float>* %42, align 4
  %44 = insertelement <16 x float> %43, float 1.300000e+01, i64 3
  store <16 x float> %44, <16 x float>* %42, align 4
  %45 = bitcast [16 x float]* %2 to <16 x float>*
  %46 = load <16 x float>, <16 x float>* %45, align 4
  %47 = insertelement <16 x float> %46, float 1.400000e+01, i64 7
  store <16 x float> %47, <16 x float>* %45, align 4
  %48 = bitcast [16 x float]* %2 to <16 x float>*
  %49 = load <16 x float>, <16 x float>* %48, align 4
  %50 = insertelement <16 x float> %49, float 1.500000e+01, i64 11
  store <16 x float> %50, <16 x float>* %48, align 4
  %51 = bitcast [16 x float]* %2 to <16 x float>*
  %52 = load <16 x float>, <16 x float>* %51, align 4
  %53 = insertelement <16 x float> %52, float 1.600000e+01, i64 15
  store <16 x float> %53, <16 x float>* %51, align 4
  %54 = bitcast [16 x float]* %2 to <16 x float>*
  %55 = load <16 x float>, <16 x float>* %54, align 4
  %56 = fmul <16 x float> %55, <float 2.000000e+00, float 2.000000e+00, float 2.000000e+00, float 2.000000e+00, float 2.000000e+00, float 2.000000e+00, float 2.000000e+00, float 2.000000e+00, float 2.000000e+00, float 2.000000e+00, float 2.000000e+00, float 2.000000e+00, float 2.000000e+00, float 2.000000e+00, float 2.000000e+00, float 2.000000e+00>
  %57 = bitcast [16 x float]* %3 to <16 x float>*
  store <16 x float> %56, <16 x float>* %57, align 4
  store i32 0, i32* %4, align 4
  br label %58

58:                                               ; preds = %83, %0
  %59 = load i32, i32* %4, align 4
  %60 = icmp slt i32 %59, 4
  br i1 %60, label %61, label %86

61:                                               ; preds = %58
  %62 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str, i64 0, i64 0))
  store i32 0, i32* %5, align 4
  br label %63

63:                                               ; preds = %78, %61
  %64 = load i32, i32* %5, align 4
  %65 = icmp slt i32 %64, 4
  br i1 %65, label %66, label %81

66:                                               ; preds = %63
  %67 = load i32, i32* %4, align 4
  %68 = sext i32 %67 to i64
  %69 = load i32, i32* %5, align 4
  %70 = sext i32 %69 to i64
  %71 = bitcast [16 x float]* %3 to <16 x float>*
  %72 = load <16 x float>, <16 x float>* %71, align 4
  %73 = mul i64 %70, 4
  %74 = add i64 %73, %68
  %75 = extractelement <16 x float> %72, i64 %74
  %76 = fpext float %75 to double
  %77 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.1, i64 0, i64 0), double %76)
  br label %78

78:                                               ; preds = %66
  %79 = load i32, i32* %5, align 4
  %80 = add nsw i32 %79, 1
  store i32 %80, i32* %5, align 4
  br label %63, !llvm.loop !3

81:                                               ; preds = %63
  %82 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.2, i64 0, i64 0))
  br label %83

83:                                               ; preds = %81
  %84 = load i32, i32* %4, align 4
  %85 = add nsw i32 %84, 1
  store i32 %85, i32* %4, align 4
  br label %58, !llvm.loop !5

86:                                               ; preds = %58
  %87 = load i32, i32* %1, align 4
  ret i32 %87
}

declare i32 @printf(i8*, ...) #1

attributes #0 = { noinline nounwind optnone ssp uwtable "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+cx8,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+cx8,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{!"Homebrew clang version 12.0.1"}
!3 = distinct !{!3, !4}
!4 = !{!"llvm.loop.mustprogress"}
!5 = distinct !{!5, !4}
