#ifdef OPT_SS
    #ifdef COEF_SEGMENT_WIDTH
        #define SEGMENT_WIDTH ALIGNMENT/sizeof(double)*COEF_SEGMENT_WIDTH
    #endif 
    #ifdef PADDING 
        #ifdef COEF_PADDING_SIZE
            #define PADDING_SIZE ALIGNMENT/sizeof(double)*COEF_PADDING_SIZE
        #endif
    #endif
#endif

#ifdef OPT_CSS
    #ifdef COEF_SEGMENT_WIDTH
        #define SEGMENT_WIDTH ALIGNMENT/sizeof(double)*COEF_SEGMENT_WIDTH
    #endif
    #ifdef PADDING 
        #ifdef COEF_PADDING_SIZE
            #define PADDING_SIZE ALIGNMENT/sizeof(double)*COEF_PADDING_SIZE
        #endif
    #endif
#endif