#ifdef OSDEBUG
#define ASSERT(EX) if (EX) ; else assfail("EX", __FILE__, __LINE__)
#else
#define ASSERT(EX)
#endif

#define PFLOGON 1
#define PHASHLOGON 0
#define SWLOGON 0
#define LOCKLOGON 0
#define SPTLOGON 1
