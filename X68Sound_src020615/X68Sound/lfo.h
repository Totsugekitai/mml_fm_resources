#define	SIZELFOTBL	512				// 2^9
#define	SIZELFOTBL_BITS	9
#define	LFOPRECISION	4096	// 2^12
//#define	PMTBLMAXVAL	(128)
//#define	PMTBLMAXVAL_BITS	(7)
//#define	AMTBLMAXVAL	(256)
//#define	AMTBLMAXVAL_BITS	(8)
//#define	LFOTIMECYCLE	 1073741824		// 2^30
//#define LFOTIMECYCLE_BITS	30
//#define	LFORNDTIMECYCLE	 (LFOTIMECYCLE>>8)		// 2^22
//#define	CYCLE2PMAM	(30-8)				// log2(LFOTIMECYCLE/SIZEPMAMTBL)
//#define	LFOHZ		0.0009313900811
//int		LFOSTEPTBL[256];
//int		LFOSTEPTBL3[256];		// Wave form 3 �p
//short	PMSTBL[8]={ 0,1,2,4,8,16,64,128 };
int	PMSMUL[8]={ 0,1,2,4,8,16,32,32 };
int	PMSSHL[8]={ 0,0,0,0,0, 0, 1, 2 };



class Lfo {
	int Pmsmul[N_CH];	// 0, 1, 2, 4, 8, 16, 32, 32
	int Pmsshl[N_CH];	// 0, 0, 0, 0, 0,  0,  1,  2
	int	Ams[N_CH];	// ���V�t�g�� 31(0), 0(1), 1(2), 2(3)
	int	PmdPmsmul[N_CH];	// Pmd*Pmsmul[]
	int	Pmd;
	int	Amd;

	int LfoStartingFlag;	// 0:LFO��~��  1:LFO���쒆
	int	LfoOverFlow;	// LFO t�̃I�[�o�[�t���[�l
	int	LfoTime;	// LFO��p t
	int	LfoTimeAdd;	// LFO��p��t
	int LfoIdx;	// LFO�e�[�u���ւ̃C���f�b�N�X�l
	int LfoSmallCounter;	// LFO�����������J�E���^ (0�`15�̒l���Ƃ�)
	int LfoSmallCounterStep;	// LFO�����������J�E���^�p�X�e�b�v�l (16�`31)
	int	Lfrq;		// LFO���g���ݒ�l LFRQ
	int	LfoWaveForm;	// LFO wave form

	int	PmTblValue,AmTblValue;
	int	PmValue[N_CH],AmValue[N_CH];

	char	PmTbl0[SIZELFOTBL], PmTbl2[SIZELFOTBL];
	unsigned char	AmTbl0[SIZELFOTBL], AmTbl2[SIZELFOTBL];

	inline void CulcTblValue();
	inline void	CulcPmValue(int ch);
	inline void	CulcAmValue(int ch);
	inline void CulcAllPmValue();
	inline void CulcAllAmValue();

public:
	Lfo(void);
	~Lfo() {};

	inline void Init();
	inline void InitSamprate();

	inline void LfoReset();
	inline void LfoStart();
	inline void SetLFRQ(int n);
	inline void SetPMDAMD(int n);
	inline void	SetWaveForm(int n);
	inline void	SetPMSAMS(int ch, int n);

	inline void	Update();
	inline int	GetPmValue(int ch);
	inline int	GetAmValue(int ch);
};

Lfo::Lfo(void) {
	int i;

	for (i=0; i<N_CH; ++i) {
		Pmsmul[i] = 0;
		Pmsshl[i] = 0;
		Ams[i] = 31;
		PmdPmsmul[i] = 0;

		PmValue[i] = 0;
		AmValue[i] = 0;
	}
	Pmd = 0;
	Amd = 0;

	LfoStartingFlag = 0;
	LfoOverFlow = 0;
	LfoTime = 0;
	LfoTimeAdd = 0;
	LfoIdx = 0;
	LfoSmallCounter = 0;
	LfoSmallCounterStep = 0;
	Lfrq = 0;
	LfoWaveForm = 0;

	PmTblValue = 0;
	AmTblValue = 255;

	// PM Wave Form 0,3
	for (i=0; i<=127; ++i) {
		PmTbl0[i] = i;
		PmTbl0[i+128] = i-127;
		PmTbl0[i+256] = i;
		PmTbl0[i+384] = i-127;
	}
	// AM Wave Form 0,3
	for (i=0; i<=255; ++i) {
		AmTbl0[i] = 255-i;
		AmTbl0[i+256] = 255-i;
	}

	// PM Wave Form 2
	for (i=0; i<=127; ++i) {
		PmTbl2[i] = i;
		PmTbl2[i+128] = 127-i;
		PmTbl2[i+256] = -i;
		PmTbl2[i+384] = i-127;
	}
	// AM Wave Form 2
	for (i=0; i<=255; ++i) {
		AmTbl2[i] = 255-i;
		AmTbl2[i+256] = i;
	}

};

inline void Lfo::Init() {
	LfoTimeAdd = LFOPRECISION*62500/Samprate;

	LfoSmallCounter = 0;

	SetLFRQ(0);
	SetPMDAMD(0);
	SetPMDAMD(128+0);
	SetWaveForm(0);
	{
		int ch;
		for (ch=0; ch<N_CH; ++ch) {
			SetPMSAMS(ch, 0);
		}
	}
	LfoReset();
	LfoStart();
}
inline void Lfo::InitSamprate() {
	LfoTimeAdd = LFOPRECISION*62500/Samprate;
}

inline void Lfo::LfoReset() {
	LfoStartingFlag = 0;

//	LfoTime �̓��Z�b�g����Ȃ��I�I
	LfoIdx = 0;

	CulcTblValue();
	CulcAllPmValue();
	CulcAllAmValue();
}
inline void Lfo::LfoStart() {
	LfoStartingFlag = 1;
}
inline void Lfo::SetLFRQ(int n) {
	Lfrq = n & 255;

	LfoSmallCounterStep = 16+(Lfrq&15);
	int	shift = 15-(Lfrq>>4);
	if (shift == 0) {
		shift = 1;
		LfoSmallCounterStep <<= 1;
	}
	LfoOverFlow = (8<<shift) * LFOPRECISION;

//	LfoTime �̓��Z�b�g�����
	LfoTime = 0;
}
inline void Lfo::SetPMDAMD(int n) {
	if (n & 0x80) {
		Pmd = n&0x7F;
		int ch;
		for (ch=0; ch<N_CH; ++ch) {
			PmdPmsmul[ch] = Pmd * Pmsmul[ch];
		}
		CulcAllPmValue();
	} else {
		Amd = n&0x7F;
		CulcAllAmValue();
	}
}
inline void	Lfo::SetWaveForm(int n) {
	LfoWaveForm = n&3;

	CulcTblValue();
	CulcAllPmValue();
	CulcAllAmValue();
}
inline void	Lfo::SetPMSAMS(int ch, int n) {
	int pms = (n>>4)&7;
	Pmsmul[ch] = PMSMUL[pms];
	Pmsshl[ch] = PMSSHL[pms];
	PmdPmsmul[ch] = Pmd*Pmsmul[ch];
	CulcPmValue(ch);

	Ams[ch] = ((n&3)-1) & 31;
	CulcAmValue(ch);
}

inline void	Lfo::Update() {
	if (LfoStartingFlag == 0) {
		return;
	}

	LfoTime += LfoTimeAdd;
	if (LfoTime >= LfoOverFlow) {
		LfoTime = 0;
		LfoSmallCounter += LfoSmallCounterStep;
		switch (LfoWaveForm) {
		case 0:
			{
				int idxadd = LfoSmallCounter>>4;
				LfoIdx = (LfoIdx+idxadd) & (SIZELFOTBL-1);
				PmTblValue = PmTbl0[LfoIdx];
				AmTblValue = AmTbl0[LfoIdx];
				break;
			}
		case 1:
			{
				int idxadd = LfoSmallCounter>>4;
				LfoIdx = (LfoIdx+idxadd) & (SIZELFOTBL-1);
				if ((LfoIdx&(SIZELFOTBL/2-1)) < SIZELFOTBL/4) {
					PmTblValue = 128;
					AmTblValue = 256;
				} else {
					PmTblValue = -128;
					AmTblValue = 0;
				}
			}
			break;
		case 2:
			{
				int idxadd = LfoSmallCounter>>4;
				LfoIdx = (LfoIdx+idxadd+idxadd) & (SIZELFOTBL-1);
				PmTblValue = PmTbl2[LfoIdx];
				AmTblValue = AmTbl2[LfoIdx];
				break;
			}
		case 3:
			{
				LfoIdx = irnd() >> (32-SIZELFOTBL_BITS);
				PmTblValue = PmTbl0[LfoIdx];
				AmTblValue = AmTbl0[LfoIdx];
				break;
			}
		}
		LfoSmallCounter &= 15;

		CulcAllPmValue();
		CulcAllAmValue();
	}
}


inline int Lfo::GetPmValue(int ch) {
	return PmValue[ch];
}
inline int Lfo::GetAmValue(int ch) {
	return AmValue[ch];
}

inline void Lfo::CulcTblValue() {
	switch (LfoWaveForm) {
	case 0:
		PmTblValue = PmTbl0[LfoIdx];
		AmTblValue = AmTbl0[LfoIdx];
		break;
	case 1:
		if ((LfoIdx&(SIZELFOTBL/2-1)) < SIZELFOTBL/4) {
			PmTblValue = 128;
			AmTblValue = 256;
		} else {
			PmTblValue = -128;
			AmTblValue = 0;
		}
		break;
	case 2:
		PmTblValue = PmTbl2[LfoIdx];
		AmTblValue = AmTbl2[LfoIdx];
		break;
	case 3:
		PmTblValue = PmTbl0[LfoIdx];
		AmTblValue = AmTbl0[LfoIdx];
		break;
	}
}
inline void	Lfo::CulcPmValue(int ch) {
	if (PmTblValue >= 0) {
		PmValue[ch] = ((PmTblValue*PmdPmsmul[ch])>>(7+5))<<Pmsshl[ch];
	} else {
		PmValue[ch] = -( (((-PmTblValue)*PmdPmsmul[ch])>>(7+5))<<Pmsshl[ch] );
	}
}
inline void	Lfo::CulcAmValue(int ch) {
	AmValue[ch] = (((AmTblValue*Amd)>>7)<<Ams[ch])&(int)0x7FFFFFFF;
}

inline void Lfo::CulcAllPmValue() {
	int ch;
	for (ch=0; ch<N_CH; ++ch) {
		CulcPmValue(ch);
	}
}
inline void Lfo::CulcAllAmValue() {
	int ch;
	for (ch=0; ch<N_CH; ++ch) {
		CulcAmValue(ch);
	}
}
