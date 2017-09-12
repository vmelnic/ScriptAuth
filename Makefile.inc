all: $(BUILDDIR)scriptauth$(DLSUFFICS)

scriptauth$(OBJSUFFICS): scriptauth.c
	$(CC) $(DCFLAGS) $(CFLAGS) scriptauth.c

$(BUILDDIR)scriptauth$(DLSUFFICS): scriptauth$(OBJSUFFICS)
	$(LN) $(LNOUT)../../$(BUILDDIR)scriptauth$(DLSUFFICS) $(LDFLAGS) $(DLFLAGS) $(LIBSPREFIX)pam$(LIBSSUFFIX) scriptauth$(OBJSUFFICS)
