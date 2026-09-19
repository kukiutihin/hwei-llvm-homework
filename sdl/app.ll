; ModuleID = 'app.c'
source_filename = "app.c"
target datalayout = "e-m:o-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-n32:64-S128-Fn32"
target triple = "arm64-apple-macosx14.0.0"

; Function Attrs: mustprogress nofree norecurse nosync nounwind sspstrong willreturn memory(none) uwtable(sync)
define range(i32 24478, 16642286) i32 @colorByTemp(i32 noundef %0) local_unnamed_addr #0 {
  %2 = icmp slt i32 %0, 11
  br i1 %2, label %30, label %3

3:                                                ; preds = %1
  %4 = icmp samesign ult i32 %0, 21
  br i1 %4, label %30, label %5

5:                                                ; preds = %3
  %6 = icmp samesign ult i32 %0, 31
  br i1 %6, label %30, label %7

7:                                                ; preds = %5
  %8 = icmp samesign ult i32 %0, 41
  br i1 %8, label %30, label %9

9:                                                ; preds = %7
  %10 = icmp samesign ult i32 %0, 51
  br i1 %10, label %30, label %11

11:                                               ; preds = %9
  %12 = icmp samesign ult i32 %0, 61
  br i1 %12, label %30, label %13

13:                                               ; preds = %11
  %14 = icmp samesign ult i32 %0, 71
  br i1 %14, label %30, label %15

15:                                               ; preds = %13
  %16 = icmp samesign ult i32 %0, 81
  br i1 %16, label %30, label %17

17:                                               ; preds = %15
  %18 = icmp samesign ult i32 %0, 91
  br i1 %18, label %30, label %19

19:                                               ; preds = %17
  %20 = icmp samesign ult i32 %0, 101
  br i1 %20, label %30, label %21

21:                                               ; preds = %19
  %22 = icmp samesign ult i32 %0, 111
  br i1 %22, label %30, label %23

23:                                               ; preds = %21
  %24 = icmp samesign ult i32 %0, 121
  br i1 %24, label %30, label %25

25:                                               ; preds = %23
  %26 = icmp samesign ult i32 %0, 131
  br i1 %26, label %30, label %27

27:                                               ; preds = %25
  %28 = icmp samesign ult i32 %0, 141
  %29 = select i1 %28, i32 7864320, i32 16642285
  br label %30

30:                                               ; preds = %27, %25, %23, %21, %19, %17, %15, %13, %11, %9, %7, %5, %3, %1
  %31 = phi i32 [ 659752, %1 ], [ 1847654, %3 ], [ 24478, %5 ], [ 34739, %7 ], [ 43158, %9 ], [ 181146, %11 ], [ 10676445, %13 ], [ 16052702, %15 ], [ 15912079, %17 ], [ 15382093, %19 ], [ 15958344, %21 ], [ 14701372, %23 ], [ 12653087, %25 ], [ %29, %27 ]
  ret i32 %31
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind sspstrong willreturn memory(argmem: read) uwtable(sync)
define i32 @calcTemp(i32 noundef %0, i32 noundef %1, ptr noundef readonly captures(none) %2) local_unnamed_addr #1 {
  %4 = mul i32 %1, 1536
  %5 = add i32 %4, %0
  %6 = sext i32 %5 to i64
  %7 = getelementptr inbounds i32, ptr %2, i64 %6
  %8 = load i32, ptr %7, align 4, !tbaa !6
  %9 = icmp sgt i32 %1, 0
  br i1 %9, label %10, label %16

10:                                               ; preds = %3
  %11 = add i32 %5, -1536
  %12 = sext i32 %11 to i64
  %13 = getelementptr inbounds i32, ptr %2, i64 %12
  %14 = load i32, ptr %13, align 4, !tbaa !6
  %15 = icmp samesign ult i32 %1, 767
  br i1 %15, label %16, label %22

16:                                               ; preds = %3, %10
  %17 = phi i32 [ %14, %10 ], [ %8, %3 ]
  %18 = add i32 %5, 1536
  %19 = sext i32 %18 to i64
  %20 = getelementptr inbounds i32, ptr %2, i64 %19
  %21 = load i32, ptr %20, align 4, !tbaa !6
  br label %22

22:                                               ; preds = %10, %16
  %23 = phi i32 [ %17, %16 ], [ %14, %10 ]
  %24 = phi i32 [ %21, %16 ], [ %8, %10 ]
  %25 = icmp sgt i32 %0, 0
  br i1 %25, label %26, label %32

26:                                               ; preds = %22
  %27 = add i32 %5, -1
  %28 = sext i32 %27 to i64
  %29 = getelementptr inbounds i32, ptr %2, i64 %28
  %30 = load i32, ptr %29, align 4, !tbaa !6
  %31 = icmp samesign ult i32 %0, 1535
  br i1 %31, label %32, label %38

32:                                               ; preds = %22, %26
  %33 = phi i32 [ %30, %26 ], [ %8, %22 ]
  %34 = add i32 %5, 1
  %35 = sext i32 %34 to i64
  %36 = getelementptr inbounds i32, ptr %2, i64 %35
  %37 = load i32, ptr %36, align 4, !tbaa !6
  br label %38

38:                                               ; preds = %26, %32
  %39 = phi i32 [ %33, %32 ], [ %30, %26 ]
  %40 = phi i32 [ %37, %32 ], [ %8, %26 ]
  %41 = sitofp i32 %8 to double
  %42 = shl i32 %8, 2
  %43 = sub i32 %23, %42
  %44 = add i32 %43, %24
  %45 = add i32 %44, %39
  %46 = add i32 %45, %40
  %47 = sitofp i32 %46 to double
  %48 = tail call double @llvm.fmuladd.f64(double %47, double 2.500000e-01, double %41)
  %49 = fptosi double %48 to i32
  ret i32 %49
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.start.p0(i64 immarg, ptr captures(none)) #2

; Function Attrs: mustprogress nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare double @llvm.fmuladd.f64(double, double, double) #3

; Function Attrs: nofree norecurse nosync nounwind sspstrong memory(argmem: readwrite) uwtable(sync)
define void @recalculateField(ptr noundef readonly captures(none) %0, ptr noundef writeonly captures(none) %1) local_unnamed_addr #4 {
  %3 = getelementptr i8, ptr %0, i64 -6144
  %4 = getelementptr inbounds nuw i8, ptr %0, i64 6144
  %5 = getelementptr i8, ptr %0, i64 -4
  %6 = getelementptr inbounds nuw i8, ptr %0, i64 4
  br label %7

7:                                                ; preds = %2, %48
  %8 = phi i64 [ 0, %2 ], [ %63, %48 ]
  %9 = mul nuw nsw i64 %8, 1536
  %10 = icmp eq i64 %8, 0
  %11 = icmp eq i64 %8, 767
  %12 = getelementptr inbounds nuw i32, ptr %0, i64 %9
  %13 = load i32, ptr %12, align 4, !tbaa !6
  br i1 %10, label %17, label %14

14:                                               ; preds = %7
  %15 = getelementptr i32, ptr %3, i64 %9
  %16 = load i32, ptr %15, align 4, !tbaa !6
  br i1 %11, label %21, label %17

17:                                               ; preds = %14, %7
  %18 = phi i32 [ %16, %14 ], [ %13, %7 ]
  %19 = getelementptr inbounds nuw i32, ptr %4, i64 %9
  %20 = load i32, ptr %19, align 4, !tbaa !6
  br label %21

21:                                               ; preds = %14, %17
  %22 = phi i32 [ %18, %17 ], [ %16, %14 ]
  %23 = phi i32 [ %20, %17 ], [ %13, %14 ]
  %24 = getelementptr inbounds nuw i32, ptr %6, i64 %9
  %25 = load i32, ptr %24, align 4, !tbaa !6
  %26 = sitofp i32 %13 to double
  %27 = shl i32 %13, 2
  %28 = sub i32 %22, %27
  %29 = add i32 %28, %23
  %30 = add i32 %29, %13
  %31 = add i32 %30, %25
  %32 = sitofp i32 %31 to double
  %33 = tail call double @llvm.fmuladd.f64(double %32, double 2.500000e-01, double %26)
  %34 = fptosi double %33 to i32
  %35 = getelementptr inbounds nuw i32, ptr %1, i64 %9
  store i32 %34, ptr %35, align 4, !tbaa !6
  br label %65

36:                                               ; preds = %48
  ret void

37:                                               ; preds = %77
  %38 = add nuw nsw i64 %94, %9
  %39 = getelementptr inbounds nuw i32, ptr %0, i64 %38
  %40 = load i32, ptr %39, align 4, !tbaa !6
  br i1 %10, label %44, label %41

41:                                               ; preds = %37
  %42 = getelementptr i32, ptr %3, i64 %38
  %43 = load i32, ptr %42, align 4, !tbaa !6
  br i1 %11, label %48, label %44

44:                                               ; preds = %41, %37
  %45 = phi i32 [ %43, %41 ], [ %40, %37 ]
  %46 = getelementptr inbounds nuw i32, ptr %4, i64 %38
  %47 = load i32, ptr %46, align 4, !tbaa !6
  br label %48

48:                                               ; preds = %44, %41
  %49 = phi i32 [ %45, %44 ], [ %43, %41 ]
  %50 = phi i32 [ %47, %44 ], [ %40, %41 ]
  %51 = getelementptr i32, ptr %5, i64 %38
  %52 = load i32, ptr %51, align 4, !tbaa !6
  %53 = sitofp i32 %40 to double
  %54 = shl i32 %40, 2
  %55 = sub i32 %49, %54
  %56 = add i32 %55, %50
  %57 = add i32 %56, %52
  %58 = add i32 %57, %40
  %59 = sitofp i32 %58 to double
  %60 = tail call double @llvm.fmuladd.f64(double %59, double 2.500000e-01, double %53)
  %61 = fptosi double %60 to i32
  %62 = getelementptr inbounds nuw i32, ptr %1, i64 %38
  store i32 %61, ptr %62, align 4, !tbaa !6
  %63 = add nuw nsw i64 %8, 1
  %64 = icmp eq i64 %63, 768
  br i1 %64, label %36, label %7, !llvm.loop !10

65:                                               ; preds = %21, %77
  %66 = phi i64 [ 1, %21 ], [ %94, %77 ]
  %67 = add nuw nsw i64 %66, %9
  %68 = getelementptr inbounds nuw i32, ptr %0, i64 %67
  %69 = load i32, ptr %68, align 4, !tbaa !6
  br i1 %10, label %73, label %70

70:                                               ; preds = %65
  %71 = getelementptr i32, ptr %3, i64 %67
  %72 = load i32, ptr %71, align 4, !tbaa !6
  br i1 %11, label %77, label %73

73:                                               ; preds = %70, %65
  %74 = phi i32 [ %72, %70 ], [ %69, %65 ]
  %75 = getelementptr inbounds nuw i32, ptr %4, i64 %67
  %76 = load i32, ptr %75, align 4, !tbaa !6
  br label %77

77:                                               ; preds = %70, %73
  %78 = phi i32 [ %74, %73 ], [ %72, %70 ]
  %79 = phi i32 [ %76, %73 ], [ %69, %70 ]
  %80 = getelementptr i32, ptr %5, i64 %67
  %81 = load i32, ptr %80, align 4, !tbaa !6
  %82 = getelementptr inbounds nuw i32, ptr %6, i64 %67
  %83 = load i32, ptr %82, align 4, !tbaa !6
  %84 = sitofp i32 %69 to double
  %85 = shl i32 %69, 2
  %86 = sub i32 %78, %85
  %87 = add i32 %86, %79
  %88 = add i32 %87, %81
  %89 = add i32 %88, %83
  %90 = sitofp i32 %89 to double
  %91 = tail call double @llvm.fmuladd.f64(double %90, double 2.500000e-01, double %84)
  %92 = fptosi double %91 to i32
  %93 = getelementptr inbounds nuw i32, ptr %1, i64 %67
  store i32 %92, ptr %93, align 4, !tbaa !6
  %94 = add nuw nsw i64 %66, 1
  %95 = icmp eq i64 %94, 1535
  br i1 %95, label %37, label %65, !llvm.loop !12
}

; Function Attrs: nounwind sspstrong uwtable(sync)
define void @drawField(ptr noundef readonly captures(none) %0) local_unnamed_addr #5 {
  br label %2

2:                                                ; preds = %1, %8
  %3 = phi i64 [ 0, %1 ], [ %9, %8 ]
  %4 = mul nuw nsw i64 %3, 6144
  %5 = getelementptr inbounds nuw i8, ptr %0, i64 %4
  %6 = trunc nuw nsw i64 %3 to i32
  br label %11

7:                                                ; preds = %8
  ret void

8:                                                ; preds = %43
  %9 = add nuw nsw i64 %3, 1
  %10 = icmp eq i64 %9, 768
  br i1 %10, label %7, label %2, !llvm.loop !14

11:                                               ; preds = %2, %43
  %12 = phi i64 [ 0, %2 ], [ %46, %43 ]
  %13 = getelementptr inbounds nuw i32, ptr %5, i64 %12
  %14 = load i32, ptr %13, align 4, !tbaa !6
  %15 = icmp slt i32 %14, 11
  br i1 %15, label %43, label %16

16:                                               ; preds = %11
  %17 = icmp samesign ult i32 %14, 21
  br i1 %17, label %43, label %18

18:                                               ; preds = %16
  %19 = icmp samesign ult i32 %14, 31
  br i1 %19, label %43, label %20

20:                                               ; preds = %18
  %21 = icmp samesign ult i32 %14, 41
  br i1 %21, label %43, label %22

22:                                               ; preds = %20
  %23 = icmp samesign ult i32 %14, 51
  br i1 %23, label %43, label %24

24:                                               ; preds = %22
  %25 = icmp samesign ult i32 %14, 61
  br i1 %25, label %43, label %26

26:                                               ; preds = %24
  %27 = icmp samesign ult i32 %14, 71
  br i1 %27, label %43, label %28

28:                                               ; preds = %26
  %29 = icmp samesign ult i32 %14, 81
  br i1 %29, label %43, label %30

30:                                               ; preds = %28
  %31 = icmp samesign ult i32 %14, 91
  br i1 %31, label %43, label %32

32:                                               ; preds = %30
  %33 = icmp samesign ult i32 %14, 101
  br i1 %33, label %43, label %34

34:                                               ; preds = %32
  %35 = icmp samesign ult i32 %14, 111
  br i1 %35, label %43, label %36

36:                                               ; preds = %34
  %37 = icmp samesign ult i32 %14, 121
  br i1 %37, label %43, label %38

38:                                               ; preds = %36
  %39 = icmp samesign ult i32 %14, 131
  br i1 %39, label %43, label %40

40:                                               ; preds = %38
  %41 = icmp samesign ult i32 %14, 141
  %42 = select i1 %41, i32 7864320, i32 16642285
  br label %43

43:                                               ; preds = %11, %16, %18, %20, %22, %24, %26, %28, %30, %32, %34, %36, %38, %40
  %44 = phi i32 [ 659752, %11 ], [ 1847654, %16 ], [ 24478, %18 ], [ 34739, %20 ], [ 43158, %22 ], [ 181146, %24 ], [ 10676445, %26 ], [ 16052702, %28 ], [ 15912079, %30 ], [ 15382093, %32 ], [ 15958344, %34 ], [ 14701372, %36 ], [ 12653087, %38 ], [ %42, %40 ]
  %45 = trunc nuw nsw i64 %12 to i32
  tail call void @simPutPixel(i32 noundef %45, i32 noundef %6, i32 noundef %44) #9
  %46 = add nuw nsw i64 %12, 1
  %47 = icmp eq i64 %46, 1536
  br i1 %47, label %8, label %11, !llvm.loop !15
}

declare void @simPutPixel(i32 noundef, i32 noundef, i32 noundef) local_unnamed_addr #6

; Function Attrs: noreturn nounwind sspstrong uwtable(sync)
define void @app() local_unnamed_addr #7 {
  %1 = alloca [1179648 x i32], align 4
  call void @llvm.lifetime.start.p0(i64 4718592, ptr nonnull %1) #9
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 4 dereferenceable(4718592) %1, i8 0, i64 4718592, i1 false)
  br label %2

2:                                                ; preds = %7, %0
  %3 = phi i64 [ 0, %0 ], [ %8, %7 ]
  %4 = mul nuw nsw i64 %3, 6144
  %5 = getelementptr inbounds nuw i8, ptr %1, i64 %4
  %6 = trunc nuw nsw i64 %3 to i32
  br label %10

7:                                                ; preds = %42
  %8 = add nuw nsw i64 %3, 1
  %9 = icmp eq i64 %8, 768
  br i1 %9, label %47, label %2, !llvm.loop !14

10:                                               ; preds = %42, %2
  %11 = phi i64 [ 0, %2 ], [ %45, %42 ]
  %12 = getelementptr inbounds nuw i32, ptr %5, i64 %11
  %13 = load i32, ptr %12, align 4, !tbaa !6
  %14 = icmp slt i32 %13, 11
  br i1 %14, label %42, label %15

15:                                               ; preds = %10
  %16 = icmp samesign ult i32 %13, 21
  br i1 %16, label %42, label %17

17:                                               ; preds = %15
  %18 = icmp samesign ult i32 %13, 31
  br i1 %18, label %42, label %19

19:                                               ; preds = %17
  %20 = icmp samesign ult i32 %13, 41
  br i1 %20, label %42, label %21

21:                                               ; preds = %19
  %22 = icmp samesign ult i32 %13, 51
  br i1 %22, label %42, label %23

23:                                               ; preds = %21
  %24 = icmp samesign ult i32 %13, 61
  br i1 %24, label %42, label %25

25:                                               ; preds = %23
  %26 = icmp samesign ult i32 %13, 71
  br i1 %26, label %42, label %27

27:                                               ; preds = %25
  %28 = icmp samesign ult i32 %13, 81
  br i1 %28, label %42, label %29

29:                                               ; preds = %27
  %30 = icmp samesign ult i32 %13, 91
  br i1 %30, label %42, label %31

31:                                               ; preds = %29
  %32 = icmp samesign ult i32 %13, 101
  br i1 %32, label %42, label %33

33:                                               ; preds = %31
  %34 = icmp samesign ult i32 %13, 111
  br i1 %34, label %42, label %35

35:                                               ; preds = %33
  %36 = icmp samesign ult i32 %13, 121
  br i1 %36, label %42, label %37

37:                                               ; preds = %35
  %38 = icmp samesign ult i32 %13, 131
  br i1 %38, label %42, label %39

39:                                               ; preds = %37
  %40 = icmp samesign ult i32 %13, 141
  %41 = select i1 %40, i32 7864320, i32 16642285
  br label %42

42:                                               ; preds = %39, %37, %35, %33, %31, %29, %27, %25, %23, %21, %19, %17, %15, %10
  %43 = phi i32 [ 659752, %10 ], [ 1847654, %15 ], [ 24478, %17 ], [ 34739, %19 ], [ 43158, %21 ], [ 181146, %23 ], [ 10676445, %25 ], [ 16052702, %27 ], [ 15912079, %29 ], [ 15382093, %31 ], [ 15958344, %33 ], [ 14701372, %35 ], [ 12653087, %37 ], [ %41, %39 ]
  %44 = trunc nuw nsw i64 %11 to i32
  tail call void @simPutPixel(i32 noundef %44, i32 noundef %6, i32 noundef %43) #9
  %45 = add nuw nsw i64 %11, 1
  %46 = icmp eq i64 %45, 1536
  br i1 %46, label %7, label %10, !llvm.loop !15

47:                                               ; preds = %7
  tail call void @simFlush() #9
  unreachable
}

; Function Attrs: mustprogress nocallback nofree nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr writeonly captures(none), i8, i64, i1 immarg) #8

declare void @simFlush(...) local_unnamed_addr #6

attributes #0 = { mustprogress nofree norecurse nosync nounwind sspstrong willreturn memory(none) uwtable(sync) "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="4" "target-cpu"="apple-m1" "target-features"="+aes,+altnzcv,+ccdp,+ccidx,+ccpp,+complxnum,+crc,+dit,+dotprod,+flagm,+fp-armv8,+fp16fml,+fptoint,+fullfp16,+jsconv,+lse,+neon,+pauth,+perfmon,+predres,+ras,+rcpc,+rdm,+sb,+sha2,+sha3,+specrestrict,+ssbs,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8a" "zero-call-used-regs"="used-gpr" }
attributes #1 = { mustprogress nofree norecurse nosync nounwind sspstrong willreturn memory(argmem: read) uwtable(sync) "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="4" "target-cpu"="apple-m1" "target-features"="+aes,+altnzcv,+ccdp,+ccidx,+ccpp,+complxnum,+crc,+dit,+dotprod,+flagm,+fp-armv8,+fp16fml,+fptoint,+fullfp16,+jsconv,+lse,+neon,+pauth,+perfmon,+predres,+ras,+rcpc,+rdm,+sb,+sha2,+sha3,+specrestrict,+ssbs,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8a" "zero-call-used-regs"="used-gpr" }
attributes #2 = { mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite) }
attributes #3 = { mustprogress nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #4 = { nofree norecurse nosync nounwind sspstrong memory(argmem: readwrite) uwtable(sync) "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="4" "target-cpu"="apple-m1" "target-features"="+aes,+altnzcv,+ccdp,+ccidx,+ccpp,+complxnum,+crc,+dit,+dotprod,+flagm,+fp-armv8,+fp16fml,+fptoint,+fullfp16,+jsconv,+lse,+neon,+pauth,+perfmon,+predres,+ras,+rcpc,+rdm,+sb,+sha2,+sha3,+specrestrict,+ssbs,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8a" "zero-call-used-regs"="used-gpr" }
attributes #5 = { nounwind sspstrong uwtable(sync) "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="4" "target-cpu"="apple-m1" "target-features"="+aes,+altnzcv,+ccdp,+ccidx,+ccpp,+complxnum,+crc,+dit,+dotprod,+flagm,+fp-armv8,+fp16fml,+fptoint,+fullfp16,+jsconv,+lse,+neon,+pauth,+perfmon,+predres,+ras,+rcpc,+rdm,+sb,+sha2,+sha3,+specrestrict,+ssbs,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8a" "zero-call-used-regs"="used-gpr" }
attributes #6 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="4" "target-cpu"="apple-m1" "target-features"="+aes,+altnzcv,+ccdp,+ccidx,+ccpp,+complxnum,+crc,+dit,+dotprod,+flagm,+fp-armv8,+fp16fml,+fptoint,+fullfp16,+jsconv,+lse,+neon,+pauth,+perfmon,+predres,+ras,+rcpc,+rdm,+sb,+sha2,+sha3,+specrestrict,+ssbs,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8a" "zero-call-used-regs"="used-gpr" }
attributes #7 = { noreturn nounwind sspstrong uwtable(sync) "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="4" "target-cpu"="apple-m1" "target-features"="+aes,+altnzcv,+ccdp,+ccidx,+ccpp,+complxnum,+crc,+dit,+dotprod,+flagm,+fp-armv8,+fp16fml,+fptoint,+fullfp16,+jsconv,+lse,+neon,+pauth,+perfmon,+predres,+ras,+rcpc,+rdm,+sb,+sha2,+sha3,+specrestrict,+ssbs,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8a" "zero-call-used-regs"="used-gpr" }
attributes #8 = { mustprogress nocallback nofree nounwind willreturn memory(argmem: write) }
attributes #9 = { nounwind }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 2, !"SDK Version", [2 x i32] [i32 14, i32 4]}
!1 = !{i32 1, !"wchar_size", i32 4}
!2 = !{i32 8, !"PIC Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 1}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"clang version 21.1.8"}
!6 = !{!7, !7, i64 0}
!7 = !{!"int", !8, i64 0}
!8 = !{!"omnipotent char", !9, i64 0}
!9 = !{!"Simple C/C++ TBAA"}
!10 = distinct !{!10, !11}
!11 = !{!"llvm.loop.mustprogress"}
!12 = distinct !{!12, !11, !13}
!13 = !{!"llvm.loop.peeled.count", i32 2}
!14 = distinct !{!14, !11}
!15 = distinct !{!15, !11}
