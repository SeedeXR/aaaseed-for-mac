
#ifdef AAA_UPDATE_H
#error "UPDATE_H included more than once."
#endif
#define AAA_UPDATE_H 1


#define	UPDATE_ALL_MASK		0xffffffff

#define	UPDATE_SET(a)		(update |= (1<<(a)))
#define	UPDATE_CLEAR(a)		(update &= ~(1<<(a)))
#define	UPDATE_ALL_SET()	(update |= UPDATE_ALL_MASK)
#define	UPDATE_ALL_CLEAR()	(update &= ~UPDATE_ALL_MASK)
#define	UPDATE_GET(a)		(update & (1<<(a)))

#endif
