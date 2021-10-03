; ModuleID = 'exception.c'
source_filename = "exception.c"
target datalayout = "e-m:o-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx12.0.0"

%struct._my_Unwind_Exception = type { %struct._Unwind_Exception, i64, [8 x i8] }
%struct._Unwind_Exception = type { i64, void (i32, %struct._Unwind_Exception*)*, i64, i64 }
%struct._Unwind_Context = type opaque

@.str = private unnamed_addr constant [36 x i8] c"Personality function, lookup phase\0A\00", align 1
@.str.1 = private unnamed_addr constant [31 x i8] c"Personality function, cleanup\0A\00", align 1
@.str.2 = private unnamed_addr constant [29 x i8] c"Personality function, error\0A\00", align 1

; Function Attrs: noinline nounwind optnone ssp uwtable
define dso_local i32 @__my_personality_fn(i32 %0, i32 %1, i64 %2, %struct._my_Unwind_Exception* %3, %struct._Unwind_Context* %4) #0 !dbg !29 {
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  %8 = alloca i32, align 4
  %9 = alloca i64, align 8
  %10 = alloca %struct._my_Unwind_Exception*, align 8
  %11 = alloca %struct._Unwind_Context*, align 8
  store i32 %0, i32* %7, align 4
  call void @llvm.dbg.declare(metadata i32* %7, metadata !60, metadata !DIExpression()), !dbg !61
  store i32 %1, i32* %8, align 4
  call void @llvm.dbg.declare(metadata i32* %8, metadata !62, metadata !DIExpression()), !dbg !63
  store i64 %2, i64* %9, align 8
  call void @llvm.dbg.declare(metadata i64* %9, metadata !64, metadata !DIExpression()), !dbg !65
  store %struct._my_Unwind_Exception* %3, %struct._my_Unwind_Exception** %10, align 8
  call void @llvm.dbg.declare(metadata %struct._my_Unwind_Exception** %10, metadata !66, metadata !DIExpression()), !dbg !67
  store %struct._Unwind_Context* %4, %struct._Unwind_Context** %11, align 8
  call void @llvm.dbg.declare(metadata %struct._Unwind_Context** %11, metadata !68, metadata !DIExpression()), !dbg !69
  %12 = load i32, i32* %8, align 4, !dbg !70
  %13 = and i32 %12, 1, !dbg !72
  %14 = icmp ne i32 %13, 0, !dbg !72
  br i1 %14, label %15, label %17, !dbg !73

15:                                               ; preds = %5
  %16 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([36 x i8], [36 x i8]* @.str, i64 0, i64 0)), !dbg !74
  store i32 6, i32* %6, align 4, !dbg !76
  br label %25, !dbg !76

17:                                               ; preds = %5
  %18 = load i32, i32* %8, align 4, !dbg !77
  %19 = and i32 %18, 2, !dbg !79
  %20 = icmp ne i32 %19, 0, !dbg !79
  br i1 %20, label %21, label %23, !dbg !80

21:                                               ; preds = %17
  %22 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([31 x i8], [31 x i8]* @.str.1, i64 0, i64 0)), !dbg !81
  store i32 7, i32* %6, align 4, !dbg !83
  br label %25, !dbg !83

23:                                               ; preds = %17
  %24 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([29 x i8], [29 x i8]* @.str.2, i64 0, i64 0)), !dbg !84
  store i32 3, i32* %6, align 4, !dbg !86
  br label %25, !dbg !86

25:                                               ; preds = %23, %21, %15
  %26 = load i32, i32* %6, align 4, !dbg !87
  ret i32 %26, !dbg !87
}

; Function Attrs: nofree nosync nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

declare i32 @printf(i8*, ...) #2

attributes #0 = { noinline nounwind optnone ssp uwtable "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+cx8,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nofree nosync nounwind readnone speculatable willreturn }
attributes #2 = { "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+cx8,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!24, !25, !26, !27}
!llvm.ident = !{!28}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "Homebrew clang version 12.0.1", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2, nameTableKind: None, sysroot: "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk", sdk: "MacOSX.sdk")
!1 = !DIFile(filename: "exception.c", directory: "/Users/Eliyahu/ZnO/tmp")
!2 = !{!3, !17}
!3 = !DICompositeType(tag: DW_TAG_enumeration_type, file: !4, line: 32, baseType: !5, size: 32, elements: !6)
!4 = !DIFile(filename: "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/unwind.h", directory: "")
!5 = !DIBasicType(name: "unsigned int", size: 32, encoding: DW_ATE_unsigned)
!6 = !{!7, !8, !9, !10, !11, !12, !13, !14, !15, !16}
!7 = !DIEnumerator(name: "_URC_NO_REASON", value: 0, isUnsigned: true)
!8 = !DIEnumerator(name: "_URC_OK", value: 0, isUnsigned: true)
!9 = !DIEnumerator(name: "_URC_FOREIGN_EXCEPTION_CAUGHT", value: 1, isUnsigned: true)
!10 = !DIEnumerator(name: "_URC_FATAL_PHASE2_ERROR", value: 2, isUnsigned: true)
!11 = !DIEnumerator(name: "_URC_FATAL_PHASE1_ERROR", value: 3, isUnsigned: true)
!12 = !DIEnumerator(name: "_URC_NORMAL_STOP", value: 4, isUnsigned: true)
!13 = !DIEnumerator(name: "_URC_END_OF_STACK", value: 5, isUnsigned: true)
!14 = !DIEnumerator(name: "_URC_HANDLER_FOUND", value: 6, isUnsigned: true)
!15 = !DIEnumerator(name: "_URC_INSTALL_CONTEXT", value: 7, isUnsigned: true)
!16 = !DIEnumerator(name: "_URC_CONTINUE_UNWIND", value: 8, isUnsigned: true)
!17 = !DICompositeType(tag: DW_TAG_enumeration_type, file: !4, line: 48, baseType: !5, size: 32, elements: !18)
!18 = !{!19, !20, !21, !22, !23}
!19 = !DIEnumerator(name: "_UA_SEARCH_PHASE", value: 1, isUnsigned: true)
!20 = !DIEnumerator(name: "_UA_CLEANUP_PHASE", value: 2, isUnsigned: true)
!21 = !DIEnumerator(name: "_UA_HANDLER_FRAME", value: 4, isUnsigned: true)
!22 = !DIEnumerator(name: "_UA_FORCE_UNWIND", value: 8, isUnsigned: true)
!23 = !DIEnumerator(name: "_UA_END_OF_STACK", value: 16, isUnsigned: true)
!24 = !{i32 7, !"Dwarf Version", i32 4}
!25 = !{i32 2, !"Debug Info Version", i32 3}
!26 = !{i32 1, !"wchar_size", i32 4}
!27 = !{i32 7, !"PIC Level", i32 2}
!28 = !{!"Homebrew clang version 12.0.1"}
!29 = distinct !DISubprogram(name: "__my_personality_fn", scope: !1, file: !1, line: 10, type: !30, scopeLine: 10, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !59)
!30 = !DISubroutineType(types: !31)
!31 = !{!32, !33, !34, !35, !38, !57}
!32 = !DIDerivedType(tag: DW_TAG_typedef, name: "_Unwind_Reason_Code", file: !4, line: 46, baseType: !3)
!33 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!34 = !DIDerivedType(tag: DW_TAG_typedef, name: "_Unwind_Action", file: !4, line: 54, baseType: !17)
!35 = !DIDerivedType(tag: DW_TAG_typedef, name: "uint64_t", file: !36, line: 31, baseType: !37)
!36 = !DIFile(filename: "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/_types/_uint64_t.h", directory: "")
!37 = !DIBasicType(name: "long long unsigned int", size: 64, encoding: DW_ATE_unsigned)
!38 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !39, size: 64)
!39 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "_my_Unwind_Exception", file: !1, line: 5, size: 384, elements: !40)
!40 = !{!41, !56}
!41 = !DIDerivedType(tag: DW_TAG_member, name: "internal", scope: !39, file: !1, line: 6, baseType: !42, size: 256, align: 128)
!42 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "_Unwind_Exception", file: !4, line: 122, size: 256, align: 128, elements: !43)
!43 = !{!44, !45, !51, !55}
!44 = !DIDerivedType(tag: DW_TAG_member, name: "exception_class", scope: !42, file: !4, line: 123, baseType: !35, size: 64)
!45 = !DIDerivedType(tag: DW_TAG_member, name: "exception_cleanup", scope: !42, file: !4, line: 124, baseType: !46, size: 64, offset: 64)
!46 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !47, size: 64)
!47 = !DISubroutineType(types: !48)
!48 = !{null, !32, !49}
!49 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !50, size: 64)
!50 = !DIDerivedType(tag: DW_TAG_typedef, name: "_Unwind_Exception", file: !4, line: 120, baseType: !42)
!51 = !DIDerivedType(tag: DW_TAG_member, name: "private_1", scope: !42, file: !4, line: 129, baseType: !52, size: 64, offset: 128)
!52 = !DIDerivedType(tag: DW_TAG_typedef, name: "uintptr_t", file: !53, line: 34, baseType: !54)
!53 = !DIFile(filename: "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_uintptr_t.h", directory: "")
!54 = !DIBasicType(name: "long unsigned int", size: 64, encoding: DW_ATE_unsigned)
!55 = !DIDerivedType(tag: DW_TAG_member, name: "private_2", scope: !42, file: !4, line: 130, baseType: !52, size: 64, offset: 192)
!56 = !DIDerivedType(tag: DW_TAG_member, name: "myThing", scope: !39, file: !1, line: 7, baseType: !35, size: 64, offset: 256)
!57 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !58, size: 64)
!58 = !DICompositeType(tag: DW_TAG_structure_type, name: "_Unwind_Context", file: !4, line: 56, flags: DIFlagFwdDecl)
!59 = !{}
!60 = !DILocalVariable(name: "version", arg: 1, scope: !29, file: !1, line: 10, type: !33)
!61 = !DILocation(line: 10, column: 45, scope: !29)
!62 = !DILocalVariable(name: "actions", arg: 2, scope: !29, file: !1, line: 10, type: !34)
!63 = !DILocation(line: 10, column: 69, scope: !29)
!64 = !DILocalVariable(name: "exceptionClass", arg: 3, scope: !29, file: !1, line: 10, type: !35)
!65 = !DILocation(line: 10, column: 87, scope: !29)
!66 = !DILocalVariable(name: "exceptionObject", arg: 4, scope: !29, file: !1, line: 10, type: !38)
!67 = !DILocation(line: 10, column: 132, scope: !29)
!68 = !DILocalVariable(name: "context", arg: 5, scope: !29, file: !1, line: 10, type: !57)
!69 = !DILocation(line: 10, column: 173, scope: !29)
!70 = !DILocation(line: 11, column: 6, scope: !71)
!71 = distinct !DILexicalBlock(scope: !29, file: !1, line: 11, column: 6)
!72 = !DILocation(line: 11, column: 14, scope: !71)
!73 = !DILocation(line: 11, column: 6, scope: !29)
!74 = !DILocation(line: 12, column: 3, scope: !75)
!75 = distinct !DILexicalBlock(scope: !71, file: !1, line: 11, column: 34)
!76 = !DILocation(line: 13, column: 3, scope: !75)
!77 = !DILocation(line: 14, column: 13, scope: !78)
!78 = distinct !DILexicalBlock(scope: !71, file: !1, line: 14, column: 13)
!79 = !DILocation(line: 14, column: 21, scope: !78)
!80 = !DILocation(line: 14, column: 13, scope: !71)
!81 = !DILocation(line: 15, column: 3, scope: !82)
!82 = distinct !DILexicalBlock(scope: !78, file: !1, line: 14, column: 42)
!83 = !DILocation(line: 16, column: 3, scope: !82)
!84 = !DILocation(line: 18, column: 3, scope: !85)
!85 = distinct !DILexicalBlock(scope: !78, file: !1, line: 17, column: 9)
!86 = !DILocation(line: 19, column: 3, scope: !85)
!87 = !DILocation(line: 21, column: 1, scope: !29)
