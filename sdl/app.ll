; ModuleID = 'app.c'
source_filename = "app.c"
target datalayout = "e-m:o-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-n32:64-S128-Fn32"
target triple = "arm64-apple-macosx14.0.0"

@colors = internal unnamed_addr constant [10 x i32] [i32 4, i32 1444665, i32 4328040, i32 6952814, i32 9643623, i32 12203605, i32 14504250, i32 15955986, i32 16557322, i32 16181408], align 4

; Function Attrs: mustprogress nofree norecurse nosync nounwind sspstrong willreturn memory(none) uwtable(sync)
define float @rgb2linear(i32 noundef %0) local_unnamed_addr #0 {
  %2 = uitofp i32 %0 to float
  %3 = fdiv float %2, 2.550000e+02
  %4 = fpext float %3 to double
  %5 = fcmp ugt double %4, 4.045000e-02
  br i1 %5, label %8, label %6

6:                                                ; preds = %1
  %7 = fdiv double %4, 1.292000e+01
  br label %12

8:                                                ; preds = %1
  %9 = fadd double %4, 5.500000e-02
  %10 = fdiv double %9, 1.055000e+00
  %11 = tail call double @llvm.pow.f64(double %10, double 2.400000e+00)
  br label %12

12:                                               ; preds = %8, %6
  %13 = phi double [ %7, %6 ], [ %11, %8 ]
  %14 = fptrunc double %13 to float
  ret float %14
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.start.p0(i64 immarg, ptr captures(none)) #1

; Function Attrs: mustprogress nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare double @llvm.pow.f64(double, double) #2

; Function Attrs: mustprogress nofree norecurse nosync nounwind sspstrong willreturn memory(none) uwtable(sync)
define range(i32 0, 256) i32 @linear2rgb(float noundef %0) local_unnamed_addr #0 {
  %2 = fpext float %0 to double
  %3 = fcmp ugt double %2, 3.130800e-03
  br i1 %3, label %6, label %4

4:                                                ; preds = %1
  %5 = fmul double %2, 1.292000e+01
  br label %9

6:                                                ; preds = %1
  %7 = tail call double @llvm.pow.f64(double %2, double 0x3FDAAAAAAAAAAAAB)
  %8 = tail call double @llvm.fmuladd.f64(double %7, double 1.055000e+00, double -5.500000e-02)
  br label %9

9:                                                ; preds = %6, %4
  %10 = phi double [ %5, %4 ], [ %8, %6 ]
  %11 = fptrunc double %10 to float
  %12 = fmul float %11, 2.550000e+02
  %13 = fptoui float %12 to i8
  %14 = zext i8 %13 to i32
  ret i32 %14
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare double @llvm.fmuladd.f64(double, double, double) #2

; Function Attrs: mustprogress nofree norecurse nosync nounwind sspstrong willreturn memory(none) uwtable(sync)
define range(i32 0, 16777216) i32 @interpolate(i32 noundef %0, i32 noundef %1, float noundef %2) local_unnamed_addr #0 {
  %4 = lshr i32 %0, 16
  %5 = and i32 %4, 255
  %6 = uitofp nneg i32 %5 to float
  %7 = fdiv float %6, 2.550000e+02
  %8 = fpext float %7 to double
  %9 = fcmp ugt double %8, 4.045000e-02
  br i1 %9, label %12, label %10

10:                                               ; preds = %3
  %11 = fdiv double %8, 1.292000e+01
  br label %16

12:                                               ; preds = %3
  %13 = fadd double %8, 5.500000e-02
  %14 = fdiv double %13, 1.055000e+00
  %15 = tail call double @llvm.pow.f64(double %14, double 2.400000e+00)
  br label %16

16:                                               ; preds = %10, %12
  %17 = phi double [ %11, %10 ], [ %15, %12 ]
  %18 = fptrunc double %17 to float
  %19 = lshr i32 %0, 8
  %20 = and i32 %19, 255
  %21 = uitofp nneg i32 %20 to float
  %22 = fdiv float %21, 2.550000e+02
  %23 = fpext float %22 to double
  %24 = fcmp ugt double %23, 4.045000e-02
  br i1 %24, label %27, label %25

25:                                               ; preds = %16
  %26 = fdiv double %23, 1.292000e+01
  br label %31

27:                                               ; preds = %16
  %28 = fadd double %23, 5.500000e-02
  %29 = fdiv double %28, 1.055000e+00
  %30 = tail call double @llvm.pow.f64(double %29, double 2.400000e+00)
  br label %31

31:                                               ; preds = %25, %27
  %32 = phi double [ %26, %25 ], [ %30, %27 ]
  %33 = fptrunc double %32 to float
  %34 = and i32 %0, 255
  %35 = uitofp nneg i32 %34 to float
  %36 = fdiv float %35, 2.550000e+02
  %37 = fpext float %36 to double
  %38 = fcmp ugt double %37, 4.045000e-02
  br i1 %38, label %41, label %39

39:                                               ; preds = %31
  %40 = fdiv double %37, 1.292000e+01
  br label %45

41:                                               ; preds = %31
  %42 = fadd double %37, 5.500000e-02
  %43 = fdiv double %42, 1.055000e+00
  %44 = tail call double @llvm.pow.f64(double %43, double 2.400000e+00)
  br label %45

45:                                               ; preds = %39, %41
  %46 = phi double [ %40, %39 ], [ %44, %41 ]
  %47 = fptrunc double %46 to float
  %48 = lshr i32 %1, 16
  %49 = and i32 %48, 255
  %50 = uitofp nneg i32 %49 to float
  %51 = fdiv float %50, 2.550000e+02
  %52 = fpext float %51 to double
  %53 = fcmp ugt double %52, 4.045000e-02
  br i1 %53, label %56, label %54

54:                                               ; preds = %45
  %55 = fdiv double %52, 1.292000e+01
  br label %60

56:                                               ; preds = %45
  %57 = fadd double %52, 5.500000e-02
  %58 = fdiv double %57, 1.055000e+00
  %59 = tail call double @llvm.pow.f64(double %58, double 2.400000e+00)
  br label %60

60:                                               ; preds = %54, %56
  %61 = phi double [ %55, %54 ], [ %59, %56 ]
  %62 = fptrunc double %61 to float
  %63 = lshr i32 %1, 8
  %64 = and i32 %63, 255
  %65 = uitofp nneg i32 %64 to float
  %66 = fdiv float %65, 2.550000e+02
  %67 = fpext float %66 to double
  %68 = fcmp ugt double %67, 4.045000e-02
  br i1 %68, label %71, label %69

69:                                               ; preds = %60
  %70 = fdiv double %67, 1.292000e+01
  br label %75

71:                                               ; preds = %60
  %72 = fadd double %67, 5.500000e-02
  %73 = fdiv double %72, 1.055000e+00
  %74 = tail call double @llvm.pow.f64(double %73, double 2.400000e+00)
  br label %75

75:                                               ; preds = %69, %71
  %76 = phi double [ %70, %69 ], [ %74, %71 ]
  %77 = fptrunc double %76 to float
  %78 = and i32 %1, 255
  %79 = uitofp nneg i32 %78 to float
  %80 = fdiv float %79, 2.550000e+02
  %81 = fpext float %80 to double
  %82 = fcmp ugt double %81, 4.045000e-02
  br i1 %82, label %85, label %83

83:                                               ; preds = %75
  %84 = fdiv double %81, 1.292000e+01
  br label %89

85:                                               ; preds = %75
  %86 = fadd double %81, 5.500000e-02
  %87 = fdiv double %86, 1.055000e+00
  %88 = tail call double @llvm.pow.f64(double %87, double 2.400000e+00)
  br label %89

89:                                               ; preds = %83, %85
  %90 = phi double [ %84, %83 ], [ %88, %85 ]
  %91 = fptrunc double %90 to float
  %92 = fsub float %62, %18
  %93 = tail call float @llvm.fmuladd.f32(float %92, float %2, float %18)
  %94 = fpext float %93 to double
  %95 = fcmp ugt double %94, 3.130800e-03
  br i1 %95, label %98, label %96

96:                                               ; preds = %89
  %97 = fmul double %94, 1.292000e+01
  br label %101

98:                                               ; preds = %89
  %99 = tail call double @llvm.pow.f64(double %94, double 0x3FDAAAAAAAAAAAAB)
  %100 = tail call double @llvm.fmuladd.f64(double %99, double 1.055000e+00, double -5.500000e-02)
  br label %101

101:                                              ; preds = %96, %98
  %102 = phi double [ %97, %96 ], [ %100, %98 ]
  %103 = fsub float %77, %33
  %104 = tail call float @llvm.fmuladd.f32(float %103, float %2, float %33)
  %105 = fpext float %104 to double
  %106 = fcmp ugt double %105, 3.130800e-03
  br i1 %106, label %109, label %107

107:                                              ; preds = %101
  %108 = fmul double %105, 1.292000e+01
  br label %112

109:                                              ; preds = %101
  %110 = tail call double @llvm.pow.f64(double %105, double 0x3FDAAAAAAAAAAAAB)
  %111 = tail call double @llvm.fmuladd.f64(double %110, double 1.055000e+00, double -5.500000e-02)
  br label %112

112:                                              ; preds = %107, %109
  %113 = phi double [ %108, %107 ], [ %111, %109 ]
  %114 = fsub float %91, %47
  %115 = tail call float @llvm.fmuladd.f32(float %114, float %2, float %47)
  %116 = fpext float %115 to double
  %117 = fcmp ugt double %116, 3.130800e-03
  br i1 %117, label %120, label %118

118:                                              ; preds = %112
  %119 = fmul double %116, 1.292000e+01
  br label %123

120:                                              ; preds = %112
  %121 = tail call double @llvm.pow.f64(double %116, double 0x3FDAAAAAAAAAAAAB)
  %122 = tail call double @llvm.fmuladd.f64(double %121, double 1.055000e+00, double -5.500000e-02)
  br label %123

123:                                              ; preds = %118, %120
  %124 = phi double [ %119, %118 ], [ %122, %120 ]
  %125 = fptrunc double %113 to float
  %126 = fmul float %125, 2.550000e+02
  %127 = fptoui float %126 to i8
  %128 = zext i8 %127 to i32
  %129 = fptrunc double %102 to float
  %130 = fmul float %129, 2.550000e+02
  %131 = fptoui float %130 to i8
  %132 = zext i8 %131 to i32
  %133 = fptrunc double %124 to float
  %134 = fmul float %133, 2.550000e+02
  %135 = fptoui float %134 to i8
  %136 = zext i8 %135 to i32
  %137 = shl nuw nsw i32 %132, 16
  %138 = shl nuw nsw i32 %128, 8
  %139 = or disjoint i32 %138, %137
  %140 = or disjoint i32 %139, %136
  ret i32 %140
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare float @llvm.fmuladd.f32(float, float, float) #2

; Function Attrs: mustprogress nofree norecurse nosync nounwind sspstrong willreturn memory(none) uwtable(sync)
define range(i32 0, 16777216) i32 @colorByTemp(float noundef %0) local_unnamed_addr #0 {
  %2 = fcmp ult float %0, 1.000000e+02
  br i1 %2, label %3, label %20

3:                                                ; preds = %1
  %4 = fpext float %0 to double
  %5 = tail call double @llvm.minnum.f64(double %4, double 1.000000e+02)
  %6 = tail call double @llvm.maxnum.f64(double %5, double 0.000000e+00)
  %7 = fdiv double %6, 1.000000e+01
  %8 = fptrunc double %7 to float
  %9 = fptoui float %8 to i32
  %10 = zext i32 %9 to i64
  %11 = getelementptr inbounds nuw [10 x i32], ptr @colors, i64 0, i64 %10
  %12 = load i32, ptr %11, align 4, !tbaa !6
  %13 = add i32 %9, 1
  %14 = zext i32 %13 to i64
  %15 = getelementptr inbounds nuw [10 x i32], ptr @colors, i64 0, i64 %14
  %16 = load i32, ptr %15, align 4, !tbaa !6
  %17 = uitofp i32 %9 to float
  %18 = fsub float %8, %17
  %19 = tail call i32 @interpolate(i32 noundef %12, i32 noundef %16, float noundef %18)
  br label %20

20:                                               ; preds = %1, %3
  %21 = phi i32 [ %19, %3 ], [ 15955986, %1 ]
  ret i32 %21
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare double @llvm.minnum.f64(double, double) #2

; Function Attrs: mustprogress nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare double @llvm.maxnum.f64(double, double) #2

; Function Attrs: mustprogress nofree norecurse nosync nounwind sspstrong willreturn memory(argmem: read) uwtable(sync)
define float @calcTemp(i32 noundef %0, i32 noundef %1, ptr noundef readonly captures(none) %2) local_unnamed_addr #3 {
  %4 = mul i32 %1, 1536
  %5 = lshr exact i32 %4, 2
  %6 = add i32 %5, %0
  %7 = zext i32 %6 to i64
  %8 = getelementptr inbounds nuw float, ptr %2, i64 %7
  %9 = load float, ptr %8, align 4, !tbaa !10
  %10 = icmp eq i32 %1, 0
  br i1 %10, label %19, label %11

11:                                               ; preds = %3
  %12 = add i32 %4, -1536
  %13 = lshr exact i32 %12, 2
  %14 = add i32 %13, %0
  %15 = zext i32 %14 to i64
  %16 = getelementptr inbounds nuw float, ptr %2, i64 %15
  %17 = load float, ptr %16, align 4, !tbaa !10
  %18 = icmp ult i32 %1, 191
  br i1 %18, label %19, label %27

19:                                               ; preds = %3, %11
  %20 = phi float [ %17, %11 ], [ %9, %3 ]
  %21 = add nuw nsw i32 %4, 1536
  %22 = lshr exact i32 %21, 2
  %23 = add i32 %22, %0
  %24 = zext i32 %23 to i64
  %25 = getelementptr inbounds nuw float, ptr %2, i64 %24
  %26 = load float, ptr %25, align 4, !tbaa !10
  br label %27

27:                                               ; preds = %11, %19
  %28 = phi float [ %20, %19 ], [ %17, %11 ]
  %29 = phi float [ %26, %19 ], [ %9, %11 ]
  %30 = icmp eq i32 %0, 0
  br i1 %30, label %37, label %31

31:                                               ; preds = %27
  %32 = add i32 %6, -1
  %33 = zext i32 %32 to i64
  %34 = getelementptr inbounds nuw float, ptr %2, i64 %33
  %35 = load float, ptr %34, align 4, !tbaa !10
  %36 = icmp ult i32 %0, 383
  br i1 %36, label %37, label %43

37:                                               ; preds = %27, %31
  %38 = phi float [ %35, %31 ], [ %9, %27 ]
  %39 = add nuw i32 %6, 1
  %40 = zext i32 %39 to i64
  %41 = getelementptr inbounds nuw float, ptr %2, i64 %40
  %42 = load float, ptr %41, align 4, !tbaa !10
  br label %43

43:                                               ; preds = %31, %37
  %44 = phi float [ %38, %37 ], [ %35, %31 ]
  %45 = phi float [ %42, %37 ], [ %9, %31 ]
  %46 = fpext float %9 to double
  %47 = fadd float %28, %29
  %48 = fadd float %47, %44
  %49 = fadd float %48, %45
  %50 = fpext float %49 to double
  %51 = tail call double @llvm.fmuladd.f64(double %46, double -4.000000e+00, double %50)
  %52 = tail call double @llvm.fmuladd.f64(double %51, double 2.500000e-01, double %46)
  %53 = fptrunc double %52 to float
  ret float %53
}

; Function Attrs: nofree norecurse nosync nounwind sspstrong memory(argmem: readwrite) uwtable(sync)
define void @recalculateField(ptr noundef readonly captures(none) %0, ptr noundef writeonly captures(none) %1) local_unnamed_addr #4 {
  %3 = getelementptr inbounds nuw i8, ptr %0, i64 4
  br label %4

4:                                                ; preds = %2, %53
  %5 = phi i64 [ 0, %2 ], [ %69, %53 ]
  %6 = mul nuw nsw i64 %5, 1536
  %7 = lshr exact i64 %6, 2
  %8 = icmp eq i64 %5, 0
  %9 = mul nuw nsw i64 %5, 384
  %10 = add nuw i64 %6, 4294965760
  %11 = lshr exact i64 %10, 2
  %12 = icmp eq i64 %5, 191
  %13 = add nuw nsw i64 %6, 1536
  %14 = and i64 %11, 1073741696
  %15 = getelementptr inbounds nuw i8, ptr %0, i64 %6
  %16 = load float, ptr %15, align 4, !tbaa !10
  br i1 %8, label %20, label %17

17:                                               ; preds = %4
  %18 = getelementptr inbounds nuw float, ptr %0, i64 %14
  %19 = load float, ptr %18, align 4, !tbaa !10
  br i1 %12, label %24, label %20

20:                                               ; preds = %17, %4
  %21 = phi float [ %19, %17 ], [ %16, %4 ]
  %22 = getelementptr inbounds nuw i8, ptr %0, i64 %13
  %23 = load float, ptr %22, align 4, !tbaa !10
  br label %24

24:                                               ; preds = %17, %20
  %25 = phi float [ %21, %20 ], [ %19, %17 ]
  %26 = phi float [ %23, %20 ], [ %16, %17 ]
  %27 = getelementptr inbounds nuw i8, ptr %3, i64 %6
  %28 = load float, ptr %27, align 4, !tbaa !10
  %29 = fpext float %16 to double
  %30 = fadd float %25, %26
  %31 = fadd float %30, %16
  %32 = fadd float %31, %28
  %33 = fpext float %32 to double
  %34 = tail call double @llvm.fmuladd.f64(double %29, double -4.000000e+00, double %33)
  %35 = tail call double @llvm.fmuladd.f64(double %34, double 2.500000e-01, double %29)
  %36 = fptrunc double %35 to float
  %37 = getelementptr inbounds nuw float, ptr %1, i64 %9
  store float %36, ptr %37, align 4, !tbaa !10
  %38 = getelementptr inbounds nuw float, ptr %0, i64 %14
  %39 = getelementptr inbounds nuw i8, ptr %0, i64 %13
  %40 = getelementptr inbounds nuw float, ptr %1, i64 %9
  br label %71

41:                                               ; preds = %53
  ret void

42:                                               ; preds = %83
  %43 = add nuw nsw i64 %101, %7
  %44 = getelementptr inbounds nuw float, ptr %0, i64 %43
  %45 = load float, ptr %44, align 4, !tbaa !10
  br i1 %8, label %49, label %46

46:                                               ; preds = %42
  %47 = getelementptr inbounds nuw float, ptr %38, i64 %101
  %48 = load float, ptr %47, align 4, !tbaa !10
  br i1 %12, label %53, label %49

49:                                               ; preds = %46, %42
  %50 = phi float [ %48, %46 ], [ %45, %42 ]
  %51 = getelementptr inbounds nuw float, ptr %39, i64 %101
  %52 = load float, ptr %51, align 4, !tbaa !10
  br label %53

53:                                               ; preds = %49, %46
  %54 = phi float [ %50, %49 ], [ %48, %46 ]
  %55 = phi float [ %52, %49 ], [ %45, %46 ]
  %56 = add nuw nsw i64 %43, 4294967295
  %57 = and i64 %56, 4294967294
  %58 = getelementptr inbounds nuw float, ptr %0, i64 %57
  %59 = load float, ptr %58, align 4, !tbaa !10
  %60 = fpext float %45 to double
  %61 = fadd float %54, %55
  %62 = fadd float %61, %59
  %63 = fadd float %62, %45
  %64 = fpext float %63 to double
  %65 = tail call double @llvm.fmuladd.f64(double %60, double -4.000000e+00, double %64)
  %66 = tail call double @llvm.fmuladd.f64(double %65, double 2.500000e-01, double %60)
  %67 = fptrunc double %66 to float
  %68 = getelementptr inbounds nuw float, ptr %40, i64 %101
  store float %67, ptr %68, align 4, !tbaa !10
  %69 = add nuw nsw i64 %5, 1
  %70 = icmp eq i64 %69, 192
  br i1 %70, label %41, label %4, !llvm.loop !12

71:                                               ; preds = %24, %83
  %72 = phi i64 [ 1, %24 ], [ %101, %83 ]
  %73 = add nuw nsw i64 %72, %7
  %74 = getelementptr inbounds nuw float, ptr %0, i64 %73
  %75 = load float, ptr %74, align 4, !tbaa !10
  br i1 %8, label %79, label %76

76:                                               ; preds = %71
  %77 = getelementptr inbounds nuw float, ptr %38, i64 %72
  %78 = load float, ptr %77, align 4, !tbaa !10
  br i1 %12, label %83, label %79

79:                                               ; preds = %76, %71
  %80 = phi float [ %78, %76 ], [ %75, %71 ]
  %81 = getelementptr inbounds nuw float, ptr %39, i64 %72
  %82 = load float, ptr %81, align 4, !tbaa !10
  br label %83

83:                                               ; preds = %76, %79
  %84 = phi float [ %80, %79 ], [ %78, %76 ]
  %85 = phi float [ %82, %79 ], [ %75, %76 ]
  %86 = add nuw nsw i64 %73, 4294967295
  %87 = and i64 %86, 4294967295
  %88 = getelementptr inbounds nuw float, ptr %0, i64 %87
  %89 = load float, ptr %88, align 4, !tbaa !10
  %90 = getelementptr inbounds nuw float, ptr %3, i64 %73
  %91 = load float, ptr %90, align 4, !tbaa !10
  %92 = fpext float %75 to double
  %93 = fadd float %84, %85
  %94 = fadd float %93, %89
  %95 = fadd float %94, %91
  %96 = fpext float %95 to double
  %97 = tail call double @llvm.fmuladd.f64(double %92, double -4.000000e+00, double %96)
  %98 = tail call double @llvm.fmuladd.f64(double %97, double 2.500000e-01, double %92)
  %99 = fptrunc double %98 to float
  %100 = getelementptr inbounds nuw float, ptr %40, i64 %72
  store float %99, ptr %100, align 4, !tbaa !10
  %101 = add nuw nsw i64 %72, 1
  %102 = icmp eq i64 %101, 383
  br i1 %102, label %42, label %71, !llvm.loop !14
}

; Function Attrs: nounwind sspstrong uwtable(sync)
define void @drawField(ptr noundef readonly captures(none) %0) local_unnamed_addr #5 {
  br label %2

2:                                                ; preds = %1, %8
  %3 = phi i64 [ 0, %1 ], [ %9, %8 ]
  %4 = mul nuw nsw i64 %3, 1536
  %5 = getelementptr inbounds nuw i8, ptr %0, i64 %4
  %6 = trunc nuw nsw i64 %3 to i32
  br label %11

7:                                                ; preds = %8
  ret void

8:                                                ; preds = %33
  %9 = add nuw nsw i64 %3, 1
  %10 = icmp eq i64 %9, 192
  br i1 %10, label %7, label %2, !llvm.loop !16

11:                                               ; preds = %2, %33
  %12 = phi i64 [ 0, %2 ], [ %36, %33 ]
  %13 = getelementptr inbounds nuw float, ptr %5, i64 %12
  %14 = load float, ptr %13, align 4, !tbaa !10
  %15 = fcmp ult float %14, 1.000000e+02
  br i1 %15, label %16, label %33

16:                                               ; preds = %11
  %17 = fpext float %14 to double
  %18 = tail call double @llvm.minnum.f64(double %17, double 1.000000e+02)
  %19 = tail call double @llvm.maxnum.f64(double %18, double 0.000000e+00)
  %20 = fdiv double %19, 1.000000e+01
  %21 = fptrunc double %20 to float
  %22 = fptoui float %21 to i32
  %23 = zext i32 %22 to i64
  %24 = getelementptr inbounds nuw [10 x i32], ptr @colors, i64 0, i64 %23
  %25 = load i32, ptr %24, align 4, !tbaa !6
  %26 = add i32 %22, 1
  %27 = zext i32 %26 to i64
  %28 = getelementptr inbounds nuw [10 x i32], ptr @colors, i64 0, i64 %27
  %29 = load i32, ptr %28, align 4, !tbaa !6
  %30 = uitofp i32 %22 to float
  %31 = fsub float %21, %30
  %32 = tail call i32 @interpolate(i32 noundef %25, i32 noundef %29, float noundef %31)
  br label %33

33:                                               ; preds = %11, %16
  %34 = phi i32 [ %32, %16 ], [ 15955986, %11 ]
  %35 = trunc nuw nsw i64 %12 to i32
  tail call void @simPutPixel(i32 noundef %35, i32 noundef %6, i32 noundef %34) #10
  %36 = add nuw nsw i64 %12, 1
  %37 = icmp eq i64 %36, 384
  br i1 %37, label %8, label %11, !llvm.loop !17
}

declare void @simPutPixel(i32 noundef, i32 noundef, i32 noundef) local_unnamed_addr #6

; Function Attrs: nofree norecurse nosync nounwind sspstrong memory(argmem: readwrite) uwtable(sync)
define void @addObjectCircle(i32 noundef %0, ptr noundef captures(none) %1) local_unnamed_addr #4 {
  %3 = lshr i32 %0, 16
  %4 = and i32 %0, 65535
  br label %5

5:                                                ; preds = %2, %74
  %6 = phi i32 [ 30, %2 ], [ %20, %74 ]
  %7 = phi i32 [ 0, %2 ], [ %75, %74 ]
  %8 = add nuw i32 %7, %3
  %9 = sub nsw i32 %3, %7
  %10 = add nuw i32 %7, %4
  %11 = mul i32 %10, 1536
  %12 = lshr exact i32 %11, 2
  %13 = add nuw nsw i32 %3, %12
  %14 = sub nsw i32 %4, %7
  %15 = mul i32 %14, 1536
  %16 = lshr exact i32 %15, 2
  %17 = add nuw nsw i32 %3, %16
  %18 = mul i32 %7, %7
  br label %19

19:                                               ; preds = %5, %22
  %20 = phi i32 [ %70, %22 ], [ %6, %5 ]
  %21 = icmp sgt i32 %20, -1
  br i1 %21, label %22, label %74

22:                                               ; preds = %19
  %23 = add nuw i32 %20, %4
  %24 = mul i32 %23, 1536
  %25 = lshr exact i32 %24, 2
  %26 = add i32 %25, %8
  %27 = zext i32 %26 to i64
  %28 = getelementptr inbounds nuw float, ptr %1, i64 %27
  %29 = load float, ptr %28, align 4, !tbaa !10
  %30 = fadd float %29, 9.000000e+01
  store float %30, ptr %28, align 4, !tbaa !10
  %31 = sub nsw i32 %4, %20
  %32 = mul i32 %31, 1536
  %33 = lshr exact i32 %32, 2
  %34 = add i32 %33, %8
  %35 = zext i32 %34 to i64
  %36 = getelementptr inbounds nuw float, ptr %1, i64 %35
  %37 = load float, ptr %36, align 4, !tbaa !10
  %38 = fadd float %37, 9.000000e+01
  store float %38, ptr %36, align 4, !tbaa !10
  %39 = add i32 %25, %9
  %40 = zext i32 %39 to i64
  %41 = getelementptr inbounds nuw float, ptr %1, i64 %40
  %42 = load float, ptr %41, align 4, !tbaa !10
  %43 = fadd float %42, 9.000000e+01
  store float %43, ptr %41, align 4, !tbaa !10
  %44 = add i32 %33, %9
  %45 = zext i32 %44 to i64
  %46 = getelementptr inbounds nuw float, ptr %1, i64 %45
  %47 = load float, ptr %46, align 4, !tbaa !10
  %48 = fadd float %47, 9.000000e+01
  store float %48, ptr %46, align 4, !tbaa !10
  %49 = add nuw i32 %20, %13
  %50 = zext i32 %49 to i64
  %51 = getelementptr inbounds nuw float, ptr %1, i64 %50
  %52 = load float, ptr %51, align 4, !tbaa !10
  %53 = fadd float %52, 9.000000e+01
  store float %53, ptr %51, align 4, !tbaa !10
  %54 = add nuw i32 %20, %17
  %55 = zext i32 %54 to i64
  %56 = getelementptr inbounds nuw float, ptr %1, i64 %55
  %57 = load float, ptr %56, align 4, !tbaa !10
  %58 = fadd float %57, 9.000000e+01
  store float %58, ptr %56, align 4, !tbaa !10
  %59 = sub nsw i32 %3, %20
  %60 = add nsw i32 %59, %12
  %61 = zext i32 %60 to i64
  %62 = getelementptr inbounds nuw float, ptr %1, i64 %61
  %63 = load float, ptr %62, align 4, !tbaa !10
  %64 = fadd float %63, 9.000000e+01
  store float %64, ptr %62, align 4, !tbaa !10
  %65 = add nsw i32 %59, %16
  %66 = zext i32 %65 to i64
  %67 = getelementptr inbounds nuw float, ptr %1, i64 %66
  %68 = load float, ptr %67, align 4, !tbaa !10
  %69 = fadd float %68, 9.000000e+01
  store float %69, ptr %67, align 4, !tbaa !10
  %70 = add nsw i32 %20, -1
  %71 = mul i32 %70, %70
  %72 = add i32 %71, %18
  %73 = icmp slt i32 %72, 900
  br i1 %73, label %74, label %19, !llvm.loop !18

74:                                               ; preds = %19, %22
  %75 = add nuw nsw i32 %7, 1
  %76 = icmp slt i32 %75, %20
  br i1 %76, label %5, label %77, !llvm.loop !19

77:                                               ; preds = %74
  ret void
}

; Function Attrs: nofree norecurse nosync nounwind sspstrong memory(argmem: readwrite) uwtable(sync)
define void @addObjectRectangle(i32 noundef %0, ptr noundef captures(none) %1) local_unnamed_addr #4 {
  %3 = and i32 %0, 65535
  %4 = add nsw i32 %3, -30
  %5 = tail call i32 @llvm.umin.i32(i32 %3, i32 161)
  %6 = add nuw nsw i32 %5, 30
  %7 = icmp ugt i32 %4, %6
  br i1 %7, label %72, label %8

8:                                                ; preds = %2
  %9 = icmp ult i32 %0, 23199744
  %10 = lshr i32 %0, 16
  %11 = add nuw nsw i32 %10, 30
  %12 = select i1 %9, i32 %11, i32 383
  %13 = add nsw i32 %10, -30
  %14 = icmp ugt i32 %13, %12
  %15 = zext i32 %13 to i64
  %16 = add nuw nsw i32 %12, 1
  %17 = zext nneg i32 %4 to i64
  %18 = add nuw nsw i32 %5, 31
  %19 = add nuw nsw i32 %12, 30
  %20 = sub nsw i32 %19, %10
  %21 = zext i32 %20 to i64
  %22 = add nuw nsw i64 %21, 1
  %23 = icmp ult i32 %20, 3
  %24 = icmp ult i32 %20, 15
  %25 = and i64 %22, 8589934576
  %26 = icmp eq i64 %22, %25
  %27 = add nuw nsw i64 %25, %15
  %28 = and i64 %22, 12
  %29 = icmp eq i64 %28, 0
  %30 = and i64 %22, 8589934588
  %31 = add nuw nsw i64 %30, %15
  %32 = icmp eq i64 %22, %30
  br label %33

33:                                               ; preds = %8, %73
  %34 = phi i64 [ %17, %8 ], [ %74, %73 ]
  br i1 %14, label %73, label %35

35:                                               ; preds = %33
  %36 = mul nuw nsw i64 %34, 1536
  %37 = getelementptr inbounds nuw i8, ptr %1, i64 %36
  br i1 %23, label %38, label %40

38:                                               ; preds = %60, %71, %35
  %39 = phi i64 [ %15, %35 ], [ %27, %60 ], [ %31, %71 ]
  br label %77

40:                                               ; preds = %35
  br i1 %24, label %61, label %41

41:                                               ; preds = %40
  %42 = getelementptr float, ptr %37, i64 %15
  br label %43

43:                                               ; preds = %43, %41
  %44 = phi i64 [ 0, %41 ], [ %57, %43 ]
  %45 = getelementptr float, ptr %42, i64 %44
  %46 = getelementptr inbounds nuw i8, ptr %45, i64 16
  %47 = getelementptr inbounds nuw i8, ptr %45, i64 32
  %48 = getelementptr inbounds nuw i8, ptr %45, i64 48
  %49 = load <4 x float>, ptr %45, align 4, !tbaa !10
  %50 = load <4 x float>, ptr %46, align 4, !tbaa !10
  %51 = load <4 x float>, ptr %47, align 4, !tbaa !10
  %52 = load <4 x float>, ptr %48, align 4, !tbaa !10
  %53 = fadd <4 x float> %49, splat (float 9.000000e+01)
  %54 = fadd <4 x float> %50, splat (float 9.000000e+01)
  %55 = fadd <4 x float> %51, splat (float 9.000000e+01)
  %56 = fadd <4 x float> %52, splat (float 9.000000e+01)
  store <4 x float> %53, ptr %45, align 4, !tbaa !10
  store <4 x float> %54, ptr %46, align 4, !tbaa !10
  store <4 x float> %55, ptr %47, align 4, !tbaa !10
  store <4 x float> %56, ptr %48, align 4, !tbaa !10
  %57 = add nuw i64 %44, 16
  %58 = icmp eq i64 %57, %25
  br i1 %58, label %59, label %43, !llvm.loop !20

59:                                               ; preds = %43
  br i1 %26, label %73, label %60

60:                                               ; preds = %59
  br i1 %29, label %38, label %61

61:                                               ; preds = %60, %40
  %62 = phi i64 [ %25, %60 ], [ 0, %40 ]
  %63 = getelementptr float, ptr %37, i64 %15
  br label %64

64:                                               ; preds = %64, %61
  %65 = phi i64 [ %62, %61 ], [ %69, %64 ]
  %66 = getelementptr float, ptr %63, i64 %65
  %67 = load <4 x float>, ptr %66, align 4, !tbaa !10
  %68 = fadd <4 x float> %67, splat (float 9.000000e+01)
  store <4 x float> %68, ptr %66, align 4, !tbaa !10
  %69 = add nuw i64 %65, 4
  %70 = icmp eq i64 %69, %30
  br i1 %70, label %71, label %64, !llvm.loop !23

71:                                               ; preds = %64
  br i1 %32, label %73, label %38

72:                                               ; preds = %73, %2
  ret void

73:                                               ; preds = %77, %59, %71, %33
  %74 = add nuw nsw i64 %34, 1
  %75 = trunc i64 %74 to i32
  %76 = icmp eq i32 %18, %75
  br i1 %76, label %72, label %33, !llvm.loop !24

77:                                               ; preds = %38, %77
  %78 = phi i64 [ %82, %77 ], [ %39, %38 ]
  %79 = getelementptr inbounds nuw float, ptr %37, i64 %78
  %80 = load float, ptr %79, align 4, !tbaa !10
  %81 = fadd float %80, 9.000000e+01
  store float %81, ptr %79, align 4, !tbaa !10
  %82 = add nuw nsw i64 %78, 1
  %83 = trunc i64 %82 to i32
  %84 = icmp eq i32 %16, %83
  br i1 %84, label %73, label %77, !llvm.loop !25
}

; Function Attrs: noreturn nounwind sspstrong uwtable(sync)
define void @app() local_unnamed_addr #7 {
  %1 = alloca [73728 x float], align 4
  %2 = alloca [73728 x float], align 4
  call void @llvm.lifetime.start.p0(i64 294912, ptr nonnull %1) #10
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 4 dereferenceable(294912) %1, i8 0, i64 294912, i1 false)
  call void @llvm.lifetime.start.p0(i64 294912, ptr nonnull %2) #10
  call void @llvm.memset.p0.i64(ptr noundef nonnull align 4 dereferenceable(294912) %2, i8 0, i64 294912, i1 false)
  br label %3

3:                                                ; preds = %145, %0
  %4 = phi ptr [ %2, %0 ], [ %5, %145 ]
  %5 = phi ptr [ %1, %0 ], [ %4, %145 ]
  %6 = tail call i32 @simHasClick() #10
  %7 = icmp eq i32 %6, 0
  br i1 %7, label %12, label %8

8:                                                ; preds = %3, %8
  %9 = tail call i32 @simGetClick() #10
  call void @addObjectCircle(i32 noundef %9, ptr noundef nonnull %5)
  %10 = tail call i32 @simHasClick() #10
  %11 = icmp eq i32 %10, 0
  br i1 %11, label %12, label %8, !llvm.loop !26

12:                                               ; preds = %8, %3
  %13 = getelementptr inbounds nuw i8, ptr %5, i64 4
  %14 = getelementptr inbounds nuw i8, ptr %5, i64 1532
  br label %15

15:                                               ; preds = %60, %12
  %16 = phi i64 [ 0, %12 ], [ %76, %60 ]
  %17 = mul nuw nsw i64 %16, 1536
  %18 = lshr exact i64 %17, 2
  %19 = icmp eq i64 %16, 0
  %20 = add nuw nsw i64 %17, 4294965760
  %21 = lshr exact i64 %20, 2
  %22 = icmp eq i64 %16, 191
  %23 = add nuw nsw i64 %17, 1536
  %24 = and i64 %21, 1073741696
  %25 = getelementptr inbounds nuw i8, ptr %5, i64 %17
  %26 = load float, ptr %25, align 4, !tbaa !10
  br i1 %19, label %30, label %27

27:                                               ; preds = %15
  %28 = getelementptr inbounds nuw float, ptr %5, i64 %24
  %29 = load float, ptr %28, align 4, !tbaa !10
  br i1 %22, label %34, label %30

30:                                               ; preds = %27, %15
  %31 = phi float [ %29, %27 ], [ %26, %15 ]
  %32 = getelementptr inbounds nuw i8, ptr %5, i64 %23
  %33 = load float, ptr %32, align 4, !tbaa !10
  br label %34

34:                                               ; preds = %30, %27
  %35 = phi float [ %31, %30 ], [ %29, %27 ]
  %36 = phi float [ %33, %30 ], [ %26, %27 ]
  %37 = getelementptr inbounds nuw i8, ptr %13, i64 %17
  %38 = load float, ptr %37, align 4, !tbaa !10
  %39 = fpext float %26 to double
  %40 = fadd float %35, %36
  %41 = fadd float %26, %40
  %42 = fadd float %38, %41
  %43 = fpext float %42 to double
  %44 = tail call double @llvm.fmuladd.f64(double %39, double -4.000000e+00, double %43)
  %45 = tail call double @llvm.fmuladd.f64(double %44, double 2.500000e-01, double %39)
  %46 = fptrunc double %45 to float
  %47 = getelementptr inbounds nuw i8, ptr %4, i64 %17
  store float %46, ptr %47, align 4, !tbaa !10
  %48 = getelementptr inbounds nuw float, ptr %5, i64 %24
  %49 = getelementptr inbounds nuw i8, ptr %5, i64 %23
  br label %78

50:                                               ; preds = %90
  %51 = getelementptr inbounds nuw i8, ptr %14, i64 %17
  %52 = load float, ptr %51, align 4, !tbaa !10
  br i1 %19, label %56, label %53

53:                                               ; preds = %50
  %54 = getelementptr inbounds nuw i8, ptr %48, i64 1532
  %55 = load float, ptr %54, align 4, !tbaa !10
  br i1 %22, label %60, label %56

56:                                               ; preds = %53, %50
  %57 = phi float [ %55, %53 ], [ %52, %50 ]
  %58 = getelementptr inbounds nuw i8, ptr %49, i64 1532
  %59 = load float, ptr %58, align 4, !tbaa !10
  br label %60

60:                                               ; preds = %53, %56
  %61 = phi float [ %57, %56 ], [ %55, %53 ]
  %62 = phi float [ %59, %56 ], [ %52, %53 ]
  %63 = add nuw nsw i64 %18, 382
  %64 = and i64 %63, 4294967294
  %65 = getelementptr inbounds nuw float, ptr %5, i64 %64
  %66 = load float, ptr %65, align 4, !tbaa !10
  %67 = fpext float %52 to double
  %68 = fadd float %61, %62
  %69 = fadd float %68, %66
  %70 = fadd float %69, %52
  %71 = fpext float %70 to double
  %72 = tail call double @llvm.fmuladd.f64(double %67, double -4.000000e+00, double %71)
  %73 = tail call double @llvm.fmuladd.f64(double %72, double 2.500000e-01, double %67)
  %74 = fptrunc double %73 to float
  %75 = getelementptr inbounds nuw i8, ptr %47, i64 1532
  store float %74, ptr %75, align 4, !tbaa !10
  %76 = add nuw nsw i64 %16, 1
  %77 = icmp eq i64 %76, 192
  br i1 %77, label %110, label %15, !llvm.loop !12

78:                                               ; preds = %90, %34
  %79 = phi i64 [ 1, %34 ], [ %108, %90 ]
  %80 = add nuw nsw i64 %79, %18
  %81 = getelementptr inbounds nuw float, ptr %5, i64 %80
  %82 = load float, ptr %81, align 4, !tbaa !10
  br i1 %19, label %86, label %83

83:                                               ; preds = %78
  %84 = getelementptr inbounds nuw float, ptr %48, i64 %79
  %85 = load float, ptr %84, align 4, !tbaa !10
  br i1 %22, label %90, label %86

86:                                               ; preds = %83, %78
  %87 = phi float [ %85, %83 ], [ %82, %78 ]
  %88 = getelementptr inbounds nuw float, ptr %49, i64 %79
  %89 = load float, ptr %88, align 4, !tbaa !10
  br label %90

90:                                               ; preds = %86, %83
  %91 = phi float [ %87, %86 ], [ %85, %83 ]
  %92 = phi float [ %89, %86 ], [ %82, %83 ]
  %93 = add nuw nsw i64 %80, 4294967295
  %94 = and i64 %93, 4294967295
  %95 = getelementptr inbounds nuw float, ptr %5, i64 %94
  %96 = load float, ptr %95, align 4, !tbaa !10
  %97 = getelementptr inbounds nuw float, ptr %13, i64 %80
  %98 = load float, ptr %97, align 4, !tbaa !10
  %99 = fpext float %82 to double
  %100 = fadd float %91, %92
  %101 = fadd float %100, %96
  %102 = fadd float %101, %98
  %103 = fpext float %102 to double
  %104 = tail call double @llvm.fmuladd.f64(double %99, double -4.000000e+00, double %103)
  %105 = tail call double @llvm.fmuladd.f64(double %104, double 2.500000e-01, double %99)
  %106 = fptrunc double %105 to float
  %107 = getelementptr inbounds nuw float, ptr %47, i64 %79
  store float %106, ptr %107, align 4, !tbaa !10
  %108 = add nuw nsw i64 %79, 1
  %109 = icmp eq i64 %108, 383
  br i1 %109, label %50, label %78, !llvm.loop !27

110:                                              ; preds = %60, %115
  %111 = phi i64 [ %116, %115 ], [ 0, %60 ]
  %112 = mul nuw nsw i64 %111, 1536
  %113 = getelementptr inbounds nuw i8, ptr %4, i64 %112
  %114 = trunc nuw nsw i64 %111 to i32
  br label %118

115:                                              ; preds = %140
  %116 = add nuw nsw i64 %111, 1
  %117 = icmp eq i64 %116, 192
  br i1 %117, label %145, label %110, !llvm.loop !16

118:                                              ; preds = %140, %110
  %119 = phi i64 [ 0, %110 ], [ %143, %140 ]
  %120 = getelementptr inbounds nuw float, ptr %113, i64 %119
  %121 = load float, ptr %120, align 4, !tbaa !10
  %122 = fcmp ult float %121, 1.000000e+02
  br i1 %122, label %123, label %140

123:                                              ; preds = %118
  %124 = fpext float %121 to double
  %125 = tail call double @llvm.minnum.f64(double %124, double 1.000000e+02)
  %126 = tail call double @llvm.maxnum.f64(double %125, double 0.000000e+00)
  %127 = fdiv double %126, 1.000000e+01
  %128 = fptrunc double %127 to float
  %129 = fptoui float %128 to i32
  %130 = zext i32 %129 to i64
  %131 = getelementptr inbounds nuw [10 x i32], ptr @colors, i64 0, i64 %130
  %132 = load i32, ptr %131, align 4, !tbaa !6
  %133 = add i32 %129, 1
  %134 = zext i32 %133 to i64
  %135 = getelementptr inbounds nuw [10 x i32], ptr @colors, i64 0, i64 %134
  %136 = load i32, ptr %135, align 4, !tbaa !6
  %137 = uitofp i32 %129 to float
  %138 = fsub float %128, %137
  %139 = tail call i32 @interpolate(i32 noundef %132, i32 noundef %136, float noundef %138)
  br label %140

140:                                              ; preds = %123, %118
  %141 = phi i32 [ %139, %123 ], [ 15955986, %118 ]
  %142 = trunc nuw nsw i64 %119 to i32
  tail call void @simPutPixel(i32 noundef %142, i32 noundef %114, i32 noundef %141) #10
  %143 = add nuw nsw i64 %119, 1
  %144 = icmp eq i64 %143, 384
  br i1 %144, label %115, label %118, !llvm.loop !17

145:                                              ; preds = %115
  tail call void @simFlush() #10
  br label %3
}

; Function Attrs: mustprogress nocallback nofree nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr writeonly captures(none), i8, i64, i1 immarg) #8

declare i32 @simHasClick(...) local_unnamed_addr #6

declare i32 @simGetClick(...) local_unnamed_addr #6

declare void @simFlush(...) local_unnamed_addr #6

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare i32 @llvm.umin.i32(i32, i32) #9

attributes #0 = { mustprogress nofree norecurse nosync nounwind sspstrong willreturn memory(none) uwtable(sync) "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="4" "target-cpu"="apple-m1" "target-features"="+aes,+altnzcv,+ccdp,+ccidx,+ccpp,+complxnum,+crc,+dit,+dotprod,+flagm,+fp-armv8,+fp16fml,+fptoint,+fullfp16,+jsconv,+lse,+neon,+pauth,+perfmon,+predres,+ras,+rcpc,+rdm,+sb,+sha2,+sha3,+specrestrict,+ssbs,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8a" "zero-call-used-regs"="used-gpr" }
attributes #1 = { mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite) }
attributes #2 = { mustprogress nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #3 = { mustprogress nofree norecurse nosync nounwind sspstrong willreturn memory(argmem: read) uwtable(sync) "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="4" "target-cpu"="apple-m1" "target-features"="+aes,+altnzcv,+ccdp,+ccidx,+ccpp,+complxnum,+crc,+dit,+dotprod,+flagm,+fp-armv8,+fp16fml,+fptoint,+fullfp16,+jsconv,+lse,+neon,+pauth,+perfmon,+predres,+ras,+rcpc,+rdm,+sb,+sha2,+sha3,+specrestrict,+ssbs,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8a" "zero-call-used-regs"="used-gpr" }
attributes #4 = { nofree norecurse nosync nounwind sspstrong memory(argmem: readwrite) uwtable(sync) "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="4" "target-cpu"="apple-m1" "target-features"="+aes,+altnzcv,+ccdp,+ccidx,+ccpp,+complxnum,+crc,+dit,+dotprod,+flagm,+fp-armv8,+fp16fml,+fptoint,+fullfp16,+jsconv,+lse,+neon,+pauth,+perfmon,+predres,+ras,+rcpc,+rdm,+sb,+sha2,+sha3,+specrestrict,+ssbs,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8a" "zero-call-used-regs"="used-gpr" }
attributes #5 = { nounwind sspstrong uwtable(sync) "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="4" "target-cpu"="apple-m1" "target-features"="+aes,+altnzcv,+ccdp,+ccidx,+ccpp,+complxnum,+crc,+dit,+dotprod,+flagm,+fp-armv8,+fp16fml,+fptoint,+fullfp16,+jsconv,+lse,+neon,+pauth,+perfmon,+predres,+ras,+rcpc,+rdm,+sb,+sha2,+sha3,+specrestrict,+ssbs,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8a" "zero-call-used-regs"="used-gpr" }
attributes #6 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="4" "target-cpu"="apple-m1" "target-features"="+aes,+altnzcv,+ccdp,+ccidx,+ccpp,+complxnum,+crc,+dit,+dotprod,+flagm,+fp-armv8,+fp16fml,+fptoint,+fullfp16,+jsconv,+lse,+neon,+pauth,+perfmon,+predres,+ras,+rcpc,+rdm,+sb,+sha2,+sha3,+specrestrict,+ssbs,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8a" "zero-call-used-regs"="used-gpr" }
attributes #7 = { noreturn nounwind sspstrong uwtable(sync) "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="4" "target-cpu"="apple-m1" "target-features"="+aes,+altnzcv,+ccdp,+ccidx,+ccpp,+complxnum,+crc,+dit,+dotprod,+flagm,+fp-armv8,+fp16fml,+fptoint,+fullfp16,+jsconv,+lse,+neon,+pauth,+perfmon,+predres,+ras,+rcpc,+rdm,+sb,+sha2,+sha3,+specrestrict,+ssbs,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8a" "zero-call-used-regs"="used-gpr" }
attributes #8 = { mustprogress nocallback nofree nounwind willreturn memory(argmem: write) }
attributes #9 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
attributes #10 = { nounwind }

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
!10 = !{!11, !11, i64 0}
!11 = !{!"float", !8, i64 0}
!12 = distinct !{!12, !13}
!13 = !{!"llvm.loop.mustprogress"}
!14 = distinct !{!14, !13, !15}
!15 = !{!"llvm.loop.peeled.count", i32 2}
!16 = distinct !{!16, !13}
!17 = distinct !{!17, !13}
!18 = distinct !{!18, !13}
!19 = distinct !{!19, !13}
!20 = distinct !{!20, !13, !21, !22}
!21 = !{!"llvm.loop.isvectorized", i32 1}
!22 = !{!"llvm.loop.unroll.runtime.disable"}
!23 = distinct !{!23, !13, !21, !22}
!24 = distinct !{!24, !13}
!25 = distinct !{!25, !13, !22, !21}
!26 = distinct !{!26, !13}
!27 = distinct !{!27, !13, !15}
