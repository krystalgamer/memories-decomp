













































typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;
typedef signed long long s64;
typedef unsigned long long u64;









































typedef struct  {
	short	m[3][3];
        long     t[3];
} MATRIX;

typedef struct {
	long 	vx, vy;
	long 	vz, pad;
} VECTOR;

typedef struct {
	short	vx, vy;
	short	vz, pad;
} SVECTOR;

typedef struct {
	u8	r, g, b, cd;
} CVECTOR;

typedef struct {
	short vx, vy;
} DVECTOR;


typedef struct {
	SVECTOR v;
	VECTOR sxyz;
	DVECTOR sxy;
	CVECTOR rgb;
	short txuv,pad;
	long  chx,chy;
} EVECTOR;

typedef struct {
	SVECTOR v;
	u8 uv[2]; u16 pad;
	CVECTOR c;
	DVECTOR sxy;
	u32  sz;
} RVECTOR;


typedef struct {
	RVECTOR r01,r12,r20;
	RVECTOR	*   r0,*   r1,*   r2;
	u32 *   rtn;
} CRVECTOR3;

typedef struct {
	u32 	ndiv;
	u32 	pih,piv;
	u16 clut,tpage;
	CVECTOR	rgbc;
	u32	*   ot;
	RVECTOR r0,r1,r2;
	CRVECTOR3 cr[5];
} DIVPOLYGON3;

typedef struct {
	RVECTOR r01,r02,r31,r32,rc;
	RVECTOR	*   r0,*   r1,*   r2,*   r3;
	u32 *   rtn;
} CRVECTOR4;

typedef struct {
	u32 	ndiv;
	u32 	pih,piv;
	u16 clut,tpage;
	CVECTOR	rgbc;
	u32	*   ot;
	RVECTOR r0,r1,r2,r3;
	CRVECTOR4 cr[5];
} DIVPOLYGON4;

typedef struct {
        short   xy[3];
        short   uv[2];
        short   rgb[3];
} SPOL;


typedef struct {
        short   sxy[4][2];
        short   sz[4][2];
        short   uv[4][2];
        short   rgb[4][3];
        short   code;
} POL4;

typedef struct {
        short   sxy[3][2];
        short   sz[3][2];
        short   uv[3][2];
        short   rgb[3][3];
        short   code;
} POL3;

typedef struct {
        SVECTOR         *   v;
        SVECTOR         *   n;
        SVECTOR         *   u;
        CVECTOR         *   c;
        u32          len;
} TMESH;

typedef struct {
        SVECTOR         *   v;
        SVECTOR         *   n;
        SVECTOR         *   u;
        CVECTOR         *   c;
        u32          lenv;
        u32          lenh;
} QMESH;









extern void InitGeom();

extern void EigenMatrix(MATRIX *m, MATRIX *t);
extern int  IsIdMatrix(MATRIX *m);
extern MATRIX *MulMatrix0(MATRIX *m0,MATRIX *m1,MATRIX *m2);
extern MATRIX *MulRotMatrix0(MATRIX *m0,MATRIX *m1);
extern MATRIX *MulMatrix(MATRIX *m0,MATRIX *m1);
extern MATRIX *MulMatrix2(MATRIX *m0,MATRIX *m1);
extern MATRIX *MulRotMatrix(MATRIX *m0);
extern MATRIX *SetMulMatrix(MATRIX *m0,MATRIX *m1);
extern MATRIX *SetMulRotMatrix(MATRIX *m0);
extern VECTOR *ApplyMatrix(MATRIX *m,SVECTOR *v0,VECTOR *v1);
extern VECTOR *ApplyRotMatrix(SVECTOR *v0,VECTOR *v1);
extern VECTOR *ApplyRotMatrixLV(VECTOR *v0,VECTOR *v1);
extern VECTOR *ApplyMatrixLV(MATRIX *m,VECTOR *v0,VECTOR *v1);
extern SVECTOR *ApplyMatrixSV(MATRIX *m,SVECTOR *v0,SVECTOR *v1);
extern VECTOR *ApplyTransposeMatrixLV(MATRIX *m,VECTOR *v0,VECTOR *v1);
extern MATRIX *RotMatrix(SVECTOR *r,MATRIX *m);
extern MATRIX *RotMatrixXZY(SVECTOR *r,MATRIX *m);
extern MATRIX *RotMatrixYXZ(SVECTOR *r,MATRIX *m);
extern MATRIX *RotMatrixYZX(SVECTOR *r,MATRIX *m);
extern MATRIX *RotMatrixZXY(SVECTOR *r,MATRIX *m);
extern MATRIX *RotMatrixZYX(SVECTOR *r,MATRIX *m);
extern MATRIX *RotMatrix_gte(SVECTOR *r,MATRIX *m);
extern MATRIX *RotMatrixYXZ_gte(SVECTOR *r,MATRIX *m);
extern MATRIX *RotMatrixZYX_gte(SVECTOR *r,MATRIX *m);
extern MATRIX *RotMatrixX(long  r,MATRIX *m);
extern MATRIX *RotMatrixY(long  r,MATRIX *m);
extern MATRIX *RotMatrixZ(long  r,MATRIX *m);
extern MATRIX *RotMatrixC(SVECTOR *r,MATRIX *m);
extern MATRIX *TransMatrix(MATRIX *m,VECTOR *v);
extern MATRIX *ScaleMatrix(MATRIX *m,VECTOR *v);
extern MATRIX *ScaleMatrixL(MATRIX *m,VECTOR *v);
extern MATRIX *TransposeMatrix(MATRIX *m0,MATRIX *m1);
extern MATRIX *CompMatrix(MATRIX *m0,MATRIX *m1,MATRIX *m2);
extern MATRIX *CompMatrixLV(MATRIX *m0,MATRIX *m1,MATRIX *m2);

extern void MatrixNormal(MATRIX *m, MATRIX *n);
extern void MatrixNormal_0(MATRIX *m, MATRIX *n);
extern void MatrixNormal_1(MATRIX *m, MATRIX *n);
extern void MatrixNormal_2(MATRIX *m, MATRIX *n);

extern void SetRotMatrix(MATRIX *m);
extern void SetLightMatrix(MATRIX *m);
extern void SetColorMatrix(MATRIX *m);
extern void SetTransMatrix(MATRIX *m);
extern void PushMatrix();
extern void PopMatrix();
extern void ReadRotMatrix(MATRIX *m);
extern void ReadLightMatrix(MATRIX *m);
extern void ReadColorMatrix(MATRIX *m);
extern void SetRGBcd(CVECTOR *v);
extern void SetBackColor(long  rbk,long  gbk,long  bbk);
extern void SetFarColor(long  rfc,long  gfc,long  bfc);
extern void SetGeomOffset(long  ofx,long  ofy);
extern void SetGeomScreen(long  h);
extern void ReadSZfifo3(long  *sz0,long  *sz1,long  *sz2);
extern void ReadSZfifo4(long  *szx,long  *sz0,long  *sz1,long  *sz2);
extern void ReadSXSYfifo(long  *sxy0,long  *sxy1,long  *sxy2);
extern void ReadRGBfifo(CVECTOR *v0,CVECTOR *v1,CVECTOR *v2);
extern void ReadGeomOffset(long  *ofx,long  *ofy);
extern long  ReadGeomScreen();

extern void TransRot_32(VECTOR *v0, VECTOR *v1, long  *flag);
extern long  TransRotPers(SVECTOR *v0, long  *sxy, long  *p, long  *flag);
extern long  TransRotPers3(SVECTOR *v0, SVECTOR *v1, SVECTOR *v2, long  *sxy0,
		long  *sxy1, long  *sxy2, long  *p, long  *flag);

extern void pers_map(int abuf, SVECTOR **vertex, int tex[4][2], u16 *dtext);
extern void PhongLine(int istart_x, int iend_x, int p, int q, u16 **pixx,
		int fs, int ft, int i4, int det);

extern long  RotTransPers(SVECTOR *v0,long  *sxy,long  *p,long  *flag);
extern long  RotTransPers3(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,
			long  *sxy0,long  *sxy1,long  *sxy2,long  *p,long  *flag);
extern void RotTrans(SVECTOR *v0,VECTOR *v1,long  *flag);
extern void RotTransSV(SVECTOR *v0,SVECTOR *v1,long  *flag);
extern void LocalLight(SVECTOR *v0,VECTOR *v1);
extern void LightColor(VECTOR *v0,VECTOR *v1);
extern void DpqColorLight(VECTOR *v0,CVECTOR *v1,long  p,CVECTOR *v2);
extern void DpqColor(CVECTOR *v0,long  p,CVECTOR *v1);
extern void DpqColor3(CVECTOR *v0,CVECTOR *v1,CVECTOR *v2,
			long  p,CVECTOR *v3,CVECTOR *v4,CVECTOR *v5);
extern void Intpl(VECTOR *v0,long  p,CVECTOR *v1);
extern VECTOR *Square12(VECTOR *v0,VECTOR *v1);
extern VECTOR *Square0(VECTOR *v0,VECTOR *v1);
extern VECTOR *SquareSL12(SVECTOR *v0,VECTOR *v1);
extern VECTOR *SquareSL0(SVECTOR *v0,VECTOR *v1);
extern SVECTOR *SquareSS12(SVECTOR *v0,SVECTOR *v1);
extern SVECTOR *SquareSS0(SVECTOR *v0,SVECTOR *v1);
extern void NormalColor(SVECTOR *v0,CVECTOR *v1);
extern void NormalColor3(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,
			CVECTOR *v3,CVECTOR *v4,CVECTOR *v5);
extern void NormalColorDpq(SVECTOR *v0,CVECTOR *v1,long  p,CVECTOR *v2);
extern void NormalColorDpq3(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,CVECTOR *v3,
			long  p,CVECTOR *v4,CVECTOR *v5,CVECTOR *v6);
extern void NormalColorCol(SVECTOR *v0,CVECTOR *v1,CVECTOR *v2);
extern void NormalColorCol3(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,CVECTOR *v3,
			CVECTOR *v4,CVECTOR *v5,CVECTOR *v6);
extern void ColorDpq(VECTOR *v0,CVECTOR *v1,long  p,CVECTOR *v2);
extern void ColorCol(VECTOR *v0,CVECTOR *v1,CVECTOR *v2);
extern long  NormalClip(long  sxy0,long  sxy1,long  sxy2);
extern long  AverageZ3(long  sz0,long  sz1,long  sz2);
extern long  AverageZ4(long  sz0,long  sz1,long  sz2,long  sz3);
extern void OuterProduct12(VECTOR *v0,VECTOR *v1,VECTOR *v2);
extern void OuterProduct0(VECTOR *v0,VECTOR *v1,VECTOR *v2);
extern long  Lzc(long  data);


extern long  RotTransPers4(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,SVECTOR *v3,
			long  *sxy0,long  *sxy1,long  *sxy2,long  *sxy3,
			long  *p,long  *flag);
extern void RotTransPersN(SVECTOR *v0,DVECTOR *v1,u16 *sz,u16 *p,
			u16 *flag,long  n);
extern void RotTransPers3N(SVECTOR *v0,DVECTOR *v1,u16 *sz,u16 *flag,
			long  n);
extern void RotMeshH(short *Yheight,DVECTOR *Vo,u16 *sz,u16 *flag,
			short Xoffset,short Zoffset,short m,short n,
			DVECTOR *base);
extern long  RotAverage3(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,
			long  *sxy0,long  *sxy1,long  *sxy2,long  *p,long  *flag);
extern long  RotAverage4(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,SVECTOR *v3,
			long  *sxy0,long  *sxy1,long  *sxy2,long  *sxy3,
			long  *p,long  *flag);
extern long  RotNclip3(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,
			long  *sxy0,long  *sxy1,long  *sxy2,long  *p,long  *otz,
			long  *flag);
extern long  RotNclip4(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,SVECTOR *v3,
			long  *sxy0,long  *sxy1,long  *sxy2,long  *sxy3,
			long  *p,long  *otz,long  *flag);
extern long  RotAverageNclip3(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,
			long  *sxy0,long  *sxy1,long  *sxy2,
			long  *p,long  *otz,long  *flag);
extern long  RotAverageNclip4(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,SVECTOR *v3,
			long  *sxy0,long  *sxy1,long  *sxy2,long  *sxy3,
			long  *p,long  *otz,long  *flag);
extern long  RotColorDpq(SVECTOR *v0,SVECTOR *v1,CVECTOR *v2,
			long  *sxy,CVECTOR *v3,long  *flag);
extern long  RotColorDpq3(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,
			SVECTOR *v3,SVECTOR *v4,SVECTOR *v5,CVECTOR *v6,
			long  *sxy0,long  *sxy1,long  *sxy2,
			CVECTOR *v7,CVECTOR *v8,CVECTOR *v9,long  *flag);
extern long  RotAverageNclipColorDpq3(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,
			SVECTOR *v3,SVECTOR *v4,SVECTOR *v5,CVECTOR *v6,
			long  *sxy0,long  *sxy1,long  *sxy2,
			CVECTOR *v7,CVECTOR *v8,CVECTOR *v9,
			long  *otz,long  *flag);
extern long  RotAverageNclipColorCol3(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,
			SVECTOR *v3,SVECTOR *v4,SVECTOR *v5,CVECTOR *v6,
			long  *sxy0,long  *sxy1,long  *sxy2,
			CVECTOR *v7,CVECTOR *v8,CVECTOR *v9,
			long  *otz,long  *flag);
extern long  RotColorMatDpq(SVECTOR *v0,SVECTOR *v1,CVECTOR *v2,long  *sxy,
			CVECTOR *v3,long  matc,long  flag);
extern void ColorMatDpq(SVECTOR *v0,CVECTOR *v1,long  p,CVECTOR *v2,long  matc);
extern void ColorMatCol(SVECTOR *v0,CVECTOR *v1,CVECTOR *v2,long  matc);
extern void LoadAverage12(VECTOR *v0,VECTOR *v1,long  p0,long  p1,VECTOR *v2);
extern void LoadAverageShort12(SVECTOR *v0,SVECTOR *v1,long  p0,long  p1,
			SVECTOR *v2);
extern void LoadAverage0(VECTOR *v0,VECTOR *v1,long  p0,long  p1,VECTOR *v2);
extern void LoadAverageShort0(SVECTOR *v0,SVECTOR *v1,long  p0,long  p1,
			SVECTOR *v2);
extern void LoadAverageByte(u8 *v0,u8 *v1,long  p0,long  p1,u8 *v2);
extern void LoadAverageCol(u8 *v0,u8 *v1,long  p0,long  p1,u8 *v2);
extern long  VectorNormal(VECTOR *v0, VECTOR *v1);
extern long  VectorNormalS(VECTOR *v0, SVECTOR *v1);
extern long  VectorNormalSS(SVECTOR *v0, SVECTOR *v1);
extern long  SquareRoot0(long  a);
extern long  SquareRoot12(long  a);
extern void InvSquareRoot(long  a, long  *b, long  *c);
extern void gteMIMefunc(SVECTOR *otp, SVECTOR *dfp, long  n, long  p);
extern void SetFogFar(long  a,long  h);
extern void SetFogNear(long  a,long  h);
extern void SetFogNearFar(long  a,long  b,long  h);
extern void SubPol4(POL4 *p, SPOL *sp, int ndiv);
extern void SubPol3(POL3 *p, SPOL *sp, int ndiv);

extern int rcos(int a);
extern int rsin(int a);
extern int ccos(int a);
extern int csin(int a);
extern int cln(int a);
extern int csqrt(int a);
extern int catan(int a);
extern long  ratan2(long  y, long  x);


extern void RotPMD_F3(long  *pa,u32 *ot,int otlen,int id,int backc);
extern void RotPMD_G3(long  *pa,u32 *ot,int otlen,int id,int backc);
extern void RotPMD_FT3(long  *pa,u32 *ot,int otlen,int id,int backc);
extern void RotPMD_GT3(long  *pa,u32 *ot,int otlen,int id,int backc);
extern void RotPMD_F4(long  *pa,u32 *ot,int otlen,int id,int backc);
extern void RotPMD_G4(long  *pa,u32 *ot,int otlen,int id,int backc);
extern void RotPMD_FT4(long  *pa,u32 *ot,int otlen,int id,int backc);
extern void RotPMD_GT4(long  *pa,u32 *ot,int otlen,int id,int backc);

extern void RotPMD_SV_F3(long  *pa,long  *va,u32 *ot,int otlen,int id,
			int backc);
extern void RotPMD_SV_G3(long  *pa,long  *va,u32 *ot,int otlen,int id,
			int backc);
extern void RotPMD_SV_FT3(long  *pa,long  *va,u32 *ot,int otlen,int id,
			int backc);
extern void RotPMD_SV_GT3(long  *pa,long  *va,u32 *ot,int otlen,int id,
			int backc);
extern void RotPMD_SV_F4(long  *pa,long  *va,u32 *ot,int otlen,int id,
			int backc);
extern void RotPMD_SV_G4(long  *pa,long  *va,u32 *ot,int otlen,int id,
			int backc);
extern void RotPMD_SV_FT4(long  *pa,long  *va,u32 *ot,int otlen,int id,
			int backc);
extern void RotPMD_SV_GT4(long  *pa,long  *va,u32 *ot,int otlen,int id,
			int backc);


extern void InitClip(EVECTOR *evbfad,long  hw,long  vw,long  h,long  near,long  far);
extern long  Clip3F(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,EVECTOR **evmx);
extern long  Clip3FP(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,EVECTOR **evmx);
extern long  Clip4F(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,SVECTOR *v3,
			EVECTOR **evmx);
extern long  Clip4FP(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,SVECTOR *v3,
			EVECTOR **evmx);
extern long  Clip3FT(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,
		short *uv0,short *uv1,short *uv2,EVECTOR **evmx);
extern long  Clip3FTP(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,
		short *uv0,short *uv1,short *uv2,EVECTOR **evmx);
extern long  Clip4FT(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,SVECTOR *v3,
		short *uv0,short *uv1,short *uv2,short *uv3,EVECTOR **evmx);
extern long  Clip4FTP(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,SVECTOR *v3,
		short *uv0,short *uv1,short *uv2,short *uv3,EVECTOR **evmx);
extern long  Clip3G(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,
		CVECTOR *rgb0,CVECTOR *rgb1,CVECTOR *rgb2,EVECTOR **evmx);
extern long  Clip3GP(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,
		CVECTOR *rgb0,CVECTOR *rgb1,CVECTOR *rgb2,EVECTOR **evmx);
extern long  Clip4G(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,SVECTOR *v3,
		CVECTOR *rgb0,CVECTOR *rgb1,CVECTOR *rgb2,CVECTOR *rgb3,
		EVECTOR **evmx);
extern long  Clip4GP(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,SVECTOR *v3,
		CVECTOR *rgb0,CVECTOR *rgb1,CVECTOR *rgb2,CVECTOR *rgb3,
		EVECTOR **evmx);
extern long  Clip3GT(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,
		short *uv0,short *uv1,short *uv2,
		CVECTOR *rgb0,CVECTOR *rgb1,CVECTOR *rgb2,
		EVECTOR **evmx);
extern long  Clip3GTP(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,
		short *uv0,short *uv1,short *uv2,
		CVECTOR *rgb0,CVECTOR *rgb1,CVECTOR *rgb2,
		EVECTOR **evmx);
extern long  Clip4GT(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,SVECTOR *v3,
		short *uv0,short *uv1,short *uv2,short *uv3,
		CVECTOR *rgb0,CVECTOR *rgb1,CVECTOR *rgb2,CVECTOR *rgb3,
		EVECTOR **evmx);
extern long  Clip4GTP(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,SVECTOR *v3,
		short *uv0,short *uv1,short *uv2,short *uv3,
		CVECTOR *rgb0,CVECTOR *rgb1,CVECTOR *rgb2,CVECTOR *rgb3,
		EVECTOR **evmx);

extern void RotTransPers_nom(SVECTOR *v0);
extern void RotTransPers3_nom(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2);
extern void RotTransPers4_nom(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,SVECTOR *v3);
extern void RotTrans_nom(SVECTOR *v0);
extern void RotAverage3_nom(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2);
extern void RotNclip3_nom(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2);
extern void RotAverageNclip3_nom(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2);
extern void RotAverageNclipColorDpq3_nom(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,
			SVECTOR *v3,SVECTOR *v4,SVECTOR *v5,CVECTOR *v6);
extern void RotAverageNclipColorCol3_nom(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,
			SVECTOR *v3,SVECTOR *v4,SVECTOR *v5,CVECTOR *v6);
extern void RotColorDpq_nom(SVECTOR *v0,SVECTOR *v1,CVECTOR *v2);
extern long  RotColorDpq3_nom(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,
			SVECTOR *v3,SVECTOR *v4,SVECTOR *v5,CVECTOR *v6);
extern void NormalColor_nom(SVECTOR *v0);
extern void NormalColor3_nom(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2);
extern void NormalColorDpq_nom(SVECTOR *v0,CVECTOR *v1,long  p);
extern void NormalColorDpq3_nom(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,
			CVECTOR *v3,long  p);
extern void NormalColorCol_nom(SVECTOR *v0,CVECTOR *v1);
extern void NormalColorCol3_nom(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,
			CVECTOR *v3);







































































































extern void RotSMD_F3(long  *pa,u32 *ot,int otlen,int id,
			int sclip, int hclip, int vclip, int nclipmode);
extern void RotSMD_G3(long  *pa,u32 *ot,int otlen,int id,
			int sclip, int hclip, int vclip, int nclipmode);
extern void RotSMD_FT3(long  *pa,u32 *ot,int otlen,int id,
			int sclip, int hclip, int vclip, int nclipmode);
extern void RotSMD_GT3(long  *pa,u32 *ot,int otlen,int id,
			int sclip, int hclip, int vclip, int nclipmode);
extern void RotSMD_F4(long  *pa,u32 *ot,int otlen,int id,
			int sclip, int hclip, int vclip, int nclipmode);
extern void RotSMD_G4(long  *pa,u32 *ot,int otlen,int id,
			int sclip, int hclip, int vclip, int nclipmode);
extern void RotSMD_FT4(long  *pa,u32 *ot,int otlen,int id,
			int sclip, int hclip, int vclip, int nclipmode);
extern void RotSMD_GT4(long  *pa,u32 *ot,int otlen,int id,
			int sclip, int hclip, int vclip, int nclipmode);

extern void RotSMD_SV_F3(long  *pa,long  *va,u32 *ot,int otlen,int id,
			int sclip, int hclip, int vclip, int nclipmode);
extern void RotSMD_SV_G3(long  *pa,long  *va,u32 *ot,int otlen,int id,
			int sclip, int hclip, int vclip, int nclipmode);
extern void RotSMD_SV_FT3(long  *pa,long  *va,u32 *ot,int otlen,int id,
			int sclip, int hclip, int vclip, int nclipmode);
extern void RotSMD_SV_GT3(long  *pa,long  *va,u32 *ot,int otlen,int id,
			int sclip, int hclip, int vclip, int nclipmode);
extern void RotSMD_SV_F4(long  *pa,long  *va,u32 *ot,int otlen,int id,
			int sclip, int hclip, int vclip, int nclipmode);
extern void RotSMD_SV_G4(long  *pa,long  *va,u32 *ot,int otlen,int id,
			int sclip, int hclip, int vclip, int nclipmode);
extern void RotSMD_SV_FT4(long  *pa,long  *va,u32 *ot,int otlen,int id,
			int sclip, int hclip, int vclip, int nclipmode);
extern void RotSMD_SV_GT4(long  *pa,long  *va,u32 *ot,int otlen,int id,
			int sclip, int hclip, int vclip, int nclipmode);



extern void RotRMD_F3(long  *pa,u32 *ot,int otlen,int id,
			int sclip, int hclip, int vclip, int nclipmode);
extern void RotRMD_G3(long  *pa,u32 *ot,int otlen,int id,
			int sclip, int hclip, int vclip, int nclipmode);
extern void RotRMD_FT3(long  *pa,u32 *ot,int otlen,int id,
			int sclip, int hclip, int vclip, int nclipmode);
extern void RotRMD_GT3(long  *pa,u32 *ot,int otlen,int id,
			int sclip, int hclip, int vclip, int nclipmode);
extern void RotRMD_F4(long  *pa,u32 *ot,int otlen,int id,
			int sclip, int hclip, int vclip, int nclipmode);
extern void RotRMD_G4(long  *pa,u32 *ot,int otlen,int id,
			int sclip, int hclip, int vclip, int nclipmode);
extern void RotRMD_FT4(long  *pa,u32 *ot,int otlen,int id,
			int sclip, int hclip, int vclip, int nclipmode);
extern void RotRMD_GT4(long  *pa,u32 *ot,int otlen,int id,
			int sclip, int hclip, int vclip, int nclipmode);

extern void RotRMD_SV_F3(long  *pa,long  *va,u32 *ot,int otlen,int id,
			int sclip, int hclip, int vclip, int nclipmode);
extern void RotRMD_SV_G3(long  *pa,long  *va,u32 *ot,int otlen,int id,
			int sclip, int hclip, int vclip, int nclipmode);
extern void RotRMD_SV_FT3(long  *pa,long  *va,u32 *ot,int otlen,int id,
			int sclip, int hclip, int vclip, int nclipmode);
extern void RotRMD_SV_GT3(long  *pa,long  *va,u32 *ot,int otlen,int id,
			int sclip, int hclip, int vclip, int nclipmode);
extern void RotRMD_SV_F4(long  *pa,long  *va,u32 *ot,int otlen,int id,
			int sclip, int hclip, int vclip, int nclipmode);
extern void RotRMD_SV_G4(long  *pa,long  *va,u32 *ot,int otlen,int id,
			int sclip, int hclip, int vclip, int nclipmode);
extern void RotRMD_SV_FT4(long  *pa,long  *va,u32 *ot,int otlen,int id,
			int sclip, int hclip, int vclip, int nclipmode);
extern void RotRMD_SV_GT4(long  *pa,long  *va,u32 *ot,int otlen,int id,
			int sclip, int hclip, int vclip, int nclipmode);

extern long  p2otz(long  p, long  projection);
extern long  otz2p(long  otz, long  projection);























































































































extern	int (*   GPU_printf)(char *fmt, ...);




















































































































































































typedef struct {
	short x, y;
	short w, h;
} RECT;

typedef struct {
	int x, y;
	int w, h;
} RECT32;




typedef struct {
	u32	tag;
	u32	code[15];
} DR_ENV;

typedef struct {
	RECT	clip;
	short	ofs[2];
	RECT	tw;
	u16 tpage;
	u8	dtd;
	u8	dfe;
	u8	isbg;
	u8	r0, g0, b0;
	DR_ENV	dr_env;
} DRAWENV;

typedef struct {
	RECT	disp;
	RECT	screen;
	u8	isinter;
	u8	isrgb24;
	u8	pad0, pad1;
} DISPENV;




typedef struct {
	unsigned	addr: 24;
	unsigned 	len:   8;
	u8		r0, g0, b0, code;
} P_TAG;

typedef struct {
	u8	r0, g0, b0, code;
} P_CODE;

typedef struct {
	u32	tag;
	u8	r0, g0, b0, code;
	short	x0, 	y0;
	short	x1,	y1;
	short	x2,	y2;
} POLY_F3;

typedef struct {
	u32	tag;
	u8	r0, g0, b0, code;
	short	x0, 	y0;
	short	x1,	y1;
	short	x2,	y2;
	short	x3,	y3;
} POLY_F4;

typedef struct {
	u32	tag;
	u8	r0, g0, b0, code;
	short	x0, 	y0;
	u8	u0, v0;	u16	clut;
	short	x1,	y1;
	u8	u1, v1;	u16	tpage;
	short	x2,	y2;
	u8	u2, v2;	u16	pad1;
} POLY_FT3;

typedef struct {
	u32	tag;
	u8	r0, g0, b0, code;
	short	x0, 	y0;
	u8	u0, v0;	u16	clut;
	short	x1,	y1;
	u8	u1, v1;	u16	tpage;
	short	x2,	y2;
	u8	u2, v2;	u16	pad1;
	short	x3,	y3;
	u8	u3, v3;	u16	pad2;
} POLY_FT4;

typedef struct {
	u32	tag;
	u8	r0, g0, b0, code;
	short	x0, 	y0;
	u8	r1, g1, b1, pad1;
	short	x1,	y1;
	u8	r2, g2, b2, pad2;
	short	x2,	y2;
} POLY_G3;

typedef struct {
	u32	tag;
	u8	r0, g0, b0, code;
	short	x0, 	y0;
	u8	r1, g1, b1, pad1;
	short	x1,	y1;
	u8	r2, g2, b2, pad2;
	short	x2,	y2;
	u8	r3, g3, b3, pad3;
	short	x3,	y3;
} POLY_G4;

typedef struct {
	u32	tag;
	u8	r0, g0, b0, code;
	short	x0, 	y0;
	u8	u0, v0;	u16	clut;
	u8	r1, g1, b1, p1;
	short	x1,	y1;
	u8	u1, v1;	u16	tpage;
	u8	r2, g2, b2, p2;
	short	x2,	y2;
	u8	u2, v2;	u16	pad2;
} POLY_GT3;

typedef struct {
	u32	tag;
	u8	r0, g0, b0, code;
	short	x0, 	y0;
	u8	u0, v0;	u16	clut;
	u8	r1, g1, b1, p1;
	short	x1,	y1;
	u8	u1, v1;	u16	tpage;
	u8	r2, g2, b2, p2;
	short	x2,	y2;
	u8	u2, v2;	u16	pad2;
	u8	r3, g3, b3, p3;
	short	x3,	y3;
	u8	u3, v3;	u16	pad3;
} POLY_GT4;




typedef struct {
	u32	tag;
	u8	r0, g0, b0, code;
	short	x0, 	y0;
	short	x1,	y1;
} LINE_F2;

typedef struct {
	u32	tag;
	u8	r0, g0, b0, code;
	short	x0, 	y0;
	u8	r1, g1, b1, p1;
	short	x1,	y1;
} LINE_G2;

typedef struct {
	u32	tag;
	u8	r0, g0, b0, code;
	short	x0, 	y0;
	short	x1,	y1;
	short	x2,	y2;
	u32	pad;
} LINE_F3;

typedef struct {
	u32	tag;
	u8	r0, g0, b0, code;
	short	x0, 	y0;
	u8	r1, g1, b1, p1;
	short	x1,	y1;
	u8	r2, g2, b2, p2;
	short	x2,	y2;
	u32	pad;
} LINE_G3;

typedef struct {
	u32	tag;
	u8	r0, g0, b0, code;
	short	x0, 	y0;
	short	x1,	y1;
	short	x2,	y2;
	short	x3,	y3;
	u32	pad;
} LINE_F4;

typedef struct {
	u32	tag;
	u8	r0, g0, b0, code;
	short	x0, 	y0;
	u8	r1, g1, b1, p1;
	short	x1,	y1;
	u8	r2, g2, b2, p2;
	short	x2,	y2;
	u8	r3, g3, b3, p3;
	short	x3,	y3;
	u32	pad;
} LINE_G4;




typedef struct {
	u32	tag;
	u8	r0, g0, b0, code;
	short	x0, 	y0;
	u8	u0, v0;	u16	clut;
	short	w,	h;
} SPRT;

typedef struct {
	u32	tag;
	u8	r0, g0, b0, code;
	short	x0, 	y0;
	u8	u0, v0;	u16	clut;
} SPRT_16;

typedef struct {
	u32	tag;
	u8	r0, g0, b0, code;
	short	x0, 	y0;
	u8	u0, v0;	u16	clut;
} SPRT_8;




typedef struct {
	u32	tag;
	u8	r0, g0, b0, code;
	short	x0, 	y0;
	short	w,	h;
} TILE;

typedef struct {
	u32	tag;
	u8	r0, g0, b0, code;
	short	x0, 	y0;
} TILE_16;

typedef struct {
	u32	tag;
	u8	r0, g0, b0, code;
	short	x0, 	y0;
} TILE_8;

typedef struct {
	u32	tag;
	u8	r0, g0, b0, code;
	short	x0, 	y0;
} TILE_1;




typedef struct {
	u32	tag;
	u32	code[2];
} DR_MODE;

typedef struct {
	u32	tag;
	u32	code[2];
} DR_TWIN;

typedef struct {
	u32	tag;
	u32	code[2];
} DR_AREA;

typedef struct {
	u32	tag;
	u32	code[2];
} DR_OFFSET;

typedef struct {
	u32	tag;
	u32	code[5];
} DR_MOVE;

typedef struct {
	u32	tag;
	u32	code[3];
	u32	p[13];
} DR_LOAD;

typedef	struct {
	u32	tag;
	u32	code[1];
} DR_TPAGE;

typedef struct {
        u32  tag;
        u32  code[2];
} DR_STP;










typedef struct {
	u32	id;
	u8	r0, g0, b0, p0;
	u8	r1, g1, b1, p1;
	u8	r2, g2, b2, p2;
	u8	r3, g3, b3, p3;
	u16	tpage, clut;
	u8	u0, v0, u1, v1;
	u8	u2, v2, u3, v3;


	SVECTOR	x0, x1, x2, x3;
	SVECTOR	n0, n1, n2, n3;


	SVECTOR	*   v_ofs;
	SVECTOR	*   n_ofs;

	u16	vert0, vert1;
	u16	vert2, vert3;
	u16	norm0, norm1;
	u16	norm2, norm3;


} TMD_PRIM;




typedef struct {
	u32  mode;
	RECT	*   crect;
	u32	*   caddr;
	RECT	*   prect;
	u32	*   paddr;
} TIM_IMAGE;







extern int FntPrint();



extern int KanjiFntPrint();



extern DISPENV *GetDispEnv(DISPENV *env);
extern DISPENV *PutDispEnv(DISPENV *env);
extern DISPENV *SetDefDispEnv(DISPENV *env, int x, int y, int w, int h);
extern DRAWENV *GetDrawEnv(DRAWENV *env);
extern DRAWENV *PutDrawEnv(DRAWENV *env);
extern DRAWENV *SetDefDrawEnv(DRAWENV *env, int x, int y, int w, int h);
extern TIM_IMAGE *ReadTIM(TIM_IMAGE *timimg);
extern TMD_PRIM *ReadTMD(TMD_PRIM *tmdprim);
extern int CheckPrim(char *s, u32 *p);
extern int ClearImage(RECT *rect, u8 r, u8 g, u8 b);
extern int ClearImage2(RECT *rect, u8 r, u8 g, u8 b);
extern int DrawSync(int mode);
extern int FntOpen(int x, int y, int w, int h, int isbg, int n);
extern int GetGraphDebug(void) ;
extern int GetTimSize(u8 *sjis);
extern int IsEndPrim(void *p) ;
extern int KanjiFntOpen(int x, int y, int w, int h, int dx, int dy, int cx, int cy, int isbg, int n);
extern void KanjiFntClose(void);
extern int Krom2Tim(u8 *sjis, u32 *taddr, int dx, int dy, int cdx, int cdy, u32 fg, u32 bg);
extern int LoadImage(RECT *rect, u32 *p);
extern int MargePrim(void *p0, void *p1);
extern int MoveImage(RECT *rect, int x, int y);
extern int OpenTIM(u32 *addr);
extern int OpenTMD(u32 *tmd, int obj_no);
extern int ResetGraph(int mode);
extern int SetGraphDebug(int level);
extern int StoreImage(RECT *rect, u32 *p);
extern u32 *ClearOTag(u32 *ot, int n);
extern u32 *ClearOTagR(u32 *ot, int n);
extern u32 *FntFlush(int id);
extern u32 *KanjiFntFlush(int id);
extern u32 DrawSyncCallback(void (*func)(void));
extern u16 GetClut(int x, int y) ;
extern u16 GetTPage(int tp, int abr, int x, int y) ;
extern u16 LoadClut(u32 *clut, int x, int y);
extern u16 LoadClut2(u32 *clut, int x, int y);
extern u16 LoadTPage(u32 *pix, int tp, int abr, int x, int y, int w, int h);
extern void *NextPrim(void *p) ;
extern void AddPrim(void *ot, void *p) ;
extern void AddPrims(void *ot, void *p0, void *p1) ;
extern void CatPrim(void *p0, void *p1) ;
extern void DrawOTag(u32 *p);
extern void DrawOTagIO(u32 *p);
extern void DrawOTagEnv(u32 *p, DRAWENV *env);
extern void DrawPrim(void *p);
extern void DumpClut(u16 clut) ;
extern void DumpDispEnv(DISPENV *env);
extern void DumpDrawEnv(DRAWENV *env);
extern void DumpOTag(u32 *p);
extern void DumpTPage(u16 tpage) ;
extern void FntLoad(int tx, int ty);
extern void SetDispMask(int mask);
extern void SetDrawArea(DR_AREA *p, RECT *r);
extern void SetDrawEnv(DR_ENV *dr_env, DRAWENV *env);
extern void SetDrawLoad(DR_LOAD *p, RECT *rect);
extern void SetDrawMode(DR_MODE *p, int dfe, int dtd, int tpage, RECT *tw);
extern void SetDrawTPage(DR_TPAGE *p, int dfe, int dtd, int tpage);
extern void SetDrawMove(DR_MOVE *p, RECT *rect, int x, int y) ;
extern void SetDrawOffset(DR_OFFSET *p, u16 *ofs);
extern void SetDrawStp(DR_STP *p, int pbw);
extern void SetDumpFnt(int id);
extern void SetLineF2(LINE_F2 *p) ;
extern void SetLineF3(LINE_F3 *p) ;
extern void SetLineF4(LINE_F4 *p) ;
extern void SetLineG2(LINE_G2 *p) ;
extern void SetLineG3(LINE_G3 *p) ;
extern void SetLineG4(LINE_G4 *p) ;
extern void SetPolyF3(POLY_F3 *p) ;
extern void SetPolyF4(POLY_F4 *p) ;
extern void SetPolyFT3(POLY_FT3 *p) ;
extern void SetPolyFT4(POLY_FT4 *p) ;
extern void SetPolyG3(POLY_G3 *p) ;
extern void SetPolyG4(POLY_G4 *p) ;
extern void SetPolyGT3(POLY_GT3 *p) ;
extern void SetPolyGT4(POLY_GT4 *p) ;
extern void SetSemiTrans(void *p, int abe) ;
extern void SetShadeTex(void *p, int tge) ;
extern void SetSprt(SPRT *p) ;
extern void SetSprt16(SPRT_16 *p) ;
extern void SetSprt8(SPRT_8 *p) ;
extern void SetTexWindow(DR_TWIN *p, RECT *tw);
extern void SetTile(TILE *p) ;
extern void SetTile1(TILE_1 *p) ;
extern void SetTile16(TILE_16 *p) ;
extern void SetTile8(TILE_8 *p) ;
extern void TermPrim(void *p) ;
extern u32 *BreakDraw(void);
extern void ContinueDraw(u32 *insaddr, u32 *contaddr);
extern int IsIdleGPU(int max_count);
extern int GetODE(void);
extern int LoadImage2(RECT *rect, u32 *p);
extern int StoreImage2(RECT *rect, u32 *p);
extern int MoveImage2(RECT *rect, int x, int y);
extern int DrawOTag2(u32 *p);
extern void GetDrawMode(DR_MODE *p);
extern void GetTexWindow(DR_TWIN *p);
extern void GetDrawArea(DR_AREA *p);
extern void GetDrawOffset(DR_OFFSET *p);
extern void GetDrawEnv2(DR_ENV *p);



























typedef unsigned char PACKET;










typedef struct {
	VECTOR  scale;
	SVECTOR rotate;
	VECTOR  trans;
}       GsCOORD2PARAM;

typedef struct _GsCOORDINATE2 {
	unsigned long  flg;
	MATRIX  coord;
	MATRIX  workm;
	GsCOORD2PARAM *   param;
	struct _GsCOORDINATE2 *   super;
	struct _GsCOORDINATE2 *   sub;
}       GsCOORDINATE2;

typedef struct {
	MATRIX  view;
	GsCOORDINATE2 *   super;
}       GsVIEW2;

typedef struct {
	long     vpx, vpy, vpz;
	long     vrx, vry, vrz;
	long     rz;
	GsCOORDINATE2 *   super;
}       GsRVIEW2;

typedef struct {
	int     vx, vy, vz;
	unsigned char r, g, b;
}       GsF_LIGHT;


typedef struct {
	unsigned p:24;
	unsigned char num:8;
}       GsOT_TAG;


typedef struct {
	unsigned long  length;
	GsOT_TAG *   org;
	unsigned long  offset;
	unsigned long  point;
	GsOT_TAG *   tag;
}       GsOT;

typedef struct {
	unsigned long  attribute;
	GsCOORDINATE2 *   coord2;
	unsigned long  *   tmd;
	unsigned long  id;
}       GsDOBJ2;

typedef struct {
	unsigned long  attribute;
	GsCOORDINATE2 *   coord2;
	unsigned long  *   pmd;
	unsigned long  *   base;
	unsigned long  *   sv;
	unsigned long  id;
}       GsDOBJ3;

typedef struct {
	unsigned long  attribute;
	GsCOORDINATE2 *   coord2;
	unsigned long  *   tmd;
	unsigned long  id;
}       GsDOBJ4;

typedef struct {
	unsigned long  attribute;
	GsCOORDINATE2 *   coord2;
	unsigned long  *   tmd;
	unsigned long  *   packet;
	unsigned long  id;
}       GsDOBJ5;

typedef struct {
	unsigned long  attribute;
	short   x, y;
	unsigned short w, h;
	unsigned short tpage;
	unsigned char u, v;
	short   cx, cy;
	unsigned char r, g, b;
	short   mx, my;
	short   scalex, scaley;
	long     rotate;
}       GsSPRITE;

typedef struct {
	unsigned char u, v;
	unsigned short cba;
	unsigned short flag;
	unsigned short tpage;
}       GsCELL;

typedef struct {
	unsigned char cellw, cellh;
	unsigned short ncellw, ncellh;
	GsCELL *   base;
	unsigned short *   index;
}       GsMAP;

typedef struct {
	unsigned long  attribute;
	short   x, y;
	short   w, h;
	short   scrollx, scrolly;
	unsigned char r, g, b;
	GsMAP  *   map;
	short   mx, my;
	short   scalex, scaley;
	long     rotate;
}       GsBG;

typedef struct {
	unsigned long  attribute;
	short   x0, y0;
	short   x1, y1;
	unsigned char r, g, b;
}       GsLINE;

typedef struct {
	unsigned long  attribute;
	short   x0, y0;
	short   x1, y1;
	unsigned char r0, g0, b0;
	unsigned char r1, g1, b1;
}       GsGLINE;

typedef struct {
	unsigned long  attribute;
	short   x, y;
	unsigned short w, h;
	unsigned char r, g, b;
}       GsBOXF;

typedef struct {
	short   dqa;
	long     dqb;
	unsigned char rfc, gfc, bfc;
}       GsFOGPARAM;


typedef struct {
	unsigned long  pmode;
	short   px, py;
	unsigned short pw, ph;
	unsigned long  *   pixel;
	short   cx, cy;
	unsigned short cw, ch;
	unsigned long  *   clut;
}       GsIMAGE;

typedef struct {
	short   offx, offy;
}       _GsPOSITION;

typedef struct {
	GsDOBJ2 *   top;
	int     nobj;
	int     maxobj;
}       GsOBJTABLE2;

typedef struct {
	PACKET
	* (*   f3[2][3]) ();
	PACKET
	* (*   nf3[2]) ();
	PACKET
	* (*   g3[2][3]) ();
	PACKET
	* (*   ng3[2]) ();
	PACKET
	* (*   tf3[2][3]) ();
	PACKET
	* (*   ntf3[2]) ();
	PACKET
	* (*   tg3[2][3]) ();
	PACKET
	* (*   ntg3[2]) ();
	PACKET
	* (*   f4[2][3]) ();
	PACKET
	* (*   nf4[2]) ();
	PACKET
	* (*   g4[2][3]) ();
	PACKET
	* (*   ng4[2]) ();
	PACKET
	* (*   tf4[2][3]) ();
	PACKET
	* (*   ntf4[2]) ();
	PACKET
	* (*   tg4[2][3]) ();
	PACKET
	* (*   ntg4[2]) ();
	PACKET
	* (*   f3g[3])();
	PACKET
	* (*   g3g[3])();
	PACKET
	* (*   f4g[3])();
	PACKET
	* (*   g4g[3])();
}       _GsFCALL;




























































































typedef struct {
	u8  out, in, dummy, cd;
	u8  r0, g0, b0, code;
	u16 n0, v0;
	u16 v1, v2;
}       TMD_P_F3;

typedef struct {
	u8  out, in, dummy, cd;
	u8  r0, g0, b0, code;
	u16 n0, v0;
	u16 n1, v1;
	u16 n2, v2;
}       TMD_P_G3;

typedef struct {
	u8	out, in, dummy, cd;
	u8	r0, g0, b0, code;
	u8	r1, g1, b1, dummy1;
	u8	r2, g2, b2, dummy2;
	u16 n0, v0;
	u16 v1, v2;
}       TMD_P_F3G;

typedef struct {
	u8	out, in, dummy, cd;
	u8	r0, g0, b0, code;
	u8	r1, g1, b1, dummy1;
	u8	r2, g2, b2, dummy2;
	u16 n0, v0;
	u16 n1, v1;
	u16 n2, v2;
}       TMD_P_G3G;

typedef struct {
	u8  out, in, dummy, cd;
	u8  r0, g0, b0, code;
	u16 v0, v1;
	u16 v2, p;
}       TMD_P_NF3;

typedef struct {
	u8  out, in, dummy, cd;
	u8  r0, g0, b0, code;
	u8  r1, g1, b1, p1;
	u8  r2, g2, b2, p2;
	u16 v0, v1;
	u16 v2, p;
}       TMD_P_NG3;

typedef struct {
	u8  out, in, dummy, cd;
	u8  r0, g0, b0, code;
	u16 n0, v0;
	u16 v1, v2;
	u16 v3, p;
}       TMD_P_F4;

typedef struct {
	u8  out, in, dummy, cd;
	u8  r0, g0, b0, code;
	u16 n0, v0;
	u16 n1, v1;
	u16 n2, v2;
	u16 n3, v3;
}       TMD_P_G4;

typedef struct {
	u8	out, in, dummy, cd;
	u8	r0, g0, b0, code;
	u8	r1, g1, b1, dummy1;
	u8	r2, g2, b2, dummy2;
	u8	r3, g3, b3, dummy3;
	u16 n0, v0;
	u16 v1, v2;
	u16 v3, dummy4;
}       TMD_P_F4G;

typedef struct {
	u8	out, in, dummy, cd;
	u8	r0, g0, b0, code;
	u8	r1, g1, b1, dummy1;
	u8	r2, g2, b2, dummy2;
	u8	r3, g3, b3, dummy3;
	u16 n0, v0;
	u16 n1, v1;
	u16 n2, v2;
	u16 n3, v3;
}       TMD_P_G4G;

typedef struct {
	u8  out, in, dummy, cd;
	u8  r0, g0, b0, code;
	u16 v0, v1;
	u16 v2, v3;
}       TMD_P_NF4;

typedef struct {
	u8  out, in, dummy, cd;
	u8  r0, g0, b0, code;
	u8  r1, g1, b1, p1;
	u8  r2, g2, b2, p2;
	u8  r3, g3, b3, p3;
	u16 v0, v1;
	u16 v2, v3;
}       TMD_P_NG4;

typedef struct {
	u8  out, in, dummy, cd;
	u8  tu0, tv0;
	u16 clut;
	u8  tu1, tv1;
	u16 tpage;
	u8  tu2, tv2;
	u16 p;
	u16 n0, v0;
	u16 v1, v2;
}       TMD_P_TF3;

typedef struct {
	u8  out, in, dummy, cd;
	u8  tu0, tv0;
	u16 clut;
	u8  tu1, tv1;
	u16 tpage;
	u8  tu2, tv2;
	u16 p;
	u16 n0, v0;
	u16 n1, v1;
	u16 n2, v2;
}       TMD_P_TG3;

typedef struct {
	u8  out, in, dummy, cd;
	u8  tu0, tv0;
	u16 clut;
	u8  tu1, tv1;
	u16 tpage;
	u8  tu2, tv2;
	u16 p0;
	u8  r0, g0, b0, p1;
	u16 v0, v1;
	u16 v2, p2;
}       TMD_P_TNF3;

typedef struct {
	u8  out, in, dummy, cd;
	u8  tu0, tv0;
	u16 clut;
	u8  tu1, tv1;
	u16 tpage;
	u8  tu2, tv2;
	u16 p0;
	u8  r0, g0, b0, p1;
	u8  r1, g1, b1, p2;
	u8  r2, g2, b2, p3;
	u16 v0, v1;
	u16 v2, p4;
}       TMD_P_TNG3;

typedef struct {
	u8  out, in, dummy, cd;
	u8  tu0, tv0;
	u16 clut;
	u8  tu1, tv1;
	u16 tpage;
	u8  tu2, tv2;
	u16 p0;
	u8  tu3, tv3;
	u16 p1;
	u16 n0, v0;
	u16 v1, v2;
	u16 v3, p2;
}       TMD_P_TF4;

typedef struct {
	u8  out, in, dummy, cd;
	u8  tu0, tv0;
	u16 clut;
	u8  tu1, tv1;
	u16 tpage;
	u8  tu2, tv2;
	u16 p0;
	u8  tu3, tv3;
	u16 p1;
	u16 n0, v0;
	u16 n1, v1;
	u16 n2, v2;
	u16 n3, v3;
}       TMD_P_TG4;

typedef struct {
	u8  out, in, dummy, cd;
	u8  tu0, tv0;
	u16 clut;
	u8  tu1, tv1;
	u16 tpage;
	u8  tu2, tv2;
	u16 p0;
	u8  tu3, tv3;
	u16 p1;
	u8  r0, g0, b0, p2;
	u16 v0, v1;
	u16 v2, v3;
}       TMD_P_TNF4;

typedef struct {
	u8  out, in, dummy, cd;
	u8  tu0, tv0;
	u16 clut;
	u8  tu1, tv1;
	u16 tpage;
	u8  tu2, tv2;
	u16 p0;
	u8  tu3, tv3;
	u16 p1;
	u8  r0, g0, b0, p2;
	u8  r1, g1, b1, p3;
	u8  r2, g2, b2, p4;
	u8  r3, g3, b3, p5;
	u16 v0, v1;
	u16 v2, v3;
}       TMD_P_TNG4;

struct TMD_STRUCT {
	u32 *   vertop;
	u32  vern;
	u32 *   nortop;
	u32  norn;
	u32 *   primtop;
	u32  primn;
	u32  scale;
};











typedef struct {
	short   vx, vy, vz;
	u8  tu, tv;
}       VERT;

typedef struct {
	short   vx, vy, vz;
	u8  tu, tv;
	CVECTOR col;
}       VERTC;


typedef struct {
	u32  limit;
	long     hwd, vwd;
	int     shift;
	u32 *   org;
	u32 *   pk;
	long     otz;
	long     adivz;
	short   adivw, adivh;
	long     flg0;
	long     flg;
	short   minx, miny, maxx, maxy;
	short   hwd0, vwd0;
	u32 *   tag;
	POLY_FT4 si;
}       GsADIV_FT4;

typedef struct {
	VERT    vt[4];
}       GsADIV_P_FT4;



typedef struct {
	u32  limit;
	long     hwd, vwd;
	int     shift;
	u32 *   org;
	u32 *   pk;
	long     otz;
	long     adivz;
	short   adivw, adivh;
	long     flg0;
	long     flg;
	short   minx, miny, maxx, maxy;
	short   hwd0, vwd0;
	u32 *   tag;
	POLY_GT4 si;
}       GsADIV_GT4;

typedef struct {
	VERTC   vt[4];
}       GsADIV_P_GT4;


typedef struct {
	u32  limit;
	long     hwd, vwd;
	int     shift;
	u32 *   org;
	u32 *   pk;
	long     otz;
	long     adivz;
	short   adivw, adivh;
	long     flg0;
	long     flg;
	short   minx, miny, maxx, maxy;
	short   hwd0, vwd0;
	u32 *   tag;
	POLY_G4 si;
}       GsADIV_G4;

typedef struct {
	VERTC   vt[4];
}       GsADIV_P_G4;

typedef struct {
	u32  limit;
	long     hwd, vwd;
	int     shift;
	u32 *   org;
	u32 *   pk;
	long     otz;
	long     adivz;
	short   adivw, adivh;
	long     flg0;
	long     flg;
	short   minx, miny, maxx, maxy;
	short   hwd0, vwd0;
	u32 *   tag;
	POLY_F4 si;
}       GsADIV_F4;

typedef struct {
	VERT    vt[4];
}       GsADIV_P_F4;


typedef struct {
	u32  limit;
	long     hwd, vwd;
	int     shift;
	u32 *   org;
	u32 *   pk;
	long     otz;
	long     adivz;
	short   adivw, adivh;
	long     flg;
	short   minx, miny, maxx, maxy;
	short   hwd0, vwd0;
	u32 *   tag;
	POLY_FT3 si;
}       GsADIV_FT3;

typedef struct {
	VERT    vt[3];
}       GsADIV_P_FT3;

typedef struct {
	u32  limit;
	long     hwd, vwd;
	int     shift;
	u32 *   org;
	u32 *   pk;
	long     otz;
	long     adivz;
	short   adivw, adivh;
	long     flg;
	short   minx, miny, maxx, maxy;
	short   hwd0, vwd0;
	u32 *   tag;
	POLY_GT3 si;
}       GsADIV_GT3;

typedef struct {
	VERTC   vt[3];
}       GsADIV_P_GT3;

typedef struct {
	u32  limit;
	long     hwd, vwd;
	int     shift;
	u32 *   org;
	u32 *   pk;
	long     otz;
	long     adivz;
	short   adivw, adivh;
	long     flg;
	short   minx, miny, maxx, maxy;
	short   hwd0, vwd0;
	u32 *   tag;
	POLY_G3 si;
}       GsADIV_G3;

typedef struct {
	VERTC   vt[3];
}       GsADIV_P_G3;

typedef struct {
	u32  limit;
	long     hwd, vwd;
	int     shift;
	u32 *   org;
	u32 *   pk;
	long     otz;
	long     adivz;
	short   adivw, adivh;
	long     flg;
	short   minx, miny, maxx, maxy;
	short   hwd0, vwd0;
	u32 *   tag;
	POLY_F3 si;
}       GsADIV_F3;

typedef struct {
	VERT    vt[3];
}       GsADIV_P_F3;






	void    GsInitGraph(unsigned short x, unsigned short y, unsigned short intmode,
		              unsigned short dith, unsigned short varmmode);
	void    GsInit3D(void);
	void    GsMapModelingData(unsigned long  *p);

	void    GsSetProjection(long  h);
	int     GsSetFlatLight(int id, GsF_LIGHT * lt);
	void    GsSetLightMode(int mode);
	void    GsSetFogParam(GsFOGPARAM * fogparm);
	void    GsSetAmbient(long  r, long  g, long  b);
	void    GsDrawOt(GsOT * ot);
	void    GsSetWorkBase(PACKET * outpacketp);

	void    GsSortObject3(GsDOBJ3 * objp, GsOT * ot, int shift);
	void    GsSortObject4(GsDOBJ2 * objp, GsOT * ot, int shift, u32 * scratch);
	void    GsSortObject5(GsDOBJ5 * objp, GsOT * ot, int shift, u32 * scratch);
	void    GsSortObject5J(GsDOBJ5 * objp, GsOT * ot, int shift, u32 * scratch);

	void    GsSortSprite(GsSPRITE * sp, GsOT * ot, unsigned short pri);
	void    GsSortSpriteB(GsSPRITE * sp, GsOT * ot, unsigned short pri,
				unsigned short flip);
	void    GsSortFastSprite(GsSPRITE * sp, GsOT * ot, unsigned short pri);
	void    GsSortFastSpriteB(GsSPRITE * sp, GsOT * ot, unsigned short pri,
				unsigned short flip);
	void    GsSortFlipSprite(GsSPRITE * sp, GsOT * ot, unsigned short pri);
	void    GsSortBg(GsBG * bg, GsOT * ot, unsigned short pri);
	void    GsSortFastBg(GsBG * bg, GsOT * ot, unsigned short pri);
	void    GsInitFixBg16(GsBG * bg, u32 * work);
	void    GsSortFixBg16(GsBG * bg, u32 * work, GsOT * otp, unsigned short pri);
	void    GsInitFixBg32(GsBG * bg, u32 * work);
	void    GsSortFixBg32(GsBG * bg, u32 * work, GsOT * otp, unsigned short pri);
	void    GsSortLine(GsLINE * lp, GsOT * ot, unsigned short pri);
	void    GsSortGLine(GsGLINE * lp, GsOT * ot, unsigned short pri);
	void    GsSortBoxFill(GsBOXF * bp, GsOT * ot, unsigned short pri);
	void    GsSortPoly(void *pp, GsOT * ot, unsigned short pri);

	void    GsClearOt(unsigned short offset, unsigned short point, GsOT * otp);
	GsOT   *GsSortOt(GsOT * ot_src, GsOT * ot_dest);
	GsOT   *GsCutOt(GsOT * ot_src, GsOT * ot_dest);
	void    GsDefDispBuff(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1);
	void    GsSortClear(unsigned char, unsigned char, unsigned char, GsOT *);
	void    GsGetTimInfo(unsigned long  *im, GsIMAGE * tim);
	void    GsSwapDispBuff(void);
	int     GsGetActiveBuff(void);
	void    GsSetDrawBuffClip(void);
	void    GsSetDrawBuffOffset(void);
	void    GsSetClip(RECT * clip);
	DRAWENV *GsSetClip2(RECT * clip);
	void    GsSetOffset(long  x, long  y);
	void    GsSetOrign(long  x, long  y);

	void    GsInitCoordinate2(GsCOORDINATE2 * super, GsCOORDINATE2 * base);
	void    GsMulCoord0(MATRIX * m1, MATRIX * m2, MATRIX * m3);
	void    GsMulCoord2(MATRIX * m1, MATRIX * m2);
	void    GsMulCoord3(MATRIX * m1, MATRIX * m2);
	void    GsGetLw(GsCOORDINATE2 * m, MATRIX * out);
	void    GsGetLs(GsCOORDINATE2 * m, MATRIX * out);
	void    GsGetLws(GsCOORDINATE2 * m, MATRIX * outw, MATRIX * outs);

	u32  GsLinkObject3(unsigned long  pmd_base, GsDOBJ3 * objp);
	void    GsLinkObject4(unsigned long  tmd_base, GsDOBJ2 * objp, int n);
	void    GsLinkObject5(unsigned long  tmd_base, GsDOBJ5 * objp, int n);

	void    GsSetLightMatrix(MATRIX * mp);
	void    GsSetLightMatrix2(MATRIX * mp);
	int     GsSetRefView2(GsRVIEW2 * pv);
	int     GsSetRefView2L(GsRVIEW2 * pv);
	int     GsSetView2(GsVIEW2 * pv);
	void    GsSetLsMatrix(MATRIX * mp);
	void    GsSetClip2D(RECT * rectp);
	void    GsInitVcount();
	long     GsGetVcount();
	void    GsClearVcount();
	void	GsDefDispBuff2(u16 x0, u16 y0, u16 x1, u16 y1);
	void	GsDrawOtIO(GsOT *ot);
	PACKET *GsGetWorkBase();
	void	GsInitGraph2( u16 x, u16 y, u16 intmode, u16 dith, u16 vrammode);
	void	GsSortObject4J(GsDOBJ2 *objp, GsOT *otp, int shift, u32 *scratch);
	void    GsClearDispArea(unsigned char r, unsigned char g, unsigned char b);

	u32 *GsPresetObject(GsDOBJ5 * objp, u32 * base_addr);
	void    GsScaleScreen(SVECTOR * scale);

	PACKET *GsA4divF3L(TMD_P_F3 * op, VERT * vp, VERT * np, PACKET * pk, int n,
			           int shift, GsOT * ot, u32 * scratch);
	PACKET *GsA4divF3LFG(TMD_P_F3 * op, VERT * vp, VERT * np, PACKET * pk, int n,
			             int shift, GsOT * ot, u32 * scratch);
	PACKET *GsA4divF3NL(TMD_P_F3 * op, VERT * vp, VERT * np, PACKET * pk, int n,
			            int shift, GsOT * ot, u32 * scratch);
	PACKET *GsA4divNF3(TMD_P_NF3 * op, VERT * vp, PACKET * pk, int n,
			           int shift, GsOT * ot, u32 * scratch);
	PACKET *GsA4divF4L(TMD_P_F4 * op, VERT * vp, VERT * np, PACKET * pk, int n,
			           int shift, GsOT * ot, u32 * scratch);
	PACKET *GsA4divF4LFG(TMD_P_F4 * op, VERT * vp, VERT * np, PACKET * pk, int n,
			             int shift, GsOT * ot, u32 * scratch);
	PACKET *GsA4divF4NL(TMD_P_F4 * op, VERT * vp, VERT * np, PACKET * pk, int n,
			            int shift, GsOT * ot, u32 * scratch);
	PACKET *GsA4divNF4(TMD_P_NF4 * op, VERT * vp, PACKET * pk, int n,
			           int shift, GsOT * ot, u32 * scratch);
	PACKET *GsA4divG3L(TMD_P_G3 * op, VERT * vp, VERT * np, PACKET * pk, int n,
			           int shift, GsOT * ot, u32 * scratch);
	PACKET *GsA4divG3LFG(TMD_P_G3 * op, VERT * vp, VERT * np, PACKET * pk, int n,
			             int shift, GsOT * ot, u32 * scratch);
	PACKET *GsA4divG3NL(TMD_P_G3 * op, VERT * vp, VERT * np, PACKET * pk, int n,
			            int shift, GsOT * ot, u32 * scratch);
	PACKET *GsA4divNG3(TMD_P_NG3 * op, VERT * vp, PACKET * pk, int n,
			           int shift, GsOT * ot, u32 * scratch);
	PACKET *GsA4divG4L(TMD_P_G4 * op, VERT * vp, VERT * np, PACKET * pk, int n,
			           int shift, GsOT * ot, u32 * scratch);
	PACKET *GsA4divG4LFG(TMD_P_G4 * op, VERT * vp, VERT * np, PACKET * pk, int n,
			             int shift, GsOT * ot, u32 * scratch);
	PACKET *GsA4divG4NL(TMD_P_G4 * op, VERT * vp, VERT * np, PACKET * pk, int n,
			            int shift, GsOT * ot, u32 * scratch);
	PACKET *GsA4divNG4(TMD_P_NG4 * op, VERT * vp, PACKET * pk, int n,
			           int shift, GsOT * ot, u32 * scratch);
	PACKET *GsA4divTF3L(TMD_P_TF3 * op, VERT * vp, VERT * np, PACKET * pk, int n,
			            int shift, GsOT * ot, u32 * scratch);
	PACKET *GsA4divTF3LFG(TMD_P_TF3 * op, VERT * vp, VERT * np, PACKET * pk, int n,
			            int shift, GsOT * ot, u32 * scratch);
	PACKET *GsA4divTF3NL(TMD_P_TF3 * op, VERT * vp, VERT * np, PACKET * pk, int n,
			             int shift, GsOT * ot, u32 * scratch);
	PACKET *GsA4divTNF3(TMD_P_TNF3 * op, VERT * vp, PACKET * pk, int n,
			            int shift, GsOT * ot, u32 * scratch);
	PACKET *GsA4divTF4L(TMD_P_TF4 * op, VERT * vp, VERT * np, PACKET * pk, int n,
			            int shift, GsOT * ot, u32 * scratch);
	PACKET *GsA4divTF4LFG(TMD_P_TF4 * op, VERT * vp, VERT * np, PACKET * pk, int n,
			            int shift, GsOT * ot, u32 * scratch);
	PACKET *GsA4divTF4NL(TMD_P_TF4 * op, VERT * vp, VERT * np, PACKET * pk, int n,
			             int shift, GsOT * ot, u32 * scratch);
	PACKET *GsA4divTNF4(TMD_P_TNF4 * op, VERT * vp, PACKET * pk, int n,
			            int shift, GsOT * ot, u32 * scratch);
	PACKET *GsA4divTF4LM(TMD_P_TF4 * op, VERT * vp, VERT * np, PACKET * pk, int n,
			            int shift, GsOT * ot, u32 * scratch);
	PACKET *GsA4divTF4LFGM(TMD_P_TF4 * op, VERT * vp, VERT * np, PACKET * pk, int n,
			            int shift, GsOT * ot, u32 * scratch);
	PACKET *GsA4divTF4NLM(TMD_P_TF4 * op, VERT * vp, VERT * np, PACKET * pk, int n,
			             int shift, GsOT * ot, u32 * scratch);
	PACKET *GsA4divTNF4M(TMD_P_TNF4 * op, VERT * vp, PACKET * pk, int n,
			            int shift, GsOT * ot, u32 * scratch);
	PACKET *GsA4divTG3L(TMD_P_TG3 * op, VERT * vp, VERT * np, PACKET * pk, int n,
			            int shift, GsOT * ot, u32 * scratch);
	PACKET *GsA4divTG3LFG(TMD_P_TG3 * op, VERT * vp, VERT * np, PACKET * pk, int n,
			            int shift, GsOT * ot, u32 * scratch);
	PACKET *GsA4divTG3NL(TMD_P_TG3 * op, VERT * vp, VERT * np, PACKET * pk, int n,
			             int shift, GsOT * ot, u32 * scratch);
	PACKET *GsA4divTNG3(TMD_P_TNG3 * op, VERT * vp, PACKET * pk, int n,
			            int shift, GsOT * ot, u32 * scratch);
	PACKET *GsA4divTG4L(TMD_P_TG4 * op, VERT * vp, VERT * np, PACKET * pk, int n,
			            int shift, GsOT * ot, u32 * scratch);
	PACKET *GsA4divTG4LFG(TMD_P_TG4 * op, VERT * vp, VERT * np, PACKET * pk, int n,
			            int shift, GsOT * ot, u32 * scratch);
	PACKET *GsA4divTG4NL(TMD_P_TG4 * op, VERT * vp, VERT * np, PACKET * pk, int n,
			             int shift, GsOT * ot, u32 * scratch);
	PACKET *GsA4divTNG4(TMD_P_TNG4 * op, VERT * vp, PACKET * pk, int n,
			            int shift, GsOT * ot, u32 * scratch);
	PACKET *GsA4divTG4LM(TMD_P_TG4 * op, VERT * vp, VERT * np, PACKET * pk, int n,
			            int shift, GsOT * ot, u32 * scratch);
	PACKET *GsA4divTG4LFGM(TMD_P_TG4 * op, VERT * vp, VERT * np, PACKET * pk, int n,
			            int shift, GsOT * ot, u32 * scratch);
	PACKET *GsA4divTG4NLM(TMD_P_TG4 * op, VERT * vp, VERT * np, PACKET * pk, int n,
			             int shift, GsOT * ot, u32 * scratch);
	PACKET *GsA4divTNG4M(TMD_P_TNG4 * op, VERT * vp, PACKET * pk, int n,
			            int shift, GsOT * ot, u32 * scratch);
	PACKET *GsTMDfastF3GL(TMD_P_F3G *op, VERT *vp, VERT *np, PACKET *pk,
						int n, int shift, GsOT *ot, u32 *scratch);
	PACKET *GsTMDfastF3GLFG(TMD_P_F3G *op, VERT *vp, VERT *np, PACKET *pk,
						int n, int shift, GsOT *ot, u32 *scratch);
	PACKET *GsTMDfastF3GNL(TMD_P_F3G *op, VERT *vp, VERT *np, PACKET *pk,
						int n, int shift, GsOT *ot, u32 *scratch);
	PACKET *GsTMDfastG3GL(TMD_P_G3G *op, VERT *vp, VERT *np, PACKET *pk,
						int n, int shift, GsOT *ot, u32 *scratch);
	PACKET *GsTMDfastG3GLFG(TMD_P_G3G *op, VERT *vp, VERT *np, PACKET *pk,
						int n, int shift, GsOT *ot, u32 *scratch);
	PACKET *GsTMDfastG3GNL(TMD_P_G3G *op, VERT *vp, VERT *np, PACKET *pk,
						int n, int shift, GsOT *ot, u32 *scratch);
	PACKET *GsPrstF3GL(TMD_P_F3G *op, VERT *vp, VERT *np, PACKET *pk,
						int n, int shift, GsOT *ot, u32 *scratch);
	PACKET *GsPrstF3GLFG(TMD_P_F3G *op, VERT *vp, VERT *np, PACKET *pk,
						int n, int shift, GsOT *ot, u32 *scratch);
	PACKET *GsPrstF3GNL(TMD_P_F3G *op, VERT *vp, VERT *np, PACKET *pk,
						int n, int shift, GsOT *ot, u32 *scratch);
	PACKET *GsPrstG3GL(TMD_P_G3G *op, VERT *vp, VERT *np, PACKET *pk,
						int n, int shift, GsOT *ot, u32 *scratch);
	PACKET *GsPrstG3GLFG(TMD_P_G3G *op, VERT *vp, VERT *np, PACKET *pk,
						int n, int shift, GsOT *ot, u32 *scratch);
	PACKET *GsPrstG3GNL(TMD_P_G3G *op, VERT *vp, VERT *np, PACKET *pk,
						int n, int shift, GsOT *ot, u32 *scratch);
	PACKET *GsTMDfastG3M(TMD_P_G3 *op, VERT *vp, VERT *np, PACKET *pk,
						int n, int shift, GsOT *ot, u32 *scratch);
	PACKET *GsTMDfastG3MFG(TMD_P_G3 *op, VERT *vp, VERT *np, PACKET *pk,
						int n, int shift, GsOT *ot, u32 *scratch);
	PACKET *GsTMDfastTG3M(TMD_P_TG3 *op, VERT *vp, VERT *np, PACKET *pk,
						int n, int shift, GsOT *ot, u32 *scratch);
	PACKET *GsTMDfastTG3MFG(TMD_P_TG3 *op, VERT *vp, VERT *np, PACKET *pk,
						int n, int shift, GsOT *ot, u32 *scratch);
	PACKET *GsTMDfastF4GL(TMD_P_F4G *op, VERT *vp, VERT *np, PACKET *pk,
						int n, int shift, GsOT *ot, u32 *scratch);
	PACKET *GsTMDfastF4GLFG(TMD_P_F4G *op, VERT *vp, VERT *np, PACKET *pk,
						int n, int shift, GsOT *ot, u32 *scratch);
	PACKET *GsTMDfastF4GNL(TMD_P_F4G *op, VERT *vp, VERT *np, PACKET *pk,
						int n, int shift, GsOT *ot, u32 *scratch);
	PACKET *GsTMDfastG4GL(TMD_P_G4G *op, VERT *vp, VERT *np, PACKET *pk,
						int n, int shift, GsOT *ot, u32 *scratch);
	PACKET *GsTMDfastG4GLFG(TMD_P_G4G *op, VERT *vp, VERT *np, PACKET *pk,
						int n, int shift, GsOT *ot, u32 *scratch);
	PACKET *GsTMDfastG4GNL(TMD_P_G4G *op, VERT *vp, VERT *np, PACKET *pk,
						int n, int shift, GsOT *ot, u32 *scratch);
	PACKET *GsTMDfastG4M(TMD_P_G4 *op, VERT *vp, VERT *np, PACKET *pk,
						int n, int shift, GsOT *ot, u32 *scratch);
	PACKET *GsTMDfastG4MFG(TMD_P_G4 *op, VERT *vp, VERT *np, PACKET *pk,
						int n, int shift, GsOT *ot, u32 *scratch);
	PACKET *GsTMDfastTG4M(TMD_P_TG4 *op, VERT *vp, VERT *np, PACKET *pk,
						int n, int shift, GsOT *ot, u32 *scratch);
	PACKET *GsTMDfastTG4MFG(TMD_P_TG4 *op, VERT *vp, VERT *np, PACKET *pk,
						int n, int shift, GsOT *ot, u32 *scratch);
	PACKET *GsTMDfastF3M(TMD_P_F3 *op, VERT *vp, VERT *np, PACKET *pk,
						int n, int shift, GsOT *ot, u32 *scratch);
	PACKET *GsTMDfastF3MFG(TMD_P_F3 *op, VERT *vp, VERT *np, PACKET *pk,
						int n, int shift, GsOT *ot, u32 *scratch);
	PACKET *GsTMDfastTF3M(TMD_P_TF3 *op, VERT *vp, VERT *np, PACKET *pk,
						int n, int shift, GsOT *ot, u32 *scratch);
	PACKET *GsTMDfastTF3MFG(TMD_P_TF3 *op, VERT *vp, VERT *np, PACKET *pk,
						int n, int shift, GsOT *ot, u32 *scratch);
	PACKET *GsTMDfastF4M(TMD_P_F4 *op, VERT *vp, VERT *np, PACKET *pk,
						int n, int shift, GsOT *ot, u32 *scratch);
	PACKET *GsTMDfastF4MFG(TMD_P_F4 *op, VERT *vp, VERT *np, PACKET *pk,
						int n, int shift, GsOT *ot, u32 *scratch);
	PACKET *GsTMDfastTF4M(TMD_P_TF4 *op, VERT *vp, VERT *np, PACKET *pk,
						int n, int shift, GsOT *ot, u32 *scratch);
	PACKET *GsTMDfastTF4MFG(TMD_P_TF4 *op, VERT *vp, VERT *np, PACKET *pk,
						int n, int shift, GsOT *ot, u32 *scratch);




	POLY_FT4 *DivideFT4(SVECTOR *v0, SVECTOR *v1, SVECTOR *v2, SVECTOR *v3,
			u32 *uv0, u32 *uv1, u32 *uv2, u32 *uv3,
			CVECTOR *rgbc, POLY_FT4 *s, u32 *ot, DIVPOLYGON4 *divp);






extern RECT CLIP2;
extern short PSDBASEX[2], PSDBASEY[2];
extern short PSDIDX;
extern u32 PSDCNT;
extern _GsPOSITION POSITION;
extern DRAWENV GsDRAWENV;
extern DISPENV GsDISPENV;
extern MATRIX GsWSMATRIX;
extern MATRIX GsWSMATRIX_ORG;
extern long  HWD0, VWD0;
extern MATRIX GsLIGHTWSMATRIX;
extern MATRIX GsIDMATRIX;
extern MATRIX GsIDMATRIX2;
extern PACKET *   GsOUT_PACKET_P;
extern long  GsADIVZ;
extern short GsADIVW, GsADIVH;
extern int GsLIGHT_MODE;
extern u32 GsMATE_C, GsLMODE, GsLIGNR, GsLIOFF, GsZOVER, GsBACKC, GsNDIV;
extern u32 GsTRATE, GsTON, GsDISPON;


















GsOT *func_80058F10(void);



void func_800862C0(void *work);


















extern u32 *   D_800FE240 __attribute__((section(".data")));



void func_8005B260(u32 *src, GsOT *ot, s32 idx, s32 flags);



void Graphics_SubmitTextureWindowPacket(
    u32 *src,
    GsOT *ot,
    s32 idx,
    s32 offx,
    s32 offy,
    s32 maskx,
    s32 masky
);



void func_8005B4D8(u32 *src, GsOT *ot, s32 idx, s32 flags);













typedef struct {
    u8 pad0[0x58];
    SVECTOR corner[4];
    u8 inner[4];
    u8 outer[4];
    s32 size;
    u8 pad84[0x0C];
} ModelVariantQuad;



typedef struct {
    SVECTOR inner[8];
    SVECTOR outer[8];
    u8 color[4];
    u8 pad84[4];
    s32 angle;
    s32 count;
} ModelVariantRing;




typedef struct {
    SVECTOR inner[8];
    SVECTOR outer[8];
    u8 pad80[4];
    u8 color[4];
    s32 angle;
    s32 count;
} ModelVariantSpokeRing;




typedef struct {
    SVECTOR v0[4];
    SVECTOR v1[4];
    SVECTOR v2[4];
    SVECTOR v3[4];
    u8 outer[4];
    u8 inner[4];
    s32 size;
    u8 pad8C[0x0C];
} ModelVariantSheet;




typedef struct {
    SVECTOR a[9];
    SVECTOR b[9];
    SVECTOR c[9];
    long  sa[9];
    long  sb[9];
    long  sc[9];
    u8 ca[9][4];
    u8 cb[9][4];
    u8 pad18C[0x18];
    s32 otz[9];
} ModelVariantBand;


typedef struct {
    SVECTOR a[9];
    SVECTOR b[9];
    SVECTOR c[9];
    long  sa[9];
    long  sb[9];
    long  sc[9];
    u8 ca[9][4];
    u8 cb[9][4];
    u8 pad18C[0x18];
    s32 otz[9];
    u8 pad1C8[0x24];
} ModelVariantBandPadded;



typedef struct {
    SVECTOR a[5];
    SVECTOR b[5];
    SVECTOR c[5];
    long  sa[5];
    long  sb[5];
    long  sc[5];
    u8 ca[5][4];
    u8 cb[5][4];
    u8 padDC[0x18];
    s32 otz[5];
    long  flag[5];
} ModelVariantBandShort;




typedef struct {
    SVECTOR a[2];
    long  sa[2];
    s32 angle[2];
    SVECTOR b[2];
    long  sb[2];
    s32 width[2];
    u8 unknown_40[0x24];
    s32 otz[2];
    u16 ox[2];
    u16 oy[2];
} ModelVariantRibbon;



typedef struct {
    SVECTOR a[2];
    long  sa[2];
    s32 angle[2];
    SVECTOR b[2];
    long  sb[2];
    s32 width[2];
    u8 unknown_40[0x1C];
    s32 otz[2];
    u16 ox[2];
    u16 oy[2];
} ModelVariantRibbonShort;


typedef struct {
    SVECTOR point[13];
    u8 pad68[0x14];
} ModelVariantStrand;



typedef struct {
    SVECTOR near[6][6];
    SVECTOR far[6][6];
    CVECTOR color;
    u8 unknown_244[0x10];
    s32 scale;
    u8 unknown_258[8];
} ModelVariantWeb;


typedef struct {
    SVECTOR near[5][6];
    SVECTOR far[5][6];
    CVECTOR color;
    u8 unknown_1E4[0x10];
    s32 scale;
    u8 unknown_1F8[8];
} ModelVariantWebSmall;


typedef struct {
    SVECTOR near[4][6];
    SVECTOR far[4][6];
    CVECTOR color;
    u8 unknown_184[0x10];
    s32 scale;
    s32 done;
    u8 unknown_19C[4];
} ModelVariantWebNarrow;


typedef struct {
    SVECTOR point[13];
    u8 pad68[0x1C];
} ModelVariantStrandWide;



typedef struct {
    SVECTOR v0[4];
    SVECTOR v1[4];
    SVECTOR v2[4];
    SVECTOR v3[4];
    u8 outer[4];
    u8 inner[4];
    s32 size;
    u8 pad8C[0x0C];
    s32 shown;
} ModelVariantSheetSet;



typedef struct {
    SVECTOR a[17];
    SVECTOR b[17];
    s32 scale;
    s32 count;
} ModelVariantCurtain;








typedef struct {
    u8 unknown00[0x10];
    SVECTOR a[2];
    long  sa[2];
    s32 angle[2];
    SVECTOR b[2];
    long  sb[2];
    s32 width[2];
    u8 unknown50[0x14];
    s32 otz[2];
    s16 ox[2];
    s16 oy[2];
    u8 unknown74[8];
} Variant425SpiralArm;









void func_8013C864(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long  status[12][2];
    long  p;
    long  flag;
    u8 *work;
    u8 *timing;
    GsOT *ot;
    s16 i;
    s32 phi;
    s32 eighth;
    s32 r;
    Variant425SpiralArm *arm;
    POLY_GT4 *poly;
    s16 k;
    s32 spread;
    s32 turn;
    s32 theta;
    s16 length;
    s16 size;
    s32 dx;
    s32 dy;

    work = ctx;
    ot = func_80058F10();
    poly = (POLY_GT4 *)(work + 0x1778);
    turn = ratan2((*(s32 *)((u8 *)( work ) + (  0x190C ))) , (*(s32 *)((u8 *)( work ) + (  0x1904 ))) ) + 0xC00;
    if ((*(s32 *)((u8 *)( work ) + (  0x1960 )))  >= 0) {
    r = (*(s16 *)((u8 *)( work ) + (  0x1934 )))  / 2;
    eighth = (*(s16 *)((u8 *)( work ) + (  0x1934 )))  / 8;
    if (r * (*(s16 *)((u8 *)( work ) + (  0x1932 )))  < 0) {
        length = 0;
    }
    if (eighth * (*(s16 *)((u8 *)( work ) + (  0x1932 )))  < 0) {
        length = 0;
    }
    if ((*(s32 *)((u8 *)( work ) + (  0x1960 )))  >= 3) {
        timing = work + 0x1070;
        spread = (*(s32 *)((u8 *)( timing ) + (  0x88 )))  / 8;
    } else {
        spread = 0x400 - (*(s16 *)((u8 *)( work ) + (  0x1936 ))) ;
    }
    arm = (Variant425SpiralArm *)(work + 0x4E0);
    for (i = 0; i < 12; i++, arm++) {
        phi = (i << 12) / 12 + (*(s16 *)((u8 *)( work ) + (  0x1930 )))  * 2;
        theta = (i << 13) / 12 + (*(s16 *)((u8 *)( work ) + (  0x1930 ))) ;
        for (k = 0; k < 2; k++) {
            if (k == 0) {
                length = spread / 128;
            } else {
                length = spread / 32;
            }
            if (k == 0) {
                ( &arm->a[k] )->vx =   0 , ( &arm->a[k] )->vy =   0 , ( &arm->a[k] )->vz =   0  ;
            } else {
                ( &arm->a[k] )->vx =   rcos(phi) * (rsin(theta) * (r * k) >> 12) >> 12 , ( &arm->a[k] )->vy =
                          rcos(theta) * (r * k) >> 12 , ( &arm->a[k] )->vz =
                          rsin(phi) * (rsin(theta) * (r * k) >> 12) >> 12  ;
            }
            ( &arm->b[k] )->vx =   arm->a[k].vx + (rcos(turn) * length >> 12) , ( &arm->b[k] )->vy =   arm->a[k].vy , ( &arm->b[k] )->vz =
                      arm->a[k].vz + (rsin(turn) * length >> 12)  ;
        }
    }
    if (!((*(s32 *)((u8 *)( work ) + (  0x191C )))  & 1)) {
        size = 0x1000;
    } else {
        size = 0x1200;
    }
    arm = (Variant425SpiralArm *)(work + 0x4E0);
    rot.vx = 0;
    rot.vy = 0;
    rot.vz = 0;
    m.t[0] = (*(s16 *)((u8 *)( work ) + (  0x18E8 ))) ;
    m.t[1] = (*(s16 *)((u8 *)( work ) + (  0x18EA ))) ;
    m.t[2] = (*(s16 *)((u8 *)( work ) + (  0x18EC ))) ;
    scale.vx = size;
    scale.vy = size;
    scale.vz = size;
    RotMatrix(&rot, &m);
    ScaleMatrix(&m, &scale);
    coord.coord = m;
    coord.super = 0;
    coord.flg = 0;
    GsGetLs(&coord, &ls);
    GsSetLsMatrix(&ls);
    for (i = 0; i < 12; i++, arm++) {
        for (k = 0; k < 2; k++) {
            if (k == 1) {
                arm->otz[1] = RotTransPers4(&arm->a[0], &arm->a[1], &arm->a[0], &arm->a[1],
                                            &arm->sa[0], &arm->sa[1], &arm->sa[0], &arm->sa[1],
                                            &p, &status[i][1]);
                RotTransPers(&arm->b[1], &arm->sb[1], &p, &flag);
                dx = (s16)arm->sa[1] - (s16)arm->sa[0];
                dy = (arm->sa[1] >> 16) - (arm->sa[0] >> 16);
                arm->angle[1] = ratan2(dy, dx) + 0xC00;
                arm->width[1] = (s16)arm->sb[1] - (s16)arm->sa[1];
                arm->ox[1] = rcos(arm->angle[1]) * arm->width[1] >> 12;
                arm->oy[1] = rsin(arm->angle[1]) * arm->width[1] >> 12;
            } else {
                arm->otz[k] = RotTransPers4(&arm->a[k], &arm->a[k + 1], &arm->a[k], &arm->a[k + 1],
                                            &arm->sa[k], &arm->sa[k + 1], &arm->sa[k], &arm->sa[k + 1],
                                            &p, &status[i][k]);
                RotTransPers(&arm->b[k], &arm->sb[k], &p, &flag);
                dx = (s16)arm->sa[k + 1] - (s16)arm->sa[k];
                dy = (arm->sa[k + 1] >> 16) - (arm->sa[k] >> 16);
                arm->angle[k] = ratan2(dy, dx) + 0xC00;
                arm->width[k] = (s16)arm->sb[k] - (s16)arm->sa[k];
                arm->ox[k] = rcos(arm->angle[k]) * arm->width[k] >> 12;
                arm->oy[k] = rsin(arm->angle[k]) * arm->width[k] >> 12;
            }
        }
    }
    arm = (Variant425SpiralArm *)(work + 0x4E0);
    for (i = 0; i < 12; i++, arm++) {
        u8 r0;
        u8 g0;
        u8 b0;
        u8 r2;
        u8 g2;
        u8 b2;

        if ((((*(s32 *)((u8 *)( work ) + (  0x191C )))  + i) & 7) == 0) {
            r0 = 128;
            g0 = 0;
            b0 = 0;
            r2 = 192;
            g2 = 128;
            b2 = 128;
        } else if ((((*(s32 *)((u8 *)( work ) + (  0x191C )))  + i) & 7) == 1) {
            r0 = 128;
            g0 = 64;
            b0 = 0;
            r2 = 192;
            g2 = 160;
            b2 = 128;
        } else if ((((*(s32 *)((u8 *)( work ) + (  0x191C )))  + i) & 7) == 2) {
            r0 = 128;
            g0 = 128;
            b0 = 0;
            r2 = 192;
            g2 = 192;
            b2 = 128;
        } else if ((((*(s32 *)((u8 *)( work ) + (  0x191C )))  + i) & 7) == 3) {
            r0 = 0;
            g0 = 128;
            b0 = 0;
            r2 = 128;
            g2 = 192;
            b2 = 128;
        } else if ((((*(s32 *)((u8 *)( work ) + (  0x191C )))  + i) & 7) == 4) {
            r0 = 0;
            g0 = 128;
            b0 = 128;
            r2 = 128;
            g2 = 192;
            b2 = 192;
        } else if ((((*(s32 *)((u8 *)( work ) + (  0x191C )))  + i) & 7) == 5) {
            r0 = 0;
            g0 = 0;
            b0 = 128;
            r2 = 128;
            g2 = 128;
            b2 = 192;
        } else if ((((*(s32 *)((u8 *)( work ) + (  0x191C )))  + i) & 7) == 6) {
            r0 = 128;
            g0 = 0;
            b0 = 128;
            r2 = 192;
            g2 = 128;
            b2 = 192;
        } else if ((((*(s32 *)((u8 *)( work ) + (  0x191C )))  + i) & 7) == 7) {
            r0 = 128;
            g0 = 0;
            b0 = 64;
            r2 = 192;
            g2 = 128;
            b2 = 160;
        }
        for (k = 0; k < 1; k++) {
            poly->x0 = arm->sa[k] + arm->ox[k];
            poly->y0 = (arm->sa[k] >> 16) + arm->oy[k];
            poly->x1 = arm->sa[k + 1] + arm->ox[k + 1];
            poly->y1 = (arm->sa[k + 1] >> 16) + arm->oy[k + 1];
            poly->x2 = arm->sa[k];
            poly->y2 = arm->sa[k] >> 16;
            poly->x3 = arm->sa[k + 1];
            poly->y3 = arm->sa[k + 1] >> 16;
            if (k + 1 == 1) {
                poly->r0 = r0;
                poly->g0 = g0;
                poly->b0 = b0;
                poly->r1 = 0;
                poly->g1 = 0;
                poly->b1 = 0;
                poly->r2 = r2;
                poly->g2 = g2;
                poly->b2 = b2;
                poly->r3 = 0;
                poly->g3 = 0;
                poly->b3 = 0;
            } else {
                poly->r0 = r0;
                poly->g0 = g0;
                poly->b0 = b0;
                poly->r1 = r0;
                poly->g1 = g0;
                poly->b1 = b0;
                poly->r2 = r2;
                poly->g2 = g2;
                poly->b2 = b2;
                poly->r3 = r2;
                poly->g3 = g2;
                poly->b3 = b2;
            }
            if (arm->otz[k] >= 0 && status[i][k] >= 0) {
                GsSortPoly(poly, ot, arm->otz[k]);
            }
            poly->x0 = arm->sa[k] - arm->ox[k];
            poly->y0 = (arm->sa[k] >> 16) - arm->oy[k];
            poly->x1 = arm->sa[k + 1] - arm->ox[k + 1];
            poly->y1 = (arm->sa[k + 1] >> 16) - arm->oy[k + 1];
            poly->x2 = arm->sa[k];
            poly->y2 = arm->sa[k] >> 16;
            poly->x3 = arm->sa[k + 1];
            poly->y3 = arm->sa[k + 1] >> 16;
            if (k + 1 == 1) {
                poly->r0 = r0;
                poly->g0 = g0;
                poly->b0 = b0;
                poly->r1 = 0;
                poly->g1 = 0;
                poly->b1 = 0;
                poly->r2 = r2;
                poly->g2 = g2;
                poly->b2 = b2;
                poly->r3 = 0;
                poly->g3 = 0;
                poly->b3 = 0;
            } else {
                poly->r0 = r0;
                poly->g0 = g0;
                poly->b0 = b0;
                poly->r1 = r0;
                poly->g1 = g0;
                poly->b1 = b0;
                poly->r2 = r2;
                poly->g2 = g2;
                poly->b2 = b2;
                poly->r3 = r2;
                poly->g3 = g2;
                poly->b3 = b2;
            }
            if (arm->otz[k] >= 0 && status[i][k] >= 0) {
                GsSortPoly(poly, ot, arm->otz[k]);
            }
        }
    }
    }
    (*(s16 *)((u8 *)( work ) + (  0x1930 )))  += 16;
    if ((u32)(*(s32 *)((u8 *)( (*(s32 *)((u8 *)( work ) + (  0x1938 )))  ) + (  0x24 )))  <= (u32)(*(s32 *)((u8 *)( work ) + (  0x1920 )))  &&
        (*(s32 *)((u8 *)( work ) + (  0x1964 )))  == 0 && (*(s16 *)((u8 *)( work ) + (  0x1934 )))  < 0x400) {
        (*(s16 *)((u8 *)( work ) + (  0x1934 )))  += (*(s32 *)((u8 *)( work ) + (  0x1928 )))  * 64;
        if ((*(s16 *)((u8 *)( work ) + (  0x1934 )))  >= 0x400) {
            (*(s16 *)((u8 *)( work ) + (  0x1934 )))  = 0x400;
            (*(s32 *)((u8 *)( work ) + (  0x1964 )))  = 2;
        }
    }
}
