; ModuleID = 'static.cpp'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

%struct.TestStruct = type { i32 }

; Function Attrs: nounwind uwtable
define void @_Z14destroy_structP10TestStruct(%struct.TestStruct* %pts) #0 {
  %1 = alloca %struct.TestStruct*, align 8
  store %struct.TestStruct* %pts, %struct.TestStruct** %1, align 8
  %2 = load %struct.TestStruct*, %struct.TestStruct** %1, align 8
  %3 = bitcast %struct.TestStruct* %2 to i8*
  call void @free(i8* %3) #3
  ret void
}

; Function Attrs: nounwind
declare void @free(i8*) #1

; Function Attrs: norecurse nounwind uwtable
define i32 @main() #2 {
  %1 = alloca i32, align 4
  %ts = alloca %struct.TestStruct, align 4
  %pts = alloca %struct.TestStruct*, align 8
  store i32 0, i32* %1, align 4
  store %struct.TestStruct* %ts, %struct.TestStruct** %pts, align 8
  %2 = load %struct.TestStruct*, %struct.TestStruct** %pts, align 8
  call void @_Z14destroy_structP10TestStruct(%struct.TestStruct* %2)
  %3 = load %struct.TestStruct*, %struct.TestStruct** %pts, align 8
  %4 = getelementptr inbounds %struct.TestStruct, %struct.TestStruct* %3, i32 0, i32 0
  store i32 999, i32* %4, align 4
  %5 = getelementptr inbounds %struct.TestStruct, %struct.TestStruct* %ts, i32 0, i32 0
  %6 = load i32, i32* %5, align 4
  ret i32 %6
}

attributes #0 = { nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { norecurse nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { nounwind }

!llvm.ident = !{!0}

!0 = !{!"clang version 3.8.0-2ubuntu4 (tags/RELEASE_380/final)"}
