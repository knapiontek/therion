; ModuleID = 'test.cpp'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: norecurse nounwind uwtable
define i32 @main() #0 {
  %1 = alloca i32, align 4
  %i = alloca i32, align 4
  %z = alloca i32, align 4
  %f = alloca float, align 4
  %d = alloca double, align 8
  store i32 0, i32* %1, align 4
  store i32 11, i32* %i, align 4
  %2 = load i32, i32* %i, align 4
  %3 = mul nsw i32 %2, 3
  %4 = add nsw i32 4, %3
  store i32 %4, i32* %z, align 4
  store float 0x3FF3333340000000, float* %f, align 4
  store double 3.400000e+00, double* %d, align 8
  %5 = load i32, i32* %z, align 4
  %6 = sitofp i32 %5 to float
  %7 = load float, float* %f, align 4
  %8 = fadd float %6, %7
  %9 = fpext float %8 to double
  %10 = load double, double* %d, align 8
  %11 = fadd double %9, %10
  %12 = fptosi double %11 to i32
  ret i32 %12
}

attributes #0 = { norecurse nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = !{!"clang version 3.8.0-2ubuntu4 (tags/RELEASE_380/final)"}
