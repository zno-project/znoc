; ModuleID = '/Users/Eliyahu/ZnO/znoc/../templates.zno'
source_filename = "/Users/Eliyahu/ZnO/znoc/../templates.zno"
target datalayout = "e-m:o-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-darwin21.0.0"

%_GYT6Thingy10instance_0 = type { i32, i32 }
%_GYT6Thingy10instance_1 = type { i32, %_GYT6Thingy10instance_0 }

declare void @printI(i32)

declare i32* @allocate()

declare void @deallocate(i32*)

declare void @set_ptr(i32*, i32)

define void @main(%_GYT6Thingy10instance_1* %0) {
entry:
  %1 = getelementptr %_GYT6Thingy10instance_1, %_GYT6Thingy10instance_1* %0, i32 0, i32 1
  %2 = getelementptr %_GYT6Thingy10instance_0, %_GYT6Thingy10instance_0* %1, i32 0, i32 1
  %3 = load i32, i32* %2
  call void @printI(i32 %3)
  ret void
}

!llvm.module.flags = !{!0, !1}

!0 = !{i32 2, !"Dwarf Version", i32 3}
!1 = !{i32 1, !"Debug Info Version", i32 3}
