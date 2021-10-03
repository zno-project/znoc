; ModuleID = '/Users/Eliyahu/ZnO/znoc/../templates.zno'
source_filename = "/Users/Eliyahu/ZnO/znoc/../templates.zno"
target datalayout = "e-m:o-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-darwin21.0.0"

%_GYT6Thingy10instance_0 = type { i32, i32 }
%_GYT6Thingy10instance_1 = type { i32, i32 }
%_GYT6Thingy10instance_2 = type { i64, i64 }

declare void @printI(i32)

declare i32* @allocate()

declare void @deallocate(i32*)

declare void @set_ptr(i32*, i32)

define i32 @_GF17printAStructThing(%_GYT6Thingy10instance_0 %s) {
entry:
  %s_stack = alloca %_GYT6Thingy10instance_0, align 8
  store %_GYT6Thingy10instance_0 %s, %_GYT6Thingy10instance_0* %s_stack, align 4
  %0 = bitcast %_GYT6Thingy10instance_0* %s_stack to i32*
  %1 = load i32, i32* %0, align 4
  call void @printI(i32 %1)
  %2 = getelementptr %_GYT6Thingy10instance_0, %_GYT6Thingy10instance_0* %s_stack, i32 0, i32 1
  %3 = load i32, i32* %2, align 4
  ret i32 %3
}

define void @main() {
entry:
  %x_stack = alloca %_GYT6Thingy10instance_1, align 8
  %p_stack = alloca %_GYT6Thingy10instance_2, align 8
  %s_stack = alloca %_GYT6Thingy10instance_1, align 8
  %0 = load %_GYT6Thingy10instance_1, %_GYT6Thingy10instance_1* %s_stack, align 4
  %1 = load %_GYT6Thingy10instance_2, %_GYT6Thingy10instance_2* %p_stack, align 8
  %2 = bitcast %_GYT6Thingy10instance_1* %s_stack to i32*
  store i32 32, i32* %2, align 4
  %3 = bitcast %_GYT6Thingy10instance_1* %s_stack to i32*
  %4 = load i32, i32* %3, align 4
  %5 = getelementptr %_GYT6Thingy10instance_1, %_GYT6Thingy10instance_1* %s_stack, i32 0, i32 1
  store i32 65, i32* %5, align 4
  %6 = getelementptr %_GYT6Thingy10instance_1, %_GYT6Thingy10instance_1* %s_stack, i32 0, i32 1
  %7 = load i32, i32* %6, align 4
  %8 = getelementptr %_GYT6Thingy10instance_2, %_GYT6Thingy10instance_2* %p_stack, i32 0, i32 1
  store i64 72, i64* %8, align 4
  %9 = getelementptr %_GYT6Thingy10instance_2, %_GYT6Thingy10instance_2* %p_stack, i32 0, i32 1
  %10 = load i64, i64* %9, align 8
  %11 = bitcast %_GYT6Thingy10instance_1* %s_stack to i32*
  %12 = load i32, i32* %11, align 4
  call void @printI(i32 %12)
  %13 = getelementptr %_GYT6Thingy10instance_1, %_GYT6Thingy10instance_1* %s_stack, i32 0, i32 1
  %14 = load i32, i32* %13, align 4
  %15 = load %_GYT6Thingy10instance_1, %_GYT6Thingy10instance_1* %x_stack, align 4
  %16 = bitcast %_GYT6Thingy10instance_1* %x_stack to %_GYT6Thingy10instance_1*
  %17 = bitcast %_GYT6Thingy10instance_1* %16 to i32*
  %18 = load i32, i32* %17, align 4
  call void @printI(i32 %18)
  ret void
}

!llvm.module.flags = !{!0, !1}

!0 = !{i32 2, !"Dwarf Version", i32 3}
!1 = !{i32 1, !"Debug Info Version", i32 3}
