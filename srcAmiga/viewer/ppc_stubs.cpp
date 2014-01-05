// PPC stubs

extern "ASM" short Load__PcP05IMAGE(void *arg1,void *arg2);
extern "ASM" short __saveds _PPCStub_Load__PcP05IMAGE(long *args)
{
	return Load__PcP05IMAGE(
		(void *) args[0],
		(void *) args[1]
	);
}

extern "ASM" void Cleanup__P05IMAGE(void *arg1);
extern "ASM" void __saveds _PPCStub_Cleanup__P05IMAGE(long *args)
{
	Cleanup__P05IMAGE(
		(void *) args[0]
	);
}

