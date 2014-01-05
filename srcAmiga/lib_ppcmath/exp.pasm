/*620 <exp> stwu  r1,-80(r1)
624 <exp+4> stfd   f1,32(r1)
628 <exp+8> mffs   f9
0000062c <exp+c> lis r3,0
		RELOC: 0000062e R_PPC_ADDR16_HA log_table_base
00000630 <exp+10> lwz   r10,32(r1)
00000634 <exp+14> addi  r3,r3,0
		RELOC: 00000636 R_PPC_ADDR16_LO log_table_base
00000638 <exp+18> lfd   f0,-1200(r3)
0000063c <exp+1c> rlwinm   r12,r10,12,21,31
00000640 <exp+20> lfd   f11,-1088(r3)
00000644 <exp+24> addi  r9,r12,-970
00000648 <exp+28> lfd   f6,-1096(r3)
0000064c <exp+2c> mtfsf 255,f0
00000650 <exp+30> cmplwi   cr6,r9,64
00000654 <exp+34> lfd   f8,-1080(r3)
00000658 <exp+38> bge   cr6,00000778 <exp_no_computation>
0000065c <exp+3c> addi  r8,r3,-1064
00000660 <exp+40> lfd   f3,-1072(r3)
00000664 <exp+44> fmadd f10,f1,f11,f6
00000668 <exp+48> lfd   f4,-520(r3)
0000066c <exp+4c> lfd   f7,-528(r3)
00000670 <exp+50> stfd  f10,32(r1)
00000674 <exp+54> fsub  f10,f10,f6
00000678 <exp+58> lwz   r6,36(r1)
0000067c <exp+5c> fnmsub   f12,f10,f8,f1
00000680 <exp+60> rlwinm   r9,r6,4,23,27
00000684 <exp+64> lfd   f5,-536(r3)
00000688 <exp+68> srawi r11,r6,5
0000068c <exp+6c> lfdx  f8,r8,r9
00000690 <exp+70> addi  r8,r8,8
00000694 <exp+74> lfd   f2,-544(r3)
00000698 <exp+78> fmsub f10,f10,f3,f8
0000069c <exp+7c> lfd   f0,-552(r3)
000006a0 <exp+80> fadd  f3,f12,f10
000006a4 <exp+84> lfdx  f11,r8,r9
000006a8 <exp+88> fmul  f6,f3,f3
000006ac <exp+8c> addi  r9,r11,1021
000006b0 <exp+90> fmadd f4,f3,f4,f7
000006b4 <exp+94> cmplwi   cr6,r9,2044
000006b8 <exp+98> fmadd f5,f3,f5,f2
000006bc <exp+9c> fmul  f2,f6,f3
000006c0 <exp+a0> fmadd f0,f6,f0,f10
000006c4 <exp+a4> fmadd f5,f6,f4,f5
000006c8 <exp+a8> fmadd f0,f2,f5,f0
000006cc <exp+ac> fadd  f0,f0,f12
000006d0 <exp+b0> fmadd f0,f0,f11,f11
000006d4 <exp+b4> stfd  f0,32(r1)
000006d8 <exp+b8> lwz   r5,32(r1)
000006dc <exp+bc> bge   cr6,00000700 <exp_possible_over_under>
000006e0 <exp+c0> rlwinm   r6,r5,12,21,31
000006e4 <exp+c4> add   r6,r11,r6
000006e8 <exp_result_ok> rlwimi  r5,r6,20,1,11
000006ec <exp_result_ok+4> stw   r5,32(r1)
000006f0 <exp_result_ok+8> lfd   f1,32(r1)
000006f4 <exp_result_ok+c> mtfsf 255,f9
000006f8 <exp_result_ok+10> addi r1,r1,80
000006fc <exp_result_ok+14> blr
00000700 <exp_possible_over_under> rlwinm r6,r5,12,21,31
00000704 <exp_possible_over_under+4> add. r6,r11,r6
00000708 <exp_possible_over_under+8> addi r7,r6,-1
0000070c <exp_possible_over_under+c> cmplwi  cr7,r7,2046
00000710 <exp_possible_over_under+10> blt-   cr7,000006e8 <exp_result_ok>
00000714 <exp_possible_over_under+14> bgt+   00000758 <exp_aix_overflow>
00000718 <exp_possible_over_under+18> cmpwi  cr6,r6,-52
0000071c <exp_possible_over_under+1c> bgt cr6,0000073c <exp_aix_denorm>
00000720 <exp_possible_over_under+20> mr. r5,r5
00000724 <exp_possible_over_under+24> lfd f1,-1176(r3)
00000728 <exp_possible_over_under+28> bge 00000730 <exp_aix_return_zero>
0000072c <exp_possible_over_under+2c> fneg   f1,f1
00000730 <exp_aix_return_zero> mtfsf   255,f9
00000734 <exp_aix_return_zero+4> addi  r1,r1,80
00000738 <exp_aix_return_zero+8> blr
0000073c <exp_aix_denorm> addi   r6,r6,64
00000740 <exp_aix_denorm+4> lfd  f2,-1112(r3)
00000744 <exp_aix_denorm+8> rlwimi  r5,r6,20,1,11
00000748 <exp_aix_denorm+c> stw  r5,32(r1)
0000074c <exp_aix_denorm+10> lfd f1,32(r1)
00000750 <exp_aix_denorm+14> fmul   f1,f1,f2
00000754 <exp_aix_denorm+18> b   0000075c <exp_aix_range_returns>
00000758 <exp_aix_overflow> lfd  f1,-1184(r3)
0000075c <exp_aix_range_returns> lis   r5,0
		RELOC: 0000075e R_PPC_ADDR16_HA errno
00000760 <exp_aix_range_returns+4> li  r3,34
00000764 <exp_aix_range_returns+8> stw r3,0(r5)
		RELOC: 00000766 R_PPC_ADDR16_LO errno
00000768 <exp_aix_range_returns+c> lfd f2,64(r1)
0000076c <exp_aix_range_returns+10> mtfsf 255,f9
00000770 <exp_aix_range_returns+14> addi  r1,r1,80
00000774 <exp_aix_range_returns+18> blr
00000778 <exp_no_computation> cmpwi cr6,r9,0
0000077c <exp_no_computation+4> stfd   f9,64(r1)
00000780 <exp_no_computation+8> lwz r11,36(r1)
00000784 <exp_no_computation+c> cmpwi  cr7,r12,2047
00000788 <exp_no_computation+10> lwz   r8,68(r1)
0000078c <exp_no_computation+14> cmpwi cr5,r10,0
00000790 <exp_no_computation+18> bge   cr6,000007c0 <exp_inf_large_nan>
00000794 <exp_zero_or_small> clrlwi r9,r10,1
00000798 <exp_zero_or_small+4> or.  r9,r9,r11
0000079c <exp_zero_or_small+8> blt+ 000007b0 <exp_zero>
000007a0 <exp_small> fmr   f1,f0
000007a4 <exp_small+4> mtfsf  255,f9
000007a8 <exp_small+8> addi   r1,r1,80
000007ac <exp_small+c> blr
000007b0 <exp_zero> fmr f1,f0
000007b4 <exp_exact> mtfsf 255,f9
000007b8 <exp_exact+4> addi   r1,r1,80
000007bc <exp_exact+8> blr
000007c0 <exp_inf_large_nan> beq cr7,000007d8 <exp_inf_nan>
000007c4 <exp_inf_large_nan+4> blt  cr5,000007d0 <exp_unf>
000007c8 <exp_ovf> lfd  f1,-1184(r3)
000007cc <exp_ovf+4> b  0000075c <exp_aix_range_returns>
000007d0 <exp_unf> fsub f1,f0,f0
000007d4 <exp_unf+4> b  0000075c <exp_aix_range_returns>
000007d8 <exp_inf_nan> clrlwi r9,r10,12
000007dc <exp_inf_nan+4> or.  r9,r9,r11
000007e0 <exp_inf_nan+8> bne  000007f0 <exp_nan>
000007e4 <exp_inf> bge  cr5,000007b4 <exp_exact>
000007e8 <exp_inf+4> fsub  f1,f0,f0
000007ec <exp_inf+8> b  000007b4 <exp_exact>
000007f0 <exp_nan> rlwinm. r9,r10,0,12,12
000007f4 <exp_nan+4> beq   000007fc <exp_snan>
000007f8 <exp_qnan> b   00000810 <exp_aix_domain_returns>
000007fc <exp_snan> stw r11,36(r1)
00000800 <exp_snan+4> oris r9,r10,8
00000804 <exp_snan+8> stw  r9,32(r1)
00000808 <exp_snan+c> lfd  f1,32(r1)
0000080c <exp_snan+10> b   00000810 <exp_aix_domain_returns>
00000810 <exp_aix_domain_returns> lis  r5,0
		RELOC: 00000812 R_PPC_ADDR16_HA errno
00000814 <exp_aix_domain_returns+4> li r3,33
00000818 <exp_aix_domain_returns+8> stw   r3,0(r5)
		RELOC: 0000081a R_PPC_ADDR16_LO errno
0000081c <exp_aix_domain_returns+c> lfd   f2,64(r1)
00000820 <exp_aix_domain_returns+10> mtfsf   255,f2
00000824 <exp_aix_domain_returns+14> addi r1,r1,80
00000828 <exp_aix_domain_returns+18> blr
0000082c <exp_aix_domain_returns+1c> stwu r1,-80(r1)
00000830 <exp_aix_domain_returns+20> stfd f1,32(r1)
00000834 <exp_aix_domain_returns+24> mffs f9
00000838 <exp_aix_domain_returns+28> lis  r3,0
		RELOC: 0000083a R_PPC_ADDR16_HA log_table_base
0000083c <exp_aix_domain_returns+2c> lwz  r10,32(r1)
00000840 <exp_aix_domain_returns+30> addi r3,r3,0
		RELOC: 00000842 R_PPC_ADDR16_LO log_table_base
00000844 <exp_aix_domain_returns+34> lfd  f0,-1200(r3)
00000848 <exp_aix_domain_returns+38> rlwinm  r12,r10,12,21,31
0000084c <exp_aix_domain_returns+3c> lfd  f11,-1088(r3)
00000850 <exp_aix_domain_returns+40> addi r9,r12,-999
00000854 <exp_aix_domain_returns+44> lfd  f6,-1096(r3)
00000858 <exp_aix_domain_returns+48> mtfsf   255,f0
0000085c <exp_aix_domain_returns+4c> cmplwi  cr6,r9,32
00000860 <exp_aix_domain_returns+50> lfd  f8,-1080(r3)
00000864 <exp_aix_domain_returns+54> bge  cr6,0000093c <sexp_no_computation>
00000868 <exp_aix_domain_returns+58> addi r8,r3,-1064
0000086c <exp_aix_domain_returns+5c> lfd  f3,-1072(r3)
00000870 <exp_aix_domain_returns+60> fmadd   f10,f1,f11,f6
00000874 <exp_aix_domain_returns+64> lfd  f4,-520(r3)
00000878 <exp_aix_domain_returns+68> lfd  f7,-528(r3)
0000087c <exp_aix_domain_returns+6c> stfd f10,32(r1)
00000880 <exp_aix_domain_returns+70> fsub f10,f10,f6
00000884 <exp_aix_domain_returns+74> lwz  r6,36(r1)
00000888 <exp_aix_domain_returns+78> fnmsub  f12,f10,f8,f1
0000088c <exp_aix_domain_returns+7c> rlwinm  r9,r6,4,23,27
00000890 <exp_aix_domain_returns+80> lfd  f5,-536(r3)
00000894 <exp_aix_domain_returns+84> srawi   r11,r6,5
00000898 <exp_aix_domain_returns+88> lfdx f8,r8,r9
0000089c <exp_aix_domain_returns+8c> addi r8,r8,8
000008a0 <exp_aix_domain_returns+90> lfd  f2,-544(r3)
000008a4 <exp_aix_domain_returns+94> fmsub   f10,f10,f3,f8
000008a8 <exp_aix_domain_returns+98> lfd  f0,-552(r3)
000008ac <exp_aix_domain_returns+9c> fadd f3,f12,f10
000008b0 <exp_aix_domain_returns+a0> lfdx f11,r8,r9
000008b4 <exp_aix_domain_returns+a4> fmul f6,f3,f3
000008b8 <exp_aix_domain_returns+a8> fmadd   f4,f3,f4,f7
000008bc <exp_aix_domain_returns+ac> fmadd   f5,f3,f5,f2
000008c0 <exp_aix_domain_returns+b0> fmul f2,f6,f3
000008c4 <exp_aix_domain_returns+b4> fmadd   f0,f6,f0,f10
000008c8 <exp_aix_domain_returns+b8> fmadd   f5,f6,f4,f5
000008cc <exp_aix_domain_returns+bc> fmadd   f0,f2,f5,f0
000008d0 <exp_aix_domain_returns+c0> fadd f0,f0,f12
000008d4 <exp_aix_domain_returns+c4> fmadd   f0,f0,f11,f11
000008d8 <exp_aix_domain_returns+c8> stfd f0,32(r1)
000008dc <exp_aix_domain_returns+cc> lwz  r5,32(r1)
000008e0 <exp_aix_domain_returns+d0> rlwinm  r6,r5,12,21,31
000008e4 <exp_aix_domain_returns+d4> add  r6,r11,r6
000008e8 <exp_aix_domain_returns+d8> rlwimi  r5,r6,20,1,11
000008ec <exp_aix_domain_returns+dc> stw  r5,32(r1)
000008f0 <exp_aix_domain_returns+e0> lfd  f1,32(r1)
000008f4 <exp_aix_domain_returns+e4> frsp f1,f1
000008f8 <exp_aix_domain_returns+e8> mtfsf   255,f9
000008fc <exp_aix_domain_returns+ec> addi r1,r1,80
00000900 <exp_aix_domain_returns+f0> blr
00000904 <sexp_aix_domain_returns> lis r5,0
		RELOC: 00000906 R_PPC_ADDR16_HA errno
00000908 <sexp_aix_domain_returns+4> li   r3,33
0000090c <sexp_aix_domain_returns+8> stw  r3,0(r5)
		RELOC: 0000090e R_PPC_ADDR16_LO errno
00000910 <sexp_aix_domain_returns+c> lfd  f2,64(r1)
00000914 <sexp_aix_domain_returns+10> mtfsf  255,f2
00000918 <sexp_aix_domain_returns+14> addi   r1,r1,80
0000091c <sexp_aix_domain_returns+18> blr
00000920 <sexp_aix_range_returns> lis  r5,0
		RELOC: 00000922 R_PPC_ADDR16_HA errno
00000924 <sexp_aix_range_returns+4> li r3,34
00000928 <sexp_aix_range_returns+8> stw   r3,0(r5)
		RELOC: 0000092a R_PPC_ADDR16_LO errno
0000092c <sexp_aix_range_returns+c> lfd   f2,64(r1)
00000930 <sexp_aix_range_returns+10> mtfsf   255,f9
00000934 <sexp_aix_range_returns+14> addi r1,r1,80
00000938 <sexp_aix_range_returns+18> blr
0000093c <sexp_no_computation> cmpwi   cr6,r9,0
00000940 <sexp_no_computation+4> stfd  f9,64(r1)
00000944 <sexp_no_computation+8> lwz   r11,36(r1)
00000948 <sexp_no_computation+c> cmpwi cr7,r12,2047
0000094c <sexp_no_computation+10> lwz  r8,68(r1)
00000950 <sexp_no_computation+14> cmpwi   cr5,r10,0
00000954 <sexp_no_computation+18> bge  cr6,00000984 <sexp_inf_large_nan>
00000958 <sexp_zero_or_small> clrlwi   r9,r10,1
0000095c <sexp_zero_or_small+4> or. r9,r9,r11
00000960 <sexp_zero_or_small+8> blt+   00000974 <sexp_zero>
00000964 <sexp_small> fmr  f1,f0
00000968 <sexp_small+4> mtfsf 255,f9
0000096c <sexp_small+8> addi  r1,r1,80
00000970 <sexp_small+c> blr
00000974 <sexp_zero> fmr   f1,f0
00000978 <sexp_exact> mtfsf   255,f9
0000097c <sexp_exact+4> addi  r1,r1,80
00000980 <sexp_exact+8> blr
00000984 <sexp_inf_large_nan> beq   cr7,0000099c <sexp_inf_nan>
00000988 <sexp_inf_large_nan+4> blt cr5,00000994 <sexp_unf>
0000098c <sexp_ovf> lfd f1,-1184(r3)
00000990 <sexp_ovf+4> b 00000920 <sexp_aix_range_returns>
00000994 <sexp_unf> fsub   f1,f0,f0
00000998 <sexp_unf+4> b 00000920 <sexp_aix_range_returns>
0000099c <sexp_inf_nan> clrlwi   r9,r10,12
000009a0 <sexp_inf_nan+4> or. r9,r9,r11
000009a4 <sexp_inf_nan+8> bne 000009b4 <sexp_nan>
000009a8 <sexp_inf> bge cr5,00000978 <sexp_exact>
000009ac <sexp_inf+4> fsub f1,f0,f0
000009b0 <sexp_inf+8> b 00000978 <sexp_exact>
000009b4 <sexp_nan> rlwinm.   r9,r10,0,12,12
000009b8 <sexp_nan+4> beq  000009c0 <sexp_snan>
000009bc <sexp_qnan> b  00000904 <sexp_aix_domain_returns>
000009c0 <sexp_snan> stw   r11,36(r1)
000009c4 <sexp_snan+4> oris   r9,r10,8
000009c8 <sexp_snan+8> stw r9,32(r1)
000009cc <sexp_snan+c> lfd f1,32(r1)
000009d0 <sexp_snan+10> b  00000904 <sexp_aix_domain_returns>
000009d4 <log> stwu  r1,-80(r1)
000009d8 <log+4> stfd   f1,48(r1)
000009dc <log+8> mffs   f4
000009e0 <log+c> lis r11,19264
000009e4 <log_restart> stfd   f1,32(r1)
000009e8 <log_restart+4> lis  r3,0
		RELOC: 000009ea R_PPC_ADDR16_HA log_table_base
000009ec <log_restart+8> lwz  r4,32(r1)
000009f0 <log_restart+c> addi r3,r3,0
		RELOC: 000009f2 R_PPC_ADDR16_LO log_table_base
000009f4 <log_restart+10> rlwinm r5,r4,20,24,31
000009f8 <log_restart+14> lfd f0,-1200(r3)
000009fc <log_restart+18> clrlwi r7,r4,12
00000a00 <log_restart+1c> oris   r7,r7,16368
00000a04 <log_restart+20> stw r7,32(r1)
00000a08 <log_restart+24> mtfsf  255,f0
00000a0c <log_restart+28> addi   r6,r5,1
00000a10 <log_restart+2c> lfd f9,32(r1)
00000a14 <log_restart+30> rlwinm r6,r6,3,20,27
00000a18 <log_restart+34> lfdx   f8,r3,r6
00000a1c <log_restart+38> addi   r7,r5,-107
00000a20 <log_restart+3c> lfd f7,-1208(r3)
00000a24 <log_restart+40> rlwinm r7,r7,1,31,31
00000a28 <log_restart+44> lfd f6,-1216(r3)
00000a2c <log_restart+48> fmsub  f13,f8,f9,f0
00000a30 <log_restart+4c> rlwinm.   r8,r4,12,21,31
00000a34 <log_restart+50> stfd   f4,64(r1)
00000a38 <log_restart+54> lfd f5,-1224(r3)
00000a3c <log_restart+58> cmpwi  cr6,r4,0
00000a40 <log_restart+5c> fmul   f12,f8,f9
00000a44 <log_restart+60> subfc  r9,r7,r8
00000a48 <log_restart+64> lfd f3,-1248(r3)
00000a4c <log_restart+68> add r5,r9,r11
00000a50 <log_restart+6c> lfd f4,-1232(r3)
00000a54 <log_restart+70> fmadd  f0,f13,f7,f6
00000a58 <log_restart+74> beq 00000af0 <log_x_denorm_zero>
00000a5c <log_restart+78> addi   r7,r6,8
00000a60 <log_restart+7c> stw r5,40(r1)
00000a64 <log_restart+80> frsp   f12,f12
00000a68 <log_restart+84> cmpwi  r8,2046
00000a6c <log_restart+88> lfs f11,-1128(r3)
00000a70 <log_restart+8c> fmul   f7,f13,f13
00000a74 <log_restart+90> blt cr6,00000b34 <log_x_negative>
00000a78 <log_restart+94> rlwinm r6,r6,0,21,27
00000a7c <log_restart+98> lfs f10,40(r1)
00000a80 <log_restart+9c> fmadd  f0,f13,f0,f5
00000a84 <log_restart+a0> addi   r9,r9,-1022
00000a88 <log_restart+a4> lfdx   f6,r3,r7
00000a8c <log_restart+a8> fmsub  f5,f8,f9,f12
00000a90 <log_restart+ac> bgt 00000b3c <log_x_inf_nan>
00000a94 <log_restart+b0> lfd f8,-1240(r3)
00000a98 <log_restart+b4> fmul   f9,f13,f3
00000a9c <log_restart+b8> or. r6,r6,r9
00000aa0 <log_restart+bc> lfd f3,-1152(r3)
00000aa4 <log_restart+c0> fmadd  f0,f13,f0,f4
00000aa8 <log_restart+c4> lfd f4,-1160(r3)
00000aac <log_restart+c8> fsubs  f10,f10,f11
00000ab0 <log_restart+cc> fadd   f12,f12,f6
00000ab4 <log_restart+d0> lfd f6,64(r1)
00000ab8 <log_restart+d4> fmadd  f0,f13,f0,f8
00000abc <log_restart+d8> fmadd  f5,f10,f3,f5
00000ac0 <log_restart+dc> fmadd  f12,f10,f4,f12
00000ac4 <log_restart+e0> fmadd  f0,f7,f0,f9
00000ac8 <log_restart+e4> beq 00000ae0 <log_special>
00000acc <log_normal> fmadd   f5,f13,f0,f5
00000ad0 <log_normal+4> fadd  f1,f12,f5
00000ad4 <log_normal+8> mtfsf 255,f6
00000ad8 <log_normal+c> addi  r1,r1,80
00000adc <log_normal+10> blr
00000ae0 <log_special> fmadd  f1,f13,f0,f13
00000ae4 <log_special+4> mtfsf   255,f6
00000ae8 <log_special+8> addi r1,r1,80
00000aec <log_special+c> blr
00000af0 <log_x_denorm_zero> clrlwi r9,r4,12
00000af4 <log_x_denorm_zero+4> lwz  r5,36(r1)
00000af8 <log_x_denorm_zero+8> or.  r5,r5,r9
00000afc <log_x_denorm_zero+c> lfd  f4,-1120(r3)
00000b00 <log_x_denorm_zero+10> addi   r11,r11,-52
00000b04 <log_x_denorm_zero+14> fmul   f1,f1,f4
00000b08 <log_x_denorm_zero+18> lfd f4,64(r1)
00000b0c <log_x_denorm_zero+1c> mtfsf  255,f4
00000b10 <log_x_denorm_zero+20> bne 000009e4 <log_restart>
00000b14 <log_x_zero> lfd  f1,-1192(r3)
00000b18 <log_aix_domain_returns> lis  r5,0
		RELOC: 00000b1a R_PPC_ADDR16_HA errno
00000b1c <log_aix_domain_returns+4> li r3,33
00000b20 <log_aix_domain_returns+8> stw   r3,0(r5)
		RELOC: 00000b22 R_PPC_ADDR16_LO errno
00000b24 <log_aix_domain_returns+c> lfd   f2,64(r1)
00000b28 <log_aix_domain_returns+10> mtfsf   255,f2
00000b2c <log_aix_domain_returns+14> addi r1,r1,80
00000b30 <log_aix_domain_returns+18> blr
00000b34 <log_x_negative> lfd f1,-1168(r3)
00000b38 <log_x_negative+4> b 00000b18 <log_aix_domain_returns>
00000b3c <log_x_inf_nan> lwz  r6,36(r1)
00000b40 <log_x_inf_nan+4> clrlwi   r9,r4,12
00000b44 <log_x_inf_nan+8> or.   r9,r9,r6
00000b48 <log_x_inf_nan+c> beq+  00000b68 <log_x_infinity>
00000b4c <log_x_inf_nan+10> rlwinm. r9,r4,13,31,31
00000b50 <log_x_inf_nan+14> bne  00000b64 <log_x_is_qnan>
00000b54 <log_x_inf_nan+18> oris r9,r4,8
00000b58 <log_x_inf_nan+1c> stw  r9,32(r1)
00000b5c <log_x_inf_nan+20> lfd  f1,32(r1)
00000b60 <log_x_inf_nan+24> b 00000b18 <log_aix_domain_returns>
00000b64 <log_x_is_qnan> b 00000b18 <log_aix_domain_returns>
00000b68 <log_x_infinity> mr. r4,r4
00000b6c <log_x_infinity+4> blt- 00000b34 <log_x_negative>
00000b70 <log_x_infinity+8> lfd  f2,64(r1)
00000b74 <log_x_infinity+c> mtfsf   255,f2
00000b78 <log_x_infinity+10> addi   r1,r1,80
00000b7c <log_x_infinity+14> blr
00000b80 <log10> stwu   r1,-80(r1)
00000b84 <log10+4> stfd f1,48(r1)
00000b88 <log10+8> mffs f4
00000b8c <log10+c> lis  r11,19264
00000b90 <log10_restart> stfd f1,32(r1)
00000b94 <log10_restart+4> lis   r3,0
		RELOC: 00000b96 R_PPC_ADDR16_HA log_table_base
00000b98 <log10_restart+8> lwz   r4,32(r1)
00000b9c <log10_restart+c> addi  r3,r3,0
		RELOC: 00000b9e R_PPC_ADDR16_LO log_table_base
00000ba0 <log10_restart+10> rlwinm  r5,r4,20,24,31
00000ba4 <log10_restart+14> lfd  f0,-1200(r3)
00000ba8 <log10_restart+18> clrlwi  r7,r4,12
00000bac <log10_restart+1c> oris r7,r7,16368
00000bb0 <log10_restart+20> stw  r7,32(r1)
00000bb4 <log10_restart+24> mtfsf   255,f0
00000bb8 <log10_restart+28> addi r6,r5,1
00000bbc <log10_restart+2c> lfd  f9,32(r1)
00000bc0 <log10_restart+30> rlwinm  r6,r6,3,20,27
00000bc4 <log10_restart+34> lfdx f8,r3,r6
00000bc8 <log10_restart+38> addi r7,r5,-107
00000bcc <log10_restart+3c> lfd  f7,-1208(r3)
00000bd0 <log10_restart+40> rlwinm  r7,r7,1,31,31
00000bd4 <log10_restart+44> lfd  f6,-1216(r3)
00000bd8 <log10_restart+48> fmsub   f13,f8,f9,f0
00000bdc <log10_restart+4c> rlwinm. r8,r4,12,21,31
00000be0 <log10_restart+50> stfd f4,64(r1)
00000be4 <log10_restart+54> lfd  f5,-1224(r3)
00000be8 <log10_restart+58> cmpwi   cr6,r4,0
00000bec <log10_restart+5c> fmul f12,f8,f9
00000bf0 <log10_restart+60> subfc   r9,r7,r8
00000bf4 <log10_restart+64> lfd  f3,-1248(r3)
00000bf8 <log10_restart+68> add  r5,r9,r11
00000bfc <log10_restart+6c> lfd  f4,-1232(r3)
00000c00 <log10_restart+70> fmadd   f0,f13,f7,f6
00000c04 <log10_restart+74> beq  00000cb4 <log10_x_denorm_zero>
00000c08 <log10_restart+78> addi r7,r6,8
00000c0c <log10_restart+7c> stw  r5,40(r1)
00000c10 <log10_restart+80> frsp f12,f12
00000c14 <log10_restart+84> cmpwi   r8,2046
00000c18 <log10_restart+88> lfs  f11,-1128(r3)
00000c1c <log10_restart+8c> fmul f7,f13,f13
00000c20 <log10_restart+90> blt  cr6,00000cf8 <log10_x_negative>
00000c24 <log10_restart+94> rlwinm  r6,r6,0,21,27
00000c28 <log10_restart+98> lfs  f10,40(r1)
00000c2c <log10_restart+9c> fmadd   f0,f13,f0,f5
00000c30 <log10_restart+a0> addi r9,r9,-1022
00000c34 <log10_restart+a4> lfdx f6,r3,r7
00000c38 <log10_restart+a8> fmsub   f5,f8,f9,f12
00000c3c <log10_restart+ac> bgt  00000d00 <log10_x_inf_nan>
00000c40 <log10_restart+b0> lfd  f8,-1240(r3)
00000c44 <log10_restart+b4> fmul f9,f13,f3
00000c48 <log10_restart+b8> or.  r6,r6,r9
00000c4c <log10_restart+bc> lfd  f3,-1152(r3)
00000c50 <log10_restart+c0> fmadd   f0,f13,f0,f4
00000c54 <log10_restart+c4> lfd  f4,-1160(r3)
00000c58 <log10_restart+c8> fsubs   f10,f10,f11
00000c5c <log10_restart+cc> fadd f12,f12,f6
00000c60 <log10_restart+d0> lfd  f6,64(r1)
00000c64 <log10_restart+d4> fmadd   f0,f13,f0,f8
00000c68 <log10_restart+d8> lfd  f2,-1136(r3)
00000c6c <log10_restart+dc> fmadd   f5,f10,f3,f5
00000c70 <log10_restart+e0> fmadd   f12,f10,f4,f12
00000c74 <log10_restart+e4> lfd  f11,-1144(r3)
00000c78 <log10_restart+e8> fmadd   f0,f7,f0,f9
00000c7c <log10_restart+ec> beq  00000c9c <log10_special>
00000c80 <log10_normal> fmadd f5,f13,f0,f5
00000c84 <log10_normal+4> fmul   f7,f2,f12
00000c88 <log10_normal+8> fmadd  f7,f11,f5,f7
00000c8c <log10_normal+c> fmadd  f1,f11,f12,f7
00000c90 <log10_normal+10> mtfsf 255,f6
00000c94 <log10_normal+14> addi  r1,r1,80
00000c98 <log10_normal+18> blr
00000c9c <log10_special> fmadd   f8,f11,f0,f2
00000ca0 <log10_special+4> fmul  f8,f13,f8
00000ca4 <log10_special+8> fmadd f1,f13,f11,f8
00000ca8 <log10_special+c> mtfsf 255,f6
00000cac <log10_special+10> addi r1,r1,80
00000cb0 <log10_special+14> blr
00000cb4 <log10_x_denorm_zero> clrlwi  r9,r4,12
00000cb8 <log10_x_denorm_zero+4> lwz   r5,36(r1)
00000cbc <log10_x_denorm_zero+8> or.   r5,r5,r9
00000cc0 <log10_x_denorm_zero+c> lfd   f4,-1120(r3)
00000cc4 <log10_x_denorm_zero+10> addi r11,r11,-52
00000cc8 <log10_x_denorm_zero+14> fmul f1,f1,f4
00000ccc <log10_x_denorm_zero+18> lfd  f4,64(r1)
00000cd0 <log10_x_denorm_zero+1c> mtfsf   255,f4
00000cd4 <log10_x_denorm_zero+20> bne  00000b90 <log10_restart>
00000cd8 <log10_x_zero> lfd   f1,-1192(r3)
00000cdc <log10_aix_domain_returns> lis   r5,0
		RELOC: 00000cde R_PPC_ADDR16_HA errno
00000ce0 <log10_aix_domain_returns+4> li  r3,33
00000ce4 <log10_aix_domain_returns+8> stw r3,0(r5)
		RELOC: 00000ce6 R_PPC_ADDR16_LO errno
00000ce8 <log10_aix_domain_returns+c> lfd f2,64(r1)
00000cec <log10_aix_domain_returns+10> mtfsf 255,f2
00000cf0 <log10_aix_domain_returns+14> addi  r1,r1,80
00000cf4 <log10_aix_domain_returns+18> blr
00000cf8 <log10_x_negative> lfd  f1,-1168(r3)
00000cfc <log10_x_negative+4> b  00000cdc <log10_aix_domain_returns>
00000d00 <log10_x_inf_nan> lwz   r6,36(r1)
00000d04 <log10_x_inf_nan+4> clrlwi r9,r4,12
00000d08 <log10_x_inf_nan+8> or. r9,r9,r6
00000d0c <log10_x_inf_nan+c> beq+   00000d2c <log10_x_infinity>
00000d10 <log10_x_inf_nan+10> rlwinm.  r9,r4,13,31,31
00000d14 <log10_x_inf_nan+14> bne   00000d28 <log10_x_is_qnan>
00000d18 <log10_x_inf_nan+18> oris  r9,r4,8
00000d1c <log10_x_inf_nan+1c> stw   r9,32(r1)
00000d20 <log10_x_inf_nan+20> lfd   f1,32(r1)
00000d24 <log10_x_inf_nan+24> b  00000cdc <log10_aix_domain_returns>
00000d28 <log10_x_is_qnan> b  00000cdc <log10_aix_domain_returns>
00000d2c <log10_x_infinity> mr.  r4,r4
00000d30 <log10_x_infinity+4> blt-  00000cf8 <log10_x_negative>
00000d34 <log10_x_infinity+8> lfd   f2,64(r1)
00000d38 <log10_x_infinity+c> mtfsf 255,f2
00000d3c <log10_x_infinity+10> addi r1,r1,80
00000d40 <log10_x_infinity+14> blr*/

