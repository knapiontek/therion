; ModuleID = 'malloc.cpp'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

%struct.TestStruct = type { i32, double }

; Function Attrs: norecurse nounwind uwtable
define i32 @main() #0 {
  %1 = alloca i32, align 4
  %ts = alloca %struct.TestStruct*, align 8
  store i32 0, i32* %1, align 4
  %2 = call noalias i8* @malloc(i64 16) #2
  %3 = bitcast i8* %2 to %struct.TestStruct*
  store %struct.TestStruct* %3, %struct.TestStruct** %ts, align 8
  %4 = load %struct.TestStruct*, %struct.TestStruct** %ts, align 8
  %5 = getelementptr inbounds %struct.TestStruct, %struct.TestStruct* %4, i32 0, i32 0
  store i32 1, i32* %5, align 8
  %6 = load %struct.TestStruct*, %struct.TestStruct** %ts, align 8
  %7 = getelementptr inbounds %struct.TestStruct, %struct.TestStruct* %6, i32 0, i32 1
  store double 2.000000e+00, double* %7, align 8
  %8 = load %struct.TestStruct*, %struct.TestStruct** %ts, align 8
  %9 = ptrtoint %struct.TestStruct* %8 to i64
  %10 = trunc i64 %9 to i32
  ret i32 %10
}

; Function Attrs: nounwind
declare noalias i8* @malloc(i64) #1

attributes #0 = { norecurse nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { nounwind }

!llvm.ident = !{!0}

!0 = !{!"clang version 3.8.0-2ubuntu4 (tags/RELEASE_380/final)"}
