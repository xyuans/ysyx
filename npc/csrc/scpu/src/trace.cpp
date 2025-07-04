  int cur = (iringbuf.cur+1) % 16;
  iringbuf.pc_buf[cur] = top->pc;
  iringbuf.inst_buf[cur] = top->inst;
  iringbuf.cur = cur;

  
        int cur = (iringbuf.cur + 1) % 16;
        for (int i = 0; i < 15; i++) {
          printf("   pc:0x%x    inst:0x%x\n", iringbuf.pc_buf[cur], iringbuf.inst_buf[cur]);
          cur = (cur + 1) % 16;
        }
        printf("-->pc:0x%x    inst:0x%x\n", iringbuf.pc_buf[cur], iringbuf.inst_buf[cur]);

